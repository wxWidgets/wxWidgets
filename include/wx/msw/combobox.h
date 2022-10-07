/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/combobox.h
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#include "wx/choice.h"
#include "wx/textentry.h"

#if wxUSE_COMBOBOX

// ----------------------------------------------------------------------------
// Combobox control
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxComboBox : public wxChoice,
                                    public wxTextEntry
{
public:
    wxComboBox() { Init(); }

    wxComboBox(wxWindow *parent, wxWindowID id,
            const wxString& value = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxComboBoxNameStr))
    {
        Init();
        Create(parent, id, value, pos, size, n, choices, style, validator, name);

    }

    wxComboBox(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxComboBoxNameStr))
    {
        Init();

        Create(parent, id, value, pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    // See wxComboBoxBase discussion of IsEmpty().
    bool IsListEmpty() const { return wxItemContainer::IsEmpty(); }
    bool IsTextEmpty() const { return wxTextEntry::IsEmpty(); }

    // resolve ambiguities among virtual functions inherited from both base
    // classes
    virtual void Clear() wxOVERRIDE;
    virtual wxString GetValue() const wxOVERRIDE;
    virtual void SetValue(const wxString& value) wxOVERRIDE;
    virtual wxString GetStringSelection() const wxOVERRIDE
        { return wxChoice::GetStringSelection(); }
    virtual void Popup() { MSWDoPopupOrDismiss(true); }
    virtual void Dismiss() { MSWDoPopupOrDismiss(false); }
    virtual void SetSelection(int n) wxOVERRIDE { wxChoice::SetSelection(n); }
    virtual void SetSelection(long from, long to) wxOVERRIDE
        { wxTextEntry::SetSelection(from, to); }
    virtual int GetSelection() const wxOVERRIDE { return wxChoice::GetSelection(); }
    virtual bool ContainsHWND(WXHWND hWnd) const wxOVERRIDE;
    virtual void GetSelection(long *from, long *to) const wxOVERRIDE;

    virtual bool IsEditable() const wxOVERRIDE;

    // implementation only from now on
    virtual bool MSWCommand(WXUINT param, WXWORD id) wxOVERRIDE;
    bool MSWProcessEditMsg(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) wxOVERRIDE;
    bool MSWShouldPreProcessMessage(WXMSG *pMsg) wxOVERRIDE;

    // Standard event handling
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);

    void OnUpdateCut(wxUpdateUIEvent& event);
    void OnUpdateCopy(wxUpdateUIEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUpdateRedo(wxUpdateUIEvent& event);
    void OnUpdateDelete(wxUpdateUIEvent& event);
    void OnUpdateSelectAll(wxUpdateUIEvent& event);

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const wxOVERRIDE;

#if wxUSE_UXTHEME
    // override wxTextEntry method to work around Windows bug
    virtual bool SetHint(const wxString& hint) wxOVERRIDE;
#endif // wxUSE_UXTHEME

    virtual void SetLayoutDirection(wxLayoutDirection dir) wxOVERRIDE;

    virtual const wxTextEntry* WXGetTextEntry() const wxOVERRIDE { return this; }

protected:
#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip(wxToolTip *tip) wxOVERRIDE;
#endif

    virtual wxSize DoGetSizeFromTextSize(int xlen, int ylen = -1) const wxOVERRIDE;

    // Override this one to avoid eating events from our popup listbox.
    virtual wxWindow *MSWFindItem(long id, WXHWND hWnd) const wxOVERRIDE;

    // this is the implementation of GetEditHWND() which can also be used when
    // we don't have the edit control, it simply returns NULL then
    //
    // try not to use this function unless absolutely necessary (as in the
    // message handling code where the edit control might not be created yet
    // for the messages we receive during the control creation) as normally
    // just testing for IsEditable() and using GetEditHWND() should be enough
    WXHWND GetEditHWNDIfAvailable() const;

    virtual void EnableTextChangedEvents(bool enable) wxOVERRIDE
    {
        m_allowTextEvents = enable;
    }

    // Recreate the native control entirely while preserving its initial
    // contents and attributes: this is useful if the height of the items must
    // be changed as the native control doesn't seem to support doing this once
    // it had been already determined.
    void MSWRecreate();

private:
    // there are the overridden wxTextEntry methods which should only be called
    // when we do have an edit control so they assert if this is not the case
    virtual wxWindow *GetEditableWindow() wxOVERRIDE;
    virtual WXHWND GetEditHWND() const wxOVERRIDE;

    // Common part of MSWProcessEditMsg() and MSWProcessSpecialKey(), return
    // true if the key was processed.
    bool MSWProcessEditSpecialKey(WXWPARAM vkey);

#if wxUSE_OLE
    virtual void MSWProcessSpecialKey(wxKeyEvent& event) wxOVERRIDE;
#endif // wxUSE_OLE

    // common part of all ctors
    void Init()
    {
        m_allowTextEvents = true;
    }

    // normally true, false if text events are currently disabled
    bool m_allowTextEvents;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxComboBox);
    wxDECLARE_EVENT_TABLE();
};

#endif // wxUSE_COMBOBOX

#endif // _WX_COMBOBOX_H_
