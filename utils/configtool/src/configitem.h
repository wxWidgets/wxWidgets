/////////////////////////////////////////////////////////////////////////////
// Name:        configitem.h
// Purpose:     wxWidgets Configuration Tool config item class
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-03
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _CT_CONFIGITEM_H_
#define _CT_CONFIGITEM_H_

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "property.h"

class ctConfigToolDoc;

/*!
 * The type of config item
 */

enum ctConfigType
{
    ctTypeUnknown,
    ctTypeGroup,        // A group with no checkbox
    ctTypeCheckGroup,   // A group that can be switched on/off (check)
    ctTypeRadioGroup,   // A group that can be switched on/off (radio)
    ctTypeString,       // An option with a string value
    ctTypeInteger,      // An option with an integer value
    ctTypeBoolCheck,    // An on/off option
    ctTypeBoolRadio     // An on/off mutually exclusive option
};

/*!
 * ctConfigItem represents a configuration setting.
 * Each setting has a number of properties, some of
 * which may be specific to a particular kind of
 * setting, so we make it quite generic and extensible
 * by using a property list.
 */

class ctConfigItem: public wxObject
{
public:
    /// Ctor and dtor
    ctConfigItem(ctConfigItem* parent, ctConfigType type, const wxString& name);
    ctConfigItem();
    virtual ~ctConfigItem();

    /// Copy constructor.
    ctConfigItem(const ctConfigItem& item) : wxObject()
    {
        (*this) = item;
    }

/// Operations

    /// Assignment operator.
    void operator= (const ctConfigItem& item);

    /// Create a clone
    ctConfigItem* Clone()
    {
        ctConfigItem* item = new ctConfigItem;
        *item = *this;
        return item;
    }

    /// Create a clone of this and children
    ctConfigItem* DeepClone();

    /// Do additional actions to apply the property to the internal
    /// representation.
    void ApplyProperty(ctProperty* prop, const wxVariant& oldValue);

    /// Clear children
    void Clear();

    /// Add a child
    void AddChild(ctConfigItem* item);

    /// Remove (but don't delete) a child
    void RemoveChild(ctConfigItem* item);

    /// Initialise standard properties
    void InitProperties();

    /// Convert string containing config item names to
    /// an array of config item names
    static void StringToArray(const wxString& items, wxArrayString& itemsArray);

    /// Convert array containing config item names to
    /// a string
    static void ArrayToString(const wxArrayString& itemsArray, wxString& items);

    /// Populate a list of items found in the string.
    static void StringToItems(ctConfigItem* topItem, const wxString& items, wxList& list);

    /// Find an item in this hierarchy
    ctConfigItem* FindItem(const wxString& name);

    /// Find the next sibling
    ctConfigItem* FindNextSibling();

    /// Find the previous sibling
    ctConfigItem* FindPreviousSibling();

    /// Sync appearance
    void Sync();

    /// Detach: remove from parent, and remove tree items
    void Detach();

    /// Attach: insert before the given position
    void Attach(ctConfigItem* parent, ctConfigItem* insertbefore);

    /// Hide from tree: make sure tree deletions won't delete
    /// the config items
    void DetachFromTree();

    /// Evaluate the depends-on properties:
    /// if any of the depends-on items are disabled,
    /// then this one is disabled (and inactive).
    void EvaluateDependencies();

    /// Propagate a change in enabled/disabled status
    void PropagateChange(wxList& considered);

    /// Process radio button selection
    void PropagateRadioButton(wxList& considered);

    // An item is in the active context if:
    // The context field is empty; or
    // The context field contains a symbol that is currently enabled.
    bool IsInActiveContext();

/// Accessors

    /// Returns the name property.
    wxString GetName() const { return GetPropertyString(wxT("name")); }

    /// Sets the name property.
    void SetName(const wxString& name ) ;

    /// Returns the value property.
    wxVariant GetValue() const { return m_properties.FindPropertyValue(wxT("value")); }

    /// Sets the value property.
    void SetValue(const wxVariant& value ) ;

    /// Returns the string for the given property.
    wxString GetPropertyString(const wxString& propName) const { return m_properties.FindPropertyValueString(propName); }

    /// Sets the string for the given property.
    void SetPropertyString(const wxString& propName, const wxString& value) { m_properties.SetProperty(propName, value); }

    /// Can we edit this property?
    bool CanEditProperty(const wxString& propName) const ;

    /// Returns the list of properties for
    /// this item.
    ctProperties& GetProperties() { return m_properties; }

    /// Set the default property.
    void SetDefaultProperty(const wxString& defaultProp) { m_defaultProperty = defaultProp; }

    /// Get the default property.
    wxString GetDefaultProperty() const { return m_defaultProperty; }

    /// Is this item modified?
    bool IsModified() const { return m_modified; }

    /// Mark this as modified.
    void Modify(bool modified = true) { m_modified = modified; }

    /// Is this item enabled? (checked/unchecked)
    bool IsEnabled() const { return m_enabled; }

    /// Enable or disable (check/uncheck)
    void Enable(bool enable = true) { m_enabled = enable; }

    /// Is this item active? (sensitive to user input)
    bool IsActive() const { return m_active; }

    /// Make this (in)active
    void SetActive(bool active = true) { m_active = active; }

    /// Set the type
    void SetType(ctConfigType type) { m_type = type; }

    // Get the type
    ctConfigType GetType() const { return m_type; }

    /// Set the tree item id
    void SetTreeItem(wxTreeItemId id) { m_treeItemId = id; }

    // Get the type
    wxTreeItemId GetTreeItemId() const { return m_treeItemId ; }

    /// Get the list of children
    wxList& GetChildren() { return m_children; }

    /// Get the nth child
    ctConfigItem* GetChild(int n) const;

    /// Get the child count
    int GetChildCount() const;

    /// Get the list of dependents
    wxList& GetDependents() { return m_dependents; }

    /// Get the parent
    ctConfigItem* GetParent() const { return m_parent; }

    /// Set the parent
    void SetParent(ctConfigItem* parent) { m_parent = parent; }

    /// Get the associated document (currently, assumes
    /// there's only ever one document active)
    ctConfigToolDoc* GetDocument() ;

    /// Can have children?
    bool CanHaveChildren() const;

    /// Get description, which may be dynamically
    /// generated depending on the property.
    wxString GetDescription(ctProperty* property);

    /// Get the title for the property editor
    wxString GetTitle();

protected:

    /// The properties for this item.
    ctProperties    m_properties;

    /// The default property, from the point of
    /// of double-clicking the config item.
    wxString        m_defaultProperty;

    /// Whether modified
    bool            m_modified;

    /// The type of the config item
    ctConfigType    m_type;

    /// The corresponding tree item
    wxTreeItemId    m_treeItemId;

    /// Is this option enabled? (checked/unchecked)
    bool            m_enabled;

    /// Is this option active? (i.e. sensitive to user input)
    bool            m_active;

    /// The list of children.
    wxList          m_children;

    /// The list of items that are dependent upon
    // this one. This is refreshed when the configuration
    // structurally changes, and is not saved to file.
    wxList          m_dependents;

    /// The parent config item
    ctConfigItem*   m_parent;

DECLARE_CLASS(ctConfigItem)
};

#endif
// _CT_CONFIGITEM_H_
