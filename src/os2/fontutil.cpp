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

wxFontEncoding wxGetFontEncFromCharSet(
  int                               nCharSet
)
{
    wxFontEncoding                  eFontEncoding;

    switch (nCharSet)
    {
        default:
        case 1250:
            eFontEncoding = wxFONTENCODING_CP1250;
            break;

        case 1252:
            eFontEncoding = wxFONTENCODING_CP1252;
            break;

        case 921:
            eFontEncoding = wxFONTENCODING_ISO8859_4;
            break;

        case 1251:
            eFontEncoding = wxFONTENCODING_CP1251;
            break;

        case 864:
            eFontEncoding = wxFONTENCODING_ISO8859_6;
            break;

        case 869:
            eFontEncoding = wxFONTENCODING_ISO8859_7;
            break;

        case 862:
            eFontEncoding = wxFONTENCODING_ISO8859_8;
            break;

        case 857:
            eFontEncoding = wxFONTENCODING_ISO8859_9;
            break;

        case 874:
            eFontEncoding = wxFONTENCODING_ISO8859_11;
            break;

        case 437:
            eFontEncoding = wxFONTENCODING_CP437;
            break;
    }
    return eFontEncoding;
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
  LOGFONT*                          pFattrs  // OS2 GPI FATTRS
, PFACENAMEDESC                     pFaceName
, HPS                               hPS
, long*                             pflId
, wxString&                         sFaceName
, wxFont*                           pFont
)
{
    LONG                            lNumFonts = 0L;       // For system font count
    ERRORID                         vError;               // For logging API errors
    LONG                            lTemp;
    bool                            bInternalPS = FALSE;  // if we have to create one
    PFONTMETRICS                    pFM = NULL;

    //
    // Initial house cleaning to free data buffers and ensure we have a
    // functional PS to work with
    //
    if (!hPS)
    {
        hPS = ::WinGetPS(HWND_DESKTOP);
        bInternalPS = TRUE;
    }

    //
    // Determine the number of fonts.
    //
    lNumFonts = ::GpiQueryFonts( hPS
                                ,QF_PUBLIC
                                ,NULL
                                ,&lTemp
                                ,(LONG) sizeof(FONTMETRICS)
                                ,NULL
                               );

    //
    // Allocate space for the font metrics.
    //
    pFM = new FONTMETRICS[lNumFonts + 1];

    //
    // Retrieve the font metrics.
    //
    lTemp = lNumFonts;
    lTemp = ::GpiQueryFonts( hPS
                            ,QF_PUBLIC
                            ,NULL
                            ,&lTemp
                            ,(LONG) sizeof(FONTMETRICS)
                            ,pFM
                           );
    pFont->SetFM( pFM
                 ,(int)lNumFonts
                );

    wxString                        sVals;

    for (int i = 0; i < lNumFonts; i++)
    {
         sVals << "Face: " << pFM[i].szFacename
               << "Family: " << pFM[i].szFamilyname
               << " PointSize: " << pFM[i].lEmHeight
               << " Height: " << pFM[i].lXHeight
               ;
         sVals = "";
    }

    //
    // Initialize FATTR and FACENAMEDESC
    //
    pFattrs->usRecordLength = sizeof(FATTRS);
    pFattrs->fsFontUse = FATTR_FONTUSE_OUTLINE |       // only outline fonts allowed
                          FATTR_FONTUSE_TRANSFORMABLE;  // may be transformed
    pFattrs->fsType = 0;
    pFattrs->lMaxBaselineExt = pFattrs->lAveCharWidth = 0;
    pFattrs->idRegistry = 0;
    pFattrs->lMatch = 0;

    pFaceName->usSize = sizeof(FACENAMEDESC);
    pFaceName->usWidthClass = FWIDTH_NORMAL;
    pFaceName->usReserved = 0;
    pFaceName->flOptions = 0;

    //
    // This does the actual selection of fonts
    //
    wxOS2SelectMatchingFontByName( pFattrs
                                  ,pFaceName
                                  ,pFM
                                  ,(int)lNumFonts
                                  ,pFont
                                 );
    //
    // We should now have the correct FATTRS set with the selected
    // font, so now we need to generate an ID
    //
    long                            lNumLids = ::GpiQueryNumberSetIds(hPS);
    long                            lGpiError;

    if(lNumLids )
    {
        long                        alTypes[255];
        STR8                        azNames[255];
        long                        alIds[255];

        if(!::GpiQuerySetIds( hPS
                             ,lNumLids
                             ,alTypes
                             ,azNames
                             ,alIds
                            ))
        {
            if (bInternalPS)
                ::WinReleasePS(hPS);
            return;
        }

        for(unsigned long LCNum = 0; LCNum < lNumLids; LCNum++)
            if(alIds[LCNum] == *pflId)
               ++*pflId;
        if(*pflId > 254)  // wow, no id available!
        {
            if (bInternalPS)
               ::WinReleasePS(hPS);
           return;
        }
    }

    //
    // Release and delete the current font
    //
    ::GpiSetCharSet(hPS, LCID_DEFAULT);/* release the font before deleting */
    ::GpiDeleteSetId(hPS, 1L);         /* delete the logical font          */

    //
    // Now build a facestring
    //
    char                            zFacename[128];

    strcpy(zFacename, pFattrs->szFacename);

    if(::GpiQueryFaceString( hPS
                            ,zFacename
                            ,pFaceName
                            ,FACESIZE
                            ,pFattrs->szFacename
                           ) == GPI_ERROR)
    {
        vError = ::WinGetLastError(vHabmain);
    }
    sFaceName = zFacename;

    //
    // That's it, we now have everything we need to actually create the font
    //
} // end of wxFillLogFont

void wxOS2SelectMatchingFontByName(
  PFATTRS                           pFattrs
, PFACENAMEDESC                     pFaceName
, PFONTMETRICS                      pFM
, int                               nNumFonts
, const wxFont*                     pFont
)
{
    int                             i;
    int                             nDiff0;
    int                             nPointSize;
    int                             nDiff;
    int                             nIs;
    int                             nIndex;
    int                             nMinDiff;
    int                             nMinDiff0;
    int                             nApirc;
    int                             anDiff[16];
    int                             anMinDiff[16];
    STR8                            zFn;
    char                            zFontFaceName[FACESIZE];
    wxString                        sFaceName;
    USHORT                          usWeightClass;
    int                             fsSelection = 0;

    nMinDiff0 = 0xf000;
    for(i = 0;i < 16; i++)
        anMinDiff[i] = nMinDiff0;

    switch (pFont->GetFamily())
    {
        case wxSCRIPT:
            sFaceName = wxT("Script");
            break;

        case wxDECORATIVE:
        case wxROMAN:
            sFaceName = wxT("Times New Roman");
            break;

        case wxTELETYPE:
        case wxMODERN:
            sFaceName = wxT("Courier") ;
            break;

        case wxSWISS:
            sFaceName = wxT("WarpSans") ;
            break;

        case wxDEFAULT:
        default:
            sFaceName = wxT("Helv") ;
    }

    switch (pFont->GetWeight())
    {
        default:
            wxFAIL_MSG(_T("unknown font weight"));
            // fall through
            usWeightClass = FWEIGHT_DONT_CARE;
            break;

        case wxNORMAL:
            usWeightClass = FWEIGHT_NORMAL;
            break;

        case wxLIGHT:
            usWeightClass = FWEIGHT_LIGHT;
            break;

        case wxBOLD:
            usWeightClass = FWEIGHT_BOLD;
            break;

         case wxFONTWEIGHT_MAX:
            usWeightClass = FWEIGHT_ULTRA_BOLD;
            break;
    }
    pFaceName->usWeightClass = usWeightClass;

    switch (pFont->GetStyle())
    {
        case wxITALIC:
        case wxSLANT:
            fsSelection = FM_SEL_ITALIC;
            pFaceName->flOptions = FTYPE_ITALIC;
            break;

        default:
            wxFAIL_MSG(wxT("unknown font slant"));
            // fall through

        case wxNORMAL:
            fsSelection  = 0;
            break;
    }

    wxStrncpy(zFontFaceName, sFaceName.c_str(), WXSIZEOF(zFontFaceName));
    nPointSize = pFont->GetPointSize();

    //
    // Matching logic to find the right FM struct
    //
    nIndex = 0;
    for(i = 0, nIs = 0; i < nNumFonts; i++)
    {
        int                         nEmHeight = 0;
        int                         nXHeight = 0;

        anDiff[0] = wxGpiStrcmp(pFM[i].szFamilyname, zFontFaceName);
        anDiff[1] = abs(pFM[i].lEmHeight - nPointSize);
        anDiff[2] = abs(pFM[i].usWeightClass -  usWeightClass);
        anDiff[3] = abs((pFM[i].fsSelection & 0x2f) -  fsSelection);
        if(anDiff[0] == 0)
        {
            nEmHeight = (int)pFM[i].lEmHeight;
            nXHeight  =(int)pFM[i].lXHeight;
            if( (nIs & 0x01) == 0)
            {
                nIs = 1;
                nIndex = i;
                anMinDiff[1] = anDiff[1];
                anMinDiff[2] = anDiff[2];
                anMinDiff[3] = anDiff[3];
            }
            else if(anDiff[3] < anMinDiff[3])
            {
                nIndex = i;
                anMinDiff[3] = anDiff[3];
            }
            else if(anDiff[2] < anMinDiff[2])
            {
                nIndex = i;
                anMinDiff[2] = anDiff[2];
            }
            else if(anDiff[1] < anMinDiff[1])
            {
                nIndex = i;
                anMinDiff[1] = anDiff[1];
            }
            anMinDiff[0] = 0;
        }
        else if(anDiff[0] < anMinDiff[0])
        {
              nIs = 2;
              nIndex = i;
              anMinDiff[3] = anDiff[3];
              anMinDiff[2] = anDiff[2];
              anMinDiff[1] = anDiff[1];
              anMinDiff[0] = anDiff[0];
        }
        else if(anDiff[0] == anMinDiff[0])
        {
            if(anDiff[3] < anMinDiff[3])
            {
                nIndex = i;
                anMinDiff[3] = anDiff[3];
                nIs = 2;
            }
            else if(anDiff[2] < anMinDiff[2])
            {
                nIndex = i;
                anMinDiff[2] = anDiff[2];
                nIs = 2;
            }
            else if(anDiff[1] < anMinDiff[1])
            {
                nIndex = i;
                anMinDiff[1] = anDiff[1];
                nIs = 2;
            }
        }
    }

    //
    // Fill in the FATTRS with the best match from FONTMETRICS
    //
    pFattrs->usRecordLength = sizeof(FATTRS);            // sets size of structure
    pFattrs->fsSelection    = pFM[nIndex].fsSelection; // uses default selection
    pFattrs->lMatch         = pFM[nIndex].lMatch;      // force match
    pFattrs->idRegistry     = pFM[nIndex].idRegistry;  // uses default registry
    pFattrs->usCodePage     = pFM[nIndex].usCodePage;  // code-page
    if(pFM[nIndex].lMatch)
    {
        pFattrs->lMaxBaselineExt = pFM[nIndex].lMaxBaselineExt; // requested font height
        pFattrs->lAveCharWidth   = pFM[nIndex].lAveCharWidth ;  // requested font width
    }
    else
    {
        pFattrs->lMaxBaselineExt = 0;
        pFattrs->lAveCharWidth   = 0;
    }
    pFattrs->fsType    = 0;// pfm->fsType;              /* uses default type       */
    pFattrs->fsFontUse = 0;

    wxStrcpy(pFattrs->szFacename, pFM[nIndex].szFacename);
    // Debug
    strcpy(zFontFaceName, pFM[nIndex].szFacename);
    strcpy(zFontFaceName, pFattrs->szFacename);

    if(usWeightClass >= FWEIGHT_BOLD)
        pFattrs->fsSelection |= FATTR_SEL_BOLD;
    if(pFont->GetUnderlined())
        pFattrs->fsSelection |= FATTR_SEL_UNDERSCORE;
    if(fsSelection & FM_SEL_ITALIC)
        pFattrs->fsSelection |= FATTR_SEL_ITALIC;
} // end of wxOS2SelectMatchingFontByName

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

