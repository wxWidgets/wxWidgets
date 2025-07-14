/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/accel.cpp
// Purpose:     wxAcceleratorTable
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_ACCEL

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

#include "wx/accel.h"

#include "wx/msw/private.h"
#include "wx/msw/private/keyboard.h"

#include <vector>

wxIMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable, wxObject);

// ----------------------------------------------------------------------------
// data defining wxAcceleratorTable
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorRefData: public wxObjectRefData
{
    friend class WXDLLIMPEXP_FWD_CORE wxAcceleratorTable;
public:
    explicit wxAcceleratorRefData(HACCEL hAccel);
    virtual ~wxAcceleratorRefData();

    HACCEL GetHACCEL() const { return m_hAccel; }

protected:
    const HACCEL m_hAccel;

    wxDECLARE_NO_COPY_CLASS(wxAcceleratorRefData);
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxAcceleratorRefData
// ----------------------------------------------------------------------------

#define M_ACCELDATA ((wxAcceleratorRefData *)m_refData)

wxAcceleratorRefData::wxAcceleratorRefData(HACCEL hAccel)
    : m_hAccel(hAccel)
{
}

wxAcceleratorRefData::~wxAcceleratorRefData()
{
    if (m_hAccel)
    {
        DestroyAcceleratorTable((HACCEL) m_hAccel);
    }
}

// ----------------------------------------------------------------------------
// wxAcceleratorTable
// ----------------------------------------------------------------------------

// Load from .rc resource
wxAcceleratorTable::wxAcceleratorTable(const wxString& resource)
{
    HACCEL hAccel = ::LoadAccelerators(wxGetInstance(), resource.t_str());
    if ( hAccel )
        m_refData = new wxAcceleratorRefData(hAccel);
}

// Create from an array
wxAcceleratorTable::wxAcceleratorTable(int n, const wxAcceleratorEntry entries[])
{
    if ( n == 0 )
    {
        // This is valid but useless.
        return;
    }

    wxCHECK_RET( n > 0, "Invalid number of accelerator entries" );

    std::vector<ACCEL> arr(n);
    for ( int i = 0; i < n; i++ )
    {
        int flags = entries[i].GetFlags();

        BYTE fVirt = FVIRTKEY;
        if ( flags & wxACCEL_ALT )
            fVirt |= FALT;
        if ( flags & wxACCEL_SHIFT )
            fVirt |= FSHIFT;
        if ( flags & wxACCEL_CTRL )
            fVirt |= FCONTROL;

        WORD key = wxMSWKeyboard::WXToVK(entries[i].GetKeyCode());

        arr[i].fVirt = fVirt;
        arr[i].key = key;
        arr[i].cmd = (WORD)entries[i].GetCommand();
    }

    const HACCEL hAccel = ::CreateAcceleratorTable(&arr[0], n);
    if ( hAccel )
        m_refData = new wxAcceleratorRefData(hAccel);
}

bool wxAcceleratorTable::IsOk() const
{
    return m_refData != nullptr;
}

void wxAcceleratorTable::SetHACCEL(WXHACCEL hAccel)
{
    // This should really do AllocExclusive() to reuse the existing object if
    // it is not shared, but for now keep things simple and just always create
    // a new wxAcceleratorRefData object.
    UnRef();

    if ( hAccel )
        m_refData = new wxAcceleratorRefData(hAccel);
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
    return IsOk() && ::TranslateAccelerator(GetHwndOf(window), GetHaccel(), msg);
}

#endif // wxUSE_ACCEL

