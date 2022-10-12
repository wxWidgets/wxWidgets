/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/anybutton.h
// Purpose:     wxGTK wxAnyButton class declaration
// Author:      Robert Roebling
// Created:     1998-05-20 (extracted from button.h)
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_ANYBUTTON_H_
#define _WX_GTK_ANYBUTTON_H_

//-----------------------------------------------------------------------------
// wxAnyButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnyButton : public wxAnyButtonBase
{
public:
    wxAnyButton()
    {
        m_isCurrent =
        m_isPressed = false;
    }

    // implementation
    // --------------

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // called from GTK callbacks: they update the button state and call
    // GTKUpdateBitmap()
    void GTKMouseEnters();
    void GTKMouseLeaves();
    void GTKPressed();
    void GTKReleased();

protected:
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

    virtual void DoEnable(bool enable) override;

    virtual wxBitmap DoGetBitmap(State which) const override;
    virtual void DoSetBitmap(const wxBitmapBundle& bitmap, State which) override;
    virtual void DoSetBitmapPosition(wxDirection dir) override;

    // update the bitmap to correspond to the current button state
    void GTKUpdateBitmap();

private:
    typedef wxAnyButtonBase base_type;

    // focus event handler: calls GTKUpdateBitmap()
    void GTKOnFocus(wxFocusEvent& event);

    // return the state whose bitmap is being currently shown (so this is
    // different from the real current state, e.g. it could be State_Normal
    // even if the button is pressed if no button was set for State_Pressed)
    State GTKGetCurrentBitmapState() const;

    // show the given bitmap (must be valid)
    void GTKDoShowBitmap(const wxBitmapBundle& bitmap);


    // the bitmaps for the different state of the buttons, all of them may be
    // invalid and the button only shows a bitmap at all if State_Normal bitmap
    // is valid
    wxBitmapBundle m_bitmaps[State_Max];

    // true iff mouse is currently over the button
    bool m_isCurrent;

    // true iff the button is in pressed state
    bool m_isPressed;

    wxDECLARE_NO_COPY_CLASS(wxAnyButton);
};

#endif // _WX_GTK_ANYBUTTON_H_
