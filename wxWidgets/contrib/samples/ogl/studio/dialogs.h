/////////////////////////////////////////////////////////////////////////////
// Name:        contrib/samples/ogl/studio/dialogs.h
// Purpose:     Miscellaneous dialogs
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _STUDIO_DIALOGS_H_
#define _STUDIO_DIALOGS_H_

#include "wx/wx.h"
#include "wx/notebook.h"

#include "wx/ogl/ogl.h" // base header of OGL, includes and adjusts wx/deprecated/setup.h

#if wxUSE_WX_RESOURCES

/*
 * Label editing dialog (about to become obsolete)
 */

class csLabelEditingDialog: public wxDialog
{
DECLARE_CLASS(csLabelEditingDialog)
public:
    csLabelEditingDialog(wxWindow* parent);

    void SetShapeLabel(const wxString& label);
    inline wxString GetShapeLabel() const { return m_label; }

    void OnOK(wxCommandEvent& event);

protected:
    wxString    m_label;

DECLARE_EVENT_TABLE()
};

/*
 * Settings dialog (tabbed)
 */

class csSettingsDialog: public wxDialog
{
DECLARE_CLASS(csSettingsDialog)
public:
    csSettingsDialog(wxWindow* parent);

    void OnOK(wxCommandEvent& event);

    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();

protected:

    wxPanel*    m_generalSettings;
    wxPanel*    m_diagramSettings;
    wxNotebook* m_notebook;

DECLARE_EVENT_TABLE()
};

#define ID_PROPERTY_NOTEBOOK    1000
#define ID_GENERAL_SETTINGS     1002
#define ID_DIAGRAM_SETTINGS     1003

/*
 * csGeneralShapePropertiesDialog
 * Name, description etc.
 */

class csGeneralShapePropertiesDialog: public wxPanel
{
DECLARE_CLASS(csGeneralShapePropertiesDialog)
public:
    csGeneralShapePropertiesDialog();

    void SetShapeLabel(const wxString& label);
    inline wxString GetShapeLabel() const { return m_label; }

protected:
    wxString    m_label;

DECLARE_EVENT_TABLE()
};

/*
 * Shape properties dialog (tabbed)
 */

class csShapePropertiesDialog: public wxDialog
{
DECLARE_CLASS(csShapePropertiesDialog)
public:
    csShapePropertiesDialog(wxWindow* parent, const wxString& title, wxPanel* attributeDialog, const wxString& attributeDialogName);

    void OnOK(wxCommandEvent& event);

    // Set some suitable defaults in the attribute dialogs (in the first instance,
    // just set all wxChoices to the first element)
    void SetDefaults();

// Accessors
    csGeneralShapePropertiesDialog* GetGeneralPropertiesDialog() const { return m_generalPropertiesDialog; }


protected:

    // Attributes, specific to each shape
    wxPanel*                        m_attributeDialog;
    wxPanel*                        m_alternativeAttributeDialog;

   // General properties, same for each shape, e.g. name/description
    csGeneralShapePropertiesDialog* m_generalPropertiesDialog;

    wxNotebook* m_notebook;

DECLARE_EVENT_TABLE()
};

#endif // wxUSE_WX_RESOURCES

#define ID_SHAPE_PROPERTY_NOTEBOOK    1000

//// Specific attribute-editing panel classes below here

/*
 * csThinRectangleDialog
 */

class csThinRectangleDialog: public wxPanel
{
DECLARE_CLASS(csThinRectangleDialog)
public:
    csThinRectangleDialog();

DECLARE_EVENT_TABLE()
};

/*
 * csWideRectangleDialog
 */

class csWideRectangleDialog: public wxPanel
{
DECLARE_CLASS(csWideRectangleDialog)
public:
    csWideRectangleDialog();

DECLARE_EVENT_TABLE()
};

/*
 * csTriangleDialog
 */

class csTriangleDialog: public wxPanel
{
DECLARE_CLASS(csTriangleDialog)
public:
    csTriangleDialog();

DECLARE_EVENT_TABLE()
};

/*
 * csSemiCircleDialog
 */

class csSemiCircleDialog: public wxPanel
{
DECLARE_CLASS(csSemiCircleDialog)
public:
    csSemiCircleDialog();

DECLARE_EVENT_TABLE()
};

/*
 * csCircleDialog
 */

class csCircleDialog: public wxPanel
{
DECLARE_CLASS(csCircleDialog)
public:
    csCircleDialog();

DECLARE_EVENT_TABLE()
};

/*
 * csCircleShadowDialog
 */

class csCircleShadowDialog: public wxPanel
{
DECLARE_CLASS(csCircleShadowDialog)
public:
    csCircleShadowDialog();

DECLARE_EVENT_TABLE()
};

/*
 * csOctagonDialog
 */

class csOctagonDialog: public wxPanel
{
DECLARE_CLASS(csOctagonDialog)
public:
    csOctagonDialog();

DECLARE_EVENT_TABLE()
};

/*
 * csGroupDialog
 */

class csGroupDialog: public wxPanel
{
DECLARE_CLASS(csGroupDialog)
public:
    csGroupDialog();

DECLARE_EVENT_TABLE()
};

/*
 * csTextBoxDialog
 */

class csTextBoxDialog: public wxPanel
{
DECLARE_CLASS(csTextBoxDialog)
public:
    csTextBoxDialog();

DECLARE_EVENT_TABLE()
};

#endif
    // _STUDIO_DIALOGS_H_
