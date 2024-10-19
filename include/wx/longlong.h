/////////////////////////////////////////////////////////////////////////////
// Name:        wx/longlong.h
// Purpose:     Legacy support for 64-bit integers predating universal long
//              long availability
// Author:      Jeffrey C. Ollie <jeff@ollie.clive.ia.us>, Vadim Zeitlin
// Created:     10.02.99
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LONGLONG_H
#define _WX_LONGLONG_H

#include "wx/defs.h"
#include "wx/iosfwrap.h"
#include "wx/string.h"

#include <limits.h>     // for LONG_MAX

class WXDLLIMPEXP_FWD_BASE wxULongLong;

class WXDLLIMPEXP_BASE wxWARN_UNUSED wxLongLong
{
public:
    // ctors
        // default ctor initializes to 0
    wxLongLong() : m_ll(0) { }
        // from long long
    wxLongLong(wxLongLong_t ll) : m_ll(ll) { }
        // from 2 longs
    wxLongLong(wxInt32 hi, wxUint32 lo)
    {
        // cast first to avoid precision loss and use unsigned shift to avoid
        // undefined behaviour if the high part is negative (and then cast
        // again to avoid warning about the possible sign change)
        m_ll = (wxLongLong_t)(((wxULongLong_t) hi) << 32);
        m_ll |= (wxLongLong_t) lo;
    }

    // default copy ctor is ok

    // no dtor

    // assignment operators
        // from native 64 bit integer
#ifndef wxLongLongIsLong
    wxLongLong& operator=(wxLongLong_t ll)
        { m_ll = ll; return *this; }
    wxLongLong& operator=(wxULongLong_t ll)
        { m_ll = ll; return *this; }
#endif // !wxLongLong
    wxLongLong& operator=(const wxULongLong &ll);
    wxLongLong& operator=(int l)
        { m_ll = l; return *this; }
    wxLongLong& operator=(long l)
        { m_ll = l; return *this; }
    wxLongLong& operator=(unsigned int l)
        { m_ll = l; return *this; }
    wxLongLong& operator=(unsigned long l)
        { m_ll = l; return *this; }


        // from double: this one has an explicit name because otherwise we
        // would have ambiguity with "ll = int" and also because we don't want
        // to have implicit conversions between doubles and wxLongLongs
    wxLongLong& Assign(double d)
        { m_ll = (wxLongLong_t)d; return *this; }

    // assignment operators from wxLongLong is ok

    // accessors
        // get high part
    wxInt32 GetHi() const
        { return wx_truncate_cast(wxInt32, m_ll >> 32); }
        // get low part
    wxUint32 GetLo() const
        { return wx_truncate_cast(wxUint32, m_ll); }

        // get absolute value
    wxLongLong Abs() const { return wxLongLong(*this).Abs(); }
    wxLongLong& Abs() { if ( m_ll < 0 ) m_ll = -m_ll; return *this; }

        // convert to native long long
    wxLongLong_t GetValue() const { return m_ll; }

        // convert to long with range checking in debug mode (only!)
    long ToLong() const
    {
        // This assert is useless if long long is the same as long (which is
        // the case under the standard Unix LP64 model).
#ifdef wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG
        wxASSERT_MSG( (m_ll >= LONG_MIN) && (m_ll <= LONG_MAX),
                      wxT("wxLongLong to long conversion loss of precision") );
#endif

        return wx_truncate_cast(long, m_ll);
    }

        // convert to double
    double ToDouble() const { return wx_truncate_cast(double, m_ll); }

    // don't provide implicit conversion to wxLongLong_t or we will have an
    // ambiguity for all arithmetic operations
    //operator wxLongLong_t() const { return m_ll; }

    // operations
        // addition
    wxLongLong operator+(const wxLongLong& ll) const
        { return wxLongLong(m_ll + ll.m_ll); }
    wxLongLong& operator+=(const wxLongLong& ll)
        { m_ll += ll.m_ll; return *this; }
    friend wxLongLong operator+(long l, const wxLongLong& ll)
        { return ll + l; }

    wxLongLong operator+(const wxLongLong_t ll) const
        { return wxLongLong(m_ll + ll); }
    wxLongLong& operator+=(const wxLongLong_t ll)
        { m_ll += ll; return *this; }

        // pre increment
    wxLongLong& operator++()
        { m_ll++; return *this; }

        // post increment
    wxLongLong operator++(int)
        { wxLongLong value(*this); m_ll++; return value; }

        // negation operator
    wxLongLong operator-() const
        { return wxLongLong(-m_ll); }
    wxLongLong& Negate() { m_ll = -m_ll; return *this; }

        // subtraction
    wxLongLong operator-(const wxLongLong& ll) const
        { return wxLongLong(m_ll - ll.m_ll); }
    wxLongLong& operator-=(const wxLongLong& ll)
        { m_ll -= ll.m_ll; return *this; }
    friend wxLongLong operator-(long l, const wxLongLong& ll)
    {
        return wxLongLong(l) - ll;
    }

    wxLongLong operator-(const wxLongLong_t ll) const
        { return wxLongLong(m_ll - ll); }
    wxLongLong& operator-=(const wxLongLong_t ll)
        { m_ll -= ll; return *this; }

        // pre decrement
    wxLongLong& operator--()
        { m_ll--; return *this; }

        // post decrement
    wxLongLong operator--(int)
        { wxLongLong value(*this); m_ll--; return value; }

    // shifts
        // left shift
    wxLongLong operator<<(int shift) const
        { return wxLongLong(m_ll << shift); }
    wxLongLong& operator<<=(int shift)
        { m_ll <<= shift; return *this; }

        // right shift
    wxLongLong operator>>(int shift) const
        { return wxLongLong(m_ll >> shift); }
    wxLongLong& operator>>=(int shift)
        { m_ll >>= shift; return *this; }

    // bitwise operators
    wxLongLong operator&(const wxLongLong& ll) const
        { return wxLongLong(m_ll & ll.m_ll); }
    wxLongLong& operator&=(const wxLongLong& ll)
        { m_ll &= ll.m_ll; return *this; }

    wxLongLong operator|(const wxLongLong& ll) const
        { return wxLongLong(m_ll | ll.m_ll); }
    wxLongLong& operator|=(const wxLongLong& ll)
        { m_ll |= ll.m_ll; return *this; }

    wxLongLong operator^(const wxLongLong& ll) const
        { return wxLongLong(m_ll ^ ll.m_ll); }
    wxLongLong& operator^=(const wxLongLong& ll)
        { m_ll ^= ll.m_ll; return *this; }

    // multiplication/division
    wxLongLong operator*(const wxLongLong& ll) const
        { return wxLongLong(m_ll * ll.m_ll); }
    wxLongLong operator*(long l) const
        { return wxLongLong(m_ll * l); }
    wxLongLong& operator*=(const wxLongLong& ll)
        { m_ll *= ll.m_ll; return *this; }
    wxLongLong& operator*=(long l)
        { m_ll *= l; return *this; }

    wxLongLong operator/(const wxLongLong& ll) const
        { return wxLongLong(m_ll / ll.m_ll); }
    wxLongLong operator/(long l) const
        { return wxLongLong(m_ll / l); }
    wxLongLong& operator/=(const wxLongLong& ll)
        { m_ll /= ll.m_ll; return *this; }
    wxLongLong& operator/=(long l)
        { m_ll /= l; return *this; }

    wxLongLong operator%(const wxLongLong& ll) const
        { return wxLongLong(m_ll % ll.m_ll); }
    wxLongLong operator%(long l) const
        { return wxLongLong(m_ll % l); }

    // comparison
    bool operator==(const wxLongLong& ll) const
        { return m_ll == ll.m_ll; }
    bool operator==(long l) const
        { return m_ll == l; }
    bool operator!=(const wxLongLong& ll) const
        { return m_ll != ll.m_ll; }
    bool operator!=(long l) const
        { return m_ll != l; }
    bool operator<(const wxLongLong& ll) const
        { return m_ll < ll.m_ll; }
    bool operator<(long l) const
        { return m_ll < l; }
    bool operator>(const wxLongLong& ll) const
        { return m_ll > ll.m_ll; }
    bool operator>(long l) const
        { return m_ll > l; }
    bool operator<=(const wxLongLong& ll) const
        { return m_ll <= ll.m_ll; }
    bool operator<=(long l) const
        { return m_ll <= l; }
    bool operator>=(const wxLongLong& ll) const
        { return m_ll >= ll.m_ll; }
    bool operator>=(long l) const
        { return m_ll >= l; }

    friend bool operator<(long l, const wxLongLong& ll) { return ll > l; }
    friend bool operator>(long l, const wxLongLong& ll) { return ll < l; }
    friend bool operator<=(long l, const wxLongLong& ll) { return ll >= l; }
    friend bool operator>=(long l, const wxLongLong& ll) { return ll <= l; }
    friend bool operator==(long l, const wxLongLong& ll) { return ll == l; }
    friend bool operator!=(long l, const wxLongLong& ll) { return ll != l; }

    // miscellaneous

        // return the string representation of this number
    wxString ToString() const
    {
        return wxString::Format(wxASCII_STR("%" wxLongLongFmtSpec "d"), m_ll);
    }

        // conversion to byte array: returns a pointer to static buffer!
    void *asArray() const;

#if wxUSE_STD_IOSTREAM
        // input/output
    friend WXDLLIMPEXP_BASE
    std::ostream& operator<<(std::ostream&, const wxLongLong&);
#endif

    friend WXDLLIMPEXP_BASE
    wxString& operator<<(wxString&, const wxLongLong&);

#if wxUSE_STREAMS
    friend WXDLLIMPEXP_BASE
    class wxTextOutputStream& operator<<(class wxTextOutputStream&, const wxLongLong&);
    friend WXDLLIMPEXP_BASE
    class wxTextInputStream& operator>>(class wxTextInputStream&, wxLongLong&);
#endif

private:
    wxLongLong_t  m_ll;
};

class WXDLLIMPEXP_BASE wxWARN_UNUSED wxULongLong
{
public:
    // ctors
        // default ctor initializes to 0
    wxULongLong() : m_ll(0) { }
        // from long long
    wxULongLong(wxULongLong_t ll) : m_ll(ll) { }
        // from 2 longs
    wxULongLong(wxUint32 hi, wxUint32 lo) : m_ll(0)
    {
        // cast to wxLongLong_t first to avoid precision loss!
        m_ll = ((wxULongLong_t) hi) << 32;
        m_ll |= (wxULongLong_t) lo;
    }

    // default copy ctor is ok

    // no dtor

    // assignment operators
        // from native 64 bit integer
#ifndef wxLongLongIsLong
    wxULongLong& operator=(wxULongLong_t ll)
        { m_ll = ll; return *this; }
    wxULongLong& operator=(wxLongLong_t ll)
        { m_ll = ll; return *this; }
#endif // !wxLongLong
    wxULongLong& operator=(int l)
        { m_ll = l; return *this; }
    wxULongLong& operator=(long l)
        { m_ll = l; return *this; }
    wxULongLong& operator=(unsigned int l)
        { m_ll = l; return *this; }
    wxULongLong& operator=(unsigned long l)
        { m_ll = l; return *this; }
    wxULongLong& operator=(const wxLongLong &ll)
        { m_ll = ll.GetValue(); return *this; }

    // assignment operators from wxULongLong is ok

    // accessors
        // get high part
    wxUint32 GetHi() const
        { return wx_truncate_cast(wxUint32, m_ll >> 32); }
        // get low part
    wxUint32 GetLo() const
        { return wx_truncate_cast(wxUint32, m_ll); }

        // convert to native ulong long
    wxULongLong_t GetValue() const { return m_ll; }

        // convert to ulong with range checking in debug mode (only!)
    unsigned long ToULong() const
    {
        wxASSERT_MSG( m_ll <= ULONG_MAX,
                      wxT("wxULongLong to long conversion loss of precision") );

        return wx_truncate_cast(unsigned long, m_ll);
    }

        // convert to double
    double ToDouble() const { return wx_truncate_cast(double, m_ll); }

    // operations
        // addition
    wxULongLong operator+(const wxULongLong& ll) const
        { return wxULongLong(m_ll + ll.m_ll); }
    wxULongLong& operator+=(const wxULongLong& ll)
        { m_ll += ll.m_ll; return *this; }
    friend wxULongLong operator+(unsigned long l, const wxULongLong& ull)
        { return ull + l; }

    wxULongLong operator+(const wxULongLong_t ll) const
        { return wxULongLong(m_ll + ll); }
    wxULongLong& operator+=(const wxULongLong_t ll)
        { m_ll += ll; return *this; }

        // pre increment
    wxULongLong& operator++()
        { m_ll++; return *this; }

        // post increment
    wxULongLong operator++(int)
        { wxULongLong value(*this); m_ll++; return value; }

        // subtraction
    wxULongLong operator-(const wxULongLong& ll) const
        { return wxULongLong(m_ll - ll.m_ll); }
    wxULongLong& operator-=(const wxULongLong& ll)
        { m_ll -= ll.m_ll; return *this; }
    friend wxULongLong operator-(unsigned long l, const wxULongLong& ull)
    {
        return wxULongLong(l - ull.m_ll);
    }

    wxULongLong operator-(const wxULongLong_t ll) const
        { return wxULongLong(m_ll - ll); }
    wxULongLong& operator-=(const wxULongLong_t ll)
        { m_ll -= ll; return *this; }

        // pre decrement
    wxULongLong& operator--()
        { m_ll--; return *this; }

        // post decrement
    wxULongLong operator--(int)
        { wxULongLong value(*this); m_ll--; return value; }

    // shifts
        // left shift
    wxULongLong operator<<(int shift) const
        { return wxULongLong(m_ll << shift); }
    wxULongLong& operator<<=(int shift)
        { m_ll <<= shift; return *this; }

        // right shift
    wxULongLong operator>>(int shift) const
        { return wxULongLong(m_ll >> shift); }
    wxULongLong& operator>>=(int shift)
        { m_ll >>= shift; return *this; }

    // bitwise operators
    wxULongLong operator&(const wxULongLong& ll) const
        { return wxULongLong(m_ll & ll.m_ll); }
    wxULongLong& operator&=(const wxULongLong& ll)
        { m_ll &= ll.m_ll; return *this; }

    wxULongLong operator|(const wxULongLong& ll) const
        { return wxULongLong(m_ll | ll.m_ll); }
    wxULongLong& operator|=(const wxULongLong& ll)
        { m_ll |= ll.m_ll; return *this; }

    wxULongLong operator^(const wxULongLong& ll) const
        { return wxULongLong(m_ll ^ ll.m_ll); }
    wxULongLong& operator^=(const wxULongLong& ll)
        { m_ll ^= ll.m_ll; return *this; }

    // multiplication/division
    wxULongLong operator*(const wxULongLong& ll) const
        { return wxULongLong(m_ll * ll.m_ll); }
    wxULongLong operator*(unsigned long l) const
        { return wxULongLong(m_ll * l); }
    wxULongLong& operator*=(const wxULongLong& ll)
        { m_ll *= ll.m_ll; return *this; }
    wxULongLong& operator*=(unsigned long l)
        { m_ll *= l; return *this; }

    wxULongLong operator/(const wxULongLong& ll) const
        { return wxULongLong(m_ll / ll.m_ll); }
    wxULongLong operator/(unsigned long l) const
        { return wxULongLong(m_ll / l); }
    wxULongLong& operator/=(const wxULongLong& ll)
        { m_ll /= ll.m_ll; return *this; }
    wxULongLong& operator/=(unsigned long l)
        { m_ll /= l; return *this; }

    wxULongLong operator%(const wxULongLong& ll) const
        { return wxULongLong(m_ll % ll.m_ll); }
    wxULongLong operator%(unsigned long l) const
        { return wxULongLong(m_ll % l); }

    // comparison
    bool operator==(const wxULongLong& ll) const
        { return m_ll == ll.m_ll; }
    bool operator==(unsigned long l) const
        { return m_ll == l; }
    bool operator!=(const wxULongLong& ll) const
        { return m_ll != ll.m_ll; }
    bool operator!=(unsigned long l) const
        { return m_ll != l; }
    bool operator<(const wxULongLong& ll) const
        { return m_ll < ll.m_ll; }
    bool operator<(unsigned long l) const
        { return m_ll < l; }
    bool operator>(const wxULongLong& ll) const
        { return m_ll > ll.m_ll; }
    bool operator>(unsigned long l) const
        { return m_ll > l; }
    bool operator<=(const wxULongLong& ll) const
        { return m_ll <= ll.m_ll; }
    bool operator<=(unsigned long l) const
        { return m_ll <= l; }
    bool operator>=(const wxULongLong& ll) const
        { return m_ll >= ll.m_ll; }
    bool operator>=(unsigned long l) const
        { return m_ll >= l; }

    friend bool operator<(unsigned long l, const wxULongLong& ull) { return ull > l; }
    friend bool operator>(unsigned long l, const wxULongLong& ull) { return ull < l; }
    friend bool operator<=(unsigned long l, const wxULongLong& ull) { return ull >= l; }
    friend bool operator>=(unsigned long l, const wxULongLong& ull) { return ull <= l; }
    friend bool operator==(unsigned long l, const wxULongLong& ull) { return ull == l; }
    friend bool operator!=(unsigned long l, const wxULongLong& ull) { return ull != l; }

    // miscellaneous

        // return the string representation of this number
    wxString ToString() const
    {
        return wxString::Format(wxASCII_STR("%" wxLongLongFmtSpec "u"), m_ll);
    }

        // conversion to byte array: returns a pointer to static buffer!
    void *asArray() const;

#if wxUSE_STD_IOSTREAM
        // input/output
    friend WXDLLIMPEXP_BASE
    std::ostream& operator<<(std::ostream&, const wxULongLong&);
#endif

    friend WXDLLIMPEXP_BASE
    wxString& operator<<(wxString&, const wxULongLong&);

#if wxUSE_STREAMS
    friend WXDLLIMPEXP_BASE
    class wxTextOutputStream& operator<<(class wxTextOutputStream&, const wxULongLong&);
    friend WXDLLIMPEXP_BASE
    class wxTextInputStream& operator>>(class wxTextInputStream&, wxULongLong&);
#endif

private:
    wxULongLong_t  m_ll;
};

inline
wxLongLong& wxLongLong::operator=(const wxULongLong &ll)
{
    m_ll = ll.GetValue();
    return *this;
}

// ----------------------------------------------------------------------------
// Specialize numeric_limits<> for our long long wrapper classes.
// ----------------------------------------------------------------------------

#include <limits>

namespace std
{

template<> struct numeric_limits<wxLongLong>  : numeric_limits<wxLongLong_t> {};
template<> struct numeric_limits<wxULongLong> : numeric_limits<wxULongLong_t> {};

} // namespace std

// ----------------------------------------------------------------------------
// Specialize wxArgNormalizer to allow using wxLongLong directly with wx pseudo
// vararg functions.
// ----------------------------------------------------------------------------

// Notice that this must be done here and not in wx/strvararg.h itself because
// we can't include wx/longlong.h from there as this header itself includes
// wx/string.h which includes wx/strvararg.h too, so to avoid the circular
// dependencies we can only do it here (or add another header just for this but
// it doesn't seem necessary).
#include "wx/strvararg.h"

template<>
struct wxFormatStringSpecifier<wxLongLong>
{
    enum { value = wxFormatString::Arg_LongLongInt };
};

template<>
struct WXDLLIMPEXP_BASE wxArgNormalizer<wxLongLong>
{
     wxArgNormalizer(wxLongLong value,
                     const wxFormatString *fmt, unsigned index)
         : m_value(value)
     {
         wxASSERT_ARG_TYPE( fmt, index, wxFormatString::Arg_LongLongInt );
     }

     wxLongLong_t get() const { return m_value.GetValue(); }

     wxLongLong m_value;
};

// Compatibility typedefs: these types were never supposed to be used outside
// of wx itself but nevertheless appear in the existing code, so define them to
// let it continue to build.
using wxLongLongNative = wxLongLong;
using wxULongLongNative = wxULongLong;

#endif // _WX_LONGLONG_H
