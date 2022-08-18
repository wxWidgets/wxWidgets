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
        //
        // Note the special case of SW_MINIMIZE: while GetWindowPlacement()
        // returns SW_SHOWMINIMIZED when the window is iconized, we restore it
        // as SW_MINIMIZE as this is what the code in wxTLW checks to determine
        // whether the window is supposed to be iconized or not.
        //
        // Just to confuse matters further, note that SW_MAXIMIZE is exactly
        // the same thing as SW_SHOWMAXIMIZED.
        int tmp;
        UINT& show = m_placement.showCmd;
        if ( ser.RestoreField(wxPERSIST_TLW_MAXIMIZED, &tmp) && tmp )
            show = SW_MAXIMIZE;
        else if ( ser.RestoreField(wxPERSIST_TLW_ICONIZED, &tmp) && tmp )
            show = SW_MINIMIZE;
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

        if (m_placement.showCmd != SW_SHOWMAXIMIZED && m_placement.showCmd != SW_SHOWMINIMIZED)
        {
            RECT rcWindow;
            ::GetWindowRect(tlw->GetHWND(), &rcWindow);
            // Height and width should be the same unless the user performed
            // an Aero Snap operation.
            const RECT rcNormal = m_placement.rcNormalPosition;
            if ((rcWindow.bottom - rcWindow.top) != (rcNormal.bottom - rcNormal.top) ||
                (rcWindow.right - rcWindow.left) != (rcNormal.right - rcNormal.left))
            {
                WinStruct<MONITORINFO> mi;
                if (!::GetMonitorInfo(::MonitorFromWindow(tlw->GetHWND(),
                    MONITOR_DEFAULTTONEAREST), &mi))
                {
                    wxLogLastError("GetMonitorInfo");
                    return false;
                }

                // If the tray is on the top or the left, then the rectangle needs to
                // be adjusted to match what ::SetWindowPlacement expects.
                if (mi.rcMonitor.top < mi.rcWork.top ||
                    mi.rcMonitor.left < mi.rcWork.left)
                {
                    // Negative offset to eliminate the tray width/height.
                    OffsetRect(&rcWindow, (mi.rcMonitor.left - mi.rcWork.left),
                         (mi.rcMonitor.top - mi.rcWork.top));
                }

                ::CopyRect(&m_placement.rcNormalPosition, &rcWindow);
            }
        }

        return true;
    }

    virtual bool ApplyTo(wxTopLevelWindow* tlw) wxOVERRIDE
    {
        // There is a subtlety here: if the window is currently hidden,
        // restoring its geometry shouldn't show it, so we must use SW_HIDE as
        // show command, but showing it later should restore it to the correct
        // state, so we need to remember it in wxTLW itself. And even if it's
        // currently shown, we still need to update its show command, so that
        // it matches the real window state after SetWindowPlacement() call.
        tlw->MSWSetShowCommand(m_placement.showCmd);
        if ( !tlw->IsShown() )
        {
            m_placement.showCmd = SW_HIDE;
        }

        const wxSize oldDPI = tlw->GetDPI();
        if ( !::SetWindowPlacement(GetHwndOf(tlw), &m_placement) )
        {
            wxLogLastError(wxS("SetWindowPlacement"));
            return false;
        }

        // Check if a window DPI hasn't changed, as the result of being placed
        // on a monitor with a different DPI from the main one because in this
        // case its size is not restored properly.
        const wxSize newDPI = tlw->GetDPI();
        if ( oldDPI != newDPI )
        {
            // So try setting the placement again as now it will use the
            // correct scaling factor, because the window is already on the
            // correct monitor.
            if ( !::SetWindowPlacement(GetHwndOf(tlw), &m_placement) )
            {
                wxLogLastError(wxS("SetWindowPlacement (2nd time)"));
                return false;
            }
        }

        return true;
    }

private:
    WINDOWPLACEMENT m_placement;
};

#endif // _WX_MSW_PRIVATE_TLWGEOM_H_
