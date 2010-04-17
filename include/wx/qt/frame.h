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

#include "wx/qt/frame_qt.h"
#include <QtCore/QPointer>

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
    
protected:
        virtual wxQtFrame *GetHandle() const;

private:
        QPointer< wxQtFrame > m_qtFrame;
};

#endif // _WX_QT_FRAME_H_
