///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/checkbox.h
// Purpose:     wxCheckBox declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_CHECKBOX_H_
#define _WX_UNIV_CHECKBOX_H_

#ifdef __GNUG__
    #pragma interface "univcheckbox.h"
#endif

// ----------------------------------------------------------------------------
// the actions supported by wxCheckBox
// ----------------------------------------------------------------------------

#define wxACTION_CHECKBOX_CHECK   _T("check")   // SetValue(TRUE)
#define wxACTION_CHECKBOX_CLEAR   _T("clear")   // SetValue(FALSE)
#define wxACTION_CHECKBOX_TOGGLE  _T("toggle")  // toggle the check state

// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCheckBox : public wxCheckBoxBase
{
public:
    // checkbox constants
    enum State
    {
        State_Normal,
        State_Pressed,
        State_Disabled,
        State_Current,
        State_Max
    };

    enum Status
    {
        Status_Checked,
        Status_Unchecked,
        Status_Unknown,
        Status_Max
    };

    // constructors
    wxCheckBox() { Init(); }

    wxCheckBox(wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxCheckBoxNameStr)
    {
        Init();

        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr);

    // implement the checkbox interface
    virtual void SetValue(bool value);
    virtual bool GetValue() const;

    // set/get the margin between the checkbox bitmap and the label
    void SetMargin(wxCoord margin) { m_checkMargin = margin; }
    wxCoord GetMargin() const { return m_checkMargin; }

    // set/get the bitmaps to use for the checkbox indicator
    void SetBitmap(const wxBitmap& bmp, State state, Status status);
    wxBitmap GetBitmap(State state, Status status) const;

protected:
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1,
                               const wxString& strArg = wxEmptyString);
    virtual void DoDraw(wxControlRenderer *renderer);
    virtual wxSize DoGetBestClientSize() const;

    // common part of all ctors
    void Init();

private:
    // the current check status
    Status m_status;

    // the bitmaps to use for the different states
    wxBitmap m_bitmaps[State_Max][Status_Max];

    // the distance between the checkbox and the label (-1 means default)
    wxCoord m_checkMargin;

    DECLARE_DYNAMIC_CLASS(wxCheckBox)
};

#endif // _WX_UNIV_CHECKBOX_H_
