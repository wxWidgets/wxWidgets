/////////////////////////////////////////////////////////////////////////////
// Name:        accel.cpp
// Purpose:     wxAcceleratorTable
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "accel.h"
#endif

#include "wx/setup.h"
#include "wx/accel.h"
#include "wx/string.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable, wxObject)
#endif

class WXDLLEXPORT wxAcceleratorRefData: public wxObjectRefData
{
    friend class WXDLLEXPORT wxAcceleratorTable;
public:
    wxAcceleratorRefData();
    ~wxAcceleratorRefData();

/* TODO: implementation
    inline HACCEL GetHACCEL() const { return m_hAccel; }
protected:
    HACCEL      m_hAccel;
*/
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

