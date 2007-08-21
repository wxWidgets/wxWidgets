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

template<typename T>
class wxVector
{
public:
    typedef size_t size_type;
    typedef T value_type;
    typedef value_type* iterator;
    typedef value_type& reference;

    wxVector() : m_allocsize(16), m_size(0), m_capacity(0), m_objects(0) {}

    wxVector(const wxVector& c)
    {
        wxCHECK2(Copy(c), return);
    }

    ~wxVector()
    {
        clear();
    }

    void clear()
    {
        for (size_type i = 0; i < size(); i++)
            delete m_objects[i];
        free(m_objects);
        m_objects = 0;
        m_size = m_capacity = 0;
    }

    void reserve(size_type n)
    {
        if ( !Alloc(n) )
        {
            wxFAIL_MSG( _T("out of memory in wxVector::reserve()") );
        }
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
        wxCHECK(Copy(vb), *this);
        return *this;
    }

    void push_back(const value_type& o)
    {
        wxCHECK2(Alloc(size() + 1), return);
        Append(new value_type(o));
    }

    void pop_back()
    {
        RemoveAt(size() - 1);
    }

    const value_type& at(size_type idx) const
    {
        wxASSERT(idx < m_size);
        return *m_objects[idx];
    }

    value_type& at(size_type idx)
    {
        wxASSERT(idx < m_size);
        return *m_objects[idx];
    }

    const value_type& operator[](size_type idx) const  { return at(idx); }
    value_type& operator[](size_type idx) { return at(idx); }
    const value_type& front() const { return at(0); }
    value_type& front() { return at(0); }
    const value_type& back() const { return at(size() - 1); }
    value_type& back() { return at(size() - 1); }

    iterator begin() { return m_objects[0]; }
    iterator end() { return m_objects[size()]; }

    iterator erase(iterator first, iterator last)
    {
        size_type idx = first - begin();
        RemoveAt(idx, last - first);
        return begin() + idx;
    }
    iterator erase(iterator it)
    {
        size_type idx = it - begin();
        RemoveAt(idx);
        return begin() + idx;
    }

#if WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED( size_type erase(size_type n) );
#endif // WXWIN_COMPATIBILITY_2_8

    iterator insert(iterator it, const value_type& v = value_type())
    {
        wxCHECK2(Alloc(size() + 1), return 0);
        size_type idx = it - begin();
        InsertAt(new value_type(v), idx);
        return begin() + idx;
    }

private:
    bool Alloc(size_type sz)
    {
        // work in multiples of m_allocsize;
        sz = (sz / m_allocsize + 1) * m_allocsize;
        if (sz <= m_capacity)
            return true;

        // try to realloc
        void *mem = realloc(m_objects, sizeof(value_type*) * sz);
        if (! mem)
            return false; // failed
        // success
        m_objects = (value_type **) mem;
        m_capacity = sz;
        return true;
    }

    void Append(value_type *obj)
    {
        wxASSERT(m_size < m_capacity);
        m_objects[m_size] = obj;
        m_size++;
    }

    void InsertAt(size_type idx, value_type *obj)
    {
        wxASSERT(idx <= m_size);
        wxASSERT(m_size < m_capacity);
        if (idx < m_size)
            memmove(
                m_objects + idx + 1,
                m_objects + idx,
                ( m_size - idx ) * sizeof(value_type*) );

        m_size++;
    }

    void RemoveAt(size_type idx)
    {
        wxASSERT(idx < m_size);
        delete m_objects[idx];
        if (idx < m_size - 1)
            memcpy(
                m_objects + idx,
                m_objects + idx + 1,
                ( m_size - idx - 1 ) * sizeof(value_type*) );
        m_size--;
    }

    void RemoveAt(size_type idx, size_type count)
    {
        if (count == 0)
            return;
        wxASSERT(idx < m_size);
        size_type i;
        for (i = 0; i < count; i++)
            delete m_objects[idx+1];
        if (idx < m_size - count)
            memcpy(
                m_objects + idx,
                m_objects + idx + count,
                ( m_size - idx - count ) * sizeof(value_type*) );
        m_size -= count;
    }
    bool Copy(const wxVector& vb)
    {
        clear();
        if (! Alloc(vb.size()))
            return false;

        for (size_type i = 0; i < vb.size(); i++)
        {
            value_type *o = new value_type(vb.at(i));
            if (! o)
                return false;
            Append(o);
        }

        return true;
    }

private:
    size_type m_allocsize;
    size_type m_size,
              m_capacity;
    value_type **m_objects;
};

#if WXWIN_COMPATIBILITY_2_8
template<typename T>
typename wxVector<T>::size_type wxVector<T>::erase(size_type n)
{
    RemoveAt(n);
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
