/////////////////////////////////////////////////////////////////////////////
// Name:        configtoolview.h
// Purpose:     View class
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _CT_CONFIGTOOLVIEW_H_
#define _CT_CONFIGTOOLVIEW_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "configtoolview.cpp"
#endif

#include "wx/docview.h"
#include "wx/treectrl.h"
#include "wx/fdrepdlg.h"
#include "configitem.h"

class ctConfigTreeCtrl;
class WXDLLEXPORT wxNotebookEvent;

/*
 * ctConfigToolView
 */

class ctConfigItem;
class ctConfigToolView: public wxView
{
    DECLARE_DYNAMIC_CLASS(ctConfigToolView)
public:
    ctConfigToolView(){};
    ~ctConfigToolView(){};

//// Overrides

    bool OnCreate(wxDocument *doc, long flags);
    void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = true);
    void OnChangeFilename();

//// Operations

    /// Gets the tree item in sync with the item.
    void SyncItem(ctConfigTreeCtrl* treeControl, ctConfigItem* item);

    /// Add item and its children to the tree
    void AddItems(ctConfigTreeCtrl* treeControl, ctConfigItem* item);

    /// Clicked an icon
    void OnIconLeftDown(ctConfigTreeCtrl* treeControl, ctConfigItem* item);

    /// Add an item
    void AddItem(ctConfigType type, const wxString& msg);

    /// Regenerate setup.h and configure command
    void RegenerateSetup();

//// Accessors

    /// Returns the selected config item, if any.
    ctConfigItem* GetSelection();

//// Event handlers

    /// General disabler
    void OnUpdateDisable(wxUpdateUIEvent& event);

    /// Enable add item menu items
    void OnUpdateAddItem(wxUpdateUIEvent& event);

    /// Add a checkbox item
    void OnAddCheckBoxItem(wxCommandEvent& event);

    /// Add a radiobutton item
    void OnAddRadioButtonItem(wxCommandEvent& event);

    /// Add a group item
    void OnAddGroupItem(wxCommandEvent& event);

    /// Add a check group item
    void OnAddCheckGroupItem(wxCommandEvent& event);

    /// Add a radio group item
    void OnAddRadioGroupItem(wxCommandEvent& event);

    /// Add a string item
    void OnAddStringItem(wxCommandEvent& event);

    /// Delete an item
    void OnDeleteItem(wxCommandEvent& event);

    /// Rename an item
    void OnRenameItem(wxCommandEvent& event);

    /// Copy an item to the clipboard
    void OnCopy(wxCommandEvent& event);

    /// Copy an item to the clipboard and cut the item
    void OnCut(wxCommandEvent& event);

    /// Paste an item from the clipboard to the tree
    void OnPaste(wxCommandEvent& event);

    /// Item help
    void OnItemHelp(wxCommandEvent& event);

    /// Update for copy command
    void OnUpdateCopy(wxUpdateUIEvent& event);

    /// Update for cut
    void OnUpdateCut(wxUpdateUIEvent& event);

    /// Update for paste
    void OnUpdatePaste(wxUpdateUIEvent& event);

    /// Update for item help
    void OnUpdateItemHelp(wxUpdateUIEvent& event);

    // Context menu events

    /// Copy an item to the clipboard
    void OnContextCopy(wxCommandEvent& event);

    /// Copy an item to the clipboard and cut the item
    void OnContextCut(wxCommandEvent& event);

    /// Paste an item from the clipboard to the tree
    void OnContextPasteBefore(wxCommandEvent& event);

    /// Paste an item from the clipboard to the tree
    void OnContextPasteAfter(wxCommandEvent& event);

    /// Paste an item from the clipboard to the tree
    void OnContextPasteAsChild(wxCommandEvent& event);

    /// Copy an item to the clipboard
    void OnUpdateContextCopy(wxUpdateUIEvent& event);

    /// Copy an item to the clipboard and cut the item
    void OnUpdateContextCut(wxUpdateUIEvent& event);

    /// Paste an item from the clipboard to the tree
    void OnUpdateContextPasteBefore(wxUpdateUIEvent& event);

    /// Paste an item from the clipboard to the tree
    void OnUpdateContextPasteAfter(wxUpdateUIEvent& event);

    /// Paste an item from the clipboard to the tree
    void OnUpdateContextPasteAsChild(wxUpdateUIEvent& event);

    // Custom property events

    /// Add a custom property
    void OnAddCustomProperty(wxCommandEvent& event);

    /// Edit a custom property
    void OnEditCustomProperty(wxCommandEvent& event);

    /// Delete a custom property
    void OnDeleteCustomProperty(wxCommandEvent& event);

    /// Add a custom property: update event
    void OnUpdateAddCustomProperty(wxUpdateUIEvent& event);

    /// Edit a custom property: update event
    void OnUpdateEditCustomProperty(wxUpdateUIEvent& event);

    /// Delete a custom property: update event
    void OnUpdateDeleteCustomProperty(wxUpdateUIEvent& event);

    // Notebook events

    /// Regenerate if selected a tab
    void OnTabSelect(wxNotebookEvent& event);

    /// Saving setup file
    void OnSaveSetupFile(wxCommandEvent& event);

    /// Save configure command file
    void OnSaveConfigureCommand(wxCommandEvent& event);

    /// Saving setup file update handler
    void OnUpdateSaveSetupFile(wxUpdateUIEvent& event);

    /// Save configure command file update handler
    void OnUpdateSaveConfigureCommand(wxUpdateUIEvent& event);

    // Find

    /// Find text
    void OnFind(wxCommandEvent& event);

    /// Update find text
    void OnUpdateFind(wxUpdateUIEvent& event);

    // Go (now: save setup.h or configurewx.sh. later: invoke configure/make)

    /// Save default file type
    void OnGo(wxCommandEvent& event);

    /// Update
    void OnUpdateGo(wxUpdateUIEvent& event);

DECLARE_EVENT_TABLE()

protected:
};

/*
 * ctConfigToolHint
 *
 * Hint to pass to UpdateAllViews
 *
 */

// Update hint symbols
#define ctNoHint                0
#define ctAllSaved              1
#define ctClear                 2
#define ctValueChanged          3
#define ctSelChanged            4
#define ctFilenameChanged       5
#define ctInitialUpdate         6

class ctConfigItem;
class ctConfigToolHint: public wxObject
{
public:
    ctConfigToolHint(ctConfigItem* item, int op) { m_item = item; m_op = op; }

    ctConfigItem*   m_item;
    int             m_op;
};

/*
 * ctFindReplaceDialog
 */

class ctFindReplaceDialog: public wxFindReplaceDialog
{
public:
    // constructors and destructors
    ctFindReplaceDialog( wxWindow* parent, const wxString& title,
        long style = 0 );

    void OnFind(wxFindDialogEvent& event);
    void OnClose(wxFindDialogEvent& event);

    // If wrap is true, go back to the beginning if at the end of the
    // document.
    bool DoFind(const wxString& textToFind, bool matchCase, bool wholeWord, bool wrap = true);

    ctConfigItem* FindNextItem(ctConfigToolDoc* doc,
                                                      ctConfigItem* item,
                                                      const wxString& text,
                                                      bool matchCase,
                                                      bool matchWordOnly,
                                                      bool wrap,
                                                      bool skipFirst);
    static wxFindReplaceData    sm_findData;
    static wxString             sm_currentItem; // card name

private:
    DECLARE_EVENT_TABLE()
};

#endif
        // _CT_CONFIGTOOLVIEW_H_
