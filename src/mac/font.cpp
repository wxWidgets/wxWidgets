/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "font.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/font.h"
#include "wx/gdicmn.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)
#endif

wxFontRefData::wxFontRefData()
{
	m_style = 0;
  	m_pointSize = 0;
  	m_family = 0;
  	m_style = 0;
  	m_weight = 0;
  	m_underlined = 0;
  	m_faceName = "";

  	m_macFontSize = m_pointSize ; 
  	m_macFontNum = systemFont ;
  	m_macFontStyle = normal ;
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

  	m_macFontSize = m_pointSize ; 
  	m_macFontNum = systemFont ;
  	m_macFontStyle = normal ;
}

wxFontRefData::~wxFontRefData()
{
    // TODO: delete font data
}

void wxFontRefData::MacFindFont()
{
	if( m_faceName == "" )
	{
		switch( m_family )
		{
			case wxDEFAULT :
				m_macFontNum = ::GetAppFont() ;
				break ;
			case wxDECORATIVE :
				::GetFNum( "\pTimes" , &m_macFontNum) ;
				break ;
			case wxROMAN :
				::GetFNum( "\pTimes" , &m_macFontNum) ;
				break ;
			case wxSCRIPT :
				::GetFNum( "\pTimes" , &m_macFontNum) ;
				break ;
			case wxSWISS :
				::GetFNum( "\pHelvetica" , &m_macFontNum) ;
				break ;
			case wxMODERN :
				::GetFNum( "\pMonaco" , &m_macFontNum) ;
				break ;
		}
	}
	else
	{
		if ( m_faceName == "systemfont" )
			m_macFontNum = ::GetSysFont() ;
		else if ( m_faceName == "applicationfont" )
			m_macFontNum = ::GetAppFont() ;
		else
		{
			strcpy(wxBuffer, m_faceName);
			C2PStr(wxBuffer);
			::GetFNum( (unsigned char*) wxBuffer, &m_macFontNum);
		}
	}

	m_macFontStyle = 0;
	if (m_weight == wxBOLD)
		 m_macFontStyle |= bold;
	if (m_style == wxITALIC || m_style == wxSLANT) 
		m_macFontStyle |= italic;
	if (m_underlined) 
		m_macFontStyle |= underline;
	m_macFontSize = m_pointSize ;
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
	M_FONTDATA->MacFindFont() ;
    return TRUE;
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

