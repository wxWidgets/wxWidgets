/////////////////////////////////////////////////////////////////////////////
// Name:        propeditor.cpp
// Purpose:     Property editor for wxWidgets configuration tool
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-03
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _CT_PROPEDITOR_H_
#define _CT_PROPEDITOR_H_

#include "wx/splitter.h"
#include "wx/grid.h"

#include "configitem.h"
#include "property.h"

class WXDLLEXPORT wxHtmlWindow;

class ctPropertyEditorGrid;

/*!
 * A container window for the property editor.
 */

class ctPropertyEditor: public wxPanel
{
    DECLARE_CLASS(ctPropertyEditor)
  public:
    ctPropertyEditor(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
    virtual ~ctPropertyEditor(){}

// Event handlers

    /// Handles detailed editing event.
    void OnEditDetails(wxCommandEvent& event);

    /// Handles detailed editing update event.
    void OnUpdateEditDetails(wxUpdateUIEvent& event);

    /// Intercept selection event.
    void OnSelectCell(wxGridEvent& event);

    /// Intercept cell data change event.
    void OnChangeCell(wxGridEvent& event);

    /// Double-click to show specialised editor.
    void OnDClickCell(wxGridEvent& event);

// Operations

    /// Creates the controls
    void CreateControls(wxWindow* parent);

    /// Clear grid editor
    void ClearEditor();

    /// Can we edit the details of the selected property?
    bool CanEditDetails();

    /// Edit the details of this cell appropriately.
    bool EditDetails(wxWindow* parent);

    /// Shows the given config item
    void ShowItem(ctConfigItem* configItem);

    /// Updates the proxy display, assuming it was already displayed.
    void UpdateItem();

    /// Display attribute at given row
    bool DisplayProperty(int row, ctProperty* prop, bool valueOnly = false);

    /// Display attribute value
    bool DisplayProperty(ctProperty* prop);

    /// Edit the default property
    bool EditDefaultProperty(ctConfigItem* proxy);

    /// Select the default property
    bool DisplayDefaultProperty();

    /// Determine background colour for this property.
    void DeterminePropertyColour(ctProperty* prop, wxColour& colour);

    /// Update the title at the top of the property editor
    void UpdateTitle();

    /// Update the description
    void UpdateDescription(int row = -1);

    /// Wraps a description string in HTML
    wxString WrapDescription(const wxString& str);

    /// Find the selected property
    ctProperty* FindSelectedProperty(int& row);

    /// Find the nth property
    ctProperty* FindProperty(int row);

    /// Apply the cell value to the property, and notify the
    /// proxy object.
    void ApplyCellValueToProperty(int row, int col);

    /// Apply the cell value to the property, and notify the
    /// proxy object. This will be undoable.
    void ApplyPropertyValue(ctConfigItem* item, ctProperty* property, const wxVariant& variant);

    /// Show/hide the description control
    void ShowDescriptionWindow(bool show);

// Accessors

    //// Returns the grid used as the attribute editor
    ctPropertyEditorGrid* GetAttributeEditorGrid() const { return m_attributeEditorGrid; }

    /// Returns the title text control
    wxTextCtrl* GetElementTitleTextCtrl() const { return m_elementTitleTextCtrl; }

    /// Returns the splitter window between the property grid and
    /// the description window.
    wxSplitterWindow* GetSplitterWindow() const { return m_splitterWindow; }

    /// Returns the HTML description window
    wxHtmlWindow* GetDescriptionWindow() const { return m_propertyDescriptionWindow; }

    /// Returns the config item
    ctConfigItem* GetItem() const { return m_item; }

    /// Sets the proxy
    void SetItem(ctConfigItem* configItem) { m_item = configItem; }

DECLARE_EVENT_TABLE()

protected:
    // Dialog controls
    wxButton*               m_attributeEditorEditDetails;
    ctPropertyEditorGrid*   m_attributeEditorGrid;
    wxHtmlWindow*           m_propertyDescriptionWindow;
    wxSplitterWindow*       m_splitterWindow;

    // Displays the title of the element being edited
    wxTextCtrl*             m_elementTitleTextCtrl;

    // The config item object being edited
    ctConfigItem*           m_item;
};

#define ctID_ATTRIBUTE_EDITOR_INSERT        2001
#define ctID_ATTRIBUTE_EDITOR_GRID          2002
#define ctID_ATTRIBUTE_EDITOR_DESCRIPTION   2003
#define ctID_ATTRIBUTE_EDITOR_EDIT_DETAILS  2004

/*!
 * Attribute editor grid
 */

class ctPropertyEditorGrid: public wxGrid
{
DECLARE_CLASS(ctPropertyEditorGrid)
public:
    ctPropertyEditorGrid(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
        const wxSize& sz = wxDefaultSize, long style = wxWANTS_CHARS);

    /// Intercept size event.
    void OnSize(wxSizeEvent& event);

    void SetStretchableColumn(int which) { m_stretchableColumn = which; }
    int GetStretchableColumn() const { return m_stretchableColumn; }

    /// An array denoting which columns to display,
    /// and in what order.
    void SetColumnsToDisplay(wxArrayString& columns) { m_columnsToDisplay = columns; }
    wxArrayString& GetColumnsToDisplay() { return m_columnsToDisplay; }

    /// Use m_columnsToDisplay to set the label strings
    void DisplayLabels();

    /// Clear attributes
    bool ClearAttributes();

DECLARE_EVENT_TABLE()

private:
    int             m_stretchableColumn;
    wxArrayString   m_columnsToDisplay;
};

// Utility functions

/// Translate the value to one which can be edited in a single-line
/// text control
wxString ctConvertToSingleText(const wxString& value);

/// Translate back to 'real' characters, i.e. newlines are real
/// newlines.
wxString ctConvertToMultilineText(const wxString& value);

//----------------------------------------------------------------------------
// ctMultiLineTextEditor
//----------------------------------------------------------------------------

class ctMultiLineTextEditor: public wxDialog
{
public:
    // constructors and destructors
    ctMultiLineTextEditor( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxString& msg,
        const wxString& value,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

    bool AddControls(wxWindow* parent, const wxString& msg);

    wxString GetText() const { return m_text; }
    void SetText(const wxString& text) { m_text = text; }

private:
    DECLARE_EVENT_TABLE()
    wxString m_text;
};

/*
 * Special-purpose splitter window for changing sash look and
 * also saving sash width
 */

class ctSplitterWindow: public wxSplitterWindow
{
public:
    ctSplitterWindow(wxWindow* parent, wxWindowID id,
        const wxPoint& pos, const wxSize& size, long style);

    // Draws the sash
    //virtual void DrawSash(wxDC& dc);

    void OnChangeSash(wxSplitterEvent& event);

    void UpdateSettings(bool updateSettings) { m_updateSettings = updateSettings; }

    int GetLastPosition() const { return m_position; }
    void SetLastPosition(int pos) { m_position = pos; }

protected:
    // Don't update settings if the window is still being created,
    // since it could override the saved settings
    bool    m_updateSettings;
    int     m_position;

DECLARE_EVENT_TABLE()
};

/*!
 * Use a single-line text control.
 */

class ctGridCellTextEditor: public wxGridCellTextEditor
{
public:
    virtual void Create(wxWindow* parent, wxWindowID id,
        wxEvtHandler* evtHandler);
};

#define ctGRID_VALUE_STRING wxT("singlelinestring")

#define ctID_PROPERTY_EDITOR_SPLITTER 1600

#endif
  // _WB_PROPEDITOR_H_

