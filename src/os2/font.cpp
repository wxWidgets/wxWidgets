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
        Init(data.m_nPointSize, data.m_nFamily, data.m_nStyle, data.m_nWeight,
             data.m_bUnderlined, data.m_sFaceName, data.m_vEncoding);

        m_nFontId = data.m_nFontId;
    }

    wxFontRefData( int             nSize
                  ,int             nFamily
                  ,int             nStyle
                  ,int             nWeight
                  ,bool            bUnderlined
                  ,const wxString& sFaceName
                  ,wxFontEncoding  vEncoding
                 )
    {
        Init(nSize, nFamily, nStyle, nWeight, bUnderlined, sFaceName, vEncoding);
    }

    virtual ~wxFontRefData();

protected:
    // common part of all ctors
    void Init( int             nSize
              ,int             nFamily
              ,int             nStyle
              ,int             nWeight
              ,bool            bUnderlined
              ,const wxString& rsFaceName
              ,wxFontEncoding  vEncoding
             );

    //
    // If TRUE, the pointer to the actual font is temporary and SHOULD NOT BE
    // DELETED by destructor
    //
    bool                            m_bTemporary;
    int                             m_nFontId;

    //
    // Font characterstics
    //
    int                             m_nPointSize;
    int                             m_nFamily;
    int                             m_nStyle;
    int                             m_nWeight;
    bool                            m_bUnderlined;
    wxString                        m_sFaceName;
    wxFontEncoding                  m_vEncoding;

    //
    // Some PM specific stuff
    //
    WXHFONT                         m_hFont;
    PFONTMETRICS                    m_pFM;       // array of FONTMETRICS structs
    int                             m_nNumFonts; // number of fonts in array
    HPS                             m_hPS;       // PS handle this font belongs to
    FATTRS                          m_vFattrs;   // Current fattrs struct
    FACENAMEDESC                    m_vFname;    // Current facename struct
}; // end of CLASS wxFontRefData

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

void wxFontRefData::Init(
  int                               nPointSize
, int                               nFamily
, int                               nStyle
, int                               nWeight
, bool                              bUnderlined
, const wxString&                   rsFaceName
, wxFontEncoding                    vEncoding
)
{
    m_nStyle      = nStyle;
    m_nPointSize  = nPointSize;
    m_nFamily     = nFamily;
    m_nStyle      = nStyle;
    m_nWeight     = nWeight;
    m_bUnderlined = bUnderlined;
    m_sFaceName   = rsFaceName;
    m_vEncoding   = vEncoding;
    m_nFontId     = 0;
    m_bTemporary  = FALSE;

    m_hFont       = 0;
    m_pFM         = (PFONTMETRICS)NULL;
    m_hPS         = NULLHANDLE;
    m_nNumFonts   = 0;
} // end of wxFontRefData::Init

wxFontRefData::~wxFontRefData()
{
    if (m_pFM)
        delete [] m_pFM;
    m_pFM = (PFONTMETRICS)NULL;
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

void wxFont::Init()
{
    if ( wxTheFontList )
        wxTheFontList->Append(this);
}

// ----------------------------------------------------------------------------
// Constructor for a font. Note that the real construction is done
// in wxDC::SetFont, when information is available about scaling etc.
// ----------------------------------------------------------------------------
bool wxFont::Create(
  int                               nPointSize
, int                               nFamily
, int                               nStyle
, int                               nWeight
, bool                              bUnderlined
, const wxString&                   rsFaceName
, wxFontEncoding                    vEncoding
)
{
    UnRef();
    m_refData = new wxFontRefData( nPointSize
                                  ,nFamily
                                  ,nStyle
                                  ,nWeight
                                  ,bUnderlined
                                  ,rsFaceName
                                  ,vEncoding
                                 );
    RealizeResource();
    return TRUE;
} // end of wxFont::Create

wxFont::~wxFont()
{
    if (wxTheFontList)
        wxTheFontList->DeleteObject(this);
} // end of wxFont::~wxFont

// ----------------------------------------------------------------------------
// real implementation
// Boris' Kovalenko comments:
//   Because OS/2 fonts are associated with PS we can not create the font
//   here, but we may check that font definition is true
// ----------------------------------------------------------------------------

bool wxFont::RealizeResource()
{
    LONG                            lNumFonts = 0L;
    LONG                            lTemp = 0L;
    PFONTMETRICS                    pFM = NULL;
    ERRORID                         vError;

    if ( GetResourceHandle() )
    {
        // VZ: the old code returned FALSE in this case, but it doesn't seem
        //     to make sense because the font _was_ created
        wxLogDebug(wxT("Calling wxFont::RealizeResource() twice"));

       return TRUE;
    }

    LONG                            flId;
    bool                            bInternalPS = FALSE;  // if we have to create one

    //
    // Now cheking
    //
    flId = 1L;
    if (!M_FONTDATA->m_hPS)
    {
        M_FONTDATA->m_hPS = ::WinGetPS(HWND_DESKTOP);
        bInternalPS = TRUE;
    }

    if (M_FONTDATA->m_pFM)
    {
        delete [] M_FONTDATA->m_pFM;
        M_FONTDATA->m_pFM = NULL;
    }
    //
    // Determine the number of fonts.
    //
    lNumFonts = ::GpiQueryFonts( M_FONTDATA->m_hPS
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
    lTemp = ::GpiQueryFonts( M_FONTDATA->m_hPS
                            ,QF_PUBLIC
                            ,NULL
                            ,&lTemp
                            ,(LONG) sizeof(FONTMETRICS)
                            ,pFM
                           );
    SetFM( pFM
          ,(int)lNumFonts
         );

    wxString                        sVals;

    for (int i = 0; i < lNumFonts; i++)
    {
         sVals << "Face: " <<M_FONTDATA->m_pFM[i].szFacename
               << "Family: " <<M_FONTDATA->m_pFM[i].szFamilyname
               << " PointSize: " << M_FONTDATA->m_pFM[i].lEmHeight
               << " Height: " << M_FONTDATA->m_pFM[i].lXHeight
               ;
         sVals = "";
    }
    M_FONTDATA->m_vFattrs.usRecordLength = sizeof(FATTRS);
    M_FONTDATA->m_vFattrs.fsFontUse = FATTR_FONTUSE_OUTLINE |       // only outline fonts allowed
                                      FATTR_FONTUSE_TRANSFORMABLE;  // may be transformed
    M_FONTDATA->m_vFattrs.fsType = 0;
    M_FONTDATA->m_vFattrs.lMaxBaselineExt = M_FONTDATA->m_vFattrs.lAveCharWidth = 0;
    M_FONTDATA->m_vFattrs.idRegistry = 0;
    M_FONTDATA->m_vFattrs.lMatch = 0;

    M_FONTDATA->m_vFname.usSize = sizeof(FACENAMEDESC);
    M_FONTDATA->m_vFname.usWidthClass = FWIDTH_NORMAL;
    M_FONTDATA->m_vFname.usReserved = 0;
    M_FONTDATA->m_vFname.flOptions = 0;

    OS2SelectMatchingFontByName();

    long                            lNumLids = ::GpiQueryNumberSetIds(M_FONTDATA->m_hPS);
    long                            lGpiError;

    //
    // First we should generate unique id
    //
    if(lNumLids )
    {
        long                        alTypes[255];
        STR8                        azNames[255];
        long                        alIds[255];

        if(!::GpiQuerySetIds( M_FONTDATA->m_hPS
                             ,lNumLids
                             ,alTypes
                             ,azNames
                             ,alIds
                            ))
        {
            if (bInternalPS)
                ::WinReleasePS(M_FONTDATA->m_hPS);
            return 0;
        }

        for(unsigned long LCNum = 0; LCNum < lNumLids; LCNum++)
            if(alIds[LCNum] == flId)
               ++flId;
        if(flId > 254)  // wow, no id available!
        {
            if (bInternalPS)
               ::WinReleasePS(M_FONTDATA->m_hPS);
           return 0;
        }
    }

    //
    // Release and delete the current font
    //
    ::GpiSetCharSet(M_FONTDATA->m_hPS, LCID_DEFAULT);/* release the font before deleting */
    ::GpiDeleteSetId(M_FONTDATA->m_hPS, 1L);         /* delete the logical font          */

    //
    // Now build a facestring
    //
    char                            zFacename[128];
    strcpy(zFacename, M_FONTDATA->m_vFattrs.szFacename);

    if(::GpiQueryFaceString( M_FONTDATA->m_hPS
                            ,zFacename
                            ,&M_FONTDATA->m_vFname
                            ,FACESIZE
                            ,M_FONTDATA->m_vFattrs.szFacename
                           ) == GPI_ERROR)
    {
        vError = ::WinGetLastError(vHabmain);
    }

    strcpy(zFacename, M_FONTDATA->m_vFattrs.szFacename);

    if(::GpiCreateLogFont( M_FONTDATA->m_hPS
                          ,NULL
                          ,flId
                          ,&M_FONTDATA->m_vFattrs
                         ) != GPI_ERROR)
       M_FONTDATA->m_hFont = (WXHFONT)1;


    if (bInternalPS)
    {
        if(M_FONTDATA->m_hFont)
            ::GpiDeleteSetId( M_FONTDATA->m_hPS
                             ,flId
                            );

        ::WinReleasePS(M_FONTDATA->m_hPS);
    }
    else
        ::GpiSetCharSet(M_FONTDATA->m_hPS, flId); // sets font for presentation space
    if (!M_FONTDATA->m_hFont)
    {
        wxLogLastError("CreateFont");
    }
    M_FONTDATA->m_nFontId = flId;
    return(M_FONTDATA->m_hFont != 0);
} // end of wxFont::RealizeResource

bool wxFont::FreeResource(
  bool                              bForce
)
{
    if (GetResourceHandle())
    {
        M_FONTDATA->m_hFont = 0;
        ::GpiDeleteSetId( M_FONTDATA->m_hPS
                         ,M_FONTDATA->m_nFontId
                        );
        return TRUE;
    }
    return FALSE;
} // end of wxFont::FreeResource

WXHANDLE wxFont::GetResourceHandle()
{
    if (!M_FONTDATA)
        return 0;
    else
        return (WXHANDLE)M_FONTDATA->m_hFont;
} // end of wxFont::GetResourceHandle

bool wxFont::IsFree() const
{
    return (M_FONTDATA && (M_FONTDATA->m_hFont == 0));
} // end of wxFont::IsFree

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
} // end of wxFont::Unshare

// ----------------------------------------------------------------------------
// change font attribute: we recreate font when doing it
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(
  int                               nPointSize
)
{
    Unshare();

    M_FONTDATA->m_nPointSize = nPointSize;

    RealizeResource();
} // end of wxFont::SetPointSize

void wxFont::SetFamily(
  int                               nFamily
)
{
    Unshare();

    M_FONTDATA->m_nFamily = nFamily;

    RealizeResource();
} // end of wxFont::SetFamily

void wxFont::SetStyle(
  int                               nStyle
)
{
    Unshare();

    M_FONTDATA->m_nStyle = nStyle;

    RealizeResource();
} // end of wxFont::SetStyle

void wxFont::SetWeight(
  int                               nWeight
)
{
    Unshare();

    M_FONTDATA->m_nWeight = nWeight;

    RealizeResource();
} // end of wxFont::SetWeight

void wxFont::SetFaceName(
  const wxString&                   rsFaceName
)
{
    Unshare();

    M_FONTDATA->m_sFaceName = rsFaceName;

    RealizeResource();
} // end of wxFont::SetFaceName

void wxFont::SetUnderlined(
  bool                              bUnderlined
)
{
    Unshare();

    M_FONTDATA->m_bUnderlined = bUnderlined;

    RealizeResource();
} // end of wxFont::SetUnderlined

void wxFont::SetEncoding(
  wxFontEncoding                    vEncoding
)
{
    Unshare();

    M_FONTDATA->m_vEncoding = vEncoding;

    RealizeResource();
} // end of wxFont::SetEncoding

void wxFont::SetPS(
  HPS                               hPS
)
{
    Unshare();

    M_FONTDATA->m_hPS = hPS;

    RealizeResource();
} // end of wxFont::SetPS

void wxFont::SetFM(
  PFONTMETRICS                      pFM
, int                               nNumFonts
)
{
    //
    // Don't realize the font with this one
    //
    M_FONTDATA->m_pFM = pFM;
    M_FONTDATA->m_nNumFonts = nNumFonts;
} // end of wxFont::SetFM

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
    wxFontRefData*                  pTmp;

    pTmp = M_FONTDATA;
    if(pTmp)
      return pTmp->m_nPointSize;
    else
        return 10;
} // end of wxFont::GetPointSize

int wxFont::GetFamily() const
{
    return M_FONTDATA->m_nFamily;
} // end of wxFont::GetFamily

int wxFont::GetFontId() const
{
    return M_FONTDATA->m_nFontId;
} // end of wxFont::GetFontId

int wxFont::GetStyle() const
{
    return M_FONTDATA->m_nStyle;
} // end of wxFont::GetStyle

int wxFont::GetWeight() const
{
    return M_FONTDATA->m_nWeight;
}

bool wxFont::GetUnderlined() const
{
    return M_FONTDATA->m_bUnderlined;
} // end of wxFont::GetUnderlined

wxString wxFont::GetFaceName() const
{
    wxString                        sStr;

    if ( M_FONTDATA )
        sStr = M_FONTDATA->m_sFaceName ;
    return sStr;
} // end of wxFont::GetFaceName

wxFontEncoding wxFont::GetEncoding() const
{
    return M_FONTDATA->m_vEncoding;
} // end of wxFont::GetEncoding

HPS wxFont::GetPS() const
{
    return M_FONTDATA->m_hPS;
} // end of wxFont::GetPS

void wxFont::OS2SelectMatchingFontByName()
{
    int                             i;
    int                             nDiff0;
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

    switch (GetFamily())
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

    switch (GetWeight())
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
    M_FONTDATA->m_vFname.usWeightClass = usWeightClass;

    switch (GetStyle())
    {
        case wxITALIC:
        case wxSLANT:
            fsSelection = FM_SEL_ITALIC;
            M_FONTDATA->m_vFname.flOptions = FTYPE_ITALIC;
            break;

        default:
            wxFAIL_MSG(wxT("unknown font slant"));
            // fall through

        case wxNORMAL:
            fsSelection  = 0;
            break;
    }

    wxStrncpy(zFontFaceName, sFaceName.c_str(), WXSIZEOF(zFontFaceName));
    M_FONTDATA->m_nPointSize = GetPointSize();
    nIndex = 0;
    for(i = 0, nIs = 0; i < M_FONTDATA->m_nNumFonts; i++)
    {
        // Debug code
        int nPointSize = M_FONTDATA->m_nPointSize;
        int nEmHeight = 0;
        int nXHeight  = 0;
        anDiff[0] = wxGpiStrcmp(M_FONTDATA->m_pFM[i].szFamilyname, zFontFaceName);
        anDiff[1] = abs(M_FONTDATA->m_pFM[i].lEmHeight - M_FONTDATA->m_nPointSize);
        anDiff[2] = abs(M_FONTDATA->m_pFM[i].usWeightClass -  usWeightClass);
        anDiff[3] = abs((M_FONTDATA->m_pFM[i].fsSelection & 0x2f) -  fsSelection);
        if(anDiff[0] == 0)
        {
            nEmHeight = (int)M_FONTDATA->m_pFM[i].lEmHeight;
            nXHeight  =(int)M_FONTDATA->m_pFM[i].lXHeight;
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

    M_FONTDATA->m_vFattrs.usRecordLength = sizeof(FATTRS);                        // sets size of structure
    M_FONTDATA->m_vFattrs.fsSelection    = M_FONTDATA->m_pFM[nIndex].fsSelection; // uses default selection
    M_FONTDATA->m_vFattrs.lMatch         = M_FONTDATA->m_pFM[nIndex].lMatch;      // force match
    M_FONTDATA->m_vFattrs.idRegistry     = M_FONTDATA->m_pFM[nIndex].idRegistry;  // uses default registry
    M_FONTDATA->m_vFattrs.usCodePage     = M_FONTDATA->m_pFM[nIndex].usCodePage;  // code-page
    if(M_FONTDATA->m_pFM[nIndex].lMatch)
    {
        M_FONTDATA->m_vFattrs.lMaxBaselineExt = M_FONTDATA->m_pFM[nIndex].lMaxBaselineExt; // requested font height
        M_FONTDATA->m_vFattrs.lAveCharWidth   = M_FONTDATA->m_pFM[nIndex].lAveCharWidth ;  // requested font width
    }
    else
    {
        M_FONTDATA->m_vFattrs.lMaxBaselineExt = 0;
        M_FONTDATA->m_vFattrs.lAveCharWidth   = 0;
    }
    M_FONTDATA->m_vFattrs.fsType    = 0;// pfm->fsType;              /* uses default type       */
    M_FONTDATA->m_vFattrs.fsFontUse = 0;

    wxStrcpy(M_FONTDATA->m_vFattrs.szFacename, M_FONTDATA->m_pFM[nIndex].szFacename);
    // Debug
    strcpy(zFontFaceName, M_FONTDATA->m_pFM[nIndex].szFacename);
    strcpy(zFontFaceName, M_FONTDATA->m_vFattrs.szFacename);

    if(usWeightClass >= FWEIGHT_BOLD)
        M_FONTDATA->m_vFattrs.fsSelection |= FATTR_SEL_BOLD;
    if(GetUnderlined())
        M_FONTDATA->m_vFattrs.fsSelection |= FATTR_SEL_UNDERSCORE;
    if(fsSelection & FM_SEL_ITALIC)
        M_FONTDATA->m_vFattrs.fsSelection |= FATTR_SEL_ITALIC;
}



