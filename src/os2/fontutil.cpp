///////////////////////////////////////////////////////////////////////////////
// Name:        msw/fontutil.cpp
// Purpose:     font-related helper functions for wxMSW
// Author:      Modified by David Webster for OS/2
// Modified by:
// Created:     01.03.00
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////
#define DEBUG_PRINTF(NAME)   { static int raz=0; \
  printf( #NAME " %i\n",raz); fflush(stdout);       \
   raz++;                                        \
 }

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "fontutil.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif //WX_PRECOMP

#include "wx/os2/private.h"

#include "wx/fontutil.h"
#include "wx/fontmap.h"

#include "wx/tokenzr.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNativeEncodingInfo
// ----------------------------------------------------------------------------

// convert to/from the string representation: format is
//      encodingid;facename[;charset]

bool wxNativeEncodingInfo::FromString(
  const wxString&                   rsStr
)
{
    wxStringTokenizer               vTokenizer(rsStr, _T(";"));
    wxString                        sEncid = vTokenizer.GetNextToken();
    long                            lEnc;

    if (!sEncid.ToLong(&lEnc))
        return FALSE;
    encoding = (wxFontEncoding)lEnc;
    facename = vTokenizer.GetNextToken();
    if (!facename)
        return FALSE;

    wxString                        sTmp = vTokenizer.GetNextToken();

    if (!sTmp)
    {
        charset = 850;
    }
    else
    {
        if ( wxSscanf(sTmp, _T("%u"), &charset) != 1 )
        {
            // should be a number!
            return FALSE;
        }
    }
    return TRUE;
} // end of wxNativeEncodingInfo::FromString

wxString wxNativeEncodingInfo::ToString() const
{
    wxString                        sStr;

    sStr << (long)encoding << _T(';') << facename;

    if (charset != 850)
    {
        sStr << _T(';') << charset;
    }
    return sStr;
} // end of wxNativeEncodingInfo::ToString

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

bool wxGetNativeFontEncoding(
  wxFontEncoding                    vEncoding
, wxNativeEncodingInfo*             pInfo
)
{
    wxCHECK_MSG(pInfo, FALSE, _T("bad pointer in wxGetNativeFontEncoding") );
    if (vEncoding == wxFONTENCODING_DEFAULT)
    {
        vEncoding = wxFont::GetDefaultEncoding();
    }
    switch (vEncoding)
    {
        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_15:
        case wxFONTENCODING_CP1250:
            pInfo->charset = 1250;
            break;

        case wxFONTENCODING_ISO8859_2:
        case wxFONTENCODING_CP1252:
            pInfo->charset = 1252;
            break;

        case wxFONTENCODING_ISO8859_4:
        case wxFONTENCODING_ISO8859_10:
            pInfo->charset = 921; // what is baltic?
            break;

        case wxFONTENCODING_ISO8859_5:
        case wxFONTENCODING_CP1251:
            pInfo->charset = 1251;
            break;

        case wxFONTENCODING_ISO8859_6:
            pInfo->charset = 864;
            break;

        case wxFONTENCODING_ISO8859_7:
            pInfo->charset = 869;
            break;

        case wxFONTENCODING_ISO8859_8:
            pInfo->charset = 862;
            break;

        case wxFONTENCODING_ISO8859_9:
            pInfo->charset = 857;
            break;

        case wxFONTENCODING_ISO8859_11:
            pInfo->charset = 874; // what is thai
            break;

        case wxFONTENCODING_CP437:
            pInfo->charset = 437;
            break;

        default:
            wxFAIL_MSG(wxT("unsupported encoding"));
            // fall through

        case wxFONTENCODING_SYSTEM:
            pInfo->charset = 850;
            break;
    }
    return TRUE;
} // end of wxGetNativeFontEncoding

bool wxTestFontEncoding(
  const wxNativeEncodingInfo&       rInfo
)
{
    FATTRS                          vLogFont;
    HPS                             hPS;

    hPS = ::WinGetPS(HWND_DESKTOP);

    memset(&vLogFont, '\0', sizeof(FATTRS));           // all default values
    vLogFont.usRecordLength = sizeof(FATTRS);
    vLogFont.usCodePage = rInfo.charset;
    vLogFont.lMaxBaselineExt = 0L;                    // Outline fonts should use 0
    vLogFont.lAveCharWidth = 0L;                      // Outline fonts should use 0
    vLogFont.fsFontUse = FATTR_FONTUSE_OUTLINE |      // only outline fonts allowed
                         FATTR_FONTUSE_TRANSFORMABLE; // may be transformed

    strncpy(vLogFont.szFacename, rInfo.facename.c_str(), sizeof(vLogFont.szFacename));

    if (!::GpiCreateLogFont( hPS
                            ,NULL
                            ,1L
                            ,&vLogFont
                           ))
    {
        ::WinReleasePS(hPS);
        return FALSE;
    }
    ::WinReleasePS(hPS);
    return TRUE;
} // end of wxTestFontEncoding

// ----------------------------------------------------------------------------
// wxFont <-> LOGFONT conversion
// ----------------------------------------------------------------------------

void wxFillLogFont(
  LOGFONT*                          pLogFont // OS2 GPI FATTRS
, PFACENAMEDESC                     pFaceName
, const wxFont*                     pFont
)
{
    wxString                        sFace;
    USHORT                          uWeight;
    int                             nItalic;

    pLogFont->fsSelection = 0;
    pLogFont->fsSelection = FATTR_SEL_OUTLINE; // we will alway use only outlines
    pFaceName->usWeightClass = 0;
    pFaceName->flOptions = 0;
    switch (pFont->GetFamily())
    {
        case wxSCRIPT:
            sFace = _T("Script");
            break;

        case wxDECORATIVE:
        case wxROMAN:
            sFace = _T("Times New Roman");
            break;

        case wxTELETYPE:
        case wxMODERN:
            sFace = _T("Courier New");
            break;

        case wxSWISS:
            sFace = _T("WarpSans");
            break;

        case wxDEFAULT:
        default:
            sFace = _T("Helv");
    }

    switch (pFont->GetWeight())
    {
        default:
            wxFAIL_MSG(_T("unknown font weight"));
            uWeight = FWEIGHT_DONT_CARE;
            break;

        case wxNORMAL:
            uWeight = FWEIGHT_NORMAL;
            break;

        case wxLIGHT:
            uWeight = FWEIGHT_LIGHT;
            break;

        case wxBOLD:
            uWeight = FWEIGHT_BOLD;
            pLogFont->fsSelection |= FATTR_SEL_BOLD;
            break;

        case wxFONTWEIGHT_MAX:
            uWeight = FWEIGHT_ULTRA_BOLD;
            pLogFont->fsSelection |= FATTR_SEL_BOLD;
            break;
    }
    pFaceName->usWeightClass |= uWeight;

    switch (pFont->GetStyle())
    {
        case wxITALIC:
        case wxSLANT:
            nItalic = FTYPE_ITALIC;
            pLogFont->fsSelection |= FATTR_SEL_ITALIC;
            break;

        default:
            wxFAIL_MSG(wxT("unknown font slant"));
            // fall through

        case wxNORMAL:
            nItalic = 0;
            break;
    }
    pFaceName->flOptions |= nItalic;
    if(pFont->GetUnderlined())
        pLogFont->fsSelection |= FATTR_SEL_UNDERSCORE;

    //
    // In PM a font's height is expressed in points.  A point equals
    // approximately 1/72 of an inch.  We'll assume for now that,
    // like Windows, that fonts are 96 dpi.
    //
    DEVOPENSTRUC                    vDop = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    HDC                             hDC = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    LONG                            lStart = CAPS_FAMILY;
    LONG                            lCount = CAPS_VERTICAL_RESOLUTION;
    LONG                            alArray[CAPS_VERTICAL_RESOLUTION];
    LONG                            lRes;
    int                             nPpInch;


    ::DevQueryCaps(hDC, lStart, lCount, alArray);
    lRes = alArray[CAPS_VERTICAL_RESOLUTION-1];
    if (lRes > 0)
        nPpInch = (int)(lRes/39.6); // lres is in pixels per meter
    else
        nPpInch = 96;

    int                             nHeight = (pFont->GetPointSize() * nPpInch/72);
    wxString                        sFacename = pFont->GetFaceName();

    if (!!sFacename)
    {
        sFace = sFacename;
    }
    //else: ff_face is a reasonable default facename for this font family

    //
    // Deal with encoding now
    //
    wxNativeEncodingInfo            vInfo;
    wxFontEncoding                  vEncoding = pFont->GetEncoding();

    if (!wxGetNativeFontEncoding( vEncoding
                                 ,&vInfo
                                ))
    {
        if ( !wxTheFontMapper->GetAltForEncoding( vEncoding
                                                 ,&vInfo
                                                ))
        {
            //
            // Unsupported encoding, replace with the default
            //
            vInfo.charset = 850;
        }
    }

    if (!vInfo.facename.IsEmpty() )
    {
        //
        // The facename determined by the encoding overrides everything else
        //
        sFace = vInfo.facename;
    }

    //
    // Transfer all the data to LOGFONT
    //
    pLogFont->usRecordLength = sizeof(FATTRS);
    wxStrcpy(pLogFont->szFacename, sFace.c_str());
    pLogFont->usCodePage = vInfo.charset;
    pLogFont->fsFontUse |= FATTR_FONTUSE_OUTLINE |
                           FATTR_FONTUSE_TRANSFORMABLE;
} // end of wxFillLogFont

wxFont wxCreateFontFromLogFont(
  const LOGFONT*                    pLogFont
, const PFONTMETRICS                pFM
, PFACENAMEDESC                     pFaceName
)
{
    //
    // Extract family from facename
    //
    int                             nFontFamily;

    if (strcmp(pLogFont->szFacename, "Times New Roman") == 0)
        nFontFamily = wxROMAN;
    else if (strcmp(pLogFont->szFacename, "WarpSans") == 0)
        nFontFamily = wxSWISS;
    else if (strcmp(pLogFont->szFacename, "Script") == 0)
        nFontFamily = wxSCRIPT;
    else if (strcmp(pLogFont->szFacename, "Courier New") == 0)
        nFontFamily = wxMODERN;
    else
        nFontFamily = wxSWISS;

    //
    // Weight and Style
    //
    int                             nFontWeight = wxNORMAL;

    switch (pFaceName->usWeightClass)
    {
        case FWEIGHT_LIGHT:
            nFontWeight = wxLIGHT;
            break;

        default:
        case FWEIGHT_NORMAL:
            nFontWeight = wxNORMAL;
            break;

        case FWEIGHT_BOLD:
            nFontWeight = wxBOLD;
            break;
    }

    int                             nFontStyle;

    if(pLogFont->fsSelection & FATTR_SEL_ITALIC)
        nFontStyle = wxITALIC;
    else
        nFontStyle = wxNORMAL;

    bool                            bFontUnderline = (pLogFont->fsSelection & FATTR_SEL_UNDERSCORE);
    wxString                        sFontFace = pLogFont->szFacename;
    int                             nFontPoints = pFM->lEmHeight;
    wxFontEncoding                  vFontEncoding;

    switch (pLogFont->usCodePage)
    {
        default:
            wxFAIL_MSG(wxT("unsupported charset"));
            // fall through

        case 850:
            vFontEncoding = wxFONTENCODING_CP1252;
            break;

        case 1250:
            vFontEncoding = wxFONTENCODING_CP1250;
            break;

        case 921:
            vFontEncoding = wxFONTENCODING_CP1257;
            break;

        case 866:
            vFontEncoding = wxFONTENCODING_CP1251;
            break;

        case 864:
            vFontEncoding = wxFONTENCODING_CP1256;
            break;

        case 869:
            vFontEncoding = wxFONTENCODING_CP1253;
            break;

        case 862:
            vFontEncoding = wxFONTENCODING_CP1255;
            break;

        case 857:
            vFontEncoding = wxFONTENCODING_CP1254;
            break;

        case 874:
            vFontEncoding = wxFONTENCODING_CP437;
            break;

        case 437:
            vFontEncoding = wxFONTENCODING_CP437;
            break;
    }

    return wxFont( nFontPoints
                  ,nFontFamily
                  ,nFontStyle
                  ,nFontWeight
                  ,bFontUnderline
                  ,sFontFace
                  ,vFontEncoding
                 );
} // end of wxCreateFontFromLogFont

int wxGpiStrcmp(
  char*                             s0
, char*                             s1
)
{   int                             l0;
    int                             l1;
    int                             l;
    int                             d;
    int                             d1;
    int                             i;
    int                             rc;

    rc = 0;
    if(s0 == NULL)
    {
        if(s1 == NULL)
            return 0;
        else
            return 32;
    }
    else if(s1 == NULL)
        return 32;

    l0 = strlen(s0);
    l1 = strlen(s1);
    l  = l0;
    if(l0 != l1)
    {
        rc++;
        if(l1 < l0)
            l = l1;
    }
    for(i=0;i<l;i++)
    {
        d = s0[i]-s1[i];
        if(!d)
            continue;
        d1 = toupper(s0[i]) - toupper(s1[i]);
        if(!d1)
            continue;
        rc += abs(d);
    }
    return rc;
}

