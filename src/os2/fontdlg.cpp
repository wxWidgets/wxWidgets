/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.cpp
// Purpose:     wxFontDialog class. NOTE: you can use the generic class
//              if you wish, instead of implementing this.
// Author:      David Webster
// Modified by:
// Created:     10/06/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#endif

#include "wx/fontdlg.h"

#define INCL_PM
#include <os2.h>

#include "wx/os2/private.h"
#include "wx/cmndata.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)

/*
 * wxFontDialog
 */
wxFontDialog::wxFontDialog()
{
    m_dialogParent = NULL;
}

wxFontDialog::wxFontDialog(wxWindow *parent, wxFontData *data)
{
    Create(parent, data);
}

bool wxFontDialog::Create(wxWindow *parent, wxFontData *data)
{
    m_dialogParent = parent;

    if (data)
        m_fontData = *data;
    return TRUE;
}

int wxFontDialog::ShowModal()
{
    // TODO: show (maybe create) the dialog
    return wxID_CANCEL;
}

