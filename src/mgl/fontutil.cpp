/////////////////////////////////////////////////////////////////////////////
// Name:        mgl/fontutil.cpp
// Purpose:     Font helper functions for MGL
// Author:      Vaclav Slavik
// Created:     2001/04/29
// RCS-ID:      $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "fontutil.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/log.h"
#include "wx/fontutil.h"
#include "wx/encinfo.h"
#include "wx/fontmap.h"
#include "wx/tokenzr.h"
#include "wx/hash.h"

#include "wx/listimpl.cpp"
#include "wx/sysopt.h"
#include "wx/mgl/private.h"

#include <mgraph.h>

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNativeEncodingInfo
// ----------------------------------------------------------------------------

// convert to/from the string representation: format is
//      encoding[;facename]
bool wxNativeEncodingInfo::FromString(const wxString& s)
{
    wxStringTokenizer tokenizer(s, _T(";"));

    wxString encid = tokenizer.GetNextToken();
    long enc;
    if ( !encid.ToLong(&enc) )
        return FALSE;
    encoding = (wxFontEncoding)enc;

    // ok even if empty
    facename = tokenizer.GetNextToken();

    return TRUE;
}

wxString wxNativeEncodingInfo::ToString() const
{
    wxString s;
    s << (long)encoding;
    if ( !!facename )
    {
        s << _T(';') << facename;
    }

    return s;
}

// ----------------------------------------------------------------------------
// common functions
// ----------------------------------------------------------------------------

bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                             wxNativeEncodingInfo *info)
{
    wxCHECK_MSG( info, FALSE, _T("bad pointer in wxGetNativeFontEncoding") );

    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    switch ( encoding )
    {
        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_2:
        case wxFONTENCODING_ISO8859_3:
        case wxFONTENCODING_ISO8859_4:
        case wxFONTENCODING_ISO8859_5:
        case wxFONTENCODING_ISO8859_6:
        case wxFONTENCODING_ISO8859_7:
        case wxFONTENCODING_ISO8859_8:
        case wxFONTENCODING_ISO8859_9:
        case wxFONTENCODING_ISO8859_10:
        case wxFONTENCODING_ISO8859_11:
        case wxFONTENCODING_ISO8859_13:
        case wxFONTENCODING_ISO8859_14:
        case wxFONTENCODING_ISO8859_15:
            info->mglEncoding = MGL_ENCODING_ISO8859_1 +
                                (encoding - wxFONTENCODING_ISO8859_1);
            break;

        case wxFONTENCODING_KOI8:
            info->mglEncoding = MGL_ENCODING_KOI8;
            break;

        case wxFONTENCODING_CP1250:
        case wxFONTENCODING_CP1251:
        case wxFONTENCODING_CP1252:
        case wxFONTENCODING_CP1253:
        case wxFONTENCODING_CP1254:
        case wxFONTENCODING_CP1255:
        case wxFONTENCODING_CP1256:
        case wxFONTENCODING_CP1257:
            info->mglEncoding = MGL_ENCODING_CP1250 +
                                (encoding - wxFONTENCODING_CP1250);
            break;

        case wxFONTENCODING_SYSTEM:
            info->mglEncoding = MGL_ENCODING_ASCII;
            break;

        default:
            // encoding not known to MGL
            return FALSE;
    }

    info->encoding = encoding;

    return TRUE;
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    if ( !info.facename )
        return TRUE;
        
    wxMGLFontFamily *family = wxTheFontsManager->GetFamily(info.facename);
    if ( !family )
        return FALSE;
    if ( family->GetInfo()->fontLibType == MGL_BITMAPFONT_LIB )
        return (info.mglEncoding == MGL_ENCODING_ASCII ||
                info.mglEncoding == MGL_ENCODING_ISO8859_1 ||
                info.mglEncoding == MGL_ENCODING_ISO8859_15 ||
                info.mglEncoding == MGL_ENCODING_CP1252);
    else
        return TRUE;
}


// ----------------------------------------------------------------------------
// wxFontFamily, wxMGLFontInstance, wxMGLFontLibrary
// ----------------------------------------------------------------------------

WX_DECLARE_LIST(wxMGLFontInstance, wxMGLFontInstanceList);
WX_DEFINE_LIST(wxMGLFontInstanceList);
WX_DEFINE_LIST(wxMGLFontFamilyList);

wxMGLFontInstance::wxMGLFontInstance(wxMGLFontLibrary *fontLib, 
                                     float pt, bool slant, bool aa)
{
    m_fontLib = fontLib;
    m_font = NULL;
    m_pt = pt;
    m_slant = slant;
    m_aa = aa;

    float slantAngle = m_slant ? 15.0 : 0.0;

    wxLogTrace("mgl_font", "loading instance of '%s' slant=%i pt=%0.1f aa=%i", 
               m_fontLib->GetMGLfont_lib_t()->name, m_slant, m_pt, m_aa);
    m_font = MGL_loadFontInstance(m_fontLib->GetMGLfont_lib_t(), 
                                  m_pt, slantAngle, 0.0, aa);
    wxASSERT_MSG( m_font, wxT("Cannot create font instance.") );
}

wxMGLFontInstance::~wxMGLFontInstance()
{
    wxLogTrace("mgl_font", "unloading instance of '%s' slant=%i pt=%0.1f aa=%i", 
               m_fontLib->GetMGLfont_lib_t()->name, m_slant, m_pt, m_aa);
    if ( m_font )
        MGL_unloadFontInstance(m_font);
}
    
wxMGLFontLibrary::wxMGLFontLibrary(const wxString& filename, int type, 
                                   wxMGLFontFamily *parentFamily)
{
    m_family = parentFamily;
    m_type = type;
    m_fileName = filename;
    m_refs = 0;
    m_fontLib = NULL;

    m_instances = new wxMGLFontInstanceList;
    m_instances->DeleteContents(TRUE);
}

wxMGLFontLibrary::~wxMGLFontLibrary()
{
    wxLogTrace("mgl_font", "font library dtor '%s'", m_fileName.mb_str());
    delete m_instances;
}
    
void wxMGLFontLibrary::IncRef()
{
    wxLogTrace("mgl_font", "incRef(%u) '%s'", m_refs, m_fileName.c_str());
    if ( m_refs++ == 0 )
    {
        wxLogTrace("mgl_font", "opening library '%s'", m_fileName.mb_str());
        m_fontLib = MGL_openFontLib(m_fileName.mb_str());
    }
}

void wxMGLFontLibrary::DecRef()
{
    wxLogTrace("mgl_font", "decRef(%u) '%s'", m_refs, m_fileName.c_str());
    if ( --m_refs == 0 )
    {
        wxLogTrace("mgl_font", "killing instances of '%s'", m_fileName.mb_str());
        m_instances->Clear();
        wxLogTrace("mgl_font", "closing library '%s'", m_fileName.mb_str());
        MGL_closeFontLib(m_fontLib);
        m_fontLib = NULL;
    }
}

static int gs_antialiasingThreshold = -1;

wxMGLFontInstance *wxMGLFontLibrary::GetFontInstance(wxFont *font, 
                                                     float scale, bool aa)
{
    wxASSERT_MSG(m_refs > 0 && m_fontLib, wxT("font library not loaded!"));

    wxString facename;
    bool slant;
    bool antialiased;
    float pt = (float)font->GetPointSize() * scale;

    if ( gs_antialiasingThreshold == -1 )
    {
        gs_antialiasingThreshold = 10;
#if wxUSE_SYSTEM_OPTIONS
        if ( wxSystemOptions::HasOption(wxT("mgl.aa-threshold")) )
            gs_antialiasingThreshold = 
                wxSystemOptions::GetOptionInt(wxT("mgl.aa-threshold"));
        wxLogTrace("mgl_font", "AA threshold set to %i", gs_antialiasingThreshold);
#endif
    }

    // Small characters don't look good when antialiased with the algorithm
    // that FreeType uses (mere 2x2 supersampling), so lets disable it AA
    // completely for small fonts.
    if ( pt <= gs_antialiasingThreshold )
        antialiased = FALSE;
    else
        antialiased = (m_fontLib->fontLibType == MGL_BITMAPFONT_LIB) ? FALSE : aa;

    slant = (((m_type & wxFONTFACE_ITALIC) == 0) &&
             (font->GetStyle() == wxSLANT || font->GetStyle() == wxITALIC));

    // FIXME_MGL -- MGL does not yet support slant, although the API is there
    slant = FALSE;

    wxLogTrace("mgl_font", "requested instance of '%s' slant=%i pt=%0.1f aa=%i", 
               m_fileName.mb_str(), slant, pt, antialiased);

    wxMGLFontInstance *i;
    wxMGLFontInstanceList::Node *node;
    
    for (node = m_instances->GetFirst(); node; node = node->GetNext())
    {
        i = node->GetData();
        if ( i->GetPt() == pt && i->GetSlant() == slant &&
             i->GetAA() == antialiased )
        {
            wxLogTrace("mgl_font", "    got from cache: slant=%i pt=%0.1f aa=%i", 
                       i->GetSlant(), i->GetPt(), i->GetAA());
            return i;
        }
    }    
    
    i = new wxMGLFontInstance(this, pt, slant, antialiased);
    m_instances->Append(i);
    return i;   
}


wxMGLFontFamily::wxMGLFontFamily(const font_info_t *info)
{
    m_name = info->familyName;
    m_fontInfo = info;
    
    if ( info->regularFace[0] == '\0' )
        m_fontLibs[wxFONTFACE_REGULAR] = NULL;
    else
        m_fontLibs[wxFONTFACE_REGULAR] = 
            new wxMGLFontLibrary(info->regularFace, wxFONTFACE_REGULAR, this);
    
    if ( info->italicFace[0] == '\0' )
        m_fontLibs[wxFONTFACE_ITALIC] = NULL;
    else
        m_fontLibs[wxFONTFACE_ITALIC] = 
            new wxMGLFontLibrary(info->italicFace, wxFONTFACE_ITALIC, this);
    
    if ( info->boldFace[0] == '\0' )
        m_fontLibs[wxFONTFACE_BOLD] = NULL;
    else
        m_fontLibs[wxFONTFACE_BOLD] = 
            new wxMGLFontLibrary(info->boldFace, wxFONTFACE_BOLD, this);
    
    if ( info->boldItalicFace[0] == '\0' )
        m_fontLibs[wxFONTFACE_BOLD_ITALIC] = NULL;
    else
        m_fontLibs[wxFONTFACE_BOLD_ITALIC] = 
            new wxMGLFontLibrary(info->boldItalicFace, wxFONTFACE_BOLD_ITALIC, this);
            
    wxLogTrace("mgl_font", "new family '%s' (r=%s, i=%s, b=%s, bi=%s)\n",
               info->familyName, info->regularFace, info->italicFace, 
               info->boldFace, info->boldItalicFace);
}

wxMGLFontFamily::~wxMGLFontFamily()
{
    for (size_t i = 0; i < wxFONTFACE_MAX; i++)
        delete m_fontLibs[i];
}

bool wxMGLFontFamily::HasFace(int type) const
{
    return (m_fontLibs[type] != NULL);
}


// ----------------------------------------------------------------------------
// wxFontsManager
// ----------------------------------------------------------------------------

wxMGLFontLibrary *wxFontsManager::GetFontLibrary(wxFont *font)
{
    wxMGLFontFamily *family;
    int type;
    wxString facename = font->GetFaceName();
    
    if ( !facename.IsEmpty() )
        family = GetFamily(facename);
    else
        family = NULL;

    if ( !family )
    {
        facename.Empty();
        switch (font->GetFamily())
        {
            case wxSCRIPT:
                facename = wxT("Script");
                break;
            case wxDECORATIVE:
                facename = wxT("Charter");
                break;
            case wxROMAN:
                facename = wxT("Times");
                break;
            case wxTELETYPE:
            case wxMODERN:
                facename = wxT("Courier");
                break;
            case wxSWISS:
                facename = wxT("Helvetica");
                break;
            case wxDEFAULT:
            default:
                facename = wxT("Helvetica");
                break;
        }

        family = GetFamily(facename);
        if ( !family )
        {
           if ( m_list->GetFirst() )
               family = m_list->GetFirst()->GetData();
           else
               wxFAIL_MSG(wxT("Fatal error, no fonts available!"));
        }
    }

    type = wxFONTFACE_REGULAR;

    if ( font->GetWeight() == wxBOLD )
        type |= wxFONTFACE_BOLD;

    // FIXME_MGL -- this should read "if ( font->GetStyle() == wxITALIC )",
    // but since MGL does not yet support slant, we try to display it with
    // italic face (better than nothing...)
    if ( font->GetStyle() == wxITALIC || font->GetStyle() == wxSLANT )
    {
        if ( family->HasFace(type | wxFONTFACE_ITALIC) )
            type |= wxFONTFACE_ITALIC;
    }
    if ( !family->HasFace(type) )
    {
        for (int i = 0; i < wxFONTFACE_MAX; i++)
            if ( family->HasFace(i) )
            {
                type = i;
                break;
            }
    }
    
    return family->GetLibrary(type);
}

static ibool MGLAPI enum_callback(const font_info_t *info, void *cookie)
{
    wxFontsManager *db = (wxFontsManager*)cookie;
    db->AddFamily(info);
    return TRUE;
}

wxFontsManager::wxFontsManager()
{
    m_hash = new wxHashTable(wxKEY_STRING);
    m_hash->DeleteContents(FALSE);
    m_list = new wxMGLFontFamilyList;
    m_list->DeleteContents(TRUE);
    MGL_enumerateFonts(enum_callback, (void*)this);
}

wxFontsManager::~wxFontsManager()
{
    delete m_hash;
    delete m_list;
}

void wxFontsManager::AddFamily(const font_info_t *info)
{
    wxMGLFontFamily *f = new wxMGLFontFamily(info);
    m_hash->Put(f->GetName().Lower(), f);
    m_list->Append(f);
}
        
wxMGLFontFamily *wxFontsManager::GetFamily(const wxString& name) const
{
    return (wxMGLFontFamily*)m_hash->Get(name.Lower());
}


wxFontsManager *wxTheFontsManager = NULL;
