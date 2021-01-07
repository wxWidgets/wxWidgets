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
    wxDataFormat supported;
    if (m_dataObject != NULL)
    {
        if ( wxDropSource* currentSource = wxDropSource::GetCurrentDropSource() )
        {
            wxDataObject* data = currentSource->GetDataObject();
        
            if ( data )
                supported = m_dataObject->GetSupportedFormatInSource(data);
        }
    
        if ( supported == wxDF_INVALID )
            supported = m_dataObject->GetSupportedFormatInSource( m_currentDragPasteboard );
    }
    
    return supported;
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
    return GetMatchingPair() != wxDF_INVALID;
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
            transferred = m_dataObject->ReadFromSource(data);
    }

    if ( !transferred )
        transferred = m_dataObject->ReadFromSource(m_currentDragPasteboard);

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

