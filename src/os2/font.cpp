/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont class
// Author:      David Webster
// Modified by:
// Created:     10/06/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#define DEBUG_PRINTF(NAME)   { static int raz=0; \
  printf( #NAME " %i\n",raz); fflush(stdout);       \
   raz++;                                        \
 }

 #include <malloc.h>
 #include <stdio.h>

 int HeapCheck(void)
 {
    int  rc;

    if (_HEAPOK != (rc = _heapchk()))
    {
       switch(rc)
       {
          case _HEAPEMPTY:
             puts("The heap has not been initialized.");
             break;
          case _HEAPBADNODE:
             puts("A memory node is corrupted or the heap is damaged.");
             break;
          case _HEAPBADBEGIN:
             puts("The heap specified is not valid.");
             break;
       }
       fflush(stdout);
    }
    return 0;
 }

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

 #include <malloc.h>
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/setup.h"
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/font.h"
#endif // WX_PRECOMP

#include "wx/os2/private.h"

    IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

    #if wxUSE_PORTABLE_FONTS_IN_MSW
        IMPLEMENT_DYNAMIC_CLASS(wxFontNameDirectory, wxObject)
    #endif

// ----------------------------------------------------------------------------
// wxFontRefData - the internal description of the font
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontRefData: public wxGDIRefData
{
friend class WXDLLEXPORT wxFont;

public:
    wxFontRefData()
    {
        Init(12, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE,
             "", wxFONTENCODING_DEFAULT);
    }

    wxFontRefData(const wxFontRefData& data)
    {
        Init(data.m_pointSize, data.m_family, data.m_style, data.m_weight,
             data.m_underlined, data.m_faceName, data.m_encoding);

        m_fontId = data.m_fontId;
    }

    wxFontRefData(int size,
                  int family,
                  int style,
                  int weight,
                  bool underlined,
                  const wxString& faceName,
                  wxFontEncoding encoding)
    {
        Init(size, family, style, weight, underlined, faceName, encoding);
    }

    virtual ~wxFontRefData();

protected:
    // common part of all ctors
    void Init(int size,
              int family,
              int style,
              int weight,
              bool underlined,
              const wxString& faceName,
              wxFontEncoding encoding);

    // If TRUE, the pointer to the actual font is temporary and SHOULD NOT BE
    // DELETED by destructor
    bool          m_temporary;

    int           m_fontId;

    // font characterstics
    int           m_pointSize;
    int           m_family;
    int           m_style;
    int           m_weight;
    bool          m_underlined;
    wxString      m_faceName;
    wxFontEncoding m_encoding;

    // Windows font handle
    WXHFONT       m_hFont;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

void wxFontRefData::Init(int pointSize,
                         int family,
                         int style,
                         int weight,
                         bool underlined,
                         const wxString& faceName,
                         wxFontEncoding encoding)
{
    m_style = style;
    m_pointSize = pointSize;
    m_family = family;
    m_style = style;
    m_weight = weight;
    m_underlined = underlined;
    m_faceName = faceName;
    m_encoding = encoding;

    m_fontId = 0;
    m_temporary = FALSE;

    m_hFont = 0;
}

wxFontRefData::~wxFontRefData()
{
DEBUG_PRINTF(wxFontRefData::~wxFontRefData!!!)
// TODO:
//    if ( m_hFont )
//    {
//        if ( !::DeleteObject((HFONT) m_hFont) )
//        {
//            wxLogLastError("DeleteObject(font)");
//        }
//    }
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

void wxFont::Init()
{

DEBUG_PRINTF(wxFontRefData::~wxFontRefData!!!)

    if ( wxTheFontList )
        wxTheFontList->Append(this);
}

/* Constructor for a font. Note that the real construction is done
 * in wxDC::SetFont, when information is available about scaling etc.
 */
bool wxFont::Create(int pointSize,
                    int family,
                    int style,
                    int weight,
                    bool underlined,
                    const wxString& faceName,
                    wxFontEncoding encoding)
{
    UnRef();
DEBUG_PRINTF(wxFontRefData::~wxFontRefData!!!)
    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, faceName, encoding);

    RealizeResource();

    return TRUE;
}

wxFont::~wxFont()
{
  int l;
  l = sizeof(*this);
HeapCheck();
 _heap_check();

    if ( wxTheFontList )
        wxTheFontList->DeleteObject(this);
HeapCheck();
 _heap_check();

}

// ----------------------------------------------------------------------------
// real implementation
// Boris' Kovalenko comments:
//   Because OS/2 fonts are associated with PS we can not create the font
//   here, but we may check that font definition is true
// ----------------------------------------------------------------------------

bool wxFont::RealizeResource()
{
DEBUG_PRINTF(wxFont::RealizeResource)
    if ( GetResourceHandle() )
    {
        // VZ: the old code returned FALSE in this case, but it doesn't seem
        //     to make sense because the font _was_ created
        wxLogDebug(wxT("Calling wxFont::RealizeResource() twice"));

       return TRUE;
    }

    HPS          hps;
    FATTRS       fAttrs;
    FACENAMEDESC fName;
    LONG         fLid;

    fAttrs.usRecordLength = sizeof(FATTRS);
    fAttrs.fsFontUse = FATTR_FONTUSE_OUTLINE |       // only outline fonts allowed
                       FATTR_FONTUSE_TRANSFORMABLE;  // may be transformed
    fAttrs.fsType = 0;
    fAttrs.lMaxBaselineExt = fAttrs.lAveCharWidth = 0;
    fAttrs.idRegistry = 0;
    fAttrs.lMatch = 0;

    fName.usSize = sizeof(FACENAMEDESC);
    fName.usWidthClass = FWIDTH_NORMAL;
    fName.usReserved = 0;
    fName.flOptions = 0;

    wxString ff_face;

// OS/2 combines the family with styles to give a facename

    switch ( M_FONTDATA->m_family )
    {
        case wxSCRIPT:
        case wxDECORATIVE:
        case wxROMAN:
            ff_face = wxT("Times New Roman") ;
            break;

        case wxTELETYPE:
        case wxMODERN:
            ff_face = wxT("Courier") ;
            break;

        case wxSWISS:
        case wxDEFAULT:
        default:
            ff_face = wxT("Helvetica") ;
    }

    switch ( M_FONTDATA->m_style )
    {
        case wxITALIC:
        case wxSLANT:
            fAttrs.fsSelection = FATTR_SEL_ITALIC;
            break;

        default:
            wxFAIL_MSG(wxT("unknown font slant"));
            // fall through

        case wxNORMAL:
            fAttrs.fsSelection = 0;
    }

    switch ( M_FONTDATA->m_weight )
    {
        default:
            wxFAIL_MSG(wxT("unknown font weight"));
            // fall through

        case wxNORMAL:
            fName.usWeightClass = FWEIGHT_NORMAL;
            break;

        case wxLIGHT:
            fName.usWeightClass = FWEIGHT_LIGHT;
            break;

        case wxBOLD:
            fName.usWeightClass = FWEIGHT_BOLD;
            break;
    }

    if( M_FONTDATA->m_underlined )
        fAttrs.fsSelection |= FATTR_SEL_UNDERSCORE;

    wxFontEncoding encoding = M_FONTDATA->m_encoding;
    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    switch ( encoding )
    {
        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_15:
        case wxFONTENCODING_CP1250:
            fAttrs.usCodePage = 1250;
            break;

        case wxFONTENCODING_ISO8859_2:
        case wxFONTENCODING_CP1252:
            fAttrs.usCodePage = 1252;
            break;

        case wxFONTENCODING_ISO8859_4:
        case wxFONTENCODING_ISO8859_10:
            fAttrs.usCodePage = 850; // what is baltic?
            break;

        case wxFONTENCODING_ISO8859_5:
        case wxFONTENCODING_CP1251:
            fAttrs.usCodePage = 1251;
            break;

        case wxFONTENCODING_ISO8859_6:
            fAttrs.usCodePage = 850;  // what is arabic?
            break;

        case wxFONTENCODING_ISO8859_7:
            fAttrs.usCodePage = 850;  // what is greek
            break;

        case wxFONTENCODING_ISO8859_8:
            fAttrs.usCodePage = 850; // what is hebrew?
            break;

        case wxFONTENCODING_ISO8859_9:
            fAttrs.usCodePage = 857;
            break;

        case wxFONTENCODING_ISO8859_11:
            fAttrs.usCodePage = 850; // what is thai
            break;

        case wxFONTENCODING_CP437:
            fAttrs.usCodePage = 437;
            break;

        default:
            wxFAIL_MSG(wxT("unsupported encoding"));
            // fall through

        case wxFONTENCODING_SYSTEM:
            fAttrs.usCodePage = 850; // what is ANSI?
            break;
    }

// Now cheking
    fLid = 1;
    hps  = ::WinGetPS( HWND_DESKTOP );

    long numLids = ::GpiQueryNumberSetIds( hps );
    long gpiError;

    // First we should generate unique id
    if( numLids )
    {
        long Types[255];
        STR8 Names[255];
        long lIds[255];

        if( !GpiQuerySetIds(hps, numLids, Types, Names, lIds) )
        {
            ::WinReleasePS( hps );
            return 0;
        }

        for(unsigned long LCNum = 0; LCNum < numLids; LCNum++)
            if(lIds[LCNum] == fLid)
               ++fLid;
        if(fLid > 254)  // wow, no id available!
        {
           ::WinReleasePS( hps );
           return 0;
        }
    }

    // now building facestring
    if(::GpiQueryFaceString(hps, ff_face.c_str(), &fName, FACESIZE, fAttrs.szFacename) == GPI_ERROR)
    {
       ::WinReleasePS( hps );
       return 0;
    }

    // now creating font
    WXHFONT hFont = (WXHFONT)0;

    if(::GpiCreateLogFont(hps, NULL, fLid, &fAttrs) != GPI_ERROR)
       M_FONTDATA->m_hFont = hFont = (WXHFONT)1;

    if( hFont )
        ::GpiDeleteSetId(hps, fLid);

    ::WinReleasePS( hps );

    if ( !hFont )
    {
        wxLogLastError("CreateFont");
    }

    return hFont != 0;
}

bool wxFont::FreeResource(bool force)
{
    if ( GetResourceHandle() )
    {
// TODO:
//        if ( !::DeleteObject((HFONT) M_FONTDATA->m_hFont) )
//        {
//            wxLogLastError("DeleteObject(font)");
//        }

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

bool wxFont::IsFree() const
{
    return (M_FONTDATA && (M_FONTDATA->m_hFont == 0));
}

void wxFont::Unshare()
{
    // Don't change shared data
    if ( !m_refData )
    {
        m_refData = new wxFontRefData();
    }
    else
    {
        wxFontRefData* ref = new wxFontRefData(*M_FONTDATA);
        UnRef();
        m_refData = ref;
    }
}

// ----------------------------------------------------------------------------
// change font attribute: we recreate font when doing it
// ----------------------------------------------------------------------------

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

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->m_encoding = encoding;

    RealizeResource();
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
DEBUG_PRINTF(wxFont::GetPointSize)
    wxFontRefData*                  pTmp;

    pTmp = M_FONTDATA;
    if(pTmp)
      return pTmp->m_pointSize;
    else
        return 10;
}

int wxFont::GetFamily() const
{
    return M_FONTDATA->m_family;
}

int wxFont::GetFontId() const
{
    return M_FONTDATA->m_fontId;
}

int wxFont::GetStyle() const
{
    return M_FONTDATA->m_style;
}

int wxFont::GetWeight() const
{
    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    return M_FONTDATA->m_underlined;
}

wxString wxFont::GetFaceName() const
{
    wxString str;
    if ( M_FONTDATA )
        str = M_FONTDATA->m_faceName ;
    return str;
}

wxFontEncoding wxFont::GetEncoding() const
{
    return M_FONTDATA->m_encoding;
}

