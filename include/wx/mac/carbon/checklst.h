///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/checklst.h
// Purpose:     wxCheckListBox class - a listbox with checkable items
//              Note: this is an optional class.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_CHECKLST_H_
#define _WX_MAC_CHECKLST_H_

class wxMacCheckListControl
{
public :
    virtual bool            MacIsChecked(unsigned int n) const = 0;
    virtual void            MacCheck(unsigned int n, bool bCheck = true) = 0;

    virtual ~wxMacCheckListControl() { }
};

class WXDLLEXPORT wxCheckListBox : public wxCheckListBoxBase
{
public:
    // ctors
    wxCheckListBox() { Init(); }
    wxCheckListBox(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   int nStrings = 0,
                   const wxString *choices = NULL,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxListBoxNameStr)
    {
        Init();

        Create(parent, id, pos, size, nStrings, choices, style, validator, name);
    }
    wxCheckListBox(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   const wxArrayString& choices,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxListBoxNameStr)
    {
        Init();

        Create(parent, id, pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int nStrings = 0,
                const wxString *choices = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxListBoxNameStr);
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxListBoxNameStr);

    // items may be checked
    bool  IsChecked(unsigned int uiIndex) const;
    void  Check(unsigned int uiIndex, bool bCheck = true);

    wxMacCheckListControl* GetPeer() const;

protected:
    void Init();

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxCheckListBox)
};

#endif // _WX_MAC_CHECKLST_H_
