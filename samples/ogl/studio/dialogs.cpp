/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.cpp
// Purpose:     Implements Studio dialogs
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/resource.h>
#include "dialogs.h"
#include "doc.h"
#include "view.h"
#include "studio.h"
#include "studio_resources.h"

IMPLEMENT_CLASS(csLabelEditingDialog, wxDialog)

BEGIN_EVENT_TABLE(csLabelEditingDialog, wxDialog)
    EVT_BUTTON(wxID_OK, csLabelEditingDialog::OnOK)
END_EVENT_TABLE()

csLabelEditingDialog::csLabelEditingDialog(wxWindow* parent)
{
    LoadFromResource(parent, "shape_label_dialog");

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
    wxDialog(parent, -1, "Settings", wxPoint(0, 0), wxSize(PROPERTY_DIALOG_WIDTH, PROPERTY_DIALOG_HEIGHT))
{
    m_generalSettings = NULL;
    m_diagramSettings = NULL;

    m_notebook = new wxNotebook(this, ID_PROPERTY_NOTEBOOK,
         wxPoint(2, 2), wxSize(PROPERTY_DIALOG_WIDTH - 4, PROPERTY_DIALOG_HEIGHT - 4));

    m_generalSettings = new wxPanel;

    bool success = m_generalSettings->LoadFromResource(m_notebook, "general_settings_dialog");
    wxASSERT_MSG( (success), "Could not load general settings panel.");
    m_notebook->AddPage(m_generalSettings, "General", TRUE);

    m_diagramSettings = new wxPanel;

    success = m_diagramSettings->LoadFromResource(m_notebook, "diagram_settings_dialog");
    wxASSERT_MSG( (success), "Could not load diagram settings panel.");
    m_notebook->AddPage(m_diagramSettings, "Diagram");

    int largeButtonWidth = 70;
    int largeButtonHeight = 22;

    wxButton* okButton = new wxButton(this, wxID_OK, "OK", wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel", wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));
    wxButton* helpButton = new wxButton(this, wxID_HELP, "Help", wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));

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
    wxASSERT_MSG( (gridSpacing != (wxTextCtrl*) NULL), "Could not find grid spacing control.");

    wxChoice* gridStyle = (wxChoice*) m_diagramSettings->FindWindow(ID_GRID_STYLE);
    wxASSERT_MSG( (gridStyle != (wxChoice*) NULL), "Could not find grid style control.");

    gridStyle->SetSelection(wxGetApp().GetGridStyle());

    wxString str;
    str.Printf("%d", wxGetApp().GetGridSpacing());
    gridSpacing->SetValue(str);

    return TRUE;
}

bool csSettingsDialog::TransferDataFromWindow()
{
    wxTextCtrl* gridSpacing = (wxTextCtrl*) m_diagramSettings->FindWindow(ID_GRID_SPACING);
    wxASSERT_MSG( (gridSpacing != (wxTextCtrl*) NULL), "Could not find grid spacing control.");

    wxChoice* gridStyle = (wxChoice*) m_diagramSettings->FindWindow(ID_GRID_STYLE);
    wxASSERT_MSG( (gridStyle != (wxChoice*) NULL), "Could not find grid style control.");

    wxGetApp().SetGridStyle(gridStyle->GetSelection());
    wxGetApp().SetGridSpacing(atoi(gridSpacing->GetValue()));

    if (wxGetApp().GetGridStyle() == csGRID_STYLE_DOTTED)
    {
        wxMessageBox("Dotted grid style not yet implemented.", "Studio", wxICON_EXCLAMATION);
        return FALSE;
    }

    // Apply settings to all open diagram documents
    wxNode* node = wxGetApp().GetDocManager()->GetDocuments().First();
    while (node)
    {
        wxDocument* doc = (wxDocument*) node->Data();
        if (doc->IsKindOf(CLASSINFO(csDiagramDocument)))
        {
            csDiagramDocument* diagramDoc = (csDiagramDocument*) doc;
            wxDiagram* diagram = (wxDiagram*) diagramDoc->GetDiagram();

            diagram->SetGridSpacing((double) wxGetApp().GetGridSpacing());
            switch (wxGetApp().GetGridStyle())
            {
                case csGRID_STYLE_NONE:
                {
                    diagram->SetSnapToGrid(FALSE);
                    break;
                }
                case csGRID_STYLE_INVISIBLE:
                {
                    diagram->SetSnapToGrid(TRUE);
                    break;
                }
                case csGRID_STYLE_DOTTED:
                {
                    // TODO (not implemented in OGL)
                    break;
                }
            }
        }
        node = node->Next();
    }

    return TRUE;
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
    wxDialog(parent, -1, title, wxPoint(0, 0), wxSize(SHAPE_PROPERTY_DIALOG_WIDTH, SHAPE_PROPERTY_DIALOG_HEIGHT))
{
    m_attributeDialog = attributeDialog;
    m_alternativeAttributeDialog = NULL;
    m_generalPropertiesDialog = NULL;

    m_notebook = new wxNotebook(this, ID_SHAPE_PROPERTY_NOTEBOOK,
         wxPoint(2, 2), wxSize(SHAPE_PROPERTY_DIALOG_WIDTH - 4, SHAPE_PROPERTY_DIALOG_HEIGHT - 4));

    m_generalPropertiesDialog = new csGeneralShapePropertiesDialog;
    bool success = m_generalPropertiesDialog->LoadFromResource(m_notebook, "general_shape_properties_dialog");
    wxASSERT_MSG( (success), "Could not load general properties panel.");
    m_notebook->AddPage(m_generalPropertiesDialog, "General");

    success = m_attributeDialog->LoadFromResource(m_notebook, attributeDialogName);
    if (!success)
    {
        wxMessageBox("Could not load the attribute dialog for this shape.", "Studio", wxICON_EXCLAMATION);
        delete m_attributeDialog;
        m_attributeDialog = NULL;
    }
    else
    {
        m_notebook->AddPage(m_attributeDialog, "Attributes");
    }

    // Try the alternative dialog (test code)
    wxString str(attributeDialogName);
    str += "1";
    m_alternativeAttributeDialog = new wxPanel;
    success = m_alternativeAttributeDialog->LoadFromResource(m_notebook, str);
    if (success)
    {
        m_notebook->AddPage(m_alternativeAttributeDialog, "Attributes (alternative)");
    }
    else
    {
        delete m_alternativeAttributeDialog;
        m_alternativeAttributeDialog = NULL;
    }

    int largeButtonWidth = 70;
    int largeButtonHeight = 22;

    wxButton* okButton = new wxButton(this, wxID_OK, "OK", wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel", wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));
    wxButton* helpButton = new wxButton(this, wxID_HELP, "Help", wxPoint(0, 0), wxSize(largeButtonWidth, largeButtonHeight));

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

    wxNode* node = m_attributeDialog->GetChildren().First();
    while (node)
    {
        wxWindow* child = (wxWindow*) node->Data();
        if (child->IsKindOf(CLASSINFO(wxChoice)))
        {
            wxChoice* choice = (wxChoice*) child;
            choice->SetSelection(0);
        }
        node = node->Next();
    }

    if (!m_alternativeAttributeDialog)
        return;

    node = m_alternativeAttributeDialog->GetChildren().First();
    while (node)
    {
        wxWindow* child = (wxWindow*) node->Data();
        if (child->IsKindOf(CLASSINFO(wxChoice)))
        {
            wxChoice* choice = (wxChoice*) child;
            choice->SetSelection(0);
        }
        node = node->Next();
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


