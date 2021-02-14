/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/clipbrd.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>
#include <QtCore/QMimeData>

#include "wx/clipbrd.h"
#include "wx/scopedarray.h"
#include "wx/scopeguard.h"
#include "wx/qt/private/converter.h"

// ----------------------------------------------------------------------------
// wxClipboard ctor/dtor
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject);

#define QtClipboard  QApplication::clipboard()
typedef wxScopedArray<wxDataFormat> wxDataFormatArray;

class QtClipBoardSignalHandler : public QObject
{
public:
    QtClipBoardSignalHandler(wxClipboard *clipboard)
        : m_clipboard(clipboard)
    {
        connect(QtClipboard, &QClipboard::changed, this, &QtClipBoardSignalHandler::changed);
    }

private:
    void changed( QClipboard::Mode mode)
    {
        if ( mode != m_clipboard->Mode() || !m_clipboard->m_sink )
            return;

        wxClipboardEvent *event = new wxClipboardEvent(wxEVT_CLIPBOARD_CHANGED);
        event->SetEventObject( m_clipboard );
        m_clipboard->m_sink->QueueEvent( event );
        m_clipboard->m_sink.Release();
    }

    wxClipboard *m_clipboard;
};

wxClipboard::wxClipboard()
{
    m_SignalHandler = new QtClipBoardSignalHandler(this);
    m_open = false;
}

wxClipboard::~wxClipboard()
{
    delete m_SignalHandler;
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, false, wxT("clipboard already open") );

    m_open = true;

    return true;
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, wxT("clipboard not open") );

    m_open = false;
}

bool wxClipboard::IsOpened() const
{
    return m_open;
}

bool wxClipboard::AddData( wxDataObject *data )
{
    QMimeData *MimeData = new QMimeData;
    data->QtAddDataTo(*MimeData);
    delete data;

    QtClipboard->setMimeData(MimeData, (QClipboard::Mode)Mode());

    return true;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    wxCHECK_MSG( data, false, wxT("data is invalid") );

    Clear();

    return AddData( data );
}

bool wxClipboard::GetData( wxDataObject& data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    const QMimeData *MimeData = QtClipboard->mimeData( (QClipboard::Mode)Mode() );
    return data.QtSetDataFrom(*MimeData);
}

void wxClipboard::Clear()
{
    QtClipboard->clear( (QClipboard::Mode)Mode() );
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{
    const QMimeData *data = QtClipboard->mimeData( (QClipboard::Mode)Mode() );
    if (format.GetType() == wxDF_BITMAP)
    {
        return data->hasImage();
    }
    return data->hasFormat(wxQtConvertString(format.GetMimeType()));
}

bool wxClipboard::IsSupportedAsync(wxEvtHandler *sink)
{
    if (m_sink.get())
        return false;  // currently busy, come back later

    wxCHECK_MSG( sink, false, wxT("no sink given") );

    m_sink = sink;

    return true;
}

int wxClipboard::Mode()
{
    return m_usePrimary ? QClipboard::Selection : QClipboard::Clipboard;
}
