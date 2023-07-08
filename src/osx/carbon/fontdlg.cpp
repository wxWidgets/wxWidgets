/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/fontdlg.cpp
// Purpose:     wxFontDialog class for carbon 10.2+.
// Author:      Ryan Norton
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FONTDLG

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/crt.h"
    #include "wx/dcclient.h"
    #include "wx/frame.h"
    #include "wx/textctrl.h"
    #include "wx/listbox.h"
    #include "wx/checkbox.h"
    #include "wx/choice.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
    #include "wx/button.h"
#endif

#include "wx/fontdlg.h"
#include "wx/fontutil.h"
#include "wx/modalhook.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog);

#include "wx/osx/private.h"

// ---------------------------------------------------------------------------
// wxFontDialog
// ---------------------------------------------------------------------------

wxFontDialog::wxFontDialog()
{
}

wxFontDialog::wxFontDialog(wxWindow *parent)
{
    Create(parent);
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData&  data)
{
    Create(parent, data);
}

wxFontDialog::~wxFontDialog()
{
}

bool wxFontDialog::Create(wxWindow *WXUNUSED(parent))
{
    return true ;
}

bool wxFontDialog::Create(wxWindow *parent, const wxFontData& data)
{
    m_fontData = data;
    return Create(parent) ;
}

int wxFontDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    OSXBeginModalDialog();
    int retval = RunMixedFontDialog(this);
    OSXEndModalDialog();

    return retval ;
}

#endif // wxUSE_FONTDLG
