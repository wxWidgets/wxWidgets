/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:     wxColour class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gdicmn.h"
#define INCL_GPI
#define INCL_PM
#include<os2.h>

IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)

// Colour

wxColour::wxColour ()
{
    m_bIsInit = FALSE;
    m_vPixel = 0;
    m_cRed = m_cBlue = m_cGreen = 0;
} // end of wxColour::wxColour

wxColour::wxColour (
  unsigned char                     cRed
, unsigned char                     cGreen
, unsigned char                     cBlue
)
{
    m_cRed    = cRed;
    m_cGreen  = cGreen;
    m_cBlue   = cBlue;
    m_bIsInit = TRUE;
    m_vPixel  = OS2RGB (m_cRed, m_cGreen, m_cBlue);
} // end of wxColour::wxColour

wxColour::wxColour (
  const wxColour&                   rCol
)
{
    m_cRed    = rCol.m_cRed;
    m_cGreen  = rCol.m_cGreen;
    m_cBlue   = rCol.m_cBlue;
    m_bIsInit = rCol.m_bIsInit;
    m_vPixel  = rCol.m_vPixel;
} // end of wxColour::wxColour

wxColour& wxColour::operator =(
  const wxColour&                   rCol
)
{
    m_cRed    = rCol.m_cRed;
    m_cGreen  = rCol.m_cGreen;
    m_cBlue   = rCol.m_cBlue;
    m_bIsInit = rCol.m_bIsInit;
    m_vPixel  = rCol.m_vPixel;
    return *this;
} // end of wxColour& wxColour::operator =

void wxColour::InitFromName(
  const wxString&                   sCol
)
{
    wxColour*                       pTheColour = wxTheColourDatabase->FindColour(sCol);

    if (pTheColour)
    {
        m_cRed    = pTheColour->Red();
        m_cGreen  = pTheColour->Green();
        m_cBlue   = pTheColour->Blue();
        m_bIsInit = TRUE;
    }
    else
    {
        m_cRed = 0;
        m_cGreen = 0;
        m_cBlue = 0;
        m_bIsInit = FALSE;
    }
    m_vPixel = OS2RGB (m_cRed, m_cGreen, m_cBlue);
} // end of wxColour::InitFromName

wxColour::~wxColour ()
{
} // end of wxColour::~wxColour

void wxColour::Set (
  unsigned char                     cRed
, unsigned char                     cGreen
, unsigned char                     cBlue
)
{
    m_cRed    = cRed;
    m_cGreen  = cGreen;
    m_cBlue   = cBlue;
    m_bIsInit = TRUE;
    m_vPixel  = OS2RGB (m_cRed, m_cGreen, m_cBlue);
} // end of wxColour::Set

//
// Obsolete
//
#if WXWIN_COMPATIBILITY
void wxColour::Get (
  unsigned char*                   pRed
, unsigned char*                   pGreen
, unsigned char*                   pBlue
) const
{
    *Red   = m_cRed;
    *Green = m_cGreen;
    *Blue  = m_cBlue;
} // end of wxColour::Get
#endif

