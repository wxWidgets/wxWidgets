/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
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
    wxFontRefData(void);
    ~wxFontRefData(void);
protected:
  bool          m_temporary;   // If TRUE, the pointer to the actual font
                               // is temporary and SHOULD NOT BE DELETED by
                               // destructor
  int           m_pointSize;
  int           m_family;
  int           m_fontId;
  int           m_style;
  int           m_weight;
  bool          m_underlined;
  wxString      m_faceName;
  WXHFONT       m_hFont;

};

#define M_FONTDATA ((wxFontRefData *)m_refData)

WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// Font
class WXDLLEXPORT wxFont: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxFont)
public:
  wxFont(void);
  wxFont(int PointSize, int Family, int Style, int Weight, bool underlined = FALSE, const wxString& Face = wxEmptyString);
  inline wxFont(const wxFont& font) { Ref(font); }
  inline wxFont(const wxFont* font) { /* UnRef(); */ if (font) Ref(*font); }

  ~wxFont(void);

  bool Create(int PointSize, int Family, int Style, int Weight, bool underlined = FALSE, const wxString& Face = wxEmptyString);

  // Internal
  virtual bool RealizeResource(void);
  virtual WXHANDLE GetResourceHandle(void) ;
  virtual bool FreeResource(bool force = FALSE);
/*
  virtual bool UseResource(void);
  virtual bool ReleaseResource(void);
*/

  virtual bool IsFree(void);
  virtual bool Ok(void) const { return (m_refData != NULL) ; }

  inline int GetPointSize(void) const { return M_FONTDATA->m_pointSize; }
  inline int GetFamily(void) const { return M_FONTDATA->m_family; }
  inline int GetFontId(void) const { return M_FONTDATA->m_fontId; } /* New font system */
  inline int GetStyle(void) const { return M_FONTDATA->m_style; }
  inline int GetWeight(void) const { return M_FONTDATA->m_weight; }
  wxString GetFamilyString(void) const ;
  wxString GetFaceName(void) const ;
  wxString GetStyleString(void) const ;
  wxString GetWeightString(void) const ;
  inline bool GetUnderlined(void) const { return M_FONTDATA->m_underlined; }

  void SetPointSize(int pointSize);
  void SetFamily(int family);
  void SetStyle(int style);
  void SetWeight(int weight);
  void SetFaceName(const wxString& faceName);
  void SetUnderlined(bool underlined);

  inline wxFont& operator = (const wxFont& font) { if (*this == font) return (*this); Ref(font); return *this; }
  inline bool operator == (const wxFont& font) { return m_refData == font.m_refData; }
  inline bool operator != (const wxFont& font) { return m_refData != font.m_refData; }
};

#endif
    // _WX_FONT_H_
