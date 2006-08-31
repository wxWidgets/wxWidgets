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
    #include "wx/dcclient.h"
    #include "wx/module.h"
#endif

#include "wx/filename.h"
#include "wx/clipbrd.h"
#include "wx/wfstream.h"

#include "wx/richtext/richtextctrl.h"

IMPLEMENT_CLASS(wxRichTextStyleDefinition, wxObject)
IMPLEMENT_CLASS(wxRichTextCharacterStyleDefinition, wxRichTextStyleDefinition)
IMPLEMENT_CLASS(wxRichTextParagraphStyleDefinition, wxRichTextStyleDefinition)

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

#if wxUSE_HTML
/*!
 * wxRichTextStyleListBox class declaration
 * A listbox to display styles.
 */

IMPLEMENT_CLASS(wxRichTextStyleListBox, wxHtmlListBox)

BEGIN_EVENT_TABLE(wxRichTextStyleListBox, wxHtmlListBox)
    EVT_LISTBOX(wxID_ANY, wxRichTextStyleListBox::OnSelect)
    EVT_LEFT_DOWN(wxRichTextStyleListBox::OnLeftDown)
END_EVENT_TABLE()

wxRichTextStyleListBox::wxRichTextStyleListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style): wxHtmlListBox(parent, id, pos, size, style)
{
    m_styleSheet = NULL;
    m_richTextCtrl = NULL;
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

    if ( item != wxNOT_FOUND )
    {
        wxRichTextStyleDefinition* def = GetStyle(item);
        if (def && GetRichTextCtrl())
        {
            wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());

            // Flags are defined within each definition, so only certain
            // attributes are applied.
            wxRichTextAttr attr(def->GetStyle());

            if (m_richTextCtrl->HasSelection())
                m_richTextCtrl->SetStyle(m_richTextCtrl->GetSelectionRange(), attr);
            else
                m_richTextCtrl->SetDefaultStyle(attr);

            m_richTextCtrl->SetFocus();
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

#endif
    // wxUSE_HTML

#endif
    // wxUSE_RICHTEXT
