/////////////////////////////////////////////////////////////////////////////
// Name:        wx/numformatter.h
// Purpose:     interface to wxNumberFormatter
// Author:      Fulvio Senore, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxNumberFormatter

    Formatting and parsing numbers using the current UI locale conventions,
    including support for using the correct decimal point character and
    thousands separators.

    This class contains only static functions, so users must not create instances
    but directly call the member functions.

    @see wxUILocale

    @since 2.9.2

    @library{wxbase}
*/
class wxNumberFormatter
{
public:
    /**
        Bit masks used with ToString().
     */
    enum Style
    {
        /**
            This flag can be used to indicate absence of any other flags below.
         */
        Style_None              = 0x00,

        /**
            If this flag is given, thousands separators will be inserted in the
            number string representation as defined by the current UI locale.
         */
        Style_WithThousandsSep  = 0x01,

        /**
            If this flag is given, trailing zeroes in a floating point number
            string representation will be omitted.

            If the number is actually integer, the decimal separator will be
            omitted as well. To give an example, formatting the number @c 1.23
            with precision 5 will normally yield "1.23000" but with this flag
            it would return "1.23". And formatting @c 123 with this flag will
            return just "123" for any precision.

            This flag can't be used with ToString() overload taking the integer
            value.
         */
        Style_NoTrailingZeroes  = 0x02
    };

    /**
        Returns string representation of an integer number.

        By default, the string will use thousands separators if appropriate for
        the current UI locale. This can be avoided by passing Style_None as @a
        flags in which case the call to the function has exactly the same
        effect as <code>wxString::Format("%ld", val)</code>.

        Notice that calling ToString() with a value of type @c int and
        non-default flags results in ambiguity between this overload and the
        one below. To resolve it, you need to cast the value to @c long.

        @param val
            The variable to convert to a string.
        @param flags
            Combination of values from the Style enumeration (except for
            Style_NoTrailingZeroes which can't be used with this overload).
    */
    ///@{
    static wxString ToString(long val, int flags = Style_WithThousandsSep);
    static wxString ToString(long long val, int flags = Style_WithThousandsSep);
    static wxString ToString(unsigned long long val, int flags = Style_WithThousandsSep);
    ///@}

    /**
        Returns string representation of a floating point number.

        @param val
            The variable to convert to a string.
        @param precision
            Number of decimals to write in formatted string.
        @param flags
            Combination of values from the Style enumeration.
    */
    static wxString
    ToString(double val, int precision, int flags = Style_WithThousandsSep);

    /**
        Format the given number using one of the floating point formats and
        ensure that the result uses the correct decimal separator.

        Prefer using ToString() if possible, i.e. if format is "%g" or "%.Nf"
        which are supported by it directly.

        @since 3.1.6
     */
    static wxString Format(const wxString& format, double val);

    /**
        Parse a string representation of a number possibly including thousands
        separators.

        These functions parse number representation in the current UI locale. On
        success they return @true and store the result at the location pointed
        to by @a val (which can't be @NULL), otherwise @false is returned.

        Note that the overload taking unsigned long long value is only
        available since wxWidgets 3.1.5. Also, unlike wxString::ToULongLong()
        and the standard functions such as @c strtoul(), this overload does
        @em not accept, i.e. returns @false, for the strings starting with the
        minus sign.

        @see wxString::ToLong(), wxString::ToDouble()
     */
    ///@{
    static bool FromString(wxString s, long *val);
    static bool FromString(wxString s, long long *val);
    static bool FromString(wxString s, unsigned long long *val);
    static bool FromString(wxString s, double *val);
    ///@}

    /**
        Get the decimal separator for the current UI locale.

        Decimal separators is always defined and we fall back to returning '.'
        in case of an error.
     */
    static wxChar GetDecimalSeparator();

    /**
        Get the thousands separator if grouping of the digits is used by the
        current UI locale.

        The value returned in @a sep should be only used if the function
        returns @true, otherwise no thousands separator should be used at all.

        @param sep
            Points to the variable receiving the thousands separator character
            if it is used by the current UI locale. May be @NULL if only the
            function return value is needed.
     */
    static bool GetThousandsSeparatorIfUsed(wxChar *sep);

};
