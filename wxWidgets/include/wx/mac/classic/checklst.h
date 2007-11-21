///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/classic/checklst.h
// Purpose:     wxCheckListBox class - a listbox with checkable items
//              Note: this is an optional class.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKLST_H_
#define _WX_CHECKLST_H_

class WXDLLEXPORT wxCheckListBox : public wxCheckListBoxBase
{
  DECLARE_DYNAMIC_CLASS(wxCheckListBox)
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
  void OnChar(wxKeyEvent& event) ;
  void OnLeftClick(wxMouseEvent& event) ;

    // metrics
    wxInt32 m_checkBoxWidth;
    wxInt32 m_checkBoxHeight;
    wxInt32    m_TextBaseLineOffset;

    // the array containing the checked status of the items
    wxArrayInt m_checks;

    // override all methods which add/delete items to update m_checks array as
    // well
    virtual void Delete(unsigned int n);
protected:
    virtual int DoAppend(const wxString& item);
    virtual void DoInsertItems(const wxArrayString& items, unsigned int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);
    virtual void DoClear();
    // common part of all ctors
    void Init();
private:
  DECLARE_EVENT_TABLE()
};

#endif
    // _WX_CHECKLST_H_
