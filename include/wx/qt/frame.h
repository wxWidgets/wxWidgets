/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/frame.h
// Purpose:     wxFrame class interface
// Author:      Peter Most
// Modified by:
// Created:     09.08.09
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_FRAME_H_
#define _WX_QT_FRAME_H_

#include "wx/frame.h"

class QMainWindow;
class QScrollArea;

class WXDLLIMPEXP_CORE wxFrame : public wxFrameBase
{
public:
    wxFrame() { Init(); }
    wxFrame(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxFrameNameStr)
    {
        Init();

        Create( parent, id, title, pos, size, style, name );
    }
    virtual ~wxFrame();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual void SetMenuBar(wxMenuBar *menubar);
    virtual void SetStatusBar(wxStatusBar *statusBar );
    virtual void SetToolBar(wxToolBar *toolbar);

    virtual void SetWindowStyleFlag( long style );

    virtual void AddChild( wxWindowBase *child );
    virtual void RemoveChild( wxWindowBase *child );

    QMainWindow *GetQMainWindow() const;
    virtual QScrollArea *QtGetScrollBarsContainer() const;

protected:
    virtual void DoGetClientSize(int *width, int *height) const;

private:
    // Common part of all ctors.
    void Init()
    {
        m_qtToolBar = NULL;
    }


    // Currently active native toolbar.
    class QToolBar* m_qtToolBar;

    wxDECLARE_DYNAMIC_CLASS( wxFrame );
};


#endif // _WX_QT_FRAME_H_
