/////////////////////////////////////////////////////////////////////////////
// Name:        configtooldoc.h
// Purpose:     Document class
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _CT_CONFIGTOOLDOC_H_
#define _CT_CONFIGTOOLDOC_H_

#ifdef __GNUG__
#pragma interface "configtooldoc.cpp"
#endif

#include "wx/docview.h"
#include "wx/cmdproc.h"

#include "configitem.h"

class wxSimpleHtmlTag;

/*!
 * ctConfigToolDoc
 */

class ctConfigToolDoc: public wxDocument
{
    DECLARE_DYNAMIC_CLASS(ctConfigToolDoc)
public:
    ctConfigToolDoc();
    ~ctConfigToolDoc();

//// Overrides
    virtual bool OnCreate(const wxString& path, long flags);
    virtual bool OnOpenDocument(const wxString& filename);
    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnNewDocument() { return TRUE; }
    virtual bool OnCloseDocument() ;
    virtual bool Save(); // Overridden only to correct bug in wxWindows, docview.cpp

//// Accessors

    /// Returns the top item.
    ctConfigItem* GetTopItem() const { return m_topItem; }

    /// Sets the top item.
    void SetTopItem(ctConfigItem* item) { m_topItem = item; }

    /// Returns the clipboard  item.
    ctConfigItem* GetClipboardItem() const { return m_clipboardItem; }

    /// Sets the clipboard item.
    void SetClipboardItem(ctConfigItem* item) ;

    /// Clears the clipboard item.
    void ClearClipboard() ;

    /// Gets the current framework directory
    wxString GetFrameworkDir(bool makeUnix);

//// Operations

    /// Add items
    void AddItems();

    /// Delete items
    void DeleteItems();

    /// Save the settings file
    bool DoSave(const wxString& filename);

    /// Recursive helper function for file saving
    bool DoSave(ctConfigItem* item, wxOutputStream& stream, int indent);

    /// Open the settings file
    bool DoOpen(const wxString& filename);

    /// Helper for file opening.
    bool DoOpen(wxSimpleHtmlTag* tag, ctConfigItem* parent);

    /// Refresh dependencies
    void RefreshDependencies();
    void RefreshDependencies(ctConfigItem* item);

    /// Clear dependencies
    void ClearDependencies(ctConfigItem* item);

    /// Generate the text of a setup.h
    wxString GenerateSetup();
        
    /// Helper function
    void GenerateSetup(ctConfigItem* item, wxString& str);
        
    /// Generate a configure command
    wxString GenerateConfigureCommand();

    /// Helper function
    void GenerateConfigureCommand(ctConfigItem* item, wxString& str);

    /// Finds the next item in the tree
    ctConfigItem* FindNextItem(ctConfigItem* item, bool wrap);
        
    /// Finds the next sibling in the tree
    ctConfigItem* FindNextSibling(ctConfigItem* item);
        
protected:
    ctConfigItem*   m_topItem;
    ctConfigItem*   m_clipboardItem;
};


/*!
 * Implements a document editing command.
 * We only need to store one state at a time,
 * since we don't have (or need) multiple selection.
 */

#define ctCMD_NEW_ELEMENT           1
#define ctCMD_PASTE                 2
#define ctCMD_CUT                   3
#define ctCMD_APPLY_PROPERTY        4

class ctConfigCommand: public wxCommand
{
public:
    ctConfigCommand(const wxString& name, int cmdId,
        ctConfigItem* activeState, ctConfigItem* savedState,
        ctConfigItem* parent = NULL, ctConfigItem* insertBefore = NULL,
        bool ignoreFirstTime = FALSE);
    ctConfigCommand(const wxString& name, int cmdId,
        ctConfigItem* activeState, ctProperties* properties,
        bool ignoreFirstTime = FALSE);
    ~ctConfigCommand();

    bool Do();
    bool Undo();
    bool DoAndUndo(bool doCmd); // Combine Do and Undo into one
    
protected:
    ctConfigItem*   m_activeState;
    ctConfigItem*   m_savedState;
    ctProperties*   m_properties;
    bool            m_ignoreThis; // Ignore 1st Do because we already did it
    int             m_cmdId;
    ctConfigItem*   m_parent;
    ctConfigItem*   m_insertBefore;
};


#endif
        // _CT_CONFIGTOOLDOC_H_
