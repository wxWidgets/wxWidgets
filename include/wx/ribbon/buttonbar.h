///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/buttonbar.h
// Purpose:     Ribbon control similar to a tool bar
// Author:      Peter Cawley
// Modified by:
// Created:     2009-07-01
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#ifndef _WX_RIBBON_BUTTON_BAR_H_
#define _WX_RIBBON_BUTTON_BAR_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/ribbon/control.h"
#include "wx/dynarray.h"

enum wxRibbonButtonBarButtonKind
{
    wxRIBBON_BUTTONBAR_BUTTON_NORMAL    = 1 << 0,
    wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN  = 1 << 1,
    wxRIBBON_BUTTONBAR_BUTTON_HYBRID    = wxRIBBON_BUTTONBAR_BUTTON_NORMAL | wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN,
};

enum wxRibbonButtonBarButtonState
{
    wxRIBBON_BUTTONBAR_BUTTON_SMALL     = 0 << 0,
    wxRIBBON_BUTTONBAR_BUTTON_MEDIUM    = 1 << 0,
    wxRIBBON_BUTTONBAR_BUTTON_LARGE     = 2 << 0,
    wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK = 3 << 0,

    wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED    = 1 << 3,
    wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_HOVERED  = 1 << 4,
    wxRIBBON_BUTTONBAR_BUTTON_NORMAL_ACTIVE     = 1 << 5,
    wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_ACTIVE   = 1 << 6,
    wxRIBBON_BUTTONBAR_BUTTON_DISABLED          = 1 << 7,
    wxRIBBON_BUTTONBAR_BUTTON_STATE_MASK        = 0xF8,
};

class wxRibbonButtonBarButtonBase;
class wxRibbonButtonBarLayout;

WX_DEFINE_USER_EXPORTED_ARRAY(wxRibbonButtonBarLayout*, wxArrayRibbonButtonBarLayout, class WXDLLIMPEXP_RIBBON);
WX_DEFINE_USER_EXPORTED_ARRAY(wxRibbonButtonBarButtonBase*, wxArrayRibbonButtonBarButtonBase, class WXDLLIMPEXP_RIBBON);

class WXDLLIMPEXP_RIBBON wxRibbonButtonBar : public wxRibbonControl
{
public:
    wxRibbonButtonBar();

    wxRibbonButtonBar(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0);

    virtual ~wxRibbonButtonBar();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    virtual wxRibbonButtonBarButtonBase* AddButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string,
                wxRibbonButtonBarButtonKind kind = wxRIBBON_BUTTONBAR_BUTTON_NORMAL);

    virtual wxRibbonButtonBarButtonBase* AddDropdownButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string = wxEmptyString);

    virtual wxRibbonButtonBarButtonBase* AddHybridButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxString& help_string = wxEmptyString);
    
    virtual wxRibbonButtonBarButtonBase* AddButton(
                int button_id,
                const wxString& label,
                const wxBitmap& bitmap,
                const wxBitmap& bitmap_small = wxNullBitmap,
                const wxBitmap& bitmap_disabled = wxNullBitmap,
                const wxBitmap& bitmap_small_disabled = wxNullBitmap,
                wxRibbonButtonBarButtonKind kind = wxRIBBON_BUTTONBAR_BUTTON_NORMAL,
                const wxString& help_string = wxEmptyString,
                wxObject* client_data = NULL);

    virtual bool Realize();
    virtual void ClearButtons();
    virtual bool DeleteButton(int button_id);
    virtual void EnableButton(int button_id, bool enable = true);

    virtual void SetArtProvider(wxRibbonArtProvider* art);
    virtual bool IsSizingContinuous() const;
    virtual wxSize GetNextSmallerSize(wxOrientation direction) const;
    virtual wxSize GetNextLargerSize(wxOrientation direction) const;

    virtual wxSize GetMinSize() const;
    virtual wxSize DoGetBestSize() const;
protected:
    wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

    void OnEraseBackground(wxEraseEvent& evt);
    void OnPaint(wxPaintEvent& evt);
    void OnSize(wxSizeEvent& evt);

    void CommonInit(long style);
    void MakeLayouts();
    static wxBitmap MakeResizedBitmap(const wxBitmap& original, wxSize size);
    static wxBitmap MakeDisabledBitmap(const wxBitmap& original);
    void FetchButtonSizeInfo(wxRibbonButtonBarButtonBase* button,
        wxRibbonButtonBarButtonState size, wxDC& dc);

    wxArrayRibbonButtonBarLayout m_layouts;
    wxArrayRibbonButtonBarButtonBase m_buttons;

    wxSize m_bitmap_size_large;
    wxSize m_bitmap_size_small;
    int m_current_layout;
    bool m_layouts_valid;

#ifndef SWIG
    DECLARE_CLASS(wxRibbonButtonBar)
    DECLARE_EVENT_TABLE()
#endif
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_BUTTON_BAR_H_
