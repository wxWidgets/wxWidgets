///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/toplevel.h
// Purpose:     wxTopLevelWindowMac is the MSW implementation of wxTLW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_TOPLEVEL_H_
#define _WX_MSW_TOPLEVEL_H_

#ifdef __GNUG__
    #pragma interface "toplevel.h"
#endif

// ----------------------------------------------------------------------------
// wxTopLevelWindowMac
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTopLevelWindowMac : public wxTopLevelWindowBase
{
public:
    // constructors and such
    wxTopLevelWindowMac() { Init(); }

    wxTopLevelWindowMac(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_FRAME_STYLE,
                        const wxString& name = wxFrameNameStr)
    {
        Init();

        (void)Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual ~wxTopLevelWindowMac();

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = TRUE);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = TRUE);
    virtual bool IsIconized() const;
    virtual void SetIcon(const wxIcon& icon);
    virtual void Restore();

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL) { return FALSE; }
    virtual bool IsFullScreen() const { return FALSE; }

    // implementation from now on
    // --------------------------

	virtual void MacCreateRealWindow( const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name ) ;
	static WindowRef MacGetWindowInUpdate() { return s_macWindowInUpdate ; }
	virtual void MacGetPortParams(Point* localOrigin, Rect* clipRect, WindowRef *window , wxWindowMac** rootwin ) ;
	virtual void MacDoGetPortClientParams(Point* localOrigin, Rect* clipRect, WindowRef *window  , wxWindowMac** rootwin) ;
  virtual void Clear() ;
  virtual ControlHandle MacGetContainerForEmbedding() ;
  WindowRef MacGetWindowRef() { return m_macWindow ; }
	virtual void MacActivate( EventRecord *ev , bool inIsActivating ) ;
	virtual void MacUpdate( long timestamp ) ;
	virtual void MacMouseDown( EventRecord *ev , short windowPart ) ;
	virtual void MacMouseUp( EventRecord *ev , short windowPart ) ;
	virtual void MacMouseMoved( EventRecord *ev , short windowPart ) ;
	virtual void MacKeyDown( EventRecord *ev ) ;
	virtual void MacFireMouseEvent( EventRecord *ev ) ;
  virtual void Raise();
  virtual void Lower();
  virtual void SetTitle( const wxString& title);
  virtual bool Show( bool show = TRUE );
  virtual void DoMoveWindow(int x, int y, int width, int height);
  void MacInvalidate( const Rect * rect, bool eraseBackground ) ;
protected:
    // common part of all ctors
    void Init();

    // is the frame currently iconized?
    bool m_iconized;

    // should the frame be maximized when it will be shown? set by Maximize()
    // when it is called while the frame is hidden
    bool m_maximizeOnShow;

    SInt16 m_macWindowBackgroundTheme ;
    WindowRef m_macWindow ;
    ControlHandle m_macRootControl ;
    wxWindowMac* m_macFocus ;
    RgnHandle m_macNoEraseUpdateRgn ;
    bool m_macNeedsErasing ;

	  static WindowRef s_macWindowInUpdate ;
};

// list of all frames and modeless dialogs
extern WXDLLEXPORT_DATA(wxWindowList) wxModelessWindows;

// associate mac windows with wx counterparts

wxTopLevelWindowMac* wxFindWinFromMacWindow( WindowRef inWindow ) ;
void wxAssociateWinWithMacWindow(WindowRef inWindow, wxTopLevelWindowMac *win) ;
void wxRemoveMacWindowAssociation(wxTopLevelWindowMac *win) ;


#endif // _WX_MSW_TOPLEVEL_H_

