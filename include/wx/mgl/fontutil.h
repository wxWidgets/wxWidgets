/////////////////////////////////////////////////////////////////////////////
// Name:        wx/nix/fontutil.h
// Purpose:     font-related helper functions for MGL
// Author:      Vaclav Slavik
// Created:     2001/05/01
// RCS-ID:      $Id$
// Copyright:   (c) 2001, Vaclav Slavik
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MGL_FONTUTIL_H_
#define _WX_MGL_FONTUTIL_H_

#ifdef __WXMGL__

struct font_info_t;

// structure representing native MGL font family
class wxMGLFontFamily : public wxObject
{
public:
    wxMGLFontFamily(font_info_t *info);
    virtual ~wxMGLFontFamily();
    
    wxString GetName() const { return m_name; }
    font_info_t *GetInfo() const { return m_fontInfo; }

private:
    wxString     m_name;
    font_info_t *m_fontInfo;
};

class wxMGLFontsDB
{
    public:
        wxMGLFontsDB();
        ~wxMGLFontsDB();
    
        void AddFontFamily(font_info_t *info);
        
        size_t GetFontsCount() const { return m_count; }
        // return info about font with given name:
        wxMGLFontFamily *GetFontFamily(const wxString& name) const;
        // return info about n-th font in the db:
        wxMGLFontFamily *GetFontFamily(size_t n) const;

    private:
        size_t m_count;
        wxHashTable *m_hash;
};

extern wxMGLFontsDB *wxTheMGLFontsDB;

#endif

#endif // _WX_MGL_FONTUTIL_H_
