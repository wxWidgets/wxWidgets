/////////////////////////////////////////////////////////////////////////////
// Name:        include/taskbarbutton.h
// Purpose:     Defines wxTaskBarButton class for manipulating buttons on the
//              windows taskbar.
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-04-30
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TASKBARBUTTON_H_
#define _WX_TASKBARBUTTON_H_

#if wxUSE_TASKBARBUTTON

// ----------------------------------------------------------------------------
// wxTaskBarButton: define wxTaskBarButton interface.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxTaskBarButton
{
public:
    wxTaskBarButton() { }
    virtual ~wxTaskBarButton() { }

    // Operations:
    virtual void SetProgressValue(int value) = 0;
    virtual void ShowInTaskbar() = 0;
    virtual void HideInTaskbar() = 0;

private:
    wxDECLARE_NO_COPY_CLASS(wxTaskBarButton);
};


#if defined(__WXMSW__)
    #include "wx/msw/taskbarbutton.h"
#endif

#endif // wxUSE_TASKBARBUTTON

#endif  // _WX_TASKBARBUTTON_H_
