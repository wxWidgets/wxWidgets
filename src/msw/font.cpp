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
#include <assert.h>

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

#if wxUSE_PORTABLE_FONTS_IN_MSW
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

wxFontRefData::wxFontRefData(const wxFontRefData& data)
{
	m_style = data.m_style;
  	m_temporary = FALSE;
  	m_pointSize = data.m_pointSize;
  	m_family = data.m_family;
  	m_fontId = data.m_fontId;
  	m_style = data.m_style;
  	m_weight = data.m_weight;
  	m_underlined = data.m_underlined;
  	m_faceName = data.m_faceName;
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

    const char* pzFace = (const char*) ff_face;
    if (!M_FONTDATA->m_faceName.IsNull())
        pzFace = (const char*) M_FONTDATA->m_faceName ;

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
    
#if wxFONT_SIZE_COMPATIBILITY
    // Incorrect, but compatible with old wxWindows behaviour
    int nHeight = (M_FONTDATA->m_pointSize*ppInch/72);
#else
    // Correct for Windows compatibility
    int nHeight = - (M_FONTDATA->m_pointSize*ppInch/72);
#endif

    bool ff_underline = M_FONTDATA->m_underlined;

    M_FONTDATA->m_hFont = (WXHFONT) CreateFont(nHeight, 0, 0, 0,ff_weight,ff_italic,(BYTE)ff_underline,
                0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                PROOF_QUALITY, DEFAULT_PITCH | ff_family, pzFace);
#ifdef WXDEBUG_CREATE
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

WXHANDLE wxFont::GetResourceHandle()
{
  if ( !M_FONTDATA )
	return 0;
  else
    return (WXHANDLE)M_FONTDATA->m_hFont ;
}

bool wxFont::IsFree()
{
  return (M_FONTDATA && (M_FONTDATA->m_hFont == 0));
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

