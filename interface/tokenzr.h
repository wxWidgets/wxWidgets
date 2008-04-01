/////////////////////////////////////////////////////////////////////////////
// Name:        tokenzr.h
// Purpose:     interface of wxStringTokenizer
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxStringTokenizer
    @wxheader{tokenzr.h}

    wxStringTokenizer helps you to break a string up into a number of tokens. It
    replaces the standard C function @c strtok() and also extends it in a
    number of ways.

    To use this class, you should create a wxStringTokenizer object, give it the
    string to tokenize and also the delimiters which separate tokens in the string
    (by default, white space characters will be used).

    Then wxStringTokenizer::GetNextToken may be called
    repeatedly until it wxStringTokenizer::HasMoreTokens
    returns @false.

    For example:

    @code
    wxStringTokenizer tkz(wxT("first:second:third:fourth"), wxT(":"));
    while ( tkz.HasMoreTokens() )
    {
        wxString token = tkz.GetNextToken();

        // process token here
    }
    @endcode

    By default, wxStringTokenizer will behave in the same way as @c strtok() if
    the delimiters string only contains white space characters but, unlike the
    standard function, it will return empty tokens if this is not the case. This
    is helpful for parsing strictly formatted data where the number of fields is
    fixed but some of them may be empty (i.e. @c TAB or comma delimited text
    files).

    The behaviour is governed by the last
    @ref wxStringTokenizer::wxstringtokenizer
    constructor/wxStringTokenizer::SetString
    parameter @c mode which may be one of the following:



    @c wxTOKEN_DEFAULT


    Default behaviour (as described above):
    same as @c wxTOKEN_STRTOK if the delimiter string contains only
    whitespaces, same as @c wxTOKEN_RET_EMPTY otherwise


    @c wxTOKEN_RET_EMPTY


    In this mode, the empty tokens in the
    middle of the string will be returned, i.e. @c "a::b:" will be tokenized in
    three tokens 'a', " and 'b'. Notice that all trailing delimiters are ignored
    in this mode, not just the last one, i.e. a string @c "a::b::" would
    still result in the same set of tokens.


    @c wxTOKEN_RET_EMPTY_ALL


    In this mode, empty trailing tokens
    (including the one after the last delimiter character) will be returned as
    well. The string @c "a::b:" will be tokenized in four tokens: the already
    mentioned ones and another empty one as the last one and a string
    @c "a::b::" will have five tokens.


    @c wxTOKEN_RET_DELIMS


    In this mode, the delimiter character
    after the end of the current token (there may be none if this is the last
    token) is returned appended to the token. Otherwise, it is the same mode as
    @c wxTOKEN_RET_EMPTY. Notice that there is no mode like this one but
    behaving like @c wxTOKEN_RET_EMPTY_ALL instead of
    @c wxTOKEN_RET_EMPTY, use @c wxTOKEN_RET_EMPTY_ALL and
    wxStringTokenizer::GetLastDelimiter to emulate it.


    @c wxTOKEN_STRTOK


    In this mode the class behaves exactly like
    the standard @c strtok() function: the empty tokens are never returned.



    @library{wxbase}
    @category{data}

    @see wxStringTokenize()
*/
class wxStringTokenizer : public wxObject
{
public:
    //@{
    /**
        Constructor. Pass the string to tokenize, a string containing delimiters
        and the mode specifying how the string should be tokenized.
    */
    wxStringTokenizer();
    wxStringTokenizer(const wxString& str,
                      const wxString& delims = " \t\r\n",
                      wxStringTokenizerMode mode = wxTOKEN_DEFAULT);
    //@}

    /**
        Returns the number of tokens remaining in the input string. The number of
        tokens returned by this function is decremented each time
        GetNextToken() is called and when it
        reaches 0 HasMoreTokens() returns
        @false.
    */
    int CountTokens() const;

    /**
        Returns the delimiter which ended scan for the last token returned by
        GetNextToken() or @c NUL if
        there had been no calls to this function yet or if it returned the trailing
        empty token in @c wxTOKEN_RET_EMPTY_ALL mode.

        @wxsince{2.7.0}
    */
    wxChar GetLastDelimiter();

    /**
        Returns the next token or empty string if the end of string was reached.
    */
    wxString GetNextToken() const;

    /**
        Returns the current position (i.e. one index after the last returned
        token or 0 if GetNextToken() has never been called) in the original
        string.
    */
    size_t GetPosition() const;

    /**
        Returns the part of the starting string without all token already extracted.
    */
    wxString GetString() const;

    /**
        Returns @true if the tokenizer has further tokens, @false if none are left.
    */
    bool HasMoreTokens() const;

    /**
        Initializes the tokenizer.
        Pass the string to tokenize, a string containing delimiters,
        and the mode specifying how the string should be tokenized.
    */
    void SetString(const wxString& to_tokenize,
                   const wxString& delims = " \t\r\n",
                   wxStringTokenizerMode mode = wxTOKEN_DEFAULT);
};

