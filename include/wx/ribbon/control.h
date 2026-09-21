///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/control.h
// Purpose:     Extension of wxControl with common ribbon methods
// Author:      Peter Cawley
// Created:     2009-06-05
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_CONTROL_H_
#define _WX_RIBBON_CONTROL_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/control.h"
#include "wx/dynarray.h"

#include <vector>

class wxRibbonBar;
class wxRibbonArtProvider;

class WXDLLIMPEXP_RIBBON wxRibbonControl : public wxControl
{
public:
    wxRibbonControl() { Init(); }

    wxRibbonControl(wxWindow *parent, wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxASCII_STR(wxControlNameStr))
    {
        Init();

        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxControlNameStr));

    virtual void SetArtProvider(wxRibbonArtProvider* art);
    wxRibbonArtProvider* GetArtProvider() const {return m_art;}

    virtual bool IsSizingContinuous() const {return true;}
    wxSize GetNextSmallerSize(wxOrientation direction, wxSize relative_to) const;
    wxSize GetNextLargerSize(wxOrientation direction, wxSize relative_to) const;
    wxSize GetNextSmallerSize(wxOrientation direction) const;
    wxSize GetNextLargerSize(wxOrientation direction) const;

    virtual bool Realize();
    bool Realise() {return Realize();}

    virtual wxRibbonBar* GetAncestorRibbonBar()const;

    // Leaves the ancestor bar's keytip mode, if it is in it.
    void DismissKeyTips();

    // Finds the best width and height given the parent's width and height
    virtual wxSize GetBestSizeForParentSize(const wxSize& WXUNUSED(parentSize)) const { return GetBestSize(); }

    // Implementation only: keyboard navigation of the items inside a control.
    //
    // The keyboard focus always stays on wxRibbonBar, which forwards the keys
    // to these functions. Note that this is not about the control itself
    // getting the focus (see wxWindow::CanAcceptFocus()). A control with
    // items which can be selected with the keyboard returns true from
    // HasFocusableItems() and overrides the rest.
    virtual bool HasFocusableItems() const { return false; }
    // Return false, changing nothing, if there is no such item.
    virtual bool FocusFirstItem() { return false; }
    virtual bool FocusLastItem() { return false; }
    virtual bool FocusNextItem(bool WXUNUSED(forward)) { return false; }
    // Moves to the item above or below (wxUP or wxDOWN), for controls with
    // items arranged in rows.
    virtual bool FocusItemInDirection(wxDirection WXUNUSED(direction)) { return false; }
    virtual void ClearFocusedItem() { }
    // Fires the focused item's action (or its dropdown, if there is one).
    virtual void ActivateFocusedItem(bool WXUNUSED(dropdown) = false) { }

    // Move through 'controls' (the ones with focusable items).
    // Return the control reached, or null if there is nowhere to go.
    static wxRibbonControl* FocusFirstItemIn(
        const std::vector<wxRibbonControl*>& controls, bool forward);
    static wxRibbonControl* FocusNextItemIn(
        const std::vector<wxRibbonControl*>& controls,
        wxRibbonControl* current, bool forward);

protected:
    wxRibbonArtProvider* m_art;

    virtual wxSize DoGetNextSmallerSize(wxOrientation direction,
                                        wxSize relative_to) const;
    virtual wxSize DoGetNextLargerSize(wxOrientation direction,
                                       wxSize relative_to) const;

private:
    void Init() { m_art = nullptr; }

#ifndef SWIG
    wxDECLARE_CLASS(wxRibbonControl);
#endif
};

WX_DEFINE_USER_EXPORTED_ARRAY_PTR(wxRibbonControl*, wxArrayRibbonControl, class WXDLLIMPEXP_RIBBON);

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_CONTROL_H_
