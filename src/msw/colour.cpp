/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:     wxColour class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "colour.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/gdicmn.h"
#include "wx/msw/private.h"

#include <string.h>

#if wxUSE_EXTENDED_RTTI

template<> void wxStringReadValue(const wxString &s , wxColour &data )
{
    // copied from VS xrc
    unsigned long tmp = 0;

    if (s.Length() != 7 || s[0u] != wxT('#')
        || wxSscanf(s.c_str(), wxT("#%lX"), &tmp) != 1)
    {
        wxLogError(_("String To Colour : Incorrect colour specification : %s"),
            s.c_str() );
        data = wxNullColour;
    }
    else
    {
        data = wxColour((unsigned char) ((tmp & 0xFF0000) >> 16) ,
            (unsigned char) ((tmp & 0x00FF00) >> 8),
            (unsigned char) ((tmp & 0x0000FF)));
    }
}

template<> void wxStringWriteValue(wxString &s , const wxColour &data )
{
    s = wxString::Format(wxT("#%02X%02X%02X"),
        data.Red(), data.Green(), data.Blue() );
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

void wxColour::InitFromName(const wxString& name)
{
    if ( wxTheColourDatabase )
    {
        wxColour col = wxTheColourDatabase->Find(name);
        if ( col.Ok() )
        {
            *this = col;
            return;
        }
    }

    // leave invalid
    Init();
}

wxColour::~wxColour()
{
}

void wxColour::Set(unsigned char r, unsigned char g, unsigned char b)
{
    m_red = r;
    m_green = g;
    m_blue = b;
    m_isInit = true;
    m_pixel = PALETTERGB(m_red, m_green, m_blue);
}

