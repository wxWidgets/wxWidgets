/////////////////////////////////////////////////////////////////////////////
// Name:        html/htmltag.h
// Purpose:     interface of wxHtmlTag
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHtmlTag

    This class represents a single HTML tag.
    It is used by @ref overview_handlers "tag handlers".

    @library{wxhtml}
    @category{html}
*/
class wxHtmlTag
{
protected:
    /**
        Constructor. You will probably never have to construct a wxHtmlTag object
        yourself. Feel free to ignore the constructor parameters.
        Have a look at src/html/htmlpars.cpp if you're interested in creating it.
    */
    wxHtmlTag(wxHtmlTag* parent, const wxString* source,
              const const_iterator& pos, const const_iterator& end_pos,
              wxHtmlTagsCache* cache, wxHtmlEntitiesParser* entParser);

public:
    /**
        Returns a string containing all parameters.
        Example : tag contains @c FONT SIZE=+2 COLOR="#000000". Call to
        tag.GetAllParams() would return @c SIZE=+2 COLOR="#000000".
    */
    const wxString GetAllParams() const;

    /**
        Returns beginning position of the text @e between this tag and paired
        ending tag.
        See explanation (returned position is marked with '|'):
        @deprecated @todo provide deprecation description
    */
    int GetBeginPos() const;

    /**
        Returns ending position of the text @e between this tag and paired
        ending tag.
        See explanation (returned position is marked with '|'):
        @deprecated @todo provide deprecation description
    */
    int GetEndPos1() const;

    /**
        Returns ending position 2 of the text @e between this tag and paired
        ending tag.
        See explanation (returned position is marked with '|'):
        @deprecated @todo provide deprecation description
    */
    int GetEndPos2() const;

    /**
        Returns tag's name. The name is always in uppercase and it doesn't contain
        " or '/' characters. (So the name of @c FONT SIZE=+2 tag is "FONT"
        and name of @c /table is "TABLE")
    */
    wxString GetName() const;

    /**
        Returns the value of the parameter. You should check whether the
        parameter exists or not (use wxHtmlTag::HasParam) first.

        @param par
            The parameter's name.
        @param with_quotes
            @true if you want to get quotes as well. See example.
    */
    wxString GetParam(const wxString& par, bool with_quotes = false) const;

    /**
        Interprets tag parameter @a par as colour specification and saves its value
        into wxColour variable pointed by @e clr.
        Returns @true on success and @false if @a par is not colour specification or
        if the tag has no such parameter.
    */
    bool GetParamAsColour(const wxString& par, wxColour* clr) const;

    /**
        Interprets tag parameter @a par as an integer and saves its value
        into int variable pointed by @e value.
        Returns @true on success and @false if @a par is not an integer or
        if the tag has no such parameter.
    */
    bool GetParamAsInt(const wxString& par, int* value) const;

    /**
        Returns @true if this tag is paired with ending tag, @false otherwise.
        See the example of HTML document:

        In this example tags HTML and BODY have ending tags, first P and BR
        doesn't have ending tag while the second P has. The third P tag (which
        is ending itself) of course doesn't have ending tag.
    */
    bool HasEnding() const;

    /**
        Returns @true if the tag has a parameter of the given name.
        Example : @c FONT SIZE=+2 COLOR="\#FF00FF" has two parameters named
        "SIZE" and "COLOR".

        @param par
            the parameter you're looking for.
    */
    bool HasParam(const wxString& par) const;

    /**
        This method scans the given parameter. Usage is exactly the same as sscanf's
        usage except that you don't pass a string but a parameter name as the first
        argument
        and you can only retrieve one value (i.e. you can use only one "%" element
        in @e format).

        @param par
            The name of the tag you want to query
        @param format
            scanf()-like format string.
        @param value
            pointer to a variable to store the value in
    */
    wxString ScanParam(const wxString& par, const wxChar* format,
                       void* value) const;
};

