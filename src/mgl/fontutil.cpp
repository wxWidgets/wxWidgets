/////////////////////////////////////////////////////////////////////////////
// Name:        mgl/fontutil.cpp
// Purpose:     Font helper functions for MGL
// Author:      Vaclav Slavik
// Created:     2001/04/29
// RCS-ID:      $Id$
// Copyright:   (c) 2001, Vaclav Slavik
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
    #pragma implementation "fontutil.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // PCH


#include "wx/fontutil.h"
#include "wx/fontmap.h"
#include "wx/tokenzr.h"
#include "wx/hash.h"
#include "wx/module.h"
#include "wx/listimpl.cpp"
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
    return (wxTheFontsManager->GetFamily(info.facename) != NULL);
    // FIXME_MGL -- do .fon files contains full unicode?
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

    float slantAngle = m_slant ? 1.0 : 0.0;

    m_font = MGL_loadFontInstance(m_fontLib->GetMGLfont_lib_t(), 
                                  m_pt, slantAngle, 0.0, aa);
}

wxMGLFontInstance::~wxMGLFontInstance()
{
    MGL_unloadFontInstance(m_font);
}
    
wxMGLFontLibrary::wxMGLFontLibrary(const wxString& filename, int type)
{
    m_type = type;
    m_fileName = filename;
    m_refs = 0;
    m_fontLib = NULL;

    m_instances = new wxMGLFontInstanceList;
    m_instances->DeleteContents(TRUE);
}

wxMGLFontLibrary::~wxMGLFontLibrary()
{
    delete m_instances;
}
    
void wxMGLFontLibrary::IncRef()
{
    if ( m_refs++ == 0 )
    {
        m_fontLib = MGL_openFontLib(m_fileName.mb_str());
    }
}

void wxMGLFontLibrary::DecRef()
{
    if ( --m_refs == 0 )
    {
        m_instances->Clear();
        MGL_closeFontLib(m_fontLib);
        m_fontLib = NULL;
    }
}

wxMGLFontInstance *wxMGLFontLibrary::GetFontInstance(wxFont *font, 
                                                     float scale, bool aa)
{
    wxString facename;
    bool slant;
    float pt = (float)font->GetPointSize() * scale;

    slant = ((m_type & wxFONTFACE_ITALIC == 0) &&
             (font->GetStyle() == wxSLANT || font->GetStyle() == wxITALIC));

    wxMGLFontInstance *i;
    wxMGLFontInstanceList::Node *node;
    
    for (node = m_instances->GetFirst(); node; node->GetNext())
    {
        i = node->GetData();
        if ( i->GetPt() == pt && i->GetSlant() == slant &&
             i->GetAA() == aa )
            return i;
    }    
    
    i = new wxMGLFontInstance(this, pt, slant, aa);
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
            new wxMGLFontLibrary(info->regularFace, wxFONTFACE_REGULAR);
    
    if ( info->italicFace[0] == '\0' )
        m_fontLibs[wxFONTFACE_ITALIC] = NULL;
    else
        m_fontLibs[wxFONTFACE_ITALIC] = 
            new wxMGLFontLibrary(info->italicFace, wxFONTFACE_ITALIC);
    
    if ( info->boldFace[0] == '\0' )
        m_fontLibs[wxFONTFACE_BOLD] = NULL;
    else
        m_fontLibs[wxFONTFACE_BOLD] = 
            new wxMGLFontLibrary(info->boldFace, wxFONTFACE_BOLD);
    
    if ( info->boldItalicFace[0] == '\0' )
        m_fontLibs[wxFONTFACE_BOLD_ITALIC] = NULL;
    else
        m_fontLibs[wxFONTFACE_BOLD_ITALIC] = 
            new wxMGLFontLibrary(info->boldItalicFace, wxFONTFACE_BOLD_ITALIC);
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
    wxString facename;
    int type;
    
    // FIXME_MGL -- handle encodings!
    if ( facename )
        family = GetFamily(font->GetFaceName());
    else
        family = NULL;
    if ( family )
        facename = font->GetFaceName();
    else
        facename.Empty();
    
    if ( !family )
    {
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
        if ( !family && m_list->GetFirst() )
        {
            family = m_list->GetFirst()->GetData();
        }
        else
        {
            wxFAIL_MSG(wxT("Fatal error, no fonts available!"));
        }
    }

    type = wxFONTFACE_REGULAR;

    if ( font->GetWeight() == wxBOLD )
        type |= wxFONTFACE_BOLD;

    if ( font->GetStyle() == wxITALIC )
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

static ibool enum_callback(const font_info_t *info, void *cookie)
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
    m_hash->Put(info->familyName, f);
    m_list->Append(f);
}
        
wxMGLFontFamily *wxFontsManager::GetFamily(const wxString& name) const
{
    return (wxMGLFontFamily*)m_hash->Get(name);
}


wxFontsManager *wxTheFontsManager = NULL;


// A module that takes care of fonts DB initialization and destruction:

class wxFontutilModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxFontutilModule)
public:
    wxFontutilModule() {}
    bool OnInit() 
    {
        wxTheFontsManager = new wxFontsManager;
        return TRUE; 
    }
    void OnExit() 
    { 
        delete wxTheFontsManager;
    }
};

IMPLEMENT_DYNAMIC_CLASS(wxFontutilModule, wxModule)
