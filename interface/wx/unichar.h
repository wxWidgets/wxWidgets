/////////////////////////////////////////////////////////////////////////////
// Name:        unichar.h
// Purpose:     interface of wxUniChar
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxUniChar

    This class represents a single Unicode character. It can be converted to
    and from @c char or @c wchar_t and implements commonly used character operations.

    @library{wxbase}
    @category{data}
*/
class wxUniChar
{
public:
    /**
        This is not wchar_t on purpose, it needs to represent the entire
        Unicode code points range and wchar_t may be too small for that
        (e.g. on Win32 where wchar_t* is encoded in UTF-16).
    */
    typedef wxUint32 value_type;

    /**
        Default ctor.
    */
    wxUniChar();

    //@{
    /**
        Create the character from 8bit character value encoded in the current
        locale's charset.
    */
    wxUniChar(char c);
    wxUniChar(unsigned char c);
    //@}

    wxUniChar(int c);
    wxUniChar(unsigned int c);
    wxUniChar(long int c);
    wxUniChar(unsigned long int c);
    wxUniChar(short int c);
    wxUniChar(unsigned short int c);

    wxUniChar(const wxUniCharRef& c);

    /**
        Returns Unicode code point value of the character.
    */
    value_type GetValue() const;

    /**
        Returns true if the character is an ASCII character.
    */
    bool IsAscii() const;

    //@{
    /**
        Conversions to char and wchar_t types: all of those are needed to be
        able to pass wxUniChars to various standard narrow and wide character
        functions.
    */
    operator char() const;
    operator unsigned char() const;
    operator wchar_t() const;
    operator int() const;
    operator unsigned int() const;
    operator long int() const;
    operator unsigned long int() const;
    operator short int() const;
    operator unsigned short int() const;
    //@}

    //@{
    /**
        Assignment operators
    */
    wxUniChar& operator=(const wxUniChar& c);
    wxUniChar& operator=(const wxUniCharRef& c);
    wxUniChar& operator=(char c);
    wxUniChar& operator=(unsigned char c);
    wxUniChar& operator=(wchar_t c);
    wxUniChar& operator=(int c);
    wxUniChar& operator=(unsigned int c);
    wxUniChar& operator=(long int c);
    wxUniChar& operator=(unsigned long int c);
    wxUniChar& operator=(short int c);
    wxUniChar& operator=(unsigned short int c);
    //@}
};


/**
    @class wxUniCharRef

    Writeable reference to a character in wxString.

    This class can be used in the same way wxChar is used, except that changing
    its value updates the underlying string object.

    @library{wxbase}
    @category{data}
*/
class wxUniCharRef
{
public:
};

