/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:     wxChoice class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICE_H_
#define _WX_CHOICE_H_

#ifdef __GNUG__
#pragma interface "choice.h"
#endif

#include "wx/ctrlsub.h"

WXDLLEXPORT_DATA(extern const char*) wxChoiceNameStr;

// Choice item
class WXDLLEXPORT wxChoice: public wxControlWithItems
{
    DECLARE_DYNAMIC_CLASS(wxChoice)

public:
    wxChoice();
    ~wxChoice();

    wxChoice(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr)
    {
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr);

    // Virtual functions required by wxControlWithItems.
    // They are not all implemented yet :-(
    virtual int GetCount() const;
    virtual int DoAppend(const wxString& item);
    virtual void DoInsertItems(const wxArrayString& items, int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);
    virtual void DoSetFirstItem(int n);
    virtual void DoSetItemClientData(int n, void* clientData);
    virtual void* DoGetItemClientData(int n) const;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(int n) const;
    virtual void Select(int n);
    virtual void SetString(int n, const wxString& s);

    // Original API
    virtual void Append(const wxString& item);
    virtual void Delete(int n);
    virtual void Clear();
    virtual int GetSelection() const ;
    virtual void SetSelection(int n);
    virtual int FindString(const wxString& s) const;
    virtual wxString GetString(int n) const ;

    virtual wxString GetStringSelection() const ;
    virtual bool SetStringSelection(const wxString& sel);

    virtual int Number() const { return m_noStrings; }
    virtual void Command(wxCommandEvent& event);

    virtual void SetColumns(int n = 1 );
    virtual int GetColumns() const ;

    void SetFocus();

    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    WXWidget GetTopWidget() const { return m_formWidget; }
    WXWidget GetMainWidget() const { return m_buttonWidget; }

protected:
    int           m_noStrings;
    WXWidget      m_menuWidget;
    WXWidget      m_buttonWidget;
    WXWidget*     m_widgetList ;
    WXWidget      m_formWidget;
    wxStringList  m_stringList;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
};

#endif
	// _WX_CHOICE_H_
