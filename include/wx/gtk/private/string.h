///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/string.h
// Purpose:     wxGtkString class declaration
// Author:      Vadim Zeitlin
// Created:     2006-10-19
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_STRING_H_
#define _WX_GTK_PRIVATE_STRING_H_

#include "wx/gtk/private/glibptr.h"

// ----------------------------------------------------------------------------
// Convenience class for g_freeing a gchar* on scope exit automatically
// ----------------------------------------------------------------------------

class wxGtkString : public wxGlibPtr<gchar>
{
public:
    explicit wxGtkString(const gchar *s) : wxGlibPtr<gchar>(s) { }

    // More string-like accessor.
    const gchar *c_str() const { return get(); }
};


// ----------------------------------------------------------------------------
// list for sorting collated strings
// ----------------------------------------------------------------------------

#include "wx/string.h"
#include "wx/vector.h"
#include "wx/sharedptr.h"

class wxGtkCollatableString
{
public:
    wxGtkCollatableString( const wxString &label, const gchar *key )
        : m_label(label), m_key(key)
    {
    }

    wxString     m_label;
    wxGtkString  m_key;
};

class wxGtkCollatedArrayString
{
public:
    wxGtkCollatedArrayString() = default;

    int Add( const wxString &new_label )
    {
        int index = 0;

        wxGtkString new_key_lower(g_utf8_casefold( new_label.utf8_str(), -1));
        gchar *new_key = g_utf8_collate_key( new_key_lower, -1);

        wxSharedPtr<wxGtkCollatableString> new_ptr( new wxGtkCollatableString( new_label, new_key ) );

        wxVector< wxSharedPtr<wxGtkCollatableString> >::iterator iter;
        for (iter = m_list.begin(); iter != m_list.end(); ++iter)
        {
            wxSharedPtr<wxGtkCollatableString> ptr = *iter;

            const gchar* key = ptr->m_key;
            if (strcmp(key,new_key) >= 0)
            {
                m_list.insert( iter, new_ptr );
                return index;
            }
            index ++;
        }

        m_list.push_back( new_ptr );
        return index;
    }

    size_t GetCount()
    {
        return m_list.size();
    }

    wxString At( size_t index )
    {
        return m_list[index]->m_label;
    }

    void Clear()
    {
        m_list.clear();
    }

    void RemoveAt( size_t index )
    {
        m_list.erase( m_list.begin() + index );
    }

private:
    wxVector< wxSharedPtr<wxGtkCollatableString> > m_list;
};


#endif // _WX_GTK_PRIVATE_STRING_H_

