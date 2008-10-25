///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/nonownedwnd.h
// Purpose:     declares wxNonOwnedWindow class
// Author:      Stefan Csomor
// Modified by: 
// Created:     2008-03-24
// RCS-ID:      $Id: nonownedwnd.h 46993 2007-06-28 08:46:04Z VS $
// Copyright:   (c) 2008 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_NONOWNEDWND_H_
#define _WX_MAC_NONOWNEDWND_H_

#include "wx/window.h"

#if wxUSE_SYSTEM_OPTIONS
    #define wxMAC_WINDOW_PLAIN_TRANSITION _T("mac.window-plain-transition")
#endif

//-----------------------------------------------------------------------------
// wxNonOwnedWindow
//-----------------------------------------------------------------------------

// This class represents "non-owned" window. A window is owned by another
// window if it has a parent and is positioned within the parent. For example,
// wxFrame is non-owned, because even though it can have a parent, it's
// location is independent of it.  This class is for internal use only, it's
// the base class for wxTopLevelWindow and wxPopupWindow.

class WXDLLIMPEXP_CORE wxNonOwnedWindow : public wxWindow
{
public:
    // constructors and such
    wxNonOwnedWindow() { Init(); }

    wxNonOwnedWindow(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxPanelNameStr)
    {
        Init();

        (void)Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);

    virtual ~wxNonOwnedWindow();

    virtual wxPoint GetClientAreaOrigin() const;

    // implement base class pure virtuals

    virtual bool SetShape(const wxRegion& region);

    virtual bool SetTransparent(wxByte alpha);
    virtual bool CanSetTransparent();

    virtual bool SetBackgroundStyle(wxBackgroundStyle style);

    // implementation from now on
    // --------------------------

    static void MacDelayedDeactivation(long timestamp);
    virtual void MacCreateRealWindow( const wxPoint& pos,
                                      const wxSize& size,
                                      long style,
                                      const wxString& name ) ;
 
    WXWindow MacGetWindowRef() { return m_macWindow ; }
    virtual void MacActivate( long timestamp , bool inIsActivating ) ;
    virtual void MacPerformUpdates() ;

    virtual void Raise();
    virtual void Lower();
    virtual bool Show( bool show = true );
    
    void ShowNoActivate( bool show = true );

/*
    virtual bool ShowWithEffect(wxShowEffect effect,
                                unsigned timeout = 0,
                                wxDirection dir = wxBOTTOM);
   
    virtual bool HideWithEffect(wxShowEffect effect,
                                unsigned timeout = 0,
                                wxDirection dir = wxBOTTOM);
*/
     
    virtual void SetExtraStyle(long exStyle) ;

    virtual bool SetBackgroundColour( const wxColour &colour );
    
    virtual void MacInstallTopLevelWindowEventHandler() ;

    bool MacGetMetalAppearance() const ;
    bool MacGetUnifiedAppearance() const ;

    void MacChangeWindowAttributes( wxUint32 attributesToSet , wxUint32 attributesToClear ) ;
    wxUint32 MacGetWindowAttributes() const ;

    WXEVENTHANDLERREF    MacGetEventHandler() { return m_macEventHandler ; }

    virtual void        MacGetContentAreaInset( int &left , int &top , int &right , int &bottom ) ;

protected:
    // common part of all ctors
    void Init();

    virtual void DoGetPosition( int *x, int *y ) const;
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual void DoGetClientSize(int *width, int *height) const;

    WXWindow m_macWindow ;

    wxWindowMac* m_macFocus ;

    static wxNonOwnedWindow *s_macDeactivateWindow;
private :
    // KH: We cannot let this be called directly since the metal appearance is now managed by an
    // extra style. Calling this function directly can result in blank white window backgrounds.
    // This is because the ExtraStyle flags get out of sync with the metal appearance and the metal
    // logic & checks cease to work as expected. To set the metal appearance, use SetExtraStyle.
    void MacSetMetalAppearance( bool on ) ;
    void MacSetUnifiedAppearance( bool on ) ;
    // binary compatible workaround TODO REPLACE
    void DoMacCreateRealWindow( wxWindow *parent,
                                      const wxPoint& pos,
                                      const wxSize& size,
                                      long style,
                                      const wxString& name );

    WXEVENTHANDLERREF    m_macEventHandler ;
};

// list of all frames and modeless dialogs
extern WXDLLIMPEXP_DATA_CORE(wxWindowList) wxModelessWindows;


#endif // _WX_MAC_NONOWNEDWND_H_
