/////////////////////////////////////////////////////////////////////////////
// Name:        encconv.h
// Purpose:     interface of wxEncodingConverter
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxEncodingConverter
    @wxheader{encconv.h}

    This class is capable of converting strings between two
    8-bit encodings/charsets. It can also convert from/to Unicode (but only
    if you compiled wxWidgets with wxUSE_WCHAR_T set to 1). Only a limited subset
    of encodings is supported by wxEncodingConverter:
    @c wxFONTENCODING_ISO8859_1..15, @c wxFONTENCODING_CP1250..1257 and
    @c wxFONTENCODING_KOI8.

    @library{wxbase}
    @category{misc}

    @see wxFontMapper, wxMBConv, @ref overview_nonenglishoverview "Writing
    non-English applications"
*/
class wxEncodingConverter : public wxObject
{
public:
    /**
        Constructor.
    */
    wxEncodingConverter();

    /**
        Return @true if (any text in) multibyte encoding @a encIn can be converted to
        another one (@e encOut) losslessly.
        Do not call this method with @c wxFONTENCODING_UNICODE as either
        parameter, it doesn't make sense (always works in one sense and always depends
        on the text to convert in the other).
    */
    static bool CanConvert(wxFontEncoding encIn,
                           wxFontEncoding encOut);

    //@{
    /**
        Convert wxString and return new wxString object.
    */
    bool Convert(const char* input, char* output) const;
    const bool Convert(const wchar_t* input, wchar_t* output) const;
    const bool Convert(const char* input, wchar_t* output) const;
    const bool Convert(const wchar_t* input, char* output) const;
    const bool Convert(char* str) const;
    const bool Convert(wchar_t* str) const;
    const wxString  Convert(const wxString& input) const;
    //@}

    /**
        Similar to
        GetPlatformEquivalents(),
        but this one will return ALL
        equivalent encodings, regardless of the platform, and including itself.
        This platform's encodings are before others in the array. And again, if @a enc
        is in the array,
        it is the very first item in it.
    */
    static wxFontEncodingArray GetAllEquivalents(wxFontEncoding enc);

    /**
        Return equivalents for given font that are used
        under given platform. Supported platforms:
         wxPLATFORM_UNIX
         wxPLATFORM_WINDOWS
         wxPLATFORM_OS2
         wxPLATFORM_MAC
         wxPLATFORM_CURRENT
        wxPLATFORM_CURRENT means the platform this binary was compiled for.
        Examples:

        Equivalence is defined in terms of convertibility:
        two encodings are equivalent if you can convert text between
        then without losing information (it may - and will - happen
        that you lose special chars like quotation marks or em-dashes
        but you shouldn't lose any diacritics and language-specific
        characters when converting between equivalent encodings).
        Remember that this function does @b NOT check for presence of
        fonts in system. It only tells you what are most suitable
        encodings. (It usually returns only one encoding.)
    */
    static wxFontEncodingArray GetPlatformEquivalents(wxFontEncoding enc,
            int platform = wxPLATFORM_CURRENT);

    /**
        Initialize conversion. Both output or input encoding may
        be wxFONTENCODING_UNICODE, but only if wxUSE_ENCODING is set to 1.
        All subsequent calls to Convert()
        will interpret its argument
        as a string in @a input_enc encoding and will output string in
        @a output_enc encoding.
        You must call this method before calling Convert. You may call
        it more than once in order to switch to another conversion.
        @e Method affects behaviour of Convert() in case input character
        cannot be converted because it does not exist in output encoding:

        @b wxCONVERT_STRICT

        follow behaviour of GNU Recode -
        just copy unconvertible  characters to output and don't change them
        (its integer value will stay the same)

        @b wxCONVERT_SUBSTITUTE

        try some (lossy) substitutions
        - e.g. replace unconvertible latin capitals with acute by ordinary
        capitals, replace en-dash or em-dash by '-' etc.

        Both modes guarantee that output string will have same length
        as input string.
    */
    bool Init(wxFontEncoding input_enc, wxFontEncoding output_enc,
              int method = wxCONVERT_STRICT);
};

