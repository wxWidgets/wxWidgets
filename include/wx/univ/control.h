/////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/control.h
// Purpose:     universal wxControl: adds handling of mnemonics
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_CONTROL_H_
#define _WX_UNIV_CONTROL_H_

#ifdef __GNUG__
    #pragma interface "control.h"
#endif

class WXDLLEXPORT wxControlRenderer;
class WXDLLEXPORT wxInputHandler;

// ----------------------------------------------------------------------------
// wxControlAction: the action is currently just a string which identifies it,
// later it might become an atom (i.e. an opaque handler to string). As one
// input event may result in several control actions (e.g. a macro expansion
// in the text control) we define an array of actions as well.
// ----------------------------------------------------------------------------

typedef wxString wxControlAction;
class WXDLLEXPORT wxControlActions : public wxArrayString
{
public:
    wxControlActions() { }
    wxControlActions(const wxControlAction& action)
        { wxArrayString::Add(action); }
    wxControlActions(const wxChar *action)
        { wxArrayString::Add(action); }

    wxControlActions& Add(const wxControlActions& other)
    {
        size_t count = other.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            wxArrayString::Add(other[n]);
        }

        return *this;
    }
};

// the list of actions which apply to all controls (other actions are defined
// in the controls headers)

#define wxACTION_NONE        _T("")           // no action to perform
#define wxACTION_HIGHLIGHT   _T("focus")      // highlight the control
#define wxACTION_UNHIGHLIGHT _T("unfocus")    // remove highlight

// ----------------------------------------------------------------------------
// wxControl: the base class for all GUI controls
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControl : public wxControlBase
{
public:
    wxControl() { Init(); }

    wxControl(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxControlNameStr)
    {
        Init();

        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxControlNameStr);

    // this function will filter out '&' characters and will put the
    // accelerator char (the one immediately after '&') into m_chAccel
    virtual void SetLabel(const wxString &label);
    virtual wxString GetLabel() const;

    // get the state information
    virtual bool IsFocused() const;
    virtual bool IsCurrent() const;
    virtual bool IsPressed() const;
    virtual bool IsDefault() const;

    // operations
    void SetCurrent(bool doit = TRUE);

    // implementation only from now on

    // return the index of the accel char in the label or -1 if none
    int GetAccelIndex() const { return m_indexAccel; }

    // return the accel char itself or 0 if none
    wxChar GetAccelChar() const
    {
        return m_indexAccel == -1 ? _T('\0') : m_label[m_indexAccel];
    }

protected:
    // create the event translator object for this control: the base class
    // action creates the default one which doesn't do anything
    virtual wxInputHandler *CreateInputHandler() const;

    // draw the controls contents
    virtual void DoDraw(wxControlRenderer *renderer);

    // perform the action which resulted from the translation of the event
    // (the exact event type depends on the action), return TRUE if the
    // control must be updated
    virtual bool PerformAction(const wxControlAction& action,
                               const wxEvent& event);

    // event handlers
    void OnMouse(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnFocus(wxFocusEvent& event);
    void OnPaint(wxPaintEvent& event);

private:
    // common part of all ctors
    void Init();

    // common part of OnMouse/OnKeyDown/Up
    void PerformActions(const wxControlActions& actions, const wxEvent& event);

    // input processor
    wxInputHandler *m_handler;

    // label with accel into
    wxString   m_label;
    int        m_indexAccel;

    // state
    bool m_isCurrent;

    DECLARE_DYNAMIC_CLASS(wxControl)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_UNIV_CONTROL_H_
