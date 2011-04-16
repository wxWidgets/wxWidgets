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

    virtual void GetCompletions(const wxString& prefix, wxArrayString& res) = 0;

    virtual ~wxTextCompleter();

private:
    wxDECLARE_NO_COPY_CLASS(wxTextCompleter);
};

#endif // _WX_TEXTCOMPLETER_H_
