///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/tlwgeom.h
// Purpose:     wxMSW-specific wxTLWGeometry class.
// Author:      Vadim Zeitlin
// Created:     2018-04-29
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_TLWGEOM_H_
#define _WX_MSW_PRIVATE_TLWGEOM_H_

#include "wx/log.h"

#include "wx/msw/private.h"

// names for MSW-specific options
#define wxPERSIST_TLW_MAX_X "xmax"
#define wxPERSIST_TLW_MAX_Y "ymax"

class wxTLWGeometry : public wxTLWGeometryBase
{
public:
    wxTLWGeometry()
    {
        wxZeroMemory(m_placement);
        m_placement.length = sizeof(m_placement);
    }

    virtual bool Save(const Serializer& ser) const wxOVERRIDE
    {
        // For compatibility with the existing saved positions/sizes, use the
        // same keys as the generic version (which was previously used under
        // MSW too).

        // Normal position and size.
        const RECT& rc = m_placement.rcNormalPosition;
        if ( !ser.SaveField(wxPERSIST_TLW_X, rc.left) ||
             !ser.SaveField(wxPERSIST_TLW_Y, rc.top) )
            return false;

        if ( !ser.SaveField(wxPERSIST_TLW_W, rc.right - rc.left) ||
             !ser.SaveField(wxPERSIST_TLW_H, rc.bottom - rc.top) )
            return false;

        // Maximized/minimized state.
        UINT show = m_placement.showCmd;
        if ( !ser.SaveField(wxPERSIST_TLW_MAXIMIZED, show == SW_SHOWMAXIMIZED) )
            return false;

        if ( !ser.SaveField(wxPERSIST_TLW_ICONIZED, show == SW_SHOWMINIMIZED) )
            return false;

        // Maximized window position.
        const POINT pt = m_placement.ptMaxPosition;
        if ( !ser.SaveField(wxPERSIST_TLW_MAX_X, pt.x) ||
             !ser.SaveField(wxPERSIST_TLW_MAX_Y, pt.y) )
            return false;

        // We don't currently save the minimized window position, it doesn't
        // seem useful for anything and is probably just a left over from
        // Windows 3.1 days, when icons were positioned on the desktop instead
        // of being located in the taskbar.

        return true;
    }

    virtual bool Restore(Serializer& ser) wxOVERRIDE
    {
        // Normal position and size.
        wxRect r;
        if ( !ser.RestoreField(wxPERSIST_TLW_X, &r.x) ||
             !ser.RestoreField(wxPERSIST_TLW_Y, &r.y) ||
             !ser.RestoreField(wxPERSIST_TLW_W, &r.width) ||
             !ser.RestoreField(wxPERSIST_TLW_H, &r.height) )
            return false;
        wxCopyRectToRECT(r, m_placement.rcNormalPosition);

        // Maximized/minimized state.
        int tmp;
        UINT& show = m_placement.showCmd;
        if ( ser.RestoreField(wxPERSIST_TLW_MAXIMIZED, &tmp) && tmp )
            show = SW_SHOWMAXIMIZED;
        else if ( ser.RestoreField(wxPERSIST_TLW_ICONIZED, &tmp) && tmp )
            show = SW_SHOWMINIMIZED;
        else
            show = SW_SHOWNORMAL;

        // Maximized window position.
        if ( ser.RestoreField(wxPERSIST_TLW_MAX_X, &r.x) &&
             ser.RestoreField(wxPERSIST_TLW_MAX_Y, &r.y) )
        {
            m_placement.ptMaxPosition.x = r.x;
            m_placement.ptMaxPosition.y = r.y;
        }

        return true;
    }

    virtual bool GetFrom(const wxTopLevelWindow* tlw) wxOVERRIDE
    {
        if ( !::GetWindowPlacement(GetHwndOf(tlw), &m_placement) )
        {
            wxLogLastError(wxS("GetWindowPlacement"));
            return false;
        }

        return true;
    }

    virtual bool ApplyTo(wxTopLevelWindow* tlw) wxOVERRIDE
    {
        if ( !::SetWindowPlacement(GetHwndOf(tlw), &m_placement) )
        {
            wxLogLastError(wxS("SetWindowPlacement"));
            return false;
        }

        return true;
    }

private:
    WINDOWPLACEMENT m_placement;
};

#endif // _WX_MSW_PRIVATE_TLWGEOM_H_
