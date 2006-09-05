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

#include "wx/docview.h"
#include "wx/cmdproc.h"

#include "configitem.h"

class wxSimpleHtmlTag;
class ctConfiguration;

/*!
 * ctConfigToolDoc
 */

class ctConfigToolDoc: public wxDocument
{
    DECLARE_DYNAMIC_CLASS(ctConfigToolDoc)
public:
    ctConfigToolDoc();
    virtual ~ctConfigToolDoc();

//// Overrides
    virtual bool OnCreate(const wxString& path, long flags);
    virtual bool OnOpenDocument(const wxString& filename);
    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnNewDocument() { return true; }
    virtual bool OnCloseDocument() ;
    virtual bool Save(); // Overridden only to correct bug in wxWidgets, docview.cpp

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
 * ctConfiguration is a configuration or a place-holder node within the
 * hierarchy of configurations.
 */

class ctConfiguration: public wxObject
{
public:
    /// Ctor and dtor
    ctConfiguration(ctConfiguration* parent, const wxString& name);
    ctConfiguration();
    virtual ~ctConfiguration();

    /// Copy constructor.
    ctConfiguration(const ctConfiguration& configuration) : wxObject()
    {
        (*this) = configuration;
    }

/// Operations

    /// Assignment operator.
    void operator= (const ctConfiguration& configuration);

    /// Create a clone
    ctConfiguration* Clone()
    {
        ctConfiguration* configuration = new ctConfiguration;
        *configuration = *this;
        return configuration;
    }

    /// Create a clone of this and children
    ctConfiguration* DeepClone();

    /// Clear children
    void Clear();

    /// Add a child
    void AddChild(ctConfiguration* config);

    /// Remove (but don't delete) a child
    void RemoveChild(ctConfiguration* config);

    /// Find an item in this hierarchy
    ctConfiguration* FindConfiguration(const wxString& name);

    /// Find the next sibling
    ctConfiguration* FindNextSibling();

    /// Find the previous sibling
    ctConfiguration* FindPreviousSibling();

    /// Detach: remove from parent, and remove tree items
    void Detach();

    /// Attach: insert before the given position
    void Attach(ctConfiguration* parent, ctConfiguration* insertbefore);

    void DetachFromTree();

/// Accessors

    /// Returns the top-level item.
    ctConfigItem* GetTopItem() const { return m_topItem; }

    /// Sets the top-level item.
    void SetTopItem(ctConfigItem* item) { m_topItem = item; }

    /// Returns the name.
    wxString GetName() const { return m_name; }

    /// Sets the name.
    void SetName(const wxString& name ) { m_name = name; }

    /// Get description.
    wxString GetDescription() const { return m_description; }

    /// Set description.
    void SetDescription(const wxString& descr) { m_description = descr; }

    /// Set the tree item id
    void SetTreeItem(wxTreeItemId id) { m_treeItemId = id; }

    // Get the type
    wxTreeItemId GetTreeItemId() const { return m_treeItemId ; }

    /// Get the list of children
    wxList& GetChildren() { return m_children; }

    /// Get the nth child
    ctConfiguration* GetChild(int n) const;

    /// Get the child count
    int GetChildCount() const;

    /// Get the parent
    ctConfiguration* GetParent() const { return m_parent; }

    /// Set the parent
    void SetParent(ctConfiguration* parent) { m_parent = parent; }

    /// Get the associated document (currently, assumes
    /// there's only ever one document active)
    ctConfigToolDoc* GetDocument() ;

protected:

    /// The corresponding tree item
    wxTreeItemId    m_treeItemId;

    /// The list of children.
    wxList          m_children;

    /// The parent config item
    ctConfiguration* m_parent;

    /// The name
    wxString        m_name;

    /// The description
    wxString        m_description;

    /// The top-level item of this description, if any
    ctConfigItem*   m_topItem;

DECLARE_CLASS(ctConfiguration)
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
        bool ignoreFirstTime = false);
    ctConfigCommand(const wxString& name, int cmdId,
        ctConfigItem* activeState, ctProperties* properties,
        bool ignoreFirstTime = false);
    virtual ~ctConfigCommand();

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
