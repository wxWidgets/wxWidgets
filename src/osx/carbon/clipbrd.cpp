/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      Stefan Csomor;
//              Generalized clipboard implementation by Matthew Flatt
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CLIPBOARD

#include "wx/clipbrd.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/bitmap.h"
#endif

#include "wx/metafile.h"
#include "wx/scopedarray.h"

#include "wx/osx/private.h"
#include "wx/osx/private/datatransfer.h"

#define wxUSE_DATAOBJ 1

#include <string.h>

// the trace mask we use with wxLogTrace() - call
// wxLog::AddTraceMask(TRACE_CLIPBOARD) to enable the trace messages from here
// (there will be a *lot* of them!)
#define TRACE_CLIPBOARD wxT("clipboard")

wxIMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject);

wxClipboard::wxClipboard()
{
    m_open = false;
    m_data = NULL ;
}

wxClipboard::~wxClipboard()
{
    delete m_data;
}

void wxClipboard::Clear()
{
    wxDELETE(m_data);

    wxOSXPasteboard::GetGeneralClipboard()->Clear();
}

bool wxClipboard::Flush()
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    wxOSXPasteboard::GetGeneralClipboard()->Flush();

    return true;
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, false, wxT("clipboard already open") );

    m_open = true;

    return true;
}

bool wxClipboard::IsOpened() const
{
    return m_open;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    if ( IsUsingPrimarySelection() )
        return false;

    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );
    wxCHECK_MSG( data, false, wxT("data is invalid") );

    Clear();

    // as we can only store one wxDataObject,
    // this is the same in this implementation
    return AddData( data );
}

bool wxClipboard::AddData( wxDataObject *data )
{
    if ( IsUsingPrimarySelection() )
        return false;

    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );
    wxCHECK_MSG( data, false, wxT("data is invalid") );

    // we can only store one wxDataObject
    Clear();

    data->WriteToSink(wxOSXPasteboard::GetGeneralClipboard());

    Flush();

    m_data = data;

    return true;
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, wxT("clipboard not open") );

    m_open = false;

    // Get rid of cached object.
    // If this is not done, copying data from
    // another application will only work once
    wxDELETE(m_data);
}

bool wxClipboard::IsSupported(const wxDataFormat &dataFormat)
{
    wxLogTrace(TRACE_CLIPBOARD, wxT("Checking if format %s is available"),
               dataFormat.GetId().c_str());

    if ( m_data )
        return m_data->IsSupported( dataFormat );

    return wxOSXPasteboard::GetGeneralClipboard()->IsSupported(dataFormat);
}

bool wxClipboard::GetData( wxDataObject& data )
{
    if ( IsUsingPrimarySelection() )
        return false;

    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    size_t formatcount = data.GetFormatCount(wxDataObject::Set);
    wxScopedArray<wxDataFormat> array(formatcount);
    data.GetAllFormats( array.get(), wxDataObject::Set );

    bool transferred = false;

    if ( m_data )
    {
        for (size_t i = 0; !transferred && i < formatcount; i++)
        {
            wxDataFormat format = array[ i ];
            if ( m_data->IsSupported( format ) )
            {
                int dataSize = m_data->GetDataSize( format );
                transferred = true;

                if (dataSize == 0)
                {
                    data.SetData( format, 0, 0 );
                }
                else
                {
                    wxMemoryBuffer mem(dataSize);
                    void *d = mem.GetWriteBuf(dataSize);
                    m_data->GetDataHere( format, d );
                    data.SetData( format, dataSize, d );
                    mem.UngetWriteBuf(dataSize);
                }
            }
        }
    }

    // get formats from wxDataObjects
    if ( !transferred )
    {
        transferred = data.ReadFromSource(wxOSXPasteboard::GetGeneralClipboard());
    }

    return transferred;
}

#endif
