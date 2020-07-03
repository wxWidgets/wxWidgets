/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/srchctrl.h
// Purpose:     wxSearchCtrl class
// Author:      Kettab Ali
// Created:     2019-12-23
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_SEARCHCTRL_H_
#define _WX_MSW_SEARCHCTRL_H_

#if wxUSE_SEARCHCTRL

// ----------------------------------------------------------------------------
// wxSearchCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSearchCtrl : public wxSearchCtrlBase
{
public:
    // creation
    // --------

    wxSearchCtrl() : wxSearchCtrlBase()
    {
        Init();
    }

    wxSearchCtrl(wxWindow *parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxSearchCtrlNameStr)
        : wxSearchCtrlBase()
    {
        Init();
        Create(parent, id, value, pos, size, style, validator, name);
    }

    virtual ~wxSearchCtrl();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxSearchCtrlNameStr);

#if wxUSE_MENUS
    // get/set search button menu
    // --------------------------
    virtual void SetMenu( wxMenu* menu ) wxOVERRIDE;
    virtual wxMenu* GetMenu() wxOVERRIDE { return m_menu; }
#endif // wxUSE_MENUS

    // get/set search options
    // ----------------------
    virtual void ShowSearchButton( bool show ) wxOVERRIDE;
    virtual bool IsSearchButtonVisible() const wxOVERRIDE;

    virtual void ShowCancelButton( bool show ) wxOVERRIDE;
    virtual bool IsCancelButtonVisible() const wxOVERRIDE;

    virtual void SetDescriptiveText(const wxString& text) wxOVERRIDE;
    virtual wxString GetDescriptiveText() const wxOVERRIDE;

#if wxUSE_MENUS
    void PopupSearchMenu();
#endif // wxUSE_MENUS

    // intercept WM_NCCALCSIZE & WM_NCPAINT
    virtual bool MSWHandleMessage(WXLRESULT *result,
                                  WXUINT message,
                                  WXWPARAM wParam,
                                  WXLPARAM lParam) wxOVERRIDE;

private:
    void Init();

    void HighlightButton();

    void ClickButton();

    void DrawButtons(int width);

    void ToggleCancelButtonVisibility();

    // Event handlers
    void OnText(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);

    bool HasMenu() const
    {
#if wxUSE_MENUS
        return m_menu != NULL;
#else // !wxUSE_MENUS
        return false;
#endif // wxUSE_MENUS/!wxUSE_MENUS
    }

private:
    wxRect m_searchButtonRect;
    wxRect m_cancelButtonRect;

    bool m_isCancelButtonShown;
    bool m_mouseInCancelButton;

    wxColour m_highlightColour;

#if wxUSE_MENUS
    wxMenu *m_menu;
#endif // wxUSE_MENUS

    wxDECLARE_DYNAMIC_CLASS(wxSearchCtrl);
    wxDECLARE_EVENT_TABLE();
};

#endif // wxUSE_SEARCHCTRL

#endif // _WX_MSW_SEARCHCTRL_H_
