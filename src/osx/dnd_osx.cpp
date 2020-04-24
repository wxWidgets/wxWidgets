///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/dnd_osx.cpp
// Purpose:     Mac common wxDropTarget, wxDropSource implementations
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) 1998 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/dnd.h"
#include "wx/scopedarray.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/gdicmn.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"

//----------------------------------------------------------------------------
// wxDropTarget
//----------------------------------------------------------------------------

wxDragResult wxDropTarget::OnDragOver(
    wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
    wxDragResult def )
{
    return CurrentDragHasSupportedFormat() ? def : wxDragNone;
}

wxDataFormat wxDropTarget::GetMatchingPair()
{
    if (m_dataObject == NULL)
        return wxDataFormat(wxDF_INVALID);

    if (wxDropSource* currentSource = wxDropSource::GetCurrentDropSource())
    {
        wxDataObject* data = currentSource->GetDataObject();

        if (data)
        {
            size_t formatcount = data->GetFormatCount();
            wxScopedArray<wxDataFormat> array(formatcount);
            data->GetAllFormats(array.get());
            for (size_t i = 0; i < formatcount; i++)
            {
                wxDataFormat format = array[i];
                if (m_dataObject->IsSupported(format, wxDataObject::Set))
                {
                    return format;
                }
            }
        }
    }
    // TODO: m_currentDragPasteboard

    return wxDataFormat(wxDF_INVALID);
}

bool wxDropTarget::OnDrop( wxCoord WXUNUSED(x), wxCoord WXUNUSED(y) )
{
    if (m_dataObject == NULL)
        return false;

    return CurrentDragHasSupportedFormat();
}

wxDragResult wxDropTarget::OnData(
    wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
    wxDragResult def )
{
    if (m_dataObject == NULL)
        return wxDragNone;

    if (!CurrentDragHasSupportedFormat())
        return wxDragNone;

    return GetData() ? def : wxDragNone;
}

bool wxDropTarget::CurrentDragHasSupportedFormat()
{
    bool supported = false;
    if (m_dataObject == NULL)
        return false;

    if ( wxDropSource* currentSource = wxDropSource::GetCurrentDropSource() )
    {
        wxDataObject* data = currentSource->GetDataObject();

        if ( data )
        {
            size_t formatcount = data->GetFormatCount();
            wxScopedArray<wxDataFormat> array(formatcount);
            data->GetAllFormats( array.get() );
            for (size_t i = 0; !supported && i < formatcount; i++)
            {
                wxDataFormat format = array[i];
                if ( m_dataObject->IsSupported( format, wxDataObject::Set ) )
                {
                    supported = true;
                    break;
                }
            }
        }
    }

    if ( !supported )
    {
        supported = m_dataObject->CanReadFromSource( m_currentDragPasteboard );
    }

    return supported;
}

bool wxDropTarget::GetData()
{
    if (m_dataObject == NULL)
        return false;

    if ( !CurrentDragHasSupportedFormat() )
        return false;

    bool transferred = false;
    if ( wxDropSource* currentSource = wxDropSource::GetCurrentDropSource() )
    {
        wxDataObject* data = currentSource->GetDataObject();

        if (data != NULL)
        {
            size_t formatcount = data->GetFormatCount();
            wxScopedArray<wxDataFormat> array(formatcount);
            data->GetAllFormats( array.get() );
            for (size_t i = 0; !transferred && i < formatcount; i++)
            {
                wxDataFormat format = array[i];
                if ( m_dataObject->IsSupported( format, wxDataObject::Set ) )
                {
                    int size = data->GetDataSize( format );
                    transferred = true;

                    if (size == 0)
                    {
                        m_dataObject->SetData( format, 0, 0 );
                    }
                    else
                    {
                        wxCharBuffer d(size);
                        data->GetDataHere( format, d.data() );
                        m_dataObject->SetData( format, size, d.data() );
                    }
                }
            }
        }
    }

    if ( !transferred )
    {
        transferred = m_dataObject->ReadFromSource(m_currentDragPasteboard);
    }

    return transferred;
}

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// drag request

wxDropSource::~wxDropSource()
{
}

bool wxDropSource::MacInstallDefaultCursor(wxDragResult effect)
{
    const wxCursor& cursor = GetCursor(effect);
    bool result = cursor.IsOk();

    if ( result )
        cursor.MacInstall();

    return result;
}

#endif // wxUSE_DRAG_AND_DROP

