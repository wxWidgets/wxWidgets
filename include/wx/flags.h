/////////////////////////////////////////////////////////////////////////////
// Name:        wx/flags.h
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "flags.h"
#endif

// wxFlags should be applied to an enum, then this can be used like
// bitwise operators but keeps the type safety and information, the
// enums must be in a sequence , their value determines the bit position
// that they represent

template <class T> class wxFlags
{
	friend class wxEnumData ;
public:
    wxFlags(long val) { m_data = val ; }
    wxFlags() { m_data = 0; }
    wxFlags(const wxFlags &src) { m_data = src.m_data; }
    wxFlags(const T el) { m_data |= 1 << el; }

    operator long() const { return m_data ; }

    wxFlags &operator =(const wxFlags &rhs)
    {
		m_data = rhs.m_data;
		return *this;
    }
    wxFlags &operator +=(const wxFlags &rhs) // union
    {
		m_data |= rhs.m_data;
		return *this;
    }
    wxFlags &operator -=(const wxFlags &rhs) // difference
    {
		m_data ^= rhs.m_data;
		return *this;
    }

    wxFlags &operator *=(const wxFlags &rhs) // intersection
    {
		m_data &= rhs.m_data;
		return *this;
    }

    wxFlags operator +(const wxFlags &rhs) const // union
    {
		wxFlags<T> s;
		s.m_data = m_data | rhs.m_data;
		return s;
    }
    wxFlags operator -(const wxFlags &rhs) const // difference
    {
		wxFlags<T> s;
		s.m_data = m_data ^ rhs.m_data;
		return s;
    }
    wxFlags operator *(const wxFlags &rhs) const // intersection
    {
		wxFlags<T> s;
		s.m_data = m_data & rhs.m_data;
		return s;
    }

    wxFlags& Set(const T el) //Add element
    {
		m_data |= 1 << el;
		return *this;
    }
    wxFlags& Clear(const T el) //remove element
    {
		m_data &= ~(1 << el);
		return *this;
    }

    bool Contains(const T el) const
    {
		return (m_data & (1 << el)) ? true : false;
    }

    wxFlags &Clear()
    {
		m_data = 0;
		return *this;
    }

    bool Empty() const
    {
		return m_data == 0;
    }

    bool operator ==(const wxFlags &rhs) const
    {
		return m_data == rhs.m_data;
    }

    bool operator !=(const wxFlags &rhs) const
    {
		return !operator==(rhs);
    }
private :
	int m_data;
};


#endif
