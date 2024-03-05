/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/srchctlg.h
// Purpose:     generic wxSearchCtrl class
// Author:      Vince Harron
// Created:     2006-02-19
// Copyright:   Vince Harron
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_SEARCHCTRL_H_
#define _WX_GENERIC_SEARCHCTRL_H_

#if wxUSE_SEARCHCTRL

#include "wx/bitmap.h"

class WXDLLIMPEXP_FWD_CORE wxSearchButton;
class WXDLLIMPEXP_FWD_CORE wxSearchTextCtrl;

// ----------------------------------------------------------------------------
// wxSearchCtrl is a combination of wxTextCtrl and wxSearchButton
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSearchCtrl : public wxSearchCtrlBase
{
public:
    // creation
    // --------

    wxSearchCtrl();
    wxSearchCtrl(wxWindow *parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxSearchCtrlNameStr));

    virtual ~wxSearchCtrl();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxSearchCtrlNameStr));

#if wxUSE_MENUS
    // get/set search button menu
    // --------------------------
    virtual void SetMenu( wxMenu* menu ) override;
    virtual wxMenu* GetMenu() override;
#endif // wxUSE_MENUS

    // get/set search options
    // ----------------------
    virtual void ShowSearchButton( bool show ) override;
    virtual bool IsSearchButtonVisible() const override;

    virtual void ShowCancelButton( bool show ) override;
    virtual bool IsCancelButtonVisible() const override;

    virtual void SetDescriptiveText(const wxString& text) override;
    virtual wxString GetDescriptiveText() const override;

    // accessors
    // ---------

    virtual wxString GetRange(long from, long to) const override;

    virtual bool IsEditable() const override;

    // If the return values from and to are the same, there is no selection.
    virtual void GetSelection(long* from, long* to) const override;

    virtual wxString GetStringSelection() const override;

    // operations
    // ----------

    virtual void ChangeValue(const wxString& value) override;

    // editing
    virtual void Clear() override;
    virtual void Replace(long from, long to, const wxString& value) override;
    virtual void Remove(long from, long to) override;

    // set the max number of characters which may be entered in a single line
    // text control
    virtual void SetMaxLength(unsigned long WXUNUSED(len)) override;

    // writing text inserts it at the current position, appending always
    // inserts it at the end
    virtual void WriteText(const wxString& text) override;
    virtual void AppendText(const wxString& text) override;

    // insert the character which would have resulted from this key event,
    // return true if anything has been inserted
    virtual bool EmulateKeyPress(const wxKeyEvent& event);

    // Clipboard operations
    virtual void Copy() override;
    virtual void Cut() override;
    virtual void Paste() override;

    virtual bool CanCopy() const override;
    virtual bool CanCut() const override;
    virtual bool CanPaste() const override;

    // Undo/redo
    virtual void Undo() override;
    virtual void Redo() override;

    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;

    // Insertion point
    virtual void SetInsertionPoint(long pos) override;
    virtual void SetInsertionPointEnd() override;
    virtual long GetInsertionPoint() const override;
    virtual wxTextPos GetLastPosition() const override;

    virtual void SetSelection(long from, long to) override;
    virtual void SelectAll() override;
    virtual void SetEditable(bool editable) override;

    // Autocomplete
    virtual bool DoAutoCompleteStrings(const wxArrayString &choices) override;
    virtual bool DoAutoCompleteFileNames(int flags) override;
    virtual bool DoAutoCompleteCustom(wxTextCompleter *completer) override;

    virtual bool ShouldInheritColours() const override;

    // wxWindow overrides
    virtual bool SetFont(const wxFont& font) override;
    virtual bool SetBackgroundColour(const wxColour& colour) override;

    // search control generic only
    void SetSearchBitmap( const wxBitmap& bitmap );
    void SetCancelBitmap( const wxBitmap& bitmap );
#if wxUSE_MENUS
    void SetSearchMenuBitmap( const wxBitmap& bitmap );
#endif // wxUSE_MENUS

protected:
    virtual void DoSetValue(const wxString& value, int flags) override;
    virtual wxString DoGetValue() const override;

    // override the base class virtuals involved into geometry calculations
    virtual wxSize DoGetBestClientSize() const override;

    virtual void RecalcBitmaps();

    void Init();

    virtual wxBitmap RenderSearchBitmap( int x, int y, bool renderDrop );
    virtual wxBitmap RenderCancelBitmap( int x, int y );

    void OnCancelButton( wxCommandEvent& event );

    void OnSize( wxSizeEvent& event );

    void OnDPIChanged(wxDPIChangedEvent& event);

    bool HasMenu() const
    {
#if wxUSE_MENUS
        return m_menu != nullptr;
#else // !wxUSE_MENUS
        return false;
#endif // wxUSE_MENUS/!wxUSE_MENUS
    }

private:
    friend class wxSearchButton;

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    // Implement wxMSW-specific pure virtual function by forwarding it to the
    // real text entry.
    virtual WXHWND GetEditHWND() const override;
#endif

    // Implement pure virtual function inherited from wxCompositeWindow.
    virtual wxWindowList GetCompositeWindowParts() const override;

    // Position the child controls using the current window size.
    void LayoutControls();

#if wxUSE_MENUS
    void PopupSearchMenu();
#endif // wxUSE_MENUS

    // the subcontrols
    wxSearchTextCtrl *m_text;
    wxSearchButton *m_searchButton;
    wxSearchButton *m_cancelButton;
#if wxUSE_MENUS
    wxMenu *m_menu;
#endif // wxUSE_MENUS

    bool m_searchBitmapUser;
    bool m_cancelBitmapUser;
#if wxUSE_MENUS
    bool m_searchMenuBitmapUser;
#endif // wxUSE_MENUS

    wxBitmap m_searchBitmap;
    wxBitmap m_cancelBitmap;
#if wxUSE_MENUS
    wxBitmap m_searchMenuBitmap;
#endif // wxUSE_MENUS

private:
    wxDECLARE_DYNAMIC_CLASS(wxSearchCtrl);

    wxDECLARE_EVENT_TABLE();
};

#endif // wxUSE_SEARCHCTRL

#endif // _WX_GENERIC_SEARCHCTRL_H_

