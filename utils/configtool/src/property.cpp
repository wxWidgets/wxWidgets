/////////////////////////////////////////////////////////////////////////////
// Name:        property.cpp
// Purpose:     ctProperty objects represent name/value pairs,
//              for properties of a configuration item.
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-03
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "property.h"
#endif

#include "wx/wx.h"

#include "property.h"

IMPLEMENT_CLASS(ctProperty, wxObject)
IMPLEMENT_CLASS(ctProperties, wxObject)

void ctProperties::AddProperty(ctProperty* property, const wxString& insertAfter)
{
    ctProperty* oldProp = FindProperty(property->GetName());
    if (oldProp)
        m_list.DeleteObject(oldProp);

    wxNode* insertBeforeNode = NULL;
    if (insertAfter)
    {
        ctProperty* insertAfterProp = FindProperty(insertAfter);
        if (insertAfterProp)
        {
            wxNode* node = m_list.Member(insertAfterProp);
            if (node)
                insertBeforeNode = node->GetNext();
        }
    }
    if (insertBeforeNode)
        m_list.Insert(insertBeforeNode, property);
    else
        m_list.Append(property);

    if (oldProp)
    {
        property->GetVariant() = oldProp->GetVariant();

        delete oldProp;
    }
}

void ctProperties::RemoveProperty(ctProperty* property)
{
    m_list.DeleteObject(property);
}

void ctProperties::DeleteProperty(const wxString& name)
{
    ctProperty* prop = FindProperty(name);
    if (prop)
    {
        m_list.DeleteObject(prop);
        delete prop;
    }
}

ctProperty* ctProperties::FindProperty(const wxString& name) const
{
    wxNode* node = m_list.GetFirst();
    while (node)
    {
        ctProperty* prop = (ctProperty*) node->GetData();
        if (prop->GetName() == name)
            return prop;

        node = node->GetNext();
    }
    return (ctProperty*) NULL;
}

wxVariant ctProperties::FindPropertyValue(const wxString& name) const
{
    ctProperty* prop = FindProperty(name);
    if (prop)
        return prop->m_variant;
    else
        return wxEmptyString;
}

wxString ctProperties::FindPropertyValueString(const wxString& name) const
{
    ctProperty* prop = FindProperty(name);
    if (prop)
        return prop->m_variant.GetString();
    else
        return wxEmptyString;
}

ctProperty* ctProperties::FindOrCreateProperty(const wxString& name)
{
    ctProperty* prop = FindProperty(name);
    if (!prop)
    {
        prop = new ctProperty(name);
        AddProperty(prop);
    }
    return prop;
}

void ctProperties::Clear()
{
    m_list.DeleteContents(TRUE);
    m_list.Clear();
    m_list.DeleteContents(FALSE);
}

void ctProperties::Copy(const ctProperties& properties)
{
    wxNode* node = properties.m_list.GetFirst();
    while (node)
    {
        ctProperty* prop = (ctProperty*) node->GetData();

        AddProperty(new ctProperty(* prop));

        node = node->GetNext();
    }
}

void ctProperties::SetProperty(const wxString& name, const wxString& value)
{
    ctProperty* prop = FindOrCreateProperty(name);
    prop->SetValue(value);
}

void ctProperties::SetProperty(const wxString& name, long value)
{
    ctProperty* prop = FindOrCreateProperty(name);
    prop->GetVariant() = value;
}

void ctProperties::SetProperty(const wxString& name, bool value)
{
    ctProperty* prop = FindOrCreateProperty(name);
    prop->GetVariant() = (bool) value;
}

void ctProperties::SetProperty(const wxString& name, const wxVariant& value)
{
    ctProperty* prop = FindOrCreateProperty(name);
    prop->SetValue(value);
}

ctProperty* ctProperties::GetNth(int i) const
{
    wxASSERT( i < (int) GetCount() );
    if (i < (int) GetCount())
    {
        wxNode* node = m_list.Nth(i);
        return (ctProperty*) node->GetData();
    }
    return NULL;
}

#if 0
/// Combine the styles of all selected properties
/// with this group name.
long ctProperties::CombineStyles(const wxString& groupName)
{
    long styleValue = 0;

    wxNode* node = m_list.GetFirst();
    while (node)
    {
        ctProperty* prop = (ctProperty*) node->GetData();

        if (prop->GetGroupName() == groupName && prop->GetVariant().GetBool())
        {
            styleValue |= prop->GetStyleValue();
        }

        node = node->GetNext();
    }
    return styleValue;
}

/// Combine the styles of all selected properties
/// with this group name.
wxString ctProperties::CombineStylesString(const wxString& groupName)
{
    wxString styleList;

    wxNode* node = m_list.GetFirst();
    while (node)
    {
        ctProperty* prop = (ctProperty*) node->GetData();

        if (prop->GetGroupName() == groupName && prop->GetVariant().GetBool())
        {
            if (styleList.IsEmpty())
                styleList = prop->GetName();
            else
                styleList = styleList + _T("|") + prop->GetName();
        }

        node = node->GetNext();
    }
    return styleList;
}

// Remove any spurious properties that need garbage
// collecting.
void ctProperties::RemoveHiddenProperties()
{
    wxNode* node = m_list.GetFirst();
    while (node)
    {
        ctProperty* prop = (ctProperty*) node->GetData();
        wxNode* next = node->GetNext();

        if (!prop->IsShown())
        {
            delete node;
            delete prop;
        }

        node = next;
    }
}
#endif