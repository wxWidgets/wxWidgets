/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.cpp
// Purpose:     wxFontDialog class for carbon 10.2+.
// Author:      Ryan Norton
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "fontdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/cmndata.h"
#endif

#include "wx/fontdlg.h"
    

#if !USE_NATIVE_FONT_DIALOG_FOR_MACOSX

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)
#endif

#include "wx/mac/private.h"

// ---------------------------------------------------------------------------
// wxFontDialog stub for mac OS's without a native font dialog
// ---------------------------------------------------------------------------

wxFontDialog::wxFontDialog()
{
    m_dialogParent = NULL;
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData&  data)
{
    Create(parent, data);
}

wxFontDialog::~wxFontDialog()
{
    // empty
}

void wxFontDialog::SetData(wxFontData& fontdata)
{
    m_fontData = fontdata;
}

bool wxFontDialog::Create(wxWindow *parent, const wxFontData& data)
{
    m_dialogParent = parent;

    m_fontData = data;

    // TODO: you may need to do dialog creation here, unless it's
    // done in ShowModal.
    return TRUE;
}

bool wxFontDialog::IsShown() const
{
    return false;
}

int wxFontDialog::ShowModal()
{
    // TODO: show (maybe create) the dialog
    return wxID_CANCEL;
}

#endif // 10.2+