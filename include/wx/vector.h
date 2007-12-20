///////////////////////////////////////////////////////////////////////////////
// Name:        wx/vector.h
// Purpose:     STL vector clone
// Author:      Lindsay Mathieson
// Modified by: Vaclav Slavik - make it a template
// Created:     30.07.2001
// Copyright:   (c) 2001 Lindsay Mathieson <lindsay@mathieson.org>,
//                  2007 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VECTOR_H_
#define _WX_VECTOR_H_

#include "wx/defs.h"

#if wxUSE_STL

#include <vector>
#define wxVector std::vector

#else // !wxUSE_STL

#include "wx/utils.h"

template<typename T>
class wxVector
{
public:
    typedef size_t size_type;
    typedef T value_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef value_type& reference;

    wxVector() : m_size(0), m_capacity(0), m_values(NULL) {}

    wxVector(const wxVector& c)
    {
        Copy(c);
    }

    ~wxVector()
    {
        clear();
    }

    void clear()
    {
        delete[] m_values;
        m_values = NULL;
        m_size = m_capacity = 0;
    }

    void reserve(size_type n)
    {
        if ( n <= m_capacity )
            return;

        // increase the size twice, unless we're already too big or unless
        // more is requested
        //
        // NB: casts to size_t are needed to suppress mingw32 warnings about
        //     mixing enums and ints in the same expression
        const size_type increment = m_size > 0
                                     ? wxMin(m_size, (size_type)ALLOC_MAX_SIZE)
                                     : (size_type)ALLOC_INITIAL_SIZE;
        if ( m_capacity + increment > n )
            n = m_capacity + increment;

        value_type *mem = new value_type[n];

        if ( m_values )
        {
            for ( size_type i = 0; i < m_size; ++i )
                mem[i] = m_values[i];
            delete[] m_values;
        }

        m_values = mem;
        m_capacity = n;
    }

    size_type size() const
    {
        return m_size;
    }

    size_type capacity() const
    {
        return m_capacity;
    }

    bool empty() const
    {
        return size() == 0;
    }

    wxVector& operator=(const wxVector& vb)
    {
        Copy(vb);
        return *this;
    }

    void push_back(const value_type& v)
    {
        reserve(size() + 1);
        m_values[m_size++] = v;
    }

    void pop_back()
    {
        erase(end() - 1);
    }

    const value_type& at(size_type idx) const
    {
        wxASSERT(idx < m_size);
        return m_values[idx];
    }

    value_type& at(size_type idx)
    {
        wxASSERT(idx < m_size);
        return m_values[idx];
    }

    const value_type& operator[](size_type idx) const  { return at(idx); }
    value_type& operator[](size_type idx) { return at(idx); }
    const value_type& front() const { return at(0); }
    value_type& front() { return at(0); }
    const value_type& back() const { return at(size() - 1); }
    value_type& back() { return at(size() - 1); }

    const_iterator begin() const { return m_values; }
    iterator begin() { return m_values; }
    const_iterator end() const { return m_values + size(); }
    iterator end() { return m_values + size(); }

    iterator insert(iterator it, const value_type& v = value_type())
    {
        size_t idx = it - begin();

        reserve(size() + 1);

        // unless we're inserting at the end, move following values out of
        // the way:
        for ( size_t n = m_size; n != idx; --n )
            m_values[n] = m_values[n-1];

        m_values[idx] = v;
        m_size++;

        return begin() + idx;
    }

    iterator erase(iterator it)
    {
        return erase(it, it + 1);
    }

    iterator erase(iterator first, iterator last)
    {
        if ( first == last )
            return first;
        wxASSERT( first < end() && last <= end() );

        size_type index = first - begin();
        size_type count = last - first;

        // move the remaining values over to the freed space:
        for ( iterator i = last; i < end(); ++i )
            *(i - count) = *i;

        // erase items behind the new end of m_values:
        for ( iterator j = end() - count; j < end(); ++j )
            *j = value_type();

        m_size -= count;

        return begin() + index;
    }

#if WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED( size_type erase(size_type n) );
#endif // WXWIN_COMPATIBILITY_2_8

private:
    // VC6 can't compile static const int members
    enum { ALLOC_INITIAL_SIZE = 16 };
    enum { ALLOC_MAX_SIZE = 4096 };

    void Copy(const wxVector& vb)
    {
        clear();
        reserve(vb.size());

        for ( const_iterator i = vb.begin(); i != vb.end(); ++i )
            push_back(*i);
    }

private:
    size_type m_size,
              m_capacity;
    value_type *m_values;
};

#if WXWIN_COMPATIBILITY_2_8
template<typename T>
inline typename wxVector<T>::size_type wxVector<T>::erase(size_type n)
{
    erase(begin() + n);
    return n;
}
#endif // WXWIN_COMPATIBILITY_2_8

#endif // wxUSE_STL/!wxUSE_STL

#if WXWIN_COMPATIBILITY_2_8
    #define WX_DECLARE_VECTORBASE(obj, cls) typedef wxVector<obj> cls
    #define _WX_DECLARE_VECTOR(obj, cls, exp) WX_DECLARE_VECTORBASE(obj, cls)
    #define WX_DECLARE_VECTOR(obj, cls) WX_DECLARE_VECTORBASE(obj, cls)
#endif // WXWIN_COMPATIBILITY_2_8

#endif // _WX_VECTOR_H_
