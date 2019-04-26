/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/toolbar.h
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CLIPBRD_H_
#define _WX_QT_CLIPBRD_H_

#include "wx/weakref.h"

class QtClipBoardSignalHandler;

class WXDLLIMPEXP_CORE wxClipboard : public wxClipboardBase
{
public:
    wxClipboard();
    ~wxClipboard();

    virtual bool Open() wxOVERRIDE;
    virtual void Close() wxOVERRIDE;
    virtual bool IsOpened() const wxOVERRIDE;

    virtual bool AddData( wxDataObject *data ) wxOVERRIDE;
    virtual bool SetData( wxDataObject *data ) wxOVERRIDE;
    virtual bool GetData( wxDataObject& data ) wxOVERRIDE;
    virtual void Clear() wxOVERRIDE;
    virtual bool IsSupported( const wxDataFormat& format ) wxOVERRIDE;
    virtual bool IsSupportedAsync(wxEvtHandler *sink) wxOVERRIDE;

private:
    friend class QtClipBoardSignalHandler;
    int Mode();

    QtClipBoardSignalHandler *m_SignalHandler;
    wxEvtHandlerRef    m_sink;

    bool m_open;

    wxDECLARE_DYNAMIC_CLASS(wxClipboard);
};

#endif // _WX_QT_CLIPBRD_H_
