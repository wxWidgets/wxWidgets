///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/aboutdlgg.h
// Purpose:     generic wxAboutDialog implementation
// Author:      Vadim Zeitlin
// Created:     2006-10-07
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_ABOUTDLGG_H_
#define _WX_GENERIC_ABOUTDLGG_H_

#include "wx/defs.h"

#if wxUSE_ABOUTDLG

#include "wx/dialog.h"

class WXDLLIMPEXP_CORE wxAboutDialogInfo;
class WXDLLIMPEXP_CORE wxSizer;

// ----------------------------------------------------------------------------
// wxAboutDialog: generic "About" dialog implementation
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAboutDialog : public wxDialog
{
public:
    // constructors and Create() method
    // --------------------------------

    // default ctor, you must use Create() to really initialize the dialog
    wxAboutDialog() { Init(); }

    // ctor which fully initializes the object
    wxAboutDialog(const wxAboutDialogInfo& info)
    {
        Init();

        (void)Create(info);
    }

    // this method must be called if and only if the default ctor was used
    bool Create(const wxAboutDialogInfo& info);

protected:
    // common part of all ctors
    void Init() { m_sizerText = NULL; }

    // add arbitrary control to the text sizer contents
    void AddControl(wxWindow *win);

    // add the text, if it's not empty, to the text sizer contents
    void AddText(const wxString& text);


    wxSizer *m_sizerText;
};

// unlike wxAboutBox which can show either the native or generic about dialog,
// this function always shows the generic one
WXDLLIMPEXP_CORE void wxGenericAboutBox(const wxAboutDialogInfo& info);

#endif // wxUSE_ABOUTDLG

#endif // _WX_GENERIC_ABOUTDLGG_H_

