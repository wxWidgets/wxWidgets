/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/notebook.h
// Purpose:     wxWinUI wxNotebook declaration (WinUI TabView)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_NOTEBOOK_H_
#define _WX_WINUI_NOTEBOOK_H_

#include "wx/arrstr.h"

#include <memory>

class wxWinUINotebookImpl;

class WXDLLIMPEXP_CORE wxNotebook : public wxNotebookBase
{
public:
    wxNotebook();
    wxNotebook(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxASCII_STR(wxNotebookNameStr));
    ~wxNotebook() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxNotebookNameStr));

    // wxBookCtrlBase overrides
    int SetSelection(size_t nPage) override;
    int ChangeSelection(size_t nPage) override;

    bool SetPageText(size_t nPage, const wxString& strText) override;
    wxString GetPageText(size_t nPage) const override;

    int GetPageImage(size_t nPage) const override;
    bool SetPageImage(size_t nPage, int nImage) override;

    bool InsertPage(size_t nPage,
                    wxNotebookPage *pPage,
                    const wxString& strText,
                    bool bSelect = false,
                    int imageId = NO_IMAGE) override;

    bool DeleteAllPages() override;

    // wxNotebookBase overrides
    void SetPadding(const wxSize& padding) override;
    void SetTabSize(const wxSize& sz) override;
    wxSize CalcSizeFromPage(const wxSize& sizePage) const override;

protected:
    void Init();

    wxWindow *DoRemovePage(size_t nPage) override;

    void DoSize() override;
    wxRect GetPageRect() const override;

    void UpdateSelectedPage(size_t newsel) override;
    wxBookCtrlEvent* CreatePageChangingEvent() const override;
    void MakeChangedEvent(wxBookCtrlEvent& event) override;

    // Capture the tab-strip height from the WinUI TabView layout (once it is
    // known) so the page area can be derived deterministically from the client
    // size; called when the TabView completes a layout pass.
    void UpdateTabStripHeightFromLayout();

    // Notify of a user-driven tab change coming from the WinUI TabView.
    void OnTabViewSelectionChanged(int sel);

    std::unique_ptr<wxWinUINotebookImpl> m_winui;
    wxArrayString m_pageTexts;
    wxArrayInt m_pageImages;

    // Height (in DIPs) of the WinUI TabView tab strip, measured from its layout.
    // -1 until the first layout pass provides a usable value.
    int m_tabStripHeightDIP = -1;

    // Guard against re-entrancy while we programmatically update the TabView.
    bool m_updating = false;

private:
    // Render the page's image-list icon (if any) on its TabViewItem.
    void UpdateTabIcon(size_t nPage);

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxNotebook);
};

#endif // _WX_WINUI_NOTEBOOK_H_
