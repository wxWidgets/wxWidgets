/////////////////////////////////////////////////////////////////////////////
// Name:        wx/longlong.cpp
// Purpose:     implementation of wxLongLongNative
// Author:      Jeffrey C. Ollie <jeff@ollie.clive.ia.us>, Vadim Zeitlin
// Remarks:     this class is not public in wxWindows 2.0! It is intentionally
//              not documented and is for private use only.
// Modified by:
// Created:     10.02.99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "longlong.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LONGLONG
#include "wx/longlong.h"

#include <memory.h>     // for memset()
#include <math.h>       // for fabs()

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

    temp[0] = (m_ll >> 56) & 0xFF;
    temp[1] = (m_ll >> 48) & 0xFF;
    temp[2] = (m_ll >> 40) & 0xFF;
    temp[3] = (m_ll >> 32) & 0xFF;
    temp[4] = (m_ll >> 24) & 0xFF;
    temp[5] = (m_ll >> 16) & 0xFF;
    temp[6] = (m_ll >> 8)  & 0xFF;
    temp[7] = (m_ll >> 0)  & 0xFF;

    return temp;
}

#if wxUSE_STD_IOSTREAM

// input/output
ostream& operator<< (ostream& o, const wxLongLongNative& ll)
{
    char result[65];

    memset(result, 'A', 64);

    result[64] = '\0';

    for (int i = 0; i < 64; i++)
    {
        result[63 - i] = '0' + (char) ((ll.m_ll >> i) & 1);
    }

    return o << result;
}

#endif // wxUSE_STD_IOSTREAM

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

wxLongLongWx wxLongLongWx::operator>>(int shift) const
{
    wxLongLongWx ll(*this);
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

wxLongLongWx wxLongLongWx::operator+(const wxLongLongWx& ll) const
{
    wxLongLongWx res(*this);
    res += ll;

    return res;
}

wxLongLongWx wxLongLongWx::operator+(long l) const
{
    wxLongLongWx res(*this);
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

// comparison operators

bool wxLongLongWx::operator<(const wxLongLongWx& ll) const
{
    if ( m_hi < ll.m_hi )
        return TRUE;
    else if ( m_hi == ll.m_hi )
        return m_lo < ll.m_lo;
    else
        return FALSE;
}

bool wxLongLongWx::operator>(const wxLongLongWx& ll) const
{
    if ( m_hi > ll.m_hi )
        return TRUE;
    else if ( m_hi == ll.m_hi )
        return m_lo > ll.m_lo;
    else
        return FALSE;
}

// bitwise operators

wxLongLongWx wxLongLongWx::operator&(const wxLongLongWx& ll) const
{
    return wxLongLongWx(m_hi & ll.m_hi, m_lo & ll.m_lo);
}

wxLongLongWx wxLongLongWx::operator|(const wxLongLongWx& ll) const
{
    return wxLongLongWx(m_hi | ll.m_hi, m_lo | ll.m_lo);
}

wxLongLongWx wxLongLongWx::operator^(const wxLongLongWx& ll) const
{
    return wxLongLongWx(m_hi ^ ll.m_hi, m_lo ^ ll.m_lo);
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

wxLongLongWx wxLongLongWx::operator~() const
{
    return wxLongLongWx(~m_hi, ~m_lo);
}

// multiplication

wxLongLongWx wxLongLongWx::operator*(const wxLongLongWx& ll) const
{
    wxLongLongWx res(*this);
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
    bool negQuotient = FALSE;   // assume positive
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
        // here: dividend > divisor and both are positibe: do unsigned division
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

wxLongLongWx wxLongLongWx::operator/(const wxLongLongWx& ll) const
{
    wxLongLongWx quotient, remainder;

    Divide(ll, quotient, remainder);

    return quotient;
}

wxLongLongWx& wxLongLongWx::operator/=(const wxLongLongWx& ll)
{
    wxLongLongWx quotient, remainder;

    Divide(ll, quotient, remainder);

    return *this = quotient;
}

wxLongLongWx wxLongLongWx::operator%(const wxLongLongWx& ll) const
{
    wxLongLongWx quotient, remainder;

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

#if wxUSE_STD_IOSTREAM

// input/output
ostream& operator<< (ostream& o, const wxLongLongWx& ll)
{
    char result[65];

    memset(result, 'A', 64);

    result[64] = '\0';

    for (int i = 0; i < 32; i++)
    {
        result[31 - i] = (char) ('0' + (int) ((ll.m_hi >> i) & 1));
        result[63 - i] = (char) ('0' + (int) ((ll.m_lo >> i) & 1));
    }

    return o << result;
}
#endif // wxUSE_STD_IOSTREAM

#endif // wxUSE_LONGLONG_NATIVE

#endif // wxUSE_LONGLONG
