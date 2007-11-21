/////////////////////////////////////////////////////////////////////////////
// Name:        contrib/samples/ogl/studio/dialogs.cpp
// Purpose:     Implements Studio dialogs
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "dialogs.h"
#include "doc.h"
#include "view.h"
#include "studio.h"
#include "studio_resources.h"

#if wxUSE_WX_RESOURCES

IMPLEMENT_CLASS(csLabelEditingDialog, wxDialog)

BEGIN_EVENT_TABLE(csLabelEditingDialog, wxDialog)
    EVT_BUTTON(wxID_OK, csLabelEditingDialog::OnOK)
END_EVENT_TABLE()

csLabelEditingDialog::csLabelEditingDialog(wxWindow* parent)
{
    wxLoadFromResource(this, parent, _T("shape_label_dialog"));

    // Accelerators
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, WXK_RETURN, wxID_OK);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    Centre();

    wxTextCtrl* textCtrl = (wxTextCtrl*) FindWindow(ID_LABELTEXT);
    wxASSERT( (textCtrl != NULL) );

//    textCtrl->SetAcceleratorTable(accel);

    textCtrl->SetFocus();
}

void csLabelEditingDialog::OnOK(wxCommandEvent& event)
{
    wxTextCtrl* textCtrl = (wxTextCtrl*) FindWindow(ID_LABELTEXT);
    wxASSERT( (textCtrl != NULL) );

    SetShapeLabel(textCtrl->GetValue());

    wxDialog::OnOK(event);
}

void csLabelEditingDialog::SetShapeLabel(const wxString& label)
{
    wxTextCtrl* textCtrl = (wxTextCtrl*) FindWindow(ID_LABELTEXT);
    wxASSERT( (textCtrl != NULL) );

    m_label = label;

    textCtrl->SetValue(label);
}

IMPLEMENT_CLASS(csSettingsDialog, wxDialog)

BEGIN_EVENT_TABLE(csSettingsDialog, wxDialog)
    EVT_BUTTON(wxID_OK, csSettingsDialog::OnOK)
END_EVENT_TABLE()

#define PROPERTY_DIALOG_WIDTH   400
#define PROPERTY_DIALOG_HEIGHT  400

// For 400x400 settings dialog, size your panels to about 375x325 in dialog editor

csSettingsDialog::csSettingsDialog(wxWindow* parent):
    wxDialog(parent, wxID_ANY, _T("Settings"), wxPoint(0, 0), wxSize(PROPERTY_DIALOG_WIDTH, PROPERTY_DIALOG_HEIGHT))
{
    m_generalSettings = NULL;
    m_diagramSettings = NULL;

    m_notebook = new wxNotebook(this, ID_PROPERTY_NOTEBOOK,
         wxPoint(2, 2), wxSize(PROPERTY_DIALOG_WIDTH - 4, PROPERTY_DIALOG_HEIGHT - 4));

    m_generalSettings = new wxPanel;

    #ifdef  __WXDEBUG__
    bool success =
    #endif
                   wxLoadFromResource(m_generalSettings, m_notebook, _T("general_settings_dialog"));
    wxASSERT_MSG( (success), _T("Could not load general settings panel."));
    m_notebook->AddPage(m_generalSettings, _T("General"), true);

    m_diagramSettings = new wxPanel;

    #ifdef  __WXDEBUG__
    success =
    #endif
              wxLoadFromResource(m_diagramSettings, m_notebook, _T("diagram_settings_dialog"));
    wxASSERT_MSG( (success), _T("Could not load diagram settings panel."));
    m_notebook->AddPage(m_diagramSettings, _T("Diagram"));

    int largeButtonWidth = 70;
    int largeButtonHeight = 22;

    wxButton* okButton = new wxButton(this, wxID_OK, _T("OK"), wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"), wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));
    wxButton* helpButton = new wxButton(this, wxID_HELP, _T("Help"), wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));

    // Constraints for the notebook
    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->top.SameAs     (this, wxTop, 5);
    c->left.SameAs    (this, wxLeft, 5);
    c->right.SameAs   (this, wxRight, 5);
    c->bottom.SameAs  (cancelButton, wxTop, 5);
    m_notebook->SetConstraints(c);

    // Constraints for the Help button
    c = new wxLayoutConstraints;
    c->width.AsIs();
    c->height.AsIs();
    c->right.SameAs    (this, wxRight, 5);
    c->bottom.SameAs   (this, wxBottom, 5);
    helpButton->SetConstraints(c);

    // Constraints for the Cancel button
    c = new wxLayoutConstraints;
    c->width.AsIs();
    c->height.AsIs();
    c->right.SameAs    (helpButton, wxLeft, 5);
    c->bottom.SameAs   (this, wxBottom, 5);
    cancelButton->SetConstraints(c);

    // Constraints for the OK button
    c = new wxLayoutConstraints;
    c->width.AsIs();
    c->height.AsIs();
    c->right.SameAs    (cancelButton, wxLeft, 5);
    c->bottom.SameAs   (this, wxBottom, 5);
    okButton->SetConstraints(c);

    okButton->SetDefault();
    okButton->SetFocus();

    Layout();
    Centre(wxBOTH);
}

void csSettingsDialog::OnOK(wxCommandEvent& event)
{
    wxDialog::OnOK(event);
}

bool csSettingsDialog::TransferDataToWindow()
{
    wxTextCtrl* gridSpacing = (wxTextCtrl*) m_diagramSettings->FindWindow(ID_GRID_SPACING);
    wxASSERT_MSG( (gridSpacing != (wxTextCtrl*) NULL), _T("Could not find grid spacing control."));

    wxChoice* gridStyle = (wxChoice*) m_diagramSettings->FindWindow(ID_GRID_STYLE);
    wxASSERT_MSG( (gridStyle != (wxChoice*) NULL), _T("Could not find grid style control."));

    gridStyle->SetSelection(wxGetApp().GetGridStyle());

    wxString str;
    str.Printf(_T("%d"), wxGetApp().GetGridSpacing());
    gridSpacing->SetValue(str);

    return true;
}

bool csSettingsDialog::TransferDataFromWindow()
{
    wxTextCtrl* gridSpacing = (wxTextCtrl*) m_diagramSettings->FindWindow(ID_GRID_SPACING);
    wxASSERT_MSG( (gridSpacing != (wxTextCtrl*) NULL), _T("Could not find grid spacing control."));

    wxChoice* gridStyle = (wxChoice*) m_diagramSettings->FindWindow(ID_GRID_STYLE);
    wxASSERT_MSG( (gridStyle != (wxChoice*) NULL), _T("Could not find grid style control."));

    wxGetApp().SetGridStyle(gridStyle->GetSelection());
    wxString str = gridSpacing->GetValue();
    long grid_spacing;
    str.ToLong( &grid_spacing);
    wxGetApp().SetGridSpacing(grid_spacing);

    if (wxGetApp().GetGridStyle() == csGRID_STYLE_DOTTED)
    {
        wxMessageBox(_T("Dotted grid style not yet implemented."), _T("Studio"), wxICON_EXCLAMATION);
        return false;
    }

    // Apply settings to all open diagram documents
    wxObjectList::compatibility_iterator node = wxGetApp().GetDocManager()->GetDocuments().GetFirst();
    while (node)
    {
        wxDocument* doc = (wxDocument*) node->GetData();
        if (doc->IsKindOf(CLASSINFO(csDiagramDocument)))
        {
            csDiagramDocument* diagramDoc = (csDiagramDocument*) doc;
            wxDiagram* diagram = (wxDiagram*) diagramDoc->GetDiagram();

            diagram->SetGridSpacing((double) wxGetApp().GetGridSpacing());
            switch (wxGetApp().GetGridStyle())
            {
                case csGRID_STYLE_NONE:
                {
                    diagram->SetSnapToGrid(false);
                    break;
                }
                case csGRID_STYLE_INVISIBLE:
                {
                    diagram->SetSnapToGrid(true);
                    break;
                }
                case csGRID_STYLE_DOTTED:
                {
                    // TODO (not implemented in OGL)
                    break;
                }
            }
        }
        node = node->GetNext();
    }

    return true;
}

/*
 * Shape properties dialog (tabbed)
 */


IMPLEMENT_CLASS(csShapePropertiesDialog, wxDialog)

BEGIN_EVENT_TABLE(csShapePropertiesDialog, wxDialog)
    EVT_BUTTON(wxID_OK, csShapePropertiesDialog::OnOK)
END_EVENT_TABLE()

#define SHAPE_PROPERTY_DIALOG_WIDTH   400
#define SHAPE_PROPERTY_DIALOG_HEIGHT  400

// For 400x400 settings dialog, size your panels to about 375x325 in dialog editor

csShapePropertiesDialog::csShapePropertiesDialog(wxWindow* parent, const wxString& title,
    wxPanel* attributeDialog, const wxString& attributeDialogName):
    wxDialog(parent, wxID_ANY, title, wxPoint(0, 0), wxSize(SHAPE_PROPERTY_DIALOG_WIDTH, SHAPE_PROPERTY_DIALOG_HEIGHT))
{
    m_attributeDialog = attributeDialog;
    m_alternativeAttributeDialog = NULL;
    m_generalPropertiesDialog = NULL;

    m_notebook = new wxNotebook(this, ID_SHAPE_PROPERTY_NOTEBOOK,
         wxPoint(2, 2), wxSize(SHAPE_PROPERTY_DIALOG_WIDTH - 4, SHAPE_PROPERTY_DIALOG_HEIGHT - 4));

    m_generalPropertiesDialog = new csGeneralShapePropertiesDialog;
    #ifdef  __WXDEBUG__
    bool success =
    #endif
                   wxLoadFromResource(m_generalPropertiesDialog, m_notebook, _T("general_shape_properties_dialog"));
    wxASSERT_MSG( (success), _T("Could not load general properties panel."));
    m_notebook->AddPage(m_generalPropertiesDialog, _T("General"));

    if (!wxLoadFromResource(m_attributeDialog, m_notebook, attributeDialogName))
    {
        wxMessageBox(_T("Could not load the attribute dialog for this shape."), _T("Studio"), wxICON_EXCLAMATION);
        delete m_attributeDialog;
        m_attributeDialog = NULL;
    }
    else
    {
        m_notebook->AddPage(m_attributeDialog, _T("Attributes"));
    }

    // Try the alternative dialog (test code)
    wxString str(attributeDialogName);
    str += _T("1");
    m_alternativeAttributeDialog = new wxPanel;
    if (wxLoadFromResource(m_alternativeAttributeDialog, m_notebook, str))
    {
        m_notebook->AddPage(m_alternativeAttributeDialog, _T("Attributes (alternative)"));
    }
    else
    {
        delete m_alternativeAttributeDialog;
        m_alternativeAttributeDialog = NULL;
    }

    int largeButtonWidth = 70;
    int largeButtonHeight = 22;

    wxButton* okButton = new wxButton(this, wxID_OK, _T("OK"), wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"), wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));
    wxButton* helpButton = new wxButton(this, wxID_HELP, _T("Help"), wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));

    // Constraints for the notebook
    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->top.SameAs     (this, wxTop, 5);
    c->left.SameAs    (this, wxLeft, 5);
    c->right.SameAs   (this, wxRight, 5);
    c->bottom.SameAs  (helpButton, wxTop, 5);
    m_notebook->SetConstraints(c);

    // Constraints for the Help button
    c = new wxLayoutConstraints;
    c->width.AsIs();
    c->height.AsIs();
    c->right.SameAs    (this, wxRight, 5);
    c->bottom.SameAs   (this, wxBottom, 5);
    helpButton->SetConstraints(c);

    // Constraints for the Cancel button
    c = new wxLayoutConstraints;
    c->width.AsIs();
    c->height.AsIs();
    c->right.SameAs    (helpButton, wxLeft, 5);
    c->bottom.SameAs   (this, wxBottom, 5);
    cancelButton->SetConstraints(c);

    // Constraints for the OK button
    c = new wxLayoutConstraints;
    c->width.AsIs();
    c->height.AsIs();
    c->right.SameAs    (cancelButton, wxLeft, 5);
    c->bottom.SameAs   (this, wxBottom, 5);
    okButton->SetConstraints(c);

    okButton->SetDefault();
    okButton->SetFocus();

    SetDefaults();

    Layout();
    Centre(wxBOTH);
}

void csShapePropertiesDialog::OnOK(wxCommandEvent& event)
{
    wxTextCtrl* textCtrl = (wxTextCtrl*) m_generalPropertiesDialog->FindWindow(ID_LABELTEXT);
    wxASSERT( (textCtrl != NULL) );

    m_generalPropertiesDialog->SetShapeLabel(textCtrl->GetValue());

    wxDialog::OnOK(event);
}

// Set some suitable defaults in the attribute dialogs (in the first instance,
// just set all wxChoices to the first element)
void csShapePropertiesDialog::SetDefaults()
{
    if (!m_attributeDialog)
        return;

    wxWindowList::compatibility_iterator node = m_attributeDialog->GetChildren().GetFirst();
    while (node)
    {
        wxWindow* child = (wxWindow*) node->GetData();
        if (child->IsKindOf(CLASSINFO(wxChoice)))
        {
            wxChoice* choice = (wxChoice*) child;
            choice->SetSelection(0);
        }
        node = node->GetNext();
    }

    if (!m_alternativeAttributeDialog)
        return;

    node = m_alternativeAttributeDialog->GetChildren().GetFirst();
    while (node)
    {
        wxWindow* child = (wxWindow*) node->GetData();
        if (child->IsKindOf(CLASSINFO(wxChoice)))
        {
            wxChoice* choice = (wxChoice*) child;
            choice->SetSelection(0);
        }
        node = node->GetNext();
    }
}

/*
 * csGeneralShapePropertiesDialog
 */

IMPLEMENT_CLASS(csGeneralShapePropertiesDialog, wxPanel)

BEGIN_EVENT_TABLE(csGeneralShapePropertiesDialog, wxPanel)
END_EVENT_TABLE()

csGeneralShapePropertiesDialog::csGeneralShapePropertiesDialog()
{
}

void csGeneralShapePropertiesDialog::SetShapeLabel(const wxString& label)
{
    wxTextCtrl* textCtrl = (wxTextCtrl*) FindWindow(ID_LABELTEXT);
    wxASSERT( (textCtrl != NULL) );

    m_label = label;

    textCtrl->SetValue(label);
}

#endif // wxUSE_WX_RESOURCES

/*
 * csThinRectangleDialog
 */

IMPLEMENT_CLASS(csThinRectangleDialog, wxPanel)

BEGIN_EVENT_TABLE(csThinRectangleDialog, wxPanel)
END_EVENT_TABLE()

csThinRectangleDialog::csThinRectangleDialog()
{
}

/*
 * csWideRectangleDialog
 */

IMPLEMENT_CLASS(csWideRectangleDialog, wxPanel)

BEGIN_EVENT_TABLE(csWideRectangleDialog, wxPanel)
END_EVENT_TABLE()

csWideRectangleDialog::csWideRectangleDialog()
{
}

/*
 * csTriangleDialog
 */

IMPLEMENT_CLASS(csTriangleDialog, wxPanel)

BEGIN_EVENT_TABLE(csTriangleDialog, wxPanel)
END_EVENT_TABLE()

csTriangleDialog::csTriangleDialog()
{
}

/*
 * csSemiCircleDialog
 */

IMPLEMENT_CLASS(csSemiCircleDialog, wxPanel)

BEGIN_EVENT_TABLE(csSemiCircleDialog, wxPanel)
END_EVENT_TABLE()

csSemiCircleDialog::csSemiCircleDialog()
{
}

/*
 * csCircleDialog
 */

IMPLEMENT_CLASS(csCircleDialog, wxPanel)

BEGIN_EVENT_TABLE(csCircleDialog, wxPanel)
END_EVENT_TABLE()

csCircleDialog::csCircleDialog()
{
}

/*
 * csCircleShadowDialog
 */

IMPLEMENT_CLASS(csCircleShadowDialog, wxPanel)

BEGIN_EVENT_TABLE(csCircleShadowDialog, wxPanel)
END_EVENT_TABLE()

csCircleShadowDialog::csCircleShadowDialog()
{
}

/*
 * csOctagonDialog
 */

IMPLEMENT_CLASS(csOctagonDialog, wxPanel)

BEGIN_EVENT_TABLE(csOctagonDialog, wxPanel)
END_EVENT_TABLE()

csOctagonDialog::csOctagonDialog()
{
}

/*
 * csGroupDialog
 */

IMPLEMENT_CLASS(csGroupDialog, wxPanel)

BEGIN_EVENT_TABLE(csGroupDialog, wxPanel)
END_EVENT_TABLE()

csGroupDialog::csGroupDialog()
{
}

/*
 * csTextBoxDialog
 */

IMPLEMENT_CLASS(csTextBoxDialog, wxPanel)

BEGIN_EVENT_TABLE(csTextBoxDialog, wxPanel)
END_EVENT_TABLE()

csTextBoxDialog::csTextBoxDialog()
{
}
