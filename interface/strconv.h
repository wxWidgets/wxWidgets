/////////////////////////////////////////////////////////////////////////////
// Name:        strconv.h
// Purpose:     interface of wxMBConvUTF7
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMBConvUTF7
    @wxheader{strconv.h}

    This class converts between the UTF-7 encoding and Unicode.
    It has one predefined instance, @b wxConvUTF7.

    @b WARNING: this class is not implemented yet.

    @library{wxbase}
    @category{FIXME}

    @see wxMBConvUTF8, @ref overview_mbconv "wxMBConv classes overview"
*/
class wxMBConvUTF7 : public wxMBConv
{
public:
    /**
        Converts from UTF-7 encoding to Unicode. Returns the size of the destination
        buffer.
    */
    size_t MB2WC(wchar_t* buf, const char* psz, size_t n) const;

    /**
        Converts from Unicode to UTF-7 encoding. Returns the size of the destination
        buffer.
    */
    size_t WC2MB(char* buf, const wchar_t* psz, size_t n) const;
};



/**
    @class wxMBConvUTF8
    @wxheader{strconv.h}

    This class converts between the UTF-8 encoding and Unicode.
    It has one predefined instance, @b wxConvUTF8.

    @library{wxbase}
    @category{FIXME}

    @see wxMBConvUTF7, @ref overview_mbconv "wxMBConv classes overview"
*/
class wxMBConvUTF8 : public wxMBConv
{
public:
    /**
        Converts from UTF-8 encoding to Unicode. Returns the size of the destination
        buffer.
    */
    size_t MB2WC(wchar_t* buf, const char* psz, size_t n) const;

    /**
        Converts from Unicode to UTF-8 encoding. Returns the size of the destination
        buffer.
    */
    size_t WC2MB(char* buf, const wchar_t* psz, size_t n) const;
};



/**
    @class wxMBConvUTF16
    @wxheader{strconv.h}

    This class is used to convert between multibyte encodings and UTF-16 Unicode
    encoding (also known as UCS-2). Unlike UTF-8() encoding,
    UTF-16 uses words and not bytes and hence depends on the byte ordering:
    big or little endian. Hence this class is provided in two versions:
    wxMBConvUTF16LE and wxMBConvUTF16BE and wxMBConvUTF16 itself is just a typedef
    for one of them (native for the given platform, e.g. LE under Windows and BE
    under Mac).

    @library{wxbase}
    @category{FIXME}

    @see wxMBConvUTF8, wxMBConvUTF32, @ref overview_mbconv "wxMBConv classes overview"
*/
class wxMBConvUTF16 : public wxMBConv
{
public:
    /**
        Converts from UTF-16 encoding to Unicode. Returns the size of the destination
        buffer.
    */
    size_t MB2WC(wchar_t* buf, const char* psz, size_t n) const;

    /**
        Converts from Unicode to UTF-16 encoding. Returns the size of the destination
        buffer.
    */
    size_t WC2MB(char* buf, const wchar_t* psz, size_t n) const;
};



/**
    @class wxCSConv
    @wxheader{strconv.h}

    This class converts between any character sets and Unicode.
    It has one predefined instance, @b wxConvLocal, for the
    default user character set.

    @library{wxbase}
    @category{FIXME}

    @see wxMBConv, wxEncodingConverter, @ref overview_mbconv "wxMBConv classes overview"
*/
class wxCSConv : public wxMBConv
{
public:
    /**
        Constructor. You can specify the name of the character set you want to
        convert from/to. If the character set name is not recognized, ISO 8859-1
        is used as fall back.
    */
    wxCSConv(const wxChar* charset);
    
    /**
        Constructor. You can specify an encoding constant for the 
        character set you want to convert from/to or. If the encoding
        is not recognized, ISO 8859-1 is used as fall back.
    */
    wxCSConv(wxFontEncoding encoding);

    /**
        Destructor frees any resources needed to perform the conversion.
    */
    ~wxCSConv();

    /**
        Returns @true if the charset (or the encoding) given at constructor is really
        available to use. Returns @false if ISO 8859-1 will be used instead.
        Note this does not mean that a given string will be correctly converted.
        A malformed string may still make conversion functions return @c wxCONV_FAILED.

        @since 2.8.2
    */
    bool IsOk() const;

    /**
        Converts from the selected character set to Unicode. Returns length of string
        written to destination buffer.
    */
    size_t MB2WC(wchar_t* buf, const char* psz, size_t n) const;

    /**
        Converts from Unicode to the selected character set. Returns length of string
        written to destination buffer.
    */
    size_t WC2MB(char* buf, const wchar_t* psz, size_t n) const;
};



/**
    @class wxMBConvFile
    @wxheader{strconv.h}

    This class used to define the class instance
    @b wxConvFileName, but nowadays @b wxConvFileName is
    either of type wxConvLibc (on most platforms) or wxConvUTF8
    (on MacOS X). @b wxConvFileName converts filenames between
    filesystem multibyte encoding and Unicode. @b wxConvFileName
    can also be set to a something else at run-time which is used
    e.g. by wxGTK to use a class which checks the environment
    variable @b G_FILESYSTEM_ENCODING indicating that filenames
    should not be interpreted as UTF8 and also for converting
    invalid UTF8 characters (e.g. if there is a filename in iso8859_1)
    to strings with octal values.

    Since some platforms (such as Win32) use Unicode in the filenames,
    and others (such as Unix) use multibyte encodings, this class should only
    be used directly if wxMBFILES is defined to 1. A convenience macro,
    wxFNCONV, is defined to wxConvFileName-cWX2MB in this case. You could
    use it like this:

    @code
    wxChar *name = wxT("rawfile.doc");
    FILE *fil = fopen(wxFNCONV(name), "r");
    @endcode

    (although it would be better to use wxFopen(name, wxT("r")) in this case.)

    @library{wxbase}
    @category{FIXME}

    @see @ref overview_mbconv "wxMBConv classes overview"
*/
class wxMBConvFile : public wxMBConv
{
public:
    /**
        Converts from multibyte filename encoding to Unicode. Returns the size of the
        destination buffer.
    */
    size_t MB2WC(wchar_t* buf, const char* psz, size_t n) const;

    /**
        Converts from Unicode to multibyte filename encoding. Returns the size of the
        destination buffer.
    */
    size_t WC2MB(char* buf, const wchar_t* psz, size_t n) const;
};



/**
    @class wxMBConvUTF32
    @wxheader{strconv.h}

    This class is used to convert between multibyte encodings and UTF-32 Unicode
    encoding (also known as UCS-4). Unlike UTF-8() encoding,
    UTF-32 uses (double) words and not bytes and hence depends on the byte ordering:
    big or little endian. Hence this class is provided in two versions:
    wxMBConvUTF32LE and wxMBConvUTF32BE and wxMBConvUTF32 itself is just a typedef
    for one of them (native for the given platform, e.g. LE under Windows and BE
    under Mac).

    @library{wxbase}
    @category{FIXME}

    @see wxMBConvUTF8, wxMBConvUTF16, @ref overview_mbconv "wxMBConv classes overview"
*/
class wxMBConvUTF32 : public wxMBConv
{
public:
    /**
        Converts from UTF-32 encoding to Unicode. Returns the size of the destination
        buffer.
    */
    size_t MB2WC(wchar_t* buf, const char* psz, size_t n) const;

    /**
        Converts from Unicode to UTF-32 encoding. Returns the size of the destination
        buffer.
    */
    size_t WC2MB(char* buf, const wchar_t* psz, size_t n) const;
};



/**
    @class wxMBConv
    @wxheader{strconv.h}

    This class is the base class of a hierarchy of classes capable of converting
    text strings between multibyte (SBCS or DBCS) encodings and Unicode.

    In the documentation for this and related classes please notice that
    length of the string refers to the number of characters in the string
    not counting the terminating @c NUL, if any. While the size of the string
    is the total number of bytes in the string, including any trailing @c NUL.
    Thus, length of wide character string @c L"foo" is 3 while its size can
    be either 8 or 16 depending on whether @c wchar_t is 2 bytes (as
    under Windows) or 4 (Unix).

    @library{wxbase}
    @category{FIXME}

    @see wxCSConv, wxEncodingConverter, @ref overview_mbconv "wxMBConv classes overview"
*/
class wxMBConv
{
public:
    /**
        Trivial default constructor.
    */
    wxMBConv();

    /**
        This pure virtual function is overridden in each of the derived classes to
        return a new copy of the object it is called on. It is used for copying the
        conversion objects while preserving their dynamic type.
    */
    virtual wxMBConv* Clone() const;

    /**
        This function has the same semantics as ToWChar()
        except that it converts a wide string to multibyte one.
    */
    virtual size_t FromWChar(char* dst, size_t dstLen,
                             const wchar_t* src,
                             size_t srcLen = wxNO_LEN) const;

    /**
        This function returns 1 for most of the multibyte encodings in which the
        string is terminated by a single @c NUL, 2 for UTF-16 and 4 for UTF-32 for
        which the string is terminated with 2 and 4 @c NUL characters respectively.
        The other cases are not currently supported and @c wxCONV_FAILED
        (defined as -1) is returned for them.
    */
    size_t GetMBNulLen() const;

    /**
        Returns the maximal value which can be returned by
        GetMBNulLen() for any conversion object. Currently
        this value is 4.
        This method can be used to allocate the buffer with enough space for the
        trailing @c NUL characters for any encoding.
    */
    const size_t GetMaxMBNulLen();

    /**
        This function is deprecated, please use ToWChar() instead
        Converts from a string @a in in multibyte encoding to Unicode putting up to
        @a outLen characters into the buffer @e out.
        If @a out is @NULL, only the length of the string which would result from
        the conversion is calculated and returned. Note that this is the length and not
        size, i.e. the returned value does not include the trailing @c NUL. But
        when the function is called with a non-@NULL @a out buffer, the @a outLen
        parameter should be one more to allow to properly @c NUL-terminate the string.

        @param out
            The output buffer, may be @NULL if the caller is only
            interested in the length of the resulting string
        @param in
            The NUL-terminated input string, cannot be @NULL
        @param outLen
            The length of the output buffer but including
            NUL, ignored if out is @NULL

        @returns The length of the converted string excluding the trailing NUL.
    */
    virtual size_t MB2WC(wchar_t* out, const char* in,
                         size_t outLen) const;

    /**
        The most general function for converting a multibyte string to a wide string.
        The main case is when @a dst is not @NULL and @a srcLen is not
        @c wxNO_LEN (which is defined as @c (size_t)-1): then
        the function converts exactly @a srcLen bytes starting at @a src into
        wide string which it output to @e dst. If the length of the resulting wide
        string is greater than @e dstLen, an error is returned. Note that if
        @a srcLen bytes don't include @c NUL characters, the resulting wide string is
        not @c NUL-terminated neither.
        If @a srcLen is @c wxNO_LEN, the function supposes that the string is
        properly (i.e. as necessary for the encoding handled by this conversion)
        @c NUL-terminated and converts the entire string, including any trailing @c NUL
        bytes. In this case the wide string is also @c NUL-terminated.
        Finally, if @a dst is @NULL, the function returns the length of the needed
        buffer.
    */
    virtual size_t ToWChar(wchar_t* dst, size_t dstLen,
                           const char* src,
                           size_t srcLen = wxNO_LEN) const;

    /**
        This function is deprecated, please use FromWChar() instead
        Converts from Unicode to multibyte encoding. The semantics of this function
        (including the return value meaning) is the same as for
        wxMBConv::MB2WC.
        Notice that when the function is called with a non-@NULL buffer, the
        @a n parameter should be the size of the buffer and so it should take
        into account the trailing @c NUL, which might take two or four bytes for some
        encodings (UTF-16 and UTF-32) and not one.
    */
    virtual size_t WC2MB(char* buf, const wchar_t* psz, size_t n) const;

    //@{
    /**
        Converts from multibyte encoding to Unicode by calling
        wxMBConv::MB2WC, allocating a temporary wxWCharBuffer to hold
        the result.
        The first overload takes a @c NUL-terminated input string. The second one takes
        a
        string of exactly the specified length and the string may include or not the
        trailing @c NUL character(s). If the string is not @c NUL-terminated, a
        temporary
        @c NUL-terminated copy of it suitable for passing to wxMBConv::MB2WC
        is made, so it is more efficient to ensure that the string is does have the
        appropriate number of @c NUL bytes (which is usually 1 but may be 2 or 4
        for UTF-16 or UTF-32, see wxMBConv::GetMBNulLen),
        especially for long strings.
        If @a outLen is not-@NULL, it receives the length of the converted
        string.
    */
    const wxWCharBuffer cMB2WC(const char* in) const;
    const wxWCharBuffer cMB2WC(const char* in,
                                     size_t inLen,
                                     size_t outLen) const;
    //@}

    //@{
    /**
        Converts from multibyte encoding to the current wxChar type
        (which depends on whether wxUSE_UNICODE is set to 1). If wxChar is char,
        it returns the parameter unaltered. If wxChar is wchar_t, it returns the
        result in a wxWCharBuffer. The macro wxMB2WXbuf is defined as the correct
        return type (without const).
    */
    const char* cMB2WX(const char* psz) const;
    const wxWCharBuffer cMB2WX(const char* psz) const;
    //@}

    //@{
    /**
        Converts from Unicode to multibyte encoding by calling WC2MB,
        allocating a temporary wxCharBuffer to hold the result.
        The second overload of this function allows to convert a string of the given
        length @e inLen, whether it is @c NUL-terminated or not (for wide character
        strings, unlike for the multibyte ones, a single @c NUL is always enough).
        But notice that just as with @ref wxMBConv::mb2wc cMB2WC, it is more
        efficient to pass an already terminated string to this function as otherwise a
        copy is made internally.
        If @a outLen is not-@NULL, it receives the length of the converted
        string.
    */
    const wxCharBuffer cWC2MB(const wchar_t* in) const;
    const wxCharBuffer cWC2MB(const wchar_t* in,
                                    size_t inLen,
                                    size_t outLen) const;
    //@}

    //@{
    /**
        Converts from Unicode to the current wxChar type. If wxChar is wchar_t,
        it returns the parameter unaltered. If wxChar is char, it returns the
        result in a wxCharBuffer. The macro wxWC2WXbuf is defined as the correct
        return type (without const).
    */
    const wchar_t* cWC2WX(const wchar_t* psz) const;
    const wxCharBuffer cWC2WX(const wchar_t* psz) const;
    //@}

    //@{
    /**
        Converts from the current wxChar type to multibyte encoding. If wxChar is char,
        it returns the parameter unaltered. If wxChar is wchar_t, it returns the
        result in a wxCharBuffer. The macro wxWX2MBbuf is defined as the correct
        return type (without const).
    */
    const char* cWX2MB(const wxChar* psz) const;
    const wxCharBuffer cWX2MB(const wxChar* psz) const;
    //@}

    //@{
    /**
        Converts from the current wxChar type to Unicode. If wxChar is wchar_t,
        it returns the parameter unaltered. If wxChar is char, it returns the
        result in a wxWCharBuffer. The macro wxWX2WCbuf is defined as the correct
        return type (without const).
    */
    const wchar_t* cWX2WC(const wxChar* psz) const;
    const wxWCharBuffer cWX2WC(const wxChar* psz) const;
    //@}
};

