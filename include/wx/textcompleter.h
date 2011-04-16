///////////////////////////////////////////////////////////////////////////////
// Name:        wx/textcompleter.h
// Purpose:     Declaration of wxTextCompleter class.
// Author:      Vadim Zeitlin
// Created:     2011-04-13
// RCS-ID:      $Id: wxhead.h,v 1.12 2010-04-22 12:44:51 zeitlin Exp $
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTCOMPLETER_H_
#define _WX_TEXTCOMPLETER_H_

// ----------------------------------------------------------------------------
// wxTextCompleter: used by wxTextEnter::AutoComplete()
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextCompleter
{
public:
    wxTextCompleter() { }

    // The virtual functions to be implemented by the derived classes: the
    // first one is called to start preparing for completions for the given
    // prefix and, if it returns true, GetNext() is called until it returns an
    // empty string indicating that there are no more completions.
    virtual bool Start(const wxString& prefix) = 0;
    virtual wxString GetNext() = 0;

    virtual ~wxTextCompleter();

private:
    wxDECLARE_NO_COPY_CLASS(wxTextCompleter);
};

// ----------------------------------------------------------------------------
// wxTextCompleterSimple: returns the entire set of completions at once
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextCompleterSimple : public wxTextCompleter
{
public:
    wxTextCompleterSimple() { }

    // Must be implemented to return all the completions for the given prefix.
    virtual void GetCompletions(const wxString& prefix, wxArrayString& res) = 0;

    virtual bool Start(const wxString& prefix);
    virtual wxString GetNext();

private:
    wxArrayString m_completions;
    unsigned m_index;

    wxDECLARE_NO_COPY_CLASS(wxTextCompleterSimple);
};

#endif // _WX_TEXTCOMPLETER_H_

