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
#include "wx/list.h"

class WXDLLEXPORT wxFont;

// For every wxFont, there must be a font for each display and scale requested.
// So these objects are stored in wxFontRefData::m_fonts
class WXDLLEXPORT wxXFont: public wxObject
{
public:
    wxXFont();
    ~wxXFont();

    WXFontStructPtr     m_fontStruct;   // XFontStruct
    WXFontList          m_fontList;     // Motif XmFontList
    WXDisplay*          m_display;      // XDisplay
    int                 m_scale;        // Scale * 100
};

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

  // A list of wxXFonts
  wxList        m_fonts;
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
  inline bool operator == (const wxFont& font) const { return m_refData == font.m_refData; }
  inline bool operator != (const wxFont& font) const { return m_refData != font.m_refData; }

// Implementation

  // Find an existing, or create a new, XFontStruct
  // based on this wxFont and the given scale. Append the
  // font to list in the private data for future reference.

  // TODO This is a fairly basic implementation, that doesn't
  // allow for different facenames, and also doesn't do a mapping
  // between 'standard' facenames (e.g. Arial, Helvetica, Times Roman etc.)
  // and the fonts that are available on a particular system.
  // Maybe we need to scan the user's machine to build up a profile
  // of the fonts and a mapping file.

  // Return font struct, and optionally the Motif font list
  wxXFont* GetInternalFont(double scale = 1.0, WXDisplay* display = NULL) const;

  // These two are helper functions for convenient access of the above.
  inline WXFontStructPtr GetFontStruct(double scale = 1.0, WXDisplay* display = NULL) const
  {
    wxXFont* f = GetInternalFont(scale, display);
    return (f ? f->m_fontStruct : (WXFontStructPtr) 0);
  }
  WXFontList GetFontList(double scale = 1.0, WXDisplay* display = NULL) const
  {
    wxXFont* f = GetInternalFont(scale, display);
    return (f ? f->m_fontList : (WXFontList) 0);
  }

  WXFontStructPtr LoadQueryFont(int pointSize, int family, int style,
   int weight, bool underlined) const;
protected:
  bool RealizeResource();
  void Unshare();
};

#endif
    // _WX_FONT_H_
