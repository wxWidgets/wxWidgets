////////////////////////////////////////////////////
// Name:        classlist.h
// Purpose:     ClassListDialog definition
// Author:      Francesco Montorsi
// Modified by: 
// Created:     03/06/2007 14:49:55
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Francesco Montorsi
// Licence:     wxWidgets license
////////////////////////////////////////////////////

#ifndef _CLASSLIST_H_
#define _CLASSLIST_H_


// ----------------------------------------------------------------------------
// includes
// ----------------------------------------------------------------------------

#include "wx/choicebk.h"
#include "wx/treectrl.h"

// ----------------------------------------------------------------------------
// IDs
// ----------------------------------------------------------------------------

#define ID_LISTMODE 10006
#define ID_LISTBOX 10003
#define ID_PANEL 10007
#define ID_TREECTRL 10008
#define ID_TEXTCTRL 10004
#define ID_SHOW_ONLY_XTI 10005
#define ID_SHOW_PROPERTIES_RECURSIVELY 10002
#define SYMBOL_CLASSLISTDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_CLASSLISTDIALOG_TITLE _("wxWidgets class list")
#define SYMBOL_CLASSLISTDIALOG_IDNAME wxID_ANY
#define SYMBOL_CLASSLISTDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_CLASSLISTDIALOG_POSITION wxDefaultPosition


// ----------------------------------------------------------------------------
// ClassListDialog
// ----------------------------------------------------------------------------

class ClassListDialog: public wxDialog
{    
    // we explicitly don't want to use the following macro:
    //     DECLARE_DYNAMIC_CLASS( ClassListDialog )
    // as otherwise the ClassListDialog class would appear in the list
    // shown by this dialog!
    DECLARE_EVENT_TABLE()

public:
    // Constructors
    ClassListDialog();
    ClassListDialog( wxWindow* parent, wxWindowID id = SYMBOL_CLASSLISTDIALOG_IDNAME, 
                       const wxString& caption = SYMBOL_CLASSLISTDIALOG_TITLE, 
                       const wxPoint& pos = SYMBOL_CLASSLISTDIALOG_POSITION, 
                       const wxSize& size = SYMBOL_CLASSLISTDIALOG_SIZE, 
                       long style = SYMBOL_CLASSLISTDIALOG_STYLE );

    // Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CLASSLISTDIALOG_IDNAME, 
                 const wxString& caption = SYMBOL_CLASSLISTDIALOG_TITLE, 
                 const wxPoint& pos = SYMBOL_CLASSLISTDIALOG_POSITION, 
                 const wxSize& size = SYMBOL_CLASSLISTDIALOG_SIZE, 
                 long style = SYMBOL_CLASSLISTDIALOG_STYLE );

    // Destructor
    ~ClassListDialog();

public:     // misc

    void Init();
    void CreateControls();
    void InitControls();

    void UpdateClassInfo(const wxString &itemName);
    void UpdateFilterText();
    bool IsToDiscard(const wxString &classname) const;

    int AddClassesWithParent(const wxClassInfo *parent, const wxTreeItemId &id);

public:     // event handlers

    void OnListboxSelected( wxCommandEvent& event );
    void OnTreectrlSelChanged( wxTreeEvent& event );
    void OnChoiceBookPageChange( wxChoicebookEvent& event );
    void OnShowOnlyXTICheckbox( wxCommandEvent& event );
    void OnShowRecursiveInfoCheckbox( wxCommandEvent& event );

    // Should we show tooltips?
    static bool ShowToolTips();

protected:
    wxChoicebook* m_pChoiceBook;
    wxStaticText* m_pClassCountText;
    wxListBox* m_pRawListBox;
    wxTreeCtrl* m_pParentTreeCtrl;
    wxListBox* m_pSizeListBox;
    wxTextCtrl* m_pTextCtrl;

    int m_nTotalCount;   // number of classes in wxXTI system
    int m_nCount;       // number of shown classes
};

#endif
    // _CLASSLIST_H_
