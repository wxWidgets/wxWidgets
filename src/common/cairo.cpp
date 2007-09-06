/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/cairo.cpp
// Purpose:     Cairo library
// Author:      Anthony Betaudeau
// Created:     2007-08-25
// RCS-ID:      $Id: cairo.cpp 44625 2007-08-25 11:35:04Z VZ $
// Copyright:   (c) Anthony Bretaudeau
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/cairo.h"

#if wxUSE_CAIRO

#ifndef WX_PRECOMP
    #include "wx/module.h"
    #include "wx/log.h"
#endif


wxCairoLibrary *wxCairoLibrary::s_lib = NULL;

//----------------------------------------------------------------------------
// wxCairoLibrary
//----------------------------------------------------------------------------

wxCairoLibrary::wxCairoLibrary()
{
    m_cairo_lib = NULL;

    wxLogNull log;
    m_cairo_lib = new wxDynamicLibrary( wxT("libcairo.so") );
    m_ok = m_cairo_lib->IsLoaded();
    if (!m_ok) return;

    InitializeMethods();
}

wxCairoLibrary::~wxCairoLibrary()
{
    if (m_cairo_lib)
        delete m_cairo_lib;
}

/* static */ wxCairoLibrary* wxCairoLibrary::Get()
{
    if (s_lib)
        return s_lib;
        
    s_lib = new wxCairoLibrary();
    if (s_lib->IsOk())
        return s_lib;
        
    delete s_lib;
    s_lib = NULL;
    
    return NULL;
}

/* static */ void wxCairoLibrary::CleanUp()
{
    if (s_lib)
    {
        delete s_lib;
        s_lib = NULL;
    }
}

bool wxCairoLibrary::IsOk()
{
    return m_ok;
}

void wxCairoLibrary::InitializeMethods()
{
    m_ok = false;
    bool success;

    wxDL_METHOD_LOAD( m_cairo_lib, cairo_arc, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_arc_negative, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_clip, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_close_path, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_create, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_curve_to, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_destroy, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_fill, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_fill_preserve, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_get_target, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_image_surface_create_for_data, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_line_to, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_move_to, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_new_path, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_paint, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_pattern_add_color_stop_rgba, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_pattern_create_for_surface, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_pattern_create_linear, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_pattern_create_radial, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_pattern_destroy, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_pattern_set_extend, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_pattern_set_filter, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_rectangle, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_reset_clip, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_restore, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_rotate, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_save, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_scale, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_set_dash, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_set_fill_rule, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_set_line_cap, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_set_line_join, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_set_line_width, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_set_operator, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_set_source, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_set_source_rgba, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_stroke, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_stroke_preserve, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_surface_create_similar, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_surface_destroy, success )
    wxDL_METHOD_LOAD( m_cairo_lib, cairo_translate, success )

    m_ok = true;
}

//----------------------------------------------------------------------------
// wxCairoModule
//----------------------------------------------------------------------------

class wxCairoModule: public wxModule
{
public:
    wxCairoModule() { }
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxGtkPrintModule)
};

bool wxCairoModule::OnInit()
{
    return true;
}

void wxCairoModule::OnExit()
{
    wxCairoLibrary::CleanUp();
}

IMPLEMENT_DYNAMIC_CLASS(wxCairoModule, wxModule)

#endif
  // wxUSE_CAIRO
