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

#define wxPERSIST_DVLC_COLUMNS "Columns"

#define wxPERSIST_DVLC_HIDDEN "Hidden"
#define wxPERSIST_DVLC_POS "Position"
#define wxPERSIST_DVLC_TITLE "Title"
#define wxPERSIST_DVLC_WIDTH "Width"

#define wxPERSIST_DVLC_SORT_KEY "Sorting/Column"
#define wxPERSIST_DVLC_SORT_ASC "Sorting/Asc"

// ----------------------------------------------------------------------------
// Helper function to search for a column by its title.
// ----------------------------------------------------------------------------
wxDataViewColumn* GetColumnByTitle(wxDataViewCtrl* control, 
                                    const wxString& name)
{
    wxASSERT(control);
    
    for ( unsigned int col = 0; col < control->GetColumnCount(); col++)
    {
        if ( control->GetColumn(col)->GetTitle() == name)
            return control->GetColumn(col);
    }
    
    return NULL;
}

// ----------------------------------------------------------------------------
// wxPersistentDataViewListCtrl: Saves and restores user modified column widths
// and single column sort order. 
//
// Future improvements could be to save and restore column order if the user
// has changed it and multicolumn sorts.
// ----------------------------------------------------------------------------

class wxPersistentDataViewCtrl : public wxPersistentWindow<wxDataViewCtrl>
{
public:
    wxPersistentDataViewCtrl(wxDataViewCtrl* control)
        : wxPersistentWindow<wxDataViewCtrl>(control)
    {
    }

    virtual void Save() const wxOVERRIDE
    {
        wxDataViewCtrl* const control = Get();
        
        wxDataViewColumn* sortColumn = NULL;
        
        for ( unsigned int col = 0; col < control->GetColumnCount(); col++ )
        {
            wxDataViewColumn* column = control->GetColumn(col);
            wxASSERT(column);

            // Create a prefix string to identify each column.
            wxString columnPrefix;
            columnPrefix.Printf("/%s/%s/", wxPERSIST_DVLC_COLUMNS, 
                                                        column->GetTitle());

            // Save the column attributes.
            SaveValue(columnPrefix + wxPERSIST_DVLC_HIDDEN, column->IsHidden());
            SaveValue(columnPrefix + wxPERSIST_DVLC_POS, 
                                            control->GetColumnPosition(column));
            SaveValue(columnPrefix + wxPERSIST_DVLC_WIDTH, column->GetWidth());

            // Check if this column is the current sort key.
            if ( column->IsSortKey() )
                sortColumn = column;
        }

        // Note: The current implementation does not save and restore multi-
        // column sort keys.
        if (control->IsMultiColumnSortAllowed())
            return;

        // Save the sort key and direction if there is a valid sort.
        if ( sortColumn )
        {
            SaveValue(wxPERSIST_DVLC_SORT_KEY, sortColumn->GetTitle());
            SaveValue(wxPERSIST_DVLC_SORT_ASC, 
                                    sortColumn->IsSortOrderAscending());
        }
    }

    virtual bool Restore() wxOVERRIDE
    {
        wxDataViewCtrl* const control = Get();
        wxDataViewColumn* column;

        for ( unsigned int col = 0; col < control->GetColumnCount(); col++ )
        {
            column = control->GetColumn(col);
            wxASSERT(column);

            // Create a prefix string to identify each column within the
            // persistence store (columns are stored by title). The persistence
            // store benignly handles cases where the title is not found.
            wxString columnPrefix;
            columnPrefix.Printf("/%s/%s/", wxPERSIST_DVLC_COLUMNS, 
                                                        column->GetTitle());

            // Restore column hidden status.
            bool hidden;
            if ( RestoreValue(columnPrefix + wxPERSIST_DVLC_HIDDEN, &hidden) )
                column->SetHidden(hidden);

            // Restore the column width.
            int width;
            if ( RestoreValue(columnPrefix + wxPERSIST_DVLC_WIDTH, &width) )
                column->SetWidth(width);
                
            // TODO: Set the column's view position.
        }
        
        // Restore the sort key and order.
        wxString sortColumn;
        if ( RestoreValue(wxPERSIST_DVLC_SORT_KEY, &sortColumn) && 
                    sortColumn != "" )
        {
            bool sortAsc = true;
            column = GetColumnByTitle(control, sortColumn);
            
            if ( column )
            {
                RestoreValue(wxPERSIST_DVLC_SORT_ASC, &sortAsc);
                column->SetSortOrder(sortAsc);
                control->GetModel()->Resort();
            }
        }
        return true;
    }

    virtual wxString GetKind() const wxOVERRIDE
    { 
        return wxPERSIST_DVLC_KIND; 
    }
};

inline wxPersistentObject *wxCreatePersistentObject(wxDataViewCtrl* control)
{
    return new wxPersistentDataViewCtrl(control);
}

#endif // _WX_PERSIST_DATAVIEW_H_
