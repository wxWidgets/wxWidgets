/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_tablecell_g.cpp
// Purpose:     wxMoTableCell class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#include "wx/dcbuffer.h"

#include "wx/mobile/generic/tablectrl.h"
#include "wx/mobile/generic/tablecell.h"
#include "wx/mobile/generic/settings.h"

IMPLEMENT_CLASS(wxMoTableCell, wxObject)

wxMoTableCell::wxMoTableCell(wxMoTableCtrl* ctrl, const wxString& reuseName, int cellStyle)
{
    Init();

    CreateContentWindow(ctrl);
    m_reuseName = reuseName;
    m_cellStyle = cellStyle;
}

wxMoTableCell::~wxMoTableCell()
{
    bool accessoriesAreDifferent = (m_accessoryWindow != m_editingAccessoryWindow);
    if (m_contentWindow)
    {
        delete m_contentWindow;
        m_contentWindow = NULL;
    }
    if (m_accessoryWindow)
    {
        delete m_accessoryWindow;
        m_accessoryWindow = NULL;
    }
    if (m_editingAccessoryWindow && accessoriesAreDifferent)
    {
        delete m_editingAccessoryWindow;
        m_editingAccessoryWindow = NULL;
    }
    else
        m_editingAccessoryWindow = NULL;
}

void wxMoTableCell::Init()
{
    m_textAlignment = TextAlignmentLeft;
    m_detailTextAlignment = TextAlignmentLeft;
    m_lineBreakMode = LineBreakModeWordWrap;
    m_selected = false;
    m_selectionStyle = SelectionStyleBlue;
    //m_eventHandler = NULL;
    m_accessoryType = AccessoryTypeNone;
    m_accessoryWindow = NULL;
    m_editingAccessoryType = AccessoryTypeNone;
    m_editingAccessoryWindow = NULL;
    //m_editStyle = EditStyleNone;
    m_indentationLevel = 0;
    m_indentationWidth = 0;
    m_contentWindow = NULL;
    m_cellStyle = CellStyleDefault;
    m_detailWidth = 80;
    m_editingMode = false;
    m_shouldIndentWhileEditing = true;
    m_showReorderingButton = false;
}

void wxMoTableCell::Copy(const wxMoTableCell& cell)
{
    m_reuseName = cell.m_reuseName;
    m_font = cell.m_font;
    m_detailFont = cell.m_detailFont;
    m_text = cell.m_text;
    m_detailText = cell.m_detailText;
    m_textColour = cell.m_textColour;
    m_detailTextColour = cell.m_detailTextColour;
    m_selectedTextColour = cell.m_selectedTextColour;
    m_textAlignment = cell.m_textAlignment;
    m_detailTextAlignment = cell.m_detailTextAlignment;
    m_lineBreakMode = cell.m_lineBreakMode;
    m_bitmap = cell.m_bitmap;
    m_selectedBitmap = cell.m_selectedBitmap;
    m_selected = cell.m_selected;
    m_selectionStyle = cell.m_selectionStyle;
    //m_eventHandler = cell.m_eventHandler;
    m_accessoryType = cell.m_accessoryType;
    m_editingAccessoryType = cell.m_editingAccessoryType;
    //m_editStyle = cell.m_editStyle;
    m_indentationLevel = cell.m_indentationLevel;
    m_indentationWidth = cell.m_indentationWidth;
    m_contentWindow = cell.m_contentWindow;
    m_cellStyle = cell.m_cellStyle;
    m_detailWidth = cell.m_detailWidth;
    m_editingMode = cell.m_editingMode;
    m_shouldIndentWhileEditing = cell.m_shouldIndentWhileEditing;
    m_showReorderingButton = cell.m_showReorderingButton;
}

bool wxMoTableCell::CreateContentWindow(wxMoTableCtrl* ctrl)
{
    if (m_contentWindow)
        return true;

    m_contentWindow = new wxMoTableCellContentWindow(ctrl, wxID_ANY, wxPoint(-100, -100),
        wxSize(10, 10), wxBORDER_NONE);
    m_contentWindow->SetTableCtrl(ctrl);
    m_contentWindow->SetCell(this);

    return true;
}

// Sets the accessory window
void wxMoTableCell::SetAccessoryWindow(wxWindow* win)
{
    if (m_accessoryWindow != NULL && m_accessoryWindow != win)
    {
        if (m_accessoryWindow == m_editingAccessoryWindow)
            m_editingAccessoryWindow = NULL;

        delete m_accessoryWindow;
    }
    m_accessoryWindow = win;
}

// Sets the editing accessory window
void wxMoTableCell::SetEditingAccessoryWindow(wxWindow* win)
{
    if (m_editingAccessoryWindow != NULL && m_editingAccessoryWindow != win)
    {
        if (m_accessoryWindow == m_editingAccessoryWindow)
            m_accessoryWindow = NULL;

        delete m_editingAccessoryWindow;
    }
    m_editingAccessoryWindow = win;
}

// Sets editing mode (not yet implemented).
bool wxMoTableCell::SetEditingMode(bool editingMode, bool WXUNUSED(animated))
{
    m_editingMode = editingMode;

    // TODO

    return true;
}

// Is the delete confirmation button showing for this path?
bool wxMoTableCell::IsDeleteButtonShowing(wxMoTableCtrl* tableCtrl) const
{
    wxTablePath thisPath;
    if (tableCtrl->FindPathForCell(const_cast<wxMoTableCell*>(this), thisPath))
        return (tableCtrl->GetDeletePath() == thisPath);
    else
        return false;
}

// Prepares the cell for reuse
void wxMoTableCell::PrepareForReuse(wxMoTableCtrl* WXUNUSED(tableCtrl))
{
    SetText(wxEmptyString);
    SetDetailText(wxEmptyString);
    SetCellStyle(CellStyleDefault);
    SetIndentationLevel(0);
    SetIndentationWidth(0);
    SetAccessoryType(AccessoryTypeNone);
    SetEditingAccessoryType(AccessoryTypeNone);
    SetTextAlignment(TextAlignmentLeft);
    SetDetailTextAlignment(TextAlignmentLeft);
    SetLineBreakMode(LineBreakModeWordWrap);
    SetSelectionStyle(SelectionStyleBlue);
}

IMPLEMENT_DYNAMIC_CLASS(wxMoTableCellContentWindow, wxWindow)

BEGIN_EVENT_TABLE(wxMoTableCellContentWindow, wxWindow)
    EVT_PAINT(wxMoTableCellContentWindow::OnPaint)
    EVT_MOUSE_EVENTS(wxMoTableCellContentWindow::OnMouseEvents)
END_EVENT_TABLE()

bool wxMoTableCellContentWindow::Create(wxWindow* parent, wxWindowID id,
        const wxPoint& pos, const wxSize& sz, long style)
{
    wxWindow::Create(parent, id, pos, sz, style);

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);    
    SetInitialSize(sz);

    return true;
}

void wxMoTableCellContentWindow::Init()
{
    m_cell = NULL;
    m_tableCtrl = NULL;
}

void wxMoTableCellContentWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif
    if (!m_cell)
        return;

    bool isSelected = m_cell->GetSelected();
    dc.SetBackground(wxBrush(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_BG)));
    if (m_cell)
    {
        // Finds the path for the cell
        wxTablePath path;
        if (m_tableCtrl->FindPathForCell(m_cell, path) && m_tableCtrl->GetSelection() &&
            path == *m_tableCtrl->GetSelection())
        {
            isSelected = true;
            if (m_cell->GetSelectionStyle() == wxMoTableCell::SelectionStyleBlue)
            {
                dc.SetBackground(wxBrush(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_BLUE_SELECTION)));
            }
            else if (m_cell->GetSelectionStyle() == wxMoTableCell::SelectionStyleGrey)
            {
                dc.SetBackground(wxBrush(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_GREY_SELECTION)));
            }
        }
    }
    dc.Clear();

    wxSize sz = GetClientSize();

    if (m_cell && m_tableCtrl)
    {
        int padding = 8;
        int leftMargin = padding;

        if (m_cell->GetIndentationLevel() > 0 && (!m_tableCtrl->IsEditing() || m_cell->GetIndentWhileEditing()))
            leftMargin += m_cell->GetIndentationLevel() * m_cell->GetIndentationWidth();

        wxBitmap bitmap = m_cell->GetBitmap();
        if (isSelected && m_cell->GetSelectedBitmap().IsOk())
            bitmap = m_cell->GetSelectedBitmap();

        if (bitmap.IsOk())
        {
            int x = leftMargin;
            int y = (sz.y - bitmap.GetWidth())/2;
            dc.DrawBitmap(bitmap, x, y, true);

            leftMargin += bitmap.GetWidth();
            leftMargin += padding;
        }

        if (!m_cell->GetText().IsEmpty())
        {
            wxFont font = m_cell->GetFont();
            if (!font.IsOk())
                font = m_tableCtrl->GetFont();
            dc.SetFont(font);
            wxColour colour;
            if (isSelected)
            {
                colour = m_cell->GetSelectedTextColour();
                if (!colour.IsOk())
                    colour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_SELECTED_TEXT);
            }
            else
                colour = m_cell->GetTextColour();
            if (!colour.IsOk())
                colour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_ROW_TEXT);
            dc.SetTextForeground(colour);
            dc.SetBackgroundMode(wxTRANSPARENT);

            if (m_cell->GetCellStyle() == wxMoTableCell::CellStyleDefault)
            {
                // Default, plain style

                wxCoord w, h;
                dc.GetTextExtent(m_cell->GetText(), & w, & h);
                int x = leftMargin;
                int y = (sz.y - h)/2;
                
                dc.DrawText(m_cell->GetText(), x, y);
            }
            else if (m_cell->GetCellStyle() == wxMoTableCell::CellStyleValue1)
            {
                // A style for a cell with a label on the left side of the cell
                // with left-aligned and black text; on the right side is a label
                // that has smaller blue text and is right-aligned. The Settings
                // application uses cells in this style.
                wxCoord w, h;
                dc.GetTextExtent(m_cell->GetText(), & w, & h);
                int x = leftMargin;
                int y = (sz.y - h)/2;
                
                dc.DrawText(m_cell->GetText(), x, y);

                if (!m_cell->GetDetailText().IsEmpty())
                {
                    // If the cell is selected, use the same colour as the main label.
                    if (!isSelected)
                    {
                        wxColour colour = m_cell->GetDetailTextColour();
                        if (!colour.IsOk())
                            colour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_DETAIL_TEXT_BLUE);
                        dc.SetTextForeground(colour);
                    }

                    wxFont detailFont = m_cell->GetDetailFont();
                    if (!detailFont.IsOk())
                        detailFont = wxMoSystemSettings::GetFont(wxMO_FONT_TABLE_DETAIL);

                    dc.SetFont(detailFont);
                    dc.GetTextExtent(m_cell->GetDetailText(), & w, & h);
                    int x = sz.x - w - padding;
                    int y = (sz.y - h)/2;

                    dc.DrawText(m_cell->GetDetailText(), x, y);
                }
            }
            else if (m_cell->GetCellStyle() == wxMoTableCell::CellStyleValue2)
            {
                // A style for a cell with a label on the left side of the cell with
                // text that is right-aligned and blue; on the right side of the cell
                // is another label with smaller text (JACS: actually larger) that is
                // left-aligned and black.
                // The Phone/Contacts application uses cells in this style.

                wxCoord w, h;
                dc.GetTextExtent(m_cell->GetText(), & w, & h);
                int x = m_cell->GetDetailWidth() + leftMargin;
                int y = (sz.y - h)/2;
                
                dc.DrawText(m_cell->GetText(), x, y);

                if (!m_cell->GetDetailText().IsEmpty())
                {
                    // If the cell is selected, use the same colour as the main label.
                    if (!isSelected)
                    {
                        wxColour colour = m_cell->GetDetailTextColour();
                        if (!colour.IsOk())
                            colour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_DETAIL_TEXT_BLUE);
                        dc.SetTextForeground(colour);
                    }

                    wxFont detailFont = m_cell->GetDetailFont();
                    if (!detailFont.IsOk())
                        detailFont = wxMoSystemSettings::GetFont(wxMO_FONT_TABLE_DETAIL);

                    dc.SetFont(detailFont);
                    dc.GetTextExtent(m_cell->GetDetailText(), & w, & h);
                    int x = leftMargin + m_cell->GetDetailWidth() - w - padding;
                    int y = (sz.y - h)/2;

                    dc.DrawText(m_cell->GetDetailText(), x, y);
                }
            }
            else if (m_cell->GetCellStyle() == wxMoTableCell::CellStyleSubtitle)
            {
                // A style for a cell with a left-aligned label across the top and a
                // left-aligned label below it in smaller gray text. The iPod application
                // uses cells in this style.
                wxCoord w, h;
                dc.GetTextExtent(m_cell->GetText(), & w, & h);
                int x = leftMargin;

                wxCoord dw, dh;

                wxFont detailFont = m_cell->GetDetailFont();
                if (!detailFont.IsOk())
                    detailFont = wxMoSystemSettings::GetFont(wxMO_FONT_TABLE_DETAIL);

                dc.SetFont(detailFont);
                dc.GetTextExtent(wxT("Xy"), & dw, & dh);

                int interItemSpacing = (sz.y - dh - h)/3;

                int y = interItemSpacing;

                dc.SetFont(font);
                dc.DrawText(m_cell->GetText(), x, y);

                if (!m_cell->GetDetailText().IsEmpty())
                {
                    // If the cell is selected, use the same colour as the main label.
                    if (!isSelected)
                    {
                        wxColour colour = m_cell->GetDetailTextColour();
                        if (!colour.IsOk())
                            colour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_DETAIL_TEXT_GREY);
                        dc.SetTextForeground(colour);
                    }
                    int dy = interItemSpacing * 2 + h;
                    dc.SetFont(detailFont);
                    dc.DrawText(m_cell->GetDetailText(), x, dy);
                }
            }
        }
    }
}

void wxMoTableCellContentWindow::OnMouseEvents(wxMouseEvent& event)
{
    if (m_cell)
    {
        wxMoTableCtrl* tableCtrl = NULL;
        wxWindow* p = GetParent();
        while (p)
        {
            wxMoTableCtrl* t = wxDynamicCast(p, wxMoTableCtrl);
            if (t)
            {
                tableCtrl = t;
                break;
            }
            
            p = p->GetParent();                
        }
        
        if (tableCtrl)
        {
            // Since in wxWidgets we can't detect this control's mouse events
            // using the parent window,  we must explicitly forward events to
            // the parent in order to do scroll detection.
            // In future we might do some clever auto-pushing of an event
            // handler for whatever content the programmer uses in the content
            // or accessory areas.
            // Note that this is only going to work for controls that don't
            // do any mouse processing, and the scrolled window doesn't
            // know which child control to send the cancel touch event to - it
            // always sends it to itself (the scrolled window).
            tableCtrl->ForwardMouseEvent(this,  event);
        }
    }
}
