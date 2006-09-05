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

#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * Font dialog
 */

#ifndef wxMAC_USE_EXPERIMENTAL_FONTDIALOG
#define wxMAC_USE_EXPERIMENTAL_FONTDIALOG 1
#endif

#if wxMAC_USE_EXPERIMENTAL_FONTDIALOG

class WXDLLEXPORT wxFontDialog : public wxDialog
{
public:
    wxFontDialog();
    wxFontDialog(wxWindow *parent, const wxFontData& data);
    virtual ~wxFontDialog();

    bool Create(wxWindow *parent, const wxFontData& data);

    int ShowModal();
    wxFontData& GetFontData() { return m_fontData; }

protected:
    wxFontData m_fontData;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxFontDialog)
};

extern "C" int RunMixedFontDialog(wxFontDialog* dialog) ;

#else // wxMAC_USE_EXPERIMENTAL_FONTDIALOG

#if !USE_NATIVE_FONT_DIALOG_FOR_MACOSX

/*!
 * Forward declarations
 */

class wxFontColourSwatchCtrl;
class wxFontPreviewCtrl;
class WXDLLEXPORT wxSpinCtrl;
class WXDLLEXPORT wxSpinEvent;
class WXDLLEXPORT wxListBox;
class WXDLLEXPORT wxChoice;
class WXDLLEXPORT wxButton;
class WXDLLEXPORT wxStaticText;
class WXDLLEXPORT wxCheckBox;

/*!
 * Control identifiers
 */

#define wxID_FONTDIALOG_FACENAME 20001
#define wxID_FONTDIALOG_FONTSIZE 20002
#define wxID_FONTDIALOG_BOLD 20003
#define wxID_FONTDIALOG_ITALIC 20004
#define wxID_FONTDIALOG_UNDERLINED 20005
#define wxID_FONTDIALOG_COLOUR 20006
#define wxID_FONTDIALOG_PREVIEW 20007

#endif
    // !USE_NATIVE_FONT_DIALOG_FOR_MACOSX

class WXDLLEXPORT wxFontDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxFontDialog)

#if !USE_NATIVE_FONT_DIALOG_FOR_MACOSX
DECLARE_EVENT_TABLE()
#endif

public:
    wxFontDialog();
    wxFontDialog(wxWindow *parent, const wxFontData& data);
    virtual ~wxFontDialog();

    bool Create(wxWindow *parent, const wxFontData& data);

    int ShowModal();
    wxFontData& GetFontData() { return m_fontData; }
    bool IsShown() const;
    void OnPanelClose();
    void SetData(const wxFontData& data);

#if !USE_NATIVE_FONT_DIALOG_FOR_MACOSX

    /// Creates the controls and sizers
    void CreateControls();

    /// Initialize font
    void InitializeFont();

    /// Set controls according to current font
    void InitializeControls();

    /// Respond to font change
    void ChangeFont();

    /// Respond to colour change
    void OnColourChanged(wxCommandEvent& event);

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for wxID_FONTDIALOG_FACENAME
    void OnFontdialogFacenameSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_SPINCTRL_UPDATED event handler for wxID_FONTDIALOG_FONTSIZE
    void OnFontdialogFontsizeUpdated( wxSpinEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for wxID_FONTDIALOG_FONTSIZE
    void OnFontdialogFontsizeTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for wxID_FONTDIALOG_BOLD
    void OnFontdialogBoldClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for wxID_FONTDIALOG_ITALIC
    void OnFontdialogItalicClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for wxID_FONTDIALOG_UNDERLINED
    void OnFontdialogUnderlinedClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// Should we show tooltips?
    static bool ShowToolTips();

    wxListBox* m_facenameCtrl;
    wxSpinCtrl* m_sizeCtrl;
    wxCheckBox* m_boldCtrl;
    wxCheckBox* m_italicCtrl;
    wxCheckBox* m_underlinedCtrl;
    wxFontColourSwatchCtrl* m_colourCtrl;
    wxFontPreviewCtrl* m_previewCtrl;

    wxFont      m_dialogFont;
    bool        m_suppressUpdates;

#endif
    // !USE_NATIVE_FONT_DIALOG_FOR_MACOSX

protected:
    wxWindow*   m_dialogParent;
    wxFontData  m_fontData;
    void*		m_pEventHandlerRef;
};

#endif

#endif
    // _WX_FONTDLG_H_

