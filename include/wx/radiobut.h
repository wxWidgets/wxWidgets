///////////////////////////////////////////////////////////////////////////////
// Name:        wx/radiobut.h
// Purpose:     wxRadioButton declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.09.00
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_BASE_
#define _WX_RADIOBUT_H_BASE_

#if wxUSE_RADIOBTN

/*
   There is no wxRadioButtonBase class as wxRadioButton interface is the same
   as of wxCheckBox(Base), but under some platforms wxRadioButton really
   derives from wxCheckBox and on the others it doesn't.

   The pseudo-declaration of wxRadioButtonBase would look like this:

   class wxRadioButtonBase : public ...
   {
   public:
        virtual void SetValue(bool value);
        virtual bool GetValue() const;
   };
 */

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxRadioButtonNameStr;

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/radiobut.h"
#elif defined(__WXMSW__)
    #include "wx/msw/radiobut.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/radiobut.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/radiobut.h"
#elif defined(__WXQT__)
    #include "wx/qt/radiobut.h"
#elif defined(__WXMAC__)
    #include "wx/mac/radiobut.h"
#elif defined(__WXPM__)
    #include "wx/os2/radiobut.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/radiobut.h"
#endif

#endif // wxUSE_RADIOBTN

#endif
    // _WX_RADIOBUT_H_BASE_
