///////////////////////////////////////////////////////////////////////////////
// Name:        wx/checkbox.h
// Purpose:     wxCheckBox class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.09.00
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKBOX_H_BASE_
#define _WX_CHECKBOX_H_BASE_

#if wxUSE_CHECKBOX

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxCheckBoxNameStr;

// ----------------------------------------------------------------------------
// wxCheckBox: a control which shows a label and a box which may be checked
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCheckBoxBase : public wxControl
{
public:
    // set/get the checked status of the listbox
    virtual void SetValue(bool value) = 0;
    virtual bool GetValue() const = 0;

    bool IsChecked() const { return GetValue(); }
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/checkbox.h"
#elif defined(__WXMSW__)
    #include "wx/msw/checkbox.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/checkbox.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/checkbox.h"
#elif defined(__WXQT__)
    #include "wx/qt/checkbox.h"
#elif defined(__WXMAC__)
    #include "wx/mac/checkbox.h"
#elif defined(__WXPM__)
    #include "wx/os2/checkbox.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/checkbox.h"
#endif

#endif // wxUSE_CHECKBOX

#endif
    // _WX_CHECKBOX_H_BASE_
