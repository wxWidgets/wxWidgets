/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "font.h"
#endif

#include "wx/font.h"
#include "wx/utils.h"
#include <strings.h>

//-----------------------------------------------------------------------------
// local data
//-----------------------------------------------------------------------------

static char *wx_font_family [] = {
    "wxDEFAULT", "wxDECORATIVE", "wxMODERN", "wxROMAN", "wxSCRIPT",
    "wxSWISS", "wxTELETYPE",
};
static char *wx_font_style [] = {
    "wxDEFAULT", "wxNORMAL", "wxSLANT", "wxITALIC",
};
static char *wx_font_weight [] = {
    "wxDEFAULT", "wxNORMAL", "wxBOLD", "wxLIGHT",
};

extern wxFontNameDirectory wxTheFontNameDirectory;

//-----------------------------------------------------------------------------
// wxFont
//-----------------------------------------------------------------------------

class wxFontRefData: public wxObjectRefData
{
  public:
  
    wxFontRefData(void);
    ~wxFontRefData(void);
 
    wxList   m_scaled_xfonts;
    int      m_pointSize;
    int      m_family, m_style, m_weight;
    bool     m_underlined;
    int      m_fontId;
    char*    m_faceName;
     
};

wxFontRefData::wxFontRefData(void) : m_scaled_xfonts(wxKEY_INTEGER)
{
  m_pointSize = -1;
  m_family = -1;
  m_style = -1;
  m_weight = -1;
  m_underlined = FALSE;
  m_fontId = 0;
  m_faceName = NULL;
};

wxFontRefData::~wxFontRefData(void)
{
  wxNode *node = m_scaled_xfonts.First();
  while (node) 
  {
    wxNode *next = node->Next();
    node = next;
  };
  if (m_faceName) 
  {
    delete m_faceName;
    m_faceName = NULL;
  };
};

//-----------------------------------------------------------------------------

#define M_FONTDATA ((wxFontRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

wxFont::wxFont(void)
{
  if (wxTheFontList) wxTheFontList->Append( this );
};

wxFont::wxFont( char *xFontName )
{
  if (!xFontName) return;
  
  m_refData = new wxFontRefData();
  
};

wxFont::wxFont(int PointSize, int FontIdOrFamily, int Style, int Weight,
	       bool Underlined, const char* Face)
{
  m_refData = new wxFontRefData();
  

  if (wxTheFontList) wxTheFontList->Append( this );
};

wxFont::wxFont(int PointSize, const char *Face, int Family, int Style, 
	       int Weight, bool Underlined)
{
  m_refData = new wxFontRefData();


  if (wxTheFontList) wxTheFontList->Append( this );
};

wxFont::wxFont( const wxFont& font )
{ 
  Ref( font ); 
};

wxFont::wxFont( const wxFont* font ) 
{ 
  UnRef(); 
  if (font) Ref( *font ); 
};

wxFont::~wxFont(void)
{
  if (wxTheFontList) wxTheFontList->DeleteObject( this );
};

wxFont& wxFont::operator = ( const wxFont& font ) 
{ 
  if (*this == font) return (*this); 
  Ref( font ); 
  return *this; 
};

bool wxFont::operator == ( const wxFont& font ) 
{ 
  return m_refData == font.m_refData; 
};

bool wxFont::operator != ( const wxFont& font ) 
{ 
  return m_refData != font.m_refData; 
};

bool wxFont::Ok()
{
  return (m_refData != NULL);
};

int wxFont::GetPointSize(void) const
{
  return M_FONTDATA->m_pointSize;
};

wxString wxFont::GetFaceString(void) const
{
  return "";
};

wxString wxFont::GetFaceName(void) const
{
  return ""; 
};

int wxFont::GetFamily(void) const
{
  return M_FONTDATA->m_family;
};

wxString wxFont::GetFamilyString(void) const
{
  wxString s = wx_font_family[M_FONTDATA->m_family];
  return s;
};

int wxFont::GetFontId(void) const
{
  return M_FONTDATA->m_fontId; // stub
};

int wxFont::GetStyle(void) const
{
  return M_FONTDATA->m_style;
};

wxString wxFont::GetStyleString(void) const
{
  wxString s =  wx_font_style[M_FONTDATA->m_style];
  return s;
};

int wxFont::GetWeight(void) const
{
  return M_FONTDATA->m_weight;
};

wxString wxFont::GetWeightString(void) const
{
  wxString s = wx_font_weight[M_FONTDATA->m_weight];
  return s;
};

bool wxFont::GetUnderlined(void) const
{
  return M_FONTDATA->m_underlined;
};
