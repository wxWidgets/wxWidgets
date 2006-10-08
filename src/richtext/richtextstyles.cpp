/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextstyles.cpp
// Purpose:     Style management for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextstyles.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/clipbrd.h"
#include "wx/wfstream.h"

#include "wx/richtext/richtextctrl.h"

IMPLEMENT_CLASS(wxRichTextStyleDefinition, wxObject)
IMPLEMENT_CLASS(wxRichTextCharacterStyleDefinition, wxRichTextStyleDefinition)
IMPLEMENT_CLASS(wxRichTextParagraphStyleDefinition, wxRichTextStyleDefinition)

/*!
 * A definition
 */

void wxRichTextStyleDefinition::Copy(const wxRichTextStyleDefinition& def)
{
    m_name = def.m_name;
    m_baseStyle = def.m_baseStyle;
    m_style = def.m_style;
}

bool wxRichTextStyleDefinition::Eq(const wxRichTextStyleDefinition& def) const
{
    return (m_name == def.m_name && m_baseStyle == def.m_baseStyle && m_style == def.m_style);
}

/*!
 * Paragraph style definition
 */

void wxRichTextParagraphStyleDefinition::Copy(const wxRichTextParagraphStyleDefinition& def)
{
    wxRichTextStyleDefinition::Copy(def);

    m_nextStyle = def.m_nextStyle;
}

bool wxRichTextParagraphStyleDefinition::operator ==(const wxRichTextParagraphStyleDefinition& def) const
{
    return (Eq(def) && m_nextStyle == def.m_nextStyle);
}

/*!
 * The style manager
 */

IMPLEMENT_CLASS(wxRichTextStyleSheet, wxObject)

/// Initialisation
void wxRichTextStyleSheet::Init()
{
}

/// Add a definition to one of the style lists
bool wxRichTextStyleSheet::AddStyle(wxList& list, wxRichTextStyleDefinition* def)
{
    if (!list.Find(def))
        list.Append(def);
    return true;
}

/// Remove a style
bool wxRichTextStyleSheet::RemoveStyle(wxList& list, wxRichTextStyleDefinition* def, bool deleteStyle)
{
    wxList::compatibility_iterator node = list.Find(def);
    if (node)
    {
        wxRichTextStyleDefinition* def = (wxRichTextStyleDefinition*) node->GetData();
        list.Erase(node);
        if (deleteStyle)
            delete def;
        return true;
    }
    else
        return false;
}

/// Find a definition by name
wxRichTextStyleDefinition* wxRichTextStyleSheet::FindStyle(const wxList& list, const wxString& name) const
{
    for (wxList::compatibility_iterator node = list.GetFirst(); node; node = node->GetNext())
    {
        wxRichTextStyleDefinition* def = (wxRichTextStyleDefinition*) node->GetData();
        if (def->GetName().Lower() == name.Lower())
            return def;
    }
    return NULL;
}

/// Delete all styles
void wxRichTextStyleSheet::DeleteStyles()
{
    WX_CLEAR_LIST(wxList, m_characterStyleDefinitions);
    WX_CLEAR_LIST(wxList, m_paragraphStyleDefinitions);
}

/// Add a definition to the character style list
bool wxRichTextStyleSheet::AddCharacterStyle(wxRichTextCharacterStyleDefinition* def)
{
    def->GetStyle().SetCharacterStyleName(def->GetName());
    return AddStyle(m_characterStyleDefinitions, def);
}

/// Add a definition to the paragraph style list
bool wxRichTextStyleSheet::AddParagraphStyle(wxRichTextParagraphStyleDefinition* def)
{
    def->GetStyle().SetParagraphStyleName(def->GetName());
    return AddStyle(m_paragraphStyleDefinitions, def);
}

/// Copy
void wxRichTextStyleSheet::Copy(const wxRichTextStyleSheet& sheet)
{
    DeleteStyles();

    wxList::compatibility_iterator node;

    for (node = sheet.m_characterStyleDefinitions.GetFirst(); node; node = node->GetNext())
    {
        wxRichTextCharacterStyleDefinition* def = (wxRichTextCharacterStyleDefinition*) node->GetData();
        AddCharacterStyle(new wxRichTextCharacterStyleDefinition(*def));
    }

    for (node = sheet.m_paragraphStyleDefinitions.GetFirst(); node; node = node->GetNext())
    {
        wxRichTextParagraphStyleDefinition* def = (wxRichTextParagraphStyleDefinition*) node->GetData();
        AddParagraphStyle(new wxRichTextParagraphStyleDefinition(*def));
    }
}

/// Equality
bool wxRichTextStyleSheet::operator==(const wxRichTextStyleSheet& WXUNUSED(sheet)) const
{
    // TODO
    return false;
}


#if wxUSE_HTML
/*!
 * wxRichTextStyleListBox class declaration
 * A listbox to display styles.
 */

IMPLEMENT_CLASS(wxRichTextStyleListBox, wxHtmlListBox)

BEGIN_EVENT_TABLE(wxRichTextStyleListBox, wxHtmlListBox)
    EVT_LISTBOX(wxID_ANY, wxRichTextStyleListBox::OnSelect)
    EVT_LEFT_DOWN(wxRichTextStyleListBox::OnLeftDown)
    EVT_IDLE(wxRichTextStyleListBox::OnIdle)
END_EVENT_TABLE()

wxRichTextStyleListBox::wxRichTextStyleListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}

bool wxRichTextStyleListBox::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
{
    return wxHtmlListBox::Create(parent, id, pos, size, style);
}

wxRichTextStyleListBox::~wxRichTextStyleListBox()
{
}

/// Returns the HTML for this item
wxString wxRichTextStyleListBox::OnGetItem(size_t n) const
{
    if (!GetStyleSheet())
        return wxEmptyString;

    // First paragraph styles, then character
    if (n < GetStyleSheet()->GetParagraphStyleCount())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->GetParagraphStyle(n);

        wxString str = CreateHTML(def);
        return str;
    }

    if ((n - GetStyleSheet()->GetParagraphStyleCount()) < GetStyleSheet()->GetCharacterStyleCount())
    {
        wxRichTextCharacterStyleDefinition* def = GetStyleSheet()->GetCharacterStyle(n - GetStyleSheet()->GetParagraphStyleCount());

        wxString str = CreateHTML(def);
        return str;
    }
    return wxEmptyString;
}

// Get style for index
wxRichTextStyleDefinition* wxRichTextStyleListBox::GetStyle(size_t i) const
{
    if (!GetStyleSheet())
        return NULL;

    // First paragraph styles, then character
    if (i < GetStyleSheet()->GetParagraphStyleCount())
        return GetStyleSheet()->GetParagraphStyle(i);

    if ((i - GetStyleSheet()->GetParagraphStyleCount()) < GetStyleSheet()->GetCharacterStyleCount())
        return GetStyleSheet()->GetCharacterStyle(i - GetStyleSheet()->GetParagraphStyleCount());

    return NULL;
}

/// Updates the list
void wxRichTextStyleListBox::UpdateStyles()
{
    if (GetStyleSheet())
    {
        SetItemCount(GetStyleSheet()->GetParagraphStyleCount()+GetStyleSheet()->GetCharacterStyleCount());
        Refresh();
    }
}

// Get index for style name
int wxRichTextStyleListBox::GetIndexForStyle(const wxString& name) const
{
    if (GetStyleSheet())
    {
        int i;
        for (i = 0; i < (int) GetStyleSheet()->GetParagraphStyleCount(); i++)
        {
            wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->GetParagraphStyle(i);
            if (def->GetName() == name)
                return i;
        }
        for (i = 0; i < (int) GetStyleSheet()->GetCharacterStyleCount(); i++)
        {
            wxRichTextCharacterStyleDefinition* def = GetStyleSheet()->GetCharacterStyle(i);
            if (def->GetName() == name)
                return i + (int) GetStyleSheet()->GetParagraphStyleCount();
        }
    }
    return -1;
}

/// Set selection for string
int wxRichTextStyleListBox::SetStyleSelection(const wxString& name)
{
    int i = GetIndexForStyle(name);
    if (i > -1)
        SetSelection(i);
    return i;
}

// Convert a colour to a 6-digit hex string
static wxString ColourToHexString(const wxColour& col)
{
    wxString hex;

    hex += wxDecToHex(col.Red());
    hex += wxDecToHex(col.Green());
    hex += wxDecToHex(col.Blue());

    return hex;
}

/// Creates a suitable HTML fragment for a definition
wxString wxRichTextStyleListBox::CreateHTML(wxRichTextStyleDefinition* def) const
{
    wxString str(wxT("<table><tr>"));

    if (def->GetStyle().GetLeftIndent() > 0)
    {
        wxClientDC dc((wxWindow*) this);

        str << wxT("<td width=") << ConvertTenthsMMToPixels(dc, def->GetStyle().GetLeftIndent()) << wxT("></td>");
    }

    str << wxT("<td nowrap>");

    int size = 5;

    // Standard size is 12, say
    size += 12 - def->GetStyle().GetFontSize();

    str += wxT("<font");

    str << wxT(" size=") << size;

    if (!def->GetStyle().GetFontFaceName().IsEmpty())
        str << wxT(" face=\"") << def->GetStyle().GetFontFaceName() << wxT("\"");

    if (def->GetStyle().GetTextColour().Ok())
        str << wxT(" color=\"#") << ColourToHexString(def->GetStyle().GetTextColour()) << wxT("\"");

    str << wxT(">");

    bool hasBold = false;
    bool hasItalic = false;
    bool hasUnderline = false;

    if (def->GetStyle().GetFontWeight() == wxBOLD)
        hasBold = true;
    if (def->GetStyle().GetFontStyle() == wxITALIC)
        hasItalic = true;
    if (def->GetStyle().GetFontUnderlined())
        hasUnderline = true;

    if (hasBold)
        str << wxT("<b>");
    if (hasItalic)
        str << wxT("<i>");
    if (hasUnderline)
        str << wxT("<u>");

    str += def->GetName();

    if (hasUnderline)
        str << wxT("</u>");
    if (hasItalic)
        str << wxT("</i>");
    if (hasBold)
        str << wxT("</b>");

    str << wxT("</font>");

    str += wxT("</td></tr></table>");
    return str;
}

// Convert units in tends of a millimetre to device units
int wxRichTextStyleListBox::ConvertTenthsMMToPixels(wxDC& dc, int units) const
{
    int ppi = dc.GetPPI().x;

    // There are ppi pixels in 254.1 "1/10 mm"

    double pixels = ((double) units * (double)ppi) / 254.1;

    return (int) pixels;
}

/// React to selection
void wxRichTextStyleListBox::OnSelect(wxCommandEvent& WXUNUSED(event))
{
#if 0
    wxRichTextStyleDefinition* def = GetStyle(event.GetSelection());
    if (def)
    {
        wxMessageBox(def->GetName());
    }
#endif
}

void wxRichTextStyleListBox::OnLeftDown(wxMouseEvent& event)
{
    wxVListBox::OnLeftDown(event);

    int item = HitTest(event.GetPosition());
    if (item != wxNOT_FOUND && GetApplyOnSelection())
        ApplyStyle(item);
}

/// Auto-select from style under caret in idle time
void wxRichTextStyleListBox::OnIdle(wxIdleEvent& event)
{
    if (CanAutoSetSelection() && GetRichTextCtrl())
    {
        int adjustedCaretPos = GetRichTextCtrl()->GetAdjustedCaretPosition(GetRichTextCtrl()->GetCaretPosition());

        wxRichTextParagraph* para = GetRichTextCtrl()->GetBuffer().GetParagraphAtPosition(adjustedCaretPos);
        wxRichTextObject* obj = GetRichTextCtrl()->GetBuffer().GetLeafObjectAtPosition(adjustedCaretPos);

        wxString styleName;

        // Take into account current default style just chosen by user
        if (GetRichTextCtrl()->IsDefaultStyleShowing())
        {
            if (!GetRichTextCtrl()->GetDefaultStyleEx().GetCharacterStyleName().IsEmpty())
                styleName = GetRichTextCtrl()->GetDefaultStyleEx().GetCharacterStyleName();
            else if (!GetRichTextCtrl()->GetDefaultStyleEx().GetParagraphStyleName().IsEmpty())
                styleName = GetRichTextCtrl()->GetDefaultStyleEx().GetParagraphStyleName();
        }
        else if (obj && !obj->GetAttributes().GetCharacterStyleName().IsEmpty())
        {
            styleName = obj->GetAttributes().GetCharacterStyleName();
        }
        else if (para && !para->GetAttributes().GetParagraphStyleName().IsEmpty())
        {
            styleName = para->GetAttributes().GetParagraphStyleName();
        }

        int sel = GetSelection();
        if (!styleName.IsEmpty())
        {
            // Don't do the selection if it's already set
            if (sel == GetIndexForStyle(styleName))
                return;

            SetStyleSelection(styleName);
        }
        else if (sel != -1)
            SetSelection(-1);
    }
    event.Skip();
}

/// Do selection
void wxRichTextStyleListBox::ApplyStyle(int item)
{
    if ( item != wxNOT_FOUND )
    {
        wxRichTextStyleDefinition* def = GetStyle(item);
        if (def && GetRichTextCtrl())
        {
            GetRichTextCtrl()->ApplyStyle(def);
            GetRichTextCtrl()->SetFocus();
        }
    }
}

#if 0
wxColour wxRichTextStyleListBox::GetSelectedTextColour(const wxColour& colFg) const
{
    return *wxBLACK;
}

wxColour wxRichTextStyleListBox::GetSelectedTextBgColour(const wxColour& colBg) const
{
    return *wxWHITE;
}
#endif

#if wxUSE_COMBOCTRL

/*!
 * Style drop-down for a wxComboCtrl
 */


BEGIN_EVENT_TABLE(wxRichTextStyleComboPopup, wxRichTextStyleListBox)
    EVT_MOTION(wxRichTextStyleComboPopup::OnMouseMove)
    EVT_LEFT_DOWN(wxRichTextStyleComboPopup::OnMouseClick)
END_EVENT_TABLE()

void wxRichTextStyleComboPopup::SetStringValue( const wxString& s )
{
    m_value = SetStyleSelection(s);
}

wxString wxRichTextStyleComboPopup::GetStringValue() const
{
    int sel = m_value;
    if (sel > -1)
    {
        wxRichTextStyleDefinition* def = GetStyle(sel);
        if (def)
            return def->GetName();
    }
    return wxEmptyString;
}

//
// Popup event handlers
//

// Mouse hot-tracking
void wxRichTextStyleComboPopup::OnMouseMove(wxMouseEvent& event)
{
    // Move selection to cursor if it is inside the popup

    int itemHere = wxRichTextStyleListBox::HitTest(event.GetPosition());
    if ( itemHere >= 0 )
    {
        wxRichTextStyleListBox::SetSelection(itemHere);
        m_itemHere = itemHere;
    }
    event.Skip();
}

// On mouse left, set the value and close the popup
void wxRichTextStyleComboPopup::OnMouseClick(wxMouseEvent& WXUNUSED(event))
{
    if (m_itemHere >= 0)
        m_value = m_itemHere;

    // Ordering is important, so we don't dismiss this popup accidentally
    // by setting the focus elsewhere e.g. in ApplyStyle
    Dismiss();

    if (m_itemHere >= 0)
        wxRichTextStyleListBox::ApplyStyle(m_itemHere);
}

/*!
 * wxRichTextStyleComboCtrl
 * A combo for applying styles.
 */

IMPLEMENT_CLASS(wxRichTextStyleComboCtrl, wxComboCtrl)

BEGIN_EVENT_TABLE(wxRichTextStyleComboCtrl, wxComboCtrl)
    EVT_IDLE(wxRichTextStyleComboCtrl::OnIdle)
END_EVENT_TABLE()

bool wxRichTextStyleComboCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
{
    if (!wxComboCtrl::Create(parent, id, wxEmptyString, pos, size, style))
        return false;

    SetPopupMaxHeight(400);

    m_stylePopup = new wxRichTextStyleComboPopup;

    SetPopupControl(m_stylePopup);

    return true;
}

/// Auto-select from style under caret in idle time

// TODO: must be able to show italic, bold, combinations
// in style box. Do we have a concept of automatic, temporary
// styles that are added whenever we wish to show a style
// that doesn't exist already? E.g. "Bold, Italic, Underline".
// Word seems to generate these things on the fly.
// If there's a named style already, it uses e.g. Heading1 + Bold, Italic
// If you unembolden text in a style that has bold, it uses the
// term "Not bold".
// TODO: order styles alphabetically. This means indexes can change,
// so need a different way to specify selections, i.e. by name.

void wxRichTextStyleComboCtrl::OnIdle(wxIdleEvent& event)
{
    if (GetRichTextCtrl() && !IsPopupShown())
    {
        int adjustedCaretPos = GetRichTextCtrl()->GetAdjustedCaretPosition(GetRichTextCtrl()->GetCaretPosition());

        wxRichTextParagraph* para = GetRichTextCtrl()->GetBuffer().GetParagraphAtPosition(adjustedCaretPos);
        wxRichTextObject* obj = GetRichTextCtrl()->GetBuffer().GetLeafObjectAtPosition(adjustedCaretPos);

        wxString styleName;

        // Take into account current default style just chosen by user
        if (GetRichTextCtrl()->IsDefaultStyleShowing())
        {
            if (!GetRichTextCtrl()->GetDefaultStyleEx().GetCharacterStyleName().IsEmpty())
                styleName = GetRichTextCtrl()->GetDefaultStyleEx().GetCharacterStyleName();
            else if (!GetRichTextCtrl()->GetDefaultStyleEx().GetParagraphStyleName().IsEmpty())
                styleName = GetRichTextCtrl()->GetDefaultStyleEx().GetParagraphStyleName();
        }
        else if (obj && !obj->GetAttributes().GetCharacterStyleName().IsEmpty())
        {
            styleName = obj->GetAttributes().GetCharacterStyleName();
        }
        else if (para && !para->GetAttributes().GetParagraphStyleName().IsEmpty())
        {
            styleName = para->GetAttributes().GetParagraphStyleName();
        }

        wxString currentValue = GetValue();
        if (!styleName.IsEmpty())
        {
            // Don't do the selection if it's already set
            if (currentValue == styleName)
                return;

            SetValue(styleName);
        }
        else if (!currentValue.IsEmpty())
            SetValue(wxEmptyString);
    }
    event.Skip();
}

#endif
    // wxUSE_COMBOCTRL

#endif
    // wxUSE_HTML

#endif
    // wxUSE_RICHTEXT
