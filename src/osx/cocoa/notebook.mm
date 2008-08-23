///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/notebook.mm
// Purpose:     implementation of wxNotebook
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: notebmac.cpp 55079 2008-08-13 14:56:42Z PC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

#include "wx/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/image.h"
#endif

#include "wx/string.h"
#include "wx/imaglist.h"
#include "wx/osx/private.h"

@interface wxNSTabView : NSTabView
{
    wxWidgetImpl* m_impl;
}

- (void)setImplementation: (wxWidgetImpl *) theImplementation;
- (wxWidgetImpl*) implementation;
- (BOOL) isFlipped;
- (int) intValue;
- (void) setIntValue:(int) v;

@end

@implementation wxNSTabView

- (void)setImplementation: (wxWidgetImpl *) theImplementation
{
    m_impl = theImplementation;
}

- (wxWidgetImpl*) implementation
{
    return m_impl;
}

- (BOOL) isFlipped
{
    return YES;
}

- (int) intValue
{
    NSTabViewItem* selectedItem = [self selectedTabViewItem];
    if ( selectedItem == nil )
        return 0;
    else
        return [self indexOfTabViewItem:selectedItem]+1;
}

- (void) setIntValue:(int) v
{
    [self selectTabViewItemAtIndex:(v-1)];
}

@end

/*
#if 0
    Rect bounds = wxMacGetBoundsForControl( this, pos, size );

    if ( bounds.right <= bounds.left )
        bounds.right = bounds.left + 100;
    if ( bounds.bottom <= bounds.top )
        bounds.bottom = bounds.top + 100;

    UInt16 tabstyle = kControlTabDirectionNorth;
    if ( HasFlag(wxBK_LEFT) )
        tabstyle = kControlTabDirectionWest;
    else if ( HasFlag( wxBK_RIGHT ) )
        tabstyle = kControlTabDirectionEast;
    else if ( HasFlag( wxBK_BOTTOM ) )
        tabstyle = kControlTabDirectionSouth;

    ControlTabSize tabsize;
    switch (GetWindowVariant())
    {
        case wxWINDOW_VARIANT_MINI:
            tabsize = 3 ;
            break;

        case wxWINDOW_VARIANT_SMALL:
            tabsize = kControlTabSizeSmall;
            break;

        default:
            tabsize = kControlTabSizeLarge;
            break;
    }

    m_peer = new wxMacControl( this );
    OSStatus err = CreateTabsControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds,
        tabsize, tabstyle, 0, NULL, m_peer->GetControlRefAddr() );
    verify_noerr( err );
#endif
*/
wxWidgetImplType* wxWidgetImpl::CreateTabView( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle)
{
    NSView* sv = (wxpeer->GetParent()->GetHandle() );
    
    NSRect r = wxToNSRect( sv, wxRect( pos, size) );
    // Rect bounds = wxMacGetBoundsForControl( wxpeer, pos , size ) ;
    /*    if ( bounds.right <= bounds.left )
        bounds.right = bounds.left + 100;
    if ( bounds.bottom <= bounds.top )
        bounds.bottom = bounds.top + 100;
    */
    
    NSTabViewType tabstyle = NSTopTabsBezelBorder;
   if ( style & wxBK_LEFT )
        tabstyle = NSLeftTabsBezelBorder;
    else if ( style & wxBK_RIGHT )
        tabstyle = NSRightTabsBezelBorder;
    else if ( style & wxBK_BOTTOM )
        tabstyle = NSBottomTabsBezelBorder;
    
    wxNSTabView* v = [[wxNSTabView alloc] initWithFrame:r];
    [sv addSubview:v];
    [v setTabViewType:tabstyle];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

void wxWidgetCocoaImpl::SetupTabs( const wxNotebook& notebook)
{
    int pcount = notebook.GetPageCount();
    int cocoacount = [ (wxNSTabView*) m_osxView numberOfTabViewItems ];
    
    if ( pcount > cocoacount )
    {
        for ( int i = cocoacount ; i < pcount ; ++i )
        {
            NSTabViewItem* item = [[NSTabViewItem alloc] init];
            [(wxNSTabView*) m_osxView addTabViewItem:item];
            [item release];
        }
    }
    else if ( pcount < cocoacount )
    {
        for ( int i = cocoacount -1 ; i >= pcount ; --i )
        {
            NSTabViewItem* item = [(wxNSTabView*) m_osxView tabViewItemAtIndex:i];
            [(wxNSTabView*) m_osxView removeTabViewItem:item];
        }
    }
    
    for ( int i = 0 ; i < pcount ; ++i )
    {
        wxNotebookPage* page = notebook.GetPage(i);
        NSTabViewItem* item = [(wxNSTabView*) m_osxView tabViewItemAtIndex:i];
        [item setLabel:wxCFStringRef( page->GetLabel() , notebook.GetFont().GetEncoding() ).AsNSString()];
    }
/*
    SetMaximum( GetPageCount() ) ;

    wxNotebookPage *page;
    ControlTabInfoRecV1 info;

    const size_t countPages = GetPageCount();
    for (size_t ii = 0; ii < countPages; ii++)
    {
        page = (wxNotebookPage*) notebook->GetPage[ii];
        info.version = kControlTabInfoVersionOne;
        info.iconSuiteID = 0;
        wxCFStringRef cflabel( page->GetLabel(), GetFont().GetEncoding() ) ;
        info.name = cflabel ;
        SetData<ControlTabInfoRecV1>( ii + 1, kControlTabInfoTag, &info ) ;

        if ( GetImageList() && GetPageImage(ii) >= 0 )
        {
            const wxBitmap bmap = GetImageList()->GetBitmap( GetPageImage( ii ) ) ;
            if ( bmap.Ok() )
            {
                ControlButtonContentInfo info ;

                wxMacCreateBitmapButton( &info, bmap ) ;

                OSStatus err = SetData<ControlButtonContentInfo>( ii + 1, kControlTabImageContentTag, &info );
                if ( err != noErr )
                {
                    wxFAIL_MSG("Error when setting icon on tab");
                }

                wxMacReleaseBitmapButton( &info ) ;
            }
        }
        SetTabEnabled( ii + 1, true ) ;
    }
*/
}

#endif
