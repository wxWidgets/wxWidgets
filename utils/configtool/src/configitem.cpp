/////////////////////////////////////////////////////////////////////////////
// Name:        configitem.cpp
// Purpose:     wxWindows Configuration Tool config item class
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-03
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "configitem.h"
#endif

// Includes other headers for precompiled compilation
#include "wx/wx.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/tokenzr.h"

#include "configitem.h"
#include "configtree.h"
#include "configtooldoc.h"
#include "configtoolview.h"
#include "wxconfigtool.h"
#include "mainframe.h"

IMPLEMENT_CLASS(ctConfigItem, wxObject)

ctConfigItem::ctConfigItem()
{
    m_modified = FALSE;
    m_type = ctTypeBoolCheck;
    m_treeItemId = wxTreeItemId();
    m_enabled = TRUE;
    m_parent = NULL;
    m_active = TRUE;
}

ctConfigItem::ctConfigItem(ctConfigItem* parent, ctConfigType type, const wxString& name)
{
    m_modified = FALSE;
    m_type = type;
    m_treeItemId = wxTreeItemId();
    m_enabled = FALSE;
    m_active = TRUE;
    SetName(name);
    m_parent = parent;
    if (parent)
        parent->AddChild(this);
}

ctConfigItem::~ctConfigItem()
{
    ctConfigTreeCtrl* treeCtrl = wxGetApp().GetMainFrame()->GetConfigTreeCtrl();
    if (m_treeItemId.IsOk() && treeCtrl)
    {
        ctTreeItemData* data = (ctTreeItemData*) treeCtrl->GetItemData(m_treeItemId);
        if (data)
            data->SetConfigItem(NULL);
    }
    if (GetParent())
        GetParent()->RemoveChild(this);
    else
    {
        if (wxGetApp().GetMainFrame()->GetDocument() &&
            wxGetApp().GetMainFrame()->GetDocument()->GetTopItem() == this)
            wxGetApp().GetMainFrame()->GetDocument()->SetTopItem(NULL);
    }
    
    Clear();
}

/// Can we edit this property?
bool ctConfigItem::CanEditProperty(const wxString& propName) const
{
    ctProperty* prop = m_properties.FindProperty(propName);
    if (prop)
        return !prop->GetReadOnly();
    else
        return FALSE;
}

/// Assignment operator.
void ctConfigItem::operator= (const ctConfigItem& item)
{
    m_properties = item.m_properties;
    m_modified = item.m_modified;
    m_defaultProperty = item.m_defaultProperty;
    m_type = item.m_type;
    m_enabled = item.m_enabled;
    m_active = item.m_active;
}

/// Sets the name property.
void ctConfigItem::SetName(const wxString& name )
{
    m_properties.SetProperty(wxT("name"), name);
}

/// Clear children
void ctConfigItem::Clear()
{
    wxNode* node = m_children.GetFirst();
    while (node)
    {
        wxNode* next = node->GetNext();
        ctConfigItem* child = (ctConfigItem*) node->GetData();

        // This should delete 'node' too, assuming
        // child's m_parent points to 'this'. If not,
        // it'll be cleaned up by m_children.Clear().
        delete child;

        node = next;
    }
    m_children.Clear();
}

// Get the nth child
ctConfigItem* ctConfigItem::GetChild(int n) const
{
    wxASSERT ( n < GetChildCount() && n > -1 );

    if ( n < GetChildCount() && n > -1 )
    {
        ctConfigItem* child = wxDynamicCast(m_children.Nth(n)->Data(), ctConfigItem);
        return child;
    }
    else
        return NULL;
}

// Get the child count
int ctConfigItem::GetChildCount() const
{
    return m_children.GetCount();
}

/// Add a child
void ctConfigItem::AddChild(ctConfigItem* item)
{
    m_children.Append(item);
    item->SetParent(this);
}

/// Remove (but don't delete) a child
void ctConfigItem::RemoveChild(ctConfigItem* item)
{
    m_children.DeleteObject(item);
    item->SetParent(NULL);
}

/// Initialise standard properties
void ctConfigItem::InitProperties()
{
    ctProperty* prop = m_properties.FindProperty(wxT("name"));
    if (!prop)
    {
        prop = new ctProperty;
        m_properties.AddProperty(prop);
    }
    prop->SetDescription(_("<B>Name</B><P> The name of the configuration setting."));
    prop->SetReadOnly(TRUE);

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Description</B><P> The setting description."),
        wxVariant(wxT(""), wxT("description")),
        wxT("multiline")));

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Default-state</B><P> The default state."),
        wxVariant((bool) TRUE, wxT("default-state")),
        wxT("bool")));

    if (GetType() == ctTypeString)
    {
        m_properties.AddProperty(
            new ctProperty(
            wxT("<B>Default-value</B><P> The default value."),
            wxVariant((bool) TRUE, wxT("default-value")),
            wxT("")));
    }
    else if (GetType() == ctTypeInteger)
    {
        m_properties.AddProperty(
            new ctProperty(
            wxT("<B>Default-value</B><P> The default value."),
            wxVariant((long) 0, wxT("default-value")),
            wxT("")));
    }
    
    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Requires</B><P> When any of the given settings are 0, this setting <I>must</I> be 0. Taking wxUSE_ZIPSTREAM as an example:<P>  If wxUSE_STREAMS is 0, then wxUSE_ZIPSTREAM must be 0.<BR>If wxUSE_STREAMS is 1, then wxUSE_ZIPSTREAM may be 0 or 1."),
        wxVariant(wxT(""), wxT("requires")),
        wxT("configitems")));

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Precludes</B><P> When any of these settings are 1, this setting <I>must</I> be 0. Taking wxUSE_ODBC as an example:<P>  If wxUSE_UNICODE is 1, then wxUSE_ODBC must be 0.<BR>If wxUSE_UNICODE is 0, then wxUSE_ODBC may be 0 or 1."),
        wxVariant(wxT(""), wxT("precludes")),
        wxT("configitems")));

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Enabled-if</B><P> When any of these settings are 1, this setting <I>must</I> be 1."),
        wxVariant(wxT(""), wxT("enabled-if")),
        wxT("configitems")));

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Enabled-if-not</B><P> When any of these settings are 0, this setting <I>must</I> be 1. Taking wxUSE_TOOLBAR_SIMPLE as an example:<P>If wxUSE_TOOLBAR_NATIVE is 0, wxUSE_TOOLBAR_SIMPLE must be 1.<BR>If wxUSE_TOOLBAR_NATIVE is 1, wxUSE_TOOLBAR_SIMPLE may be 0 or 1."),
        wxVariant(wxT(""), wxT("enabled-if-not")),
        wxT("configitems")));

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Exclusivity</B><P> The settings that are mutually exclusive with this one."),
        wxVariant(wxT(""), wxT("exclusivity")),
        wxT("configitems")));

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Context</B><P> A list of symbols (config settings), at least one of which must be enabled for this item to participate in dependency rules.<P>\nIf empty, this item will always be used in dependency rules.<P>\nMostly this will be used to specify the applicable platforms, but it can contain other symbols, for example compilers."),
        wxVariant(wxT(""), wxT("context")),
        wxT("configitems")));

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Configure-command</B><P> Configure command to generate if this is on."),
        wxVariant(wxT(""), wxT("configure-command")),
        wxT("multiline")));

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Help-topic</B><P> The help topic in the wxWindows manual for this component or setting."),
        wxVariant(wxT(""), wxT("help-topic")),
        wxT("multiline")));

    m_properties.AddProperty(
        new ctProperty(
        wxT("<B>Notes</B><P> User notes."),
        wxVariant(wxT(""), wxT("notes")),
        wxT("multiline")));

    m_defaultProperty = wxT("description");
}

/// Do additional actions to apply the property to the internal
/// representation.
void ctConfigItem::ApplyProperty(ctProperty* prop, const wxVariant& oldValue)
{
    ctConfigToolDoc* doc = GetDocument();
    bool oldModified = doc->IsModified();
    doc->Modify(TRUE);

    wxString name = prop->GetName();
    if (name == wxT("requires") ||
        name == wxT("precludes") ||
        name == wxT("enabled-if") ||
        name == wxT("enabled-if-not") ||
        name == wxT("context"))
    {
        doc->RefreshDependencies();
    }
    if (doc && doc->GetFirstView() && oldModified != doc->IsModified())
        ((ctConfigToolView*)doc->GetFirstView())->OnChangeFilename();
}

/// Get the associated document (currently, assumes
/// there's only ever one document active)
ctConfigToolDoc* ctConfigItem::GetDocument()
{
    ctConfigToolDoc* doc = wxGetApp().GetMainFrame()->GetDocument();
    return doc;
}

/// Convert string containing config item names to
/// an array of config item names
void ctConfigItem::StringToArray(const wxString& items, wxArrayString& itemsArray)
{
    wxStringTokenizer tokenizer(items, wxT(","));
    while (tokenizer.HasMoreTokens())
    {
        wxString token = tokenizer.GetNextToken();
        itemsArray.Add(token);
    }
}

/// Convert array containing config item names to
/// a string
void ctConfigItem::ArrayToString(const wxArrayString& itemsArray, wxString& items)
{
    items = wxEmptyString;
    size_t i;
    for (i = 0; i < itemsArray.GetCount(); i++)
    {
        items += itemsArray[i];
        if (i < (itemsArray.GetCount() - 1))
            items += wxT(",");
    }
}

/// Populate a list of items found in the string.
void ctConfigItem::StringToItems(ctConfigItem* topItem, const wxString& items, wxList& list)
{
    wxArrayString strArray;
    StringToArray(items, strArray);
    size_t i;
    for (i = 0; i < strArray.GetCount(); i++)
    {
        wxString str(strArray[i]);
        ctConfigItem* item = topItem->FindItem(str);
        if (item)
            list.Append(item);
    }
}

/// Find an item in this hierarchy
ctConfigItem* ctConfigItem::FindItem(const wxString& name)
{
    if (GetName() == name)
        return this;

    for ( wxNode* node = GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfigItem* child = (ctConfigItem*) node->GetData();
        ctConfigItem* found = child->FindItem(name);
        if (found)
            return found;
    }
    return NULL;
}

/// Find the next sibling
ctConfigItem* ctConfigItem::FindNextSibling()
{
    if (!GetParent())
        return NULL;
    wxNode* node = GetParent()->GetChildren().Member(this);
    if (node && node->GetNext())
    {
        return (ctConfigItem*) node->GetNext()->GetData();
    }
    return NULL;
}

/// Find the previous sibling
ctConfigItem* ctConfigItem::FindPreviousSibling()
{
    if (!GetParent())
        return NULL;
    wxNode* node = GetParent()->GetChildren().Member(this);
    if (node && node->GetPrevious())
    {
        return (ctConfigItem*) node->GetPrevious()->GetData();
    }
    return NULL;
}

/// Sync appearance
void ctConfigItem::Sync()
{
    if (GetDocument())
    {
        ctConfigToolView* view = (ctConfigToolView*) GetDocument()->GetFirstView();
        if (view)
        {
            view->SyncItem(wxGetApp().GetMainFrame()->GetConfigTreeCtrl(), this);
        }
    }
}

/// Create a clone of this and children
ctConfigItem* ctConfigItem::DeepClone()
{
    ctConfigItem* newItem = Clone();

    for ( wxNode* node = GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfigItem* child = (ctConfigItem*) node->GetData();
        ctConfigItem* newChild = child->DeepClone();
        newItem->AddChild(newChild);
    }
    return newItem;
}

/// Detach: remove from parent, and remove tree items
void ctConfigItem::Detach()
{
    if (GetParent())
        GetParent()->RemoveChild(this);
    else
        GetDocument()->SetTopItem(NULL);
    SetParent(NULL);

    wxTreeItemId treeItem = GetTreeItemId();

    DetachFromTree();

    // Will delete the branch, but not the config items.
    wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->Delete(treeItem);
}

/// Hide from tree: make sure tree deletions won't delete
/// the config items
void ctConfigItem::DetachFromTree()
{
    wxTreeItemId item = GetTreeItemId();

    ctTreeItemData* data = (ctTreeItemData*) wxGetApp().GetMainFrame()->GetConfigTreeCtrl()->GetItemData(item);
    data->SetConfigItem(NULL);
    m_treeItemId = wxTreeItemId();

    for ( wxNode* node = GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        ctConfigItem* child = (ctConfigItem*) node->GetData();
        child->DetachFromTree();
    }
}

/// Attach: insert after the given position
void ctConfigItem::Attach(ctConfigItem* parent, ctConfigItem* insertBefore)
{
    if (parent)
    {
        SetParent(parent);
        wxNode* node = NULL;
        if (insertBefore)
            node = parent->GetChildren().Member(insertBefore);

        if (node)
            parent->GetChildren().Insert(node, this);
        else
            parent->GetChildren().Append(this);
    }
    else
    {
        GetDocument()->SetTopItem(this);
    }
}

/// Can have children?
bool ctConfigItem::CanHaveChildren() const
{
    return (GetType() == ctTypeGroup ||
            GetType() == ctTypeCheckGroup ||
            GetType() == ctTypeRadioGroup);
}

// An item is in the active context if:
// The context field is empty; or
// The context field contains a symbol that is currently enabled.
bool ctConfigItem::IsInActiveContext()
{
    wxString context = GetPropertyString(wxT("context"));
    if (context.IsEmpty())
        return TRUE;

    wxList contextItems;
    StringToItems(GetDocument()->GetTopItem(), context, contextItems);

    for ( wxNode* node = contextItems.GetFirst(); node; node = node->GetNext() )
    {
        ctConfigItem* otherItem = (ctConfigItem*) node->GetData();
        if (otherItem->IsEnabled())
            return TRUE;
    }
    return FALSE;
}

/// Evaluate the requires properties:
/// if any of the 'requires' items are disabled,
/// then this one is disabled (and inactive).
void ctConfigItem::EvaluateDependencies()
{
    wxList items;
    wxString requires = GetPropertyString(wxT("requires"));
    wxString precludes = GetPropertyString(wxT("precludes"));
    wxString enabledIf = GetPropertyString(wxT("enabled-if"));
    wxString enabledIfNot = GetPropertyString(wxT("enabled-if-not"));

    bool active = TRUE;
    bool enabled = IsEnabled();
    bool oldEnabled = enabled;
    bool oldActive = IsActive();
    bool explicitlyEnabled = FALSE;
    bool explicitlyDisabled = FALSE;
    bool inActiveContext = IsInActiveContext();

    // Add the parent to the list of dependencies, if the
    // parent is a check or radio group.
    ctConfigItem* parent = GetParent();
    if (parent &&
        (parent->GetType() == ctTypeCheckGroup || 
        parent->GetType() == ctTypeRadioGroup))
        items.Append(parent);

    wxList tempItems;
    StringToItems(GetDocument()->GetTopItem(), requires, tempItems);

    wxNode* node;
    for ( node = tempItems.GetFirst(); node; node = node->GetNext() )
    {
        // Only consider the dependency if both items are in
        // an active context.
        // Each is in the active context if:
        // The context field is empty; or
        // The context field contains a symbol that is currently enabled.
        ctConfigItem* otherItem = (ctConfigItem*) node->GetData();
        if (inActiveContext && otherItem->IsInActiveContext())
            items.Append(otherItem);
    }

    {
        int enabledCount = 0;
        for ( node = items.GetFirst(); node; node = node->GetNext() )
        {
            ctConfigItem* otherItem = (ctConfigItem*) node->GetData();
            
            if (otherItem->IsEnabled())
            {
                enabledCount ++;
            }
        }
        if (items.GetCount() > 0 && enabledCount == 0)
        {
            // None of the items were enabled
            enabled = FALSE;
            active = FALSE;
            explicitlyDisabled = TRUE;
        }
    }
    
    items.Clear();
    if (!enabledIfNot.IsEmpty())
    {
        StringToItems(GetDocument()->GetTopItem(), enabledIfNot, items);
        int disabledCount = 0;
        int inContextCount = 0;

        for ( wxNode* node = items.GetFirst(); node; node = node->GetNext() )
        {
            ctConfigItem* otherItem = (ctConfigItem*) node->GetData();
            
            if (inActiveContext && otherItem->IsInActiveContext())
            {
                // Make this enabled and inactive, _unless_ it's
                // already been explicitly disabled in the previous
                // requires evaluation (it really _has_ to be off)
                if (!otherItem->IsEnabled())
                {
                    disabledCount ++;
                }
                inContextCount ++;
            }
        }
        // Enable if there were no related items that were enabled
        if (inContextCount > 0 && (disabledCount == inContextCount) && !explicitlyDisabled)
        {
            explicitlyEnabled = TRUE;
            enabled = TRUE;
            active = FALSE;
        }
    }

    items.Clear();
    if (!enabledIf.IsEmpty())
    {
        StringToItems(GetDocument()->GetTopItem(), enabledIf, items);
        int enabledCount = 0;
        int inContextCount = 0;

        for ( wxNode* node = items.GetFirst(); node; node = node->GetNext() )
        {
            ctConfigItem* otherItem = (ctConfigItem*) node->GetData();
            
            if (inActiveContext && otherItem->IsInActiveContext())
            {
                // Make this enabled and inactive, _unless_ it's
                // already been explicitly disabled in the previous
                // requires evaluation (it really _has_ to be off)
                if (otherItem->IsEnabled())
                {
                    enabledCount ++;
                }
                inContextCount ++;
            }
        }
        // Enable if there were no related items that were disabled
        if (inContextCount > 0 && (enabledCount > 0) && !explicitlyDisabled)
        {
            explicitlyEnabled = TRUE;
            enabled = TRUE;
            active = FALSE;
        }
    }

    items.Clear();
    if (!precludes.IsEmpty())
    {
        StringToItems(GetDocument()->GetTopItem(), precludes, items);
        int enabledCount = 0;
        int inContextCount = 0;

        for ( wxNode* node = items.GetFirst(); node; node = node->GetNext() )
        {
            ctConfigItem* otherItem = (ctConfigItem*) node->GetData();
            
            if (inActiveContext && otherItem->IsInActiveContext())
            {
                // Make this enabled and inactive, _unless_ it's
                // already been explicitly disabled in the previous
                // requires evaluation (it really _has_ to be off)
                if (otherItem->IsEnabled())
                {
                    enabledCount ++;
                }
            }
        }
        // Disable if there were no related items that were disabled
        if (inContextCount > 0 && (enabledCount == inContextCount) && !explicitlyEnabled)
        {
            enabled = FALSE;
            active = FALSE;
        }
    }

    // Finally check a sort of dependency: whether our
    // context is active. If not, make this inactive.
    if (!IsInActiveContext())
        active = FALSE;

    SetActive(active);

    // If going active, set enabled state to the default state
    if (active &&
        oldActive != active &&
        (GetType() == ctTypeBoolCheck || GetType() == ctTypeCheckGroup) &&
        m_properties.FindProperty(wxT("default-state")))
    {
        bool defaultState = m_properties.FindProperty(wxT("default-state"))->GetVariant().GetBool();
        enabled = defaultState;
    }
    Enable(enabled);

    // Deal with setting a radio button
    if (enabled && enabled != oldEnabled &&
        (GetType() == ctTypeBoolRadio || GetType() == ctTypeRadioGroup))
    {
        wxList considered;
        PropagateRadioButton(considered);
    }
}

/// Get description, which may be dynamically
/// generated depending on the property.
wxString ctConfigItem::GetDescription(ctProperty* property)
{
    if (property->GetName() == wxT("description"))
    {
        wxString value(property->GetValue());
        if (value.IsEmpty())
            return wxT("Double-click on <B>description</B> to write a brief explanation of the setting.<P>");
        else
            return value;
    }
    else if (property->GetName() == wxT("notes"))
    {
        wxString value(property->GetValue());
        if (value.IsEmpty())
            return wxT("Double-click on <B>notes</B> to write notes about this setting.<P>");
        else
            return value;
    }
    return property->GetDescription();
}

/// Get the title for the property editor
wxString ctConfigItem::GetTitle()
{
    wxString title(GetName());
    if (GetType() == ctTypeCheckGroup ||
        GetType() == ctTypeRadioGroup ||
        GetType() == ctTypeBoolCheck ||
        GetType() == ctTypeBoolRadio)
    {
        if (IsEnabled())
            title = title + _T(" - enabled");
        else
            title = title + _T(" - disabled");
    }
    return title;
}

/// Propagate a change in enabled/disabled status
void ctConfigItem::PropagateChange(wxList& considered)
{
    if (GetType() == ctTypeCheckGroup ||
        GetType() == ctTypeRadioGroup ||
        GetType() == ctTypeBoolCheck ||
        GetType() == ctTypeBoolRadio)
    {
        // TODO: what about string, integer? Can they have
        // dependencies?
        
        for ( wxNode* node = GetDependents().GetFirst(); node; node = node->GetNext() )
        {
            ctConfigItem* child = (ctConfigItem*) node->GetData();

            // Avoid loops
            if (!considered.Member(child))
            {
                considered.Append(child);

                child->EvaluateDependencies();
                child->Sync();

                child->PropagateChange(considered);
            }
        }
    }
}

/// Process radio button selection
void ctConfigItem::PropagateRadioButton(wxList& considered)
{
    if ((GetType() == ctTypeBoolRadio || GetType() == ctTypeRadioGroup) && IsEnabled())
    {
        wxString mutuallyExclusive(GetPropertyString(wxT("exclusivity")));

        wxList list;
        StringToItems(GetDocument()->GetTopItem(), mutuallyExclusive, list);

        for ( wxNode* node = list.GetFirst(); node; node = node->GetNext() )
        {
            ctConfigItem* child = (ctConfigItem*) node->GetData();
            if (child->IsEnabled() && child != this)
            {
                child->Enable(FALSE);
                child->Sync();

                if (!considered.Member(child))
                    child->PropagateChange(considered);
            }
        }
    }
}
