/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "font.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/font.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"

#include <Xm/Xm.h>

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)
#endif

wxXFont::wxXFont()
{
    m_fontStruct = (WXFontStructPtr) 0;
    m_fontList = (WXFontList) 0;
    m_display = (WXDisplay*) 0;
    m_scale = 100;
}

wxXFont::~wxXFont()
{
    XFontStruct* fontStruct = (XFontStruct*) m_fontStruct;
    XmFontList fontList = (XmFontList) m_fontList;
    
    XmFontListFree (fontList);
    
    // TODO: why does freeing the font produce a segv???
    // Note that XFreeFont wasn't called in wxWin 1.68 either.
    //        XFreeFont((Display*) m_display, fontStruct);
}

wxFontRefData::wxFontRefData()
{
    m_style = 0;
    m_pointSize = 0;
    m_family = 0;
    m_style = 0;
    m_weight = 0;
    m_underlined = 0;
    m_faceName = "";
}

wxFontRefData::wxFontRefData(const wxFontRefData& data)
{
    m_style = data.m_style;
    m_pointSize = data.m_pointSize;
    m_family = data.m_family;
    m_style = data.m_style;
    m_weight = data.m_weight;
    m_underlined = data.m_underlined;
    m_faceName = data.m_faceName;
    
    // Don't have to copy actual fonts, because they'll be created
    // on demand.
}

wxFontRefData::~wxFontRefData()
{
    wxNode* node = m_fonts.First();
    while (node)
    {
        wxXFont* f = (wxXFont*) node->Data();
        delete f;
        node = node->Next();
    }
    m_fonts.Clear();
}

wxFont::wxFont()
{
    if ( wxTheFontList )
        wxTheFontList->Append(this);
}

wxFont::wxFont(int pointSize, int family, int style, int weight, bool underlined, const wxString& faceName)
{
    Create(pointSize, family, style, weight, underlined, faceName);
    
    if ( wxTheFontList )
        wxTheFontList->Append(this);
}

bool wxFont::Create(int pointSize, int family, int style, int weight, bool underlined, const wxString& faceName)
{
    UnRef();
    m_refData = new wxFontRefData;
    
    M_FONTDATA->m_family = family;
    M_FONTDATA->m_style = style;
    M_FONTDATA->m_weight = weight;
    M_FONTDATA->m_pointSize = pointSize;
    M_FONTDATA->m_underlined = underlined;
    M_FONTDATA->m_faceName = faceName;
    
    RealizeResource();
    
    return TRUE;
}

wxFont::~wxFont()
{
    if (wxTheFontList)
        wxTheFontList->DeleteObject(this);
}

bool wxFont::RealizeResource()
{
    // TODO: create the font (if there is a native font object)
    return FALSE;
}

void wxFont::Unshare()
{
    // Don't change shared data
    if (!m_refData)
    {
        m_refData = new wxFontRefData();
    }
    else
    {
        wxFontRefData* ref = new wxFontRefData(*(wxFontRefData*)m_refData);
        UnRef();
        m_refData = ref;
    }
}

void wxFont::SetPointSize(int pointSize)
{
    Unshare();
    
    M_FONTDATA->m_pointSize = pointSize;
    
    RealizeResource();
}

void wxFont::SetFamily(int family)
{
    Unshare();
    
    M_FONTDATA->m_family = family;
    
    RealizeResource();
}

void wxFont::SetStyle(int style)
{
    Unshare();
    
    M_FONTDATA->m_style = style;
    
    RealizeResource();
}

void wxFont::SetWeight(int weight)
{
    Unshare();
    
    M_FONTDATA->m_weight = weight;
    
    RealizeResource();
}

void wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();
    
    M_FONTDATA->m_faceName = faceName;
    
    RealizeResource();
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();
    
    M_FONTDATA->m_underlined = underlined;
    
    RealizeResource();
}

wxString wxFont::GetFamilyString() const
{
    wxString fam("");
    switch (GetFamily())
    {
    case wxDECORATIVE:
        fam = "wxDECORATIVE";
        break;
    case wxROMAN:
        fam = "wxROMAN";
        break;
    case wxSCRIPT:
        fam = "wxSCRIPT";
        break;
    case wxSWISS:
        fam = "wxSWISS";
        break;
    case wxMODERN:
        fam = "wxMODERN";
        break;
    case wxTELETYPE:
        fam = "wxTELETYPE";
        break;
    default:
        fam = "wxDEFAULT";
        break;
    }
    return fam;
}

/* New font system */
wxString wxFont::GetFaceName() const
{
    wxString str("");
    if (M_FONTDATA)
        str = M_FONTDATA->m_faceName ;
    return str;
}

wxString wxFont::GetStyleString() const
{
    wxString styl("");
    switch (GetStyle())
    {
    case wxITALIC:
        styl = "wxITALIC";
        break;
    case wxSLANT:
        styl = "wxSLANT";
        break;
    default:
        styl = "wxNORMAL";
        break;
    }
    return styl;
}

wxString wxFont::GetWeightString() const
{
    wxString w("");
    switch (GetWeight())
    {
    case wxBOLD:
        w = "wxBOLD";
        break;
    case wxLIGHT:
        w = "wxLIGHT";
        break;
    default:
        w = "wxNORMAL";
        break;
    }
    return w;
}

// Find an existing, or create a new, XFontStruct
// based on this wxFont and the given scale. Append the
// font to list in the private data for future reference.
wxXFont* wxFont::GetInternalFont(double scale, WXDisplay* display) const
{
    if (!Ok())
        return (wxXFont*) NULL;
    
    long intScale = long(scale * 100.0 + 0.5); // key for wxXFont
    int pointSize = (M_FONTDATA->m_pointSize * 10 * intScale) / 100;
    
    wxNode* node = M_FONTDATA->m_fonts.First();
    while (node)
    {
        wxXFont* f = (wxXFont*) node->Data();
        if ((!display || (f->m_display == display)) && (f->m_scale == intScale))
            return f;
        node = node->Next();
    }
    
    WXFontStructPtr font = LoadQueryFont(pointSize, M_FONTDATA->m_family,
        M_FONTDATA->m_style, M_FONTDATA->m_weight, M_FONTDATA->m_underlined);
    
    if (!font)
    {
        // search up and down by stepsize 10
        int max_size = pointSize + 20 * (1 + (pointSize/180));
        int min_size = pointSize - 20 * (1 + (pointSize/180));
        int i;
        
        // Search for smaller size (approx.)
        for (i=pointSize-10; !font && i >= 10 && i >= min_size; i -= 10)
            font = LoadQueryFont(i, M_FONTDATA->m_family, M_FONTDATA->m_style, M_FONTDATA->m_weight, M_FONTDATA->m_underlined);
        // Search for larger size (approx.)
        for (i=pointSize+10; !font && i <= max_size; i += 10)
            font = LoadQueryFont(i, M_FONTDATA->m_family, M_FONTDATA->m_style, M_FONTDATA->m_weight, M_FONTDATA->m_underlined);
        // Try default family
        if (!font && M_FONTDATA->m_family != wxDEFAULT)
            font = LoadQueryFont(pointSize, wxDEFAULT, M_FONTDATA->m_style, 
            M_FONTDATA->m_weight, M_FONTDATA->m_underlined);
        // Bogus font
        if (!font)
            font = LoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL,
            M_FONTDATA->m_underlined);
        wxASSERT_MSG( (font != (XFontStruct*) NULL), "Could not allocate even a default font -- something is wrong." );
    }
    if (font)
    {
        wxXFont* f = new wxXFont;
        f->m_fontStruct = font;
        f->m_display = ( display ? display : wxGetDisplay() );
        f->m_scale = intScale;
        f->m_fontList = XmFontListCreate ((XFontStruct*) font, XmSTRING_DEFAULT_CHARSET);
        M_FONTDATA->m_fonts.Append(f);
        return f;
    }
    return (wxXFont*) NULL;
}

WXFontStructPtr wxFont::LoadQueryFont(int pointSize, int family, int style,
                                      int weight, bool underlined) const
{
    char *xfamily;
    char *xstyle;
    char *xweight;
    switch (family)
    {
    case wxDECORATIVE: xfamily = "lucida";
        break;
    case wxROMAN:      xfamily = "times";
        break;
    case wxMODERN:     xfamily = "courier";
        break;
    case wxSWISS:      xfamily = "lucida";
        break;
    case wxDEFAULT:
    default:           xfamily = "*";
    }
    switch (style)
    {
    case wxITALIC:     xstyle = "i";
        break;
    case wxSLANT:      xstyle = "o";
        break;
    case wxNORMAL:     xstyle = "r";
        break;
    default:           xstyle = "*";
        break;
    }
    switch (weight)
    {
    case wxBOLD:       xweight = "bold";
        break;
    case wxLIGHT:
    case wxNORMAL:     xweight = "medium";
        break;
    default:           xweight = "*";
        break;
    }
    
    sprintf(wxBuffer, "-*-%s-%s-%s-normal-*-*-%d-*-*-*-*-*-*",
        xfamily, xweight, xstyle, pointSize);
    
    Display *dpy = (Display*) wxGetDisplay();
    XFontStruct* font = XLoadQueryFont(dpy, wxBuffer);
    
    return (WXFontStructPtr) font;
}
