///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dynarray.cpp
// Purpose:     implementation of wxBaseArray class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.09.97
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers
// ============================================================================

// For compilers that support precompilation, includes "wx.h".
#include  "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/intl.h"
#endif //WX_PRECOMP

#if wxUSE_STD_CONTAINERS

#include "wx/arrstr.h"

#include "wx/beforestd.h"
#include <functional>
#include "wx/afterstd.h"

// some compilers (Sun CC being the only known example) distinguish between
// extern "C" functions and the functions with C++ linkage and ptr_fun and
// wxStringCompareLess can't take wxStrcmp/wxStricmp directly as arguments in
// this case, we need the wrappers below to make this work
struct wxStringCmp
{
    typedef wxString first_argument_type;
    typedef wxString second_argument_type;
    typedef int result_type;

    int operator()(const wxString& s1, const wxString& s2) const
    {
        return s1.compare(s2);
    }
};

struct wxStringCmpNoCase
{
    typedef wxString first_argument_type;
    typedef wxString second_argument_type;
    typedef int result_type;

    int operator()(const wxString& s1, const wxString& s2) const
    {
        return s1.CmpNoCase(s2);
    }
};

int wxArrayString::Index(const wxString& str, bool bCase, bool WXUNUSED(bFromEnd)) const
{
    wxArrayString::const_iterator it;

    if (bCase)
    {
        it = std::find_if(begin(), end(),
                          std::not1(
                              std::bind2nd(
                                  wxStringCmp(), str)));
    }
    else // !bCase
    {
        it = std::find_if(begin(), end(),
                          std::not1(
                              std::bind2nd(
                                  wxStringCmpNoCase(), str)));
    }

    return it == end() ? wxNOT_FOUND : it - begin();
}

template<class F>
class wxStringCompareLess
{
public:
    wxStringCompareLess(F f) : m_f(f) { }
    bool operator()(const wxString& s1, const wxString& s2)
        { return m_f(s1, s2) < 0; }
private:
    F m_f;
};

template<class F>
wxStringCompareLess<F> wxStringCompare(F f)
{
    return wxStringCompareLess<F>(f);
}

void wxArrayString::Sort(CompareFunction function)
{
    std::sort(begin(), end(), wxStringCompare(function));
}

void wxArrayString::Sort(bool reverseOrder)
{
    if (reverseOrder)
    {
        std::sort(begin(), end(), std::greater<wxString>());
    }
    else
    {
        std::sort(begin(), end());
    }
}

int wxSortedArrayString::Index(const wxString& str,
                               bool WXUNUSED_UNLESS_DEBUG(bCase),
                               bool WXUNUSED_UNLESS_DEBUG(bFromEnd)) const
{
    wxASSERT_MSG( bCase && !bFromEnd,
                  "search parameters ignored for sorted array" );

    wxSortedArrayString::const_iterator
        it = std::lower_bound(begin(), end(), str, wxStringCompare(wxStringCmp()));

    if ( it == end() || str.Cmp(*it) != 0 )
        return wxNOT_FOUND;

    return it - begin();
}

#endif // wxUSE_STD_CONTAINERS
