///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/checklst.h
// Purpose:     wxCheckListBox class - a listbox with checkable items
// Author:      Vadim Zeitlin
// Modified by:
// Created:     16.11.97
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   __CHECKLST__H_
#define   __CHECKLST__H_

#if !wxUSE_OWNER_DRAWN
  #error  "wxCheckListBox class requires owner-drawn functionality."
#endif

class WXDLLIMPEXP_FWD_CORE wxOwnerDrawn;
class WXDLLIMPEXP_FWD_CORE wxCheckListBoxItem; // fwd decl, defined in checklst.cpp

class WXDLLIMPEXP_CORE wxCheckListBox : public wxCheckListBoxBase
{
public:
    // ctors
    wxCheckListBox();
    wxCheckListBox(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   int nStrings = 0,
                   const wxString choices[] = nullptr,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxListBoxNameStr));
    wxCheckListBox(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   const wxArrayString& choices,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxListBoxNameStr));

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));

    // items may be checked
    virtual bool IsChecked(unsigned int uiIndex) const override;
    virtual void Check(unsigned int uiIndex, bool bCheck = true) override;
    virtual void Toggle(unsigned int uiIndex);

    // we create our items ourselves and they have non-standard size,
    // so we need to override these functions
    virtual wxOwnerDrawn *CreateLboxItem(size_t n) override;
    virtual bool MSWOnMeasure(WXMEASUREITEMSTRUCT *item) override;

protected:
    virtual wxSize MSWGetFullItemSize(int w, int h) const override;

    // pressing space or clicking the check box toggles the item
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftClick(wxMouseEvent& event);

    // send an "item checked" event
    void SendEvent(unsigned int uiIndex)
    {
        wxCommandEvent event(wxEVT_CHECKLISTBOX, GetId());
        event.SetInt(uiIndex);
        event.SetEventObject(this);
        event.SetString(GetString(uiIndex));
        ProcessCommand(event);
    }

    virtual void MSWUpdateFontOnDPIChange(const wxSize& newDPI) override;

    wxSize DoGetBestClientSize() const override;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCheckListBox);
};

#endif    //_CHECKLST_H
