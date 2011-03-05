/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/frame.h
// Purpose:     wxFrame class interface
// Author:      Peter Most
// Modified by:
// Created:     09.08.09
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_FRAME_H_
#define _WX_QT_FRAME_H_

#include "wx/frame.h"
#include "wx/qt/winevent_qt.h"

#include "wx/qt/pointer_qt.h"
#include <QtGui/QMainWindow>

class WXDLLIMPEXP_CORE wxFrame : public wxFrameBase
{
public:
    wxFrame();
    wxFrame(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxFrameNameStr);

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
    
    virtual void SetWindowStyleFlag( long style );

    virtual void AddChild( wxWindowBase *child );
    virtual void RemoveChild( wxWindowBase *child );

    virtual QMainWindow *GetHandle() const;

private:
    wxQtPointer< QMainWindow > m_qtMainWindow;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxFrame );
};



class WXDLLIMPEXP_CORE wxQtMainWindow : public wxQtEventSignalHandler< QMainWindow, wxFrame >
{
    Q_OBJECT

public:
    wxQtMainWindow( wxWindow *parent, wxFrame *handler );

private Q_SLOTS:
};


#endif // _WX_QT_FRAME_H_
