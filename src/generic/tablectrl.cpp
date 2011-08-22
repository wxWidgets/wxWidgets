/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/tablectrl.cpp
// Purpose:     Common wxTableCtrl parts
// Author:      Julian Smart, Linas Valiukas
// Created:     2011-08-19
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/dc.h"
#include "wx/sizer.h"
#endif

#include "wx/tablectrl.h"
#include "wx/arrimpl.cpp"


IMPLEMENT_DYNAMIC_CLASS(wxTableCtrl, wxTableCtrlBase)
IMPLEMENT_DYNAMIC_CLASS(wxTablePath, wxObject)
IMPLEMENT_CLASS(wxTableDataSource, wxEvtHandler)
IMPLEMENT_CLASS(wxTableRow, wxObject)
IMPLEMENT_CLASS(wxTableSection, wxObject)

WX_DEFINE_EXPORTED_OBJARRAY(wxTableSectionArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxTableRowArray);
WX_DEFINE_EXPORTED_OBJARRAY(wxTablePathArray);

IMPLEMENT_DYNAMIC_CLASS(wxTableCtrlEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ROW_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ACCESSORY_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_ADD_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_DELETE_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_CONFIRM_DELETE_CLICKED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TABLE_MOVE_DRAGGED)


void wxTableCtrlBase::Init()
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
    m_separatorStyle = CellSeparatorStyleSingleLine;
    m_freezeCount = 0;
    m_editingMode = false;
    m_deleteButton = NULL;
    m_deletePath = wxTablePath(-1, -1);
    m_insertAt = wxTablePath(-1, -1);
    m_originalInsertionPoint = wxTablePath(-1, -1);
    m_allowsSelectionDuringEditing = true;
    m_allowsSelectionDuringEditing = false;
}

// Clears all data
void wxTableCtrl::Clear()
{
    m_sections.Clear();
    m_indexTitles.Clear();
    
    size_t i;
    for (i = 0; i < m_reusableCells.GetCount(); i++) {
        wxTableCell* cell = m_reusableCells[i];
        delete cell;
    }
    m_reusableCells.Clear();
    m_totalTableHeight = 0;
}

/// Finds the path for the cell.
bool wxTableCtrl::FindPathForCell(wxTableCell* cell, wxTablePath& path) const
{
    int sectionIdx;
    int sectionCount = m_sections.GetCount();
    for (sectionIdx = 0; sectionIdx < sectionCount; sectionIdx ++) {
        const wxTableSection& section = m_sections[sectionIdx];
        
        wxTableRowArray rows = section.GetRows();
        int rowCount = rows.GetCount();
        int rowIdx;
        for (rowIdx = 0; rowIdx < rowCount; rowIdx ++) {
            const wxTableRow& row = rows[rowIdx];
            const wxTableCell* rowTableCell = row.GetTableCell();
            if (rowTableCell == cell) {
                path.SetRow(rowIdx);
                path.SetSection(sectionIdx);
                
                return true;
            }
        }
    }
    
    return false;
}

// Finds the cell for the path.
wxTableCell* wxTableCtrl::FindCellForPath(const wxTablePath& path) const
{
    int sectionIdx;
    int sectionCount = m_sections.GetCount();
    for (sectionIdx = 0; sectionIdx < sectionCount; sectionIdx ++) {
        const wxTableSection& section = m_sections[sectionIdx];
        if (sectionIdx == path.GetSection()) {
            
            wxTableRowArray rows = section.GetRows();
            int rowCount = rows.GetCount();
            int rowIdx;
            for (rowIdx = 0; rowIdx < rowCount; rowIdx ++) {
                const wxTableRow& row = rows[rowIdx];
                if (path.GetRow() == rowIdx) {
                    return row.GetTableCell();
                }
            }
        }
    }
    
    return NULL;
}

/// Starts batching of operations within which no visual updates are performed.
/// You can nest Freeze/Thaw operations.
void wxTableCtrl::Freeze()
{
    wxWindow::Freeze();
    ++m_freezeCount;
}

/// Ends batching of operations, refetches the data, and refreshes the window.
void wxTableCtrl::Thaw()
{
    --m_freezeCount;
    if (m_freezeCount < 0) {
        m_freezeCount = 0;
    }
    if (m_freezeCount == 0) {
        ReloadData(false /* don't reset scrollbars */);
    }
    
    wxWindow::Thaw();
}
