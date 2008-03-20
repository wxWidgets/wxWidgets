/////////////////////////////////////////////////////////////////////////////
// Name:        longlong.h
// Purpose:     interface of wxLongLong
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxLongLong
    @wxheader{longlong.h}

    This class represents a signed 64 bit long number. It is implemented using the
    native 64 bit type where available (machines with 64 bit longs or compilers
    which have (an analog of) @e long long type) and uses the emulation code in
    the other cases which ensures that it is the most efficient solution for
    working with 64 bit integers independently of the architecture.

    wxLongLong defines all usual arithmetic operations such as addition,
    subtraction, bitwise shifts and logical operations as well as multiplication
    and division (not yet for the machines without native @e long long). It
    also has operators for implicit construction from and conversion to the native
    @e long long type if it exists and @e long.

    You would usually use this type in exactly the same manner as any other
    (built-in) arithmetic type. Note that wxLongLong is a signed type, if you
    want unsigned values use wxULongLong which has exactly the same API as
    wxLongLong except when explicitly mentioned otherwise.

    If a native (i.e. supported directly by the compiler) 64 bit integer type was
    found to exist, @e wxLongLong_t macro will be defined to correspond to it.
    Also, in this case only, two additional macros will be defined:
    wxLongLongFmtSpec() for printing 64 bit integers
    using the standard @c printf() function (but see also
    wxLongLong::ToString for a more portable solution) and
    wxLL() for defining 64 bit integer compile-time constants.

    @library{wxbase}
    @category{data}
*/
class wxLongLong
{
public:
    /**
        Constructor from 2 longs: the high and low part are combined into one
        wxLongLong.
    */
    wxLongLong(long hi, unsigned long lo);

    //@{
    /**
        Returns an absolute value of wxLongLong - either making a copy (const version)
        or modifying it in place (the second one).  Not in wxULongLong.
    */
    wxLongLong Abs();
    const wxLongLong&  Abs();
    //@}

    /**
        This allows to convert a double value to wxLongLong type. Such conversion is
        not always possible in which case the result will be silently truncated in a
        platform-dependent way.  Not in wxULongLong.
    */
    wxLongLong Assign(double d);

    /**
        Returns the high 32 bits of 64 bit integer.
    */
    long GetHi() const;

    /**
        Returns the low 32 bits of 64 bit integer.
    */
    unsigned long GetLo() const;

    /**
        Convert to native long long (only for compilers supporting it)
    */
    wxLongLong_t GetValue() const;

    /**
        Returns the value as @c double.
    */
    double ToDouble() const;

    /**
        Truncate wxLongLong to long. If the conversion loses data (i.e. the wxLongLong
        value is outside the range of built-in long type), an assert will be triggered
        in debug mode.
    */
    long ToLong() const;

    /**
        Returns the string representation of a wxLongLong.
    */
    wxString ToString() const;

    /**
        Adds 2 wxLongLongs together and returns the result.
    */
    wxLongLong operator+(const wxLongLong& ll) const;

    //@{
    /**
        Pre/post increment operator.
    */
    wxLongLong operator++();
    wxLongLong operator++(int );
    //@}

    /**
        Add another wxLongLong to this one.
    */
    wxLongLong operator+(const wxLongLong& ll);

    /**
        Subtracts 2 wxLongLongs and returns the result.
    */
    wxLongLong operator-(const wxLongLong& ll) const;

    //@{
    /**
        Pre/post decrement operator.
    */
    wxLongLong operator--();
    wxLongLong operator--(int );
    //@}

    /**
        Subtracts another wxLongLong from this one.
    */
    wxLongLong operator-(const wxLongLong& ll);

    /**
        Assignment operator from unsigned long long. The sign bit will be copied too.

        @wxsince{2.7.0}
    */
    wxLongLong& operator operator=(const wxULongLong& ll);
};



// ============================================================================
// Global functions/macros
// ============================================================================

/**
    This macro is defined to contain the @c printf() format specifier using
    which 64 bit integer numbers (i.e. those of type @c wxLongLong_t) can be
    printed. Example of using it:

    @code
    #ifdef wxLongLong_t
            wxLongLong_t ll = wxLL(0x1234567890abcdef);
            printf("Long long = %" wxLongLongFmtSpec "x\n", ll);
        #endif
    @endcode

    @see wxLL()
*/


/**
    This macro is defined for the platforms with a native 64 bit integer type and
    allows to define unsigned 64 bit compile time constants:

    @code
    #ifdef wxLongLong_t
            unsigned wxLongLong_t ll = wxULL(0x1234567890abcdef);
        #endif
    @endcode

    @see wxLL(), wxLongLong
*/
wxLongLong_t wxULL(number);

/**
    This macro is defined for the platforms with a native 64 bit integer type and
    allows to define 64 bit compile time constants:

    @code
    #ifdef wxLongLong_t
            wxLongLong_t ll = wxLL(0x1234567890abcdef);
        #endif
    @endcode

    @see wxULL(), wxLongLong
*/
wxLongLong_t wxLL(number);

