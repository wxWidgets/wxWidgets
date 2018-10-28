/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/clipbrd.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>

#include "wx/clipbrd.h"
#include "wx/scopedarray.h"
#include "wx/scopeguard.h"
#include "wx/qt/private/converter.h"

#include <QtCore/QMimeData>

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
    const size_t count = data->GetFormatCount();
    wxDataFormatArray formats(count);
    data->GetAllFormats(formats.get());

    // how to add timestamp?

    // Unfortunately I cannot find a way to use the qt clipboard with 
    // a callback to select the data type, so I must copy it all here

    for ( size_t i = 0; i < count; i++ )
    {
        const wxDataFormat format(formats[i]);

        int size = data->GetDataSize( format );
        if ( !size )
            continue;

        QByteArray bytearray(size, 0);
        data->GetDataHere(format, bytearray.data());
        MimeData->setData(wxQtConvertString(format.m_MimeType), bytearray);
    }

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
    const size_t count = data.GetFormatCount(wxDataObject::Set);
    wxDataFormatArray formats(count);
    data.GetAllFormats(formats.get(), wxDataObject::Set);

    for ( size_t i = 0; i < count; i++ )
    {
        const wxDataFormat format(formats[i]);
        
        // is this format supported by clipboard ?
        if( !MimeData->hasFormat(wxQtConvertString(format.m_MimeType)) )
            continue;

        wxTextDataObject *textdata = dynamic_cast<wxTextDataObject*>(&data);
        if ( textdata )
            textdata->SetText(wxQtConvertString(MimeData->text()));
        else
        {
            QByteArray bytearray = MimeData->data( wxQtConvertString(format.m_MimeType) ).data();
            data.SetData(format, bytearray.size(), bytearray.constData());
        }

        return true;
    }

    return false;
}

void wxClipboard::Clear()
{
    QtClipboard->clear( (QClipboard::Mode)Mode() );
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{
    const QMimeData *data = QtClipboard->mimeData( (QClipboard::Mode)Mode() );
    return data->hasFormat(wxQtConvertString(format.m_MimeType));
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
