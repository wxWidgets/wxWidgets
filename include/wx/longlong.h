/////////////////////////////////////////////////////////////////////////////
// Name:        wx/longlong.h
// Purpose:     declaration of wxLongLong class - best implementation of a 64
//              bit integer for the current platform.
// Author:      Jeffrey C. Ollie <jeff@ollie.clive.ia.us>, Vadim Zeitlin
// Remarks:     this class is not public in wxWindows 2.0! It is intentionally
//              not documented and is for private use only.
// Modified by:
// Created:     10.02.99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LONGLONG_H
#define _WX_LONGLONG_H

#ifdef __GNUG__
    #pragma interface "longlong.h"
#endif

// ----------------------------------------------------------------------------
// decide upon which class we will use
// ----------------------------------------------------------------------------

// to avoid compilation problems on 64bit machines with ambiguous method calls
// we will need this
#undef wxLongLongIsLong

// NB: we #define and not typedef wxLongLong_t because we want to be able to
//     use 'unsigned wxLongLong_t' as well
#if defined(SIZEOF_LONG) && (SIZEOF_LONG == 8)
    #define wxLongLong_t long
    #define wxLongLongIsLong
#elif defined(__VISUALC__)
    #define wxLongLong_t __int64
#elif defined(__GNUG__)
    #define wxLongLong_t long long
#elif defined(__MWERKS__)
    #if __option(longlong)
        #define wxLongLong_t long long
    #else
        #error "The 64 bit integer support in CodeWarrior has been disabled."
        #error "See the documentation on the 'longlong' pragma."
    #endif
#else
    #warning "Your compiler does not appear to support 64 bit integers, "\
             "using emulation class instead."
    #define wxUSE_LONGLONG_WX 1
#endif // compiler

// the user may predefine wxUSE_LONGLONG_NATIVE and/or wxUSE_LONGLONG_NATIVE
// to disable automatic testing (useful for the test program which defines
// both classes) but by default we only use one class
#ifndef wxLongLong_t
    #undef wxUSE_LONGLONG_NATIVE
    #define wxUSE_LONGLONG_NATIVE 0
    class WXDLLEXPORT wxLongLongWx;
    typedef wxLongLongWx wxLongLong;
#endif

#ifndef wxUSE_LONGLONG_WX
    #define wxUSE_LONGLONG_WX 0
    class WXDLLEXPORT wxLongLongNative;
    typedef wxLongLongNative wxLongLong;
#endif

// NB: if both wxUSE_LONGLONG_WX and NATIVE are defined, the user code should
//     typedef wxLongLong as it wants, we don't do it

// ----------------------------------------------------------------------------
// choose the appropriate class
// ----------------------------------------------------------------------------

// we use iostream for wxLongLong output
#include "wx/ioswrap.h"

#if wxUSE_LONGLONG_NATIVE

class WXDLLEXPORT wxLongLongNative
{
public:
    // ctors
        // default ctor initializes to 0
    wxLongLongNative() { m_ll = 0; }
        // from long long
    wxLongLongNative(wxLongLong_t ll) { m_ll = ll; }
        // from 2 longs
    wxLongLongNative(long hi, unsigned long lo)
    {
        // assign first to avoid precision loss!
        m_ll = ((wxLongLong_t) hi) << 32;
        m_ll |= (wxLongLong_t) lo;
    }

    // default copy ctor is ok in both cases

    // no dtor

    // assignment operators
        // from native 64 bit integer
    wxLongLongNative& operator=(wxLongLong_t ll)
        { m_ll = ll; return *this; }

    // assignment operators from wxLongLongNative is ok

    // accessors
        // get high part
    long GetHi() const
        { return (long)((m_ll & 0xFFFFFFFF00000000l) >> 32); }
        // get low part
    unsigned long GetLo() const
        { return (unsigned long) (m_ll & 0x00000000FFFFFFFFl); }

        // convert to native long long
    wxLongLong_t GetValue() const { return m_ll; }

    operator wxLongLong_t() const { return m_ll; }

    // operations
        // addition
    wxLongLongNative operator+(const wxLongLongNative& ll) const
        { return wxLongLongNative(m_ll + ll.m_ll); }
    wxLongLongNative& operator+=(const wxLongLongNative& ll)
        { m_ll += ll.m_ll; return *this; }

    wxLongLongNative operator+(const wxLongLong_t ll) const
        { return wxLongLongNative(m_ll + ll); }
    wxLongLongNative& operator+=(const wxLongLong_t ll)
        { m_ll += ll; return *this; }

        // pre increment
    wxLongLongNative& operator++()
        { m_ll++; return *this; }

        // post increment
    wxLongLongNative& operator++(int)
        { m_ll++; return *this; }

        // negation operator
    wxLongLongNative operator-() const
        { return wxLongLongNative(-m_ll); }

        // subtraction
    wxLongLongNative operator-(const wxLongLongNative& ll) const
        { return wxLongLongNative(m_ll - ll.m_ll); }
    wxLongLongNative& operator-=(const wxLongLongNative& ll)
        { m_ll -= ll.m_ll; return *this; }

    wxLongLongNative operator-(const wxLongLong_t ll) const
        { return wxLongLongNative(m_ll - ll); }
    wxLongLongNative& operator-=(const wxLongLong_t ll)
        { m_ll -= ll; return *this; }

        // pre decrement
    wxLongLongNative& operator--()
        { m_ll--; return *this; }

        // post decrement
    wxLongLongNative& operator--(int)
        { m_ll--; return *this; }

    // shifts
        // left shift
    wxLongLongNative operator<<(int shift) const
        { return wxLongLongNative(m_ll << shift);; }
    wxLongLongNative& operator<<=(int shift)
        { m_ll <<= shift; return *this; }

        // right shift
    wxLongLongNative operator>>(int shift) const
        { return wxLongLongNative(m_ll >> shift);; }
    wxLongLongNative& operator>>=(int shift)
        { m_ll >>= shift; return *this; }

    // bitwise operators
    wxLongLongNative operator&(const wxLongLongNative& ll) const
        { return wxLongLongNative(m_ll & ll.m_ll); }
    wxLongLongNative& operator&=(const wxLongLongNative& ll)
        { m_ll &= ll.m_ll; return *this; }

    wxLongLongNative operator|(const wxLongLongNative& ll) const
        { return wxLongLongNative(m_ll | ll.m_ll); }
    wxLongLongNative& operator|=(const wxLongLongNative& ll)
        { m_ll |= ll.m_ll; return *this; }

    wxLongLongNative operator^(const wxLongLongNative& ll) const
        { return wxLongLongNative(m_ll ^ ll.m_ll); }
    wxLongLongNative& operator^=(const wxLongLongNative& ll)
        { m_ll ^= ll.m_ll; return *this; }

    // multiplication/division TODO
    wxLongLongNative operator*(const wxLongLongNative& ll) const
        { return wxLongLongNative(m_ll * ll.m_ll); }
    wxLongLongNative& operator*=(const wxLongLongNative& ll)
        { m_ll *= ll.m_ll; return *this; }

    wxLongLongNative operator/(const wxLongLongNative& ll) const
        { return wxLongLongNative(m_ll / ll.m_ll); }
    wxLongLongNative& operator/=(const wxLongLongNative& ll)
        { m_ll /= ll.m_ll; return *this; }

    wxLongLongNative operator%(const wxLongLongNative& ll) const
        { return wxLongLongNative(m_ll % ll.m_ll); }

    // comparison
    bool operator==(const wxLongLongNative& ll) const
        { return m_ll == ll.m_ll; }
    bool operator!=(const wxLongLongNative& ll) const
        { return m_ll != ll.m_ll; }
    bool operator<(const wxLongLongNative& ll) const
        { return m_ll < ll.m_ll; }
    bool operator>(const wxLongLongNative& ll) const
        { return m_ll > ll.m_ll; }
    bool operator<=(const wxLongLongNative& ll) const
        { return m_ll <= ll.m_ll; }
    bool operator>=(const wxLongLongNative& ll) const
        { return m_ll >= ll.m_ll; }

    // miscellaneous
        // conversion to byte array: returns a pointer to static buffer!
    void *asArray() const;

        // input/output
    friend ostream& operator<<(ostream&, const wxLongLongNative&);

private:
    wxLongLong_t  m_ll;
};

#endif // wxUSE_LONGLONG_NATIVE

#if wxUSE_LONGLONG_WX

class WXDLLEXPORT wxLongLongWx
{
public:
    // ctors
        // default ctor initializes to 0
    wxLongLongWx() { m_lo = m_hi = 0; }
        // from long
    wxLongLongWx(long l)
        { m_lo = l; m_hi = (l < 0 ? -1l : 0l); }
        // from 2 longs
    wxLongLongWx(long hi, unsigned long lo)
        { m_hi = hi; m_lo = lo; }

    // default copy ctor is ok in both cases

    // no dtor

    // assignment operators
        // from long
    wxLongLongWx& operator=(long l)
        { m_lo = l; m_hi = (l < 0 ? -1l : 0l); return *this; }
        // can't have assignment operator from 2 longs

    // accessors
        // get high part
    long GetHi() const { return m_hi; }
        // get low part
    unsigned long GetLo() const { return m_lo; }

    // operations
        // addition
    wxLongLongWx operator+(const wxLongLongWx& ll) const;
    wxLongLongWx& operator+=(const wxLongLongWx& ll);
    wxLongLongWx operator+(long l) const;
    wxLongLongWx& operator+=(long l);

        // pre increment operator
    wxLongLongWx& operator++();

        // post increment operator
    wxLongLongWx& operator++(int);

        // negation operator
    wxLongLongWx operator-() const;

        // subraction
    wxLongLongWx operator-(const wxLongLongWx& ll) const;
    wxLongLongWx& operator-=(const wxLongLongWx& ll);

        // pre decrement operator
    wxLongLongWx& operator--();

        // post decrement operator
    wxLongLongWx& operator--(int);

    // shifts
        // left shift
    wxLongLongWx operator<<(int shift) const;
    wxLongLongWx& operator<<=(int shift);

        // right shift
    wxLongLongWx operator>>(int shift) const;
    wxLongLongWx& operator>>=(int shift);

    // bitwise operators
    wxLongLongWx operator&(const wxLongLongWx& ll) const;
    wxLongLongWx& operator&=(const wxLongLongWx& ll);
    wxLongLongWx operator|(const wxLongLongWx& ll) const;
    wxLongLongWx& operator|=(const wxLongLongWx& ll);
    wxLongLongWx operator^(const wxLongLongWx& ll) const;
    wxLongLongWx& operator^=(const wxLongLongWx& ll);
    wxLongLongWx operator~() const;

    // comparison
    bool operator==(const wxLongLongWx& ll) const;
    bool operator!=(const wxLongLongWx& ll) const;
    bool operator<(const wxLongLongWx& ll) const;
    bool operator>(const wxLongLongWx& ll) const;
    bool operator<=(const wxLongLongWx& ll) const;
    bool operator>=(const wxLongLongWx& ll) const;

    // multiplication
    wxLongLongWx operator*(const wxLongLongWx& ll) const;
    wxLongLongWx& operator*=(const wxLongLongWx& ll);
    void *asArray(void) const;

    // division
    void Divide(const wxLongLongWx& divisor,
                wxLongLongWx& quotient,
                wxLongLongWx& remainder) const;

    // input/output
    friend ostream& operator<<(ostream&, const wxLongLongWx&);

private:
    // long is at least 32 bits, so represent our 64bit number as 2 longs

    long m_hi;                // signed bit is in the high part
    unsigned long m_lo;
};

#endif // wxUSE_LONGLONG_WX

#endif // _WX_LONGLONG_H
