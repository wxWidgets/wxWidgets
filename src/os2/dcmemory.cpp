/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/utils.h"
#endif

#include "wx/os2/private.h"

#include "wx/dcmemory.h"

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC)

/////////////////////////////////////////////////////////////////////////////
// Memory DC
/////////////////////////////////////////////////////////////////////////////

wxMemoryDC::wxMemoryDC(void)
{
    ERRORID                         vError;
    wxString                        sError;
    HDC                             hDC;
    HPS                             hPS;
    DEVOPENSTRUC                    vDOP = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    SIZEL                           vSize = {0, 0};

    //
    // Create a memory device context
    //
    hDC = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDOP, NULLHANDLE);
    if (hDC != DEV_ERROR)
    {
        hPS = ::GpiCreatePS(vHabmain, hDC, &vSize, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
        if (hPS != GPI_ERROR)
        {
            m_hPS = hPS;
            m_hDC = hDC;
            m_ok = TRUE;
            m_bOwnsDC = TRUE;
            SetBrush(*wxWHITE_BRUSH);
            SetPen(*wxBLACK_PEN);
            if (!::GpiCreateLogColorTable( m_hPS
                                          ,0L
                                          ,LCOLF_CONSECRGB
                                          ,0L
                                          ,(LONG)wxTheColourDatabase->m_nSize
                                          ,(PLONG)wxTheColourDatabase->m_palTable
                                         ))
            {
                vError = ::WinGetLastError(vHabmain);
                sError = wxPMErrorToStr(vError);
                wxLogError("Unable to set current color table. Error: %s\n", sError);
            }
            //
            // Set the color table to RGB mode
            //
            if (!::GpiCreateLogColorTable( m_hPS
                                          ,0L
                                          ,LCOLF_RGB
                                          ,0L
                                          ,0L
                                          ,NULL
                                         ))
            {
                vError = ::WinGetLastError(vHabmain);
                sError = wxPMErrorToStr(vError);
                wxLogError("Unable to set current color table. Error: %s\n", sError);
            }
        }
        else
        {
            m_hPS = NULLHANDLE;
            m_hDC = NULLHANDLE;
            m_ok  = FALSE;
            m_bOwnsDC = FALSE;
        }
    }
    else
    {
        m_hPS = NULLHANDLE;
        m_hDC = NULLHANDLE;
        m_ok  = FALSE;
        m_bOwnsDC = FALSE;
    }
} // end of wxMemoryDC::wxMemoryDC

wxMemoryDC::wxMemoryDC(
  wxDC*                             pOldDC
)
{
    HDC                             hDC;
    HPS                             hPS;
    DEVOPENSTRUC                    vDOP = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    SIZEL                           vSize = {0, 0};

    pOldDC->BeginDrawing();

    //
    // Create a memory device context
    //
    hDC = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDOP, GetHdcOf(*pOldDC));
    if (hDC != DEV_ERROR)
    {
        hPS = ::GpiCreatePS(vHabmain, hDC, &vSize, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
        if (hPS != GPI_ERROR)
        {
            m_hPS = hPS;
            m_hDC = hDC;
            m_ok = TRUE;
            m_bOwnsDC = TRUE;
            pOldDC->EndDrawing();
            SetBrush(*wxWHITE_BRUSH);
            SetPen(*wxBLACK_PEN);
        }
        else
        {
            pOldDC->EndDrawing();
            m_hPS = NULLHANDLE;
            m_hDC = NULLHANDLE;
            m_ok  = FALSE;
            m_bOwnsDC = FALSE;
        }
    }
    else
    {
        pOldDC->EndDrawing();
        m_hPS = NULLHANDLE;
        m_hDC = NULLHANDLE;
        m_ok  = FALSE;
        m_bOwnsDC = FALSE;
    }
} // end of wxMemoryDC::wxMemoryDC

wxMemoryDC::~wxMemoryDC()
{
    if (m_hPS != NULLHANDLE)
        ::GpiDestroyPS(m_hPS);
    if (m_hDC != NULLHANDLE)
        ::DevCloseDC(m_hDC);
} // end of wxMemoryDC::~wxMemoryDC

void wxMemoryDC::SelectObject(
  const wxBitmap&                   rBitmap
)
{
    //
    // Select old bitmap out of the device context
    //
    if (m_hOldBitmap)
    {
        ::GpiSetBitmap(m_hPS, NULLHANDLE);
        if (m_vSelectedBitmap.Ok())
        {
            m_vSelectedBitmap.SetSelectedInto(NULL);
            m_vSelectedBitmap = wxNullBitmap;
        }
    }

    //
    // Check for whether the bitmap is already selected into a device context
    //
    wxCHECK_RET( !rBitmap.GetSelectedInto() ||
                 (rBitmap.GetSelectedInto() == this),
                 wxT("Bitmap is selected in another wxMemoryDC, delete the first wxMemoryDC or use SelectObject(NULL)") );

    m_vSelectedBitmap = rBitmap;

    WXHBITMAP                       hBmp = m_vSelectedBitmap.GetHBITMAP();

    if (!hBmp)
        return;

    m_vSelectedBitmap.SetSelectedInto(this);
    hBmp = (WXHBITMAP)::GpiSetBitmap(m_hPS, (HBITMAP)hBmp);

    if (hBmp == HBM_ERROR)
    {
        wxLogLastError(wxT("SelectObject(memDC, bitmap)"));
        wxFAIL_MSG(wxT("Couldn't select a bitmap into wxMemoryDC"));
    }
    else if (!m_hOldBitmap)
    {
        m_hOldBitmap = hBmp;
    }
} // end of wxMemoryDC::SelectObject

void wxMemoryDC::DoGetSize(
  int*                              pWidth
, int*                              pHeight
) const
{
    if (!m_vSelectedBitmap.Ok())
    {
        *pWidth  = 0;
        *pHeight = 0;
        return;
    }
    *pWidth = m_vSelectedBitmap.GetWidth();
    *pHeight = m_vSelectedBitmap.GetHeight();
} // end of wxMemoryDC::DoGetSize


