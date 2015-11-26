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

    virtual bool Open();
    virtual void Close();
    virtual bool IsOpened() const;

    virtual bool AddData( wxDataObject *data );
    virtual bool SetData( wxDataObject *data );
    virtual bool GetData( wxDataObject& data );
    virtual void Clear();
    virtual bool IsSupported( const wxDataFormat& format );
    virtual bool IsSupportedAsync(wxEvtHandler *sink);

private:
    friend class QtClipBoardSignalHandler;
    int Mode();

    QtClipBoardSignalHandler *m_SignalHandler;
    wxEvtHandlerRef    m_sink;

    bool m_open;

    wxDECLARE_DYNAMIC_CLASS(wxClipboard);
};

#endif // _WX_QT_CLIPBRD_H_
