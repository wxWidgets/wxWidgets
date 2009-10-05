///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/infobar.h
// Purpose:     generic wxInfoBar class declaration
// Author:      Vadim Zeitlin
// Created:     2009-07-28
// RCS-ID:      $Id: wxhead.h,v 1.11 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_INFOBAR_H_
#define _WX_GENERIC_INFOBAR_H_

class WXDLLIMPEXP_FWD_CORE wxBitmapButton;
class WXDLLIMPEXP_FWD_CORE wxStaticBitmap;
class WXDLLIMPEXP_FWD_CORE wxStaticText;

// ----------------------------------------------------------------------------
// wxInfoBar
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxInfoBar : public wxInfoBarBase
{
public:
    // the usual ctors and Create() but remember that info bar is created
    // hidden
    wxInfoBar() { Init(); }

    wxInfoBar(wxWindow *parent, wxWindowID winid = wxID_ANY)
    {
        Init();
        Create(parent, winid);
    }

    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY);


    // implement base class methods
    // ----------------------------

    virtual void ShowMessage(const wxString& msg, int flags = wxICON_NONE);

    virtual void AddButton(wxWindowID btnid, const wxString& label = wxString());


    // methods specific to this version
    // --------------------------------

    // set the effect(s) to use when showing/hiding the bar, may be
    // wxSHOW_EFFECT_NONE to disable any effects entirely
    //
    // by default, slide to bottom/top is used when it's positioned on the top
    // of the window for showing/hiding it and top/bottom when it's positioned
    // at the bottom
    void SetShowHideEffects(wxShowEffect showEffect, wxShowEffect hideEffect)
    {
        m_showEffect = showEffect;
        m_hideEffect = hideEffect;
    }

    // get effect used when showing/hiding the window
    wxShowEffect GetShowEffect() const { return m_showEffect; }
    wxShowEffect GetHideEffect() const { return m_hideEffect; }

    // set the duration of animation used when showing/hiding the bar, in ms
    void SetEffectDuration(int duration) { m_effectDuration = duration; }

    // get the currently used effect animation duration
    int GetEffectDuration() const { return m_effectDuration; }


    // overridden base class methods
    // -----------------------------

    // setting the font of this window sets it for the text control inside it
    // (default font is a larger and bold version of the normal one)
    virtual bool SetFont(const wxFont& font);

private:
    // common part of all ctors
    void Init();

    // handler for the close button
    void OnButton(wxCommandEvent& event);

    // update the parent after we're shown or hidden
    void UpdateParent();

    // change the parent background colour to match that of our sibling
    void ChangeParentBackground();

    // restore the parent background changed by the above function
    void RestoreParentBackground();

    // show/hide the bar
    void DoShow();
    void DoHide();


    // different controls making up the bar
    wxStaticBitmap *m_icon;
    wxStaticText *m_text;
    wxBitmapButton *m_button;

    // the effects to use when showing/hiding and duration for them
    wxShowEffect m_showEffect,
                 m_hideEffect;
    int m_effectDuration;

    // the original parent background colour, before we changed it
    wxColour m_origParentBgCol;

    wxDECLARE_NO_COPY_CLASS(wxInfoBar);
};

#endif // _WX_GENERIC_INFOBAR_H_

