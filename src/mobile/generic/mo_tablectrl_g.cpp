/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_tablectrl_g.cpp
// Purpose:     wxMoTableCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/*
Notes

  To construct index path in Cocoa Touch, use this constructor:

  [NSIndexPath indexPathForRow: inSection:]

  Need to call reloadData before scrolling to a row. Perhaps
  this can be incorporated into ScrollToRow.

  UITableViewCell memory management

  We need to support the reuse of cells.
  How do we manage these objects?
  Need to do what Cocoa Touch does, and maintain a list of cells
  which are no longer in use.
  Items can be cached so we're not always creating or reusing cells,
  only when more items scroll into view.

  Control needs to store just enough info to keep track of each row height,
  section dimensions etc.

  Since we need to allow customisable row appearance, we probably need to
  use content windows as per Cocoa Touch, so subwindows can be attached.
  So, the standard content window would draw the image and text, which
  would be obscured by a custom window placed in the content view.

  Add a wxMoTableCellContentWindow which does the image + label drawing,
  and can be used as the parent of custom cell windows.

  */

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
    #include "wx/image.h"
#endif

#include "wx/mstream.h"
#include "wx/dcbuffer.h"
#include "wx/arrimpl.cpp"

#include "wx/mobile/generic/utils.h"
#include "wx/mobile/generic/tablectrl.h"

#include "wx/mobile/generic/bitmaps/disclosure_16x20.inc"
#include "wx/mobile/generic/bitmaps/detail_disclosure_27x27.inc"
#include "wx/mobile/generic/bitmaps/checkmark_15x15.inc"
#include "wx/mobile/generic/bitmaps/add_button_27x27.inc"
#include "wx/mobile/generic/bitmaps/delete_button_27x27.inc"
#include "wx/mobile/generic/bitmaps/delete_button_rotated_27x27.inc"
#include "wx/mobile/generic/bitmaps/reorder_27x27.inc"

#define MO_EDIT_CONTROL_MARGIN 35

IMPLEMENT_DYNAMIC_CLASS(wxMoTableCtrl, wxMoScrolledWindow)
IMPLEMENT_DYNAMIC_CLASS(wxTablePath, wxObject)
IMPLEMENT_CLASS(wxMoTableDataSource, wxEvtHandler)
IMPLEMENT_CLASS(wxMoTableRow, wxObject)
IMPLEMENT_CLASS(wxMoTableSection, wxObject)

WX_DEFINE_EXPORTED_OBJARRAY(wxMoTableSectionArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxMoTableRowArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxTablePathArray);

BEGIN_EVENT_TABLE(wxMoTableCtrl, wxMoScrolledWindow)
    EVT_PAINT(wxMoTableCtrl::OnPaint)
    EVT_SIZE(wxMoTableCtrl::OnSize)
    EVT_TOUCH_SCROLL_DRAG(wxMoTableCtrl::OnTouchScrollDrag)
    EVT_TOUCH_SCROLL_CANCEL_TOUCHES(wxMoTableCtrl::OnCancelTouch)
    EVT_MOUSE_EVENTS(wxMoTableCtrl::OnMouseEvent)
    EVT_ERASE_BACKGROUND(wxMoTableCtrl::OnEraseBackground)
    EVT_MOUSE_CAPTURE_CHANGED(wxMoTableCtrl::OnMouseCaptureChanged)

    EVT_TABLE_ADD_CLICKED(wxID_ANY, wxMoTableCtrl::OnAddClicked)
    EVT_TABLE_DELETE_CLICKED(wxID_ANY, wxMoTableCtrl::OnDeleteClicked)
    EVT_TABLE_CONFIRM_DELETE_CLICKED(wxID_ANY, wxMoTableCtrl::OnConfirmDeleteClicked)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxTableCtrlEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ROW_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ACCESSORY_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ADD_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_DELETE_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_CONFIRM_DELETE_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_MOVE_DRAGGED)

#define wxMoTableCtrlLoadBitmap(bitmapData) LoadBitmap(bitmapData, sizeof(bitmapData))

bool wxMoTableCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    if (!wxMoScrolledWindow::Create(parent, id, pos, size, style, name))
        return false;

    SetShowVerticalScrollIndicator(true);
    SetShowHorizontalScrollIndicator(false);
    SetScrollbars(0, 1, 0, 0);

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_TABLE_ROWS));
    SetSectionFont(wxMoSystemSettings::GetFont(wxMO_FONT_TABLE_SECTIONS));
    SetInitialSize(size);

    m_disclosureBitmap = wxMoTableCtrlLoadBitmap(disclosure_16x20_png);
    m_detailDisclosureBitmap = wxMoTableCtrlLoadBitmap(detail_disclosure_27x27_png);
    m_checkmarkBitmap = wxMoTableCtrlLoadBitmap(checkmark_15x15_png);
    m_deleteBitmap = wxMoTableCtrlLoadBitmap(delete_button_27x27_png);
    m_rotatedDeleteBitmap = wxMoTableCtrlLoadBitmap(delete_button_rotated_27x27_png);
    m_addBitmap = wxMoTableCtrlLoadBitmap(add_button_27x27_png);
    m_reorderBitmap = wxMoTableCtrlLoadBitmap(reorder_27x27_png);

    if (style & wxTC_PLAIN)
    {
        m_leftRightMargin = 0;
        m_topBottomMargin = 0;
    }
    else
    {
        m_leftRightMargin = 8;
        m_topBottomMargin = 8;
    }

    return true;
}

wxMoTableCtrl::~wxMoTableCtrl()
{
    Clear();

    if (m_ownsDataSource)
    {
        delete m_dataSource;
        m_dataSource = NULL;
    }
}

void wxMoTableCtrl::Init()
{
    m_dataSource = NULL;
    m_ownsDataSource = false;
    m_leftRightMargin = 0;
    m_topBottomMargin = 0;
    m_interSectionSpacing = 8;
    m_rowHeight = 40;
    m_sectionHeaderHeight = 18;
    m_sectionFooterHeight = 0;
    m_totalTableHeight = 0;
    m_accessoryWidth = 40;
    m_selection.SetSection(-1);
    m_selection.SetRow(-1);
    m_mouseStatus = MouseStatusNone;
    m_separatorStyle = CellSeparatorStyleSingleLine;
    m_freezeCount = 0;
    m_editingMode = false;
    m_deleteButton = NULL;
    m_deletePath = wxTablePath(-1, -1);
    m_insertAt = wxTablePath(-1, -1);
    m_originalInsertionPoint = wxTablePath(-1, -1);
    m_allowsSelectionDuringEditing = false;
}

wxSize wxMoTableCtrl::DoGetBestSize() const
{
    wxSize best(100, 100);
    CacheBestSize(best);
    return best;
}

// Load a bitmap from data
wxBitmap wxMoTableCtrl::LoadBitmap(const char* bitmapData, size_t len)
{
    wxMemoryInputStream is(bitmapData, len);
    wxBitmap bitmap = wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
    return bitmap;
}

bool wxMoTableCtrl::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxWindow::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    if (!IsFrozen())
        Refresh();

    return true;
}

bool wxMoTableCtrl::SetForegroundColour(const wxColour &colour)
{
    if ( !wxWindow::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    if (!IsFrozen())
        Refresh();

    return true;
}

void wxMoTableCtrl::Freeze()
{
    wxMoWindow::Freeze();

    m_freezeCount ++;
}

void wxMoTableCtrl::Thaw()
{
    m_freezeCount --;
    if (m_freezeCount < 0)
        m_freezeCount = 0;
    if (m_freezeCount == 0)
    {
        ReloadData(false /* don't reset scrollbars */);
    }

    wxMoWindow::Thaw();
}

bool wxMoTableCtrl::Enable(bool enable)
{
    wxWindow::Enable(enable);
    if (!IsFrozen())
        Refresh();
    return true;
}

bool wxMoTableCtrl::SetFont(const wxFont& font)
{
    wxWindow::SetFont(font);
    //RecalculateDimensions();
    if (!IsFrozen())
        Refresh();

    return true;
}

bool wxMoTableCtrl::SetSectionFont(const wxFont& font)
{
    m_sectionFont = font;
    //RecalculateDimensions();
    if (!IsFrozen())
        Refresh();

    return true;
}

// Recalculates dimensions
bool wxMoTableCtrl::RecalculateDimensions()
{
    wxClientDC dc(this);

    wxCoord w, h;
    dc.SetFont(GetFont());
    dc.GetTextExtent(wxT("Xy"), & w, & h);
    m_rowHeight = wxMin(30, h*2);

    dc.SetFont(GetSectionFont());
    dc.GetTextExtent(wxT("Xy"), & w, & h);
    m_sectionHeaderHeight = wxMin(40, h*2);

    return true;
}

void wxMoTableCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    LoadVisibleData(wxRect(wxPoint(0, 0), GetClientSize()));

    // TODO: use this
    wxRegion updateRegion = GetUpdateRegion();

    wxColour backgroundColour;
    wxColour sectionTextColour;
    int leftMargin = m_leftRightMargin;
    int rightMargin = m_leftRightMargin;
    if (IsPlain())
    {
        backgroundColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_BG);
        sectionTextColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_PLAIN_SECTION_TEXT);
    }
    else
    {
        backgroundColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_SURROUND);
        sectionTextColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_GROUPED_SECTION_TEXT);
    }

    // Allow for edit controls
    if (IsEditing())
        leftMargin = leftMargin + MO_EDIT_CONTROL_MARGIN;

    wxPen plainSectionHeaderPen(wxPen(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_LINE)));
    wxBrush plainSectionHeaderBrush(wxBrush(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_PLAIN_SECTION_HEADER)));

    dc.SetBackground(wxBrush(backgroundColour));
    dc.SetFont(GetFont());
    dc.Clear();

    PrepareDC(dc);

    int sectionCount = m_sections.GetCount();

    wxColour sectionBgColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_BG));
    wxBrush brush(sectionBgColour);
    wxPen linePen(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_LINE));
    wxSize sz = GetClientSize();

    int rectCorner = 6;

    // TODO: optimize this so only data within the update region is drawn

    // Titles can be empty, if no section titles
    int sectionIdx;
    for (sectionIdx = 0; sectionIdx < sectionCount; sectionIdx ++)
    {
        const wxMoTableSection& section = m_sections[sectionIdx];

        wxRect sectionRect(leftMargin, section.m_sectionY, sz.x - leftMargin - rightMargin, section.m_sectionHeight);

        int sectionHeaderHeight = section.m_sectionHeaderHeight;
        //int sectionFooterHeight = section.m_sectionHeaderHeight;

        if (IsGrouped())
        {
            dc.SetPen(linePen);
            dc.SetBrush(brush);
            dc.DrawRoundedRectangle(sectionRect, rectCorner);
        }

        if (!section.GetSectionName().IsEmpty())
        {
            if (IsPlain())
            {
                wxRect sectionHeaderRect(0, section.m_sectionY - sectionHeaderHeight,
                    sz.x, sectionHeaderHeight);
                dc.SetPen(plainSectionHeaderPen);
                dc.SetBrush(plainSectionHeaderBrush);
                dc.DrawRectangle(sectionHeaderRect);
            }
            int w, h;
            dc.GetTextExtent(section.GetSectionName(), & w, & h);
            int x = leftMargin + 4;
            int y = (section.m_sectionY - sectionHeaderHeight) + (sectionHeaderHeight - h)/2;
            dc.SetTextForeground(sectionTextColour);
            dc.DrawText(section.GetSectionName(), x, y);
        }

        int rowCount = section.m_rows.GetCount();
        int rowIdx;
        for (rowIdx = 0; rowIdx < rowCount; rowIdx ++)
        {
            const wxMoTableRow& row = section.m_rows[rowIdx];

            wxTablePath path(sectionIdx, rowIdx);

            if (row.m_tableCell)
            {
                wxRect rowRect(leftMargin,
                    row.m_rowY,
                    sz.x - leftMargin - rightMargin,
                    row.m_rowHeight);
                if (m_selection == path && row.m_tableCell->GetSelectionStyle() != wxMoTableCell::SelectionStyleNone)
                {
                    if (row.m_tableCell->GetSelectionStyle() == wxMoTableCell::SelectionStyleBlue)
                    {
                        dc.SetPen(wxPen(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_BLUE_SELECTION)));
                        dc.SetBrush(wxBrush(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_BLUE_SELECTION)));
                    }
                    else if (row.m_tableCell->GetSelectionStyle() == wxMoTableCell::SelectionStyleGrey)
                    {
                        dc.SetPen(wxPen(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_GREY_SELECTION)));
                        dc.SetBrush(wxBrush(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_GREY_SELECTION)));
                    }
                    dc.DrawRectangle(rowRect);
                }

                int accessoryWidth = 0;

                int reorderWidth = 0;
                if (IsEditing() && row.m_tableCell->GetShowReorderingControl())
                    reorderWidth = m_accessoryWidth;

                {
                    int accessoryType;
                    if (IsEditing())
                        accessoryType = row.m_tableCell->GetEditingAccessoryType();
                    else
                        accessoryType = row.m_tableCell->GetAccessoryType();

                    if (accessoryType != wxMoTableCell::AccessoryTypeNone)
                        accessoryWidth = m_accessoryWidth;

                    if (!IsEditing() && row.m_tableCell->GetAccessoryWindow())
                        accessoryWidth = row.m_tableCell->GetAccessoryWindow()->GetSize().x;

                    if (IsEditing() && row.m_tableCell->GetEditingAccessoryWindow())
                        accessoryWidth = row.m_tableCell->GetEditingAccessoryWindow()->GetSize().x;

                    if (accessoryType == wxMoTableCell::AccessoryTypeDisclosureIndicator)
                    {
                        int x = sz.x - rightMargin - m_accessoryWidth - reorderWidth + (m_accessoryWidth - m_disclosureBitmap.GetWidth())/2;
                        int y = row.m_rowY + (row.m_rowHeight - m_disclosureBitmap.GetHeight())/2;
                        dc.DrawBitmap(m_disclosureBitmap, x, y, true);
                    }
                    else if (accessoryType == wxMoTableCell::AccessoryTypeDetailDisclosureButton)
                    {
                        int x = sz.x - rightMargin - m_accessoryWidth - reorderWidth + (m_accessoryWidth - m_detailDisclosureBitmap.GetWidth())/2;
                        int y = row.m_rowY + (row.m_rowHeight - m_detailDisclosureBitmap.GetHeight())/2;
                        dc.DrawBitmap(m_detailDisclosureBitmap, x, y, true);
                    }
                    else if (accessoryType == wxMoTableCell::AccessoryTypeCheckmark)
                    {
                        int x = sz.x - rightMargin - m_accessoryWidth - reorderWidth + (m_accessoryWidth - m_checkmarkBitmap.GetWidth())/2;
                        int y = row.m_rowY + (row.m_rowHeight - m_checkmarkBitmap.GetHeight())/2;
                        dc.DrawBitmap(m_checkmarkBitmap, x, y, true);
                    }
                }

                if (!DeletingThisRow(path))
                {
                    if (IsEditing() && row.m_tableCell->GetShowReorderingControl())
                    {
                        if (m_reorderBitmap.IsOk())
                        {
                            int x = sz.x - rightMargin - reorderWidth + (reorderWidth - m_reorderBitmap.GetWidth())/2;
                            int y = row.m_rowY + (row.m_rowHeight - m_reorderBitmap.GetHeight())/2;
                            dc.DrawBitmap(m_reorderBitmap, x, y, true);

                            if (GetCellSeparatorStyle() != CellSeparatorStyleNone)
                            {
                                dc.SetPen(linePen);

                                int lx = sz.x - rightMargin - reorderWidth;
                                dc.DrawLine(lx, row.m_rowY,
                                            lx, row.m_rowY+row.m_rowHeight);
                            }
                        }
                    }
                }
            }

            if (IsEditing() && GetDataSource() && GetDataSource()->CanEditRow(this, path))
            {
                CellEditStyle editStyle = GetDataSource()->GetCellEditStyle(this, path);

                if (editStyle == EditStyleShowDeleteButton && m_deleteBitmap.IsOk())
                {
                    // TODO: choose between delete bitmap and rotated delete bitmap
                    int x = (leftMargin - m_deleteBitmap.GetWidth())/2;
                    int y = row.m_rowY + (row.m_rowHeight - m_deleteBitmap.GetHeight())/2;
                    if (DeletingThisRow(path))
                        dc.DrawBitmap(m_rotatedDeleteBitmap, x, y);
                    else
                        dc.DrawBitmap(m_deleteBitmap, x, y);

                }
                else if (editStyle == EditStyleShowInsertButton && m_addBitmap.IsOk())
                {
                    int x = (leftMargin - m_addBitmap.GetWidth())/2;
                    int y = row.m_rowY + (row.m_rowHeight - m_addBitmap.GetHeight())/2;
                    dc.DrawBitmap(m_addBitmap, x, y);
                }
            }

            if (rowIdx != (rowCount-1) && GetCellSeparatorStyle() != CellSeparatorStyleNone)
            {
                dc.SetPen(linePen);
                dc.DrawLine(leftMargin, row.m_rowY+row.m_rowHeight,
                            sz.x - rightMargin, row.m_rowY+row.m_rowHeight);
            }

            // Draw the insertion indicator when dragging a row.
            // This isn't how it looks on Cocoa Touch, but it should
            // still allow testing of row reordering.

            if (IsEditing() && m_insertAt.IsValid())
            {
                int y = row.m_rowY;
                bool drawInsertionMark = false;

                if (path == m_insertAt)
                {
                    drawInsertionMark = true;
                }
                else if (wxTablePath(path.GetSection(), path.GetRow()+1) == m_insertAt &&
                         (path.GetRow()+1) == section.m_rowCount)
                {
                    drawInsertionMark = true;
                    y = row.m_rowY + row.m_rowHeight;
                }

                if (drawInsertionMark)
                {
                    dc.SetPen(*wxBLACK_PEN);
                    dc.SetBrush(*wxBLACK_BRUSH);
                    int arrowSize = 8;
                    wxPoint points[3];
                    points[0].x = sz.x - arrowSize;
                    points[0].y = y;
                    points[1].x = sz.x;
                    points[1].y = y-arrowSize;
                    points[2].x = sz.x;
                    points[2].y = y+arrowSize;
                    dc.DrawPolygon(3, points);
                }
            }
        }
    }

    if (IsPlain() && HasIndex())
    {
        int startX, startY;
        GetViewStart(& startX, & startY);

        dc.SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_TABLE_INDEX));
        dc.SetTextForeground(wxMoSystemSettings::GetColour(wxMO_COLOUR_TABLE_INDEX_TEXT));

        wxCoord w, h;
        dc.GetTextExtent(wxT("Xy"), & w, & h);

        int spacing = wxMax(2, ((sz.y - h*m_indexTitles.GetCount())/(1+m_indexTitles.GetCount())));
        int rightMargin = 8;
        size_t i;
        int y = spacing + startY;
        for (i = 0; i < m_indexTitles.GetCount(); i++)
        {
            wxString title(m_indexTitles[i]);
            dc.GetTextExtent(title, & w, & h);
            int x = sz.x - rightMargin - w;
            dc.DrawText(title, x, y);

            y += h;
            y += spacing;
        }
    }
}

void wxMoTableCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

// Hit test
int wxMoTableCtrl::HitTest(const wxPoint& pt, int& retSection, int& retRow, int& region) const
{
    retSection = -1;
    retRow = -1;
    region = HitTestRegionUnknown;

    wxSize sz = GetClientSize();
    int startX, startY;
    GetViewStart(& startX, & startY);

    wxPoint logicalPt = wxPoint(pt.x + startX, pt.y + startY);
    int leftMargin = m_leftRightMargin;
    int rightMargin = m_leftRightMargin;
    int rowMarginLeftRight = 4;

    // Allow for edit controls
    if (IsEditing())
        leftMargin = leftMargin + MO_EDIT_CONTROL_MARGIN;

    // First try index
    if (IsPlain() && HasIndex())
    {
        if (pt.x >= sz.x - m_accessoryWidth)
        {
            // figure out which index entry we're on
            wxClientDC dc((wxWindow*) this);
            dc.SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_TABLE_INDEX));

            wxCoord w, h;
            dc.GetTextExtent(wxT("Xy"), & w, & h);

            int spacing = wxMax(2, ((sz.y - h*m_indexTitles.GetCount())/(1+m_indexTitles.GetCount())));
            //int rightMargin = 8;
            int indexEntry = -1;
            size_t i;
            int y = spacing ;
            for (i = 0; i < m_indexTitles.GetCount(); i++)
            {
                wxString title(m_indexTitles[i]);

                if (pt.y >= (y-spacing/2) && pt.y <= (y + h + spacing/2))
                {
                    indexEntry = i;
                    break;
                }

                y += h;
                y += spacing;
            }

            if (indexEntry != -1)
            {
                retSection = indexEntry;
                return HitTestIndex;
            }
        }
    }

    int sectionIdx;
    int sectionCount = m_sections.GetCount();
    for (sectionIdx = 0; sectionIdx < sectionCount; sectionIdx ++)
    {
        const wxMoTableSection& section = m_sections[sectionIdx];

        int rowCount = section.m_rows.GetCount();
        int rowIdx;
        for (rowIdx = 0; rowIdx < rowCount; rowIdx ++)
        {
            const wxMoTableRow& row = section.m_rows[rowIdx];
            wxTablePath path(sectionIdx, rowIdx);

            if (row.m_tableCell && row.m_tableCell->GetContentWindow())
            {
                int accessoryWidth = 0;
                int accessoryMargin = 0;
                int reorderingControlWidth = 0;

                // We have to cheat a bit and make space for an accessory
                // if we have an index, since we can't draw on an arbitrary
                // content window.
                if ((!IsEditing() && row.m_tableCell->HasAccessory()) || (IsEditing() && row.m_tableCell->HasEditingAccessory()) ||
                    HasIndex())
                {
                    accessoryWidth = m_accessoryWidth;
                    accessoryMargin = rowMarginLeftRight;
                }

                if (row.m_tableCell->GetAccessoryWindow() && row.m_tableCell->GetAccessoryWindow()->IsShown())
                {
                    accessoryWidth = row.m_tableCell->GetAccessoryWindow()->GetSize().x;
                    accessoryMargin = rowMarginLeftRight;
                }

                if (row.m_tableCell->GetEditingAccessoryWindow() && row.m_tableCell->GetEditingAccessoryWindow()->IsShown())
                {
                    accessoryWidth = row.m_tableCell->GetEditingAccessoryWindow()->GetSize().x;
                    accessoryMargin = rowMarginLeftRight;
                }

                if (IsEditing() && row.m_tableCell->GetShowReorderingControl())
                    reorderingControlWidth = m_accessoryWidth;

                wxRect rowRect(leftMargin,
                    row.m_rowY,
                    sz.x - leftMargin - rightMargin,
                    row.m_rowHeight);

                if (rowRect.Contains(logicalPt))
                {
                    // Are we at the top or the bottom of this row?
                    wxRect rowRectTop(leftMargin,
                        row.m_rowY - row.m_rowHeight/2,
                        sz.x - leftMargin - rightMargin,
                        row.m_rowHeight);

                    if (rowRectTop.Contains(logicalPt))
                        region = HitTestRegionTop;
                    else
                        region = HitTestRegionBottom;
                }

                if (rowRect.Contains(logicalPt))
                {
                    wxRect contentRect(row.m_tableCell->GetContentWindow()->GetRect());

                    wxRect accessoryRect(sz.x - rightMargin - accessoryWidth - reorderingControlWidth,
                        row.m_rowY,
                        accessoryWidth,
                        row.m_rowHeight);

                    retSection = sectionIdx;
                    retRow = rowIdx;

                    if (reorderingControlWidth > 0)
                    {
                        wxRect reorderingRect(sz.x - rightMargin - reorderingControlWidth,
                        row.m_rowY,
                        reorderingControlWidth,
                        row.m_rowHeight);

                        if (reorderingRect.Contains(logicalPt))
                            return HitTestReorder;
                    }

                    if (accessoryRect.Contains(logicalPt))
                    {
                        if ((!IsEditing() && row.m_tableCell->HasAccessory()) || (IsEditing() && row.m_tableCell->HasEditingAccessory()))
                        {
                            if ((row.m_tableCell->GetAccessoryType() == wxMoTableCell::AccessoryTypeDetailDisclosureButton) ||
                                (row.m_tableCell->GetEditingAccessoryType() == wxMoTableCell::AccessoryTypeDetailDisclosureButton))
                            {
                                return HitTestAccessory;
                            }
                        }
                    }
                    return HitTestRow;
                }
                if (IsEditing() && GetDataSource() && GetDataSource()->CanEditRow((wxMoTableCtrl*) this, path))
                {
                    CellEditStyle editStyle = GetDataSource()->GetCellEditStyle((wxMoTableCtrl*) this, path);

                    if (editStyle == EditStyleShowDeleteButton && m_deleteBitmap.IsOk())
                    {
                        int x = (leftMargin - m_deleteBitmap.GetWidth())/2;
                        int y = row.m_rowY + (row.m_rowHeight - m_deleteBitmap.GetHeight())/2;
                        int w = m_deleteBitmap.GetWidth();
                        int h = m_deleteBitmap.GetHeight();
                        wxRect buttonRect(x, y, w, h);
                        if (buttonRect.Contains(logicalPt))
                        {
                            retSection = sectionIdx;
                            retRow = rowIdx;

                            return HitTestDelete;
                        }
                    }
                    else if (editStyle == EditStyleShowInsertButton && m_addBitmap.IsOk())
                    {
                        int x = (leftMargin - m_addBitmap.GetWidth())/2;
                        int y = row.m_rowY + (row.m_rowHeight - m_addBitmap.GetHeight())/2;
                        int w = m_addBitmap.GetWidth();
                        int h = m_addBitmap.GetHeight();
                        wxRect buttonRect(x, y, w, h);
                        if (buttonRect.Contains(logicalPt))
                        {
                            retSection = sectionIdx;
                            retRow = rowIdx;

                            return HitTestAdd;
                        }
                    }
                }
            }
        }
    }
    return HitTestNone;
}

void wxMoTableCtrl::OnMouseEvent(wxMouseEvent& event)
{
    if (!IsEnabled())
        return;

    wxPoint positionMidRow = event.GetPosition();
    positionMidRow.x = GetClientSize().x/2;

    if (event.LeftDown() && IsEditing())
    {
        int section, row, region;
        int hit = HitTest(event.GetPosition(), section, row, region);
        wxTablePath path(section, row);
        if (hit == HitTestReorder)
        {
            // Definitely don't want to scroll
            CancelScrolling();

            m_insertAt = path;
            m_originalInsertionPoint = path;
            m_mouseStatus = MouseStatusDraggingRow;
            if (GetCapture() != this)
                CaptureMouse();
            Refresh();

            return;
        }
    }

    if (m_mouseStatus == MouseStatusDraggingRow && event.Dragging())
    {
        int section, row, region;
        int hit = HitTest(positionMidRow, section, row, region);
        wxTablePath path(section, row);
        if (hit == HitTestAccessory || hit == HitTestRow || hit == HitTestAdd || hit == HitTestDelete || hit == HitTestReorder)
        {
            if (region == HitTestRegionBottom)
                path.SetRow(row+1);

            m_insertAt = path;
            Refresh();
            return;
        }
    }

    if (event.LeftDown() && !Deleting() && (!IsEditing() || GetAllowSelectionDuringEditing()))
    {
        m_mouseStatus = MouseStatusNone;

        int section, row, region;
        int hit = HitTest(event.GetPosition(), section, row, region);
        wxTablePath path(section, row);
        if (hit == HitTestRow)
        {
            if (path != m_selection && m_selection.IsValid())
            {
                wxMoTableCell* cell = FindCellForPath(m_selection);
                if (cell)
                    cell->SetSelected(false);
            }

            //m_mouseStatus = MouseStatusClicking;
            CaptureMouse();

            SetSelection(path);
            Update();
        }
    }
    else if (event.LeftUp())
    {
        if (GetCapture() == this)
            ReleaseMouse();

        if (m_mouseStatus == MouseStatusDraggingRow)
        {
            wxTablePath insertionPath = m_insertAt;
            wxTablePath originalInsertionPath = m_originalInsertionPoint;

            m_insertAt = wxTablePath(-1, -1);
            m_originalInsertionPoint = wxTablePath(-1, -1);
            m_mouseStatus = MouseStatusNone;

            if (insertionPath != originalInsertionPath && m_dataSource->CanMoveRow(this, originalInsertionPath))
            {
                m_dataSource->MoveRow(this, originalInsertionPath, insertionPath);

                ReloadData(false);
            }
            else
                Refresh();

            return;
        }

        if (Deleting())
        {
            HideDeleteButton();
            return;
        }

        // m_mouseStatus = MouseStatusNone;

        int section = -1, row = -1, region = HitTestRegionUnknown;
        int hit = HitTest(event.GetPosition(), section, row, region);
        wxTablePath path(section, row);
        if (hit == HitTestAccessory)
        {
            wxTableCtrlEvent event(wxEVT_COMMAND_TABLE_ACCESSORY_CLICKED, GetId(), this);
            event.SetPath(path);
            event.SetEventObject(this);

            if (!GetDataSource() || !GetDataSource()->ProcessEvent(event))
                GetEventHandler()->ProcessEvent(event);
        }
        else if (hit == HitTestAdd)
        {
            wxTableCtrlEvent event(wxEVT_COMMAND_TABLE_ADD_CLICKED, GetId(), this);
            event.SetPath(path);
            event.SetEventObject(this);

            if (!GetDataSource() || !GetDataSource()->ProcessEvent(event))
                GetEventHandler()->ProcessEvent(event);
        }
        else if (hit == HitTestDelete)
        {
            wxTableCtrlEvent event(wxEVT_COMMAND_TABLE_DELETE_CLICKED, GetId(), this);
            event.SetPath(path);
            event.SetEventObject(this);

            if (!GetDataSource() || !GetDataSource()->ProcessEvent(event))
                GetEventHandler()->ProcessEvent(event);
        }
#if 0
        // Handled by button, not icon
        else if (hit == HitTestConfirmDelete)
        {
            wxTableCtrlEvent event(wxEVT_COMMAND_TABLE_CONFIRM_DELETE_CLICKED, GetId(), this);
            event.SetPath(path);
            event.SetEventObject(this);

            if (!GetDataSource() || !GetDataSource()->ProcessEvent(event))
                GetEventHandler()->ProcessEvent(event);
        }
#endif
        else if (hit == HitTestRow && (!IsEditing() || GetAllowSelectionDuringEditing()))
        {
            if (path != m_selection && m_selection.IsValid())
            {
                wxMoTableCell* cell = FindCellForPath(m_selection);
                if (cell)
                    cell->SetSelected(false);
            }

            SetSelection(path);
            Update();

            wxTableCtrlEvent event(wxEVT_COMMAND_TABLE_ROW_SELECTED, GetId(), this);
            event.SetPath(path);
            event.SetEventObject(this);

            if (!GetDataSource() || !GetDataSource()->ProcessEvent(event))
                GetEventHandler()->ProcessEvent(event);
        }
        else if (hit == HitTestIndex)
        {
            ScrollToSection(section);
        }
    }
}

void wxMoTableCtrl::OnMouseCaptureChanged(wxMouseCaptureChangedEvent& WXUNUSED(event))
{
}

void wxMoTableCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // Reposition all content windows
    PositionAllRowControls();
}

void wxMoTableCtrl::OnTouchScrollDrag(wxTouchScrollEvent& event)
{
    if (m_deletePath.IsValid())
        HideDeleteButton();

    event.Skip();
}

// Handle touch cancelling from the scroll window
void wxMoTableCtrl::OnCancelTouch(wxTouchScrollEvent& event)
{
    //if (GetCapture() == this)
    //    ReleaseCapture();

    event.Allow();
    if (GetSelection())
        Deselect(* GetSelection());
}

// Repositions all row controls
void wxMoTableCtrl::PositionAllRowControls()
{
    // Reposition all content windows
    int sectionIdx;
    int sectionCount = m_sections.GetCount();
    for (sectionIdx = 0; sectionIdx < sectionCount; sectionIdx ++)
    {
        const wxMoTableSection& section = m_sections[sectionIdx];

        int rowCount = section.m_rows.GetCount();
        int rowIdx;
        for (rowIdx = 0; rowIdx < rowCount; rowIdx ++)
        {
            wxTablePath path(sectionIdx, rowIdx);

            const wxMoTableRow& row = section.m_rows[rowIdx];
            if (row.m_tableCell && row.m_tableCell->GetContentWindow())
                PositionRowControls(path);
        }
    }
}

void wxMoTableCtrl::OnInternalIdle()
{
    wxMoScrolledWindow::OnInternalIdle();
}

wxMoTableCell* wxMoTableCtrl::GetReusableCell(const wxString& reuseName)
{
    size_t i;
    for (i = 0; i < m_reusableCells.GetCount(); i++)
    {
        if (m_reusableCells[i]->GetReuseName() == reuseName)
        {
            m_reusableCells[i]->PrepareForReuse(this);
            return m_reusableCells[i];
        }
    }

    int startX, startY;
    GetViewStart(& startX, & startY);
    wxSize sz = GetClientSize();

    int sectionIdx;
    for (sectionIdx = 0; sectionIdx < (int) m_sections.GetCount(); sectionIdx++)
    {
        const wxMoTableSection& section = m_sections[sectionIdx];
        int rowCount = section.m_rows.GetCount();
        int rowIdx;
        for (rowIdx = 0; rowIdx < rowCount; rowIdx ++)
        {
            wxMoTableRow& row = section.m_rows[rowIdx];
            wxMoTableCell* cell = row.m_tableCell;
            if (cell && cell->GetReuseName() == reuseName)
            {
                int y = row.m_rowY - startY;
                if ((((y + row.m_rowHeight) >= 0) && (((y + row.m_rowHeight) <= sz.y))) ||
                    (((y  >= 0) && ((y <= sz.y)))))
                {
                    // Visible - can't reuse
                }
                else
                {
                    m_reusableCells.Add(cell);
                    cell->GetContentWindow()->Show(false);
                    row.m_tableCell = NULL;
                    cell->PrepareForReuse(this);
                    return cell;
                }
            }
        }
    }
    return NULL;
}

// Finds the path for the cell
bool wxMoTableCtrl::FindPathForCell(wxMoTableCell* cell, wxTablePath& path) const
{
    int sectionIdx;
    int sectionCount = m_sections.GetCount();
    for (sectionIdx = 0; sectionIdx < sectionCount; sectionIdx ++)
    {
        const wxMoTableSection& section = m_sections[sectionIdx];

        int rowCount = section.m_rows.GetCount();
        int rowIdx;
        for (rowIdx = 0; rowIdx < rowCount; rowIdx ++)
        {
            const wxMoTableRow& row = section.m_rows[rowIdx];
            if (row.m_tableCell == cell)
            {
                path.SetRow(rowIdx);
                path.SetSection(sectionIdx);

                return true;
            }
        }
    }
    return false;
}

// Finds the cell for the path.
wxMoTableCell* wxMoTableCtrl::FindCellForPath(const wxTablePath& path) const
{
    int sectionIdx;
    int sectionCount = m_sections.GetCount();
    for (sectionIdx = 0; sectionIdx < sectionCount; sectionIdx ++)
    {
        const wxMoTableSection& section = m_sections[sectionIdx];
        if (sectionIdx == path.GetSection())
        {
            int rowCount = section.m_rows.GetCount();
            int rowIdx;
            for (rowIdx = 0; rowIdx < rowCount; rowIdx ++)
            {
                const wxMoTableRow& row = section.m_rows[rowIdx];
                if (path.GetRow() == rowIdx)
                {
                    return row.m_tableCell;
                }
            }
        }
    }
    return NULL;
}

// Clears all data
void wxMoTableCtrl::Clear()
{
    m_sections.Clear();
    m_indexTitles.Clear();

    size_t i;
    for (i = 0; i < m_reusableCells.GetCount(); i++)
    {
        wxMoTableCell* cell = m_reusableCells[i];
        delete cell;
    }
    m_reusableCells.Clear();
    m_totalTableHeight = 0;

    SetScrollbars(0, 1, 0, 0);
}

bool wxMoTableCtrl::ReloadData(bool resetScrollbars)
{
    int startX, startY;
    GetViewStart(& startX, & startY);
    wxSize sz = GetClientSize();

    Clear();

    int topBottomMargin = 0;
    if (IsGrouped())
        topBottomMargin = m_topBottomMargin;

    if (m_dataSource)
    {
        m_indexTitles = m_dataSource->GetIndexTitles(this);

        int y = topBottomMargin;

        int sectionCount = m_dataSource->GetSectionCount(this);
        wxArrayString sectionTitles = m_dataSource->GetSectionTitles(this);
        wxASSERT((int) sectionTitles.GetCount() == 0 || sectionCount == (int) sectionTitles.GetCount());

        // Titles can be empty, if no section titles
        int sectionIdx;
        for (sectionIdx = 0; sectionIdx < sectionCount; sectionIdx ++)
        {
            wxString sectionTitle;
            if ((int) sectionTitles.GetCount() > sectionIdx)
                sectionTitle = sectionTitles[sectionIdx];

            int sectionHeaderHeight = m_dataSource->GetSectionHeaderHeight(this, sectionIdx);
            if (sectionHeaderHeight == -1)
                sectionHeaderHeight = GetSectionHeaderHeight();

            int sectionFooterHeight = m_dataSource->GetSectionFooterHeight(this, sectionIdx);
            if (sectionFooterHeight == -1)
                sectionFooterHeight = GetSectionFooterHeight();

            if (sectionTitle.IsEmpty())
                sectionHeaderHeight = 0;

            if (sectionIdx > 0)
                y += m_interSectionSpacing;

            y += sectionHeaderHeight;

            wxMoTableSection section;
            section.m_sectionName = sectionTitle;
            section.m_sectionY = y;

            int rowCount = m_dataSource->GetRowCount(this, sectionIdx);
            section.m_rowCount = rowCount;
            int rowIdx;
            for (rowIdx = 0; rowIdx < rowCount; rowIdx ++)
            {
                wxTablePath path(sectionIdx, rowIdx);
                int rowHeight = m_dataSource->GetRowHeight(this, path);
                if (rowHeight <= 0)
                    rowHeight = m_rowHeight;

                wxMoTableRow row;
                row.m_rowY = y;
                row.m_rowHeight = rowHeight;

                section.m_rows.Add(row);

                y += rowHeight;
            }

            section.m_sectionHeight = y - section.m_sectionY;
            section.m_sectionHeaderHeight = sectionHeaderHeight;
            section.m_sectionFooterHeight = sectionFooterHeight;

            y += sectionFooterHeight;

            m_sections.Add(section);
        }

        y += topBottomMargin;

        m_totalTableHeight = y;

        int scrollPos;
        if (resetScrollbars)
            scrollPos = 0;
        else
            scrollPos = wxMax(0, wxMin(startY, m_totalTableHeight - sz.y));

        SetScrollbars(0, 1, 0, m_totalTableHeight, 0, scrollPos);

        if (!IsFrozen())
            Refresh();
    }

    return true;
}

// Loads the data within the specified rectangle.
bool wxMoTableCtrl::LoadVisibleData(const wxRect& rect1)
{
    if (!m_dataSource)
        return false;

    int startX, startY;
    GetViewStart(& startX, & startY);

    wxRect rect(rect1);
    rect.y += startY;

    int i;
    for (i = 0; i < (int) m_sections.GetCount(); i++)
    {
        const wxMoTableSection& section = m_sections[i];
        size_t j;
        for (j = 0; j < section.m_rows.GetCount(); j++)
        {
            wxMoTableRow& row = section.m_rows[j];

            if (rect.Contains(wxPoint(20, row.m_rowY)) || rect.Contains(wxPoint(20, row.m_rowY+row.m_rowHeight-1)))
            {
                if (!row.m_tableCell)
                {
                    wxTablePath path(i, j);
                    row.m_tableCell = m_dataSource->GetCell(this, path);
                    if (row.m_tableCell)
                    {
                        row.m_tableCell->SetSelected(path == m_selection);
                        row.m_tableCell->SetEditingMode(IsEditing(), false);
                    }

                    // If we got it from the pool of reusable cells,
                    // remove it from the pool.
                    int cellIdx = m_reusableCells.Index(row.m_tableCell);
                    if (cellIdx != wxNOT_FOUND)
                        m_reusableCells.RemoveAt(cellIdx);

                    PositionRowControls(path);
                }
            }
        }
    }

    return true;
}

// Positions the controls in the specified row.
bool wxMoTableCtrl::PositionRowControls(const wxTablePath& path )
{
    int startX, startY;
    GetViewStart(& startX, & startY);

    wxSize sz(GetClientSize());
    int leftMargin = m_leftRightMargin;
    int rightMargin = m_leftRightMargin;

    // Allow for edit controls
    if (IsEditing())
        leftMargin = leftMargin + MO_EDIT_CONTROL_MARGIN;

    if (path.GetSection() >= (int) m_sections.GetCount())
        return false;

    const wxMoTableSection& section = m_sections[path.GetSection()];
    if (path.GetRow() >= (int) section.m_rows.GetCount())
        return false;

    const wxMoTableRow& row = section.m_rows[path.GetRow()];

    if (!row.m_tableCell)
        return false;

    int rowMarginTopBottom = 4;
    int rowMarginLeftRight = 4;
    int accessoryWidth = 0;
    int accessoryMargin = 0;
    int reorderingControlWidth = 0;

    // Make room for either an accessory or the index to be drawn
    if ((!IsEditing() && row.m_tableCell->HasAccessory()) ||
         (IsEditing() && row.m_tableCell->HasEditingAccessory()) ||
          HasIndex())
    {
        accessoryWidth = m_accessoryWidth;
        accessoryMargin = rowMarginLeftRight;
    }

    if (row.m_tableCell->GetAccessoryWindow() && !IsEditing())
    {
        accessoryWidth = row.m_tableCell->GetAccessoryWindow()->GetSize().x;
        accessoryMargin = rowMarginLeftRight;
    }

    if (row.m_tableCell->GetEditingAccessoryWindow() && IsEditing())
    {
        accessoryWidth = row.m_tableCell->GetEditingAccessoryWindow()->GetSize().x;
        accessoryMargin = rowMarginLeftRight;
    }

    if (DeletingThisRow(path) && m_deleteButton)
        accessoryWidth = m_deleteButton->GetSize().x + 8;

    if (IsEditing() && row.m_tableCell->GetShowReorderingControl())
        reorderingControlWidth = m_accessoryWidth;

    wxRect cellContentRect(leftMargin+rowMarginLeftRight,
        row.m_rowY + rowMarginTopBottom - startY,
        sz.x - leftMargin - rightMargin - 2*rowMarginLeftRight - accessoryWidth - accessoryMargin - reorderingControlWidth,
        row.m_rowHeight-2*rowMarginTopBottom);

    row.m_tableCell->GetContentWindow()->Show(true);
    row.m_tableCell->GetContentWindow()->SetSize(cellContentRect, wxSIZE_ALLOW_MINUS_ONE);

    if (row.m_tableCell->GetAccessoryWindow())
    {
        if (!IsEditing() && !DeletingThisRow(path))
        {
            wxSize accessorySize = row.m_tableCell->GetAccessoryWindow()->GetSize();
            wxPoint accessoryPos = wxPoint(sz.x - rightMargin - rowMarginLeftRight - accessoryWidth - accessoryMargin - reorderingControlWidth,
                row.m_rowY + rowMarginTopBottom - startY + (row.m_rowHeight - accessorySize.y)/2);
            row.m_tableCell->GetAccessoryWindow()->SetSize(wxRect(accessoryPos, accessorySize), wxSIZE_ALLOW_MINUS_ONE);
            row.m_tableCell->GetAccessoryWindow()->Show(true);
        }
        else
            row.m_tableCell->GetAccessoryWindow()->Show(false);
    }

    if (row.m_tableCell->GetEditingAccessoryWindow() && !DeletingThisRow(path))
    {
        if (IsEditing())
        {
            wxSize accessorySize = row.m_tableCell->GetEditingAccessoryWindow()->GetSize();
            wxPoint accessoryPos = wxPoint(sz.x - rightMargin - rowMarginLeftRight - accessoryWidth - accessoryMargin - reorderingControlWidth,
                row.m_rowY + rowMarginTopBottom - startY + (row.m_rowHeight - accessorySize.y)/2);
            row.m_tableCell->GetEditingAccessoryWindow()->SetSize(wxRect(accessoryPos, accessorySize), wxSIZE_ALLOW_MINUS_ONE);
            row.m_tableCell->GetEditingAccessoryWindow()->Show(true);
        }
        else
            row.m_tableCell->GetEditingAccessoryWindow()->Show(false);
    }

    if (DeletingThisRow(path) && m_deleteButton)
    {
        wxSize buttonSize = m_deleteButton->GetSize();
        buttonSize.y = row.m_rowHeight - 8;
        wxPoint buttonPos = wxPoint(sz.x - rightMargin - buttonSize.x - accessoryMargin - reorderingControlWidth,
            row.m_rowY - startY + (row.m_rowHeight - buttonSize.y)/2);
        m_deleteButton->SetSize(wxRect(buttonPos, buttonSize), wxSIZE_ALLOW_MINUS_ONE);
    }

    return true;
}

void wxMoTableCtrl::SetSelection(const wxTablePath& path)
{
    m_selection = path;
    if (m_selection.IsValid())
    {
        wxMoTableCell* cell = FindCellForPath(m_selection);
        if (cell)
            cell->SetSelected(true);
    }
    if (!IsFrozen())
        Refresh();
}

void wxMoTableCtrl::Deselect(const wxTablePath& path)
{
    if (m_selection == path)
    {
        wxTablePath oldPath = path;
        m_selection.SetSection(-1);
        m_selection.SetRow(-1);

        wxMoTableCell* cell = FindCellForPath(oldPath);
        if (cell)
            cell->SetSelected(false);

        if (!IsFrozen())
            Refresh();
    }
}

wxTablePath* wxMoTableCtrl::GetSelection() const
{
    if (m_selection.GetSection() == -1)
        return NULL;
    else
        return (wxTablePath*) & m_selection;
}

// Scroll to the given section
bool wxMoTableCtrl::ScrollToSection(int section)
{
    wxASSERT(section >= 0 && section < (int) m_sections.GetCount());
    if (section >= 0 && section < (int) m_sections.GetCount())
    {
        const wxMoTableSection& s = m_sections[(size_t) section];
        int yPos = s.m_sectionY;

        yPos -= s.m_sectionHeaderHeight;

        // Units == pixels in this case
        Scroll(0, yPos);
        return true;
    }
    else
        return false;
}

// Scroll to the given path (section/row)
bool wxMoTableCtrl::ScrollToPath(const wxTablePath& path)
{
    int section = path.GetSection();
    int row = path.GetRow();

    wxASSERT(section >= 0 && section < (int) m_sections.GetCount());
    if (section >= 0 && section < (int) m_sections.GetCount())
    {
        const wxMoTableSection& s = m_sections[(size_t) section];
        if (row < (int) s.m_rows.GetCount())
        {
            int yPos = s.m_rows[(size_t) row].m_rowY;

            // Units == pixels in this case
            Scroll(0, yPos);

            return true;
        }
        else
            return false;
    }
    else
        return false;
    return false;
}

// Deletes the given rows, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::DeleteRows(const wxTablePathArray& WXUNUSED(paths), RowAnimationStyle WXUNUSED(animationStyle))
{
    // No animation in the simulated control; we'll just reload the data
    // if necessary.
    if (!IsFrozen())
        ReloadData(false);
    return true;
}

// Inserts the given rows, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::InsertRows(const wxTablePathArray& WXUNUSED(paths), RowAnimationStyle WXUNUSED(animationStyle))
{
    // No animation in the simulated control; we'll just reload the data
    // if necessary.
    if (!IsFrozen())
        ReloadData(false);
    return true;
}

// Deletes the given sections, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::DeleteSections(const wxArrayInt& WXUNUSED(sections), RowAnimationStyle WXUNUSED(animationStyle))
{
    // No animation in the simulated control; we'll just reload the data
    // if necessary.
    if (!IsFrozen())
        ReloadData(false);
    return true;
}

// Inserts the given sections, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::InsertSections(const wxArrayInt& WXUNUSED(sections), RowAnimationStyle WXUNUSED(animationStyle))
{
    // No animation in the simulated control; we'll just reload the data
    // if necessary.
    if (!IsFrozen())
        ReloadData(false);
    return true;
}

// Refreshes the given sections, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::ReloadSections(const wxArrayInt& WXUNUSED(sections), RowAnimationStyle WXUNUSED(animationStyle))
{
    // No animation in the simulated control; we'll just reload the data
    // if necessary.
    if (!IsFrozen())
        ReloadData(false);
    return true;
}

// Inserts the given rows, by getting the new data. In the generic implementation,
// the data is completely refreshed, but on Cocoa Touch
// this will be optimized and animated.
bool wxMoTableCtrl::ReloadRows(const wxTablePathArray& WXUNUSED(paths), RowAnimationStyle WXUNUSED(animationStyle))
{
    // No animation in the simulated control; we'll just reload the data
    // if necessary.
    if (!IsFrozen())
        ReloadData(false);
    return true;
}

// Sets editing mode (not yet implemented).
bool wxMoTableCtrl::SetEditingMode(bool editingMode, bool WXUNUSED(animated))
{
    m_editingMode = editingMode;

    int sectionIdx;
    int sectionCount = m_sections.GetCount();
    for (sectionIdx = 0; sectionIdx < sectionCount; sectionIdx ++)
    {
        const wxMoTableSection& section = m_sections[sectionIdx];

        int rowCount = section.m_rows.GetCount();
        int rowIdx;
        for (rowIdx = 0; rowIdx < rowCount; rowIdx ++)
        {
            const wxMoTableRow& row = section.m_rows[rowIdx];
            if (row.m_tableCell)
            {
                row.m_tableCell->SetEditingMode(editingMode);
            }
        }
    }

    ReloadData(false);

    return true;
}

void wxMoTableCtrl::DoShowDeleteButton(bool show, const wxTablePath& path)
{
    if (show)
    {
        if (m_deletePath.IsValid())
            HideDeleteButton();

        m_deletePath = path;
        if (!m_deleteButton)
        {
            m_deleteButton = new wxMoButton(this, wxID_ANY, _("Delete"));
            m_deleteButton->SetBackgroundColour(*wxRED);
            m_deleteButton->Connect(m_deleteButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(wxMoTableCtrl::OnConfirmDeleteButtonClicked),
                NULL, this);
        }
        m_deleteButton->Show(true);
        PositionRowControls(m_deletePath);
        Refresh();
    }
    else
    {
        m_deleteButton->Show(false);
        wxTablePath oldPath = m_deletePath;
        m_deletePath = path;
        if (oldPath.IsValid())
            PositionRowControls(oldPath);
        Refresh();
    }
}

void wxMoTableCtrl::OnAddClicked(wxTableCtrlEvent& event)
{
    if (event.GetEventObject() != this)
    {
        event.Skip();
        return;
    }

    if (GetDataSource())
        GetDataSource()->CommitInsertRow(this, event.GetPath());
}

void wxMoTableCtrl::OnDeleteClicked(wxTableCtrlEvent& event)
{
    if (event.GetEventObject() != this)
    {
        event.Skip();
        return;
    }

    if (GetDataSource())
    {
        // Create a Delete button for confirmation, and redisplay the row
        ShowDeleteButton(event.GetPath());
    }
}

void wxMoTableCtrl::OnConfirmDeleteClicked(wxTableCtrlEvent& event)
{
    if (event.GetEventObject() != this)
    {
        event.Skip();
        return;
    }

    HideDeleteButton();

    if (GetDataSource())
        GetDataSource()->CommitDeleteRow(this, event.GetPath());
}

void wxMoTableCtrl::OnConfirmDeleteButtonClicked(wxCommandEvent& WXUNUSED(cmdEvent))
{
    wxTableCtrlEvent event(wxEVT_COMMAND_TABLE_CONFIRM_DELETE_CLICKED, GetId(), this);
    event.SetPath(m_deletePath);
    event.SetEventObject(this);

    if (!GetDataSource() || !GetDataSource()->ProcessEvent(event))
        GetEventHandler()->ProcessEvent(event);
}


void wxMoTableRow::Init()
{
    m_tableCell = NULL;
    m_rowY = 0;
    m_rowHeight = 0;
}

wxMoTableRow::~wxMoTableRow()
{
    if (m_tableCell)
    {
        delete m_tableCell;
        m_tableCell = NULL;
    }
}

void wxMoTableRow::Copy(const wxMoTableRow& row)
{
    m_tableCell = row.m_tableCell;
    const_cast<wxMoTableRow&>(row).m_tableCell = NULL;
    m_rowY = row.m_rowY;
    m_rowHeight = row.m_rowHeight;
}

void wxMoTableSection::Init()
{
    m_sectionWidth = 0;
    m_sectionHeight = 0;
    m_sectionY = 0;
    m_sectionHeaderHeight = 0;
    m_sectionFooterHeight = 0;
    m_rowCount = 0;
}

void wxMoTableSection::Copy(const wxMoTableSection& section)
{
    m_sectionName = section.m_sectionName;
    m_sectionWidth = section.m_sectionWidth;
    m_sectionHeight = section.m_sectionHeight;
    m_sectionY = section.m_sectionY;
    m_rows = section.m_rows;
    m_sectionHeaderHeight = section.m_sectionHeaderHeight;
    m_sectionFooterHeight = section.m_sectionFooterHeight;
    m_rowCount = section.m_rowCount;
}

// Called by the table control to commit an insertion requested by the user. This function should
// then call InsertRows in response.
bool wxMoTableDataSource::CommitInsertRow(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    wxTablePathArray paths;
    paths.Add(path);
    return ctrl->InsertRows(paths, wxMoTableCtrl::RowAnimationNone);
}

// Called by the table control to commit a deletion requested by the user. This function should
// then call DeleteRows in response.
bool wxMoTableDataSource::CommitDeleteRow(wxMoTableCtrl* ctrl, const wxTablePath& path)
{
    wxTablePathArray paths;
    paths.Add(path);
    return ctrl->DeleteRows(paths, wxMoTableCtrl::RowAnimationNone);
}
