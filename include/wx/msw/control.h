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

#ifdef __GNUG__
    #pragma interface "control.h"
#endif

#include "wx/dynarray.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxControlNameStr;

// General item class
class WXDLLEXPORT wxControl : public wxControlBase
{
    DECLARE_ABSTRACT_CLASS(wxControl)

public:
   wxControl();
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

    // Calls the callback and appropriate event handlers
    bool ProcessCommand(wxCommandEvent& event);

    // MSW-specific
#ifdef __WIN95__
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
#endif // Win95

    // For ownerdraw items
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *WXUNUSED(item)) { return FALSE; };
    virtual bool MSWOnMeasure(WXMEASUREITEMSTRUCT *WXUNUSED(item)) { return FALSE; };

    wxArrayLong GetSubcontrols() { return m_subControls; }

    void OnEraseBackground(wxEraseEvent& event);

    virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

#if WXWIN_COMPATIBILITY
    virtual void SetButtonColour(const wxColour& WXUNUSED(col)) { }
    wxColour* GetButtonColour() const { return NULL; }

    inline virtual void SetLabelFont(const wxFont& font);
    inline virtual void SetButtonFont(const wxFont& font);
    inline wxFont& GetLabelFont() const;
    inline wxFont& GetButtonFont() const;

    // Adds callback
    inline void Callback(const wxFunction function);

    wxFunction GetCallback() { return m_callback; }

protected:
    wxFunction       m_callback;     // Callback associated with the window
#endif // WXWIN_COMPATIBILITY

protected:
    // for controls like radiobuttons which are really composite this array
    // holds the ids (not HWNDs!) of the sub controls
    wxArrayLong m_subControls;

    virtual wxSize DoGetBestSize() const;

    // create the control of the given class with the given style, returns FALSE
    // if creation failed
    //
    // All parameters except classname and style are optional, if the
    // size/position are not given, they should be set later with SetSize() and,
    // label (the title of the window), of course, is left empty. The extended
    // style is determined from the style and the app 3D settings automatically
    // if it's not specified explicitly.
    bool MSWCreateControl(const wxChar *classname,
            WXDWORD style,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const wxString& label = wxEmptyString,
            WXDWORD exstyle = (WXDWORD)-1);

    // determine the extended styles combination for this window (may slightly
    // modify style parameter, this is why it's non const)
    WXDWORD GetExStyle(WXDWORD& style, bool *want3D) const;

private:
    DECLARE_EVENT_TABLE()
};


#if WXWIN_COMPATIBILITY
    inline void wxControl::Callback(const wxFunction f) { m_callback = f; };
    inline wxFont& wxControl::GetLabelFont() const { return (wxFont &)GetFont(); }
    inline wxFont& wxControl::GetButtonFont() const { return (wxFont &)GetFont(); }
    inline void wxControl::SetLabelFont(const wxFont& font) { SetFont(font); }
    inline void wxControl::SetButtonFont(const wxFont& font) { SetFont(font); }
#endif // WXWIN_COMPATIBILITY

#endif
    // _WX_CONTROL_H_
