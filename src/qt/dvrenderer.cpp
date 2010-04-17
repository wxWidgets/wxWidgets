/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dvrenderer.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dataview.h"

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

