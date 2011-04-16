/////////////////////////////////////////////////////////////////////////////
// Name:        wx/textcompleter.h
// Purpose:     interface of wxTextCompleter
// Author:      Vadim Zeitlin
// Created:     2011-04-13
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTextCompleter

    Base class for custom text completer objects.

    Custom completer objects used with wxTextEntry::AutoComplete() must derive
    from this class and implement its pure virtual method returning the
    completions. You would typically use a custom completer when the total
    number of completions is too big for performance to be acceptable if all of
    them need to be returned at once but if they can be generated
    hierarchically, i.e. only the first component initially, then the second
    one after the user finished entering the first one and so on.

    Here is a simple example of a custom completer that completes the names of
    some chess pieces. Of course, as the total list here has only four items it
    would have been much simpler to just specify the array containing all the
    completions in this example but the same approach could be used when the
    total number of completions is much higher provided the number of
    possibilities for each word is still relatively small:
    @code
    class MyTextCompleter : public wxTextCompleter
    {
    public:
        virtual void GetCompletions(const wxString& prefix, wxArrayString& res)
        {
            const wxString firstWord = prefix.BeforeFirst(' ');
            if ( firstWord == "white" )
            {
                res.push_back("white pawn");
                res.push_back("white rook");
            }
            else if ( firstWord == "black" )
            {
                res.push_back("black king");
                res.push_back("black queen");
            }
            else
            {
                res.push_back("white");
                res.push_back("black");
            }
        }
    };
    ...
    wxTextCtrl *text = ...;
    text->AutoComplete(new MyTextCompleter);
    @endcode

    @library{wxcore}

    @since 2.9.2
*/
class wxTextCompleter
{
public:
    /**
        Pure virtual method returning all possible completions for the given
        prefix.

        The custom completer should examine the provided prefix and return all
        the possible completions for it in the output array @a res.

        Please notice that the returned values should start with the prefix,
        otherwise they will be simply ignored, making adding them to the array
        in the first place useless.

        Notice that this function may be called from thread other than main one
        (this is currently always the case under MSW) so care should be taken
        if it needs to access any shared data.

        @param prefix
            The possibly empty prefix that the user had already entered.
        @param res
            Initially empty array that should be filled with all possible
            completions (possibly none if there are no valid possibilities
            starting with the given prefix).
     */
    virtual void GetCompletions(const wxString& prefix, wxArrayString& res) = 0;
};
