/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/tablectrl.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TABLECTRL_H_
#define _WX_TABLECTRL_H_

#include "wx/dynarray.h"

#include "wx/tablecell.h"
#include "wx/button.h"


/**
    @class wxTableCtrl

    @category{wxbile}
*/

class WXDLLEXPORT wxTableCtrl: public wxTableCtrlBase
{
public:

    /// Default constructor.
    wxTableCtrl() { Init(); };

    /// Constructor.
    wxTableCtrl(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxButtonNameStr);
                
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxButtonNameStr);

    virtual ~wxTableCtrl();

    /// Reloads the data. Resets the scroll position to the start
    /// if resetScrollbars is true, otherwise resets it to the nearest
    /// position to the previous one.
    virtual bool ReloadData(bool resetScrollbars = true);

    /// Loads the data within the specified rectangle.
    virtual bool LoadVisibleData(const wxRect& rect);

    /// Clears all data
    virtual void Clear();

    /// Starts batching of operations within which no visual updates are performed.
    /// You can nest Freeze/Thaw operations.
    virtual void Freeze();

    /// Ends batching of operations, refetches the data, and refreshes the window.
    virtual void Thaw();

    /// Scroll to the given section
    virtual bool ScrollToSection(int section) { ScrollToPath(wxTablePath(section, 0)); }

    /// Scroll to the given path (section/row)
    virtual bool ScrollToPath(const wxTablePath& path);

    /// Deletes the given rows, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool DeleteRows(const wxTablePathArray& paths, RowAnimationStyle animationStyle);

    /// Inserts the given rows, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool InsertRows(const wxTablePathArray& paths, RowAnimationStyle animationStyle);

    /// Deletes the given sections, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool DeleteSections(const wxArrayInt& sections, RowAnimationStyle animationStyle);

    /// Inserts the given sections, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool InsertSections(const wxArrayInt& sections, RowAnimationStyle animationStyle);

    /// Refreshes the given sections, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool ReloadSections(const wxArrayInt& sections, RowAnimationStyle animationStyle);

    /// Inserts the given rows, by getting the new data. In the generic implementation,
    /// the data is completely refreshed, but on Cocoa Touch
    /// this will be optimized and animated.
    virtual bool ReloadRows(const wxTablePathArray& paths, RowAnimationStyle animationStyle);

    /// Sets editing mode (not yet implemented).
    virtual bool SetEditingMode(bool editingMode, bool animated = true);

    // Finds a cell that isn't being displayed currently,
    // either from the current pool or from an area of
    // the table that isn't currently showing.
    wxTableCell* GetReusableCell(const wxString& reuseName);

    /// Finds the path for the cell.
    bool FindPathForCell(wxTableCell* cell, wxTablePath& path) const;

    /// Finds the cell for the path.
    wxTableCell* FindCellForPath(const wxTablePath& path) const;

    /// Sets the selection to the given path.
    void SetSelection(const wxTablePath& path);

    /// Removes the selection at the given path.
    void Deselect(const wxTablePath& path);
        
    /// Get the edit style for a row
    virtual wxTableCtrlBase::CellEditStyle GetCellEditStyle(wxTableCtrl* WXUNUSED(ctrl),
                                                            const wxTablePath& WXUNUSED(path))
    {
        return wxTableCtrl::EditStyleShowDeleteButton;
    }

    /// Gets the selection, or NULL if there is none.
    wxTablePath* GetSelection() const;

    /*
    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);
    */
    
    virtual bool HasIndex() const { return m_indexTitles.GetCount() > 0; }
    
protected:
    
    void Init() { m_dataSource = NULL; }

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxTableCtrl)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_TABLECTRL_H_
