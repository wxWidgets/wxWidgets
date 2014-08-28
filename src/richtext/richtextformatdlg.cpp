/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextformatdlg.cpp
// Purpose:     Formatting dialog for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2006-10-01
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextformatdlg.h"

#ifndef WX_PRECOMP
    #include "wx/listbox.h"
    #include "wx/combobox.h"
    #include "wx/textctrl.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
    #include "wx/statline.h"
    #include "wx/radiobut.h"
    #include "wx/icon.h"
    #include "wx/bitmap.h"
    #include "wx/dcclient.h"
    #include "wx/frame.h"
    #include "wx/checkbox.h"
    #include "wx/button.h"
    #include "wx/wxcrtvararg.h"
#endif // WX_PRECOMP

#include "wx/bookctrl.h"
#include "wx/colordlg.h"
#include "wx/settings.h"
#include "wx/module.h"
#include "wx/imaglist.h"

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"

#ifdef __WXMAC__
#include "../../src/richtext/richtextfontpage.cpp"
#include "../../src/richtext/richtextindentspage.cpp"
#include "../../src/richtext/richtexttabspage.cpp"
#include "../../src/richtext/richtextbulletspage.cpp"
#include "../../src/richtext/richtextstylepage.cpp"
#include "../../src/richtext/richtextliststylepage.cpp"
#include "../../src/richtext/richtextsizepage.cpp"
#include "../../src/richtext/richtextmarginspage.cpp"
#include "../../src/richtext/richtextborderspage.cpp"
#include "../../src/richtext/richtextbackgroundpage.cpp"
#else
#include "richtextfontpage.cpp"
#include "richtextindentspage.cpp"
#include "richtexttabspage.cpp"
#include "richtextbulletspage.cpp"
#include "richtextmarginspage.cpp"
#include "richtextsizepage.cpp"
#include "richtextborderspage.cpp"
#include "richtextbackgroundpage.cpp"
#include "richtextliststylepage.cpp"
#include "richtextstylepage.cpp"
#endif

#if 0 // def __WXMAC__
#define wxRICHTEXT_USE_TOOLBOOK 1
#else
#define wxRICHTEXT_USE_TOOLBOOK 0
#endif

bool wxRichTextFormattingDialog::sm_showToolTips = false;
bool wxRichTextFormattingDialog::sm_restoreLastPage = true;
int wxRichTextFormattingDialog::sm_lastPage = -1;

IMPLEMENT_CLASS(wxRichTextDialogPage, wxPanel)

IMPLEMENT_CLASS(wxRichTextFormattingDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(wxRichTextFormattingDialog, wxPropertySheetDialog)
    EVT_BOOKCTRL_PAGE_CHANGED(wxID_ANY, wxRichTextFormattingDialog::OnTabChanged)
    EVT_BUTTON(wxID_HELP, wxRichTextFormattingDialog::OnHelp)
    EVT_UPDATE_UI(wxID_HELP, wxRichTextFormattingDialog::OnUpdateHelp)
END_EVENT_TABLE()

IMPLEMENT_HELP_PROVISION(wxRichTextFormattingDialog)

wxRichTextFormattingDialogFactory* wxRichTextFormattingDialog::ms_FormattingDialogFactory = NULL;

void wxRichTextFormattingDialog::Init()
{
    m_styleDefinition = NULL;
    m_styleSheet = NULL;
    m_object = NULL;
    m_options = 0;
    m_ignoreUpdates = false;
}

wxRichTextFormattingDialog::~wxRichTextFormattingDialog()
{
    int sel = GetBookCtrl()->GetSelection();
    if (sel != -1 && sel < (int) m_pageIds.GetCount())
        sm_lastPage = m_pageIds[sel];

    delete m_styleDefinition;
}

bool wxRichTextFormattingDialog::Create(long flags, wxWindow* parent, const wxString& title, wxWindowID id,
        const wxPoint& pos, const wxSize& sz, long style)
{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
#ifdef __WXMAC__
    SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

    int resizeBorder = wxRESIZE_BORDER;

    GetFormattingDialogFactory()->SetSheetStyle(this);

    wxPropertySheetDialog::Create(parent, id, title, pos, sz,
        style | (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, resizeBorder)
    );

    GetFormattingDialogFactory()->CreateButtons(this);
    GetFormattingDialogFactory()->CreatePages(flags, this);

    LayoutDialog();

    if (sm_restoreLastPage && sm_lastPage != -1)
    {
        int idx = m_pageIds.Index(sm_lastPage);
        if (idx != -1)
        {
            m_ignoreUpdates = true;
            GetBookCtrl()->SetSelection(idx);
            m_ignoreUpdates = false;
        }
    }
    return true;
}

/// Get attributes from the given range
bool wxRichTextFormattingDialog::GetStyle(wxRichTextCtrl* ctrl, const wxRichTextRange& range)
{
    if (ctrl->GetFocusObject()->GetStyleForRange(range.ToInternal(), m_attributes))
        return UpdateDisplay();
    else
        return false;
}

/// Apply attributes to the given range, only applying if necessary (wxRICHTEXT_SETSTYLE_OPTIMIZE)
bool wxRichTextFormattingDialog::ApplyStyle(wxRichTextCtrl* ctrl, const wxRichTextRange& range, int flags)
{
    return ctrl->SetStyleEx(range, m_attributes, flags);
}

// Apply attributes to the object being edited, if any
bool wxRichTextFormattingDialog::ApplyStyle(wxRichTextCtrl* ctrl, int flags)
{
    if (GetObject())
    {
        ctrl->SetStyle(GetObject(), m_attributes, flags);
        return true;
    }
    else
        return false;
}

/// Set the attributes and optionally update the display
bool wxRichTextFormattingDialog::SetStyle(const wxRichTextAttr& style, bool update)
{
    m_attributes = style;
    if (update)
        UpdateDisplay();
    return true;
}

/// Set the style definition and optionally update the display
bool wxRichTextFormattingDialog::SetStyleDefinition(const wxRichTextStyleDefinition& styleDef, wxRichTextStyleSheet* sheet, bool update)
{
    m_styleSheet = sheet;

    if (m_styleDefinition)
        delete m_styleDefinition;
    m_styleDefinition = styleDef.Clone();

    return SetStyle(m_styleDefinition->GetStyle(), update);
}

/// Transfers the data and from to the window
bool wxRichTextFormattingDialog::TransferDataToWindow()
{
    if (m_styleDefinition)
        m_attributes = m_styleDefinition->GetStyle();

    if (!wxPropertySheetDialog::TransferDataToWindow())
        return false;

    return true;
}

bool wxRichTextFormattingDialog::TransferDataFromWindow()
{
    if (!wxPropertySheetDialog::TransferDataFromWindow())
        return false;

    if (m_styleDefinition)
        m_styleDefinition->GetStyle() = m_attributes;

    return true;
}

/// Update the display
bool wxRichTextFormattingDialog::UpdateDisplay()
{
    return TransferDataToWindow();
}

/// Apply the styles when a different tab is selected, so the previews are
/// up to date
void wxRichTextFormattingDialog::OnTabChanged(wxBookCtrlEvent& event)
{
    if (m_ignoreUpdates)
        return;

    if (GetBookCtrl() != event.GetEventObject())
    {
        event.Skip();
        return;
    }

    int oldPageId = event.GetOldSelection();
    if (oldPageId != -1)
    {
        wxWindow* page = GetBookCtrl()->GetPage(oldPageId);
        if (page)
            page->TransferDataFromWindow();
    }

    int pageId = event.GetSelection();
    if (pageId != -1)
    {
        wxWindow* page = GetBookCtrl()->GetPage(pageId);
        if (page)
            page->TransferDataToWindow();
    }
}

/// Respond to help command
void wxRichTextFormattingDialog::OnHelp(wxCommandEvent& event)
{
    int selPage = GetBookCtrl()->GetSelection();
    if (selPage != wxNOT_FOUND)
    {
        int pageId = -1;
        if (selPage < (int) m_pageIds.GetCount())
            pageId = m_pageIds[selPage];
        if (!GetFormattingDialogFactory()->ShowHelp(pageId, this))
            event.Skip();
    }
}

void wxRichTextFormattingDialog::OnUpdateHelp(wxUpdateUIEvent& event)
{
    event.Enable(true);
}

void wxRichTextFormattingDialog::SetFormattingDialogFactory(wxRichTextFormattingDialogFactory* factory)
{
    if (ms_FormattingDialogFactory)
        delete ms_FormattingDialogFactory;
    ms_FormattingDialogFactory = factory;
}

// Find a page by class
wxWindow* wxRichTextFormattingDialog::FindPage(wxClassInfo* info) const
{
    size_t i;
    for (i = 0; i < GetBookCtrl()->GetPageCount(); i++)
    {
        wxWindow* w = GetBookCtrl()->GetPage(i);
        if (w && w->GetClassInfo() == info)
            return w;
    }
    return NULL;
}


/*!
 * Factory for formatting dialog
 */

/// Create all pages, under the dialog's book control, also calling AddPage
bool wxRichTextFormattingDialogFactory::CreatePages(long pages, wxRichTextFormattingDialog* dialog)
{
    if (dialog->GetImageList())
        dialog->GetBookCtrl()->SetImageList(dialog->GetImageList());

    int availablePageCount = GetPageIdCount();
    int i;
    bool selected = false;
    for (i = 0; i < availablePageCount; i ++)
    {
        int pageId = GetPageId(i);
        if (pageId != -1 && (pages & pageId))
        {
            wxString title;
            wxPanel* panel = CreatePage(pageId, title, dialog);
            wxASSERT( panel != NULL );
            if (panel)
            {
                int imageIndex = GetPageImage(pageId);
                dialog->GetBookCtrl()->AddPage(panel, title, !selected, imageIndex);
                selected = true;

                dialog->AddPageId(pageId);
            }
        }
    }

    return true;
}

/// Create a page, given a page identifier
wxPanel* wxRichTextFormattingDialogFactory::CreatePage(int page, wxString& title, wxRichTextFormattingDialog* dialog)
{
    if (page == wxRICHTEXT_FORMAT_STYLE_EDITOR)
    {
        wxRichTextStylePage* page = new wxRichTextStylePage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("Style");
        return page;
    }
    else if (page == wxRICHTEXT_FORMAT_FONT)
    {
        wxRichTextFontPage* page = new wxRichTextFontPage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("Font");
        return page;
    }
    else if (page == wxRICHTEXT_FORMAT_INDENTS_SPACING)
    {
        wxRichTextIndentsSpacingPage* page = new wxRichTextIndentsSpacingPage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("Indents && Spacing");
        return page;
    }
    else if (page == wxRICHTEXT_FORMAT_TABS)
    {
        wxRichTextTabsPage* page = new wxRichTextTabsPage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("Tabs");
        return page;
    }
    else if (page == wxRICHTEXT_FORMAT_BULLETS)
    {
        wxRichTextBulletsPage* page = new wxRichTextBulletsPage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("Bullets");
        return page;
    }
    else if (page == wxRICHTEXT_FORMAT_LIST_STYLE)
    {
        wxRichTextListStylePage* page = new wxRichTextListStylePage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("List Style");
        return page;
    }
    else if (page == wxRICHTEXT_FORMAT_SIZE)
    {
        wxRichTextSizePage* page = new wxRichTextSizePage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("Size");
        return page;
    }
    else if (page == wxRICHTEXT_FORMAT_MARGINS)
    {
        wxRichTextMarginsPage* page = new wxRichTextMarginsPage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("Margins");
        return page;
    }
    else if (page == wxRICHTEXT_FORMAT_BORDERS)
    {
        wxRichTextBordersPage* page = new wxRichTextBordersPage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("Borders");
        return page;
    }
    else if (page == wxRICHTEXT_FORMAT_BACKGROUND)
    {
        wxRichTextBackgroundPage* page = new wxRichTextBackgroundPage(dialog->GetBookCtrl(), wxID_ANY);
        title = _("Background");
        return page;
    }
    else
        return NULL;
}

/// Enumerate all available page identifiers
int wxRichTextFormattingDialogFactory::GetPageId(int i) const
{
    int pages[] = {
        wxRICHTEXT_FORMAT_STYLE_EDITOR,
        wxRICHTEXT_FORMAT_FONT,
        wxRICHTEXT_FORMAT_INDENTS_SPACING,
        wxRICHTEXT_FORMAT_BULLETS,
        wxRICHTEXT_FORMAT_TABS,
        wxRICHTEXT_FORMAT_LIST_STYLE,
        wxRICHTEXT_FORMAT_SIZE,
        wxRICHTEXT_FORMAT_MARGINS,
        wxRICHTEXT_FORMAT_BORDERS,
        wxRICHTEXT_FORMAT_BACKGROUND
        };

    if (i < 0 || i >= GetPageIdCount())
        return -1;

    return pages[i];
}

/// Get the number of available page identifiers
int wxRichTextFormattingDialogFactory::GetPageIdCount() const
{
    return 10;
}

/// Set the sheet style, called at the start of wxRichTextFormattingDialog::Create
bool wxRichTextFormattingDialogFactory::SetSheetStyle(wxRichTextFormattingDialog* dialog)
{
#if wxRICHTEXT_USE_TOOLBOOK
    int sheetStyle = wxPROPSHEET_SHRINKTOFIT;
#ifdef __WXMAC__
    sheetStyle |= wxPROPSHEET_BUTTONTOOLBOOK;
#else
    sheetStyle |= wxPROPSHEET_TOOLBOOK;
#endif

    dialog->SetSheetStyle(sheetStyle);
    dialog->SetSheetInnerBorder(0);
    dialog->SetSheetOuterBorder(0);
#else
    wxUnusedVar(dialog);
#endif // wxRICHTEXT_USE_TOOLBOOK

    return true;
}

/// Create the main dialog buttons
bool wxRichTextFormattingDialogFactory::CreateButtons(wxRichTextFormattingDialog* dialog)
{
    int flags = wxOK|wxCANCEL;
#ifndef __WXWINCE__
    if (dialog->GetWindowStyleFlag() & wxRICHTEXT_FORMAT_HELP_BUTTON)
        flags |= wxHELP;
#endif

    // If using a toolbook, also follow Mac style and don't create buttons
#if !wxRICHTEXT_USE_TOOLBOOK
    dialog->CreateButtons(flags);
#endif

    return true;
}

// Invoke help for the dialog
bool wxRichTextFormattingDialogFactory::ShowHelp(int WXUNUSED(page), wxRichTextFormattingDialog* dialog)
{
    wxRichTextDialogPage* window = NULL;
    int sel = dialog->GetBookCtrl()->GetSelection();
    if (sel != -1)
        window = wxDynamicCast(dialog->GetBookCtrl()->GetPage(sel), wxRichTextDialogPage);
    if (window && window->GetHelpId() != -1)
    {
        if (window->GetUICustomization())
            return window->GetUICustomization()->ShowHelp(dialog, window->GetHelpId());
        else if (dialog->GetUICustomization())
            return dialog->GetUICustomization()->ShowHelp(dialog, window->GetHelpId());
        else
            return false;
    }
    else if (dialog->GetHelpId() != -1 && dialog->GetUICustomization())
        return dialog->ShowHelp(dialog);
    else
        return false;
}

/*
 * Module to initialise and clean up handlers
 */

class wxRichTextFormattingDialogModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxRichTextFormattingDialogModule)
public:
    wxRichTextFormattingDialogModule() {}
    bool OnInit() wxOVERRIDE { wxRichTextFormattingDialog::SetFormattingDialogFactory(new wxRichTextFormattingDialogFactory); return true; }
    void OnExit() wxOVERRIDE { wxRichTextFormattingDialog::SetFormattingDialogFactory(NULL); }
};

IMPLEMENT_DYNAMIC_CLASS(wxRichTextFormattingDialogModule, wxModule)

/*
 * Font preview control
 */

BEGIN_EVENT_TABLE(wxRichTextFontPreviewCtrl, wxWindow)
    EVT_PAINT(wxRichTextFontPreviewCtrl::OnPaint)
END_EVENT_TABLE()

wxRichTextFontPreviewCtrl::wxRichTextFontPreviewCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& sz, long style)
{
    if ((style & wxBORDER_MASK) == wxBORDER_DEFAULT)
        style |= wxBORDER_THEME;

    wxWindow::Create(parent, id, pos, sz, style);

    SetBackgroundColour(*wxWHITE);
    m_textEffects = 0;
}

void wxRichTextFontPreviewCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    wxSize size = GetSize();
    wxFont font = GetFont();

    if ((GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT) || (GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT))
    {
        double size = static_cast<double>(font.GetPointSize()) / wxSCRIPT_MUL_FACTOR;
        font.SetPointSize( static_cast<int>(size) );
    }

    if ( font.IsOk() )
    {
        dc.SetFont(font);
        // Calculate vertical and horizontal centre
        wxCoord w = 0, h = 0;

        wxString text(_("ABCDEFGabcdefg12345"));
        if (GetTextEffects() & wxTEXT_ATTR_EFFECT_CAPITALS)
            text.MakeUpper();

        dc.GetTextExtent( text, &w, &h);
        int cx = wxMax(2, (size.x/2) - (w/2));
        int cy = wxMax(2, (size.y/2) - (h/2));

        if ( GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT )
            cy -= h/2;
        if ( GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT )
            cy += h/2;

        dc.SetTextForeground(GetForegroundColour());
        dc.SetClippingRegion(2, 2, size.x-4, size.y-4);
        dc.DrawText(text, cx, cy);

        if (GetTextEffects() & wxTEXT_ATTR_EFFECT_STRIKETHROUGH)
        {
            dc.SetPen(wxPen(GetForegroundColour(), 1));
            dc.DrawLine(cx, (int) (cy + h/2 + 0.5), cx + w, (int) (cy + h/2 + 0.5));
        }

        dc.DestroyClippingRegion();
    }
}

// Helper for pages to get the top-level dialog
wxRichTextFormattingDialog* wxRichTextFormattingDialog::GetDialog(wxWindow* win)
{
    wxWindow* p = win->GetParent();
    while (p && !wxDynamicCast(p, wxRichTextFormattingDialog))
        p = p->GetParent();
    wxRichTextFormattingDialog* dialog = wxDynamicCast(p, wxRichTextFormattingDialog);
    return dialog;
}

// Helper for pages to get the attributes
wxRichTextAttr* wxRichTextFormattingDialog::GetDialogAttributes(wxWindow* win)
{
    wxRichTextFormattingDialog* dialog = GetDialog(win);
    if (dialog)
        return & dialog->GetAttributes();
    else
        return NULL;
}

#if 0
// Helper for pages to get the attributes to reset
wxRichTextAttr* wxRichTextFormattingDialog::GetDialogResetAttributes(wxWindow* win)
{
    wxRichTextFormattingDialog* dialog = GetDialog(win);
    if (dialog)
        return & dialog->GetResetAttributes();
    else
        return NULL;
}
#endif

// Helper for pages to get the style
wxRichTextStyleDefinition* wxRichTextFormattingDialog::GetDialogStyleDefinition(wxWindow* win)
{
    wxRichTextFormattingDialog* dialog = GetDialog(win);
    if (dialog)
        return dialog->GetStyleDefinition();
    else
        return NULL;
}

void wxRichTextFormattingDialog::SetDimensionValue(wxTextAttrDimension& dim, wxTextCtrl* valueCtrl, wxComboBox* unitsCtrl, wxCheckBox* checkBox, wxArrayInt* units)
{
    int unitsIdx = 0;

    if (!dim.IsValid())
    {
        if (checkBox)
            checkBox->SetValue(false);
        valueCtrl->SetValue(wxT("0"));
        unitsCtrl->SetSelection(0);
    }
    else
    {
        if (checkBox)
            checkBox->SetValue(true);
        
        if (dim.GetUnits() == wxTEXT_ATTR_UNITS_PIXELS)
        {
            unitsIdx = 0;  // By default, the 1st in the list.
            valueCtrl->SetValue(wxString::Format(wxT("%d"), (int) dim.GetValue()));
        }
        else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_TENTHS_MM)
        {
            unitsIdx = 1; // By default, the 2nd in the list.
            float value = dim.GetValue() / 100.0f;
            valueCtrl->SetValue(wxString::Format(wxT("%.2f"), value));
        }
        else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_PERCENTAGE)
        {
            unitsIdx = 2; // By default, the 3rd in the list.
            valueCtrl->SetValue(wxString::Format(wxT("%d"), (int) dim.GetValue()));
        }
        else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT)
        {
            unitsIdx = 3; // By default, the 4th in the list.
            float value = dim.GetValue() / 100.0f;
            valueCtrl->SetValue(wxString::Format(wxT("%.2f"), value));
        }
        else if (dim.GetUnits() == wxTEXT_ATTR_UNITS_POINTS)
        {
            unitsIdx = 3; // By default, the 4th in the list (we don't have points and hundredths of points in the same list)
            valueCtrl->SetValue(wxString::Format(wxT("%d"), (int) dim.GetValue()));
        }
        
        if (units)
        {
            unitsIdx = units->Index(dim.GetUnits());
            if (unitsIdx == -1)
                unitsIdx = 0;
        }

        unitsCtrl->SetSelection(unitsIdx);
    }
}

void wxRichTextFormattingDialog::GetDimensionValue(wxTextAttrDimension& dim, wxTextCtrl* valueCtrl, wxComboBox* unitsCtrl, wxCheckBox* checkBox, wxArrayInt* units)
{
    if (checkBox && !checkBox->GetValue())
    {
        dim.Reset();
    }
    else
    {
        if (units)
        {
            int unit = (*units)[unitsCtrl->GetSelection()];
            dim.SetUnits((wxTextAttrUnits) unit);
        }
        else
        {
            if (unitsCtrl->GetSelection() == 0)
                dim.SetUnits(wxTEXT_ATTR_UNITS_PIXELS);
            else if (unitsCtrl->GetSelection() == 1)
                dim.SetUnits(wxTEXT_ATTR_UNITS_TENTHS_MM);
            else if (unitsCtrl->GetSelection() == 2)
                dim.SetUnits(wxTEXT_ATTR_UNITS_PERCENTAGE);
            else if (unitsCtrl->GetSelection() == 3)
                dim.SetUnits(wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT);
        }

        int value = 0;
        if (ConvertFromString(valueCtrl->GetValue(), value, dim.GetUnits()))
            dim.SetValue(value);
    }
}

bool wxRichTextFormattingDialog::ConvertFromString(const wxString& str, int& ret, int unit)
{
    if (unit == wxTEXT_ATTR_UNITS_PIXELS)
    {
        ret = wxAtoi(str);
        return true;
    }
    else if (unit == wxTEXT_ATTR_UNITS_TENTHS_MM)
    {
        float value = 0.0;
        wxSscanf(str.c_str(), wxT("%f"), &value);
        // Convert from cm
        ret = (int) ((value * 100.0) /* + 0.5 */);
        return true;
    }
    else if (unit == wxTEXT_ATTR_UNITS_PERCENTAGE)
    {
        ret = wxAtoi(str);
        return true;
    }
    else if (unit == wxTEXT_ATTR_UNITS_HUNDREDTHS_POINT)
    {
        float value = 0.0;
        wxSscanf(str.c_str(), wxT("%f"), &value);
        ret = (int) ((value * 100.0) /* + 0.5 */);
    }
    else if (unit == wxTEXT_ATTR_UNITS_POINTS)
    {
        ret = wxAtoi(str);
        return true;
    }
    else
    {
        ret = 0;
        return false;
    }

    return true;
}

/*
 * A control for displaying a small preview of a colour or bitmap
 */

BEGIN_EVENT_TABLE(wxRichTextColourSwatchCtrl, wxControl)
    EVT_MOUSE_EVENTS(wxRichTextColourSwatchCtrl::OnMouseEvent)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxRichTextColourSwatchCtrl, wxControl)

wxRichTextColourSwatchCtrl::wxRichTextColourSwatchCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if ((style & wxBORDER_MASK) == wxBORDER_DEFAULT)
        style |= wxBORDER_THEME;

    wxControl::Create(parent, id, pos, size, style);

    SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
}

wxRichTextColourSwatchCtrl::~wxRichTextColourSwatchCtrl()
{
}

void wxRichTextColourSwatchCtrl::OnMouseEvent(wxMouseEvent& event)
{
    if (event.LeftDown())
    {
        wxWindow* parent = GetParent();
        while (parent != NULL && !wxDynamicCast(parent, wxDialog) && !wxDynamicCast(parent, wxFrame))
            parent = parent->GetParent();

        wxColourData data;
        data.SetChooseFull(true);
        data.SetColour(m_colour);
#if wxUSE_COLOURDLG
        wxColourDialog *dialog = new wxColourDialog(parent, &data);
        // Crashes on wxMac (no m_peer)
#ifndef __WXMAC__
        dialog->SetTitle(_("Colour"));
#endif
        if (dialog->ShowModal() == wxID_OK)
        {
            wxColourData retData = dialog->GetColourData();
            m_colour = retData.GetColour();
            SetBackgroundColour(m_colour);
        }
        dialog->Destroy();
#endif // wxUSE_COLOURDLG
        Refresh();

        wxCommandEvent event(wxEVT_BUTTON, GetId());
        GetEventHandler()->ProcessEvent(event);
    }
}

#if wxUSE_HTML

/*!
 * wxRichTextFontListBox class declaration
 * A listbox to display styles.
 */

IMPLEMENT_CLASS(wxRichTextFontListBox, wxHtmlListBox)

BEGIN_EVENT_TABLE(wxRichTextFontListBox, wxHtmlListBox)
END_EVENT_TABLE()

wxRichTextFontListBox::wxRichTextFontListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}

bool wxRichTextFontListBox::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
{
    if ((style & wxBORDER_MASK) == wxBORDER_DEFAULT)
        style |= wxBORDER_THEME;

    return wxHtmlListBox::Create(parent, id, pos, size, style);
}

wxRichTextFontListBox::~wxRichTextFontListBox()
{
}

/// Returns the HTML for this item
wxString wxRichTextFontListBox::OnGetItem(size_t n) const
{
    if (m_faceNames.GetCount() == 0)
        return wxEmptyString;

    wxString str = CreateHTML(m_faceNames[n]);
    return str;
}

/// Get font name for index
wxString wxRichTextFontListBox::GetFaceName(size_t i) const
{
    return m_faceNames[i];
}

/// Set selection for string, returning the index.
int wxRichTextFontListBox::SetFaceNameSelection(const wxString& name)
{
    int i = m_faceNames.Index(name);
    SetSelection(i);

    return i;
}

/// Updates the font list
void wxRichTextFontListBox::UpdateFonts()
{
    wxArrayString facenames = wxRichTextCtrl::GetAvailableFontNames();
    m_faceNames = facenames;
    m_faceNames.Sort();

    SetItemCount(m_faceNames.GetCount());
    Refresh();
}

#if 0
// Convert a colour to a 6-digit hex string
static wxString ColourToHexString(const wxColour& col)
{
    wxString hex;

    hex += wxDecToHex(col.Red());
    hex += wxDecToHex(col.Green());
    hex += wxDecToHex(col.Blue());

    return hex;
}
#endif

/// Creates a suitable HTML fragment for a definition
wxString wxRichTextFontListBox::CreateHTML(const wxString& facename) const
{
    wxString str = wxT("<font");

    str << wxT(" size=\"+2\"");;

    if (!facename.IsEmpty() && facename != _("(none)"))
        str << wxT(" face=\"") << facename << wxT("\"");
/*
    if (def->GetStyle().GetTextColour().IsOk())
        str << wxT(" color=\"#") << ColourToHexString(def->GetStyle().GetTextColour()) << wxT("\"");
*/

    str << wxT(">");

    bool hasBold = false;

    if (hasBold)
        str << wxT("<b>");

    str += facename;

    if (hasBold)
        str << wxT("</b>");

    str << wxT("</font>");

    return str;
}

#endif
    // wxUSE_HTML


#endif
    // wxUSE_RICHTEXT
