/////////////////////////////////////////////////////////////////////////////
// Name:        accel.cpp
// Purpose:     wxAcceleratorTable
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/window.h"
#endif

#include "wx/os2/accel.h"

#include "wx/os2/private.h"


#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable, wxObject)
#endif

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
    // TODO
/*
    HACCEL      m_hAccel;
*/
}

wxAcceleratorRefData::~wxAcceleratorRefData()
{
/*
  if (m_hAccel)
  {
    DestroyAcceleratorTable((HACCEL) m_hAccel);
  }
  m_hAccel = 0 ;
*/
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

/* TODO: load acelerator from resource, if appropriate for your platform
    M_ACCELDATA->m_hAccel = hAccel;
    M_ACCELDATA->m_ok = (hAccel != 0);
*/
}

extern int wxCharCodeWXToOS2(int id, bool *isVirtual);

// Create from an array
wxAcceleratorTable::wxAcceleratorTable(int n, wxAcceleratorEntry entries[])
{
    m_refData = new wxAcceleratorRefData;

/* TODO: create table from entries
 */
}

bool wxAcceleratorTable::Ok() const
{
    // TODO
    return FALSE;
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
    // TODO:
/*
    MSG *msg = (MSG *)wxmsg;

    return Ok() && ::TranslateAccelerator(GetHwndOf(window), GetHaccel(), msg);
*/
    return FALSE;
}

