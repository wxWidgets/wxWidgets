///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/spinbutt.h
// Purpose:     universal version of wxSpinButton
// Author:      Vadim Zeitlin
// Modified by:
// Created:     21.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_SPINBUTT_H_
#define _WX_UNIV_SPINBUTT_H_

#ifdef __GNUG__
    #pragma interface "univspinbutt.h"
#endif

// ----------------------------------------------------------------------------
// wxSpinButton
// ----------------------------------------------------------------------------

// actions supported by this control
#define wxACTION_SPIN_INC    _T("inc")
#define wxACTION_SPIN_DEC    _T("dec")

class WXDLLEXPORT wxSpinButton : public wxSpinButtonBase
{
public:
    wxSpinButton() { Init(); }
    wxSpinButton(wxWindow *parent,
                 wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                 const wxString& name = )
    {
        Init();

        (void)Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                const wxString& name = wxSPIN_BUTTON_NAME);

    // implement wxSpinButtonBase methods
    virtual int GetValue() const;
    virtual void SetValue(int val);

    // implementation only from now on
    enum
    {
        Arrow_First,        // left or top
        Arrow_Second,       // right or bottom
        Arrow_Max
    } Arrow;

    int GetArrowState(Arrow arrow) const;
    void SetArrowSatte(Arrow arrow, int state);

protected:
    virtual wxSize DoGetBestClientSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);
    virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

    virtual wxString GetInputHandlerType() const;
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = 0,
                               const wxString& strArg = wxEmptyString);

    // normalize the value to fit into min..max range
    int NormalizeValue(int value) const;

    // change the value by +1/-1 and send the event, return TRUE if value was
    // changed
    bool ChangeValue(int inc);

    // DoDraw() helper
    void DrawArrow(Arrow arrow, wxDC& dc, const wxRect& rect);

    // the common part of all ctors
    void Init();

    // the current controls value
    int m_value;

private:
    // the state (combination of wxCONTROL_XXX flags) of the arrows
    int m_arrowsState[Arrow_Max];
};

// ----------------------------------------------------------------------------
// wxStdSpinButtonInputHandler: manages clicks on them ouse arrow like
// wxStdScrollBarInputHandler and processes keyboard events too
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdSpinButtonInputHandler : public wxStdInputHandler
{
public:
    wxStdSpinButtonInputHandler(wxInputHandler *inphand);

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxControl *control, const wxMouseEvent& event);

    virtual ~wxStdSpinButtonInputHandler();

    // this method is called by wxScrollBarTimer only and may be overridden
    //
    // return TRUE to continue scrolling, FALSE to stop the timer
    virtual bool OnScrollTimer(wxScrollBar *scrollbar,
                               const wxControlAction& action);
};


#endif // _WX_UNIV_SPINBUTT_H_

