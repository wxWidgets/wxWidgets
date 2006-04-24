/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/colour.cpp
// Purpose:     wxColour class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/colour.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/gdicmn.h"

#include <string.h>

#if wxUSE_EXTENDED_RTTI

template<> void wxStringReadValue(const wxString &s , wxColour &data )
{
    if ( !data.Set(s) )
    {
        wxLogError(_("String To Colour : Incorrect colour specification : %s"),
            s.c_str() );
        data = wxNullColour;
    }
}

template<> void wxStringWriteValue(wxString &s , const wxColour &data )
{
    s = data.GetAsString(wxC2S_HTML_SYNTAX);
}

wxTO_STRING_IMP( wxColour )
wxFROM_STRING_IMP( wxColour )

IMPLEMENT_DYNAMIC_CLASS_WITH_COPY_AND_STREAMERS_XTI( wxColour , wxObject , "wx/colour.h" ,  &wxTO_STRING( wxColour ) , &wxFROM_STRING( wxColour ))

wxBEGIN_PROPERTIES_TABLE(wxColour)
    wxREADONLY_PROPERTY( Red, unsigned char, Red, EMPTY_MACROVALUE , 0 /*flags*/, wxT("Helpstring"), wxT("group"))
    wxREADONLY_PROPERTY( Green, unsigned char, Green, EMPTY_MACROVALUE , 0 /*flags*/, wxT("Helpstring"), wxT("group"))
    wxREADONLY_PROPERTY( Blue, unsigned char, Blue, EMPTY_MACROVALUE , 0 /*flags*/, wxT("Helpstring"), wxT("group"))
wxEND_PROPERTIES_TABLE()

wxCONSTRUCTOR_3( wxColour, unsigned char, Red, unsigned char, Green, unsigned char, Blue )

wxBEGIN_HANDLERS_TABLE(wxColour)
wxEND_HANDLERS_TABLE()
#else
IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)
#endif

// Colour

void wxColour::Init()
{
    m_isInit = false;
    m_pixel = 0;
    m_red =
    m_blue =
    m_green = 0;
}

wxColour::wxColour(const wxColour& col)
{
    *this = col;
}

wxColour& wxColour::operator=(const wxColour& col)
{
    m_red = col.m_red;
    m_green = col.m_green;
    m_blue = col.m_blue;
    m_isInit = col.m_isInit;
    m_pixel = col.m_pixel;
    return *this;
}

wxColour::~wxColour()
{
}

void wxColour::InitWith(unsigned char r, unsigned char g, unsigned char b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_isInit = true;
}
