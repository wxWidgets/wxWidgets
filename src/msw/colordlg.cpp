/////////////////////////////////////////////////////////////////////////////
// Name:        colordlg.cpp
// Purpose:     wxColourDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "colordlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/defs.h"
#include "wx/bitmap.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/colour.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/dialog.h"
#include "wx/msgdlg.h"
#endif

#include <windows.h>

#if !defined(__WIN32__) || defined(__SALFORDC__) || defined(__WXWINE__)
#include <commdlg.h>
#endif

#include "wx/msw/private.h"
#include "wx/colordlg.h"
#include "wx/cmndata.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog)
#endif

/*
 * wxColourDialog
 */

wxColourDialog::wxColourDialog(void)
{
  m_dialogParent = NULL;
}

wxColourDialog::wxColourDialog(wxWindow *parent, wxColourData *data)
{
  Create(parent, data);
}

bool wxColourDialog::Create(wxWindow *parent, wxColourData *data)
{
  m_dialogParent = parent;

  if (data)
    m_colourData = *data;
  return TRUE;
}

int wxColourDialog::ShowModal(void)
{
    CHOOSECOLOR chooseColorStruct;
    COLORREF custColours[16];
    memset(&chooseColorStruct, 0, sizeof(CHOOSECOLOR));

    int i;
    for (i = 0; i < 16; i++)
      custColours[i] = RGB(m_colourData.custColours[i].Red(), m_colourData.custColours[i].Green(), m_colourData.custColours[i].Blue());

    chooseColorStruct.lStructSize = sizeof(CHOOSECOLOR);
    chooseColorStruct.hwndOwner = (HWND) (m_dialogParent ? (HWND) m_dialogParent->GetHWND() : (HWND) NULL);
    chooseColorStruct.rgbResult = RGB(m_colourData.dataColour.Red(), m_colourData.dataColour.Green(), m_colourData.dataColour.Blue());
    chooseColorStruct.lpCustColors = custColours;

    chooseColorStruct.Flags = CC_RGBINIT;

    if (!m_colourData.GetChooseFull())
      chooseColorStruct.Flags |= CC_PREVENTFULLOPEN;

    // Do the modal dialog
    bool success = (ChooseColor(&(chooseColorStruct)) != 0);

    // Try to highlight the correct window (the parent)
    HWND hWndParent = 0;
    if (GetParent())
    {
      hWndParent = (HWND) GetParent()->GetHWND();
      if (hWndParent)
        ::BringWindowToTop(hWndParent);
    }


    // Restore values
    for (i = 0; i < 16; i++)
    {
      m_colourData.custColours[i].Set(GetRValue(custColours[i]), GetGValue(custColours[i]),
         GetBValue(custColours[i]));
    }

    m_colourData.dataColour.Set(GetRValue(chooseColorStruct.rgbResult), GetGValue(chooseColorStruct.rgbResult),
     GetBValue(chooseColorStruct.rgbResult));

    return success ? wxID_OK : wxID_CANCEL;
}

