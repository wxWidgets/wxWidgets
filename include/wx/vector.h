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

#if 0 // wxUSE_STL

// FIXME: can't do this yet, wxVector::erase() is different (takes index,
//        not iterator)
#include <vector>
#define wxVector std::vector

#else // !wxUSE_STL

template<typename T>
class wxVector
{
public:
    typedef size_t size_type;
    typedef T value_type;

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

    size_type erase(size_type idx)
    {
        RemoveAt(idx);
        return idx;
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

    void RemoveAt(size_type idx)
    {
        wxASSERT(idx < m_size);
        delete m_objects[idx];
        if (idx < m_size - 1)
            memcpy(
                m_objects + idx,
                m_objects + idx + 1,
                ( m_size - idx - 1 ) * sizeof(void*) );
        m_size--;
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

#endif // wxUSE_STL/!wxUSE_STL

#if WXWIN_COMPATIBILITY_2_8
    #define WX_DECLARE_VECTORBASE(obj, cls) typedef wxVector<obj> cls
    #define _WX_DECLARE_VECTOR(obj, cls, exp) WX_DECLARE_VECTORBASE(obj, cls)
    #define WX_DECLARE_VECTOR(obj, cls) WX_DECLARE_VECTORBASE(obj, cls)
#endif // WXWIN_COMPATIBILITY_2_8

#endif // _WX_VECTOR_H_
