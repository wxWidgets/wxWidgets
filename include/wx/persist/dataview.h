///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/dataview.h
// Purpose:     Persistence support for wxDataViewCtrl and its derivatives
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
// String constants used by wxPersistentDataViewCtrl.
// ----------------------------------------------------------------------------

#define wxPERSIST_DVC_KIND "DataView"

#define wxPERSIST_DVC_HIDDEN "Hidden"
#define wxPERSIST_DVC_POS "Position"
#define wxPERSIST_DVC_TITLE "Title"
#define wxPERSIST_DVC_WIDTH "Width"

#define wxPERSIST_DVC_SORT_KEY "Sorting/Column"
#define wxPERSIST_DVC_SORT_ASC "Sorting/Asc"

// ----------------------------------------------------------------------------
// wxPersistentDataViewCtrl: Saves and restores user modified column widths
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

        const wxDataViewColumn* sortColumn = NULL;

        for ( unsigned int col = 0; col < control->GetColumnCount(); col++ )
        {
            const wxDataViewColumn* const column = control->GetColumn(col);

            // Create a prefix string to identify each column.
            const wxString columnPrefix = MakeColumnPrefix(column);

            // Save the column attributes.
            SaveValue(columnPrefix + wxPERSIST_DVC_HIDDEN, column->IsHidden());
            SaveValue(columnPrefix + wxPERSIST_DVC_POS,
                      control->GetColumnPosition(column));
            SaveValue(columnPrefix + wxPERSIST_DVC_WIDTH, column->GetWidth());

            // Check if this column is the current sort key.
            if ( column->IsSortKey() )
                sortColumn = column;
        }

        // Note: The current implementation does not save and restore multi-
        // column sort keys.
        if ( control->IsMultiColumnSortAllowed() )
            return;

        // Save the sort key and direction if there is a valid sort.
        if ( sortColumn )
        {
            SaveValue(wxPERSIST_DVC_SORT_KEY, sortColumn->GetTitle());
            SaveValue(wxPERSIST_DVC_SORT_ASC,
                      sortColumn->IsSortOrderAscending());
        }
    }

    virtual bool Restore() wxOVERRIDE
    {
        wxDataViewCtrl* const control = Get();

        for ( unsigned int col = 0; col < control->GetColumnCount(); col++ )
        {
            wxDataViewColumn* const column = control->GetColumn(col);

            // Create a prefix string to identify each column within the
            // persistence store (columns are stored by title). The persistence
            // store benignly handles cases where the title is not found.
            const wxString columnPrefix = MakeColumnPrefix(column);

            // Restore column hidden status.
            bool hidden;
            if ( RestoreValue(columnPrefix + wxPERSIST_DVC_HIDDEN, &hidden) )
                column->SetHidden(hidden);

            // Restore the column width.
            int width;
            if ( RestoreValue(columnPrefix + wxPERSIST_DVC_WIDTH, &width) )
                column->SetWidth(width);

            // TODO: Set the column's view position.
        }

        // Restore the sort key and order if there is a valid model and sort
        // criteria.
        wxString sortColumn;
        if ( control->GetModel() &&
             RestoreValue(wxPERSIST_DVC_SORT_KEY, &sortColumn) &&
             !sortColumn.empty() )
        {
            bool sortAsc = true;
            if ( wxDataViewColumn* column = GetColumnByTitle(control, sortColumn) )
            {
                RestoreValue(wxPERSIST_DVC_SORT_ASC, &sortAsc);
                column->SetSortOrder(sortAsc);

                // Resort the control based on the new sort criteria.
                control->GetModel()->Resort();
            }
        }

        return true;
    }

    virtual wxString GetKind() const wxOVERRIDE
    {
        return wxPERSIST_DVC_KIND;
    }

private:
    // Return a (slash-terminated) prefix for the column-specific entries.
    static wxString MakeColumnPrefix(const wxDataViewColumn* column)
    {
        return wxString::Format("/Columns/%s/", column->GetTitle());
    }

    // Return the column with the given title or NULL.
    static wxDataViewColumn*
    GetColumnByTitle(wxDataViewCtrl* control, const wxString& title)
    {
        for ( unsigned int col = 0; col < control->GetColumnCount(); col++ )
        {
            if ( control->GetColumn(col)->GetTitle() == title )
                return control->GetColumn(col);
        }

        return NULL;
    }
};

inline wxPersistentObject *wxCreatePersistentObject(wxDataViewCtrl* control)
{
    return new wxPersistentDataViewCtrl(control);
}

#endif // _WX_PERSIST_DATAVIEW_H_
