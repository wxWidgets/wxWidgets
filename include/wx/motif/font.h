/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONT_H_
#define _WX_FONT_H_

#ifdef __GNUG__
#pragma interface "font.h"
#endif

#include "wx/gdiobj.h"

class WXDLLEXPORT wxFont;

class WXDLLEXPORT wxFontRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxFont;
public:
    wxFontRefData();
    wxFontRefData(const wxFontRefData& data);
    ~wxFontRefData();
protected:
  int           m_pointSize;
  int           m_family;
  int           m_style;
  int           m_weight;
  bool          m_underlined;
  wxString      m_faceName;
/* TODO: implementation
  WXHFONT       m_hFont;
*/
};

#define M_FONTDATA ((wxFontRefData *)m_refData)

WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// Font
class WXDLLEXPORT wxFont: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxFont)
public:
  wxFont();
  wxFont(int pointSize, int family, int style, int weight, bool underlined = FALSE, const wxString& faceName = wxEmptyString);
  inline wxFont(const wxFont& font) { Ref(font); }
  inline wxFont(const wxFont* font) { if (font) Ref(*font); }

  ~wxFont();

  bool Create(int pointSize, int family, int style, int weight, bool underlined = FALSE, const wxString& faceName = wxEmptyString);

  virtual bool Ok() const { return (m_refData != NULL) ; }

  inline int GetPointSize() const { return M_FONTDATA->m_pointSize; }
  inline int GetFamily() const { return M_FONTDATA->m_family; }
  inline int GetStyle() const { return M_FONTDATA->m_style; }
  inline int GetWeight() const { return M_FONTDATA->m_weight; }
  wxString GetFamilyString() const ;
  wxString GetFaceName() const ;
  wxString GetStyleString() const ;
  wxString GetWeightString() const ;
  inline bool GetUnderlined() const { return M_FONTDATA->m_underlined; }

  void SetPointSize(int pointSize);
  void SetFamily(int family);
  void SetStyle(int style);
  void SetWeight(int weight);
  void SetFaceName(const wxString& faceName);
  void SetUnderlined(bool underlined);

  inline wxFont& operator = (const wxFont& font) { if (*this == font) return (*this); Ref(font); return *this; }
  inline bool operator == (const wxFont& font) { return m_refData == font.m_refData; }
  inline bool operator != (const wxFont& font) { return m_refData != font.m_refData; }

  // Implementation
protected:
  bool RealizeResource();
  void Unshare();
};

#endif
    // _WX_FONT_H_
