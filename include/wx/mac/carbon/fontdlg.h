/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.h
// Purpose:     wxFontDialog class using fonts window services (10.2+). 
// Author:      Ryan Norton
// Modified by:
// Created:     2004-09-25
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONTDLG_H_
#define _WX_FONTDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "fontdlg.h"
#endif

#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * Font dialog
 */
 
class WXDLLEXPORT wxFontDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxFontDialog)

public:
    wxFontDialog();
    wxFontDialog(wxWindow *parent, const wxFontData& data);
    ~wxFontDialog();

    bool Create(wxWindow *parent, const wxFontData& data);

    int ShowModal();
    wxFontData& GetFontData() { return m_fontData; }
    bool IsShown() const;
    void OnPanelClose();
    void SetData(wxFontData& data);

protected:
    wxWindow*   m_dialogParent;
    wxFontData  m_fontData;
    void*		m_pEventHandlerRef;
};

#endif
    // _WX_FONTDLG_H_

