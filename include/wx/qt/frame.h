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

#include <QtCore/QPointer>
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

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual void SetMenuBar(wxMenuBar *menubar);

    virtual QMainWindow *GetHandle() const;
    virtual WXWidget GetContainer() const;

private:
    QPointer< QMainWindow > m_qtFrame;
};



class WXDLLIMPEXP_CORE wxQtFrame : public WindowEventForwarder< QMainWindow >
{
    Q_OBJECT

public:
    wxQtFrame( wxFrame *frame, QWidget *parent );

protected:
    virtual wxWindow *GetEventReceiver();
    
private Q_SLOTS:

private:
    wxFrame *m_frame;

};


#endif // _WX_QT_FRAME_H_
