///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/toolbar.h
// Purpose:     wxToolBar declaration
// Author:      Robert Roebling
// Created:     10.09.00
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_TOOLBAR_H_
#define _WX_UNIV_TOOLBAR_H_

#include "wx/button.h"      // for wxStdButtonInputHandler

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

#define wxACTION_TOOLBAR_TOGGLE  wxACTION_BUTTON_TOGGLE
#define wxACTION_TOOLBAR_PRESS   wxACTION_BUTTON_PRESS
#define wxACTION_TOOLBAR_RELEASE wxACTION_BUTTON_RELEASE
#define wxACTION_TOOLBAR_CLICK   wxACTION_BUTTON_CLICK
#define wxACTION_TOOLBAR_ENTER   wxT("enter")     // highlight the tool
#define wxACTION_TOOLBAR_LEAVE   wxT("leave")     // unhighlight the tool

// ----------------------------------------------------------------------------
// wxToolBar
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToolBar : public wxToolBarBase
{
public:
    // construction/destruction
    wxToolBar() { Init(); }
    wxToolBar(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = wxASCII_STR(wxToolBarNameStr))
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxASCII_STR(wxToolBarNameStr) );

    virtual ~wxToolBar();

    virtual bool Realize() override;

    virtual void SetWindowStyleFlag( long style ) override;

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const override;

    virtual void SetToolShortHelp(int id, const wxString& helpString) override;

    virtual void SetMargins(int x, int y) override;
    void SetMargins(const wxSize& size)
        { SetMargins((int) size.x, (int) size.y); }

    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1,
                               const wxString& strArg = wxEmptyString) override;
    static wxInputHandler *GetStdInputHandler(wxInputHandler *handlerDef);
    virtual wxInputHandler *DoGetStdInputHandler(wxInputHandler *handlerDef) override
    {
        return GetStdInputHandler(handlerDef);
    }

protected:
    // common part of all ctors
    void Init();

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool) override;
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool) override;

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable) override;
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle) override;
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle) override;

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmapBundle& bmpNormal,
                                          const wxBitmapBundle& bmpDisabled,
                                          wxItemKind kind,
                                          wxObject *clientData,
                                          const wxString& shortHelp,
                                          const wxString& longHelp) override;
    virtual wxToolBarToolBase *CreateTool(wxControl *control,
                                          const wxString& label) override;

    virtual wxSize DoGetBestClientSize() const override;
    virtual void DoDraw(wxControlRenderer *renderer) override;

    // get the bounding rect for the given tool
    wxRect GetToolRect(wxToolBarToolBase *tool) const;

    // redraw the given tool
    void RefreshTool(wxToolBarToolBase *tool);

    // (re)calculate the tool positions, should only be called if it is
    // necessary to do it, i.e. m_needsLayout == true
    void DoLayout();

    // get the rect limits depending on the orientation: top/bottom for a
    // vertical toolbar, left/right for a horizontal one
    void GetRectLimits(const wxRect& rect, wxCoord *start, wxCoord *end) const;

private:
    // have we calculated the positions of our tools?
    bool m_needsLayout;

    // the width of a separator
    wxCoord m_widthSeparator;

    // the total size of all toolbar elements
    wxCoord m_maxWidth,
            m_maxHeight;

private:
    wxDECLARE_DYNAMIC_CLASS(wxToolBar);
};

// ----------------------------------------------------------------------------
// wxToolBarTool: our implementation of wxToolBarToolBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxString& label,
                  const wxBitmapBundle& bmpNormal,
                  const wxBitmapBundle& bmpDisabled,
                  wxItemKind kind,
                  wxObject *clientData,
                  const wxString& shortHelp,
                  const wxString& longHelp)
        : wxToolBarToolBase(tbar, id, label, bmpNormal, bmpDisabled, kind,
                            clientData, shortHelp, longHelp)
    {
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control, const wxString& label)
        : wxToolBarToolBase(tbar, control, label)
    {
    }

private:
    // is this tool pressed, even temporarily? (this is different from being
    // permanently toggled which is what IsToggled() returns)
    bool IsPressed() const
        { return CanBeToggled() ? IsToggled() != m_isInverted : m_isInverted; }

    // are we temporarily pressed/unpressed?
    bool IsInverted() const { return m_isInverted; }

    // press the tool temporarily by inverting its toggle state
    void Invert() { m_isInverted = !m_isInverted; }

    // Set underMouse
    void SetUnderMouse( bool under = true ) { m_underMouse = under; }
    bool IsUnderMouse() { return m_underMouse; }

    // the tool position (for controls)
    wxCoord m_x = wxDefaultCoord;
    wxCoord m_y = wxDefaultCoord;
    wxCoord m_width = 0;
    wxCoord m_height = 0;

    // true if the tool is pressed
    bool m_isInverted = false;

    // true if the tool is under the mouse
    bool m_underMouse = false;

    friend class wxToolBar;
};

#endif // _WX_UNIV_TOOLBAR_H_
