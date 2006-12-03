/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        clrpicker.cpp
// Purpose:     Shows wxColourPickerCtrl
// Author:      Francesco Montorsi
// Created:     20/6/2006
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

#if wxUSE_COLOURPICKERCTRL

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

#include "wx/clrpicker.h"
#include "widgets.h"

#include "icons/clrpicker.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    PickerPage_Reset = wxID_HIGHEST,
    PickerPage_Colour
};


// ----------------------------------------------------------------------------
// ColourPickerWidgetsPage
// ----------------------------------------------------------------------------

class ColourPickerWidgetsPage : public WidgetsPage
{
public:
    ColourPickerWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~ColourPickerWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_clrPicker; }
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


    void OnColourChange(wxColourPickerEvent &ev);
    void OnCheckBox(wxCommandEvent &ev);
    void OnButtonReset(wxCommandEvent &ev);

    // the picker
    wxColourPickerCtrl *m_clrPicker;


    // other controls
    // --------------

    wxCheckBox *m_chkColourTextCtrl,
               *m_chkColourShowLabel;
    wxBoxSizer *m_sizer;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(ColourPickerWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ColourPickerWidgetsPage, WidgetsPage)
    EVT_BUTTON(PickerPage_Reset, ColourPickerWidgetsPage::OnButtonReset)

    EVT_COLOURPICKER_CHANGED(PickerPage_Colour, ColourPickerWidgetsPage::OnColourChange)

    EVT_CHECKBOX(wxID_ANY, ColourPickerWidgetsPage::OnCheckBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXGTK24__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(ColourPickerWidgetsPage, _T("ColourPicker"),
                       PICKER_CTRLS | FAMILY_CTRLS);

ColourPickerWidgetsPage::ColourPickerWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, clrpicker_xpm)
{
}

void ColourPickerWidgetsPage::CreateContent()
{
    // left pane
    wxSizer *boxleft = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer *clrbox = new wxStaticBoxSizer(wxVERTICAL, this, _T("&ColourPicker style"));
    m_chkColourTextCtrl = CreateCheckBoxAndAddToSizer(clrbox, _T("With textctrl"), false);
    m_chkColourShowLabel = CreateCheckBoxAndAddToSizer(clrbox, _T("With label"), false);
    boxleft->Add(clrbox, 0, wxALL|wxGROW, 5);

    boxleft->Add(new wxButton(this, PickerPage_Reset, _T("&Reset")),
                 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    Reset();    // set checkboxes state

    // create pickers
    m_clrPicker = NULL;
    CreatePicker();

    // right pane
    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_sizer->Add(1, 1, 1, wxGROW | wxALL, 5); // spacer
    m_sizer->Add(m_clrPicker, 0, wxALIGN_CENTER|wxALL, 5);
    m_sizer->Add(1, 1, 1, wxGROW | wxALL, 5); // spacer

    // global pane
    wxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
    sz->Add(boxleft, 0, wxGROW|wxALL, 5);
    sz->Add(m_sizer, 1, wxGROW|wxALL, 5);

    SetSizer(sz);
}

void ColourPickerWidgetsPage::CreatePicker()
{
    delete m_clrPicker;

    m_clrPicker = new wxColourPickerCtrl(this, PickerPage_Colour, *wxRED,
                                            wxDefaultPosition, wxDefaultSize,
                                            GetPickerStyle());
}

long ColourPickerWidgetsPage::GetPickerStyle()
{
    long style = 0;

    if ( m_chkColourTextCtrl->GetValue() )
        style |= wxCLRP_USE_TEXTCTRL;

    if ( m_chkColourShowLabel->GetValue() )
        style |= wxCLRP_SHOW_LABEL;

    return style;
}

void ColourPickerWidgetsPage::RecreatePicker()
{
    m_sizer->Remove(1);
    CreatePicker();
    m_sizer->Insert(1, m_clrPicker, 0, wxALIGN_CENTER|wxALL, 5);

    m_sizer->Layout();
}

void ColourPickerWidgetsPage::Reset()
{
    m_chkColourTextCtrl->SetValue((wxCLRP_DEFAULT_STYLE & wxCLRP_USE_TEXTCTRL) != 0);
    m_chkColourShowLabel->SetValue((wxCLRP_DEFAULT_STYLE & wxCLRP_SHOW_LABEL) != 0);
}


// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void ColourPickerWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();
    RecreatePicker();
}

void ColourPickerWidgetsPage::OnColourChange(wxColourPickerEvent& event)
{
    wxLogMessage(wxT("The colour changed to '%s' !"),
                 event.GetColour().GetAsString(wxC2S_CSS_SYNTAX).c_str());
}

void ColourPickerWidgetsPage::OnCheckBox(wxCommandEvent &event)
{
    if (event.GetEventObject() == m_chkColourTextCtrl ||
        event.GetEventObject() == m_chkColourShowLabel)
        RecreatePicker();
}

#endif  //  wxUSE_COLOURPICKERCTRL
