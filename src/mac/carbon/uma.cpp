/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/uma.cpp
// Purpose:     UMA support
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     The wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/mac/uma.h"

#if wxUSE_GUI

#include "wx/toplevel.h"
#include "wx/dc.h"

#include "wx/mac/uma.h"

static SInt32 sUMASystemVersion = 0 ;

long UMAGetSystemVersion() 
{ 
    if ( sUMASystemVersion == 0 )
    {
        verify_noerr(Gestalt(gestaltSystemVersion, &sUMASystemVersion));
    }
    return sUMASystemVersion ; 
}

void UMAInitToolbox( UInt16 WXUNUSED(inMoreMastersCalls),
                     bool WXUNUSED(isEmbedded) )
{
#if 0 // ndef __LP64__
    {
        FontFamilyID fontId ;
        Str255 fontName ;
        SInt16 fontSize ;
        Style fontStyle ;

        GetThemeFont(kThemeSmallSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;
        GetFNum( fontName, &fontId );

        TXNMacOSPreferredFontDescription fontDescriptions[] =
        {
            { fontId , (fontSize << 16) , kTXNDefaultFontStyle, kTXNSystemDefaultEncoding }
        } ;
        int noOfFontDescriptions = sizeof( fontDescriptions ) / sizeof(TXNMacOSPreferredFontDescription) ;

        OptionBits options = 0 ;

        TXNInitTextension( fontDescriptions,  noOfFontDescriptions, options );
    }
#endif
}

// process manager
long UMAGetProcessMode()
{
    OSErr err ;
    ProcessInfoRec processinfo;
    ProcessSerialNumber procno ;

    procno.highLongOfPSN = 0 ;
    procno.lowLongOfPSN = kCurrentProcess ;
    processinfo.processInfoLength = sizeof(ProcessInfoRec);
    processinfo.processName = NULL;
#ifndef __LP64__
    processinfo.processAppSpec = NULL;
#endif

    err = ::GetProcessInformation( &procno , &processinfo ) ;
    wxASSERT( err == noErr ) ;

    return processinfo.processMode ;
}

bool UMAGetProcessModeDoesActivateOnFGSwitch()
{
    return UMAGetProcessMode() & modeDoesActivateOnFGSwitch ;
}

// menu manager

#if wxMAC_USE_COCOA == 0

MenuRef UMANewMenu( SInt16 id , const wxString& title , wxFontEncoding encoding )
{
    wxString str = wxStripMenuCodes( title ) ;
    MenuRef menu ;

    CreateNewMenu( id , 0 , &menu ) ;
    SetMenuTitleWithCFString( menu , wxMacCFStringHolder(str , encoding ) ) ;

    return menu ;
}

void UMASetMenuTitle( MenuRef menu , const wxString& title , wxFontEncoding encoding )
{
    wxString str = wxStripMenuCodes( title ) ;

    SetMenuTitleWithCFString( menu , wxMacCFStringHolder(str , encoding) ) ;
}

void UMASetMenuItemText( MenuRef menu,  MenuItemIndex item, const wxString& title, wxFontEncoding encoding )
{
    // we don't strip the accels here anymore, must be done before
    wxString str = title ;

    SetMenuItemTextWithCFString( menu , item , wxMacCFStringHolder(str , encoding) ) ;
}

void UMAEnableMenuItem( MenuRef inMenu , MenuItemIndex inItem , bool enable)
{
    if ( enable )
        EnableMenuItem( inMenu , inItem ) ;
    else
        DisableMenuItem( inMenu , inItem ) ;
}

void UMAAppendSubMenuItem( MenuRef menu , const wxString& title, wxFontEncoding encoding , SInt16 id )
{
    AppendMenuItemTextWithCFString( menu,
                                CFSTR("A"), 0, 0,NULL);
    UMASetMenuItemText( menu, (SInt16) ::CountMenuItems(menu), title , encoding );
    SetMenuItemHierarchicalID( menu , CountMenuItems( menu ) , id ) ;
}

void UMAInsertSubMenuItem( MenuRef menu , const wxString& title, wxFontEncoding encoding , MenuItemIndex item , SInt16 id  )
{
    InsertMenuItemTextWithCFString( menu,
                CFSTR("A"), item, 0, 0);

    UMASetMenuItemText( menu, item+1, title , encoding );
    SetMenuItemHierarchicalID( menu , item+1 , id ) ;
}

void UMASetMenuItemShortcut( MenuRef menu , MenuItemIndex item , wxAcceleratorEntry *entry )
{
    if ( !entry )
        return ;

    UInt8 modifiers = 0 ;
    SInt16 key = entry->GetKeyCode() ;
    if ( key )
    {
        bool explicitCommandKey = (entry->GetFlags() & wxACCEL_CTRL);

        if (entry->GetFlags() & wxACCEL_ALT)
            modifiers |= kMenuOptionModifier ;

        if (entry->GetFlags() & wxACCEL_SHIFT)
            modifiers |= kMenuShiftModifier ;

        SInt16 glyph = 0 ;
        SInt16 macKey = key ;
        if ( key >= WXK_F1 && key <= WXK_F15 )
        {
            if ( !explicitCommandKey )
                modifiers |= kMenuNoCommandModifier ;

            // for some reasons this must be 0 right now
            // everything else leads to just the first function key item
            // to be selected. Thanks to Ryan Wilcox for finding out.
            macKey = 0 ;
            glyph = kMenuF1Glyph + ( key - WXK_F1 ) ;
            if ( key >= WXK_F13 )
                glyph += 12 ;
        }
        else
        {
            switch ( key )
            {
                case WXK_BACK :
                    macKey = kBackspaceCharCode ;
                    glyph = kMenuDeleteLeftGlyph ;
                    break ;

                case WXK_TAB :
                    macKey = kTabCharCode ;
                    glyph = kMenuTabRightGlyph ;
                    break ;

                case kEnterCharCode :
                    macKey = kEnterCharCode ;
                    glyph = kMenuEnterGlyph ;
                    break ;

                case WXK_RETURN :
                    macKey = kReturnCharCode ;
                    glyph = kMenuReturnGlyph ;
                    break ;

                case WXK_ESCAPE :
                    macKey = kEscapeCharCode ;
                    glyph = kMenuEscapeGlyph ;
                    break ;

                case WXK_SPACE :
                    macKey = ' ' ;
                    glyph = kMenuSpaceGlyph ;
                    break ;

                case WXK_DELETE :
                    macKey = kDeleteCharCode ;
                    glyph = kMenuDeleteRightGlyph ;
                    break ;

                case WXK_CLEAR :
                    macKey = kClearCharCode ;
                    glyph = kMenuClearGlyph ;
                    break ;

                case WXK_PAGEUP :
                    macKey = kPageUpCharCode ;
                    glyph = kMenuPageUpGlyph ;
                    break ;

                case WXK_PAGEDOWN :
                    macKey = kPageDownCharCode ;
                    glyph = kMenuPageDownGlyph ;
                    break ;

                case WXK_LEFT :
                    macKey = kLeftArrowCharCode ;
                    glyph = kMenuLeftArrowGlyph ;
                    break ;

                case WXK_UP :
                    macKey = kUpArrowCharCode ;
                    glyph = kMenuUpArrowGlyph ;
                    break ;

                case WXK_RIGHT :
                    macKey = kRightArrowCharCode ;
                    glyph = kMenuRightArrowGlyph ;
                    break ;

                case WXK_DOWN :
                    macKey = kDownArrowCharCode ;
                    glyph = kMenuDownArrowGlyph ;
                    break ;

                case WXK_HOME :
                    macKey = kHomeCharCode ;
                    glyph = kMenuNorthwestArrowGlyph ;
                    break ;

                case WXK_END :
                    macKey = kEndCharCode ;
                    glyph = kMenuSoutheastArrowGlyph ;
                    break ;
                default :
                    macKey = toupper( key ) ;
                    break ;
            }

            // we now allow non command key shortcuts
            // remove in case this gives problems
            if ( !explicitCommandKey )
                modifiers |= kMenuNoCommandModifier ;
        }

        // 1d and 1e have special meaning to SetItemCmd, so
        // do not use for these character codes.
        if (key != WXK_UP && key != WXK_RIGHT && key != WXK_DOWN && key != WXK_LEFT)
            SetItemCmd( menu, item , macKey );

        SetMenuItemModifiers( menu, item , modifiers ) ;

        if ( glyph )
            SetMenuItemKeyGlyph( menu, item , glyph ) ;
    }
}

void UMAAppendMenuItem( MenuRef menu , const wxString& title, wxFontEncoding encoding , wxAcceleratorEntry *entry )
{
    AppendMenuItemTextWithCFString( menu,
                                CFSTR("A"), 0, 0,NULL);
    // don't attempt to interpret metacharacters like a '-' at the beginning (would become a separator otherwise)
    ChangeMenuItemAttributes( menu , ::CountMenuItems(menu), kMenuItemAttrIgnoreMeta , 0 ) ;
    UMASetMenuItemText(menu, (SInt16) ::CountMenuItems(menu), title , encoding );
    UMASetMenuItemShortcut( menu , (SInt16) ::CountMenuItems(menu), entry ) ;
}

void UMAInsertMenuItem( MenuRef menu , const wxString& title, wxFontEncoding encoding , MenuItemIndex item , wxAcceleratorEntry *entry )
{
    InsertMenuItemTextWithCFString( menu,
                CFSTR("A"), item, 0, 0);

    // don't attempt to interpret metacharacters like a '-' at the beginning (would become a separator otherwise)
    ChangeMenuItemAttributes( menu , item+1, kMenuItemAttrIgnoreMeta , 0 ) ;
    UMASetMenuItemText(menu, item+1 , title , encoding );
    UMASetMenuItemShortcut( menu , item+1 , entry ) ;
}

#endif

#if wxMAC_USE_COCOA == 0

static OSStatus UMAGetHelpMenu(
                               MenuRef *        outHelpMenu,
                               MenuItemIndex *  outFirstCustomItemIndex,
                               bool             allowHelpMenuCreation);

static OSStatus UMAGetHelpMenu(
                               MenuRef *        outHelpMenu,
                               MenuItemIndex *  outFirstCustomItemIndex,
                               bool             allowHelpMenuCreation)
{
    static bool s_createdHelpMenu = false ;
    
    if ( !s_createdHelpMenu && !allowHelpMenuCreation )
    {
        return paramErr ;
    }
    
    OSStatus status = HMGetHelpMenu( outHelpMenu , outFirstCustomItemIndex ) ;
    s_createdHelpMenu = ( status == noErr ) ;
    return status ;
}

OSStatus UMAGetHelpMenu(
                        MenuRef *        outHelpMenu,
                        MenuItemIndex *  outFirstCustomItemIndex)
{
    return UMAGetHelpMenu( outHelpMenu , outFirstCustomItemIndex , true );
}

OSStatus UMAGetHelpMenuDontCreate(
                                  MenuRef *        outHelpMenu,
                                  MenuItemIndex *  outFirstCustomItemIndex)
{
    return UMAGetHelpMenu( outHelpMenu , outFirstCustomItemIndex , false );
}

#endif

// window manager

#if wxMAC_USE_QUICKDRAW

void UMAActivateControl( ControlRef inControl )
{
    ::ActivateControl( inControl ) ;
}

void UMADeactivateControl( ControlRef inControl )
{
    ::DeactivateControl( inControl ) ;
}

// others

void UMAHighlightAndActivateWindow( WindowRef inWindowRef , bool inActivate )
{
#if 1 // TODO REMOVE
    if ( inWindowRef )
    {
//        bool isHighlighted = IsWindowHighlited( inWindowRef ) ;
//        if ( inActivate != isHighlighted )
#ifndef __LP64__
        GrafPtr port ;
        GetPort( &port ) ;
        SetPortWindowPort( inWindowRef ) ;
#endif
        HiliteWindow( inWindowRef , inActivate ) ;
        ControlRef control = NULL ;
        ::GetRootControl( inWindowRef , &control ) ;
        if ( control )
        {
            if ( inActivate )
                UMAActivateControl( control ) ;
            else
                UMADeactivateControl( control ) ;
        }
#ifndef __LP64__
        SetPort( port ) ;
#endif
    }
#endif
}

Rect * UMAGetControlBoundsInWindowCoords( ControlRef theControl, Rect *bounds )
{
    GetControlBounds( theControl , bounds ) ;
    
    WindowRef tlwref = GetControlOwner( theControl ) ;
    
    wxTopLevelWindowMac* tlwwx = wxFindWinFromMacWindow( tlwref ) ;
    if ( tlwwx != NULL )
    {
        ControlRef rootControl = tlwwx->GetPeer()->GetControlRef() ;
        HIPoint hiPoint = CGPointMake( 0 , 0 ) ;
        HIViewConvertPoint( &hiPoint , HIViewGetSuperview(theControl) , rootControl ) ;
        OffsetRect( bounds , (short) hiPoint.x , (short) hiPoint.y ) ;
    }
    
    return bounds ;
}

#endif

size_t UMAPutBytesCFRefCallback( void *info, const void *bytes, size_t count )
{
    CFMutableDataRef data = (CFMutableDataRef) info;
    if ( data )
    {
        CFDataAppendBytes( data, (const UInt8*) bytes, count );
    }
    return count;
}

void UMAReleaseCFDataProviderCallback(void *info,
                                      const void *WXUNUSED(data),
                                      size_t WXUNUSED(count))
{
    if ( info )
        CFRelease( (CFDataRef) info );
}

void UMAReleaseCFDataConsumerCallback( void *info )
{
    if ( info )
        CFRelease( (CFDataRef) info );
}

CGDataProviderRef UMACGDataProviderCreateWithCFData( CFDataRef data )
{
    if ( data == NULL )
        return NULL;

    return CGDataProviderCreateWithCFData( data );
}

CGDataConsumerRef UMACGDataConsumerCreateWithCFData( CFMutableDataRef data )
{
    if ( data == NULL )
        return NULL;

    return CGDataConsumerCreateWithCFData( data );
}
#endif  // wxUSE_GUI
