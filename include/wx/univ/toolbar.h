///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/toolbar.h
// Purpose:     wxToolBar declaration
// Author:      Robert Roebling
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_TOOLBAR_H_
#define _WX_UNIV_TOOLBAR_H_

#ifdef __GNUG__
    #pragma interface "univtoolbar.h"
#endif

#include "wx/button.h"      // for wxStdButtonInputHandler

class WXDLLEXPORT wxToolBarTool;

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

#define wxACTION_TOOLBAR_TOGGLE  wxACTION_BUTTON_TOGGLE
#define wxACTION_TOOLBAR_PRESS   wxACTION_BUTTON_PRESS
#define wxACTION_TOOLBAR_RELEASE wxACTION_BUTTON_RELEASE
#define wxACTION_TOOLBAR_CLICK   wxACTION_BUTTON_CLICK
#define wxACTION_TOOLBAR_ENTER   _T("enter")     // highlight the tool
#define wxACTION_TOOLBAR_LEAVE   _T("leave")     // unhighlight the tool

// ----------------------------------------------------------------------------
// wxToolBar
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBar : public wxToolBarBase
{    
public:
    // construction/destruction
    wxToolBar() { Init(); }
    wxToolBar(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = wxToolBarNameStr)
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxToolBarNameStr );
                 
    virtual ~wxToolBar();

    virtual bool Realize();

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual void SetToolShortHelp(int id, const wxString& helpString);

protected:
    // common part of all ctors
    void Init();

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxBitmap& bitmap1,
                                          const wxBitmap& bitmap2,
                                          bool toggle,
                                          wxObject *clientData,
                                          const wxString& shortHelpString,
                                          const wxString& longHelpString);
    virtual wxToolBarToolBase *CreateTool(wxControl *control);

    // implement wxUniversal methods
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = -1,
                               const wxString& strArg = wxEmptyString);
    virtual wxSize DoGetBestClientSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);

    // get the bounding rect for the given tool
    wxRect GetToolRect(wxToolBarToolBase *tool) const;

    // redraw the given tool
    void RefreshTool(wxToolBarToolBase *tool);

    // (re)calculate the tool positions, should only be called if it is
    // necessary to do it, i.e. m_needsLayout == TRUE
    void DoLayout();
    
    // get the rect limits depending on the orientation: top/bottom for a
    // vertical toolbar, left/right for a horizontal one
    void GetRectLimits(const wxRect& rect, wxCoord *start, wxCoord *end) const;

    // wxButton actions: all these use m_toolPressed and can only be called if
    // we have one
    void Toggle();
    void Press();
    void Release();

    // this one used m_toolCurrent
    void Click();

private:
    // have we calculated the positions of our tools?
    bool m_needsLayout;

    // the width of a separator
    wxCoord m_widthSeparator;

    // the total size of all toolbar elements
    wxCoord m_maxWidth,
            m_maxHeight;
    
    // the tool over which the mouse currently is or NULL
    wxToolBarToolBase *m_toolCurrent;

    // the tool which currently has the mouse capture (i.e. the one user is
    // pressing) or NULL
    wxToolBarTool *m_toolPressed;

    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

// ----------------------------------------------------------------------------
// wxStdToolbarInputHandler: translates SPACE and ENTER keys and the left mouse
// click into button press/release actions
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdToolbarInputHandler : public wxStdButtonInputHandler
{
public:
    wxStdToolbarInputHandler(wxInputHandler *inphand);

    virtual bool HandleKey(wxInputConsumer *consumer,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxInputConsumer *consumer,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxInputConsumer *consumer, const wxMouseEvent& event);
    virtual bool HandleFocus(wxInputConsumer *consumer, const wxFocusEvent& event);
    virtual bool HandleActivation(wxInputConsumer *consumer, bool activated);
};

#endif // _WX_UNIV_TOOLBAR_H_
