/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        toggle.cpp
// Purpose:     Part of the widgets sample showing toggle control
// Author:      Dimitri Schoolwerth, Vadim Zeitlin
// Created:     27 Sep 2003
// Id:          $Id$
// Copyright:   (c) 2006 Wlodzmierz Skiba
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

#if wxUSE_TOGGLEBTN

#include "wx/tglbtn.h"

#include "widgets.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/artprov.h"
#include "wx/sizer.h"
#include "wx/dcmemory.h"

#include "icons/toggle.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    TogglePage_Reset = wxID_HIGHEST,
    TogglePage_ChangeLabel,
    TogglePage_Picker
};

// radio boxes
enum
{
    ToggleImagePos_Left,
    ToggleImagePos_Right,
    ToggleImagePos_Top,
    ToggleImagePos_Bottom
};

enum
{
    ToggleHAlign_Left,
    ToggleHAlign_Centre,
    ToggleHAlign_Right
};

enum
{
    ToggleVAlign_Top,
    ToggleVAlign_Centre,
    ToggleVAlign_Bottom
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class ToggleWidgetsPage : public WidgetsPage
{
public:
    ToggleWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~ToggleWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_toggle; }
    virtual void RecreateWidget() { CreateToggle(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnCheckOrRadioBox(wxCommandEvent& event);

    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonChangeLabel(wxCommandEvent& event);

    // reset the toggle parameters
    void Reset();

    // (re)create the toggle
    void CreateToggle();

    // helper function: create a bitmap for wxBitmapToggleButton
    wxBitmap CreateBitmap(const wxString& label);

    // the controls
    // ------------

#if wxUSE_MARKUP
    wxCheckBox *m_chkUseMarkup;
#endif // wxUSE_MARKUP
#ifdef wxHAS_BITMAPTOGGLEBUTTON
    // the check/radio boxes for styles
    wxCheckBox *m_chkBitmapOnly,
               *m_chkTextAndBitmap,
               *m_chkFit,
               *m_chkUseBitmapClass;

    // more checkboxes for wxBitmapToggleButton only
    wxCheckBox *m_chkUsePressed,
               *m_chkUseFocused,
               *m_chkUseCurrent,
               *m_chkUseDisabled;

    // and an image position choice used if m_chkTextAndBitmap is on
    wxRadioBox *m_radioImagePos;

    wxRadioBox *m_radioHAlign,
               *m_radioVAlign;
#endif // wxHAS_BITMAPTOGGLEBUTTON

    // the checkbox itself and the sizer it is in
#ifdef wxHAS_ANY_BUTTON
    wxToggleButton *m_toggle;
#else
    wxToggleButtonBase *m_toggle;
#endif // wxHAS_ANY_BUTTON
    wxSizer *m_sizerToggle;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(ToggleWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ToggleWidgetsPage, WidgetsPage)
    EVT_BUTTON(TogglePage_Reset, ToggleWidgetsPage::OnButtonReset)
    EVT_BUTTON(TogglePage_ChangeLabel, ToggleWidgetsPage::OnButtonChangeLabel)

    EVT_CHECKBOX(wxID_ANY, ToggleWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, ToggleWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ToggleWidgetsPage, wxT("ToggleButton"),
                       FAMILY_CTRLS
                       );

ToggleWidgetsPage::ToggleWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                      :WidgetsPage(book, imaglist, toggle_xpm)
{
#if wxUSE_MARKUP
    m_chkUseMarkup = (wxCheckBox *)NULL;
#endif // wxUSE_MARKUP
#ifdef wxHAS_BITMAPTOGGLEBUTTON
    // init everything
    m_chkBitmapOnly =
    m_chkTextAndBitmap =
    m_chkFit =
    m_chkUseBitmapClass =
    m_chkUsePressed =
    m_chkUseFocused =
    m_chkUseCurrent =
    m_chkUseDisabled = (wxCheckBox *)NULL;

    m_radioImagePos =
    m_radioHAlign =
    m_radioVAlign = (wxRadioBox *)NULL;
#endif // wxHAS_BITMAPTOGGLEBUTTON

    m_textLabel = (wxTextCtrl *)NULL;

    m_toggle = (wxToggleButton *)NULL;
    m_sizerToggle = (wxSizer *)NULL;
}

void ToggleWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, wxT("Styles"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

#ifdef wxHAS_BITMAPTOGGLEBUTTON
    m_chkBitmapOnly = CreateCheckBoxAndAddToSizer(sizerLeft, "&Bitmap only");
    m_chkTextAndBitmap = CreateCheckBoxAndAddToSizer(sizerLeft, "Text &and bitmap");
    m_chkFit = CreateCheckBoxAndAddToSizer(sizerLeft, wxT("&Fit exactly"));
#endif // wxHAS_BITMAPTOGGLEBUTTON
#if wxUSE_MARKUP
    m_chkUseMarkup = CreateCheckBoxAndAddToSizer(sizerLeft, "Interpret &markup");
#endif // wxUSE_MARKUP

#ifdef wxHAS_BITMAPTOGGLEBUTTON
    m_chkUseBitmapClass = CreateCheckBoxAndAddToSizer(sizerLeft,
        "Use wxBitmapToggleButton");
    m_chkUseBitmapClass->SetValue(true);

    sizerLeft->AddSpacer(5);

    wxSizer *sizerUseLabels =
        new wxStaticBoxSizer(wxVERTICAL, this,
                "&Use the following bitmaps in addition to the normal one?");
    m_chkUsePressed = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Pressed (small help icon)");
    m_chkUseFocused = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Focused (small error icon)");
    m_chkUseCurrent = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Current (small warning icon)");
    m_chkUseDisabled = CreateCheckBoxAndAddToSizer(sizerUseLabels,
        "&Disabled (broken image icon)");
    sizerLeft->Add(sizerUseLabels, wxSizerFlags().Expand().Border());

    sizerLeft->AddSpacer(10);

    static const wxString dirs[] =
    {
        "left", "right", "top", "bottom",
    };
    m_radioImagePos = new wxRadioBox(this, wxID_ANY, "Image &position",
                                     wxDefaultPosition, wxDefaultSize,
                                     WXSIZEOF(dirs), dirs);
    sizerLeft->Add(m_radioImagePos, 0, wxGROW | wxALL, 5);
    sizerLeft->AddSpacer(15);

    // should be in sync with enums Toggle[HV]Align!
    static const wxString halign[] =
    {
        wxT("left"),
        wxT("centre"),
        wxT("right"),
    };

    static const wxString valign[] =
    {
        wxT("top"),
        wxT("centre"),
        wxT("bottom"),
    };

    m_radioHAlign = new wxRadioBox(this, wxID_ANY, wxT("&Horz alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(halign), halign);
    m_radioVAlign = new wxRadioBox(this, wxID_ANY, wxT("&Vert alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(valign), valign);

    sizerLeft->Add(m_radioHAlign, 0, wxGROW | wxALL, 5);
    sizerLeft->Add(m_radioVAlign, 0, wxGROW | wxALL, 5);
#endif // wxHAS_BITMAPTOGGLEBUTTON

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    wxButton *btn = new wxButton(this, TogglePage_Reset, wxT("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, wxT("&Operations"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow = CreateSizerWithTextAndButton(TogglePage_ChangeLabel,
                                                     wxT("Change label"),
                                                     wxID_ANY,
                                                     &m_textLabel);
    m_textLabel->SetValue(wxT("&Toggle me!"));

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    // right pane
    m_sizerToggle = new wxBoxSizer(wxHORIZONTAL);
    m_sizerToggle->SetMinSize(150, 0);

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerTop->Add(m_sizerToggle, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // do create the main control
    Reset();
    CreateToggle();

    SetSizer(sizerTop);
}

void ToggleWidgetsPage::Reset()
{
#ifdef wxHAS_BITMAPTOGGLEBUTTON
    m_chkBitmapOnly->SetValue(false);
    m_chkFit->SetValue(true);
    m_chkTextAndBitmap->SetValue(false);
#if wxUSE_MARKUP
    m_chkUseMarkup->SetValue(false);
#endif // wxUSE_MARKUP
    m_chkUseBitmapClass->SetValue(true);

    m_chkUsePressed->SetValue(true);
    m_chkUseFocused->SetValue(true);
    m_chkUseCurrent->SetValue(true);
    m_chkUseDisabled->SetValue(true);

    m_radioImagePos->SetSelection(ToggleImagePos_Left);
    m_radioHAlign->SetSelection(ToggleHAlign_Centre);
    m_radioVAlign->SetSelection(ToggleVAlign_Centre);
#endif // wxHAS_BITMAPTOGGLEBUTTON

    if ( m_toggle )
    {
        m_toggle->SetValue(false);
    }
}

void ToggleWidgetsPage::CreateToggle()
{
    wxString label;
    bool value = false;

    if ( m_toggle )
    {
        label = m_toggle->GetLabel();
        value = m_toggle->GetValue();
        size_t count = m_sizerToggle->GetChildren().GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            m_sizerToggle->Remove(0);
        }

        delete m_toggle;
    }

    if ( label.empty() )
    {
        // creating for the first time or recreating a toggle button after bitmap
        // button
        label = m_textLabel->GetValue();
    }

    int flags = ms_defaultFlags;
#ifdef wxHAS_BITMAPTOGGLEBUTTON
    switch ( m_radioHAlign->GetSelection() )
    {
        case ToggleHAlign_Left:
            flags |= wxBU_LEFT;
            break;

        default:
            wxFAIL_MSG(wxT("unexpected radiobox selection"));
            // fall through

        case ToggleHAlign_Centre:
            break;

        case ToggleHAlign_Right:
            flags |= wxBU_RIGHT;
            break;
    }

    switch ( m_radioVAlign->GetSelection() )
    {
        case ToggleVAlign_Top:
            flags |= wxBU_TOP;
            break;

        default:
            wxFAIL_MSG(wxT("unexpected radiobox selection"));
            // fall through

        case ToggleVAlign_Centre:
            // centre vertical alignment is the default (no style)
            break;

        case ToggleVAlign_Bottom:
            flags |= wxBU_BOTTOM;
            break;
    }
#endif // wxHAS_BITMAPTOGGLEBUTTON

#ifdef wxHAS_BITMAPTOGGLEBUTTON
    bool showsBitmap = false;
    if ( m_chkBitmapOnly->GetValue() )
    {
        showsBitmap = true;

        wxToggleButton *btgl;
        if ( m_chkUseBitmapClass->GetValue() )
        {
          btgl = new wxBitmapToggleButton(this, TogglePage_Picker,
                                          CreateBitmap(wxT("normal")));
        }
        else
        {
          btgl = new wxToggleButton(this, TogglePage_Picker, wxT(""));
          btgl->SetBitmapLabel(CreateBitmap(wxT("normal")));
        }
#ifdef wxHAS_ANY_BUTTON
        if ( m_chkUsePressed->GetValue() )
            btgl->SetBitmapPressed(CreateBitmap(wxT("pushed")));
        if ( m_chkUseFocused->GetValue() )
            btgl->SetBitmapFocus(CreateBitmap(wxT("focused")));
        if ( m_chkUseCurrent->GetValue() )
            btgl->SetBitmapCurrent(CreateBitmap(wxT("hover")));
        if ( m_chkUseDisabled->GetValue() )
            btgl->SetBitmapDisabled(CreateBitmap(wxT("disabled")));
#endif // wxHAS_ANY_BUTTON
        m_toggle = btgl;
    }
    else // normal button
#endif // wxHAS_BITMAPTOGGLEBUTTON
    {
        m_toggle = new wxToggleButton(this, TogglePage_Picker, label,
                                      wxDefaultPosition, wxDefaultSize,
                                      flags);
    }
    m_toggle->SetValue(value);

#ifdef wxHAS_BITMAPTOGGLEBUTTON
#ifdef wxHAS_ANY_BUTTON
    if ( !showsBitmap && m_chkTextAndBitmap->GetValue() )
    {
        showsBitmap = true;

        static const wxDirection positions[] =
        {
            wxLEFT, wxRIGHT, wxTOP, wxBOTTOM
        };

        m_toggle->SetBitmap(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_BUTTON),
                            positions[m_radioImagePos->GetSelection()]);

        if ( m_chkUsePressed->GetValue() )
            m_toggle->SetBitmapPressed(wxArtProvider::GetIcon(wxART_HELP, wxART_BUTTON));
        if ( m_chkUseFocused->GetValue() )
            m_toggle->SetBitmapFocus(wxArtProvider::GetIcon(wxART_ERROR, wxART_BUTTON));
        if ( m_chkUseCurrent->GetValue() )
            m_toggle->SetBitmapCurrent(wxArtProvider::GetIcon(wxART_WARNING, wxART_BUTTON));
        if ( m_chkUseDisabled->GetValue() )
            m_toggle->SetBitmapDisabled(wxArtProvider::GetIcon(wxART_MISSING_IMAGE, wxART_BUTTON));
    }
#endif // wxHAS_ANY_BUTTON

    m_chkUseBitmapClass->Enable(showsBitmap);

    m_chkUsePressed->Enable(showsBitmap);
    m_chkUseFocused->Enable(showsBitmap);
    m_chkUseCurrent->Enable(showsBitmap);
    m_chkUseDisabled->Enable(showsBitmap);
#endif // wxHAS_BITMAPTOGGLEBUTTON

    m_sizerToggle->Add(0, 0, 1, wxCENTRE);
    m_sizerToggle->Add(m_toggle, 1, wxCENTRE);
    m_sizerToggle->Add(0, 0, 1, wxCENTRE);
    m_sizerToggle->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ToggleWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateToggle();
}

void ToggleWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateToggle();
}

void ToggleWidgetsPage::OnButtonChangeLabel(wxCommandEvent& WXUNUSED(event))
{
    const wxString labelText = m_textLabel->GetValue();

#if wxUSE_MARKUP
    if ( m_chkUseMarkup->GetValue() )
        m_toggle->SetLabelMarkup(labelText);
    else
#endif // wxUSE_MARKUP
        m_toggle->SetLabel(labelText);
}

#ifdef wxHAS_BITMAPTOGGLEBUTTON
// ----------------------------------------------------------------------------
// bitmap toggle button stuff
// ----------------------------------------------------------------------------

wxBitmap ToggleWidgetsPage::CreateBitmap(const wxString& label)
{
    wxBitmap bmp(180, 70); // shouldn't hardcode but it's simpler like this
    wxMemoryDC dc;
    dc.SelectObject(bmp);
    dc.SetBackground(wxBrush(*wxCYAN));
    dc.Clear();
    dc.SetTextForeground(*wxBLACK);
    dc.DrawLabel(wxStripMenuCodes(m_textLabel->GetValue()) + wxT("\n")
                    wxT("(") + label + wxT(" state)"),
                 wxArtProvider::GetBitmap(wxART_INFORMATION),
                 wxRect(10, 10, bmp.GetWidth() - 20, bmp.GetHeight() - 20),
                 wxALIGN_CENTRE);

    return bmp;
}
#endif // wxHAS_BITMAPTOGGLEBUTTON

#endif // wxUSE_TOGGLEBTN
