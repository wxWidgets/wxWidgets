/////////////////////////////////////////////////////////////////////////////
// Name:        busyinfo.h
// Purpose:     Information window (when app is busy)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __INFOWIN_H__
#define __INFOWIN_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/wxprec.h"

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#include "wx/frame.h"

#if wxUSE_BUSYINFO

class WXDLLEXPORT wxInfoFrame : public wxFrame
{
    public:
        wxInfoFrame(wxWindow *parent, const wxString& message);
};


//--------------------------------------------------------------------------------
// wxBusyInfo
//                  Displays progress information
//                  Can be used in exactly same way as wxBusyCursor
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxBusyInfo : public wxObject
{
    public:
        wxBusyInfo(const wxString& message);
        ~wxBusyInfo();

    private:
        wxInfoFrame *m_InfoFrame;
};


#endif

#endif
