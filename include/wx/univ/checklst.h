///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/checklst.h
// Purpose:     wxCheckListBox class for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.09.00
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_CHECKLST_H_
#define _WX_UNIV_CHECKLST_H_

#ifdef __GNUG__
    #pragma interface "univchecklst.h"
#endif

// ----------------------------------------------------------------------------
// actions
// ----------------------------------------------------------------------------

#define wxACTION_CHECKLISTBOX_TOGGLE _T("toggle")

// ----------------------------------------------------------------------------
// wxCheckListBox
// ----------------------------------------------------------------------------

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

    // no Create(): the base class does everything

    // implement check list box methods
    virtual bool IsChecked(size_t item) const;
    virtual void Check(size_t item, bool check = TRUE);

    // and input handling
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1l,
                               const wxString& strArg = wxEmptyString);

    // override all methods which add/delete items to update m_checks array as
    // well
    virtual void Delete(int n);

protected:
    virtual int DoAppend(const wxString& item);
    virtual void DoInsertItems(const wxArrayString& items, int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);
    virtual void DoClear();

    // draw the check items instead of the usual ones
    virtual void DoDrawRange(wxControlRenderer *renderer,
                             int itemFirst, int itemLast);

    // take them also into account for size calculation
    virtual wxSize DoGetBestClientSize() const;

    // our input handler
    virtual wxString GetInputHandlerType() const;

    // common part of all ctors
    void Init();

private:
    // the array containing the checked status of the items
    wxArrayInt m_checks;

    DECLARE_DYNAMIC_CLASS(wxCheckListBox)
};

#endif // _WX_UNIV_CHECKLST_H_

