/////////////////////////////////////////////////////////////////////////////
// Name:        wx/longlong.cpp
// Purpose:     implementation of wxLongLongNative
// Author:      Jeffrey C. Ollie <jeff@ollie.clive.ia.us>, Vadim Zeitlin
// Remarks:     this class is not public in wxWidgets 2.0! It is intentionally
//              not documented and is for private use only.
// Modified by:
// Created:     10.02.99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers
// ============================================================================

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "longlong.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LONGLONG
#include "wx/longlong.h"
#include "wx/math.h"       // for fabs()

#if defined(__MWERKS__) && defined(__WXMSW__)
#include <string.h>     // for memset()
#else
#include <memory.h>     // for memset()
#endif

// ============================================================================
// implementation
// ============================================================================

#if wxUSE_LONGLONG_NATIVE

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void *wxLongLongNative::asArray() const
{
    static unsigned char temp[8];

    temp[0] = (unsigned char)((m_ll >> 56) & 0xFF);
    temp[1] = (unsigned char)((m_ll >> 48) & 0xFF);
    temp[2] = (unsigned char)((m_ll >> 40) & 0xFF);
    temp[3] = (unsigned char)((m_ll >> 32) & 0xFF);
    temp[4] = (unsigned char)((m_ll >> 24) & 0xFF);
    temp[5] = (unsigned char)((m_ll >> 16) & 0xFF);
    temp[6] = (unsigned char)((m_ll >> 8)  & 0xFF);
    temp[7] = (unsigned char)((m_ll >> 0)  & 0xFF);

    return temp;
}

void *wxULongLongNative::asArray() const
{
    static unsigned char temp[8];

    temp[0] = (unsigned char)((m_ll >> 56) & 0xFF);
    temp[1] = (unsigned char)((m_ll >> 48) & 0xFF);
    temp[2] = (unsigned char)((m_ll >> 40) & 0xFF);
    temp[3] = (unsigned char)((m_ll >> 32) & 0xFF);
    temp[4] = (unsigned char)((m_ll >> 24) & 0xFF);
    temp[5] = (unsigned char)((m_ll >> 16) & 0xFF);
    temp[6] = (unsigned char)((m_ll >> 8)  & 0xFF);
    temp[7] = (unsigned char)((m_ll >> 0)  & 0xFF);

    return temp;
}

#if wxUSE_LONGLONG_WX
wxLongLongNative::wxLongLongNative(wxLongLongWx ll)
{
    // assign first to avoid precision loss!
    m_ll = ll.GetHi();
    m_ll <<= 32;
    m_ll |= ll.GetLo();
}

wxLongLongNative& wxLongLongNative::operator=(wxLongLongWx ll)
{
    // assign first to avoid precision loss!
    m_ll = ll.GetHi();
    m_ll <<= 32;
    m_ll |= ll.GetLo();
    return *this;
}
#endif

#endif // wxUSE_LONGLONG_NATIVE

// ============================================================================
// wxLongLongWx: emulation of 'long long' using 2 longs
// ============================================================================

#if wxUSE_LONGLONG_WX

// assignment
wxLongLongWx& wxLongLongWx::Assign(double d)
{
    bool positive = d >= 0;
    d = fabs(d);
    if ( d <= ULONG_MAX )
    {
        m_hi = 0;
        m_lo = (long)d;
    }
    else
    {
        m_hi = (unsigned long)(d / (1.0 + (double)ULONG_MAX));
        m_lo = (unsigned long)(d - ((double)m_hi * (1.0 + (double)ULONG_MAX)));
    }

#ifdef wxLONGLONG_TEST_MODE
    m_ll = (wxLongLong_t)d;

    Check();
#endif // wxLONGLONG_TEST_MODE

    if ( !positive )
        Negate();

    return *this;
}

wxLongLongWx wxLongLongWx::operator<<(int shift) const
{
    wxLongLongWx ll(*this);
    ll <<= shift;

    return ll;
}

wxULongLongWx wxULongLongWx::operator<<(int shift) const
{
    wxULongLongWx ll(*this);
    ll <<= shift;

    return ll;
}

wxLongLongWx& wxLongLongWx::operator<<=(int shift)
{
    if (shift != 0)
    {
        if (shift < 32)
        {
            m_hi <<= shift;
            m_hi |= m_lo >> (32 - shift);
            m_lo <<= shift;
        }
        else
        {
            m_hi = m_lo << (shift - 32);
            m_lo = 0;
        }
    }

#ifdef wxLONGLONG_TEST_MODE
    m_ll <<= shift;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator<<=(int shift)
{
    if (shift != 0)
    {
        if (shift < 32)
        {
            m_hi <<= shift;
            m_hi |= m_lo >> (32 - shift);
            m_lo <<= shift;
        }
        else
        {
            m_hi = m_lo << (shift - 32);
            m_lo = 0;
        }
    }

#ifdef wxLONGLONG_TEST_MODE
    m_ll <<= shift;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxLongLongWx wxLongLongWx::operator>>(int shift) const
{
    wxLongLongWx ll(*this);
    ll >>= shift;

    return ll;
}

wxULongLongWx wxULongLongWx::operator>>(int shift) const
{
    wxULongLongWx ll(*this);
    ll >>= shift;

    return ll;
}

wxLongLongWx& wxLongLongWx::operator>>=(int shift)
{
    if (shift != 0)
    {
        if (shift < 32)
        {
            m_lo >>= shift;
            m_lo |= m_hi << (32 - shift);
            m_hi >>= shift;
        }
        else
        {
            m_lo = m_hi >> (shift - 32);
            m_hi = (m_hi < 0 ? -1L : 0);
        }
    }

#ifdef wxLONGLONG_TEST_MODE
    m_ll >>= shift;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator>>=(int shift)
{
    if (shift != 0)
    {
        if (shift < 32)
        {
            m_lo >>= shift;
            m_lo |= m_hi << (32 - shift);
            m_hi >>= shift;
        }
        else
        {
            m_lo = m_hi >> (shift - 32);
            m_hi = 0;
        }
    }

#ifdef wxLONGLONG_TEST_MODE
    m_ll >>= shift;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxLongLongWx wxLongLongWx::operator+(const wxLongLongWx& ll) const
{
    wxLongLongWx res(*this);
    res += ll;

    return res;
}

wxULongLongWx wxULongLongWx::operator+(const wxULongLongWx& ll) const
{
    wxULongLongWx res(*this);
    res += ll;

    return res;
}

wxLongLongWx wxLongLongWx::operator+(long l) const
{
    wxLongLongWx res(*this);
    res += l;

    return res;
}

wxULongLongWx wxULongLongWx::operator+(unsigned long l) const
{
    wxULongLongWx res(*this);
    res += l;

    return res;
}

wxLongLongWx& wxLongLongWx::operator+=(const wxLongLongWx& ll)
{
    unsigned long previous = m_lo;

    m_lo += ll.m_lo;
    m_hi += ll.m_hi;

    if ((m_lo < previous) || (m_lo < ll.m_lo))
        m_hi++;

#ifdef wxLONGLONG_TEST_MODE
    m_ll += ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator+=(const wxULongLongWx& ll)
{
    unsigned long previous = m_lo;

    m_lo += ll.m_lo;
    m_hi += ll.m_hi;

    if ((m_lo < previous) || (m_lo < ll.m_lo))
        m_hi++;

#ifdef wxLONGLONG_TEST_MODE
    m_ll += ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxLongLongWx& wxLongLongWx::operator+=(long l)
{
    unsigned long previous = m_lo;

    m_lo += l;
    if (l < 0)
        m_hi += -1l;

    if ((m_lo < previous) || (m_lo < (unsigned long)l))
        m_hi++;

#ifdef wxLONGLONG_TEST_MODE
    m_ll += l;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator+=(unsigned long l)
{
    unsigned long previous = m_lo;

    m_lo += l;

    if ((m_lo < previous) || (m_lo < l))
        m_hi++;

#ifdef wxLONGLONG_TEST_MODE
    m_ll += l;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

// pre increment
wxLongLongWx& wxLongLongWx::operator++()
{
    m_lo++;
    if (m_lo == 0)
        m_hi++;

#ifdef wxLONGLONG_TEST_MODE
    m_ll++;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator++()
{
    m_lo++;
    if (m_lo == 0)
        m_hi++;

#ifdef wxLONGLONG_TEST_MODE
    m_ll++;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

// negation
wxLongLongWx wxLongLongWx::operator-() const
{
    wxLongLongWx res(*this);
    res.Negate();

    return res;
}

wxLongLongWx& wxLongLongWx::Negate()
{
    m_hi = ~m_hi;
    m_lo = ~m_lo;

    m_lo++;
    if ( m_lo == 0 )
        m_hi++;

#ifdef wxLONGLONG_TEST_MODE
    m_ll = -m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

// subtraction

wxLongLongWx wxLongLongWx::operator-(const wxLongLongWx& ll) const
{
    wxLongLongWx res(*this);
    res -= ll;

    return res;
}

wxLongLongWx wxULongLongWx::operator-(const wxULongLongWx& ll) const
{
    wxASSERT(m_hi <= LONG_MAX );
    wxASSERT(ll.m_hi <= LONG_MAX );

    wxLongLongWx res( (long)m_hi , m_lo );
    wxLongLongWx op( (long)ll.m_hi , ll.m_lo );
    res -= op;

    return res;
}

wxLongLongWx& wxLongLongWx::operator-=(const wxLongLongWx& ll)
{
    unsigned long previous = m_lo;

    m_lo -= ll.m_lo;
    m_hi -= ll.m_hi;

    if (previous < ll.m_lo)
        m_hi--;

#ifdef wxLONGLONG_TEST_MODE
    m_ll -= ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator-=(const wxULongLongWx& ll)
{
    unsigned long previous = m_lo;

    m_lo -= ll.m_lo;
    m_hi -= ll.m_hi;

    if (previous < ll.m_lo)
        m_hi--;

#ifdef wxLONGLONG_TEST_MODE
    m_ll -= ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

// pre decrement
wxLongLongWx& wxLongLongWx::operator--()
{
    m_lo--;
    if (m_lo == 0xFFFFFFFF)
        m_hi--;

#ifdef wxLONGLONG_TEST_MODE
    m_ll--;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator--()
{
    m_lo--;
    if (m_lo == 0xFFFFFFFF)
        m_hi--;

#ifdef wxLONGLONG_TEST_MODE
    m_ll--;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

// comparison operators

bool wxLongLongWx::operator<(const wxLongLongWx& ll) const
{
    if ( m_hi < ll.m_hi )
        return true;
    else if ( m_hi == ll.m_hi )
        return m_lo < ll.m_lo;
    else
        return false;
}

bool wxULongLongWx::operator<(const wxULongLongWx& ll) const
{
    if ( m_hi < ll.m_hi )
        return true;
    else if ( m_hi == ll.m_hi )
        return m_lo < ll.m_lo;
    else
        return false;
}

bool wxLongLongWx::operator>(const wxLongLongWx& ll) const
{
    if ( m_hi > ll.m_hi )
        return true;
    else if ( m_hi == ll.m_hi )
        return m_lo > ll.m_lo;
    else
        return false;
}

bool wxULongLongWx::operator>(const wxULongLongWx& ll) const
{
    if ( m_hi > ll.m_hi )
        return true;
    else if ( m_hi == ll.m_hi )
        return m_lo > ll.m_lo;
    else
        return false;
}

// bitwise operators

wxLongLongWx wxLongLongWx::operator&(const wxLongLongWx& ll) const
{
    return wxLongLongWx(m_hi & ll.m_hi, m_lo & ll.m_lo);
}

wxULongLongWx wxULongLongWx::operator&(const wxULongLongWx& ll) const
{
    return wxULongLongWx(m_hi & ll.m_hi, m_lo & ll.m_lo);
}

wxLongLongWx wxLongLongWx::operator|(const wxLongLongWx& ll) const
{
    return wxLongLongWx(m_hi | ll.m_hi, m_lo | ll.m_lo);
}

wxULongLongWx wxULongLongWx::operator|(const wxULongLongWx& ll) const
{
    return wxULongLongWx(m_hi | ll.m_hi, m_lo | ll.m_lo);
}

wxLongLongWx wxLongLongWx::operator^(const wxLongLongWx& ll) const
{
    return wxLongLongWx(m_hi ^ ll.m_hi, m_lo ^ ll.m_lo);
}

wxULongLongWx wxULongLongWx::operator^(const wxULongLongWx& ll) const
{
    return wxULongLongWx(m_hi ^ ll.m_hi, m_lo ^ ll.m_lo);
}

wxLongLongWx& wxLongLongWx::operator&=(const wxLongLongWx& ll)
{
    m_lo &= ll.m_lo;
    m_hi &= ll.m_hi;

#ifdef wxLONGLONG_TEST_MODE
    m_ll &= ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator&=(const wxULongLongWx& ll)
{
    m_lo &= ll.m_lo;
    m_hi &= ll.m_hi;

#ifdef wxLONGLONG_TEST_MODE
    m_ll &= ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxLongLongWx& wxLongLongWx::operator|=(const wxLongLongWx& ll)
{
    m_lo |= ll.m_lo;
    m_hi |= ll.m_hi;

#ifdef wxLONGLONG_TEST_MODE
    m_ll |= ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator|=(const wxULongLongWx& ll)
{
    m_lo |= ll.m_lo;
    m_hi |= ll.m_hi;

#ifdef wxLONGLONG_TEST_MODE
    m_ll |= ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxLongLongWx& wxLongLongWx::operator^=(const wxLongLongWx& ll)
{
    m_lo ^= ll.m_lo;
    m_hi ^= ll.m_hi;

#ifdef wxLONGLONG_TEST_MODE
    m_ll ^= ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator^=(const wxULongLongWx& ll)
{
    m_lo ^= ll.m_lo;
    m_hi ^= ll.m_hi;

#ifdef wxLONGLONG_TEST_MODE
    m_ll ^= ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxLongLongWx wxLongLongWx::operator~() const
{
    return wxLongLongWx(~m_hi, ~m_lo);
}

wxULongLongWx wxULongLongWx::operator~() const
{
    return wxULongLongWx(~m_hi, ~m_lo);
}

// multiplication

wxLongLongWx wxLongLongWx::operator*(const wxLongLongWx& ll) const
{
    wxLongLongWx res(*this);
    res *= ll;

    return res;
}

wxULongLongWx wxULongLongWx::operator*(const wxULongLongWx& ll) const
{
    wxULongLongWx res(*this);
    res *= ll;

    return res;
}

wxLongLongWx& wxLongLongWx::operator*=(const wxLongLongWx& ll)
{
    wxLongLongWx t(m_hi, m_lo);
    wxLongLongWx q(ll.m_hi, ll.m_lo);

    m_hi = m_lo = 0;

#ifdef wxLONGLONG_TEST_MODE
    wxLongLong_t llOld = m_ll;
    m_ll = 0;
#endif // wxLONGLONG_TEST_MODE

    int counter = 0;
    do
    {
        if ((q.m_lo & 1) != 0)
            *this += t;
        q >>= 1;
        t <<= 1;
        counter++;
    }
    while ((counter < 64) && ((q.m_hi != 0) || (q.m_lo != 0)));

#ifdef wxLONGLONG_TEST_MODE
    m_ll = llOld * ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

wxULongLongWx& wxULongLongWx::operator*=(const wxULongLongWx& ll)
{
    wxULongLongWx t(m_hi, m_lo);
    wxULongLongWx q(ll.m_hi, ll.m_lo);

    m_hi = m_lo = 0;

#ifdef wxLONGLONG_TEST_MODE
    wxULongLong_t llOld = m_ll;
    m_ll = 0;
#endif // wxLONGLONG_TEST_MODE

    int counter = 0;
    do
    {
        if ((q.m_lo & 1) != 0)
            *this += t;
        q >>= 1;
        t <<= 1;
        counter++;
    }
    while ((counter < 64) && ((q.m_hi != 0) || (q.m_lo != 0)));

#ifdef wxLONGLONG_TEST_MODE
    m_ll = llOld * ll.m_ll;

    Check();
#endif // wxLONGLONG_TEST_MODE

    return *this;
}

// division

void wxLongLongWx::Divide(const wxLongLongWx& divisorIn,
                          wxLongLongWx& quotient,
                          wxLongLongWx& remainder) const
{
    if ((divisorIn.m_lo == 0) && (divisorIn.m_hi == 0))
    {
        // provoke division by zero error and silence the compilers warnings
        // about an expression without effect and unused variable
        long dummy = divisorIn.m_lo/divisorIn.m_hi;
        dummy += 0;
    }

    // VZ: I'm writing this in a hurry and it's surely not the fastest way to
    //     do this - any improvements are more than welcome
    //
    //     code inspired by the snippet at
    //          http://www.bearcave.com/software/divide.htm
    //
    //     Copyright notice:
    //
    //     Use of this program, for any purpose, is granted the author, Ian
    //     Kaplan, as long as this copyright notice is included in the source
    //     code or any source code derived from this program. The user assumes
    //     all responsibility for using this code.

    // init everything
    wxLongLongWx dividend = *this,
                 divisor = divisorIn;

    quotient = 0l;
    remainder = 0l;

    // always do unsigned division and adjust the signs later: in C integer
    // division, the sign of the remainder is the same as the sign of the
    // dividend, while the sign of the quotient is the product of the signs of
    // the dividend and divisor. Of course, we also always have
    //
    //      dividend = quotient*divisor + remainder
    //
    // with 0 <= abs(remainder) < abs(divisor)
    bool negRemainder = dividend.m_hi < 0;
    bool negQuotient = false;   // assume positive
    if ( dividend.m_hi < 0 )
    {
        negQuotient = !negQuotient;
        dividend = -dividend;
    }
    if ( divisor.m_hi < 0 )
    {
        negQuotient = !negQuotient;
        divisor = -divisor;
    }

    // check for some particular cases
    if ( divisor > dividend )
    {
        remainder = dividend;
    }
    else if ( divisor == dividend )
    {
        quotient = 1l;
    }
    else
    {
        // here: dividend > divisor and both are positive: do unsigned division
        size_t nBits = 64u;
        wxLongLongWx d;

        #define IS_MSB_SET(ll)  ((ll.m_hi) & (1 << (8*sizeof(long) - 1)))

        while ( remainder < divisor )
        {
            remainder <<= 1;
            if ( IS_MSB_SET(dividend) )
            {
                remainder |= 1;
            }

            d = dividend;
            dividend <<= 1;

            nBits--;
        }

        // undo the last loop iteration
        dividend = d;
        remainder >>= 1;
        nBits++;

        for ( size_t i = 0; i < nBits; i++ )
        {
            remainder <<= 1;
            if ( IS_MSB_SET(dividend) )
            {
                remainder |= 1;
            }

            wxLongLongWx t = remainder - divisor;
            dividend <<= 1;
            quotient <<= 1;
            if ( !IS_MSB_SET(t) )
            {
                quotient |= 1;

                remainder = t;
            }
        }
    }

    // adjust signs
    if ( negRemainder )
    {
        remainder = -remainder;
    }

    if ( negQuotient )
    {
        quotient = -quotient;
    }
}

void wxULongLongWx::Divide(const wxULongLongWx& divisorIn,
                           wxULongLongWx& quotient,
                           wxULongLongWx& remainder) const
{
    if ((divisorIn.m_lo == 0) && (divisorIn.m_hi == 0))
    {
        // provoke division by zero error and silence the compilers warnings
        // about an expression without effect and unused variable
        unsigned long dummy = divisorIn.m_lo/divisorIn.m_hi;
        dummy += 0;
    }

    // VZ: I'm writing this in a hurry and it's surely not the fastest way to
    //     do this - any improvements are more than welcome
    //
    //     code inspired by the snippet at
    //          http://www.bearcave.com/software/divide.htm
    //
    //     Copyright notice:
    //
    //     Use of this program, for any purpose, is granted the author, Ian
    //     Kaplan, as long as this copyright notice is included in the source
    //     code or any source code derived from this program. The user assumes
    //     all responsibility for using this code.

    // init everything
    wxULongLongWx dividend = *this,
                  divisor = divisorIn;

    quotient = 0l;
    remainder = 0l;

    // check for some particular cases
    if ( divisor > dividend )
    {
        remainder = dividend;
    }
    else if ( divisor == dividend )
    {
        quotient = 1l;
    }
    else
    {
        // here: dividend > divisor
        size_t nBits = 64u;
        wxULongLongWx d;

        #define IS_MSB_SET(ll)  ((ll.m_hi) & (1 << (8*sizeof(long) - 1)))

        while ( remainder < divisor )
        {
            remainder <<= 1;
            if ( IS_MSB_SET(dividend) )
            {
                remainder |= 1;
            }

            d = dividend;
            dividend <<= 1;

            nBits--;
        }

        // undo the last loop iteration
        dividend = d;
        remainder >>= 1;
        nBits++;

        for ( size_t i = 0; i < nBits; i++ )
        {
            remainder <<= 1;
            if ( IS_MSB_SET(dividend) )
            {
                remainder |= 1;
            }

            wxULongLongWx t = remainder - divisor;
            dividend <<= 1;
            quotient <<= 1;
            if ( !IS_MSB_SET(t) )
            {
                quotient |= 1;

                remainder = t;
            }
        }
    }
}

wxLongLongWx wxLongLongWx::operator/(const wxLongLongWx& ll) const
{
    wxLongLongWx quotient, remainder;

    Divide(ll, quotient, remainder);

    return quotient;
}

wxULongLongWx wxULongLongWx::operator/(const wxULongLongWx& ll) const
{
    wxULongLongWx quotient, remainder;

    Divide(ll, quotient, remainder);

    return quotient;
}

wxLongLongWx& wxLongLongWx::operator/=(const wxLongLongWx& ll)
{
    wxLongLongWx quotient, remainder;

    Divide(ll, quotient, remainder);

    *this = quotient;

    return *this;
}

wxULongLongWx& wxULongLongWx::operator/=(const wxULongLongWx& ll)
{
    wxULongLongWx quotient, remainder;

    Divide(ll, quotient, remainder);

    *this = quotient;

    return *this;
}

wxLongLongWx wxLongLongWx::operator%(const wxLongLongWx& ll) const
{
    wxLongLongWx quotient, remainder;

    Divide(ll, quotient, remainder);

    return remainder;
}

wxULongLongWx wxULongLongWx::operator%(const wxULongLongWx& ll) const
{
    wxULongLongWx quotient, remainder;

    Divide(ll, quotient, remainder);

    return remainder;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

// temporary - just for testing
void *wxLongLongWx::asArray(void) const
{
    static unsigned char temp[8];

    temp[0] = (char)((m_hi >> 24) & 0xFF);
    temp[1] = (char)((m_hi >> 16) & 0xFF);
    temp[2] = (char)((m_hi >> 8)  & 0xFF);
    temp[3] = (char)((m_hi >> 0)  & 0xFF);
    temp[4] = (char)((m_lo >> 24) & 0xFF);
    temp[5] = (char)((m_lo >> 16) & 0xFF);
    temp[6] = (char)((m_lo >> 8)  & 0xFF);
    temp[7] = (char)((m_lo >> 0)  & 0xFF);

    return temp;
}

void *wxULongLongWx::asArray(void) const
{
    static unsigned char temp[8];

    temp[0] = (char)((m_hi >> 24) & 0xFF);
    temp[1] = (char)((m_hi >> 16) & 0xFF);
    temp[2] = (char)((m_hi >> 8)  & 0xFF);
    temp[3] = (char)((m_hi >> 0)  & 0xFF);
    temp[4] = (char)((m_lo >> 24) & 0xFF);
    temp[5] = (char)((m_lo >> 16) & 0xFF);
    temp[6] = (char)((m_lo >> 8)  & 0xFF);
    temp[7] = (char)((m_lo >> 0)  & 0xFF);

    return temp;
}

#endif // wxUSE_LONGLONG_WX

#define LL_TO_STRING(name)                                           \
    wxString name::ToString() const                                  \
    {                                                                \
        /* TODO: this is awfully inefficient, anything better? */    \
        wxString result;                                             \
                                                                     \
        name ll = *this;                                             \
                                                                     \
        bool neg;                                                    \
        if ( ll < 0 )                                                \
        {                                                            \
            ll.Negate();                                             \
            neg = true;                                              \
        }                                                            \
        else                                                         \
        {                                                            \
            neg = false;                                             \
        }                                                            \
                                                                     \
        while ( ll != 0 )                                            \
        {                                                            \
            result.Prepend((wxChar)(_T('0') + (ll % 10).ToLong()));  \
            ll /= 10;                                                \
        }                                                            \
                                                                     \
        if ( result.empty() )                                        \
            result = _T('0');                                        \
        else if ( neg )                                              \
            result.Prepend(_T('-'));                                 \
                                                                     \
        return result;                                               \
    }

#define ULL_TO_STRING(name)                                          \
    wxString name::ToString() const                                  \
    {                                                                \
        /* TODO: this is awfully inefficient, anything better? */    \
        wxString result;                                             \
                                                                     \
        name ll = *this;                                             \
                                                                     \
        while ( ll != 0 )                                            \
        {                                                            \
            result.Prepend((wxChar)(_T('0') + (ll % 10).ToULong())); \
            ll /= 10;                                                \
        }                                                            \
                                                                     \
        if ( result.empty() )                                        \
            result = _T('0');                                        \
                                                                     \
        return result;                                               \
    }

#if wxUSE_LONGLONG_NATIVE
    LL_TO_STRING(wxLongLongNative)
    ULL_TO_STRING(wxULongLongNative)
#endif

#if wxUSE_LONGLONG_WX
    LL_TO_STRING(wxLongLongWx)
    ULL_TO_STRING(wxULongLongWx)
#endif

#if wxUSE_STD_IOSTREAM

// input/output
WXDLLIMPEXP_BASE
wxSTD ostream& operator<< (wxSTD ostream& o, const wxLongLong& ll)
{
    return o << ll.ToString();
}

WXDLLIMPEXP_BASE
wxSTD ostream& operator<< (wxSTD ostream& o, const wxULongLong& ll)
{
    return o << ll.ToString();
}

#endif // wxUSE_STD_IOSTREAM

WXDLLIMPEXP_BASE wxString& operator<< (wxString& s, const wxLongLong& ll)
{
    return s << ll.ToString();
}

WXDLLIMPEXP_BASE wxString& operator<< (wxString& s, const wxULongLong& ll)
{
    return s << ll.ToString();
}

#endif // wxUSE_LONGLONG
