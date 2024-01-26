/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/toolbar.h
// Purpose:     wxToolBar class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_TBAR95_H_
#define _WX_MSW_TBAR95_H_

#if wxUSE_TOOLBAR

#include "wx/dynarray.h"
#include "wx/imaglist.h"

class WXDLLIMPEXP_CORE wxToolBar : public wxToolBarBase
{
public:
    // ctors and dtor
    wxToolBar() { Init(); }

    wxToolBar(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxToolBarNameStr))
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxToolBarNameStr));

    virtual ~wxToolBar();

    // override/implement base class virtuals
    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const override;

    virtual bool Realize() override;

    virtual wxSize GetToolSize() const override;

    virtual void SetRows(int nRows) override;

    virtual void SetToolNormalBitmap(int id, const wxBitmapBundle& bitmap) override;
    virtual void SetToolDisabledBitmap(int id, const wxBitmapBundle& bitmap) override;

    virtual void SetToolPacking(int packing) override;

    // implementation only from now on
    // -------------------------------

    virtual void SetWindowStyleFlag(long style) override;

    virtual bool MSWCommand(WXUINT param, WXWORD id) override;
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) override;

    void OnMouseEvent(wxMouseEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    void SetFocus() override {}

    static WXHBITMAP MapBitmap(WXHBITMAP bitmap, int width, int height);

    // override WndProc mainly to process WM_SIZE
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }

#ifdef wxHAS_MSW_BACKGROUND_ERASE_HOOK
    virtual bool MSWEraseBgHook(WXHDC hDC) override;
    virtual WXHBRUSH MSWGetBgBrushForChild(WXHDC hDC, wxWindowMSW *child) override;
#endif // wxHAS_MSW_BACKGROUND_ERASE_HOOK

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmapBundle& bmpNormal,
                                          const wxBitmapBundle& bmpDisabled = wxNullBitmap,
                                          wxItemKind kind = wxITEM_NORMAL,
                                          wxObject *clientData = nullptr,
                                          const wxString& shortHelp = wxEmptyString,
                                          const wxString& longHelp = wxEmptyString) override;

    virtual wxToolBarToolBase *CreateTool(wxControl *control,
                                          const wxString& label) override;
protected:
    // common part of all ctors
    void Init();

    virtual bool MSWGetDarkModeSupport(MSWDarkModeSupport& support) const override;
    virtual int MSWGetToolTipMessage() const override;

    // create the native toolbar control
    bool MSWCreateToolbar(const wxPoint& pos, const wxSize& size);

    // recreate the control completely
    void Recreate();

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool) override;
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool) override;

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable) override;
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle) override;
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle) override;

    virtual void DoSetToolBitmapSize(const wxSize& size) override;

    // return the appropriate size and flags for the toolbar control
    virtual wxSize DoGetBestSize() const override;

    // handlers for various events
    bool HandleSize(WXWPARAM wParam, WXLPARAM lParam);
#ifdef wxHAS_MSW_BACKGROUND_ERASE_HOOK
    bool HandlePaint(WXWPARAM wParam, WXLPARAM lParam);
#endif // wxHAS_MSW_BACKGROUND_ERASE_HOOK
    void HandleMouseMove(WXWPARAM wParam, WXLPARAM lParam);

    // should be called whenever the toolbar size changes
    void UpdateSize();

    // create m_disabledImgList (but doesn't fill it), set it to nullptr if it is
    // unneeded
    void CreateDisabledImageList();

    // get the Windows toolbar style of this control
    long GetMSWToolbarStyle() const;

    // set native toolbar padding
    void MSWSetPadding(WXWORD padding);

    void RealizeHelper();
    void OnDPIChanged(wxDPIChangedEvent& event);

    // the big bitmap containing all bitmaps of the toolbar buttons
    WXHBITMAP m_hBitmap;

    // the image list with disabled images, may be null if we use
    // system-provided versions of them
    wxImageList *m_disabledImgList;

    // the total number of toolbar elements
    size_t m_nButtons;

    // the sum of the sizes of the fixed items (i.e. excluding stretchable
    // spaces) in the toolbar direction
    int m_totalFixedSize;

    // the tool the cursor is in
    wxToolBarToolBase *m_pInTool;

private:
    // makes sure tool bitmap size is sufficient for all tools
    void AdjustToolBitmapSize();

    // update the sizes of stretchable spacers to consume all extra space we
    // have
    void UpdateStretchableSpacersSize();

#ifdef wxHAS_MSW_BACKGROUND_ERASE_HOOK
    // do erase the toolbar background, always do it for the entire control as
    // the caller sets the clipping region correctly to exclude parts which
    // should not be erased
    void MSWDoEraseBackground(WXHDC hDC);

    // return the brush to use for erasing the toolbar background
    WXHBRUSH MSWGetToolbarBgBrush();
#endif // wxHAS_MSW_BACKGROUND_ERASE_HOOK

    // Return true if we're showing the labels for the embedded controls: we
    // only do it if text is enabled and, somewhat less expectedly, if icons
    // are enabled too because showing both the control and its label when only
    // text is shown for the other buttons is too inconsistent to be useful.
    bool AreControlLabelsShown() const
    {
        return HasFlag(wxTB_TEXT) && !HasFlag(wxTB_NOICONS);
    }

    // Return the size required to accommodate the given tool which must be of
    // "control" type.
    wxSize MSWGetFittingtSizeForControl(class wxToolBarTool* tool) const;


    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxToolBar);
    wxDECLARE_NO_COPY_CLASS(wxToolBar);
};

#endif // wxUSE_TOOLBAR

#endif // _WX_MSW_TBAR95_H_

