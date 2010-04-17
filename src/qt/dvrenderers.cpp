/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dvrenderers.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dataview.h"

wxDataViewTextRenderer::wxDataViewTextRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode,
                        int align)
    : wxDataViewRenderer( varianttype, mode, align )
{
}

bool wxDataViewTextRenderer::SetValue( const wxVariant &value )
{
    return false;
}

bool wxDataViewTextRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

void wxDataViewTextRenderer::SetAlignment( int align )
{
}

//==============================================================================

wxDataViewBitmapRenderer::wxDataViewBitmapRenderer( const wxString &varianttype,
                          wxDataViewCellMode mode,
                          int align)
    : wxDataViewRenderer( varianttype, mode, align )
{
}

bool wxDataViewBitmapRenderer::SetValue( const wxVariant &value )
{
    return false;
}

bool wxDataViewBitmapRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

//==============================================================================


wxDataViewToggleRenderer::wxDataViewToggleRenderer( const wxString &varianttype,
                          wxDataViewCellMode mode,
                          int align)
    : wxDataViewRenderer( varianttype, mode, align )
{
}

bool wxDataViewToggleRenderer::SetValue( const wxVariant &value )
{
    return false;
}

bool wxDataViewToggleRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

