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

#include <bitset>

// wxFlags should be applied to an enum, then this can be used like
// bitwise operators but keeps the type safety and information, the
// enums must be in a sequence , their value determines the bit position
// that they represent
// The api is made as close as possible to <bitset> 

template <class T> class wxFlags
{
	friend class wxEnumData ;
public:
    // creates a wxFlags<> object with all flags initialized to 0
    wxFlags() { m_data = 0; }

    // created a wxFlags<> object initialized according to the bits of the 
    // integral value val
    wxFlags(unsigned long val) { m_data = val ; }

    // copies the content in the new wxFlags<> object from another one
    wxFlags(const wxFlags &src) { m_data = src.m_data; }

    // creates a wxFlags<> object that has the specific flag set
    wxFlags(const T el) { m_data |= 1 << el; }

    // returns the integral value that the bits of this object represent
    unsigned long to_ulong() const { return m_data ; }

    // assignment
    wxFlags &operator =(const wxFlags &rhs)
    {
		m_data = rhs.m_data;
		return *this;
    }

    // bitwise or operator, sets all bits that are in rhs and leaves
    // the rest unchanged
    wxFlags &operator |=(const wxFlags &rhs) 
    {
		m_data |= rhs.m_data;
		return *this;
    }

    // bitwsie exclusive-or operator, toggles the value of all bits
    // that are set in bits and leaves all others unchanged
    wxFlags &operator ^=(const wxFlags &rhs) // difference
    {
		m_data ^= rhs.m_data;
		return *this;
    }

    // bitwise and operator, resets all bits that are not in rhs and leaves
    // all others unchanged
    wxFlags &operator &=(const wxFlags &rhs) // intersection
    {
		m_data &= rhs.m_data;
		return *this;
    }

    // bitwise or operator, returns a new bitset that has all bits set that set are in 
    // bitset2 or in this bitset
   wxFlags operator |(const wxFlags &bitset2) const // union
    {
		wxFlags<T> s;
		s.m_data = m_data | bitset2.m_data;
		return s;
    }

    // bitwise exclusive-or operator, returns a new bitset that has all bits set that are set either in 
    // bitset2 or in this bitset but not in both
    wxFlags operator ^(const wxFlags &bitset2) const // difference
    {
		wxFlags<T> s;
		s.m_data = m_data ^ bitset2.m_data;
		return s;
    }

    // bitwise and operator, returns a new bitset that has all bits set that are set both in 
    // bitset2 and in this bitset
    wxFlags operator &(const wxFlags &bitset2) const // intersection
    {
		wxFlags<T> s;
		s.m_data = m_data & bitset2.m_data;
		return s;
    }

    // sets appropriate the bit to true
    wxFlags& set(const T el) //Add element
    {
		m_data |= 1 << el;
		return *this;
    }
    
    // clears the appropriate flag to false
    wxFlags& reset(const T el) //remove element
    {
		m_data &= ~(1 << el);
		return *this;
    }

    // clear all flags
    wxFlags& reset()
    {
		m_data = 0;
		return *this;
    }

    // true if this flag is set
    bool test(const T el) const
    {
		return (m_data & (1 << el)) ? true : false;
    }

    // true if no flag is set
    bool none() const
    {
		return m_data == 0;
    }

    // true if any flag is set
    bool any() const
    {
		return m_data != 0;
    }

    // true if both have the same flags
    bool operator ==(const wxFlags &rhs) const
    {
		return m_data == rhs.m_data;
    }

    // true if both differ in their flags set
    bool operator !=(const wxFlags &rhs) const
    {
		return !operator==(rhs);
    }

    bool operator[] (const T el) const { return test(el) ; }

private :
	unsigned long m_data;
};


#endif
