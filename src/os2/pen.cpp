/////////////////////////////////////////////////////////////////////////////
// Name:        pen.cpp
// Purpose:     wxPen
// Author:      David Webster
// Modified by:
// Created:     10/10/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/pen.h"
#endif

#include "wx/os2/private.h"
#include "assert.h"

IMPLEMENT_DYNAMIC_CLASS(wxPen, wxGDIObject)

wxPenRefData::wxPenRefData()
{
    m_nStyle  = wxSOLID;
    m_nWidth  = 1;
    m_nJoin   = wxJOIN_ROUND ;
    m_nCap    = wxCAP_ROUND ;
    m_hPen    = 0L;
} // end of wxPenRefData::wxPenRefData

wxPenRefData::wxPenRefData(
  const wxPenRefData&               rData
)
{
    m_nStyle  = rData.m_nStyle;
    m_nWidth  = rData.m_nWidth;
    m_nJoin   = rData.m_nJoin;
    m_nCap    = rData.m_nCap;
    m_vColour = rData.m_vColour;
    m_hPen    = 0L;
} // end of wxPenRefData::wxPenRefData

wxPenRefData::~wxPenRefData()
{
} // end of wxPenRefData::~wxPenRefData

//
// Pens
//
wxPen::wxPen()
{
    if ( wxThePenList )
        wxThePenList->AddPen(this);
} // end of wxPen::wxPen

wxPen::~wxPen()
{
    if (wxThePenList)
        wxThePenList->RemovePen(this);
} // end of wxPen::wxPen

// Should implement Create
wxPen::wxPen(
  const wxColour&                   rColour
, int                               nWidth
, int                               nStyle
)
{
    m_refData = new wxPenRefData;

    M_PENDATA->m_vColour = rColour;
    M_PENDATA->m_nWidth  = nWidth;
    M_PENDATA->m_nStyle  = nStyle;
    M_PENDATA->m_nJoin   = wxJOIN_ROUND ;
    M_PENDATA->m_nCap    = wxCAP_ROUND ;
    M_PENDATA->m_hPen    = 0L;

    RealizeResource();

    if ( wxThePenList )
        wxThePenList->AddPen(this);
} // end of wxPen::wxPen

wxPen::wxPen(
  const wxBitmap&                   rStipple
, int                               nWidth
)
{
    m_refData = new wxPenRefData;

    M_PENDATA->m_vStipple = rStipple;
    M_PENDATA->m_nWidth   = nWidth;
    M_PENDATA->m_nStyle   = wxSTIPPLE;
    M_PENDATA->m_nJoin    = wxJOIN_ROUND ;
    M_PENDATA->m_nCap     = wxCAP_ROUND ;
    M_PENDATA->m_hPen     = 0;

    RealizeResource();

    if ( wxThePenList )
        wxThePenList->AddPen(this);
} // end of wxPen::wxPen

bool wxPen::RealizeResource()
{
    BOOL                            bOk;
    ERRORID                         vError;
    wxString                        sError;

    if (M_PENDATA && M_PENDATA->m_hPen == 0L)
    {
        SIZEL                   vSize = {0, 0};
        DEVOPENSTRUC            vDop = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
        HDC                     hDC = ::DevOpenDC( vHabmain
                                                  ,OD_MEMORY
                                                  ,"*"
                                                  ,5L
                                                  ,(PDEVOPENDATA)&vDop
                                                  ,NULLHANDLE
                                                 );
        M_PENDATA->m_hPen = (WXHPEN)::GpiCreatePS( vHabmain
                                                  ,hDC
                                                  ,&vSize
                                                  ,PU_PELS | GPIT_MICRO | GPIA_ASSOC
                                                 );
    }
    if (M_PENDATA)
    {
        //
        // Set the color table to RGB mode
        //
        if (!::GpiCreateLogColorTable( (HPS)M_PENDATA->m_hPen
                                      ,0L
                                      ,LCOLF_RGB
                                      ,0L
                                      ,0L
                                      ,NULL
                                     ))
        {
            vError = ::WinGetLastError(vHabmain);
            sError = wxPMErrorToStr(vError);
            wxLogError("Unable to set current color table to RGB mode. Error: %s\n", sError);
            return FALSE;
        }
        if (M_PENDATA->m_nStyle == wxTRANSPARENT)
        {
            return TRUE;
        }

        COLORREF                    vPmColour = 0L;
        USHORT                      uLineType = wx2os2PenStyle(M_PENDATA->m_nStyle);

        vPmColour = M_PENDATA->m_vColour.GetPixel();

        USHORT                      uJoin = 0L;

        switch(M_PENDATA->m_nJoin)
        {
            case wxJOIN_BEVEL:
                uJoin = LINEJOIN_BEVEL;
                break;

            case wxJOIN_MITER:
                uJoin = LINEJOIN_MITRE;
                break;

            case wxJOIN_ROUND:
                uJoin = LINEJOIN_ROUND;
                break;
        }

        USHORT                      uCap = 0L;

        switch(M_PENDATA->m_nCap)
        {
            case wxCAP_PROJECTING:
                uCap = LINEEND_SQUARE;
                break;

            case wxCAP_BUTT:
                uCap = LINEEND_FLAT;
                break;

            case wxCAP_ROUND:
                uCap = LINEEND_ROUND;
                break;
        }
        m_vLineBundle.lColor     = (LONG)vPmColour;
        m_vLineBundle.usMixMode  = FM_OVERPAINT;
        m_vLineBundle.fxWidth    = M_PENDATA->m_nWidth;
        m_vLineBundle.lGeomWidth = M_PENDATA->m_nWidth;
        m_vLineBundle.usType     = uLineType;
        m_vLineBundle.usEnd      = uCap;
        m_vLineBundle.usJoin     = uJoin;

        bOk = ::GpiSetAttrs( M_PENDATA->m_hPen
                            ,PRIM_LINE
                            ,LBB_COLOR | LBB_MIX_MODE | LBB_WIDTH | LBB_GEOM_WIDTH | LBB_TYPE | LBB_END | LBB_JOIN
                            ,0L
                            ,&m_vLineBundle
                           );
        if (!bOk)
        {
            vError = ::WinGetLastError(vHabmain);
            sError = wxPMErrorToStr(vError);
            wxLogError("Can't set Gpi attributes for a LINEBUNDLE. Error: %s\n", sError);
            return FALSE;
        }

        switch(M_PENDATA->m_nStyle)
        {
            case wxSTIPPLE:
                ::GpiSetBitmapId( M_PENDATA->m_hPen
                                 ,(USHORT)M_PENDATA->m_vStipple.GetHBITMAP()
                                 ,(USHORT)M_PENDATA->m_vStipple.GetId()
                                );
                ::GpiSetPatternSet( M_PENDATA->m_hPen
                                   ,(USHORT)M_PENDATA->m_vStipple.GetId()
                                  );
                break;

            case wxBDIAGONAL_HATCH:
                m_vAreaBundle.usSymbol = PATSYM_DIAG3;
                m_vAreaBundle.usSet = LCID_DEFAULT;
                break;

            case wxCROSSDIAG_HATCH:
                m_vAreaBundle.usSymbol = PATSYM_DIAGHATCH;
                m_vAreaBundle.usSet = LCID_DEFAULT;
                break;

            case wxFDIAGONAL_HATCH:
                m_vAreaBundle.usSymbol = PATSYM_DIAG1;
                m_vAreaBundle.usSet = LCID_DEFAULT;
                break;

            case wxCROSS_HATCH:
                m_vAreaBundle.usSymbol = PATSYM_HATCH;
                m_vAreaBundle.usSet = LCID_DEFAULT;
                break;

            case wxHORIZONTAL_HATCH:
                m_vAreaBundle.usSymbol = PATSYM_HORIZ;
                m_vAreaBundle.usSet = LCID_DEFAULT;
                break;

            case wxVERTICAL_HATCH:
                m_vAreaBundle.usSymbol = PATSYM_VERT;
                m_vAreaBundle.usSet = LCID_DEFAULT;
                break;

            default:
                m_vAreaBundle.usSymbol = PATSYM_SOLID;
                m_vAreaBundle.usSet = LCID_DEFAULT;
                break;
        }

        m_vAreaBundle.lColor        = vPmColour;
        m_vAreaBundle.lBackColor    = RGB_WHITE;
        m_vAreaBundle.usMixMode     = FM_OVERPAINT;
        m_vAreaBundle.usBackMixMode = BM_OVERPAINT;

        bOk = ::GpiSetAttrs( M_PENDATA->m_hPen
                            ,PRIM_AREA
                            ,ABB_COLOR | ABB_BACK_COLOR | ABB_MIX_MODE | ABB_BACK_MIX_MODE |
                             ABB_SET | ABB_SYMBOL
                            ,ABB_REF_POINT
                            ,&m_vAreaBundle
                           );
        if (!bOk)
        {
            vError = ::WinGetLastError(vHabmain);
            sError = wxPMErrorToStr(vError);
            wxLogError("Can't set Gpi attributes for an AREABUNDLE. Error: %s\n", sError);
        }
        return bOk;
    }
    return FALSE;
} // end of wxPen::RealizeResource

WXHANDLE wxPen::GetResourceHandle()
{
    if (!M_PENDATA)
        return 0;
    else
        return (WXHANDLE)M_PENDATA->m_hPen;
} // end of wxPen::GetResourceHandle

bool wxPen::FreeResource(
  bool                              bForce
)
{
    if (M_PENDATA && (M_PENDATA->m_hPen != 0))
    {
        M_PENDATA->m_hPen = 0;
        return TRUE;
    }
    else return FALSE;
} // end of wxPen::FreeResource

bool wxPen::IsFree() const
{
    return (M_PENDATA && M_PENDATA->m_hPen == 0);
}

void wxPen::Unshare()
{
    // Don't change shared data
    if (!m_refData)
    {
        m_refData = new wxPenRefData();
    }
    else
    {
        wxPenRefData* ref = new wxPenRefData(*(wxPenRefData*)m_refData);
        UnRef();
        m_refData = ref;
    }
} // end of wxPen::Unshare

void wxPen::SetColour(
  const wxColour&                   rColour
)
{
    Unshare();
    M_PENDATA->m_vColour = rColour;
    RealizeResource();
} // end of wxPen::SetColour

void wxPen::SetColour(
  unsigned char                     cRed
, unsigned char                     cGreen
, unsigned char                     cBlue
)
{
    Unshare();
    M_PENDATA->m_vColour.Set(cRed, cGreen, cBlue);
    RealizeResource();
} // end of wxPen::SetColour

void wxPen::SetPS(
  HPS                               hPS
)
{
    Unshare();
    if (M_PENDATA->m_hPen)
        ::GpiDestroyPS(M_PENDATA->m_hPen);
    M_PENDATA->m_hPen = hPS;
    RealizeResource();
} // end of WxWinGdi_CPen::SetPS

void wxPen::SetWidth(
  int                               nWidth
)
{
    Unshare();
    M_PENDATA->m_nWidth = nWidth;
    RealizeResource();
} // end of wxPen::SetWidth

void wxPen::SetStyle(
  int                               nStyle
)
{
    Unshare();
    M_PENDATA->m_nStyle = nStyle;
    RealizeResource();
} // end of wxPen::SetStyle

void wxPen::SetStipple(
  const wxBitmap&                   rStipple
)
{
    Unshare();
    M_PENDATA->m_vStipple = rStipple;
    M_PENDATA->m_nStyle = wxSTIPPLE;
    RealizeResource();
} // end of wxPen::SetStipple

void wxPen::SetDashes(
  int                               nNbDashes
, const wxDash*                     pDash
)
{
    //
    // Does nothing under OS/2
    //
} // end of wxPen::SetDashes

void wxPen::SetJoin(
  int                               nJoin
)
{
    Unshare();
    M_PENDATA->m_nJoin = nJoin;
    RealizeResource();
} // end of wxPen::SetJoin

void wxPen::SetCap(
  int                               nCap
)
{
    Unshare();
    M_PENDATA->m_nCap = nCap;
    RealizeResource();
} // end of wxPen::SetCap

int wx2os2PenStyle(
  int                               nWxStyle
)
{
    int                             nPMStyle = 0;

    switch (nWxStyle)
    {
        case wxDOT:
            nPMStyle = LINETYPE_DOT;
            break;

        case wxDOT_DASH:
            nPMStyle = LINETYPE_DASHDOT;
            break;

        case wxSHORT_DASH:
            nPMStyle = LINETYPE_SHORTDASH;
            break;

        case wxLONG_DASH:
            nPMStyle = LINETYPE_LONGDASH;
            break;

        case wxTRANSPARENT:
            nPMStyle = LINETYPE_INVISIBLE;
            break;

        case wxUSER_DASH:
            nPMStyle = LINETYPE_DASHDOUBLEDOT; // We must make a choice... This is mine!
            break;

        case wxSOLID:
        default:
            nPMStyle = LINETYPE_SOLID;
            break;
    }
    return nPMStyle;
} // end of wx2os2PenStyle


