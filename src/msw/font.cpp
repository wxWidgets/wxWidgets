/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "font.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/font.h"
#endif

#include "wx/msw/private.h"
#include "assert.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

#if USE_PORTABLE_FONTS_IN_MSW
IMPLEMENT_DYNAMIC_CLASS(wxFontNameDirectory, wxObject)
#endif

#endif

wxFontRefData::wxFontRefData(void)
{
	m_style = 0;
  	m_temporary = FALSE;
  	m_pointSize = 0;
  	m_family = 0;
  	m_fontId = 0;
  	m_style = 0;
  	m_weight = 0;
  	m_underlined = 0;
  	m_faceName = "";
  	m_hFont = 0;
}

wxFontRefData::~wxFontRefData(void)
{
	if ( m_hFont )
    	::DeleteObject((HFONT) m_hFont);
}

wxFont::wxFont(void)
{
    if ( wxTheFontList )
        wxTheFontList->Append(this);
}

/* Constructor for a font. Note that the real construction is done
 * in wxDC::SetFont, when information is available about scaling etc.
 */
wxFont::wxFont(int PointSize, int Family, int Style, int Weight, bool Underlined, const wxString& Face)
{
  Create(PointSize, Family, Style, Weight, Underlined, Face);

    if ( wxTheFontList )
        wxTheFontList->Append(this);
}

bool wxFont::Create(int PointSize, int Family, int Style, int Weight, bool Underlined, const wxString& Face)
{
  UnRef();
  m_refData = new wxFontRefData;

  M_FONTDATA->m_family = Family;
  M_FONTDATA->m_style = Style;
  M_FONTDATA->m_weight = Weight;
  M_FONTDATA->m_pointSize = PointSize;
  M_FONTDATA->m_underlined = Underlined;
  M_FONTDATA->m_faceName = Face;
  M_FONTDATA->m_temporary = FALSE;
  M_FONTDATA->m_hFont = 0;

  RealizeResource();

  return TRUE;
}

wxFont::~wxFont()
{
  if (wxTheFontList)
    wxTheFontList->DeleteObject(this);
}

bool wxFont::RealizeResource(void)
{
  if (M_FONTDATA && !M_FONTDATA->m_hFont)
  {
    BYTE ff_italic;
    int ff_weight = 0;
    int ff_family = 0;
    wxString ff_face("");

    switch (M_FONTDATA->m_family)
    {
      case wxSCRIPT:     ff_family = FF_SCRIPT ;
                         ff_face = "Script" ;
                         break ;
      case wxDECORATIVE: ff_family = FF_DECORATIVE;
                         break;
      case wxROMAN:      ff_family = FF_ROMAN;
                         ff_face = "Times New Roman" ;
                         break;
      case wxTELETYPE:
      case wxMODERN:     ff_family = FF_MODERN;
                         ff_face = "Courier New" ;
                         break;
      case wxSWISS:      ff_family = FF_SWISS;
                         ff_face = "Arial";
                         break;
      case wxDEFAULT:
      default:           ff_family = FF_SWISS;
                         ff_face = "Arial" ; 
    }

    if (M_FONTDATA->m_style == wxITALIC || M_FONTDATA->m_style == wxSLANT)
      ff_italic = 1;
    else
      ff_italic = 0;

    if (M_FONTDATA->m_weight == wxNORMAL)
      ff_weight = FW_NORMAL;
    else if (M_FONTDATA->m_weight == wxLIGHT)
      ff_weight = FW_LIGHT;
    else if (M_FONTDATA->m_weight == wxBOLD)
      ff_weight = FW_BOLD;

#if defined(__X__) || (defined(__WINDOWS__) && USE_PORTABLE_FONTS_IN_MSW)
    ff_face = wxTheFontNameDirectory.GetScreenName(M_FONTDATA->m_family, M_FONTDATA->m_weight, M_FONTDATA->m_style);
#else
    ff_face = M_FONTDATA->m_faceName;
	if ( ff_face.IsNull() )
		ff_face = "";
#endif

/* Always calculate fonts using the screen DC (is this the best strategy?)
 * There may be confusion if a font is selected into a printer
 * DC (say), because the height will be calculated very differently.
    // What sort of display is it?
    int technology = ::GetDeviceCaps(dc, TECHNOLOGY);

    int nHeight;
    
    if (technology != DT_RASDISPLAY && technology != DT_RASPRINTER)
    {
      // Have to get screen DC Caps, because a metafile will return 0.
      HDC dc2 = ::GetDC(NULL);
      nHeight = M_FONTDATA->m_pointSize*GetDeviceCaps(dc2, LOGPIXELSY)/72;
      ::ReleaseDC(NULL, dc2);
    }
    else
    {
      nHeight = M_FONTDATA->m_pointSize*GetDeviceCaps(dc, LOGPIXELSY)/72;
    }
*/
    // Have to get screen DC Caps, because a metafile will return 0.
    HDC dc2 = ::GetDC(NULL);
    int ppInch = ::GetDeviceCaps(dc2, LOGPIXELSY);
    ::ReleaseDC(NULL, dc2);

    // New behaviour: apparently ppInch varies according to
    // Large/Small Fonts setting in Windows. This messes
    // up fonts. So, set ppInch to a constant 96 dpi.
    ppInch = 96;
    
#if FONT_SIZE_COMPATIBILITY
    // Incorrect, but compatible with old wxWindows behaviour
    int nHeight = (M_FONTDATA->m_pointSize*ppInch/72);
#else
    // Correct for Windows compatibility
    int nHeight = - (M_FONTDATA->m_pointSize*ppInch/72);
#endif

    bool ff_underline = M_FONTDATA->m_underlined;

    M_FONTDATA->m_hFont = (WXHFONT) CreateFont(nHeight, 0, 0, 0,ff_weight,ff_italic,(BYTE)ff_underline,
                0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                PROOF_QUALITY, DEFAULT_PITCH | ff_family, (ff_face == "" ? NULL : (const char *)ff_face));
#ifdef DEBUG_CREATE
    if (m_hFont==NULL) wxError("Cannot create font","Internal Error") ;
#endif
    return (M_FONTDATA->m_hFont != (WXHFONT) NULL);
  }
  return FALSE;
}

bool wxFont::FreeResource(bool force)
{
  if (M_FONTDATA && M_FONTDATA->m_hFont)
  {
    ::DeleteObject((HFONT) M_FONTDATA->m_hFont);
    M_FONTDATA->m_hFont = 0;
    return TRUE;
  }
  return FALSE;
}

/*
bool wxFont::UseResource(void)
{
  IncrementResourceUsage();
  return TRUE;
}

bool wxFont::ReleaseResource(void)
{
  DecrementResourceUsage();
  return TRUE;
}
*/
  
WXHANDLE wxFont::GetResourceHandle(void)
{
  if ( !M_FONTDATA )
	return 0;
  else
    return (WXHANDLE)M_FONTDATA->m_hFont ;
}

bool wxFont::IsFree(void)
{
  return (M_FONTDATA && (M_FONTDATA->m_hFont == 0));
}

void wxFont::SetPointSize(const int pointSize)
{
    if ( !m_refData )
        m_refData = new wxFontRefData;
    M_FONTDATA->m_pointSize = pointSize;
}

void wxFont::SetFamily(const int family)
{
    if ( !m_refData )
        m_refData = new wxFontRefData;
    M_FONTDATA->m_family = family;
}

void wxFont::SetStyle(const int style)
{
    if ( !m_refData )
        m_refData = new wxFontRefData;
    M_FONTDATA->m_style = style;
}

void wxFont::SetWeight(const int weight)
{
    if ( !m_refData )
        m_refData = new wxFontRefData;
    M_FONTDATA->m_weight = weight;
}

void wxFont::SetFaceName(const wxString& faceName)
{
    if ( !m_refData )
        m_refData = new wxFontRefData;
    M_FONTDATA->m_faceName = faceName;
}

void wxFont::SetUnderlined(const bool underlined)
{
    if ( !m_refData )
        m_refData = new wxFontRefData;
    M_FONTDATA->m_underlined = underlined;
}

wxString wxFont::GetFamilyString(void) const
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
wxString wxFont::GetFaceName(void) const
{
  wxString str("");
  if (M_FONTDATA)
	str = M_FONTDATA->m_faceName ;
  return str;
}

wxString wxFont::GetStyleString(void) const
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

wxString wxFont::GetWeightString(void) const
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

