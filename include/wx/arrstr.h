///////////////////////////////////////////////////////////////////////////////
// Name:        wx/arrstr.h
// Purpose:     wxArrayString class
// Author:      Mattia Barbon and Vadim Zeitlin
// Modified by:
// Created:     07/07/03
// Copyright:   (c) 2003 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ARRSTR_H
#define _WX_ARRSTR_H

#include "wx/defs.h"
#include "wx/string.h"

#if wxUSE_STD_CONTAINERS_COMPATIBLY
    #include <vector>
#endif

// these functions are only used in STL build now but we define them in any
// case for compatibility with the existing code outside of the library which
// could be using them
inline int wxCMPFUNC_CONV wxStringSortAscending(const wxString& s1, const wxString& s2)
{
    return s1.Cmp(s2);
}

inline int wxCMPFUNC_CONV wxStringSortDescending(const wxString& s1, const wxString& s2)
{
    return wxStringSortAscending(s2, s1);
}

// This comparison function ignores case when comparing strings differing not
// in case only, i.e. this ensures that "Aa" comes before "AB", unlike with
// wxStringSortAscending().
inline int wxCMPFUNC_CONV
wxDictionaryStringSortAscending(const wxString& s1, const wxString& s2)
{
    const int cmp = s1.CmpNoCase(s2);
    return cmp ? cmp : s1.Cmp(s2);
}

inline int wxCMPFUNC_CONV
wxDictionaryStringSortDescending(const wxString& s1, const wxString& s2)
{
    return wxDictionaryStringSortAscending(s2, s1);
}

#include "wx/dynarray.h"

typedef int (wxCMPFUNC_CONV *CMPFUNCwxString)(wxString*, wxString*);
typedef wxString _wxArraywxBaseArrayStringBase;
_WX_DECLARE_BASEARRAY_2(_wxArraywxBaseArrayStringBase, wxBaseArrayStringBase,
                        wxArray_SortFunction<wxString>,
                        class WXDLLIMPEXP_BASE);
WX_DEFINE_USER_EXPORTED_TYPEARRAY(wxString, wxArrayStringBase,
                                  wxBaseArrayStringBase, WXDLLIMPEXP_BASE);

class WXDLLIMPEXP_BASE wxArrayString : public wxArrayStringBase
{
public:
    // type of function used by wxArrayString::Sort()
    typedef int (wxCMPFUNC_CONV *CompareFunction)(const wxString& first,
                                                  const wxString& second);

    wxArrayString() { }
    wxArrayString(const wxArrayString& a) : wxArrayStringBase(a) { }
    wxArrayString(size_t sz, const char** a);
    wxArrayString(size_t sz, const wchar_t** a);
    wxArrayString(size_t sz, const wxString* a);

    int Index(const wxString& str, bool bCase = true, bool bFromEnd = false) const;

    void Sort(bool reverseOrder = false);
    void Sort(CompareFunction function);
    void Sort(CMPFUNCwxString function) { wxArrayStringBase::Sort(function); }

    size_t Add(const wxString& string, size_t copies = 1)
    {
        wxArrayStringBase::Add(string, copies);
        return size() - copies;
    }
};

_WX_DEFINE_SORTED_TYPEARRAY_2(wxString, wxSortedArrayStringBase,
                              wxBaseArrayStringBase, = wxStringSortAscending,
                              class WXDLLIMPEXP_BASE, wxArrayString::CompareFunction);

class WXDLLIMPEXP_BASE wxSortedArrayString : public wxSortedArrayStringBase
{
public:
    wxSortedArrayString() : wxSortedArrayStringBase(wxStringSortAscending)
        { }
    wxSortedArrayString(const wxSortedArrayString& array)
        : wxSortedArrayStringBase(array)
        { }
    wxSortedArrayString(const wxArrayString& src)
        : wxSortedArrayStringBase(wxStringSortAscending)
    {
        reserve(src.size());

        for ( size_t n = 0; n < src.size(); n++ )
            Add(src[n]);
    }
    explicit wxSortedArrayString(wxArrayString::CompareFunction compareFunction)
        : wxSortedArrayStringBase(compareFunction)
        { }

    int Index(const wxString& str, bool bCase = true, bool bFromEnd = false) const;

private:
    void Insert()
    {
        wxFAIL_MSG( "wxSortedArrayString::Insert() is not to be used" );
    }

    void Sort()
    {
        wxFAIL_MSG( "wxSortedArrayString::Sort() is not to be used" );
    }
};

// this class provides a temporary wxString* from a
// wxArrayString
class WXDLLIMPEXP_BASE wxCArrayString
{
public:
    wxCArrayString( const wxArrayString& array )
        : m_array( array ), m_strings( NULL )
    { }
    ~wxCArrayString() { delete[] m_strings; }

    size_t GetCount() const { return m_array.GetCount(); }
    wxString* GetStrings()
    {
        if( m_strings ) return m_strings;
        size_t count = m_array.GetCount();
        m_strings = new wxString[count];
        for( size_t i = 0; i < count; ++i )
            m_strings[i] = m_array[i];
        return m_strings;
    }

    wxString* Release()
    {
        wxString *r = GetStrings();
        m_strings = NULL;
        return r;
    }

private:
    const wxArrayString& m_array;
    wxString* m_strings;
};


// ----------------------------------------------------------------------------
// helper functions for working with arrays
// ----------------------------------------------------------------------------

// by default, these functions use the escape character to escape the
// separators occurring inside the string to be joined, this can be disabled by
// passing '\0' as escape

WXDLLIMPEXP_BASE wxString wxJoin(const wxArrayString& arr,
                                 const wxChar sep,
                                 const wxChar escape = wxT('\\'));

WXDLLIMPEXP_BASE wxArrayString wxSplit(const wxString& str,
                                       const wxChar sep,
                                       const wxChar escape = wxT('\\'));


// ----------------------------------------------------------------------------
// This helper class allows to pass both C array of wxStrings or wxArrayString
// using the same interface.
//
// Use it when you have two methods taking wxArrayString or (int, wxString[]),
// that do the same thing. This class lets you iterate over input data in the
// same way whether it is a raw array of strings or wxArrayString.
//
// The object does not take ownership of the data -- internally it keeps
// pointers to the data, therefore the data must be disposed of by user
// and only after this object is destroyed. Usually it is not a problem as
// only temporary objects of this class are used.
// ----------------------------------------------------------------------------

class wxArrayStringsAdapter
{
public:
    // construct an adapter from a wxArrayString
    wxArrayStringsAdapter(const wxArrayString& strings)
        : m_type(wxSTRING_ARRAY), m_size(strings.size())
    {
        m_data.array = &strings;
    }

    // construct an adapter from a wxString[]
    wxArrayStringsAdapter(unsigned int n, const wxString *strings)
        : m_type(wxSTRING_POINTER), m_size(n)
    {
        m_data.ptr = strings;
    }

#if wxUSE_STD_CONTAINERS_COMPATIBLY
    // construct an adapter from a vector of strings
    wxArrayStringsAdapter(const std::vector<wxString>& strings)
        : m_type(wxSTRING_POINTER), m_size(strings.size())
    {
        m_data.ptr = &strings[0];
    }
#endif // wxUSE_STD_CONTAINERS_COMPATIBLY

    // construct an adapter from a single wxString
    wxArrayStringsAdapter(const wxString& s)
        : m_type(wxSTRING_POINTER), m_size(1)
    {
        m_data.ptr = &s;
    }

    // default copy constructor is ok

    // iteration interface
    size_t GetCount() const { return m_size; }
    bool IsEmpty() const { return GetCount() == 0; }
    const wxString& operator[] (unsigned int i) const
    {
        wxASSERT_MSG( i < GetCount(), wxT("index out of bounds") );
        if(m_type == wxSTRING_POINTER)
            return m_data.ptr[i];
        return m_data.array->Item(i);
    }
    wxArrayString AsArrayString() const
    {
        if(m_type == wxSTRING_ARRAY)
            return *m_data.array;
        return wxArrayString(GetCount(), m_data.ptr);
    }

private:
    // type of the data being held
    enum wxStringContainerType
    {
        wxSTRING_ARRAY,  // wxArrayString
        wxSTRING_POINTER // wxString[]
    };

    wxStringContainerType m_type;
    size_t m_size;
    union
    {
        const wxString *      ptr;
        const wxArrayString * array;
    } m_data;

    wxDECLARE_NO_ASSIGN_CLASS(wxArrayStringsAdapter);
};

#endif // _WX_ARRSTR_H
