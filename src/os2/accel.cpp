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
} // end of wxAcceleratorRefData::wxAcceleratorRefData

wxAcceleratorRefData::~wxAcceleratorRefData()
{
    if (m_hAccel)
    {
        WinDestroyAccelTable((HACCEL) m_hAccel);
    }
    m_hAccel = 0 ;
} // end of wxAcceleratorRefData::~wxAcceleratorRefData

wxAcceleratorTable::wxAcceleratorTable()
{
    m_refData = NULL;
} // end of wxAcceleratorTable::wxAcceleratorTable

wxAcceleratorTable::~wxAcceleratorTable()
{
} // end of wxAcceleratorTable::~wxAcceleratorTable

// Load from .rc resource
wxAcceleratorTable::wxAcceleratorTable(
  const wxString&                  rResource
)
{
    HACCEL                          hAccel;
    ULONG                           ulId;

    m_refData = new wxAcceleratorRefData;

    ulId = atol((char*)rResource.c_str());
    hAccel = ::WinLoadAccelTable( vHabmain
                                 ,NULL // resources always in .exe
                                 ,(ULONG)ulId
                                );
    if (wxTheApp->GetTopWindow() != NULL)
    {
        //
        // If we have accelerators the top window is the frame
        //
        wxFrame*                    pFrame = (wxFrame*)wxTheApp->GetTopWindow();

        ::WinSetAccelTable( vHabmain
                           ,(HWND)pFrame->GetFrame()
                           ,hAccel
                          );
    }
    M_ACCELDATA->m_hAccel = hAccel;
    M_ACCELDATA->m_ok = (hAccel != 0);
}

extern int wxCharCodeWXToOS2(
  int                               nId
, bool*                             pbIsVirtual
);

// Create from an array
wxAcceleratorTable::wxAcceleratorTable(
  int                               n
, wxAcceleratorEntry                vaEntries[]
)
{
    int                             nAccelLength = ((sizeof(ACCEL) * n) + sizeof(ACCELTABLE));
    PACCELTABLE                     pArr;
    int                             i;

    m_refData = new wxAcceleratorRefData;
    pArr = (PACCELTABLE) new BYTE[nAccelLength];

    for (i = 0; i < n; i++)
    {
        USHORT                      uVirt = AF_CHAR;

        if (vaEntries[i].GetFlags() & wxACCEL_ALT)
            uVirt |= AF_ALT;
        if (vaEntries[i].GetFlags() & wxACCEL_SHIFT)
            uVirt |= AF_SHIFT;
        if (vaEntries[i].GetFlags() & wxACCEL_CTRL)
            uVirt |= AF_CONTROL;

        bool                        bIsVirtual;
        USHORT                      uKey = wxCharCodeWXToOS2( vaEntries[i].GetKeyCode()
                                                             ,&bIsVirtual
                                                            );
        if (bIsVirtual)
            uVirt = AF_CHAR | AF_VIRTUALKEY;

        USHORT                      uCmd = vaEntries[i].GetCommand();

        pArr->aaccel[i].fs  = uVirt;
        pArr->aaccel[i].key = uKey;
        pArr->aaccel[i].cmd = uCmd;
    }
    pArr->codepage = 437; // default to english Fix???
    pArr->cAccel = (USHORT)n;
    M_ACCELDATA->m_hAccel = ::WinCreateAccelTable( vHabmain
                                                  ,pArr
                                                 );
    if (wxTheApp->GetTopWindow() != NULL)
    {
        //
        // If we have accelerators the top window is the frame
        //
        wxFrame*                    pFrame = (wxFrame*)wxTheApp->GetTopWindow();

        ::WinSetAccelTable( vHabmain
                           ,(HWND)pFrame->GetFrame()
                           ,M_ACCELDATA->m_hAccel
                          );
    }

    delete[] pArr;
    M_ACCELDATA->m_ok = (M_ACCELDATA->m_hAccel != 0);
} // end of wxAcceleratorTable::wxAcceleratorTable

bool wxAcceleratorTable::Ok() const
{
    return(M_ACCELDATA && (M_ACCELDATA->m_ok));
} // end of wxAcceleratorTable::Ok

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

bool wxAcceleratorTable::Translate(
  WXHWND                            hWnd
, WXMSG*                            pWxmsg
) const
{
    PQMSG                           pMsg = (PQMSG)pWxmsg;

    return Ok() && ::WinTranslateAccel( vHabmain
                                       ,(HWND)hWnd
                                       ,GetHaccel()
                                       ,pMsg
                                       );
} // end of wxAcceleratorTable::Translate

