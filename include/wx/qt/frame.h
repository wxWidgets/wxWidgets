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
    ~wxFrame();
    wxFrame(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxFrameNameStr);

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

    virtual QMainWindow *GetHandle() const;
    virtual WXWidget QtGetScrollBarsContainer() const;

private:
    wxQtPointer< QMainWindow > m_qtFrame;
};



class WXDLLIMPEXP_CORE wxQtFrame : public wxQtEventForwarder< wxFrame, QMainWindow >
{
    Q_OBJECT

public:
    wxQtFrame( wxFrame *frame, QWidget *parent );

private Q_SLOTS:
};


#endif // _WX_QT_FRAME_H_
