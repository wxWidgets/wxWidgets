/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "control.h"
#endif

#include "wx/dynarray.h"

// General item class
class WXDLLEXPORT wxControl : public wxControlBase
{
public:
    wxControl() { }

    wxControl(wxWindow *parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxControlNameStr)
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxControlNameStr);

    virtual ~wxControl();

    // Simulates an event
    virtual void Command(wxCommandEvent& event) { ProcessCommand(event); }


    // implementation from now on
    // --------------------------

    virtual wxVisualAttributes GetDefaultAttributes() const
    {
        return GetClassDefaultAttributes(GetWindowVariant());
    }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // Calls the callback and appropriate event handlers
    bool ProcessCommand(wxCommandEvent& event);

    // MSW-specific
#ifdef __WIN95__
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
#endif // Win95

    // For ownerdraw items
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *WXUNUSED(item)) { return false; };
    virtual bool MSWOnMeasure(WXMEASUREITEMSTRUCT *WXUNUSED(item)) { return false; };

    const wxArrayLong& GetSubcontrols() const { return m_subControls; }

protected:
    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const;

    // return default best size (doesn't really make any sense, override this)
    virtual wxSize DoGetBestSize() const;

    // This is a helper for all wxControls made with UPDOWN native control.
    // In wxMSW it was only wxSpinCtrl derived from wxSpinButton but in
    // WinCE of Smartphones this happens also for native wxTextCtrl,
    // wxChoice and others.
    virtual wxSize GetBestSpinerSize(const bool is_vertical) const;

    // create the control of the given Windows class: this is typically called
    // from Create() method of the derived class passing its label, pos and
    // size parameter (style parameter is not needed because m_windowStyle is
    // supposed to had been already set and so is used instead when this
    // function is called)
    bool MSWCreateControl(const wxChar *classname,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size);

    // NB: the method below is deprecated now, with MSWGetStyle() the method
    //     above should be used instead! Once all the controls are updated to
    //     implement MSWGetStyle() this version will disappear.
    //
    // create the control of the given class with the given style (combination
    // of WS_XXX flags, i.e. Windows style, not wxWidgets one), returns
    // false if creation failed
    //
    // All parameters except classname and style are optional, if the
    // size/position are not given, they should be set later with SetSize()
    // and, label (the title of the window), of course, is left empty. The
    // extended style is determined from the style and the app 3D settings
    // automatically if it's not specified explicitly.
    bool MSWCreateControl(const wxChar *classname,
                          WXDWORD style,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          const wxString& label = wxEmptyString,
                          WXDWORD exstyle = (WXDWORD)-1);

    // default style for the control include WS_TABSTOP if it AcceptsFocus()
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

    // default handling of WM_CTLCOLORxxx
    virtual WXHBRUSH MSWControlColor(WXHDC pDC);

    // call this from the derived class MSWControlColor() if you want to show
    // the control greyed out
    WXHBRUSH MSWControlColorDisabled(WXHDC pDC);

    // call this from the derived class MSWControlColor() if you want to always
    // paint the background (as all opaque controls do)
    WXHBRUSH MSWControlColorSolid(WXHDC pDC)
    {
        return MSWControlColorSolid(pDC, GetBackgroundColour());
    }

    // common part of the 3 methods above
    WXHBRUSH MSWControlColorSolid(WXHDC pDC, wxColour colBg);

    // another WM_CTLCOLOR-related function: if it returns false, we return
    // NULL_BRUSH from MSWControlColor() to prevent the system from erasing the
    // background at all
    //
    // override this function to return true for controls which do want to
    // draw their background themselves
    virtual bool MSWAlwaysDrawBg() const { return false; }

    // this is a helper for the derived class GetClassDefaultAttributes()
    // implementation: it returns the right colours for the classes which
    // contain something else (e.g. wxListBox, wxTextCtrl, ...) instead of
    // being simple controls (such as wxButton, wxCheckBox, ...)
    static wxVisualAttributes
        GetCompositeControlsDefaultAttributes(wxWindowVariant variant);

    // for controls like radiobuttons which are really composite this array
    // holds the ids (not HWNDs!) of the sub controls
    wxArrayLong m_subControls;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxControl)
};

#endif // _WX_CONTROL_H_
