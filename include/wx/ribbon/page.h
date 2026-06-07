///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/page.h
// Purpose:     Container for ribbon-bar-style interface panels
// Author:      Peter Cawley
// Created:     2009-05-25
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_PAGE_H_
#define _WX_RIBBON_PAGE_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/ribbon/control.h"
#include "wx/ribbon/panel.h"
#include "wx/bmpbndl.h"

class wxRibbonBar;
class wxRibbonPageScrollButton;

class WXDLLIMPEXP_RIBBON wxRibbonPage : public wxRibbonControl
{
public:
    wxRibbonPage();

    wxRibbonPage(wxRibbonBar* parent,
                 wxWindowID id = wxID_ANY,
                 const wxString& label = wxEmptyString,
                 const wxBitmapBundle& icon = wxBitmapBundle(),
                 long style = 0);

    virtual ~wxRibbonPage();

    bool Create(wxRibbonBar* parent,
                wxWindowID id = wxID_ANY,
                const wxString& label = wxEmptyString,
                const wxBitmapBundle& icon = wxBitmapBundle(),
                long style = 0);

    void SetArtProvider(wxRibbonArtProvider* art) override;

    wxBitmap GetIcon() { return m_icon.GetBitmapFor(this); }
    const wxBitmapBundle& GetIconBundle() const { return m_icon; }
    virtual wxSize GetMinSize() const override;
    void SetSizeWithScrollButtonAdjustment(int x, int y, int width, int height);
    void AdjustRectToIncludeScrollButtons(wxRect* rect) const;

    bool DismissExpandedPanel();

    virtual bool Realize() override;
    virtual bool Show(bool show = true) override;
    virtual bool Layout() override;
    virtual bool ScrollLines(int lines) override;
    bool ScrollPixels(int pixels);
    bool ScrollSections(int sections);

    wxOrientation GetMajorAxis() const;

    virtual void RemoveChild(wxWindowBase *child) override;

    void HideIfExpanded();

    wxRibbonPanel* GetPanel(int n);
    wxRibbonPanel* GetPanelById(wxWindowID id);
    size_t GetPanelCount() const;

protected:
    virtual wxSize DoGetBestSize() const override;
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) override;
    bool DoActualLayout();
    void OnEraseBackground(wxEraseEvent& evt);
    void OnPaint(wxPaintEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnDPIChanged(wxDPIChangedEvent& evt);

    bool ExpandPanels(wxOrientation direction, int maximumAmount);
    bool CollapsePanels(wxOrientation direction, int minimumAmount);
    bool ShowScrollButtons();
    void HideScrollButtons();

    void CommonInit(const wxString& label, const wxBitmapBundle& icon);
    void PopulateSizeCalcArray(wxSize (wxWindow::*getSize)(void) const);

    wxArrayRibbonControl m_collapseStack;
    wxBitmapBundle m_icon;
    wxSize m_oldSize;
    // NB: Scroll button windows are siblings rather than children (to get correct clipping of children)
    wxRibbonPageScrollButton* m_scrollLeftBtn = nullptr;
    wxRibbonPageScrollButton* m_scrollRightBtn = nullptr;
    wxSize* m_sizeCalcArray = nullptr;
    size_t m_sizeCalcArraySize;
    int m_scrollAmount;
    int m_scrollAmountLimit;
    int m_sizeInMajorAxisForChildren;
    bool m_scrollButtonsVisible;

#ifndef SWIG
    wxDECLARE_CLASS(wxRibbonPage);
    wxDECLARE_EVENT_TABLE();
#endif
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_PAGE_H_
