/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlgg.h
// Purpose:     wxFontDialog
// Author:      Robert Roebling
// Created:    
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTK_FONTDLGH__
#define __GTK_FONTDLGH__

#ifdef __GNUG__
#pragma interface "fontdlg.h"
#endif

#include "wx/setup.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/dialog.h"
#include "wx/cmndata.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxFontDialog;

//-----------------------------------------------------------------------------
// wxFontDialog
//-----------------------------------------------------------------------------

class wxFontDialog: public wxDialog
{
public:
    wxFontDialog() {}
    wxFontDialog( wxWindow *parent, wxFontData *data = (wxFontData *) NULL );
    ~wxFontDialog();

    inline wxFontData& GetFontData() { return m_fontData; }

//protected:
    wxFontData m_fontData;

private:
    DECLARE_DYNAMIC_CLASS(wxFontDialog)
};

#endif
