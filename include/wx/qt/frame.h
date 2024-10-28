/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/frame.h
// Purpose:     wxFrame class interface
// Author:      Peter Most
// Created:     09.08.09
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_FRAME_H_
#define _WX_QT_FRAME_H_

#include "wx/frame.h"

class QMainWindow;
class QToolBar;

class WXDLLIMPEXP_CORE wxFrame : public wxFrameBase
{
public:
    wxFrame() = default;

    wxFrame(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        Create( parent, id, title, pos, size, style, name );
    }
    virtual ~wxFrame();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual void SetMenuBar(wxMenuBar *menubar) override;

#if wxUSE_STATUSBAR
    virtual void SetStatusBar(wxStatusBar *statusBar ) override;
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    virtual void SetToolBar(wxToolBar *toolbar) override;
#endif // wxUSE_TOOLBAR

    virtual void SetWindowStyleFlag( long style ) override;

    virtual void AddChild( wxWindowBase *child ) override;
    virtual void RemoveChild( wxWindowBase *child ) override;

    QMainWindow *GetQMainWindow() const;

protected:
    virtual wxPoint GetClientAreaOrigin() const override;

    virtual QWidget* QtGetParentWidget() const override;

private:
    // Currently active native toolbar.
    QToolBar* m_qtToolBar = nullptr;

    wxDECLARE_DYNAMIC_CLASS(wxFrame);
};


#endif // _WX_QT_FRAME_H_
