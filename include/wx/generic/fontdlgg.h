/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/fontdlgg.h
// Purpose:     wxGenericFontDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_FONTDLGG_H
#define _WX_GENERIC_FONTDLGG_H

#include "wx/gdicmn.h"
#include "wx/font.h"

#define USE_SPINCTRL_FOR_POINT_SIZE 0

/*
 * FONT DIALOG
 */

class WXDLLIMPEXP_FWD_CORE wxChoice;
class WXDLLIMPEXP_FWD_CORE wxText;
class WXDLLIMPEXP_FWD_CORE wxCheckBox;
class WXDLLIMPEXP_FWD_CORE wxFontPreviewer;

enum
{
    wxID_FONT_UNDERLINE = 3000,
    wxID_FONT_STYLE,
    wxID_FONT_WEIGHT,
    wxID_FONT_FAMILY,
    wxID_FONT_COLOUR,
    wxID_FONT_SIZE
};

class WXDLLIMPEXP_CORE wxGenericFontDialog : public wxFontDialogBase
{
public:
    wxGenericFontDialog() { Init(); }
    wxGenericFontDialog(wxWindow *parent)
        : wxFontDialogBase(parent) { Init(); }
    wxGenericFontDialog(wxWindow *parent, const wxFontData& data)
        : wxFontDialogBase(parent, data) { Init(); }
    virtual ~wxGenericFontDialog();

    virtual int ShowModal() wxOVERRIDE;

    // Internal functions
    void OnCloseWindow(wxCloseEvent& event);

    virtual void CreateWidgets();
    virtual void InitializeFont();

    void OnChangeFont(wxCommandEvent& event);

#if USE_SPINCTRL_FOR_POINT_SIZE
    void OnChangeSize(wxSpinEvent& event);
#endif

protected:

    virtual bool DoCreate(wxWindow *parent) wxOVERRIDE;

private:

    // common part of all ctors
    void Init();

    void DoChangeFont();

    wxFont m_dialogFont;

    wxChoice *m_familyChoice;
    wxChoice *m_styleChoice;
    wxChoice *m_weightChoice;
    wxChoice *m_colourChoice;
    wxCheckBox *m_underLineCheckBox;

#if USE_SPINCTRL_FOR_POINT_SIZE
    wxSpinCtrl *m_pointSizeSpin;
#else
    wxChoice   *m_pointSizeChoice;
#endif

    wxFontPreviewer *m_previewer;
    bool       m_useEvents;

    //  static bool fontDialogCancelled;
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxGenericFontDialog);
};

#endif // _WX_GENERIC_FONTDLGG_H
