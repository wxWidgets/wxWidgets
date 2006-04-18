/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/uma.cpp
// Purpose:     UMA support
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     The wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_GUI

#include "wx/dc.h"
#include <MacTextEditor.h>

#ifndef __DARWIN__
#  include <Navigation.h>
#  if defined(TARGET_CARBON)
#    if PM_USE_SESSION_APIS
#      include <PMCore.h>
#    endif
#    include <PMApplication.h>
#  else
#    include <Printing.h>
#  endif
#endif

#ifndef __DARWIN__
#include <Scrap.h>
#endif
#include "wx/mac/uma.h"


// since we have decided that we only support 8.6 upwards we are
// checking for these minimum requirements in the startup code of
// the application so all wxWidgets code can safely assume that appearance 1.1
// windows manager, control manager, navigation services etc. are
// present

static bool    sUMAHasAppearance = false ;
static long sUMAAppearanceVersion = 0 ;
static long sUMASystemVersion = 0 ;
static bool sUMAHasAquaLayout = false ;

static bool sUMAHasInittedAppearance = false;

extern int gAGABackgroundColor ;
bool UMAHasAppearance() { return sUMAHasAppearance ; }
long UMAGetAppearanceVersion() { return sUMAAppearanceVersion ; }
long UMAGetSystemVersion() { return sUMASystemVersion ; }

static bool    sUMAHasWindowManager = false ;
static long sUMAWindowManagerAttr = 0 ;

bool UMAHasWindowManager() { return sUMAHasWindowManager ; }
long UMAGetWindowManagerAttr() { return sUMAWindowManagerAttr ; }
bool UMAHasAquaLayout() { return sUMAHasAquaLayout ; }


void UMACleanupToolbox()
{
    if (sUMAHasInittedAppearance)
    {
        UnregisterAppearanceClient() ;
    }
    if ( NavServicesAvailable() )
    {
        NavUnload() ;
    }
  if ( TXNTerminateTextension != (void*) kUnresolvedCFragSymbolAddress )
      TXNTerminateTextension( ) ;
}
void UMAInitToolbox( UInt16 inMoreMastersCalls, bool isEmbedded )
{
#if !TARGET_CARBON
    ::MaxApplZone();
    for (long i = 1; i <= inMoreMastersCalls; i++)
        ::MoreMasters();

    if (!isEmbedded)
    {
        ::InitGraf(&qd.thePort);
        ::InitFonts();
        ::InitMenus();
        ::TEInit();
        ::InitDialogs(0L);
        ::FlushEvents(everyEvent, 0);
    }

    long total,contig;
    PurgeSpace(&total, &contig);
#endif

    ::InitCursor();

    if ( Gestalt(gestaltSystemVersion, &sUMASystemVersion) != noErr)
        sUMASystemVersion = 0x0000 ;

    long theAppearance ;
    if ( Gestalt( gestaltAppearanceAttr, &theAppearance ) == noErr )
    {
        sUMAHasAppearance = true ;
        OSStatus status = RegisterAppearanceClient();
        // If status equals appearanceProcessRegisteredErr it means the
        // appearance client already was registered (For example if we run
        // embedded, the host might have registered it). In such a case
        // we don't unregister it later on.
        if (status != appearanceProcessRegisteredErr)
        {
            // Appearance client wasn't registered yet.
            sUMAHasInittedAppearance = true;
        }

        if ( Gestalt( gestaltAppearanceVersion, &theAppearance ) == noErr )
        {
            sUMAAppearanceVersion = theAppearance ;
        }
        else
        {
            sUMAAppearanceVersion = 0x0100 ;
        }
    }
    if ( Gestalt( gestaltWindowMgrAttr, &sUMAWindowManagerAttr ) == noErr )
    {
        sUMAHasWindowManager = sUMAWindowManagerAttr & gestaltWindowMgrPresent ;
    }

#if TARGET_CARBON
// Call currently implicitely done :        InitFloatingWindows() ;
#else
    if (!isEmbedded)
    {
        if ( sUMAHasWindowManager )
            InitFloatingWindows() ;
        else
            InitWindows();
    }
#endif

    if ( NavServicesAvailable() )
    {
        NavLoad() ;
    }

  long menuMgrAttr ;
  Gestalt( gestaltMenuMgrAttr , &menuMgrAttr ) ;
  if ( menuMgrAttr & gestaltMenuMgrAquaLayoutMask )
    sUMAHasAquaLayout = true ;

  if ( TXNInitTextension != (void*) kUnresolvedCFragSymbolAddress )
  {
    FontFamilyID fontId ;
    Str255 fontName ;
    SInt16 fontSize ;
    Style fontStyle ;
    GetThemeFont(kThemeSmallSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;
    GetFNum( fontName, &fontId );

    TXNMacOSPreferredFontDescription fontDescriptions[] =
    {
        { fontId , (fontSize << 16) ,kTXNDefaultFontStyle, kTXNSystemDefaultEncoding }
    } ;
    int noOfFontDescriptions = sizeof( fontDescriptions ) / sizeof(TXNMacOSPreferredFontDescription) ;

      // kTXNAlwaysUseQuickDrawTextMask might be desirable because of speed increases but it crashes the app under OS X upon key stroke
#if 0
    // leads to unexpected content for clients, TODO configurable
    OptionBits options = kTXNWantMoviesMask | kTXNWantSoundMask | kTXNWantGraphicsMask ;
#else
    OptionBits options = 0 ;
#endif

#if TARGET_CARBON
    if ( !UMAHasAquaLayout() )
#endif
    {
        options |= kTXNAlwaysUseQuickDrawTextMask ;
    }
      TXNInitTextension(fontDescriptions,  noOfFontDescriptions, options );
  }


  UMASetSystemIsInitialized(true);

}

/*
Boolean CanUseATSUI()
    {
    long result;
    OSErr err = Gestalt(gestaltATSUVersion, &result);
    return (err == noErr);
    }
*/
// process manager
long UMAGetProcessMode()
{
    OSErr err ;
    ProcessInfoRec processinfo;
    ProcessSerialNumber procno ;

    procno.highLongOfPSN = NULL ;
    procno.lowLongOfPSN = kCurrentProcess ;
    processinfo.processInfoLength = sizeof(ProcessInfoRec);
    processinfo.processName = NULL;
    processinfo.processAppSpec = NULL;

    err = ::GetProcessInformation( &procno , &processinfo ) ;
    wxASSERT( err == noErr ) ;
    return processinfo.processMode ;
}

bool UMAGetProcessModeDoesActivateOnFGSwitch()
{
    return UMAGetProcessMode() & modeDoesActivateOnFGSwitch ;
}

// menu manager

MenuRef UMANewMenu( SInt16 id , const wxString& title , wxFontEncoding encoding )
{
    wxString str = wxStripMenuCodes( title ) ;
    MenuRef menu ;
#if TARGET_CARBON
    CreateNewMenu( id , 0 , &menu ) ;
    SetMenuTitleWithCFString( menu , wxMacCFStringHolder(str , encoding ) ) ;
#else
    Str255 ptitle ;
    wxMacStringToPascal( str , ptitle ) ;
    menu = ::NewMenu( id , ptitle ) ;
#endif
    return menu ;
}

void UMASetMenuTitle( MenuRef menu , const wxString& title , wxFontEncoding encoding)
{
    wxString str = wxStripMenuCodes( title ) ;
#if TARGET_CARBON
    SetMenuTitleWithCFString( menu , wxMacCFStringHolder(str , encoding) ) ;
#else
    Str255 ptitle ;
    wxMacStringToPascal( str , ptitle ) ;
    SetMenuTitle( menu , ptitle ) ;
#endif
}

void UMASetMenuItemText(  MenuRef menu,  MenuItemIndex item, const wxString& title , wxFontEncoding encoding)
{
    wxString str = wxStripMenuCodes( title ) ;
#if TARGET_CARBON
    SetMenuItemTextWithCFString( menu , item , wxMacCFStringHolder(str , encoding) ) ;
#else
    Str255 ptitle ;
    wxMacStringToPascal( str , ptitle ) ;
    SetMenuItemText( menu , item , ptitle ) ;
#endif
}


UInt32 UMAMenuEvent( EventRecord *inEvent )
{
        return MenuEvent( inEvent ) ;
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
    MacAppendMenu(menu, "\pA");
    UMASetMenuItemText(menu, (SInt16) ::CountMenuItems(menu), title , encoding );
    SetMenuItemHierarchicalID( menu , CountMenuItems( menu ) , id ) ;
}

void UMAInsertSubMenuItem( MenuRef menu , const wxString& title, wxFontEncoding encoding , MenuItemIndex item , SInt16 id  )
{
    MacInsertMenuItem(menu, "\pA" , item);
    UMASetMenuItemText(menu, item , title , encoding);
    SetMenuItemHierarchicalID( menu , item , id ) ;
}

void UMASetMenuItemShortcut( MenuRef menu , MenuItemIndex item , wxAcceleratorEntry *entry )
{
    if ( !entry )
        return ;

    UInt8 modifiers = 0 ;
    SInt16 key = entry->GetKeyCode() ;
    if ( key )
    {
        bool explicitCommandKey = false ;

        if ( entry->GetFlags() & wxACCEL_CTRL )
        {
            explicitCommandKey = true ;
        }

        if (entry->GetFlags() & wxACCEL_ALT )
        {
            modifiers |= kMenuOptionModifier ;
        }

        if (entry->GetFlags() & wxACCEL_SHIFT)
        {
            modifiers |= kMenuShiftModifier ;
        }

        SInt16 glyph = 0 ;
        SInt16 macKey = key ;
        if ( key >= WXK_F1 && key <= WXK_F15 )
        {
            // for some reasons this must be 0 right now
            // everything else leads to just the first function key item
            // to be selected. Thanks to Ryan Wilcox for finding out.
            macKey = 0 ;
            glyph = kMenuF1Glyph + ( key - WXK_F1 ) ;
            if ( key >= WXK_F13 )
                glyph += 13 ;
            if ( !explicitCommandKey )
                modifiers |= kMenuNoCommandModifier ;
          }
        else
        {
            switch( key )
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
            }
        }

        SetItemCmd( menu, item , macKey );
        SetMenuItemModifiers(menu, item , modifiers ) ;

        if ( glyph )
            SetMenuItemKeyGlyph(menu, item , glyph ) ;
    }
}

void UMAAppendMenuItem( MenuRef menu , const wxString& title, wxFontEncoding encoding , wxAcceleratorEntry *entry )
{
    MacAppendMenu(menu, "\pA");
    UMASetMenuItemText(menu, (SInt16) ::CountMenuItems(menu), title , encoding );
    UMASetMenuItemShortcut( menu ,  (SInt16) ::CountMenuItems(menu), entry ) ;
}

void UMAInsertMenuItem( MenuRef menu , const wxString& title, wxFontEncoding encoding , MenuItemIndex item , wxAcceleratorEntry *entry )
{
    MacInsertMenuItem( menu , "\pA" , item) ;
    UMASetMenuItemText(menu, item+1 , title , encoding );
    UMASetMenuItemShortcut( menu , item+1 , entry ) ;
}

// quickdraw

#if !TARGET_CARBON

int gPrOpenCounter = 0 ;

OSStatus UMAPrOpen()
{
    OSErr err = noErr ;
    ++gPrOpenCounter ;
    if ( gPrOpenCounter == 1 )
    {
        PrOpen() ;
        err = PrError() ;
        wxASSERT( err == noErr ) ;
    }
    return err ;
}

OSStatus UMAPrClose()
{
    OSErr err = noErr ;
    wxASSERT( gPrOpenCounter >= 1 ) ;
    if ( gPrOpenCounter == 1 )
    {
        PrClose() ;
        err = PrError() ;
        wxASSERT( err == noErr ) ;
    }
    --gPrOpenCounter ;
    return err ;
}

pascal QDGlobalsPtr GetQDGlobalsPtr (void) ;
pascal QDGlobalsPtr GetQDGlobalsPtr (void)
{
    return QDGlobalsPtr (* (Ptr*) LMGetCurrentA5 ( ) - 0xCA);
}

#endif

void UMAShowWatchCursor()
{
    OSErr err = noErr;

    CursHandle watchFob = GetCursor (watchCursor);

    if (!watchFob)
        err = nilHandleErr;
    else
    {
    #if TARGET_CARBON
//        Cursor preservedArrow;
//        GetQDGlobalsArrow (&preservedArrow);
//        SetQDGlobalsArrow (*watchFob);
//        InitCursor ( );
//        SetQDGlobalsArrow (&preservedArrow);
        SetCursor (*watchFob);
    #else
        SetCursor (*watchFob);
    #endif
    }
}

void            UMAShowArrowCursor()
{
#if TARGET_CARBON
    Cursor arrow;
    SetCursor (GetQDGlobalsArrow (&arrow));
#else
    SetCursor (&(qd.arrow));
#endif
}

// window manager

GrafPtr        UMAGetWindowPort( WindowRef inWindowRef )
{
    wxASSERT( inWindowRef != NULL ) ;
#if TARGET_CARBON
    return (GrafPtr) GetWindowPort( inWindowRef ) ;
#else
    return (GrafPtr) inWindowRef ;
#endif
}

void             UMADisposeWindow( WindowRef inWindowRef )
{
    wxASSERT( inWindowRef != NULL ) ;
    DisposeWindow( inWindowRef ) ;
}

void UMASetWTitle( WindowRef inWindowRef , const wxString& title , wxFontEncoding encoding)
{
#if TARGET_CARBON
    SetWindowTitleWithCFString( inWindowRef , wxMacCFStringHolder(title , encoding) ) ;
#else
    Str255 ptitle ;
    wxMacStringToPascal( title , ptitle ) ;
    SetWTitle( inWindowRef , ptitle ) ;
#endif
}

// appearance additions

void UMASetControlTitle( ControlHandle inControl , const wxString& title , wxFontEncoding encoding)
{
#if TARGET_CARBON
    SetControlTitleWithCFString( inControl , wxMacCFStringHolder(title , encoding) ) ;
#else
    Str255 ptitle ;
    wxMacStringToPascal( title , ptitle ) ;
    SetControlTitle( inControl , ptitle ) ;
#endif
}

void UMAActivateControl( ControlHandle inControl )
{
    // we have to add the control after again to the update rgn
    // otherwise updates get lost
    if ( !IsControlActive( inControl ) )
    {
        bool visible = IsControlVisible( inControl ) ;
        if ( visible )
            SetControlVisibility( inControl , false , false ) ;
        ::ActivateControl( inControl ) ;
        if ( visible ) {
            SetControlVisibility( inControl , true , false ) ;
            Rect ctrlBounds ;
            InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
        }
    }
}

void UMADrawControl( ControlHandle inControl )
{
    WindowRef theWindow = GetControlOwner(inControl) ;
    wxMacPortStateHelper help( (GrafPtr) GetWindowPort(theWindow) ) ;
    RgnHandle updateRgn = NewRgn() ;
    GetWindowUpdateRgn( theWindow , updateRgn ) ;
    Point zero = { 0 , 0 } ;
    LocalToGlobal( &zero ) ;
    OffsetRgn( updateRgn , -zero.h , -zero.v ) ;
    ::DrawControlInCurrentPort( inControl ) ;
    InvalWindowRgn( theWindow, updateRgn) ;
    DisposeRgn( updateRgn ) ;
}

void UMAMoveControl( ControlHandle inControl , short x , short y )
{
    bool visible = IsControlVisible( inControl ) ;
    if ( visible ) {
        SetControlVisibility( inControl , false , false ) ;
        Rect ctrlBounds ;
        InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
    }
    ::MoveControl( inControl , x , y ) ;
    if ( visible ) {
        SetControlVisibility( inControl , true , false ) ;
        Rect ctrlBounds ;
        InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
    }
}

void UMASizeControl( ControlHandle inControl , short x , short y )
{
    bool visible = IsControlVisible( inControl ) ;
    if ( visible ) {
        SetControlVisibility( inControl , false , false ) ;
        Rect ctrlBounds ;
        InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
    }
    ::SizeControl( inControl , x , y ) ;
    if ( visible ) {
        SetControlVisibility( inControl , true , false ) ;
        Rect ctrlBounds ;
        InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
    }
}

void UMADeactivateControl( ControlHandle inControl )
{
    // we have to add the control after again to the update rgn
    // otherwise updates get lost
    bool visible = IsControlVisible( inControl ) ;
    if ( visible )
        SetControlVisibility( inControl , false , false ) ;
    ::DeactivateControl( inControl ) ;
    if ( visible ) {
        SetControlVisibility( inControl , true , false ) ;
        Rect ctrlBounds ;
        InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
    }
}
// shows the control and adds the region to the update region
void UMAShowControl                        (ControlHandle             inControl)
{
    SetControlVisibility( inControl , true , false ) ;
    Rect ctrlBounds ;
    InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
}

// shows the control and adds the region to the update region
void UMAHideControl                        (ControlHandle             inControl)
{
    SetControlVisibility( inControl , false , false ) ;
    Rect ctrlBounds ;
    InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
}
// keyboard focus
OSErr UMASetKeyboardFocus                (WindowPtr                 inWindow,
                                 ControlHandle             inControl,
                                 ControlFocusPart         inPart)
{
    OSErr err = noErr;
    GrafPtr port ;
    GetPort( &port ) ;

    SetPortWindowPort( inWindow ) ;

    err = SetKeyboardFocus( inWindow , inControl , inPart ) ;
    SetPort( port ) ;
    return err ;
}


// events
void UMAUpdateControls( WindowPtr inWindow , RgnHandle inRgn )
{
    wxMacPortStateHelper help( (GrafPtr) GetWindowPort( (WindowRef) inWindow) ) ;
    RgnHandle updateRgn = NewRgn() ;
    GetWindowUpdateRgn( inWindow , updateRgn ) ;

    Point zero = { 0 , 0 } ;
    LocalToGlobal( &zero ) ;
    OffsetRgn( updateRgn , -zero.h , -zero.v ) ;

    UpdateControls( inWindow , inRgn ) ;
    InvalWindowRgn( inWindow, updateRgn) ;
    DisposeRgn( updateRgn ) ;
}

bool UMAIsWindowFloating( WindowRef inWindow )
{
    WindowClass cl ;

    GetWindowClass( inWindow , &cl ) ;
    return cl == kFloatingWindowClass ;
}

bool UMAIsWindowModal( WindowRef inWindow )
{
    WindowClass cl ;

    GetWindowClass( inWindow , &cl ) ;
    return cl < kFloatingWindowClass ;
}

// others

void UMAHighlightAndActivateWindow( WindowRef inWindowRef , bool inActivate )
{
    if ( inWindowRef )
    {
//        bool isHighlighted = IsWindowHighlited( inWindowRef ) ;
//        if ( inActivate != isHightlited )
        GrafPtr port ;
        GetPort( &port ) ;
        SetPortWindowPort( inWindowRef ) ;
        HiliteWindow( inWindowRef , inActivate ) ;
        ControlHandle control = NULL ;
        ::GetRootControl( inWindowRef , & control ) ;
        if ( control )
        {
            if ( inActivate )
                UMAActivateControl( control ) ;
            else
                UMADeactivateControl( control ) ;
        }
        SetPort( port ) ;
    }
}

OSStatus UMADrawThemePlacard( const Rect *inRect , ThemeDrawState inState )
{
    return ::DrawThemePlacard( inRect , inState ) ;
}

#if !TARGET_CARBON
static OSStatus helpMenuStatus = noErr ;
static MenuItemIndex firstCustomItemIndex = 0 ;
#endif

OSStatus UMAGetHelpMenu(
  MenuRef *        outHelpMenu,
  MenuItemIndex *  outFirstCustomItemIndex)
{
#if TARGET_CARBON
    return HMGetHelpMenu( outHelpMenu , outFirstCustomItemIndex ) ;
#else
    MenuRef helpMenuHandle ;
    helpMenuStatus = HMGetHelpMenuHandle( &helpMenuHandle ) ;
    if ( firstCustomItemIndex == 0 && helpMenuStatus == noErr )
    {
        firstCustomItemIndex = CountMenuItems( helpMenuHandle ) + 1 ;
    }
    if ( outFirstCustomItemIndex )
    {
        *outFirstCustomItemIndex = firstCustomItemIndex ;
    }
    *outHelpMenu = helpMenuHandle ;
    return helpMenuStatus ;
#endif
}

wxMacPortStateHelper::wxMacPortStateHelper( GrafPtr newport)
{
    m_clip = NULL ;
    Setup( newport ) ;
}

wxMacPortStateHelper::wxMacPortStateHelper()
{
    m_clip = NULL ;
}

void wxMacPortStateHelper::Setup( GrafPtr newport )
{
    GetPort( &m_oldPort ) ;
    SetPort( newport ) ;
    SetOrigin(0,0);
    wxASSERT_MSG( m_clip == NULL , wxT("Cannot call setup twice") ) ;
    m_clip = NewRgn() ;
    GetClip( m_clip );
    m_textFont = GetPortTextFont( (CGrafPtr) newport);
    m_textSize = GetPortTextSize( (CGrafPtr) newport);
    m_textStyle = GetPortTextFace( (CGrafPtr) newport);
    m_textMode = GetPortTextMode( (CGrafPtr) newport);
    GetThemeDrawingState( &m_drawingState ) ;
    m_currentPort = newport ;
}
void wxMacPortStateHelper::Clear()
{
    if ( m_clip )
    {
        DisposeRgn( m_clip ) ;
        DisposeThemeDrawingState( m_drawingState ) ;
        m_clip = NULL ;
    }
}

wxMacPortStateHelper::~wxMacPortStateHelper()
{
    if ( m_clip )
    {
        SetPort( m_currentPort ) ;
        SetClip( m_clip ) ;
        DisposeRgn( m_clip ) ;
        TextFont( m_textFont );
        TextSize( m_textSize );
        TextFace( m_textStyle );
        TextMode( m_textMode );
        SetThemeDrawingState( m_drawingState , true ) ;
        SetPort( m_oldPort ) ;
    }
}

OSStatus UMAPutScrap( Size size , OSType type , void *data )
{
    OSStatus err = noErr ;
#if !TARGET_CARBON
    err = PutScrap( size , type , data ) ;
#else
    ScrapRef    scrap;
    err = GetCurrentScrap (&scrap);
    if ( !err )
    {
        err = PutScrapFlavor (scrap, type , 0, size, data);
    }
#endif
    return err ;
}

#endif  // wxUSE_GUI

#if wxUSE_BASE

static bool sUMASystemInitialized = false ;

bool UMASystemIsInitialized()
{
    return sUMASystemInitialized ;
}

void UMASetSystemIsInitialized(bool val)
{
    sUMASystemInitialized = val;
}


#endif // wxUSE_BASE
