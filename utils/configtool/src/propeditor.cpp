/////////////////////////////////////////////////////////////////////////////
// Name:        propeditor.cpp
// Purpose:     wxWidgets Configuration Tool property editor
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-03
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "propeditor.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP

#include "wx/grid.h"
#include "wx/filedlg.h"

#endif

#include "wx/html/htmlwin.h"
#include "wx/tokenzr.h"
#include "wx/valgen.h"
#include "propeditor.h"
#include "symbols.h"
#include "utils.h"
#include "configtooldoc.h"
#include "configitemselector.h"

#include "bitmaps/ellipsis.xpm"


/*!
 * A container window for the property editor.
 * and attribute editor.
 */

IMPLEMENT_CLASS(ctPropertyEditor, wxPanel)

BEGIN_EVENT_TABLE(ctPropertyEditor, wxPanel)
    EVT_BUTTON(ctID_ATTRIBUTE_EDITOR_EDIT_DETAILS, ctPropertyEditor::OnEditDetails)
    EVT_GRID_SELECT_CELL(ctPropertyEditor::OnSelectCell)
    EVT_GRID_CELL_CHANGE(ctPropertyEditor::OnChangeCell)
    EVT_GRID_CELL_LEFT_DCLICK(ctPropertyEditor::OnDClickCell)
    EVT_UPDATE_UI(ctID_ATTRIBUTE_EDITOR_EDIT_DETAILS, ctPropertyEditor::OnUpdateEditDetails)
END_EVENT_TABLE()

ctPropertyEditor::ctPropertyEditor(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel(parent, id, pos, size, style)
{
    m_splitterWindow = NULL;
    m_attributeEditorGrid = NULL;
    m_propertyDescriptionWindow = NULL;
    m_elementTitleTextCtrl = NULL;

    CreateControls(this);
}

void ctPropertyEditor::CreateControls(wxWindow* parent)
{
    m_elementTitleTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    wxBitmap detailsIcon(ellipsis_xpm);

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxTextCtrl *item2 = m_elementTitleTextCtrl;
    item1->Add( item2, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_attributeEditorEditDetails = new wxButton( parent, ctID_ATTRIBUTE_EDITOR_EDIT_DETAILS, wxT("Edit..."));
    item1->Add( m_attributeEditorEditDetails, 0, wxALIGN_CENTRE|wxRIGHT|wxTOP|wxBOTTOM, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    // TODO: find sash pos from last time
    int sashPos = 100;

    m_splitterWindow = new wxSplitterWindow(parent, ctID_PROPERTY_EDITOR_SPLITTER, wxDefaultPosition, wxSize(500, 400), wxSP_3DSASH|wxSP_FULLSASH/*|wxCLIP_CHILDREN*/ |wxBORDER_NONE|wxNO_FULL_REPAINT_ON_RESIZE);
    m_splitterWindow->SetMinimumPaneSize(10);

    m_propertyDescriptionWindow = new wxHtmlWindow(m_splitterWindow, ctID_ATTRIBUTE_EDITOR_DESCRIPTION, wxDefaultPosition, wxSize(200, 60), wxSUNKEN_BORDER);
    m_propertyDescriptionWindow->SetBackgroundColour(ctDESCRIPTION_BACKGROUND_COLOUR);
    m_propertyDescriptionWindow->SetBorders(4);
    m_attributeEditorGrid = new ctPropertyEditorGrid(m_splitterWindow, ctID_ATTRIBUTE_EDITOR_GRID , wxPoint(0, 0), wxSize(200, 100), wxBORDER_SUNKEN | wxWANTS_CHARS);

    m_splitterWindow->SplitHorizontally(m_propertyDescriptionWindow, m_attributeEditorGrid, sashPos);

    // TODO: show or hide description window
//    if (some-setting)
//        ShowDescriptionWindow(false);

    item0->Add( m_splitterWindow, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    this->SetSizer( item0 );

    /// Add help text
    m_elementTitleTextCtrl->SetHelpText(_("The title of the property being edited."));
    m_attributeEditorEditDetails->SetHelpText(_("Click to use an appropriate editor for the selected property (if any)."));
    m_attributeEditorGrid->SetHelpText(_("Shows the properties of the selected item."));
    m_propertyDescriptionWindow->SetHelpText(_("Shows a description of the selected property, or a summary of the whole item."));

    /// Set up the grid to display properties
    m_attributeEditorGrid->RegisterDataType(ctGRID_VALUE_STRING,
                             new wxGridCellStringRenderer,
                             new ctGridCellTextEditor);

    m_attributeEditorGrid->CreateGrid( 0, 2, wxGrid::wxGridSelectRows );
    m_attributeEditorGrid->SetRowLabelSize(0);
    m_attributeEditorGrid->SetColLabelSize(0 /* 18 */);

    wxArrayString columns;
    columns.Add(_T("Name"));
    columns.Add(_T("Value"));

    m_attributeEditorGrid->SetColSize(0, 140);
    m_attributeEditorGrid->SetColSize(1, 80);

    m_attributeEditorGrid->SetColumnsToDisplay(columns);
    m_attributeEditorGrid->DisplayLabels();

    m_attributeEditorGrid->SetStretchableColumn(1);

    m_attributeEditorGrid->SetDefaultCellBackgroundColour(ctCELL_BACKGROUND_COLOUR);

    UpdateDescription();
}

/// Show/hide the description control
void ctPropertyEditor::ShowDescriptionWindow(bool show)
{
    if (!show)
    {
        if (m_splitterWindow->IsSplit())
            m_splitterWindow->Unsplit(m_propertyDescriptionWindow);
    }
    else
    {
        // TODO
        int pos = 100;
        m_propertyDescriptionWindow->Show(true);
        if (!m_splitterWindow->IsSplit())
        {
            m_splitterWindow->SplitHorizontally(m_propertyDescriptionWindow, m_attributeEditorGrid, pos);
        }
    }
}

/// Clear grid editor
void ctPropertyEditor::ClearEditor()
{
    m_attributeEditorGrid->ClearAttributes();
    m_propertyDescriptionWindow->SetPage(WrapDescription(wxEmptyString));
    m_elementTitleTextCtrl->SetValue(wxEmptyString);
}

/// Handles detailed editing event.
void ctPropertyEditor::OnEditDetails(wxCommandEvent& WXUNUSED(event))
{
    wxWindow* parentFrame = this;
    while (parentFrame && !parentFrame->IsKindOf(CLASSINFO(wxFrame)))
        parentFrame = parentFrame->GetParent();

    EditDetails(parentFrame);
}

/// Handles detailed editing update event.
void ctPropertyEditor::OnUpdateEditDetails(wxUpdateUIEvent& event)
{
    event.Enable(CanEditDetails());
}

/// Can we edit the details of the selected property?
bool ctPropertyEditor::CanEditDetails()
{
    if (!m_item)
        return false;

    int row;
    ctProperty* prop = FindSelectedProperty(row);
    if (!prop || prop->GetEditorType().IsEmpty())
        return false;
    return true;
}

/// Shows the item
void ctPropertyEditor::ShowItem(ctConfigItem* item)
{
    if (m_attributeEditorGrid->IsCellEditControlEnabled())
        m_attributeEditorGrid->SaveEditControlValue();

    ClearEditor();
    if (item)
    {
        m_item = item;

        UpdateTitle();

        m_attributeEditorGrid->AppendRows(m_item->GetProperties().GetCount());

        wxObjectList::compatibility_iterator node = m_item->GetProperties().GetList().GetFirst();
        int i = 0;
        while (node)
        {
            ctProperty* prop = (ctProperty*) node->GetData();
            DisplayProperty(i, prop);

            i ++;
            node = node->GetNext();
        }
        // Make sure scrollbars are up-to-date, etc.
        wxSizeEvent event(m_attributeEditorGrid->GetSize(), m_attributeEditorGrid->GetId());
        m_attributeEditorGrid->GetEventHandler()->ProcessEvent(event);

        DisplayDefaultProperty();
    }
    else
    {
        m_item = NULL;
    }
}

/// Determine background colour for this property.
void ctPropertyEditor::DeterminePropertyColour(ctProperty* prop, wxColour& colour)
{
    if (prop->IsCustom())
        colour = ctCUSTOM_CELL_BACKGROUND_COLOUR;
    else
        colour = ctCELL_BACKGROUND_COLOUR;
}

/// Update the title at the top of the property editor
void ctPropertyEditor::UpdateTitle()
{
    if (m_item)
    {
        wxString name(m_item->GetTitle());
        m_elementTitleTextCtrl->SetValue(name);
    }
}

/// Updates the item display, assuming it was already displayed.
void ctPropertyEditor::UpdateItem()
{
    if (m_attributeEditorGrid->IsCellEditControlEnabled())
        m_attributeEditorGrid->SaveEditControlValue();
    if (m_item)
    {
        UpdateTitle();

        wxObjectList::compatibility_iterator node = m_item->GetProperties().GetList().GetFirst();
        int i = 0;
        while (node)
        {
            ctProperty* prop = (ctProperty*) node->GetData();
            DisplayProperty(i, prop, true);

            i ++;
            node = node->GetNext();
        }
        // Make sure scrollbars are up-to-date, etc.
        wxSizeEvent event(m_attributeEditorGrid->GetSize(), this->GetId());
        m_attributeEditorGrid->GetEventHandler()->ProcessEvent(event);
    }
}

/// Display attribute at given row
bool ctPropertyEditor::DisplayProperty(int row, ctProperty* prop, bool valueOnly)
{
    wxColour backgroundColour;

    DeterminePropertyColour(prop, backgroundColour);

    if (!valueOnly)
    {
        m_attributeEditorGrid->SetCellBackgroundColour(row, 0, backgroundColour);
        m_attributeEditorGrid->SetCellBackgroundColour(row, 1, backgroundColour);

        m_attributeEditorGrid->SetCellValue(row, 0, prop->GetName());

        m_attributeEditorGrid->SetReadOnly(row, 0);
        // Set the alignment
        m_attributeEditorGrid->SetCellAlignment(row, 1, wxALIGN_LEFT, wxALIGN_CENTER);
    }

    if (!m_item->CanEditProperty(prop->GetName()))
    {
        m_attributeEditorGrid->SetReadOnly(row, 1);

        wxColour col(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        m_attributeEditorGrid->SetCellTextColour(row, 1, col);
    }
    else
    {
        m_attributeEditorGrid->SetReadOnly(row, 1, false);
        m_attributeEditorGrid->SetCellTextColour(row, 1, * wxBLACK);
    }

    // Set the value
    m_attributeEditorGrid->SetCellValue(row, 1, ctConvertToSingleText(prop->GetValue()));

    if (valueOnly)
        return true;

    // Set the value type
    if (prop->GetEditorType() == _T("choice"))
    {
#if 0
        wxString* strArr = prop->GetChoices().GetStringArray();

        m_attributeEditorGrid->SetCellEditor(row, 1,
                new wxGridCellChoiceEditor(prop->GetChoices().GetCount(), strArr));

        delete[] strArr;
#endif
        m_attributeEditorGrid->SetCellEditor(row, 1,
                new wxGridCellChoiceEditor(prop->GetChoices()));
    }
    else if (prop->GetEditorType() == _T("integer") || prop->GetVariant().GetType() == _T("long"))
    {
        m_attributeEditorGrid->SetCellEditor(row, 1,
                new wxGridCellNumberEditor);
    }
    else if (prop->GetEditorType() == _T("float") || prop->GetVariant().GetType() == _T("double"))
    {
        m_attributeEditorGrid->SetCellEditor(row, 1,
                new wxGridCellFloatEditor);
    }
    else if (prop->GetEditorType() == _T("bool") || prop->GetVariant().GetType() == _T("bool"))
    {
        m_attributeEditorGrid->SetCellValue(row, 1, prop->GetVariant().GetBool() ? _T("1") : _T("0"));
        m_attributeEditorGrid->SetCellEditor(row, 1,
                new wxGridCellBoolEditor);
        m_attributeEditorGrid->SetCellRenderer(row, 1, new wxGridCellBoolRenderer);
    }
    else
    {
        m_attributeEditorGrid->SetCellEditor(row, 1,
                new ctGridCellTextEditor);
    }

    return true;
}

/// Display attribute value
bool ctPropertyEditor::DisplayProperty(ctProperty* prop)
{
    if (!m_item)
        return false;

    int index = m_item->GetProperties().GetList().IndexOf(prop);
    return DisplayProperty(index, prop, true);
}

/// Display the default property
bool ctPropertyEditor::DisplayDefaultProperty()
{
    if (!m_item)
        return false;

    wxString str = m_item->GetDefaultProperty();

    ctProperty* prop = m_item->GetProperties().FindProperty(str);
    if (prop)
    {
        int index = m_item->GetProperties().GetList().IndexOf(prop);
        this->m_attributeEditorGrid->SelectRow(index);
        this->m_attributeEditorGrid->SetGridCursor(index, 1);
    }
    return true;
}

/// Edit the default property
bool ctPropertyEditor::EditDefaultProperty(ctConfigItem* item)
{
    wxString defaultPropertyName = item->GetDefaultProperty();
    if (!defaultPropertyName.IsEmpty())
    {
        ctProperty* prop = item->GetProperties().FindProperty(defaultPropertyName);
        if (prop)
        {
            int index = item->GetProperties().GetList().IndexOf(prop);
            if (index >= 0)
            {
                this->m_attributeEditorGrid->SelectRow(index);
                this->m_attributeEditorGrid->SetGridCursor(index, 1);
                EditDetails(wxTheApp->GetTopWindow());
                return true;
            }
        }

    }
    return false;
}

/// Find the selected property
ctProperty* ctPropertyEditor::FindSelectedProperty(int& row)
{
    if (!m_item)
        return NULL;

    int selRow = m_attributeEditorGrid->GetCursorRow();
    if (selRow > -1)
    {
        row = selRow;

        if (selRow < (int) m_item->GetProperties().GetCount())
        {
            ctProperty* prop = m_item->GetProperties().GetNth(selRow);
            return prop;
        }
    }
    return NULL;
}

/// Find the property
ctProperty* ctPropertyEditor::FindProperty(int row)
{
    if (!m_item)
        return NULL;

    if (row > -1)
    {
        if (row < (int) m_item->GetProperties().GetCount())
        {
            ctProperty* prop = m_item->GetProperties().GetNth(row);
            return prop;
        }
    }
    return NULL;
}

/// Edit the details of this cell appropriately.
bool ctPropertyEditor::EditDetails(wxWindow* WXUNUSED(parent))
{
    if (CanEditDetails())
    {
        int row;
        ctProperty* prop = FindSelectedProperty(row);
        if (!prop)
            return false;

        wxString type(prop->GetEditorType());
        wxString value = m_attributeEditorGrid->GetCellValue(row, 1);

        if (type == _T("multiline"))
        {
            value = ctConvertToMultilineText(value);
            wxString msg;
            msg.Printf(wxT("Edit %s:"), (const wxChar*) prop->GetName());
            ctMultiLineTextEditor dialog(wxTheApp->GetTopWindow(),
                wxID_ANY, wxT("Edit Text Property"), msg, value);
            if (dialog.ShowModal() == wxID_OK)
            {
                value = ctConvertToSingleText(dialog.GetText());
                m_attributeEditorGrid->SetCellValue(row, 1, value);
                ApplyCellValueToProperty(row, 1);
                return true;
            }
            else
                return false;
        }
        else if (type == _T("filename"))
        {
            wxString fullPath = value;
            wxString defaultDir ;
            wxString defaultFilename = wxFileNameFromPath(fullPath);

            defaultDir = wxPathOnly(value);

            wxString msg = wxT("Choose a filename");
            wxFileDialog dialog(wxTheApp->GetTopWindow(),
                    msg, defaultDir, defaultFilename, wxT("*.*"));
            if (dialog.ShowModal() == wxID_OK)
            {
                fullPath = dialog.GetPath();
                value = fullPath;

                m_attributeEditorGrid->SetCellValue(row, 1, value);
                ApplyCellValueToProperty(row, 1);
                return true;
            }
            else
                return false;
        }
        else if (type == _T("configitems"))
        {
            wxArrayString items;
            ctConfigItem::StringToArray(value, items);

            ctConfigItemsSelector dialog(wxTheApp->GetTopWindow(),
                    wxID_ANY, wxT("Select Configuration Items"));
            dialog.SetConfigList(items);
            if (dialog.ShowModal() == wxID_OK)
            {
                wxString newValue;
                items = dialog.GetConfigList();
                ctConfigItem::ArrayToString(items, newValue);

                m_attributeEditorGrid->SetCellValue(row, 1, newValue);
                ApplyCellValueToProperty(row, 1);
                return true;
            }
            else
                return false;
        }
    }

    return false;
}

/// Intercept selection event.
void ctPropertyEditor::OnSelectCell(wxGridEvent& event)
{
    int row = event.GetRow();

    UpdateDescription(row);

    event.Skip();
}

/// Update the description
void ctPropertyEditor::UpdateDescription(int row)
{
    if (row == -1)
    {
        row = m_attributeEditorGrid->GetCursorRow();
    }
    if (row == -1)
    {
        wxString str = WrapDescription(wxEmptyString);
        m_propertyDescriptionWindow->SetPage(str);
    }
    else
    {
        ctProperty* prop = FindProperty(row);
        if (prop)
        {
            wxString str = WrapDescription(m_item->GetDescription(prop));
            m_propertyDescriptionWindow->SetPage(str);
        }
    }
}

/// Wraps a description string in HTML
wxString ctPropertyEditor::WrapDescription(const wxString& s)
{
    /// Convert a colour to a 6-digit hex string
    wxColour col = ctDESCRIPTION_BACKGROUND_COLOUR;
    wxString colStr = apColourToHexString(col);
    colStr = wxT("#") + colStr;

    wxString str;
    str << _T("<HTML><BODY BGCOLOR=\"") << colStr << wxT("\"><FONT SIZE=-1>") ;
    str << s;
    str << _T("</FONT></BODY></HTML>");

    return str;
}

/// Intercept cell data change event.
void ctPropertyEditor::OnChangeCell(wxGridEvent& event)
{
    int row = event.GetRow();
    int col = event.GetCol();

    ApplyCellValueToProperty(row, col);
}

/// Double-click to show specialised editor.
void ctPropertyEditor::OnDClickCell(wxGridEvent& WXUNUSED(event))
{
    wxWindow* parentFrame = this;
    while (parentFrame && !parentFrame->IsKindOf(CLASSINFO(wxFrame)))
        parentFrame = parentFrame->GetParent();

    EditDetails(parentFrame);
}

/// Apply the cell value to the property, and notify the
/// item object.
void ctPropertyEditor::ApplyCellValueToProperty(int row, int col)
{
    static bool s_Applying = false;

    if (s_Applying)
        return;

    s_Applying = true;
    if (col == 1 && m_item)
    {
        ctProperty* prop = m_item->GetProperties().GetNth(row);

        wxString value = m_attributeEditorGrid->GetCellValue(row, col);
        if (prop->GetEditorType() == wxT("multiline"))
            value = ctConvertToMultilineText(value);

        wxVariant variant = prop->GetVariant();

        if (prop->GetVariant().GetType() == _T("bool"))
        {
            if (value == _T("1"))
                variant = true;
            else
                variant = false;
        }
        else if (prop->GetVariant().GetType() == _T("long"))
        {
            long l;
            value.ToLong(& l);
            variant = l;
        }
        else if (prop->GetVariant().GetType() == _T("double"))
        {
            double d;
            value.ToDouble(& d);
            variant = d;
        }
        else
        {
            variant = value;
        }

        ApplyPropertyValue(m_item, prop, variant);

        if (prop->GetName() == _T("description"))
            UpdateDescription(row);
    }
    s_Applying = false;
}

/// Apply the cell value to the property, and notify the
/// item object.
void ctPropertyEditor::ApplyPropertyValue(ctConfigItem* item, ctProperty* property, const wxVariant& variant)
{
    static bool s_Applying = false;

    if (s_Applying)
        return;

    s_Applying = true;

    // Save the old values
    ctProperties* oldProperties = new ctProperties(item->GetProperties());

    wxVariant oldValue = property->GetVariant();

    // Apply the new value
    property->GetVariant() = variant;
    item->ApplyProperty(property, oldValue);
    item->Modify();

    UpdateItem();

    wxString menuLabel(_T("Change ") + property->GetName());

    // This won't do anything first time Do is applied,
    // since we've already done the action for this property.
    // But when we Undo or Redo, the changed properties will be applied.
    item->GetDocument()->GetCommandProcessor()->Submit(
        new ctConfigCommand(menuLabel, ctCMD_APPLY_PROPERTY,
        item, oldProperties, true));

    s_Applying = false;
}

/*!
 * Attribute editor grid
 */

IMPLEMENT_CLASS(ctPropertyEditorGrid, wxGrid)

BEGIN_EVENT_TABLE(ctPropertyEditorGrid, wxGrid)
    EVT_SIZE(ctPropertyEditorGrid::OnSize)
END_EVENT_TABLE()

ctPropertyEditorGrid::ctPropertyEditorGrid(wxWindow* parent, wxWindowID id,
                                             const wxPoint& pos,
                                             const wxSize& sz, long style):
    wxGrid(parent, id, pos, sz, style)

{
    m_stretchableColumn = -1;
}

void ctPropertyEditorGrid::OnSize(wxSizeEvent& event)
{
    if (m_stretchableColumn != -1)
    {
        // This window's client size = the internal window's
        // client size if it has no borders
        wxSize sz = GetClientSize();

        int totalSize = 0;
        int i;
        for (i = 0; i < GetNumberCols(); i ++)
        {
            if (i != m_stretchableColumn)
            {
                totalSize += GetColSize(i);
            }
        }

        // Allow for grid lines
        totalSize += 1;

        int stretchSize = wxMax(5, sz.x - totalSize);
        SetColSize(m_stretchableColumn, stretchSize);
    }

    event.Skip();
}

/// Use m_columnsToDisplay to set the label strings
void ctPropertyEditorGrid::DisplayLabels()
{
    size_t i;
    for (i = 0; i < m_columnsToDisplay.GetCount(); i++)
    {
        SetColLabelValue(i, m_columnsToDisplay[i]);
    }
}

/// Clear attributes
bool ctPropertyEditorGrid::ClearAttributes()
{
    if (GetNumberRows() > 0)
        DeleteRows(0, GetNumberRows());
    return true;
}

/*!
 * Use a single-line text control.
 */

void ctGridCellTextEditor::Create(wxWindow* parent, wxWindowID id,
        wxEvtHandler* evtHandler)
{
    m_control = new wxTextCtrl(parent, id, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize
                              );

    wxGridCellEditor::Create(parent, id, evtHandler);
}


/// Translate the value to one which can be edited in a single-line
/// text control
wxString ctConvertToSingleText(const wxString& value)
{
    wxString value1(value);
    value1.Replace(wxT("\n"), wxT("\\n"));
    value1.Replace(wxT("\t"), wxT("\\t"));
    return value1;
}

/// Translate back to 'real' characters, i.e. newlines are real
/// newlines.
wxString ctConvertToMultilineText(const wxString& value)
{
    wxString value1(value);
    value1.Replace(wxT("\\n"), wxT("\n"));
    value1.Replace(wxT("\\t"), wxT("\t"));
    return value1;
}

//----------------------------------------------------------------------------
// ctMultiLineTextEditor
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ctMultiLineTextEditor, wxDialog)
END_EVENT_TABLE()

ctMultiLineTextEditor::ctMultiLineTextEditor( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxString& msg,
        const wxString& value,
        const wxPoint& pos,
        const wxSize& size,
        long style):
    wxDialog(parent, id, title, pos, size, style)
{
    m_text = value;
    AddControls(this, msg);
    Centre();
}

bool ctMultiLineTextEditor::AddControls(wxWindow* parent, const wxString& msg)
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item2 = new wxStaticText( parent, wxID_STATIC, msg, wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxLEFT|wxRIGHT, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(330,180), wxTE_MULTILINE|wxTE_RICH );
    item1->Add( item3, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item4 = new wxBoxSizer( wxHORIZONTAL );

    item4->Add( 5, 5, 1, wxALIGN_CENTRE|wxALL, 5 );

    wxButton *item5 = new wxButton( parent, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxButton *item6 = new wxButton( parent, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item0->Add( item1, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item3->SetValue(m_text);
    item3->SetValidator(wxGenericValidator(& m_text));


    item3->SetFocus();
    item5->SetDefault();

    parent->SetSizer(item0);
    item0->Fit(parent);

    return true;
}

/*
 * Special-purpose splitter window for changing sash look and
 * also saving sash width
 */

BEGIN_EVENT_TABLE(ctSplitterWindow, wxSplitterWindow)
    EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, ctSplitterWindow::OnChangeSash)
END_EVENT_TABLE()

ctSplitterWindow::ctSplitterWindow(wxWindow* parent, wxWindowID id,
        const wxPoint& pos, const wxSize& size, long style):
      wxSplitterWindow(parent, id, pos, size, style)
{
    m_updateSettings = false;
    m_position = 0;
}

void ctSplitterWindow::OnChangeSash(wxSplitterEvent& event)
{
    if (!m_updateSettings)
        return;

    m_position = event.GetSashPosition();
}
