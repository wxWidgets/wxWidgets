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

class WXDLLEXPORT wxControl : public wxControlBase
{
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

    // this function will filter out '&' characters and will put the
    // accelerator char (the one immediately after '&') into m_chAccel
    virtual void SetLabel(const wxString &label);
    virtual wxString GetLabel() const;

    // get the state information
    virtual bool IsFocused() const;
    virtual bool IsPressed() const;
    virtual bool IsDefault() const;

    // implementation only from now on

    // return the index of the accel char in the label or -1 if none
    int GetAccelIndex() const { return m_indexAccel; }

    // return the accel char itself or 0 if none
    wxChar GetAccelChar() const
    {
        return m_indexAccel == -1 ? _T('\0') : m_label[m_indexAccel];
    }

protected:
    // draw the controls contents
    virtual void DoDraw(wxControlRenderer *renderer);

    // event handlers
    void OnPaint(wxPaintEvent& event);

private:
    wxString   m_label;
    int        m_indexAccel;

    DECLARE_DYNAMIC_CLASS(wxControl)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_UNIV_CONTROL_H_
