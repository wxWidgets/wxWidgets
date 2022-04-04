/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/fontdlgg.cpp
// Purpose:     Generic font dialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FONTDLG && (!defined(__WXGTK__) || defined(__WXGPE__) || defined(__WXUNIVERSAL__))

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/crt.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/listbox.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/layout.h"
    #include "wx/dcclient.h"
    #include "wx/choice.h"
    #include "wx/checkbox.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/sizer.h"
#endif

#include <string.h>
#include <stdlib.h>

#include "wx/fontdlg.h"
#include "wx/generic/fontdlgg.h"

#if USE_SPINCTRL_FOR_POINT_SIZE
#include "wx/spinctrl.h"
#endif

//-----------------------------------------------------------------------------
// helper class - wxFontPreviewer
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxFontPreviewer : public wxWindow
{
public:
    wxFontPreviewer(wxWindow *parent, const wxSize& sz = wxDefaultSize) : wxWindow(parent, wxID_ANY, wxDefaultPosition, sz)
    {
    }

private:
    void OnPaint(wxPaintEvent& event);
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxFontPreviewer, wxWindow)
    EVT_PAINT(wxFontPreviewer::OnPaint)
wxEND_EVENT_TABLE()

void wxFontPreviewer::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    wxSize size = GetSize();
    wxFont font = GetFont();

    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, size.x, size.y);

    if ( font.IsOk() )
    {
        dc.SetFont(font);
        dc.SetTextForeground(GetForegroundColour());
        dc.SetClippingRegion(2, 2, size.x-4, size.y-4);
        dc.DrawText(_("ABCDEFGabcdefg12345"),
                     10, (size.y - dc.GetTextExtent(wxT("X")).y)/2);
        dc.DestroyClippingRegion();
    }
}

//-----------------------------------------------------------------------------
// helper functions
//-----------------------------------------------------------------------------

static const wxChar *wxFontWeightIntToString(int weight)
{
    switch (weight)
    {
        case wxFONTWEIGHT_LIGHT:
            return wxT("Light");
        case wxFONTWEIGHT_BOLD:
            return wxT("Bold");
        case wxFONTWEIGHT_NORMAL:
        default:
            return wxT("Normal");
    }
}

static const wxChar *wxFontStyleIntToString(int style)
{
    switch (style)
    {
        case wxFONTSTYLE_ITALIC:
            return wxT("Italic");
        case wxFONTSTYLE_SLANT:
            return wxT("Slant");
        case wxFONTSTYLE_NORMAL:
            default:
            return wxT("Normal");
    }
}

static const wxChar *wxFontFamilyIntToString(int family)
{
    switch (family)
    {
        case wxFONTFAMILY_ROMAN:
            return wxT("Roman");
        case wxFONTFAMILY_DECORATIVE:
            return wxT("Decorative");
        case wxFONTFAMILY_MODERN:
            return wxT("Modern");
        case wxFONTFAMILY_SCRIPT:
            return wxT("Script");
        case wxFONTFAMILY_TELETYPE:
            return wxT("Teletype");
        case wxFONTFAMILY_SWISS:
        default:
            return wxT("Swiss");
    }
}

static wxFontFamily wxFontFamilyStringToInt(const wxString& family)
{
    if (family.empty())
        return wxFONTFAMILY_SWISS;

    if (wxStrcmp(family, wxT("Roman")) == 0)
        return wxFONTFAMILY_ROMAN;
    else if (wxStrcmp(family, wxT("Decorative")) == 0)
        return wxFONTFAMILY_DECORATIVE;
    else if (wxStrcmp(family, wxT("Modern")) == 0)
        return wxFONTFAMILY_MODERN;
    else if (wxStrcmp(family, wxT("Script")) == 0)
        return wxFONTFAMILY_SCRIPT;
    else if (wxStrcmp(family, wxT("Teletype")) == 0)
        return wxFONTFAMILY_TELETYPE;
    else return wxFONTFAMILY_SWISS;
}

static wxFontStyle wxFontStyleStringToInt(const wxString& style)
{
    if (style.empty())
        return wxFONTSTYLE_NORMAL;
    if (wxStrcmp(style, wxT("Italic")) == 0)
        return wxFONTSTYLE_ITALIC;
    else if (wxStrcmp(style, wxT("Slant")) == 0)
        return wxFONTSTYLE_SLANT;
    else
        return wxFONTSTYLE_NORMAL;
}

static wxFontWeight wxFontWeightStringToInt(const wxString& weight)
{
    if (weight.empty())
        return wxFONTWEIGHT_NORMAL;
    if (wxStrcmp(weight, wxT("Bold")) == 0)
        return wxFONTWEIGHT_BOLD;
    else if (wxStrcmp(weight, wxT("Light")) == 0)
        return wxFONTWEIGHT_LIGHT;
    else
        return wxFONTWEIGHT_NORMAL;
}

//-----------------------------------------------------------------------------
// wxGenericFontDialog
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxGenericFontDialog, wxDialog);

wxBEGIN_EVENT_TABLE(wxGenericFontDialog, wxDialog)
    EVT_CHECKBOX(wxID_FONT_UNDERLINE, wxGenericFontDialog::OnChangeFont)
    EVT_CHOICE(wxID_FONT_STYLE, wxGenericFontDialog::OnChangeFont)
    EVT_CHOICE(wxID_FONT_WEIGHT, wxGenericFontDialog::OnChangeFont)
    EVT_CHOICE(wxID_FONT_FAMILY, wxGenericFontDialog::OnChangeFont)
    EVT_CHOICE(wxID_FONT_COLOUR, wxGenericFontDialog::OnChangeFont)
#if USE_SPINCTRL_FOR_POINT_SIZE
    EVT_SPINCTRL(wxID_FONT_SIZE, wxGenericFontDialog::OnChangeSize)
    EVT_TEXT(wxID_FONT_SIZE, wxGenericFontDialog::OnChangeFont)
#else
    EVT_CHOICE(wxID_FONT_SIZE, wxGenericFontDialog::OnChangeFont)
#endif
    EVT_CLOSE(wxGenericFontDialog::OnCloseWindow)
wxEND_EVENT_TABLE()


#define NUM_COLS 48
static wxString wxColourDialogNames[NUM_COLS]={wxT("ORANGE"),
                    wxT("GOLDENROD"),
                    wxT("WHEAT"),
                    wxT("SPRING GREEN"),
                    wxT("SKY BLUE"),
                    wxT("SLATE BLUE"),
                    wxT("MEDIUM VIOLET RED"),
                    wxT("PURPLE"),

                    wxT("RED"),
                    wxT("YELLOW"),
                    wxT("MEDIUM SPRING GREEN"),
                    wxT("PALE GREEN"),
                    wxT("CYAN"),
                    wxT("LIGHT STEEL BLUE"),
                    wxT("ORCHID"),
                    wxT("LIGHT MAGENTA"),

                    wxT("BROWN"),
                    wxT("YELLOW"),
                    wxT("GREEN"),
                    wxT("CADET BLUE"),
                    wxT("MEDIUM BLUE"),
                    wxT("MAGENTA"),
                    wxT("MAROON"),
                    wxT("ORANGE RED"),

                    wxT("FIREBRICK"),
                    wxT("CORAL"),
                    wxT("FOREST GREEN"),
                    wxT("AQUAMARINE"),
                    wxT("BLUE"),
                    wxT("NAVY"),
                    wxT("THISTLE"),
                    wxT("MEDIUM VIOLET RED"),

                    wxT("INDIAN RED"),
                    wxT("GOLD"),
                    wxT("MEDIUM SEA GREEN"),
                    wxT("MEDIUM BLUE"),
                    wxT("MIDNIGHT BLUE"),
                    wxT("GREY"),
                    wxT("PURPLE"),
                    wxT("KHAKI"),

                    wxT("BLACK"),
                    wxT("MEDIUM FOREST GREEN"),
                    wxT("KHAKI"),
                    wxT("DARK GREY"),
                    wxT("SEA GREEN"),
                    wxT("LIGHT GREY"),
                    wxT("MEDIUM SLATE BLUE"),
                    wxT("WHITE")
                    };

/*
 * Generic wxFontDialog
 */

void wxGenericFontDialog::Init()
{
    m_useEvents = false;
    m_previewer = NULL;
    Create( m_parent ) ;
}

wxGenericFontDialog::~wxGenericFontDialog()
{
}

void wxGenericFontDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}

bool wxGenericFontDialog::DoCreate(wxWindow *parent)
{
    parent = GetParentForModalDialog(parent, 0);

    if ( !wxDialog::Create( parent , wxID_ANY , wxT("Choose Font") ,
                            wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE,
        wxT("fontdialog") ) )
    {
        wxFAIL_MSG( wxT("wxFontDialog creation failed") );
        return false;
    }

    InitializeFont();
    CreateWidgets();

    // sets initial font in preview area
    DoChangeFont();

    return true;
}

int wxGenericFontDialog::ShowModal()
{
    int ret = wxDialog::ShowModal();

    if (ret != wxID_CANCEL)
    {
        m_fontData.m_chosenFont = m_dialogFont;
    }

    return ret;
}

// This should be application-settable
static bool ShowToolTips() { return false; }

void wxGenericFontDialog::CreateWidgets()
{
    wxString *families = new wxString[6],
             *styles = new wxString[3],
             *weights = new wxString[3];
    families[0] =  _("Roman");
    families[1] = _("Decorative");
    families[2] = _("Modern");
    families[3] = _("Script");
    families[4] = _("Swiss" );
    families[5] = _("Teletype" );
    styles[0] = _("Normal");
    styles[1] = _("Italic");
    styles[2] = _("Slant");
    weights[0] = _("Normal");
    weights[1] = _("Light");
    weights[2] = _("Bold");

#if !USE_SPINCTRL_FOR_POINT_SIZE
    wxString *pointSizes = new wxString[40];
    int i;
    for ( i = 0; i < 40; i++)
    {
        wxChar buf[5];
        wxSprintf(buf, wxT("%d"), i + 1);
        pointSizes[i] = buf;
    }
#endif

    // layout

    bool is_pda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);
    int noCols, noRows;
    if (is_pda)
    {
        noCols = 2; noRows = 3;
    }
    else
    {
        noCols = 3; noRows = 2;
    }

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemGridSizer4 = new wxFlexGridSizer(noRows, noCols, 0, 0);
    itemBoxSizer3->Add(itemGridSizer4, 0, wxGROW, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemGridSizer4->Add(itemBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW, 5);
    wxStaticText* itemStaticText6 = new wxStaticText( this, wxID_STATIC, _("&Font family:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_familyChoice = new wxChoice( this, wxID_FONT_FAMILY, wxDefaultPosition, wxDefaultSize, 5, families, 0 );
    m_familyChoice->SetHelpText(_("The font family."));
    if (ShowToolTips())
        m_familyChoice->SetToolTip(_("The font family."));
    itemBoxSizer5->Add(m_familyChoice, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemGridSizer4->Add(itemBoxSizer8, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW, 5);
    wxStaticText* itemStaticText9 = new wxStaticText( this, wxID_STATIC, _("&Style:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(itemStaticText9, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_styleChoice = new wxChoice( this, wxID_FONT_STYLE, wxDefaultPosition, wxDefaultSize, 3, styles, 0 );
    m_styleChoice->SetHelpText(_("The font style."));
    if (ShowToolTips())
        m_styleChoice->SetToolTip(_("The font style."));
    itemBoxSizer8->Add(m_styleChoice, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    itemGridSizer4->Add(itemBoxSizer11, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW, 5);
    wxStaticText* itemStaticText12 = new wxStaticText( this, wxID_STATIC, _("&Weight:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(itemStaticText12, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_weightChoice = new wxChoice( this, wxID_FONT_WEIGHT, wxDefaultPosition, wxDefaultSize, 3, weights, 0 );
    m_weightChoice->SetHelpText(_("The font weight."));
    if (ShowToolTips())
        m_weightChoice->SetToolTip(_("The font weight."));
    itemBoxSizer11->Add(m_weightChoice, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxVERTICAL);
    itemGridSizer4->Add(itemBoxSizer14, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW, 5);
    m_colourChoice = NULL;
    if (m_fontData.GetEnableEffects())
    {
        wxStaticText* itemStaticText15 = new wxStaticText( this, wxID_STATIC, _("C&olour:"), wxDefaultPosition, wxDefaultSize, 0 );
        itemBoxSizer14->Add(itemStaticText15, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

        wxSize colourSize = wxDefaultSize;
        if (is_pda)
            colourSize.x = 100;

        m_colourChoice = new wxChoice( this, wxID_FONT_COLOUR, wxDefaultPosition, colourSize, NUM_COLS, wxColourDialogNames, 0 );
        m_colourChoice->SetHelpText(_("The font colour."));
        if (ShowToolTips())
            m_colourChoice->SetToolTip(_("The font colour."));
        itemBoxSizer14->Add(m_colourChoice, 0, wxALIGN_LEFT|wxALL, 5);
    }

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxVERTICAL);
    itemGridSizer4->Add(itemBoxSizer17, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW, 5);
    wxStaticText* itemStaticText18 = new wxStaticText( this, wxID_STATIC, _("&Point size:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(itemStaticText18, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

#if USE_SPINCTRL_FOR_POINT_SIZE
    m_pointSizeSpin = new wxSpinCtrl(this, wxID_FONT_SIZE, wxT("12"), wxDefaultPosition, wxSize(80, wxDefaultCoord), wxSP_ARROW_KEYS, 1, 500, 12);
    m_pointSizeSpin->SetHelpText(_("The font point size."));
    if (ShowToolTips())
        m_pointSizeSpin->SetToolTip(_("The font point size."));
    itemBoxSizer17->Add(m_pointSizeSpin, 0, wxALIGN_LEFT|wxALL, 5);
#else
    m_pointSizeChoice = new wxChoice( this, wxID_FONT_SIZE, wxDefaultPosition, wxDefaultSize, 40, pointSizes, 0 );
    m_pointSizeChoice->SetHelpText(_("The font point size."));
    if (ShowToolTips())
        m_pointSizeChoice->SetToolTip(_("The font point size."));
    itemBoxSizer17->Add(m_pointSizeChoice, 0, wxALIGN_LEFT|wxALL, 5);
#endif

    m_underLineCheckBox = NULL;
    if (m_fontData.GetEnableEffects())
    {
        wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxVERTICAL);
        itemGridSizer4->Add(itemBoxSizer20, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
        m_underLineCheckBox = new wxCheckBox( this, wxID_FONT_UNDERLINE, _("&Underline"), wxDefaultPosition, wxDefaultSize, 0 );
        m_underLineCheckBox->SetValue(false);
        m_underLineCheckBox->SetHelpText(_("Whether the font is underlined."));
        if (ShowToolTips())
            m_underLineCheckBox->SetToolTip(_("Whether the font is underlined."));
        itemBoxSizer20->Add(m_underLineCheckBox, 0, wxALIGN_LEFT|wxALL, 5);
    }

    if (!is_pda)
        itemBoxSizer3->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( this, wxID_STATIC, _("Preview:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText23, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_previewer = new wxFontPreviewer( this );
    m_previewer->SetHelpText(_("Shows the font preview."));
    if (ShowToolTips())
        m_previewer->SetToolTip(_("Shows the font preview."));
    itemBoxSizer3->Add(m_previewer, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer25, 0, wxGROW, 5);
    itemBoxSizer25->Add(5, 5, 1, wxGROW|wxALL, 5);

#ifdef __WXMAC__
    wxButton* itemButton28 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    if (ShowToolTips())
        itemButton28->SetToolTip(_("Click to cancel the font selection."));
    itemBoxSizer25->Add(itemButton28, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton27 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton27->SetDefault();
    itemButton27->SetHelpText(_("Click to confirm the font selection."));
    if (ShowToolTips())
        itemButton27->SetToolTip(_("Click to confirm the font selection."));
    itemBoxSizer25->Add(itemButton27, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
#else
    wxButton* itemButton27 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton27->SetDefault();
    itemButton27->SetHelpText(_("Click to confirm the font selection."));
    if (ShowToolTips())
        itemButton27->SetToolTip(_("Click to confirm the font selection."));
    itemBoxSizer25->Add(itemButton27, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton28 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    if (ShowToolTips())
        itemButton28->SetToolTip(_("Click to cancel the font selection."));
    itemBoxSizer25->Add(itemButton28, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
#endif

    m_familyChoice->SetStringSelection( wxFontFamilyIntToString(m_dialogFont.GetFamily()) );
    m_styleChoice->SetStringSelection(wxFontStyleIntToString(m_dialogFont.GetStyle()));
    m_weightChoice->SetStringSelection(wxFontWeightIntToString(m_dialogFont.GetWeight()));

    if (m_colourChoice)
    {
        wxString name(wxTheColourDatabase->FindName(m_fontData.GetColour()));
        if ( name.empty() )
            m_colourChoice->SetStringSelection(wxT("BLACK"));
        else
            m_colourChoice->SetStringSelection(name);
    }

    if (m_underLineCheckBox)
    {
        m_underLineCheckBox->SetValue(m_dialogFont.GetUnderlined());
    }

#if USE_SPINCTRL_FOR_POINT_SIZE
    m_pointSizeSpin->SetValue(m_dialogFont.GetPointSize());
#else
    m_pointSizeChoice->SetSelection(m_dialogFont.GetPointSize()-1);
#endif

    GetSizer()->SetItemMinSize(m_previewer, is_pda ? 100 : 430, is_pda ? 40 : 100);
    GetSizer()->SetSizeHints(this);

    Centre(wxBOTH);

    delete[] families;
    delete[] styles;
    delete[] weights;
#if !USE_SPINCTRL_FOR_POINT_SIZE
    delete[] pointSizes;
#endif

    // Don't block events any more
    m_useEvents = true;

}

void wxGenericFontDialog::InitializeFont()
{
    wxFontFamily fontFamily = wxFONTFAMILY_SWISS;
    wxFontWeight fontWeight = wxFONTWEIGHT_NORMAL;
    wxFontStyle fontStyle = wxFONTSTYLE_NORMAL;
    int fontSize = 12;
    bool fontUnderline = false;

    if (m_fontData.m_initialFont.IsOk())
    {
        fontFamily = m_fontData.m_initialFont.GetFamily();
        fontWeight = m_fontData.m_initialFont.GetWeight();
        fontStyle = m_fontData.m_initialFont.GetStyle();
        fontSize = m_fontData.m_initialFont.GetPointSize();
        fontUnderline = m_fontData.m_initialFont.GetUnderlined();
    }

    m_dialogFont = wxFont(fontSize, fontFamily, fontStyle,
                          fontWeight, fontUnderline);

    if (m_previewer)
        m_previewer->SetFont(m_dialogFont);
}

void wxGenericFontDialog::OnChangeFont(wxCommandEvent& WXUNUSED(event))
{
    DoChangeFont();
}

void wxGenericFontDialog::DoChangeFont()
{
    if (!m_useEvents) return;

    wxFontFamily fontFamily = wxFontFamilyStringToInt(m_familyChoice->GetStringSelection());
    wxFontWeight fontWeight = wxFontWeightStringToInt(m_weightChoice->GetStringSelection());
    wxFontStyle fontStyle = wxFontStyleStringToInt(m_styleChoice->GetStringSelection());
#if USE_SPINCTRL_FOR_POINT_SIZE
    int fontSize = m_pointSizeSpin->GetValue();
#else
    int fontSize = wxAtoi(m_pointSizeChoice->GetStringSelection());
#endif

    // Start with previous underline setting, we want to retain it even if we can't edit it
    // m_dialogFont is always initialized because of the call to InitializeFont
    int fontUnderline = m_dialogFont.GetUnderlined();

    if (m_underLineCheckBox)
    {
        fontUnderline = m_underLineCheckBox->GetValue();
    }

    m_dialogFont = wxFont(fontSize, fontFamily, fontStyle, fontWeight, (fontUnderline != 0));
    m_previewer->SetFont(m_dialogFont);

    if ( m_colourChoice )
    {
        if ( !m_colourChoice->GetStringSelection().empty() )
        {
            wxColour col = wxTheColourDatabase->Find(m_colourChoice->GetStringSelection());
            if (col.IsOk())
            {
                m_fontData.m_fontColour = col;
            }
        }
    }
    // Update color here so that we can also use the color originally passed in
    // (EnableEffects may be false)
    if (m_fontData.m_fontColour.IsOk())
        m_previewer->SetForegroundColour(m_fontData.m_fontColour);

    m_previewer->Refresh();
}

#if USE_SPINCTRL_FOR_POINT_SIZE
void wxGenericFontDialog::OnChangeSize(wxSpinEvent& WXUNUSED(event))
{
    DoChangeFont();
}
#endif

#endif
    // wxUSE_FONTDLG
