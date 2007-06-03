/////////////////////////////////////////////////////////////////////////////
// Name:        classlist.h
// Purpose:     wxClassListDialog definition
// Author:      Francesco Montorsi
// Modified by: 
// Created:     03/06/2007 14:49:55
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Francesco Montorsi
// Licence:     wxWidgets license
/////////////////////////////////////////////////////////////////////////////

#ifndef _CLASSLIST_H_
#define _CLASSLIST_H_


// ----------------------------------------------------------------------------
// includes
// ----------------------------------------------------------------------------

////@begin includes
#include "wx/choicebk.h"
#include "wx/treectrl.h"
////@end includes

////@begin forward declarations
class wxTreeCtrl;
////@end forward declarations


// ----------------------------------------------------------------------------
// IDs
// ----------------------------------------------------------------------------

////@begin control identifiers
#define ID_LISTMODE 10006
#define ID_LISTBOX 10003
#define ID_PANEL 10007
#define ID_TREECTRL 10008
#define ID_TEXTCTRL 10004
#define SYMBOL_WXCLASSLISTDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_WXCLASSLISTDIALOG_TITLE _("wxWidgets class list")
#define SYMBOL_WXCLASSLISTDIALOG_IDNAME wxID_ANY
#define SYMBOL_WXCLASSLISTDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_WXCLASSLISTDIALOG_POSITION wxDefaultPosition
////@end control identifiers


// ----------------------------------------------------------------------------
// wxClassListDialog
// ----------------------------------------------------------------------------

class wxClassListDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( wxClassListDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxClassListDialog();
    wxClassListDialog( wxWindow* parent, wxWindowID id = SYMBOL_WXCLASSLISTDIALOG_IDNAME, 
                       const wxString& caption = SYMBOL_WXCLASSLISTDIALOG_TITLE, 
                       const wxPoint& pos = SYMBOL_WXCLASSLISTDIALOG_POSITION, 
                       const wxSize& size = SYMBOL_WXCLASSLISTDIALOG_SIZE, 
                       long style = SYMBOL_WXCLASSLISTDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXCLASSLISTDIALOG_IDNAME, 
                 const wxString& caption = SYMBOL_WXCLASSLISTDIALOG_TITLE, 
                 const wxPoint& pos = SYMBOL_WXCLASSLISTDIALOG_POSITION, 
                 const wxSize& size = SYMBOL_WXCLASSLISTDIALOG_SIZE, 
                 long style = SYMBOL_WXCLASSLISTDIALOG_STYLE );

    /// Destructor
    ~wxClassListDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    void InitControls();
    int AddClassesWithParent(const wxClassInfo *parent, const wxTreeItemId &id);

////@begin wxClassListDialog event handler declarations

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX
    void OnListboxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_TREE_SEL_CHANGED event handler for ID_TREECTRL
    void OnTreectrlSelChanged( wxTreeEvent& event );

////@end wxClassListDialog event handler declarations

////@begin wxClassListDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxClassListDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

protected:
////@begin wxClassListDialog member variables
    wxListBox* m_pListBox;
    wxTreeCtrl* m_pTreeCtrl;
    wxTextCtrl* m_pTextCtrl;
////@end wxClassListDialog member variables
};

#endif
    // _CLASSLIST_H_
