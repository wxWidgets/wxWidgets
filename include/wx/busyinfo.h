/////////////////////////////////////////////////////////////////////////////
// Name:        busyinfo.h
// Purpose:     Information window (when app is busy)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __INFOWIN_H__
#define __INFOWIN_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "busyinfo.h"
#endif

#include "wx/defs.h"

#include "wx/frame.h"

#if wxUSE_BUSYINFO

class WXDLLEXPORT wxInfoFrame : public wxFrame
{
public:
    wxInfoFrame(wxWindow *parent, const wxString& message);

private:
    DECLARE_NO_COPY_CLASS(wxInfoFrame)
};


//--------------------------------------------------------------------------------
// wxBusyInfo
//                  Displays progress information
//                  Can be used in exactly same way as wxBusyCursor
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxBusyInfo : public wxObject
{
public:
    wxBusyInfo(const wxString& message, wxWindow *parent = NULL);

    virtual ~wxBusyInfo();

private:
    wxInfoFrame *m_InfoFrame;

    DECLARE_NO_COPY_CLASS(wxBusyInfo)
};


#endif

#endif
