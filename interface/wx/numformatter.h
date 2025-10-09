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
            Thousands separators will be applied according to the grouping rules
            of the current UI locale.
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
        Style_NoTrailingZeroes  = 0x02,

        /**
            If this flag is given, a + will prefix the
            number string representation of non-negative numbers.

            This style cannot be used with Style_SignSpace.

            @since 3.3.1
         */
        Style_SignPlus          = 0x04,

        /**
            If this flag is given, a space will prefix the
            number string representation of non-negative numbers.

            This style cannot be used with Style_SignPlus.

            @since 3.3.1
         */
        Style_SignSpace         = 0x08,

        /**
            If this flag is given, the number will be interpreted
            as a currency value and formatted according to the rules
            of the current UI locale.

            @since 3.3.2
         */
        Style_Currency          = 0x10,

        /**
            If this flag is given, the currency symbol of the current
            UI locale will be prepended or appended to the currency value
            according to the rules of the current locale.

            The style will only take effect, if it is combined with Style_Currency.
            The style should not be combined with Style_CurrencyCode.

            @since 3.3.2
         */
        Style_CurrencySymbol    = 0x20,     // Currency with currency symbol

        /**
            If this flag is given, the ISO 42127 currency code of the current
            UI locale will be prepended to the currency value. It will be separated
            from the currency value by a space character.

            The style will only take effect, if it is combined with Style_Currency.
            The style should not be combined with Style_CurrencySymbol.

            @since 3.3.2
         */
        Style_CurrencyCode      = 0x40,     // Currency with ISO 4217 code

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

    /**
        Remove trailing zeroes and, if there is nothing left after it, the
        decimal separator itself from a string representing a floating point
        number.

        @param[in,out] str The string to remove zeroes from.

        @since 3.3.1
     */
    static void RemoveTrailingZeroes(wxString& str);

    /**
        Remove currency symbol or code, and grouping separators,
        according to the given style flags.

        @note This function allows to remove the currency symbol or code
            from a string with a formatted currency value, so that
            FromString() can be used afterwards to retrieve the numerical value.
            Additionally, grouping separators are removed, if necessary.

        @param[in] str
            The string to remove the currency symbol or code or grouping separators from.
        @param flags
            Combination of values from the Style enumeration.

        @since 3.3.2
     */
    static wxString RemoveCurrencySymbolOrCode(wxString str, int flags);

};
