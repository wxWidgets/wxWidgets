///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/nonownedwnd.h
// Purpose:     wxNonOwnedWindow declaration for wxMSW.
// Author:      Vadim Zeitlin
// Created:     2011-10-09
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_NONOWNEDWND_H_
#define _WX_MSW_NONOWNEDWND_H_

class wxNonOwnedWindowShapeImpl;

// ----------------------------------------------------------------------------
// wxNonOwnedWindow
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNonOwnedWindow : public wxNonOwnedWindowBase
{
public:
    wxNonOwnedWindow();
    virtual ~wxNonOwnedWindow();

    virtual bool Reparent(wxWindowBase* newParent);
    virtual void InheritAttributes() wxOVERRIDE;

protected:
    virtual bool DoClearShape() wxOVERRIDE;
    virtual bool DoSetRegionShape(const wxRegion& region) wxOVERRIDE;
#if wxUSE_GRAPHICS_CONTEXT
    virtual bool DoSetPathShape(const wxGraphicsPath& path) wxOVERRIDE;
#endif // wxUSE_GRAPHICS_CONTEXT

    virtual WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) wxOVERRIDE;

private:
    bool HandleDPIChange(const wxSize& newDPI, const wxRect& newRect);

#if wxUSE_GRAPHICS_CONTEXT
    wxNonOwnedWindowShapeImpl* m_shapeImpl;
#endif // wxUSE_GRAPHICS_CONTEXT

    // Keep track of the DPI used in this window. So when per-monitor dpi
    // awareness is enabled, both old and new DPI are known for
    // wxDPIChangedEvent and wxWindow::MSWUpdateOnDPIChange.
    wxSize m_activeDPI;

    // This window supports handling per-monitor DPI awareness when the
    // application manifest contains <dpiAwareness>PerMonitorV2</dpiAwareness>.
    bool m_perMonitorDPIaware;

    wxDECLARE_NO_COPY_CLASS(wxNonOwnedWindow);
};

#endif // _WX_MSW_NONOWNEDWND_H_
