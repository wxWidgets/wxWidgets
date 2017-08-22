///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/dataview.h
// Purpose:     Persistence support for wxDataViewListCtrl
// Author:      wxWidgets Team
// Created:     2017-08-21
// Copyright:   (c) 2017 wxWidgets.org
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PERSIST_DATAVIEW_H_
#define _WX_PERSIST_DATAVIEW_H_

#include "wx/persist/window.h"
#include "wx/dataview.h"

// ----------------------------------------------------------------------------
// String constants used by wxPersistentDataViewListCtrl.
// ----------------------------------------------------------------------------

#define wxPERSIST_DVLC_KIND "DataViewList"

#define wxPERSIST_DVLC_COL_PREFIX "Column"

#define wxPERSIST_DVLC_WIDTH "Width"
#define wxPERSIST_DVLC_SORT_KEY "SortKey"
#define wxPERSIST_DVLC_SORT_ASC "SortAsc"

// ----------------------------------------------------------------------------
// wxPersistentDataViewListCtrl: Saves and restores user modified column widths
// and single column sort order. 
//
// Future improvements could be to save and restore column order if the user
// has changed it and multicolumn sorts.
// ----------------------------------------------------------------------------

class wxPersistentDataViewListCtrl : 
                                public wxPersistentWindow<wxDataViewListCtrl>
{
public:
    wxPersistentDataViewListCtrl(wxDataViewListCtrl* control)
        : wxPersistentWindow<wxDataViewListCtrl>(control)
    {
    }

    virtual void Save() const wxOVERRIDE
    {
        wxDataViewListCtrl* const control = Get();
        
        int sortColumn = -1;
        
        for ( unsigned int col = 0; col < control->GetColumnCount(); col++ )
        {
            // Create a prefix string to identify each column.
            wxString columnPrefix;
            columnPrefix.Printf("%s%d", wxPERSIST_DVLC_COL_PREFIX, col);

            // Save the width of each column.
            int width = control->GetColumn(col)->GetWidth();
            SaveValue(columnPrefix + wxPERSIST_DVLC_WIDTH, width);
            
            // Check if this column is the current sort key.
            if ( control->GetColumn(col)->IsSortKey() )
                sortColumn = col;
        }

        // Note: The current implementation does not save and restore multi-
        // column sort keys.
        if (control->IsMultiColumnSortAllowed())
            return;

        // Save the current sort key.
        SaveValue(wxPERSIST_DVLC_SORT_KEY, sortColumn);
        
        // Save the sort direction.
        if ( sortColumn > -1 )
        {
            bool sortAsc = 
                        control->GetColumn(sortColumn)->IsSortOrderAscending();
            SaveValue(wxPERSIST_DVLC_SORT_ASC, sortAsc);
        }
    }

    virtual bool Restore() wxOVERRIDE
    {
        wxDataViewListCtrl* const control = Get();

        for ( unsigned int col = 0; col < control->GetColumnCount(); col++ )
        {
            // Create a prefix string to identify each column.
            wxString columnPrefix = 
                    wxString::Format("%s%d", wxPERSIST_DVLC_COL_PREFIX, col);
                    
            // Restore the column width.
            int width;
            if ( RestoreValue(columnPrefix + wxPERSIST_DVLC_WIDTH, &width) )
                control->GetColumn(col)->SetWidth(width);
        }
        
        // Restore the sort key and order.
        int sortColumn;
        if ( RestoreValue(wxPERSIST_DVLC_SORT_KEY, &sortColumn) && 
                    sortColumn > -1 &&
                    sortColumn < (int)control->GetColumnCount() )
        {
            bool sortAsc = true;
            
            RestoreValue(wxPERSIST_DVLC_SORT_ASC, &sortAsc);
            control->GetColumn(sortColumn)->SetSortOrder(sortAsc);
            control->GetModel()->Resort();
        }
        return true;
    }

    virtual wxString GetKind() const wxOVERRIDE
    { 
        return wxPERSIST_DVLC_KIND; 
    }
};

inline wxPersistentObject *wxCreatePersistentObject(wxDataViewListCtrl* control)
{
    return new wxPersistentDataViewListCtrl(control);
}

#endif // _WX_PERSIST_DATAVIEW_H_
