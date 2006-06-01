/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        picker.cpp
// Purpose:     Part of the widgets sample showing wx*PickerCtrl
// Author:      Francesco Montorsi
// Created:     22/4/2006
// Id:          $Id$
// Copyright:   (c) 2006 Francesco Montorsi
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COLOURPICKERCTRL || wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL || wxUSE_FONTPICKERCTRL

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
#endif

#include "wx/artprov.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/checkbox.h"
#include "wx/imaglist.h"

#include "wx/clrpicker.h"
#include "wx/filepicker.h"
#include "wx/fontpicker.h"

#include "widgets.h"

#include "icons/picker.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    PickerPage_Reset = wxID_HIGHEST,

#if wxUSE_COLOURPICKERCTRL
    PickerPage_Colour,
#endif
#if wxUSE_FILEPICKERCTRL
    PickerPage_File,
#endif
#if wxUSE_DIRPICKERCTRL
    PickerPage_Dir,
#endif
#if wxUSE_FONTPICKERCTRL
    PickerPage_Font
#endif
};


// ----------------------------------------------------------------------------
// PickerWidgetsPage
// ----------------------------------------------------------------------------

class PickerWidgetsPage : public WidgetsPage
{
public:
    PickerWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~PickerWidgetsPage(){};

    virtual wxControl *GetWidget() const { /*return m_fontPicker;*/ return NULL; }
    virtual void RecreateWidget() { RecreateAllPickers(); }

protected:
    enum PickerKind
    {
        Picker_None = -1,
        Picker_Colour,
        Picker_File,
        Picker_Dir,
        Picker_Font
    };

    // called only once at first construction
    void CreatePickers(PickerKind picker);

    // called to recreate an existing control
    void RecreatePicker(PickerKind picker);

    // recreate all existing picker controls
    void RecreateAllPickers();

    // restore the checkboxes state to the initial values
    void Reset();

    // get the initial style for the picker of the given kind
    long GetPickerStyle(PickerKind kind);


    // the pickers and the relative event handlers
#if wxUSE_COLOURPICKERCTRL
    wxColourPickerCtrl *m_clrPicker;
    void OnColourChange(wxColourPickerEvent &ev);
#endif
#if wxUSE_FILEPICKERCTRL
    wxFilePickerCtrl *m_filePicker;
    void OnFileChange(wxFileDirPickerEvent &ev);
#endif
#if wxUSE_DIRPICKERCTRL
    wxDirPickerCtrl *m_dirPicker;
    void OnDirChange(wxFileDirPickerEvent &ev);
#endif
#if wxUSE_FONTPICKERCTRL
    wxFontPickerCtrl *m_fontPicker;
    void OnFontChange(wxFontPickerEvent &ev);
#endif
    void OnCheckBox(wxCommandEvent &ev);
    void OnButtonReset(wxCommandEvent &ev);


    // other controls
    // --------------

    wxCheckBox *m_chkColourTextCtrl,
               *m_chkColourShowLabel;

    wxCheckBox *m_chkFileTextCtrl,
               *m_chkFileOverwritePrompt,
               *m_chkFileMustExist,
               *m_chkFileChangeDir;

    wxCheckBox *m_chkDirTextCtrl,
               *m_chkDirChangeDir,
               *m_chkDirMustExist;

    wxCheckBox *m_chkFontTextCtrl,
               *m_chkFontDescAsLabel,
               *m_chkFontUseFontForLabel;

    wxFlexGridSizer *m_sizerPicker;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(PickerWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PickerWidgetsPage, WidgetsPage)
    EVT_BUTTON(PickerPage_Reset, PickerWidgetsPage::OnButtonReset)

#if wxUSE_COLOURPICKERCTRL
    EVT_COLOURPICKER_CHANGED(PickerPage_Colour, PickerWidgetsPage::OnColourChange)
#endif
#if wxUSE_FILEPICKERCTRL
    EVT_FILEPICKER_CHANGED(PickerPage_File, PickerWidgetsPage::OnFileChange)
#endif
#if wxUSE_DIRPICKERCTRL
    EVT_DIRPICKER_CHANGED(PickerPage_Dir, PickerWidgetsPage::OnDirChange)
#endif
#if wxUSE_FONTPICKERCTRL
    EVT_FONTPICKER_CHANGED(PickerPage_Font, PickerWidgetsPage::OnFontChange)
#endif

    EVT_CHECKBOX(wxID_ANY, PickerWidgetsPage::OnCheckBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXGTK24__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(PickerWidgetsPage, _T("Pickers"),
                       PICKER_CTRLS | FAMILY_CTRLS);

PickerWidgetsPage::PickerWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, picker_xpm)
{
    // left pane
    wxSizer *boxleft = new wxBoxSizer(wxVERTICAL);

#if wxUSE_COLOURPICKERCTRL
    wxStaticBoxSizer *clrbox = new wxStaticBoxSizer(wxVERTICAL, this, _T("&ColourPicker style"));
    m_chkColourTextCtrl = CreateCheckBoxAndAddToSizer(clrbox, _T("With textctrl"), false);
    m_chkColourShowLabel = CreateCheckBoxAndAddToSizer(clrbox, _T("With label"), false);
    boxleft->Add(clrbox, 0, wxALL|wxGROW, 5);
    boxleft->Add(1, 1, 1, wxGROW | wxALL, 5); // spacer
#endif // wxUSE_COLOURPICKERCTRL

#if wxUSE_FILEPICKERCTRL
    wxStaticBoxSizer *filebox = new wxStaticBoxSizer(wxVERTICAL, this, _T("&FilePicker style"));
    m_chkFileTextCtrl = CreateCheckBoxAndAddToSizer(filebox, _T("With textctrl"), false);
    m_chkFileOverwritePrompt = CreateCheckBoxAndAddToSizer(filebox, _T("Overwrite prompt"), false);
    m_chkFileMustExist = CreateCheckBoxAndAddToSizer(filebox, _T("File must exist"), false);
    m_chkFileChangeDir = CreateCheckBoxAndAddToSizer(filebox, _T("Change working dir"), false);
    boxleft->Add(filebox, 0, wxALL|wxGROW, 5);
    boxleft->Add(1, 1, 1, wxGROW | wxALL, 5); // spacer
#endif // wxUSE_FILEPICKERCTRL

#if wxUSE_DIRPICKERCTRL
    wxStaticBoxSizer *dirbox = new wxStaticBoxSizer(wxVERTICAL, this, _T("&DirPicker style"));
    m_chkDirTextCtrl = CreateCheckBoxAndAddToSizer(dirbox, _T("With textctrl"), false);
    m_chkDirMustExist = CreateCheckBoxAndAddToSizer(dirbox, _T("Dir must exist"), false);
    m_chkDirChangeDir = CreateCheckBoxAndAddToSizer(dirbox, _T("Change working dir"), false);
    boxleft->Add(dirbox, 0, wxALL|wxGROW, 5);
    boxleft->Add(1, 1, 1, wxGROW | wxALL, 5); // spacer
#endif // wxUSE_DIRPICKERCTRL

#if wxUSE_FONTPICKERCTRL
    wxStaticBoxSizer *fontbox = new wxStaticBoxSizer(wxVERTICAL, this, _T("&FontPicker style"));
    m_chkFontTextCtrl = CreateCheckBoxAndAddToSizer(fontbox, _T("With textctrl"));
    m_chkFontDescAsLabel = CreateCheckBoxAndAddToSizer(fontbox, _T("Font desc as btn label"));
    m_chkFontUseFontForLabel = CreateCheckBoxAndAddToSizer(fontbox, _T("Use font for label"), false);
    boxleft->Add(fontbox, 0, wxALL|wxGROW, 5);
#endif // wxUSE_FONTPICKERCTRL

    boxleft->Add(new wxButton(this, PickerPage_Reset, _T("&Reset")),
                 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    Reset();    // set checkboxes state

    // create pickers
    m_clrPicker = NULL;
    m_filePicker = NULL;
    m_fontPicker = NULL;
    m_dirPicker = NULL;

    int nrows = 0;
#if wxUSE_COLOURPICKERCTRL
    CreatePickers(Picker_Colour);
    nrows++;
#endif // wxUSE_COLOURPICKERCTRL

#if wxUSE_FILEPICKERCTRL
    CreatePickers(Picker_File);
    nrows++;
#endif // wxUSE_FILEPICKERCTRL

#if wxUSE_DIRPICKERCTRL
    CreatePickers(Picker_Dir);
    nrows++;
#endif // wxUSE_DIRPICKERCTRL

#if wxUSE_FONTPICKERCTRL
    CreatePickers(Picker_Font);
    nrows++;
#endif // wxUSE_FONTPICKERCTRL

    m_sizerPicker = new wxFlexGridSizer(nrows, 2, 0, 0);  // 4 rows x 2 columns
#if wxUSE_COLOURPICKERCTRL
    m_sizerPicker->Add(new wxStaticText(this, wxID_ANY, wxT("wxColourPickerCtrl:")),
                       0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
    m_sizerPicker->Add(m_clrPicker, 1, wxGROW | wxALL, 5);
#endif
#if wxUSE_FILEPICKERCTRL
    m_sizerPicker->Add(new wxStaticText(this, wxID_ANY, wxT("wxFilePickerCtrl:")),
                       0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
    m_sizerPicker->Add(m_filePicker, 1, wxGROW | wxALL, 5);
#endif
#if wxUSE_DIRPICKERCTRL
    m_sizerPicker->Add(new wxStaticText(this, wxID_ANY, wxT("wxDirPickerCtrl:")),
                       0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
    m_sizerPicker->Add(m_dirPicker, 1, wxGROW | wxALL, 5);
#endif
#if wxUSE_FONTPICKERCTRL
    m_sizerPicker->Add(new wxStaticText(this, wxID_ANY, wxT("wxFontPickerCtrl:")),
                       0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
    m_sizerPicker->Add(m_fontPicker, 1, wxGROW | wxALL, 5);
#endif

    m_sizerPicker->AddGrowableCol(1, 3);

    // right pane
    wxStaticBoxSizer *
        boxright = new wxStaticBoxSizer(wxVERTICAL, this, _T("Pickers"));
    boxright->Add(m_sizerPicker, 1, wxGROW | wxALL, 5);

    // global pane
    wxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
    sz->Add(boxleft, 0, wxGROW|wxALL, 5);
    sz->Add(boxright, 1, wxGROW|wxALL, 5);

    SetSizerAndFit(sz);
}

void PickerWidgetsPage::CreatePickers(PickerKind picker)
{
    switch ( picker )
    {
#if wxUSE_COLOURPICKERCTRL
        case Picker_Colour:
            delete m_clrPicker;

            m_clrPicker = new wxColourPickerCtrl(this, PickerPage_Colour, *wxRED,
                                                 wxDefaultPosition, wxDefaultSize,
                                                 GetPickerStyle(Picker_Colour));
            break;
#endif // wxUSE_COLOURPICKERCTRL

#if wxUSE_FILEPICKERCTRL
        case Picker_File:
            delete m_filePicker;

            // pass an empty string as initial file
            m_filePicker = new wxFilePickerCtrl(this, PickerPage_File,
                                                wxEmptyString,
                                                wxT("Hello!"), wxT("*"),
                                                wxDefaultPosition, wxDefaultSize,
                                                GetPickerStyle(Picker_File));
            break;
#endif // wxUSE_FILEPICKERCTRL

#if wxUSE_DIRPICKERCTRL
        case Picker_Dir:
            delete m_dirPicker;

            m_dirPicker = new wxDirPickerCtrl(this, PickerPage_Dir,
                                              wxGetHomeDir(), wxT("Hello!"),
                                              wxDefaultPosition, wxDefaultSize,
                                              GetPickerStyle(Picker_Dir));
            break;
#endif // wxUSE_DIRPICKERCTRL

#if wxUSE_FONTPICKERCTRL
        case Picker_Font:
            delete m_fontPicker;

            m_fontPicker = new wxFontPickerCtrl(this, PickerPage_Font,
                                                *wxSWISS_FONT,
                                                wxDefaultPosition, wxDefaultSize,
                                                GetPickerStyle(Picker_Font));
            break;
#endif // wxUSE_FONTPICKERCTRL

        default:
            wxFAIL_MSG( _T("unknown picker kind") );
    }
}

long PickerWidgetsPage::GetPickerStyle(PickerKind picker)
{
    long style = 0;

    switch (picker)
    {
#if wxUSE_COLOURPICKERCTRL
        case Picker_Colour:
            if ( m_chkColourTextCtrl->GetValue() )
                style |= wxCLRP_USE_TEXTCTRL;

            if ( m_chkColourShowLabel->GetValue() )
                style |= wxCLRP_SHOW_LABEL;

            break;
#endif // wxUSE_COLOURPICKERCTRL

#if wxUSE_FILEPICKERCTRL
        case Picker_File:
            if ( m_chkFileTextCtrl->GetValue() )
                style |= wxFLP_USE_TEXTCTRL;

            if ( m_chkFileOverwritePrompt->GetValue() )
                style |= wxFLP_OVERWRITE_PROMPT;

            if ( m_chkFileMustExist->GetValue() )
                style |= wxFLP_FILE_MUST_EXIST;

            if ( m_chkFileChangeDir->GetValue() )
                style |= wxFLP_CHANGE_DIR;

            break;
#endif // wxUSE_FILEPICKERCTRL

#if wxUSE_DIRPICKERCTRL
        case Picker_Dir:
            if ( m_chkDirTextCtrl->GetValue() )
                style |= wxDIRP_USE_TEXTCTRL;

            if ( m_chkDirMustExist->GetValue() )
                style |= wxDIRP_DIR_MUST_EXIST;

            if ( m_chkDirChangeDir->GetValue() )
                style |= wxDIRP_CHANGE_DIR;

            break;
#endif // wxUSE_DIRPICKERCTRL

#if wxUSE_FONTPICKERCTRL
        case Picker_Font:
            if ( m_chkFontTextCtrl->GetValue() )
                style |= wxFNTP_USE_TEXTCTRL;

            if ( m_chkFontUseFontForLabel->GetValue() )
                style |= wxFNTP_USEFONT_FOR_LABEL;

            if ( m_chkFontDescAsLabel->GetValue() )
                 style |= wxFNTP_FONTDESC_AS_LABEL;

            break;
#endif // wxUSE_FONTPICKERCTRL

        default:
            wxFAIL_MSG( _T("unknown picker kind") );
    }

    return style;
}

void PickerWidgetsPage::RecreatePicker(PickerKind picker)
{
    switch ( picker )
    {
#if wxUSE_COLOURPICKERCTRL
        case Picker_Colour:
            m_sizerPicker->Remove(1);
            CreatePickers(Picker_Colour);
            m_sizerPicker->Insert(1, m_clrPicker, 1, wxGROW|wxALL, 5);
            break;
#endif // wxUSE_COLOURPICKERCTRL

#if wxUSE_FILEPICKERCTRL
        case Picker_File:
            m_sizerPicker->Remove(3);
            CreatePickers(Picker_File);
            m_sizerPicker->Insert(3, m_filePicker, 1, wxGROW|wxALL, 5);
            break;
#endif // wxUSE_FILEPICKERCTRL

#if wxUSE_DIRPICKERCTRL
        case Picker_Dir:
            m_sizerPicker->Remove(5);
            CreatePickers(Picker_Dir);
            m_sizerPicker->Insert(5, m_dirPicker, 1, wxGROW|wxALL, 5);
            break;
#endif // wxUSE_DIRPICKERCTRL

#if wxUSE_FONTPICKERCTRL
        case Picker_Font:
            wxASSERT(m_sizerPicker->Remove(7));
            CreatePickers(Picker_Font);
            m_sizerPicker->Insert(7, m_fontPicker, 1, wxGROW|wxALL, 5);
            break;
#endif // wxUSE_FONTPICKERCTRL

        default:
            wxFAIL_MSG( _T("unknown picker kind") );
    }

    m_sizerPicker->Layout();
}

void PickerWidgetsPage::RecreateAllPickers()
{
#if wxUSE_COLOURPICKERCTRL
    RecreatePicker(Picker_Colour);
#endif // wxUSE_COLOURPICKERCTRL

#if wxUSE_FILEPICKERCTRL
    RecreatePicker(Picker_File);
#endif // wxUSE_FILEPICKERCTRL

#if wxUSE_DIRPICKERCTRL
    RecreatePicker(Picker_Dir);
#endif // wxUSE_DIRPICKERCTRL

#if wxUSE_FONTPICKERCTRL
    RecreatePicker(Picker_Font);
#endif // wxUSE_FONTPICKERCTRL
}

void PickerWidgetsPage::Reset()
{
#if wxUSE_COLOURPICKERCTRL
    m_chkColourTextCtrl->SetValue((wxCLRP_DEFAULT_STYLE & wxCLRP_USE_TEXTCTRL) != 0);
    m_chkColourShowLabel->SetValue((wxCLRP_DEFAULT_STYLE & wxCLRP_SHOW_LABEL) != 0);
#endif

#if wxUSE_FILEPICKERCTRL
    m_chkFileTextCtrl->SetValue((wxFLP_DEFAULT_STYLE & wxFLP_USE_TEXTCTRL) != 0);
    m_chkFileOverwritePrompt->SetValue((wxFLP_DEFAULT_STYLE & wxFLP_OVERWRITE_PROMPT) != 0);
    m_chkFileMustExist->SetValue((wxFLP_DEFAULT_STYLE & wxFLP_FILE_MUST_EXIST) != 0);
    m_chkFileChangeDir->SetValue((wxFLP_DEFAULT_STYLE & wxFLP_CHANGE_DIR) != 0);
#endif

#if wxUSE_DIRPICKERCTRL
    m_chkDirTextCtrl->SetValue((wxDIRP_DEFAULT_STYLE & wxDIRP_USE_TEXTCTRL) != 0);
    m_chkDirMustExist->SetValue((wxDIRP_DEFAULT_STYLE & wxDIRP_DIR_MUST_EXIST) != 0);
    m_chkDirChangeDir->SetValue((wxDIRP_DEFAULT_STYLE & wxDIRP_CHANGE_DIR) != 0);
#endif

#if wxUSE_FONTPICKERCTRL
    m_chkFontTextCtrl->SetValue((wxFNTP_DEFAULT_STYLE & wxFNTP_USE_TEXTCTRL) != 0);
    m_chkFontUseFontForLabel->SetValue((wxFNTP_DEFAULT_STYLE & wxFNTP_USEFONT_FOR_LABEL) != 0);
    m_chkFontDescAsLabel->SetValue((wxFNTP_DEFAULT_STYLE & wxFNTP_FONTDESC_AS_LABEL) != 0);
#endif
}


// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void PickerWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    RecreateAllPickers();
}

#if wxUSE_COLOURPICKERCTRL
void PickerWidgetsPage::OnColourChange(wxColourPickerEvent& event)
{
    wxLogMessage(wxT("The colour changed to '%s' !"),
                 event.GetColour().GetAsString(wxC2S_CSS_SYNTAX).c_str());
}
#endif
#if wxUSE_FILEPICKERCTRL
void PickerWidgetsPage::OnFileChange(wxFileDirPickerEvent& event)
{
    wxLogMessage(wxT("The file changed to '%s' ! The current working directory is '%s'"),
                 event.GetPath().c_str(), wxGetCwd().c_str());
}
#endif
#if wxUSE_DIRPICKERCTRL
void PickerWidgetsPage::OnDirChange(wxFileDirPickerEvent& event)
{
    wxLogMessage(wxT("The directory changed to '%s' ! The current working directory is '%s'"),
                 event.GetPath().c_str(), wxGetCwd().c_str());
}
#endif
#if wxUSE_FONTPICKERCTRL
void PickerWidgetsPage::OnFontChange(wxFontPickerEvent& event)
{
    wxLogMessage(wxT("The font changed to '%s' with size %d !"),
                 event.GetFont().GetFaceName().c_str(), event.GetFont().GetPointSize());
}
#endif

void PickerWidgetsPage::OnCheckBox(wxCommandEvent &event)
{
    if (event.GetEventObject() == m_chkColourTextCtrl ||
        event.GetEventObject() == m_chkColourShowLabel)
        RecreatePicker(Picker_Colour);

    if (event.GetEventObject() == m_chkFileTextCtrl ||
        event.GetEventObject() == m_chkFileOverwritePrompt ||
        event.GetEventObject() == m_chkFileMustExist ||
        event.GetEventObject() == m_chkFileChangeDir)
        RecreatePicker(Picker_File);

    if (event.GetEventObject() == m_chkDirTextCtrl ||
        event.GetEventObject() == m_chkDirChangeDir ||
        event.GetEventObject() == m_chkDirMustExist)
        RecreatePicker(Picker_Dir);

    if (event.GetEventObject() == m_chkFontTextCtrl ||
        event.GetEventObject() == m_chkFontDescAsLabel ||
        event.GetEventObject() == m_chkFontUseFontForLabel)
        RecreatePicker(Picker_Font);
}

#endif  // wxUSE_COLOURPICKERCTRL || wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL || wxUSE_FONTPICKERCTRL
