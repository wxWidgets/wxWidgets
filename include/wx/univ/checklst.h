///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/checklst.h
// Purpose:     wxCheckListBox class for wxUniversal
// Author:      Vadim Zeitlin
// Created:     12.09.00
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_CHECKLST_H_
#define _WX_UNIV_CHECKLST_H_

// ----------------------------------------------------------------------------
// actions
// ----------------------------------------------------------------------------

#define wxACTION_CHECKLISTBOX_TOGGLE wxT("toggle")

// ----------------------------------------------------------------------------
// wxCheckListBox
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCheckListBox : public wxCheckListBoxBase
{
public:
    // ctors
    wxCheckListBox() { Init(); }

    wxCheckListBox(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   int nStrings = 0,
                   const wxString choices[] = nullptr,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxListBoxNameStr))
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
                   const wxString& name = wxASCII_STR(wxListBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int nStrings = 0,
                const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));

    // implement check list box methods
    virtual bool IsChecked(unsigned int item) const override;
    virtual void Check(unsigned int item, bool check = true) override;

    // and input handling
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1l,
                               const wxString& strArg = wxEmptyString) override;

    static wxInputHandler *GetStdInputHandler(wxInputHandler *handlerDef);
    virtual wxInputHandler *DoGetStdInputHandler(wxInputHandler *handlerDef) override
    {
        return GetStdInputHandler(handlerDef);
    }

protected:
    // override all methods which add/delete items to update m_checks array as
    // well
    virtual void OnItemInserted(unsigned int pos) override;
    virtual void DoDeleteOneItem(unsigned int n) override;
    virtual void DoClear() override;

    // draw the check items instead of the usual ones
    virtual void DoDrawRange(wxControlRenderer *renderer,
                             int itemFirst, int itemLast) override;

    // take them also into account for size calculation
    virtual wxSize DoGetBestClientSize() const override;

    // common part of all ctors
    void Init();

private:
    // the array containing the checked status of the items
    wxArrayInt m_checks;

    wxDECLARE_DYNAMIC_CLASS(wxCheckListBox);
};

#endif // _WX_UNIV_CHECKLST_H_
