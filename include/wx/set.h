/////////////////////////////////////////////////////////////////////////////
// Name:        wx/set.h
// Purpose:     a bitset suited for replacing the current style flags 
// Author:      Stefan Csomor
// Modified by: 
// Created:     27/07/03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SETH__
#define _WX_SETH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "set.h"
#endif

// wxSet should be applied to an enum, then this can be used like
// bitwise operators but keeps the type safety and information, the
// enums must be in a sequence , their value determines the bit position
// that they represent

template <class T> class wxSet
{
	friend class wxEnumData ;
public:
    wxSet() { m_data = 0; }
    wxSet(const wxSet &src) { m_data = src.m_data; }
    wxSet(const T el) { m_data |= 1 << el; }
    wxSet &operator =(const wxSet &rhs)
    {
		m_data = rhs.m_data;
		return *this;
    }
    wxSet &operator +=(const wxSet &rhs) // union
    {
		m_data |= rhs.m_data;
		return *this;
    }
    wxSet &operator -=(const wxSet &rhs) // difference
    {
		m_data ^= rhs.m_data;
		return *this;
    }

    wxSet &operator *=(const wxSet &rhs) // intersection
    {
		m_data &= rhs.m_data;
		return *this;
    }

    wxSet operator +(const wxSet &rhs) const // union
    {
		wxSet<T> s;
		s.m_data = m_data | rhs.m_data;
		return s;
    }
    wxSet operator -(const wxSet &rhs) const // difference
    {
		wxSet<T> s;
		s.m_data = m_data ^ rhs.m_data;
		return s;
    }
    wxSet operator *(const wxSet &rhs) const // intersection
    {
		wxSet<T> s;
		s.m_data = m_data & rhs.m_data;
		return s;
    }

    wxSet& Set(const T el) //Add element
    {
		m_data |= 1 << el;
		return *this;
    }
    wxSet& Clear(const T el) //remove element
    {
		m_data &= ~(1 << el);
		return *this;
    }

    bool Contains(const T el) const
    {
		return (m_data & (1 << el)) ? true : false;
    }

    wxSet &Clear()
    {
		m_data = 0;
		return *this;
    }

    bool Empty() const
    {
		return m_data == 0;
    }

    bool operator ==(const wxSet &rhs) const
    {
		return m_data == rhs.m_data;
    }

    bool operator !=(const wxSet &rhs) const
    {
		return !operator==(rhs);
    }
private :
	int m_data;
};


#endif