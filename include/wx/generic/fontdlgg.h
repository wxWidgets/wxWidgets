/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/fontdlgg.h
// Purpose:     wxGenericFontDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_FONTDLGG_H
#define _WX_GENERIC_FONTDLGG_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "fontdlgg.h"
#endif

#include "wx/setup.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * FONT DIALOG
 */

class WXDLLEXPORT wxChoice;
class WXDLLEXPORT wxText;
class WXDLLEXPORT wxCheckBox;
class WXDLLEXPORT wxFontPreviewer;

enum
{
    wxID_FONT_UNDERLINE = 3000,
    wxID_FONT_STYLE,
    wxID_FONT_WEIGHT,
    wxID_FONT_FAMILY,
    wxID_FONT_COLOUR,
    wxID_FONT_SIZE
};

class WXDLLEXPORT wxGenericFontDialog : public wxFontDialogBase
{
public:
    wxGenericFontDialog() { Init(); }
    wxGenericFontDialog(wxWindow *parent, const wxFontData& data)
        : wxFontDialogBase(parent, data) { Init(); }
    virtual ~wxGenericFontDialog();

    virtual int ShowModal();

    // deprecated, for backwards compatibility only
    wxGenericFontDialog(wxWindow *parent, const wxFontData *data)
        : wxFontDialogBase(parent, data) { Init(); }

    // Internal functions
    void OnCloseWindow(wxCloseEvent& event);

    virtual void CreateWidgets();
    virtual void InitializeFont();

    void OnChangeFont(wxCommandEvent& event);

protected:
    // common part of all ctors
    void Init();

    virtual bool DoCreate(wxWindow *parent);

    wxFont dialogFont;

    wxChoice *familyChoice;
    wxChoice *styleChoice;
    wxChoice *weightChoice;
    wxChoice *colourChoice;
    wxCheckBox *underLineCheckBox;
    wxChoice   *pointSizeChoice;

    wxFontPreviewer *m_previewer;
    bool       m_useEvents;

    //  static bool fontDialogCancelled;
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxGenericFontDialog)
};

WXDLLEXPORT const wxChar *wxFontFamilyIntToString(int family);
WXDLLEXPORT const wxChar *wxFontWeightIntToString(int weight);
WXDLLEXPORT const wxChar *wxFontStyleIntToString(int style);
WXDLLEXPORT int wxFontFamilyStringToInt(wxChar *family);
WXDLLEXPORT int wxFontWeightStringToInt(wxChar *weight);
WXDLLEXPORT int wxFontStyleStringToInt(wxChar *style);

#endif // _WX_GENERIC_FONTDLGG_H

