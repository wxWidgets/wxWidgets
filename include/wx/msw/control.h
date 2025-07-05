/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/control.h
// Purpose:     wxControl class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

#include "wx/dynarray.h"

// General item class
class WXDLLIMPEXP_CORE wxControl : public wxControlBase
{
public:
    wxControl() = default;

    wxControl(wxWindow *parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxControlNameStr))
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxControlNameStr));


    // Simulates an event
    virtual void Command(wxCommandEvent& event) override { ProcessCommand(event); }


    // implementation from now on
    // --------------------------

    virtual wxVisualAttributes GetDefaultAttributes() const override
    {
        return GetClassDefaultAttributes(GetWindowVariant());
    }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // Calls the callback and appropriate event handlers
    bool ProcessCommand(wxCommandEvent& event);

    // MSW-specific
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) override;

    // For ownerdraw items
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *WXUNUSED(item)) { return false; }
    virtual bool MSWOnMeasure(WXMEASUREITEMSTRUCT *WXUNUSED(item)) { return false; }

    const wxArrayLong& GetSubcontrols() const { return m_subControls; }

    // default handling of WM_CTLCOLORxxx: this is public so that wxWindow
    // could call it
    virtual WXHBRUSH MSWControlColor(WXHDC pDC, WXHWND hWnd);

    // default style for the control include WS_TABSTOP if it AcceptsFocus()
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

protected:
    // Hook for common controls for which we don't want to set the default font
    // as if we do set it, the controls don't update their font size
    // automatically in response to WM_SETTINGCHANGE if it's changed in the
    // display properties in the control panel, so avoid doing this for them.
    virtual bool MSWShouldSetDefaultFont() const { return true; }

    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const override;

    // return default best size (doesn't really make any sense, override this)
    virtual wxSize DoGetBestSize() const override;

    // create the control of the given Windows class: this is typically called
    // from Create() method of the derived class passing its label, pos and
    // size parameter (style parameter is not needed because m_windowStyle is
    // supposed to had been already set and so is used instead when this
    // function is called)
    //
    // Note that this calls MSWGetStyle() to determine the Windows styles to
    // use, so it must be implemented correctly in the derived class.
    bool MSWCreateControl(const wxChar *classname,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size);

    // Prefer the overload above, this one should only be used if the styles
    // can't be completely determined from the window style.
    bool MSWCreateControl(const wxChar *classname,
                          WXDWORD style,
                          const wxPoint& pos,
                          const wxSize& size,
                          const wxString& label,
                          WXDWORD exstyle);

    // call this from the derived class MSWControlColor() if you want to show
    // the control greyed out (and opaque)
    WXHBRUSH MSWControlColorDisabled(WXHDC pDC);

    // common part of the 3 functions above: pass wxNullColour to use the
    // appropriate background colour (meaning ours or our parents) or a fixed
    // one
    virtual WXHBRUSH DoMSWControlColor(WXHDC pDC, wxColour colBg, WXHWND hWnd);

    // Look in our GetSubcontrols() for the windows with the given ID.
    virtual wxWindow *MSWFindItem(long id, WXHWND hWnd) const override;


    // Struct used for MSWGetDarkModeSupport() below.
    struct MSWDarkModeSupport
    {
        // The name of the theme to use (also called "app name").
        const wchar_t* themeName = nullptr;

        // The theme IDs to use. If neither this field nor the theme name is
        // set, no theme is applied to the window.
        const wchar_t* themeId = nullptr;

        // For some controls we need to set the foreground explicitly, even if
        // they have some support for the dark theme.
        bool setForeground = false;
    };

    // Called after creating the control to enable dark mode support if needed.
    //
    // If this function returns true, wxControl allows using dark mode for the
    // window and set its theme to the one specified by MSWDarkModeSupport
    // fields.
    virtual bool MSWGetDarkModeSupport(MSWDarkModeSupport& support) const;

    // Return the message that can be used to retrieve the tooltip window used
    // by a native control. If this message is non-zero and sending it returns
    // a valid HWND, the dark theme is also applied to it, if appropriate.
    virtual int MSWGetToolTipMessage() const { return 0; }


    // for controls like radiobuttons which are really composite this array
    // holds the ids (not HWNDs!) of the sub controls
    wxArrayLong m_subControls;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxControl);
};

#endif // _WX_CONTROL_H_
