/////////////////////////////////////////////////////////////////////////////
// Name:        accel.cpp
// Purpose:     wxAcceleratorTable
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "accel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/window.h"
#endif

#include "wx/msw/accel.h"

#include "wx/msw/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable, wxObject)

class WXDLLEXPORT wxAcceleratorRefData: public wxObjectRefData
{
    friend class WXDLLEXPORT wxAcceleratorTable;
public:
    wxAcceleratorRefData();
    ~wxAcceleratorRefData();

    inline HACCEL GetHACCEL() const { return m_hAccel; }
protected:
    HACCEL      m_hAccel;
    bool        m_ok;
};

#define M_ACCELDATA ((wxAcceleratorRefData *)m_refData)

wxAcceleratorRefData::wxAcceleratorRefData()
{
  m_ok = FALSE;
  m_hAccel = 0;
}

wxAcceleratorRefData::~wxAcceleratorRefData()
{
  if (m_hAccel)
  {
    // This function not available in WIN16
#if !defined(__WIN16__) && !defined(__TWIN32__)
    DestroyAcceleratorTable((HACCEL) m_hAccel);
#endif
  }
  m_hAccel = 0 ;
}

wxAcceleratorTable::wxAcceleratorTable()
{
  m_refData = NULL;
}

wxAcceleratorTable::~wxAcceleratorTable()
{
}

// Load from .rc resource
wxAcceleratorTable::wxAcceleratorTable(const wxString& resource)
{
    m_refData = new wxAcceleratorRefData;

    HACCEL hAccel =
#if defined(__WIN32__) && !defined(__TWIN32__)
#ifdef UNICODE
        ::LoadAcceleratorsW(wxGetInstance(), (const wxChar *)resource);
#else
        ::LoadAcceleratorsA(wxGetInstance(), (const char *)resource);
#endif
#else
        ::LoadAccelerators(wxGetInstance(), (const wxChar *)resource);
#endif
    M_ACCELDATA->m_hAccel = hAccel;
    M_ACCELDATA->m_ok = (hAccel != 0);
}

extern int wxCharCodeWXToMSW(int id, bool *isVirtual);

// Create from an array
#if !defined(__WIN16__) && !defined(__TWIN32__) && !defined(__WXWINE__)
wxAcceleratorTable::wxAcceleratorTable(int n, const wxAcceleratorEntry entries[])
{
    // Not available in WIN16
    m_refData = new wxAcceleratorRefData;

    ACCEL* arr = new ACCEL[n];
    int i;
    for (i = 0; i < n; i++)
    {
        BYTE fVirt = 0;
        if (entries[i].m_flags & wxACCEL_ALT)
            fVirt |= FALT;
        if (entries[i].m_flags & wxACCEL_SHIFT)
            fVirt |= FSHIFT;
        if (entries[i].m_flags & wxACCEL_CTRL)
            fVirt |= FCONTROL;

        bool isVirtual;
        WORD key = wxCharCodeWXToMSW(entries[i].m_keyCode, & isVirtual);
        fVirt |= FVIRTKEY;

        WORD cmd = entries[i].m_command;

        arr[i].fVirt = fVirt;
        arr[i].key = key;
        arr[i].cmd = cmd;
    }

    M_ACCELDATA->m_hAccel = ::CreateAcceleratorTable(arr, n);
    delete[] arr;

    M_ACCELDATA->m_ok = (M_ACCELDATA->m_hAccel != 0);
}
#else // Win16
wxAcceleratorTable::wxAcceleratorTable(int WXUNUSED(n), const wxAcceleratorEntry WXUNUSED(entries)[])
{
    // No, we simply gracefully degrade; we don't expect the
    // developer to pepper their code with #ifdefs just for this.
    // wxFAIL_MSG("not implemented");
}
#endif // Win32/16

bool wxAcceleratorTable::Ok() const
{
    return (M_ACCELDATA && (M_ACCELDATA->m_ok));
}

void wxAcceleratorTable::SetHACCEL(WXHACCEL hAccel)
{
    if (!M_ACCELDATA)
        m_refData = new wxAcceleratorRefData;

    M_ACCELDATA->m_hAccel = (HACCEL) hAccel;
}

WXHACCEL wxAcceleratorTable::GetHACCEL() const
{
    if (!M_ACCELDATA)
        return 0;
    return (WXHACCEL) M_ACCELDATA->m_hAccel;
}

bool wxAcceleratorTable::Translate(wxWindow *window, WXMSG *wxmsg) const
{
    MSG *msg = (MSG *)wxmsg;
    return Ok() && ::TranslateAccelerator(GetHwndOf(window), GetHaccel(), msg);
}

