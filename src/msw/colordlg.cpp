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
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/dialog.h"
#include "wx/msgdlg.h"
#endif

#include <windows.h>

#ifndef __WIN32__
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
  dialogParent = NULL;
}

wxColourDialog::wxColourDialog(wxWindow *parent, wxColourData *data)
{
  Create(parent, data);
}

bool wxColourDialog::Create(wxWindow *parent, wxColourData *data)
{
  dialogParent = parent;
  
  if (data)
    colourData = *data;
  return TRUE;
}

int wxColourDialog::ShowModal(void)
{
    CHOOSECOLOR chooseColorStruct;
    COLORREF custColours[16];
    memset(&chooseColorStruct, 0, sizeof(CHOOSECOLOR));

    int i;
    for (i = 0; i < 16; i++)
      custColours[i] = RGB(colourData.custColours[i].Red(), colourData.custColours[i].Green(), colourData.custColours[i].Blue());

    chooseColorStruct.lStructSize = sizeof(CHOOSECOLOR);
    chooseColorStruct.hwndOwner = (HWND) (dialogParent ? (HWND) dialogParent->GetHWND() : NULL);
    chooseColorStruct.rgbResult = RGB(colourData.dataColour.Red(), colourData.dataColour.Green(), colourData.dataColour.Blue());
    chooseColorStruct.lpCustColors = custColours;

    chooseColorStruct.Flags = CC_RGBINIT;

    if (!colourData.GetChooseFull())
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
      colourData.custColours[i].Set(GetRValue(custColours[i]), GetGValue(custColours[i]),
         GetBValue(custColours[i]));
    }

    colourData.dataColour.Set(GetRValue(chooseColorStruct.rgbResult), GetGValue(chooseColorStruct.rgbResult),
     GetBValue(chooseColorStruct.rgbResult));

    return success ? wxID_OK : wxID_CANCEL;
}

