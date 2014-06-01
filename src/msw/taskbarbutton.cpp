/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/taskbarbutton.cpp
// Purpose:     Implements wxTaskbarButtonImpl class for manipulating buttons on
//              the Windows taskbar.
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-06-01
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TASKBARBUTTON

#include "wx/msw/wrapshl.h"
#include "wx/msw/private.h"
#include "wx/taskbarbutton.h"

#include <Shobjidl.h>

wxTaskBarButtonImpl::wxTaskBarButtonImpl(WXWidget parent) : m_hwnd(parent)
{
    HRESULT hr = CoCreateInstance
                 (
                    CLSID_TaskbarList,
                    NULL,
                    CLSCTX_ALL,
                    IID_ITaskbarList3,
                    reinterpret_cast<void **>(&m_taskbarList)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError(wxT("CoCreateInstance(CLSID_TaskbarButtonList)"), hr);
        return;
    }

    hr = m_taskbarList->HrInit();
    if ( FAILED(hr) )
    {
        wxLogApiError(wxT("ITaskbarButtonList3::Init"), hr);
        return;
    }
}

wxTaskBarButtonImpl::~wxTaskBarButtonImpl()
{
    if ( m_taskbarList )
      m_taskbarList->Release();
}

void wxTaskBarButtonImpl::SetProgressValue(int value)
{
    wxCHECK_RET( value >= 0 && value <= 100,
                 wxT("Invalid value, must be in the range of [0, 100].") );

    m_taskbarList->SetProgressValue(m_hwnd, value, 100);
}

#endif // wxUSE_TASKBARBUTTON
