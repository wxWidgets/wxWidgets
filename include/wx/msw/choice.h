/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/choice.h
// Purpose:     wxChoice class
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to derive from wxChoiceBase
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICE_H_
#define _WX_CHOICE_H_

struct tagCOMBOBOXINFO;

// ----------------------------------------------------------------------------
// Choice item
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxChoice : public wxChoiceBase
{
public:
    // ctors
    wxChoice() { Init(); }
    virtual ~wxChoice();

    wxChoice(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = nullptr,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxChoiceNameStr))
    {
        Init();
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }

    wxChoice(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             const wxArrayString& choices,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxChoiceNameStr))
    {
        Init();
        Create(parent, id, pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxChoiceNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxChoiceNameStr));

    virtual bool Show(bool show = true) override;

    virtual void SetLabel(const wxString& label) override;

    virtual unsigned int GetCount() const override;
    virtual int GetSelection() const override;
    virtual int GetCurrentSelection() const override;
    virtual void SetSelection(int n) override;

    virtual int FindString(const wxString& s, bool bCase = false) const override;
    virtual wxString GetString(unsigned int n) const override;
    virtual void SetString(unsigned int n, const wxString& s) override;

    virtual wxVisualAttributes GetDefaultAttributes() const override
    {
        return GetClassDefaultAttributes(GetWindowVariant());
    }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // MSW only
    virtual bool MSWCommand(WXUINT param, WXWORD id) override;
    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;
    virtual WXHBRUSH MSWControlColor(WXHDC hDC, WXHWND hWnd) override;
    virtual bool MSWShouldPreProcessMessage(WXMSG *pMsg) override;
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }

protected:
    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    // common part of all ctors
    void Init()
    {
        m_lastAcceptedSelection =
        m_pendingSelection = wxID_NONE;
        m_heightOwn = wxDefaultCoord;
    }

    virtual void DoDeleteOneItem(unsigned int n) override;
    virtual void DoClear() override;

    virtual int DoInsertItems(const wxArrayStringsAdapter& items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type) override;

    virtual void DoMoveWindow(int x, int y, int width, int height) override;
    virtual void DoSetItemClientData(unsigned int n, void* clientData) override;
    virtual void* DoGetItemClientData(unsigned int n) const override;

    // MSW implementation
    virtual wxSize DoGetBestSize() const override;
    virtual void DoGetSize(int *w, int *h) const override;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) override;
    virtual wxSize DoGetSizeFromTextSize(int xlen, int ylen = -1) const override;

    virtual bool MSWGetDarkModeSupport(MSWDarkModeSupport& support) const override;

    // Show or hide the popup part of the control.
    void MSWDoPopupOrDismiss(bool show);

    // update the height of the drop down list to fit the number of items we
    // have (without changing the visible height)
    void MSWUpdateDropDownHeight();

    // set the height of the visible part of the control to m_heightOwn
    void MSWUpdateVisibleHeight();

    // create and initialize the control
    bool CreateAndInit(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name);

    // free all memory we have (used by Clear() and dtor)
    void Free();

    // set the height for simple combo box
    int SetHeightSimpleComboBox(int nItems) const;

#if wxUSE_DEFERRED_SIZING
    virtual void MSWEndDeferWindowPos() override;
#endif // wxUSE_DEFERRED_SIZING

    // These variables are only used while the drop down is opened.
    //
    // The first one contains the item that had been originally selected before
    // the drop down was opened and the second one the item we should select
    // when the drop down is closed again.
    int m_lastAcceptedSelection,
        m_pendingSelection;

    // the height of the control itself if it was set explicitly or
    // wxDefaultCoord if it hadn't
    int m_heightOwn;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxChoice);
};

#endif // _WX_CHOICE_H_
