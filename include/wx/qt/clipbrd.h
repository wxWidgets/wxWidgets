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

    virtual bool Open() override;
    virtual void Close() override;
    virtual bool IsOpened() const override;

    virtual bool AddData( wxDataObject *data ) override;
    virtual bool SetData( wxDataObject *data ) override;
    virtual bool GetData( wxDataObject& data ) override;
    virtual void Clear() override;
    virtual bool IsSupported( const wxDataFormat& format ) override;
    virtual bool IsSupportedAsync(wxEvtHandler *sink) override;

private:
    friend class QtClipBoardSignalHandler;
    int Mode();

    QtClipBoardSignalHandler *m_SignalHandler;
    wxEvtHandlerRef    m_sink;

    bool m_open;

    wxDECLARE_DYNAMIC_CLASS(wxClipboard);
};

#endif // _WX_QT_CLIPBRD_H_
