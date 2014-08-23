/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dvrenderer.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifndef wxUSE_GENERICDATAVIEWCTRL

wxDataViewRenderer::wxDataViewRenderer( const wxString &variantType,
                    wxDataViewCellMode mode,
                    int align)
    : wxDataViewRendererBase( variantType, mode, align )
{
}

void wxDataViewRenderer::SetMode( wxDataViewCellMode mode )
{
}

wxDataViewCellMode wxDataViewRenderer::GetMode() const
{
    return wxDataViewCellMode();
}

void wxDataViewRenderer::SetAlignment( int align )
{
}

int wxDataViewRenderer::GetAlignment() const
{
    return 0;
}

void wxDataViewRenderer::EnableEllipsize(wxEllipsizeMode mode)
{
}

wxEllipsizeMode wxDataViewRenderer::GetEllipsizeMode() const
{
    return wxEllipsizeMode();
}

#endif // !wxUSE_GENERICDATAVIEWCTRL

#endif // wxUSE_DATAVIEWCTRL
