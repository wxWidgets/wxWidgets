/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mgl/fontutil.h
// Purpose:     font-related helper functions for MGL
// Author:      Vaclav Slavik
// Created:     2001/05/01
// RCS-ID:      $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MGL_FONTUTIL_H_
#define _WX_MGL_FONTUTIL_H_

#include "wx/list.h"

struct font_info_t;
struct font_lib_t;
struct font_t;

class wxMGLFontInstance;
class wxMGLFontInstanceList;
class wxMGLFontLibrary;
class wxMGLFontFamily;

enum
{
    wxFONTFACE_REGULAR     = 0,
    wxFONTFACE_ITALIC      = 1,
    wxFONTFACE_BOLD        = 2, // = (regular | bold)
    wxFONTFACE_BOLD_ITALIC = 3, // = (italic | bold)
    
    wxFONTFACE_MAX
};

// structure representing particular loaded font instance:
class wxMGLFontInstance
{
public:
    wxMGLFontInstance(wxMGLFontLibrary *fontLib, float pt, bool slant, bool aa);
    ~wxMGLFontInstance();

    struct font_t *GetMGLfont_t() const { return m_font; }

    float GetPt() const { return m_pt; }
    bool GetSlant() const { return m_slant; }
    bool GetAA() const { return m_aa; }
    
private:
    wxMGLFontLibrary *m_fontLib;
    font_t *m_font;
    float m_pt;
    bool m_slant;
    bool m_aa;    
};

// structure representing loaded font library:
class wxMGLFontLibrary
{
public:
    wxMGLFontLibrary(const wxString& filename, int type, wxMGLFontFamily *parentFamily);
    ~wxMGLFontLibrary();
    
    wxMGLFontInstance *GetFontInstance(wxFont *font, float scale, bool aa);
    wxMGLFontFamily *GetFamily() const { return m_family; }
    
    void IncRef();
    void DecRef();
    
    struct font_lib_t *GetMGLfont_lib_t() const { return m_fontLib; }
    
private:
    font_lib_t *m_fontLib;
    int m_type;
    wxString m_fileName;
    size_t m_refs;
    wxMGLFontInstanceList *m_instances;
    wxMGLFontFamily *m_family;
};

// structure representing native MGL font family
class wxMGLFontFamily : public wxObject
{
public:
    wxMGLFontFamily(const font_info_t *info);
    virtual ~wxMGLFontFamily();

    wxString GetName() const { return m_name; }
    const font_info_t *GetInfo() const { return m_fontInfo; }

    bool HasFace(int type) const;
    wxMGLFontLibrary *GetLibrary(int type) const 
            { return m_fontLibs[type]; }

private:
    wxString m_name;
    const font_info_t *m_fontInfo;
    wxMGLFontLibrary *m_fontLibs[wxFONTFACE_MAX];
};

WX_DECLARE_LIST(wxMGLFontFamily, wxMGLFontFamilyList);

class wxFontsManager
{
    public:
        wxFontsManager();
        ~wxFontsManager();
    
        void AddFamily(const font_info_t *info);
        
        // return info about font with given name:
        wxMGLFontFamily *GetFamily(const wxString& name) const;
        // return list of all families
        wxMGLFontFamilyList *GetFamilyList() { return m_list; }

        wxMGLFontLibrary *GetFontLibrary(wxFont *font);

    private:
        wxHashTable *m_hash;
        wxMGLFontFamilyList *m_list;
};

extern wxFontsManager *wxTheFontsManager;

#endif // _WX_MGL_FONTUTIL_H_
