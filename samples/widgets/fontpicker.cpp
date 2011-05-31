/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        fontpicker.cpp
// Purpose:     Shows wxFontPickerCtrl
// Author:      Francesco Montorsi
// Created:     20/6/2006
// Id:          $Id$
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
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

#if wxUSE_FONTPICKERCTRL

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/radiobox.h"
#endif

#include "wx/artprov.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/checkbox.h"
#include "wx/imaglist.h"

#include "wx/fontpicker.h"
#include "widgets.h"

#include "icons/fontpicker.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    PickerPage_Reset = wxID_HIGHEST,
    PickerPage_Font
};


// ----------------------------------------------------------------------------
// FontPickerWidgetsPage
// ----------------------------------------------------------------------------

class FontPickerWidgetsPage : public WidgetsPage
{
public:
    FontPickerWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~FontPickerWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_fontPicker; }
    virtual void RecreateWidget() { RecreatePicker(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:

    // called only once at first construction
    void CreatePicker();

    // called to recreate an existing control
    void RecreatePicker();

    // restore the checkboxes state to the initial values
    void Reset();

    // get the initial style for the picker of the given kind
    long GetPickerStyle();


    void OnFontChange(wxFontPickerEvent &ev);
    void OnCheckBox(wxCommandEvent &ev);
    void OnButtonReset(wxCommandEvent &ev);

    // the picker
    wxFontPickerCtrl *m_fontPicker;


    // other controls
    // --------------

    wxCheckBox *m_chkFontTextCtrl,
               *m_chkFontDescAsLabel,
               *m_chkFontUseFontForLabel;
    wxBoxSizer *m_sizer;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(FontPickerWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FontPickerWidgetsPage, WidgetsPage)
    EVT_BUTTON(PickerPage_Reset, FontPickerWidgetsPage::OnButtonReset)

    EVT_FONTPICKER_CHANGED(PickerPage_Font, FontPickerWidgetsPage::OnFontChange)

    EVT_CHECKBOX(wxID_ANY, FontPickerWidgetsPage::OnCheckBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXGTK24__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(FontPickerWidgetsPage, wxT("FontPicker"),
                       PICKER_CTRLS | FAMILY_CTRLS);

FontPickerWidgetsPage::FontPickerWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, fontpicker_xpm)
{
}

void FontPickerWidgetsPage::CreateContent()
{
    // left pane
    wxSizer *boxleft = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer *fontbox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("&FontPicker style"));
    m_chkFontTextCtrl = CreateCheckBoxAndAddToSizer(fontbox, wxT("With textctrl"));
    m_chkFontDescAsLabel = CreateCheckBoxAndAddToSizer(fontbox, wxT("Font desc as btn label"));
    m_chkFontUseFontForLabel = CreateCheckBoxAndAddToSizer(fontbox, wxT("Use font for label"), false);
    boxleft->Add(fontbox, 0, wxALL|wxGROW, 5);

    boxleft->Add(new wxButton(this, PickerPage_Reset, wxT("&Reset")),
                 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    Reset();    // set checkboxes state

    // create pickers
    m_fontPicker = NULL;
    CreatePicker();

    // right pane
    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_sizer->Add(1, 1, 1, wxGROW | wxALL, 5); // spacer
    m_sizer->Add(m_fontPicker, 0, wxALIGN_CENTER|wxALL, 5);
    m_sizer->Add(1, 1, 1, wxGROW | wxALL, 5); // spacer

    // global pane
    wxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
    sz->Add(boxleft, 0, wxGROW|wxALL, 5);
    sz->Add(m_sizer, 1, wxGROW|wxALL, 5);

    SetSizer(sz);
}

void FontPickerWidgetsPage::CreatePicker()
{
    delete m_fontPicker;

    m_fontPicker = new wxFontPickerCtrl(this, PickerPage_Font,
                                        *wxSWISS_FONT,
                                        wxDefaultPosition, wxDefaultSize,
                                        GetPickerStyle());
}

long FontPickerWidgetsPage::GetPickerStyle()
{
    long style = 0;

    if ( m_chkFontTextCtrl->GetValue() )
        style |= wxFNTP_USE_TEXTCTRL;

    if ( m_chkFontUseFontForLabel->GetValue() )
        style |= wxFNTP_USEFONT_FOR_LABEL;

    if ( m_chkFontDescAsLabel->GetValue() )
            style |= wxFNTP_FONTDESC_AS_LABEL;

    return style;
}

void FontPickerWidgetsPage::RecreatePicker()
{
    m_sizer->Remove(1);
    CreatePicker();
    m_sizer->Insert(1, m_fontPicker, 0, wxALIGN_CENTER|wxALL, 5);

    m_sizer->Layout();
}

void FontPickerWidgetsPage::Reset()
{
    m_chkFontTextCtrl->SetValue((wxFNTP_DEFAULT_STYLE & wxFNTP_USE_TEXTCTRL) != 0);
    m_chkFontUseFontForLabel->SetValue((wxFNTP_DEFAULT_STYLE & wxFNTP_USEFONT_FOR_LABEL) != 0);
    m_chkFontDescAsLabel->SetValue((wxFNTP_DEFAULT_STYLE & wxFNTP_FONTDESC_AS_LABEL) != 0);
}


// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void FontPickerWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();
    RecreatePicker();
}

void FontPickerWidgetsPage::OnFontChange(wxFontPickerEvent& event)
{
    wxLogMessage(wxT("The font changed to '%s' with size %d !"),
                 event.GetFont().GetFaceName().c_str(), event.GetFont().GetPointSize());
}

void FontPickerWidgetsPage::OnCheckBox(wxCommandEvent &event)
{
    if (event.GetEventObject() == m_chkFontTextCtrl ||
        event.GetEventObject() == m_chkFontDescAsLabel ||
        event.GetEventObject() == m_chkFontUseFontForLabel)
        RecreatePicker();
}

#endif  //  wxUSE_FONTPICKERCTRL
