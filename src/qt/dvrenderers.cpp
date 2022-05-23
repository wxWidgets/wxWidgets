/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dvrenderers.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifndef wxHAS_GENERIC_DATAVIEWCTRL

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

bool
wxDataViewBitmapRenderer::IsCompatibleVariantType(const wxString& variantType) const
{
    return variantType == wxS("wxBitmap")
            || variantType == wxS("wxIcon");
}

//==============================================================================

wxDataViewCustomRenderer::wxDataViewCustomRenderer( const wxString &variantType, wxDataViewCellMode mode,
        int align,bool no_init )
    : wxDataViewRenderer( variantType, mode, align )
{

}

wxDataViewCustomRenderer::~wxDataViewCustomRenderer()
{

}


bool wxDataViewCustomRenderer::Render( wxRect cell, wxDC *dc, int state )
{
    return false;
}

void wxDataViewCustomRenderer::RenderText( const wxString &text, int xoffset,  wxRect cell, wxDC *dc, int state )
{

}

wxSize wxDataViewCustomRenderer::GetSize() const
{
    return wxSize();
}


wxDC *wxDataViewCustomRenderer::GetDC()
{
    return NULL;
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

//==============================================================================

wxDataViewProgressRenderer::wxDataViewProgressRenderer( const wxString &label,
        const wxString &varianttype, wxDataViewCellMode mode, int align )
    : wxDataViewCustomRenderer( varianttype, mode, align )
{

}
wxDataViewProgressRenderer::~wxDataViewProgressRenderer()
{

}

bool wxDataViewProgressRenderer::SetValue( const wxVariant &value )
{
    return false;
}

bool wxDataViewProgressRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

bool wxDataViewProgressRenderer::Render( wxRect cell, wxDC *dc, int state )
{
    return false;
}

wxSize wxDataViewProgressRenderer::GetSize() const
{
    return wxSize();
}

//==============================================================================

wxDataViewIconTextRenderer::wxDataViewIconTextRenderer( const wxString &varianttype,
        wxDataViewCellMode mode, int align )
    : wxDataViewCustomRenderer( varianttype, mode, align )
{

}

wxDataViewIconTextRenderer::~wxDataViewIconTextRenderer()
{

}

bool wxDataViewIconTextRenderer::SetValue( const wxVariant &value )
{
    return false;
}

bool wxDataViewIconTextRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

bool wxDataViewIconTextRenderer::Render( wxRect cell, wxDC *dc, int state )
{
    return false;
}

wxSize wxDataViewIconTextRenderer::GetSize() const
{
    return wxSize();
}

wxControl *wxDataViewIconTextRenderer::CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value )
{
    return NULL;
}

bool wxDataViewIconTextRenderer::GetValueFromEditorCtrl( wxControl* editor, wxVariant &value )
{
    return false;
}

//==============================================================================

wxDataViewDateRenderer::wxDataViewDateRenderer( const wxString &varianttype,
        wxDataViewCellMode mode, int align )
    : wxDataViewCustomRenderer( varianttype, mode, align )
{

}


bool wxDataViewDateRenderer::SetValue( const wxVariant &value )
{
    return false;
}

bool wxDataViewDateRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

bool wxDataViewDateRenderer::Render( wxRect cell, wxDC *dc, int state )
{
    return false;
}

wxSize wxDataViewDateRenderer::GetSize() const
{
    return wxSize();
}

bool wxDataViewDateRenderer::Activate( wxRect cell, wxDataViewModel *model,
        const wxDataViewItem &item, unsigned int col )
{
    return false;
}

//==============================================================================

wxDataViewChoiceRenderer::wxDataViewChoiceRenderer( const wxArrayString &choices,
        wxDataViewCellMode mode, int alignment )
    : wxDataViewCustomRenderer(wxT("string"), mode, alignment )
{

}

bool wxDataViewChoiceRenderer::Render( wxRect rect, wxDC *dc, int state )
{
    return false;
}

wxSize wxDataViewChoiceRenderer::GetSize() const
{
    return wxSize();
}

bool wxDataViewChoiceRenderer::SetValue( const wxVariant &value )
{
    return false;
}

bool wxDataViewChoiceRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

void wxDataViewChoiceRenderer::SetAlignment( int align )
{

}

#endif // !wxHAS_GENERIC_DATAVIEWCTRL

#endif // wxUSE_DATAVIEWCTRL
