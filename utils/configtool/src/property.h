/////////////////////////////////////////////////////////////////////////////
// Name:        property.h
// Purpose:     ctProperty objects represent name/value pairs,
//              for properties of a configuration item.
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-03
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _CT_PROPERTY_H_
#define _CT_PROPERTY_H_

#include "wx/variant.h"

/*!
 * A property/name pair, with other specialised members for
 * storing window/sizer information.
 */

class ctProperty: public wxObject
{
    DECLARE_CLASS(ctProperty)
public:
    ctProperty()
    {
        m_readOnly = false;
        m_custom = false;
        m_show = true;
    }
    ctProperty(const ctProperty& property) : wxObject() { Copy(property); }
    ctProperty(const wxString& descr, const wxVariant& variant,
        const wxString& editorType = wxEmptyString,
        bool readOnly = false)
    {
        m_description = descr;
        m_variant = variant;
        m_editorType = editorType;
        m_show = true;
        m_readOnly = readOnly;
        m_custom = false;
    }

    ctProperty(const wxString& name, const wxString& value = wxEmptyString)
    {
        m_variant.SetName(name);
        m_variant = value;
        m_readOnly = false;
        m_custom = false;
        m_show = true;
    }

    void operator= (const ctProperty& property) { Copy(property); }
    void Copy(const ctProperty& property)
    {
        m_variant = property.m_variant;
        m_editorType = property.m_editorType;
        m_description = property.m_description;
        m_choices = property.m_choices;
        m_show = property.m_show;
        m_readOnly = property.m_readOnly;
        m_custom = property.m_custom;
    }

    bool operator== (const ctProperty& property) const
    {
        return ((m_variant == property.m_variant) &&
            (m_editorType == property.m_editorType) &&
            (m_description == property.m_description) &&
            (m_show == property.m_show) &&
            (m_custom == property.m_custom) &&
            (m_readOnly == property.m_readOnly) &&
            (m_choices == property.m_choices));
    }

    bool operator!= (const ctProperty& property) const
    {
        return !((*this) == property);
    }

    inline const wxString& GetName() const { return m_variant.GetName(); }
    inline wxString GetValue() const { return m_variant.GetString(); }
    inline wxVariant& GetVariant() { return m_variant; }
    inline const wxString& GetEditorType() const { return m_editorType; }
    inline const wxArrayString& GetChoices() const { return m_choices; }
    inline const wxString& GetDescription() const { return m_description; }
    inline bool IsCustom() const { return m_custom; }

    inline void SetName(const wxString& name) { m_variant.SetName(name); }
    inline void SetValue(const wxString& value) { m_variant = value; }
    inline void SetValue(const wxVariant& value) { m_variant = value; }
    inline void SetEditorType(const wxString& type) { m_editorType = type; }
    inline void SetChoices(const wxArrayString& choices) { m_choices = choices; }
    inline void SetDescription(const wxString& descr) { m_description = descr; }
    inline void Show(bool show) { m_show = show; }
    inline bool IsShown() const { return m_show; }
    inline bool GetReadOnly() const { return m_readOnly; }
    inline void SetReadOnly(bool readOnly) { m_readOnly = readOnly; }
    inline void SetCustom(bool custom) { m_custom = custom; }

    // The name and value
    wxVariant   m_variant;

    // The editor type name (e.g. "file")
    // used to choose an editor.
    wxString    m_editorType;

    // Array of choices
    wxArrayString   m_choices;

    // Description
    wxString        m_description;

    // Whether to show or hide (e.g. not properly initialized)
    bool            m_show;

    // Read-only
    bool            m_readOnly;

    // Whether it's a custom property (so save
    // type/description)
    bool            m_custom;
};

// A class to manage properties
class ctProperties: public wxObject
{
    DECLARE_CLASS(ctProperties)
public:
    ctProperties() {}
    ctProperties(const ctProperties& properties) : wxObject() { Copy(properties); }
    virtual ~ctProperties() { Clear(); }

    void operator = (const ctProperties& properties) { Clear(); Copy(properties); }
    void Copy(const ctProperties& properties);

    inline const wxList& GetList() const { return m_list; }

    inline size_t GetCount() const { return m_list.GetCount(); }

    void AddProperty(ctProperty* property, const wxString& insertAfter = wxEmptyString);
    void SetProperty(const wxString& name, const wxString& value);
    void SetProperty(const wxString& name, long value);
    void SetProperty(const wxString& name, bool value);
    void SetProperty(const wxString& name, const wxVariant& value);
    void RemoveProperty(ctProperty* property);
    void DeleteProperty(const wxString& name);
    ctProperty* FindProperty(const wxString& name) const;
    ctProperty* FindOrCreateProperty(const wxString& name);
    wxString FindPropertyValueString(const wxString& name) const;
    wxVariant FindPropertyValue(const wxString& name) const;
    ctProperty* GetNth(int i) const;

    void Clear();

private:
    wxList      m_list;
};

#endif
    // _CT_PROPERTY_H_

