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
#include "wx/settings.h"

#include "wx/richtext/richtextctrl.h"

IMPLEMENT_CLASS(wxRichTextStyleDefinition, wxObject)
IMPLEMENT_CLASS(wxRichTextCharacterStyleDefinition, wxRichTextStyleDefinition)
IMPLEMENT_CLASS(wxRichTextParagraphStyleDefinition, wxRichTextStyleDefinition)
IMPLEMENT_CLASS(wxRichTextListStyleDefinition, wxRichTextParagraphStyleDefinition)

/*!
 * A definition
 */

void wxRichTextStyleDefinition::Copy(const wxRichTextStyleDefinition& def)
{
    m_name = def.m_name;
    m_baseStyle = def.m_baseStyle;
    m_style = def.m_style;
    m_description = def.m_description;
}

bool wxRichTextStyleDefinition::Eq(const wxRichTextStyleDefinition& def) const
{
    return (m_name == def.m_name && m_baseStyle == def.m_baseStyle && m_style == def.m_style);
}

/// Gets the style combined with the base style
wxRichTextAttr wxRichTextStyleDefinition::GetStyleMergedWithBase(const wxRichTextStyleSheet* sheet) const
{
    if (!m_baseStyle.IsEmpty())
    {
        wxRichTextStyleDefinition* baseStyle = sheet->FindStyle(m_baseStyle);
        if (baseStyle)
        {
            wxRichTextAttr baseAttr = baseStyle->GetStyleMergedWithBase(sheet);
            baseAttr.Apply(m_style, NULL);
            return baseAttr;
        }
    }
    return m_style;
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
 * List style definition
 */

void wxRichTextListStyleDefinition::Copy(const wxRichTextListStyleDefinition& def)
{
    wxRichTextParagraphStyleDefinition::Copy(def);

    int i;
    for (i = 0; i < 10; i++)
        m_levelStyles[i] = def.m_levelStyles[i];
}

bool wxRichTextListStyleDefinition::operator ==(const wxRichTextListStyleDefinition& def) const
{
    if (!Eq(def))
        return false;
    int i;
    for (i = 0; i < 10; i++)
        if (!(m_levelStyles[i] == def.m_levelStyles[i]))
            return false;

    return true;
}

/// Sets/gets the attributes for the given level
void wxRichTextListStyleDefinition::SetLevelAttributes(int i, const wxRichTextAttr& attr)
{
    wxASSERT( (i >= 0 && i < 10) );
    if (i >= 0 && i < 10)
        m_levelStyles[i] = attr;
}

const wxRichTextAttr* wxRichTextListStyleDefinition::GetLevelAttributes(int i) const
{
    wxASSERT( (i >= 0 && i < 10) );
    if (i >= 0 && i < 10)
        return & m_levelStyles[i];
    else
        return NULL;
}

wxRichTextAttr* wxRichTextListStyleDefinition::GetLevelAttributes(int i)
{
    wxASSERT( (i >= 0 && i < 10) );
    if (i >= 0 && i < 10)
        return & m_levelStyles[i];
    else
        return NULL;
}

/// Convenience function for setting the major attributes for a list level specification
void wxRichTextListStyleDefinition::SetAttributes(int i, int leftIndent, int leftSubIndent, int bulletStyle, const wxString& bulletSymbol)
{
    wxASSERT( (i >= 0 && i < 10) );
    if (i >= 0 && i < 10)
    {
        wxRichTextAttr attr;

        attr.SetBulletStyle(bulletStyle);
        attr.SetLeftIndent(leftIndent, leftSubIndent);

        if (!bulletSymbol.IsEmpty())
        {
            if (bulletStyle & wxTEXT_ATTR_BULLET_STYLE_SYMBOL)
                attr.SetBulletText(bulletSymbol);
            else
                attr.SetBulletName(bulletSymbol);
        }

        m_levelStyles[i] = attr;
    }
}

/// Finds the level corresponding to the given indentation
int wxRichTextListStyleDefinition::FindLevelForIndent(int indent) const
{
    int i;
    for (i = 0; i < 10; i++)
    {
        if (indent < m_levelStyles[i].GetLeftIndent())
        {
            if (i > 0)
                return i - 1;
            else
                return 0;
        }
    }
    return 9;
}

/// Combine the list style with a paragraph style, using the given indent (from which
/// an appropriate level is found)
wxRichTextAttr wxRichTextListStyleDefinition::CombineWithParagraphStyle(int indent, const wxRichTextAttr& paraStyle, wxRichTextStyleSheet* styleSheet)
{
    int listLevel = FindLevelForIndent(indent);

    wxRichTextAttr attr(*GetLevelAttributes(listLevel));
    int oldLeftIndent = attr.GetLeftIndent();
    int oldLeftSubIndent = attr.GetLeftSubIndent();

    // First apply the overall paragraph style, if any
    if (styleSheet)
        attr.Apply(GetStyleMergedWithBase(styleSheet));
    else
        attr.Apply(GetStyle());

    // Then apply paragraph style, e.g. from paragraph style definition
    attr.Apply(paraStyle);

    // We override the indents according to the list definition
    attr.SetLeftIndent(oldLeftIndent, oldLeftSubIndent);

    return attr;
}

/// Combine the base and list style, using the given indent (from which
/// an appropriate level is found)
wxRichTextAttr wxRichTextListStyleDefinition::GetCombinedStyle(int indent, wxRichTextStyleSheet* styleSheet)
{
    int listLevel = FindLevelForIndent(indent);
    return GetCombinedStyleForLevel(listLevel, styleSheet);
}

/// Combine the base and list style, using the given indent (from which
/// an appropriate level is found)
wxRichTextAttr wxRichTextListStyleDefinition::GetCombinedStyleForLevel(int listLevel, wxRichTextStyleSheet* styleSheet)
{
    wxRichTextAttr attr(*GetLevelAttributes(listLevel));
    int oldLeftIndent = attr.GetLeftIndent();
    int oldLeftSubIndent = attr.GetLeftSubIndent();

    // Apply the overall paragraph style, if any
    if (styleSheet)
        attr.Apply(GetStyleMergedWithBase(styleSheet));
    else
        attr.Apply(GetStyle());

    // We override the indents according to the list definition
    attr.SetLeftIndent(oldLeftIndent, oldLeftSubIndent);

    return attr;
}

/// Is this a numbered list?
bool wxRichTextListStyleDefinition::IsNumbered(int i) const
{
    return (0 != (GetLevelAttributes(i)->GetFlags() &
                   (wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER|wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER|
                    wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER|wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER)));
}

/*!
 * The style manager
 */

IMPLEMENT_CLASS(wxRichTextStyleSheet, wxObject)

wxRichTextStyleSheet::~wxRichTextStyleSheet()
{
    DeleteStyles();

    if (m_nextSheet)
        m_nextSheet->m_previousSheet = m_previousSheet;

    if (m_previousSheet)
        m_previousSheet->m_nextSheet = m_nextSheet;

    m_previousSheet = NULL;
    m_nextSheet = NULL;
}

/// Initialisation
void wxRichTextStyleSheet::Init()
{
    m_previousSheet = NULL;
    m_nextSheet = NULL;
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

/// Remove a style
bool wxRichTextStyleSheet::RemoveStyle(wxRichTextStyleDefinition* def, bool deleteStyle)
{
    if (RemoveParagraphStyle(def, deleteStyle))
        return true;
    if (RemoveCharacterStyle(def, deleteStyle))
        return true;
    if (RemoveListStyle(def, deleteStyle))
        return true;
    return false;
}

/// Find a definition by name
wxRichTextStyleDefinition* wxRichTextStyleSheet::FindStyle(const wxList& list, const wxString& name, bool recurse) const
{
    for (wxList::compatibility_iterator node = list.GetFirst(); node; node = node->GetNext())
    {
        wxRichTextStyleDefinition* def = (wxRichTextStyleDefinition*) node->GetData();
        if (def->GetName().Lower() == name.Lower())
            return def;
    }

    if (m_nextSheet && recurse)
        return m_nextSheet->FindStyle(list, name, recurse);

    return NULL;
}

/// Delete all styles
void wxRichTextStyleSheet::DeleteStyles()
{
    WX_CLEAR_LIST(wxList, m_characterStyleDefinitions);
    WX_CLEAR_LIST(wxList, m_paragraphStyleDefinitions);
    WX_CLEAR_LIST(wxList, m_listStyleDefinitions);
}

/// Insert into list of style sheets
bool wxRichTextStyleSheet::InsertSheet(wxRichTextStyleSheet* before)
{
    m_previousSheet = before->m_previousSheet;
    m_nextSheet = before;

    before->m_previousSheet = this;
    return true;
}

/// Append to list of style sheets
bool wxRichTextStyleSheet::AppendSheet(wxRichTextStyleSheet* after)
{
    wxRichTextStyleSheet* last = after;
    while (last && last->m_nextSheet)
    {
        last = last->m_nextSheet;
    }

    if (last)
    {
        m_previousSheet = last;
        last->m_nextSheet = this;

        return true;
    }
    else
        return false;
}

/// Unlink from the list of style sheets
void wxRichTextStyleSheet::Unlink()
{
    if (m_previousSheet)
        m_previousSheet->m_nextSheet = m_nextSheet;
    if (m_nextSheet)
        m_nextSheet->m_previousSheet = m_previousSheet;

    m_previousSheet = NULL;
    m_nextSheet = NULL;
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

/// Add a definition to the list style list
bool wxRichTextStyleSheet::AddListStyle(wxRichTextListStyleDefinition* def)
{
    def->GetStyle().SetListStyleName(def->GetName());
    return AddStyle(m_listStyleDefinitions, def);
}

/// Add a definition to the appropriate style list
bool wxRichTextStyleSheet::AddStyle(wxRichTextStyleDefinition* def)
{
    wxRichTextListStyleDefinition* listDef = wxDynamicCast(def, wxRichTextListStyleDefinition);
    if (listDef)
        return AddListStyle(listDef);

    wxRichTextParagraphStyleDefinition* paraDef = wxDynamicCast(def, wxRichTextParagraphStyleDefinition);
    if (paraDef)
        return AddParagraphStyle(paraDef);

    wxRichTextCharacterStyleDefinition* charDef = wxDynamicCast(def, wxRichTextCharacterStyleDefinition);
    if (charDef)
        return AddCharacterStyle(charDef);
    
    return false;
}

/// Find any definition by name
wxRichTextStyleDefinition* wxRichTextStyleSheet::FindStyle(const wxString& name, bool recurse) const
{
    wxRichTextListStyleDefinition* listDef = FindListStyle(name, recurse);
    if (listDef)
        return listDef;

    wxRichTextParagraphStyleDefinition* paraDef = FindParagraphStyle(name, recurse);
    if (paraDef)
        return paraDef;

    wxRichTextCharacterStyleDefinition* charDef = FindCharacterStyle(name, recurse);
    if (charDef)
        return charDef;
    
    return NULL;    
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

    for (node = sheet.m_listStyleDefinitions.GetFirst(); node; node = node->GetNext())
    {
        wxRichTextListStyleDefinition* def = (wxRichTextListStyleDefinition*) node->GetData();
        AddListStyle(new wxRichTextListStyleDefinition(*def));
    }
    
    SetName(sheet.GetName());
    SetDescription(sheet.GetDescription());
}

/// Equality
bool wxRichTextStyleSheet::operator==(const wxRichTextStyleSheet& WXUNUSED(sheet)) const
{
    // TODO
    return false;
}


#if wxUSE_HTML
/*!
 * wxRichTextStyleListBox: a listbox to display styles.
 */

IMPLEMENT_CLASS(wxRichTextStyleListBox, wxHtmlListBox)

BEGIN_EVENT_TABLE(wxRichTextStyleListBox, wxHtmlListBox)
    EVT_LEFT_DOWN(wxRichTextStyleListBox::OnLeftDown)
    EVT_LEFT_DCLICK(wxRichTextStyleListBox::OnLeftDoubleClick)
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

    wxRichTextStyleDefinition* def = GetStyle(n);
    if (def)
        return CreateHTML(def);

    return wxEmptyString;
}

// Get style for index
wxRichTextStyleDefinition* wxRichTextStyleListBox::GetStyle(size_t i) const
{
    if (!GetStyleSheet())
        return NULL;

    if (GetStyleType() == wxRICHTEXT_STYLE_ALL)
    {
        // First paragraph styles, then character, then list
        if (i < GetStyleSheet()->GetParagraphStyleCount())
            return GetStyleSheet()->GetParagraphStyle(i);

        if ((i - GetStyleSheet()->GetParagraphStyleCount()) < GetStyleSheet()->GetCharacterStyleCount())
            return GetStyleSheet()->GetCharacterStyle(i - GetStyleSheet()->GetParagraphStyleCount());

        if ((i - GetStyleSheet()->GetParagraphStyleCount() - GetStyleSheet()->GetCharacterStyleCount()) < GetStyleSheet()->GetListStyleCount())
            return GetStyleSheet()->GetListStyle(i - GetStyleSheet()->GetParagraphStyleCount() - GetStyleSheet()->GetCharacterStyleCount());
    }
    else if ((GetStyleType() == wxRICHTEXT_STYLE_PARAGRAPH) && (i < GetStyleSheet()->GetParagraphStyleCount()))
    {
        return GetStyleSheet()->GetParagraphStyle(i);
    }
    else if ((GetStyleType() == wxRICHTEXT_STYLE_CHARACTER) && (i < GetStyleSheet()->GetCharacterStyleCount()))
    {
        return GetStyleSheet()->GetCharacterStyle(i);
    }
    else if ((GetStyleType() == wxRICHTEXT_STYLE_LIST) && (i < GetStyleSheet()->GetListStyleCount()))
    {
        return GetStyleSheet()->GetListStyle(i);
    }

    return NULL;
}

/// Updates the list
void wxRichTextStyleListBox::UpdateStyles()
{
    if (GetStyleSheet())
    {
        SetSelection(wxNOT_FOUND);

        if (GetStyleType() == wxRICHTEXT_STYLE_ALL)
            SetItemCount(GetStyleSheet()->GetParagraphStyleCount()+GetStyleSheet()->GetCharacterStyleCount()+GetStyleSheet()->GetListStyleCount());
        else if (GetStyleType() == wxRICHTEXT_STYLE_PARAGRAPH)
            SetItemCount(GetStyleSheet()->GetParagraphStyleCount());
        else if (GetStyleType() == wxRICHTEXT_STYLE_CHARACTER)
            SetItemCount(GetStyleSheet()->GetCharacterStyleCount());
        else if (GetStyleType() == wxRICHTEXT_STYLE_LIST)
            SetItemCount(GetStyleSheet()->GetListStyleCount());

        Refresh();

        if (GetItemCount() > 0)
        {
            SetSelection(0);
            SendSelectedEvent();
        }
    }
}

// Get index for style name
int wxRichTextStyleListBox::GetIndexForStyle(const wxString& name) const
{
    if (GetStyleSheet())
    {
        int count = GetItemCount();

        int i;
        for (i = 0; i < (int) count; i++)
        {
            wxRichTextStyleDefinition* def = GetStyle(i);
            if (def->GetName() == name)
                return i;
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
    // TODO: indicate list format for list style types

    wxString str;

    bool isCentred = false;
    
    wxRichTextAttr attr(def->GetStyleMergedWithBase(GetStyleSheet()));

    if (attr.HasAlignment() && attr.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
        isCentred = true;

    if (isCentred)
        str << wxT("<center>");

    
    str << wxT("<table><tr>");

    if (attr.GetLeftIndent() > 0)
    {
        wxClientDC dc((wxWindow*) this);

        str << wxT("<td width=") << wxMin(50, (ConvertTenthsMMToPixels(dc, attr.GetLeftIndent())/2)) << wxT("></td>");
    }

    if (isCentred)
        str << wxT("<td nowrap align=\"center\">");
    else
        str << wxT("<td nowrap>");

#ifdef __WXMSW__
    int size = 3;
#else
    int size = 4;
#endif

    int stdFontSize = 12;
    int thisFontSize = ((attr.GetFlags() & wxTEXT_ATTR_FONT_SIZE) != 0) ? attr.GetFontSize() : stdFontSize;

    if (thisFontSize < stdFontSize)
        size ++;
    else if (thisFontSize > stdFontSize)
        size --;

    str += wxT("<font");

    str << wxT(" size=") << size;

    if (!attr.GetFontFaceName().IsEmpty())
        str << wxT(" face=\"") << attr.GetFontFaceName() << wxT("\"");

    if (attr.GetTextColour().Ok())
        str << wxT(" color=\"#") << ColourToHexString(attr.GetTextColour()) << wxT("\"");

    str << wxT(">");

    bool hasBold = false;
    bool hasItalic = false;
    bool hasUnderline = false;

    if (attr.GetFontWeight() == wxBOLD)
        hasBold = true;
    if (attr.GetFontStyle() == wxITALIC)
        hasItalic = true;
    if (attr.GetFontUnderlined())
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

    if (isCentred)
        str << wxT("</centre>");

    str << wxT("</font>");

    str << wxT("</td></tr></table>");

    if (isCentred)
        str << wxT("</center>");

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

void wxRichTextStyleListBox::OnLeftDown(wxMouseEvent& event)
{
    wxVListBox::OnLeftDown(event);

    int item = HitTest(event.GetPosition());
    if (item != wxNOT_FOUND && GetApplyOnSelection())
        ApplyStyle(item);
}

void wxRichTextStyleListBox::OnLeftDoubleClick(wxMouseEvent& event)
{
    wxVListBox::OnLeftDown(event);

    int item = HitTest(event.GetPosition());
    if (item != wxNOT_FOUND && !GetApplyOnSelection())
        ApplyStyle(item);
}

/// Helper for listbox and combo control
wxString wxRichTextStyleListBox::GetStyleToShowInIdleTime(wxRichTextCtrl* ctrl, wxRichTextStyleType styleType)
{
    int adjustedCaretPos = ctrl->GetAdjustedCaretPosition(ctrl->GetCaretPosition());

    wxRichTextParagraph* para = ctrl->GetBuffer().GetParagraphAtPosition(adjustedCaretPos);
    wxRichTextObject* obj = ctrl->GetBuffer().GetLeafObjectAtPosition(adjustedCaretPos);

    wxString styleName;

    // Take into account current default style just chosen by user
    if (ctrl->IsDefaultStyleShowing())
    {
        if ((styleType == wxRICHTEXT_STYLE_ALL || styleType == wxRICHTEXT_STYLE_CHARACTER) &&
                          !ctrl->GetDefaultStyleEx().GetCharacterStyleName().IsEmpty())
            styleName = ctrl->GetDefaultStyleEx().GetCharacterStyleName();
        else if ((styleType == wxRICHTEXT_STYLE_ALL || styleType == wxRICHTEXT_STYLE_PARAGRAPH) &&
                          !ctrl->GetDefaultStyleEx().GetParagraphStyleName().IsEmpty())
            styleName = ctrl->GetDefaultStyleEx().GetParagraphStyleName();
        else if ((styleType == wxRICHTEXT_STYLE_ALL || styleType == wxRICHTEXT_STYLE_LIST) &&
                          !ctrl->GetDefaultStyleEx().GetListStyleName().IsEmpty())
            styleName = ctrl->GetDefaultStyleEx().GetListStyleName();
    }
    else if (obj && (styleType == wxRICHTEXT_STYLE_ALL || styleType == wxRICHTEXT_STYLE_CHARACTER) &&
             !obj->GetAttributes().GetCharacterStyleName().IsEmpty())
    {
        styleName = obj->GetAttributes().GetCharacterStyleName();
    }
    else if (para && (styleType == wxRICHTEXT_STYLE_ALL || styleType == wxRICHTEXT_STYLE_PARAGRAPH) &&
             !para->GetAttributes().GetParagraphStyleName().IsEmpty())
    {
        styleName = para->GetAttributes().GetParagraphStyleName();
    }
    else if (para && (styleType == wxRICHTEXT_STYLE_ALL || styleType == wxRICHTEXT_STYLE_LIST) &&
             !para->GetAttributes().GetListStyleName().IsEmpty())
    {
        styleName = para->GetAttributes().GetListStyleName();
    }

    return styleName;
}

/// Auto-select from style under caret in idle time
void wxRichTextStyleListBox::OnIdle(wxIdleEvent& event)
{
    if (CanAutoSetSelection() && GetRichTextCtrl() && wxWindow::FindFocus() != this)
    {
        wxString styleName = GetStyleToShowInIdleTime(GetRichTextCtrl(), GetStyleType());

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

/*!
 * wxRichTextStyleListCtrl class: manages a listbox and a choice control to
 * switch shown style types
 */

IMPLEMENT_CLASS(wxRichTextStyleListCtrl, wxControl)

BEGIN_EVENT_TABLE(wxRichTextStyleListCtrl, wxControl)
    EVT_CHOICE(wxID_ANY, wxRichTextStyleListCtrl::OnChooseType)
    EVT_SIZE(wxRichTextStyleListCtrl::OnSize)
END_EVENT_TABLE()

wxRichTextStyleListCtrl::wxRichTextStyleListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos,
    const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}

bool wxRichTextStyleListCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
{
    wxControl::Create(parent, id, pos, size, style);

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    if (size != wxDefaultSize)
        SetInitialSize(size);

    bool showSelector = ((style & wxRICHTEXTSTYLELIST_HIDE_TYPE_SELECTOR) == 0);

    m_styleListBox = new wxRichTextStyleListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, showSelector ? wxSIMPLE_BORDER : wxNO_BORDER);

    wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);

    if (showSelector)
    {
        wxArrayString choices;
        choices.Add(_("All styles"));
        choices.Add(_("Paragraph styles"));
        choices.Add(_("Character styles"));
        choices.Add(_("List styles"));

        m_styleChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);

        boxSizer->Add(m_styleListBox, 1, wxALL|wxEXPAND, 5);
        boxSizer->Add(m_styleChoice, 0, wxALL|wxEXPAND, 5);
    }
    else
    {
        boxSizer->Add(m_styleListBox, 1, wxALL|wxEXPAND, 0);
    }

    SetSizer(boxSizer);
    Layout();

    m_dontUpdate = true;

    if (m_styleChoice)
    {
        int i = StyleTypeToIndex(m_styleListBox->GetStyleType());
        m_styleChoice->SetSelection(i);
    }

    m_dontUpdate = false;

    return true;
}

wxRichTextStyleListCtrl::~wxRichTextStyleListCtrl()
{

}

/// React to style type choice
void wxRichTextStyleListCtrl::OnChooseType(wxCommandEvent& event)
{
    if (event.GetEventObject() != m_styleChoice)
        event.Skip();
    else
    {
        if (m_dontUpdate)
            return;

        wxRichTextStyleListBox::wxRichTextStyleType styleType = StyleIndexToType(event.GetSelection());
        m_styleListBox->SetStyleType(styleType);
    }
}

/// Lay out the controls
void wxRichTextStyleListCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    if (GetAutoLayout())
        Layout();
}

/// Get the choice index for style type
int wxRichTextStyleListCtrl::StyleTypeToIndex(wxRichTextStyleListBox::wxRichTextStyleType styleType)
{
    if (styleType == wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL)
    {
        return 0;
    }
    else if (styleType == wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH)
    {
        return 1;
    }
    else if (styleType == wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER)
    {
        return 2;
    }
    else if (styleType == wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST)
    {
        return 3;
    }
    return 0;
}

/// Get the style type for choice index
wxRichTextStyleListBox::wxRichTextStyleType wxRichTextStyleListCtrl::StyleIndexToType(int i)
{
    if (i == 1)
        return wxRichTextStyleListBox::wxRICHTEXT_STYLE_PARAGRAPH;
    else if (i == 2)
        return wxRichTextStyleListBox::wxRICHTEXT_STYLE_CHARACTER;
    else if (i == 3)
        return wxRichTextStyleListBox::wxRICHTEXT_STYLE_LIST;

    return wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL;
}

/// Associates the control with a style manager
void wxRichTextStyleListCtrl::SetStyleSheet(wxRichTextStyleSheet* styleSheet)
{
    if (m_styleListBox)
        m_styleListBox->SetStyleSheet(styleSheet);
}

wxRichTextStyleSheet* wxRichTextStyleListCtrl::GetStyleSheet() const
{
    if (m_styleListBox)
        return m_styleListBox->GetStyleSheet();
    else
        return NULL;
}

/// Associates the control with a wxRichTextCtrl
void wxRichTextStyleListCtrl::SetRichTextCtrl(wxRichTextCtrl* ctrl)
{
    if (m_styleListBox)
        m_styleListBox->SetRichTextCtrl(ctrl);
}

wxRichTextCtrl* wxRichTextStyleListCtrl::GetRichTextCtrl() const
{
    if (m_styleListBox)
        return m_styleListBox->GetRichTextCtrl();
    else
        return NULL;
}

/// Set/get the style type to display
void wxRichTextStyleListCtrl::SetStyleType(wxRichTextStyleListBox::wxRichTextStyleType styleType)
{
    if (m_styleListBox)
        m_styleListBox->SetStyleType(styleType);

    m_dontUpdate = true;

    if (m_styleChoice)
    {
        int i = StyleTypeToIndex(m_styleListBox->GetStyleType());
        m_styleChoice->SetSelection(i);
    }

    m_dontUpdate = false;
}

wxRichTextStyleListBox::wxRichTextStyleType wxRichTextStyleListCtrl::GetStyleType() const
{
    if (m_styleListBox)
        return m_styleListBox->GetStyleType();
    else
        return wxRichTextStyleListBox::wxRICHTEXT_STYLE_ALL;
}

/// Updates the style list box
void wxRichTextStyleListCtrl::UpdateStyles()
{
    if (m_styleListBox)
        m_styleListBox->UpdateStyles();
}

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
    if (GetRichTextCtrl() && !IsPopupShown() && m_stylePopup && wxWindow::FindFocus() != this)
    {
        wxString styleName = wxRichTextStyleListBox::GetStyleToShowInIdleTime(GetRichTextCtrl(), m_stylePopup->GetStyleType());

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
