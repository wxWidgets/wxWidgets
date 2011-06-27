/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/toolbar.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_TOOLBAR_H_
#define _WX_MOBILE_NATIVE_TOOLBAR_H_

#include "wx/dynarray.h"
#include "wx/imaglist.h"

// Definitions should really be in tbarbase.h so we don't have to include
// wx/toolbar.h.
#include "wx/toolbar.h"

#include "wx/mobile/native/buttonstrip.h"

/**
    wxMoToolBar styles
*/

enum {
    wxTB_NORMAL_BG =            0x00000000,
    wxTB_BLACK_OPAQUE_BG =      0x00010000,
    wxTB_BLACK_TRANSLUCENT_BG = 0x00020000
};

/**
    @class wxMoToolBar

    A horizontal toolbar containing text and/or bitmap buttons. It has the same
    API as a normal wxToolBar, with a few exceptions.

    The extra toolbar styles are:

    @li wxTB_NORMAL_BG: a normal, blue appearance.
    @li wxTB_BLACK_OPAQUE_BG: a black appearance.
    @li wxTB_BLACK_TRANSLUCENT_BG: a black, translucent appearance
      (translucency not shown in simulator).

    Unlike other wxWidgets toolbar implementations, wxMoToolBar doesn't currently
    use the wxTB_TEXT and wxTB_NOICONS styles; button text and icon presence
    or absence are determined by the values passed to each button.

    If a button has only text, then the underlying wxMoBarButton will be given the
    wxBBU_BORDERED style. If the button identifier is wxID_DONE, it will be given
    the wxBBU_DONE style. Otherwise the style is wxBBU_PLAIN.

    At present no non-button items (i.e. arbitrary controls) are allowed in a wxMoToolBar.

    Spacing works a bit differently from a regular wxToolBar. wxMoToolBar::AddSeparator or
    wxMoToolBar::AddFlexibleSeparator adds a flexible separator, such that the
    space expands to push following buttons to the right. Remaining space is divided
    between flexible separators. You can also add fixed-width separators
    using wxMoToolBar::AddFixedSeparator, passing a pixel width.

    @category{wxMobile}
*/


class WXDLLEXPORT wxMoToolBar: public wxToolBar
{
public:
    /// Default constructor.
    wxMoToolBar() { Init(); }

    /// Constructor.
    wxMoToolBar(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER | wxTB_HORIZONTAL,
                const wxString& name = wxToolBarNameStr)
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER | wxTB_HORIZONTAL,
                const wxString& name = wxToolBarNameStr);

    virtual ~wxMoToolBar();

    void Init();

    /// Add a tool.
    wxToolBarToolBase *AddTool(int toolid,
                               const wxString& label,
                               const wxBitmap& bitmap,
                               const wxBitmap& bmpDisabled,
                               wxItemKind kind = wxITEM_NORMAL,
                               const wxString& shortHelp = wxEmptyString,
                               const wxString& longHelp = wxEmptyString,
                               wxObject *data = NULL)
    {
        return wxToolBarBase::AddTool(toolid, label, bitmap, bmpDisabled, kind,
                         shortHelp, longHelp, data);
    }

    /// Add a tool.
    wxToolBarToolBase *AddTool(int toolid,
                               const wxString& label,
                               const wxBitmap& bitmap,
                               const wxString& shortHelp = wxEmptyString,
                               wxItemKind kind = wxITEM_NORMAL)
    {
        return wxToolBarBase::AddTool(toolid, label, bitmap, shortHelp, kind);
    }

    wxToolBarToolBase *AddTool(wxToolBarToolBase *tool)
    {
        return wxToolBarBase::AddTool(tool);
    }

    /// Add a check tool, i.e. a tool which can be toggled.
    /// Check tools are not currently supported.
    wxToolBarToolBase *AddCheckTool(int toolid,
                                    const wxString& label,
                                    const wxBitmap& bitmap,
                                    const wxBitmap& bmpDisabled = wxNullBitmap,
                                    const wxString& shortHelp = wxEmptyString,
                                    const wxString& longHelp = wxEmptyString,
                                    wxObject *data = NULL)
    {
        return wxToolBarBase::AddCheckTool(toolid, label, bitmap, bmpDisabled,
                       shortHelp, longHelp, data);
    }

    /// Radio tools are not currently supported.
    wxToolBarToolBase *AddRadioTool(int toolid,
                                    const wxString& label,
                                    const wxBitmap& bitmap,
                                    const wxBitmap& bmpDisabled = wxNullBitmap,
                                    const wxString& shortHelp = wxEmptyString,
                                    const wxString& longHelp = wxEmptyString,
                                    wxObject *data = NULL)
    {
        return wxToolBarBase::AddRadioTool(toolid, label, bitmap, bmpDisabled,
            shortHelp, longHelp, data);
    }

    /// Adds a flexible separator to the toolbar.
    virtual wxToolBarToolBase *AddSeparator() { return wxToolBarBase::AddSeparator(); }

    /// New function to add a flexible separator. AddSeparator is an alias
    /// for this.
    virtual wxToolBarToolBase *AddFlexibleSeparator();

    /// New function to add a flexible separator. InsertSeparator is an alias
    /// for this.
    virtual wxToolBarToolBase *InsertFlexibleSeparator(size_t pos);

    /// New function to add a fixed-width separator.
    virtual wxToolBarToolBase *AddFixedSeparator(int width);

    /// New function to insert a fixed-width separator.
    virtual wxToolBarToolBase *InsertFixedSeparator(size_t pos, int width);

    /// Creates the toolbar. Call this after adding buttons and separators.
    virtual bool Realize();

    // override/implement base class virtuals
    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual void SetFocus() {}

    wxMoButtonStrip& GetButtonStrip() { return m_buttonStrip; }
    const wxMoButtonStrip& GetButtonStrip() const { return m_buttonStrip; }

    virtual void SetBorderColour(const wxColour &colour);
    virtual wxColour GetBorderColour() const;

    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual wxColour GetBackgroundColour() const;

    virtual bool SetButtonBackgroundColour(const wxColour &colour);
    virtual wxColour GetButtonBackgroundColour() const;

    virtual bool SetForegroundColour(const wxColour &colour);
    virtual wxColour GetForegroundColour() const;

    virtual bool SetFont(const wxFont& font);
    virtual wxFont GetFont() const;

    void SetToolNormalBitmap(int id, const wxBitmap& bitmap);
    void SetToolDisabledBitmap(int id, const wxBitmap& bitmap);

    // Not implemented
    virtual void SetToolBitmapSize(const wxSize& size);

    // Not implemented.
    virtual wxSize GetToolSize() const;

    // Only 1 row is allowed
    virtual void SetRows(int nRows);

protected:

    void OnMouseEvent(wxMouseEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmap& bmpNormal,
                                          const wxBitmap& bmpDisabled,
                                          wxItemKind kind,
                                          wxObject *clientData,
                                          const wxString& shortHelp,
                                          const wxString& longHelp);
    virtual wxToolBarToolBase *CreateTool(wxControl *control);
    virtual wxToolBarToolBase *CreateTool(wxControl *control,
                                          const wxString& label);

    // return the appropriate size and flags for the toolbar control
    virtual wxSize DoGetBestSize() const;

    // should be called whenever the toolbar size changes
    void UpdateSize();

private:
    wxMoButtonStrip m_buttonStrip;
    wxColour        m_borderColour;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMoToolBar)
    DECLARE_NO_COPY_CLASS(wxMoToolBar)
};

#endif
    // _WX_MOBILE_NATIVE_TOOLBAR_H_
