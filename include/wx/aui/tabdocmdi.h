/////////////////////////////////////////////////////////////////////////////
// Name:        tabdocmdi.h
// Purpose:     Frame classes for Aui MDI document/view applications
// Author:      Julian Smart
// Modified by: Kinaou Hervé
// Created:     01/02/97
// RCS-ID:      $Id:$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_AUITABDOCMDI_H_
#define _WX_AUITABDOCMDI_H_

#if wxUSE_AUI

#include "wx/docview.h"
#include "wx/aui/tabmdi.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------


/*
 * Use this instead of wxAuiMDIParentFrame
 */

class wxAuiDocMDIChildFrame;
class WXDLLIMPEXP_AUI wxAuiDocMDIParentFrame: public wxAuiMDIParentFrame
{
public:
    wxAuiDocMDIParentFrame();
    wxAuiDocMDIParentFrame(wxDocManager *manager,
                           wxWindow *parent,
                           wxWindowID winid,
                           const wxString& title,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                           const wxString& name = wxFrameNameStr);

    virtual ~wxAuiDocMDIParentFrame();

    bool Create(wxDocManager *manager,
                wxWindow *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = wxFrameNameStr );

    // Extend event processing to search the document manager's event table
    virtual bool ProcessEvent(wxEvent& event);

    wxDocManager *GetDocumentManager(void) const { return m_docManager; }

    void OnExit(wxCommandEvent& event);
    void OnMRUFile(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

protected:
    void Init();
    wxDocManager *m_docManager;

public:
    void SetArtProvider(wxAuiTabArt* provider);
    wxAuiTabArt* GetArtProvider();
    wxAuiNotebook* GetNotebook() const;
    
#if wxUSE_MENUS
    wxMenu* GetWindowMenu() const { return m_pWindowMenu; }
    void SetWindowMenu(wxMenu* pMenu);
#endif // wxUSE_MENUS

    void SetChildMenuBar(wxAuiDocMDIChildFrame *pChild);

    wxAuiDocMDIChildFrame *GetActiveChild() const;
    void SetActiveChild(wxAuiDocMDIChildFrame* pChildFrame);

    wxAuiMDIClientWindow *GetClientWindow() const;

#if wxUSE_MENUS
    void RemoveWindowMenu(wxMenuBar *pMenuBar);
    void AddWindowMenu(wxMenuBar *pMenuBar);
    void DoHandleMenu(wxCommandEvent &event);
#endif // wxUSE_MENUS


private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxAuiDocMDIParentFrame)
};

/*
 * Use this instead of wxAuiDocMDIChildFrame
 */

class WXDLLIMPEXP_AUI wxAuiDocMDIChildFrame: public wxAuiMDIChildFrame
{
public:
    wxAuiDocMDIChildFrame();
    // Bricsys change start : add tabPosition
    wxAuiDocMDIChildFrame(wxDocument *doc,
                          wxView *view,
                          wxAuiDocMDIParentFrame *parent,
                          wxWindowID winid,
                          const wxString& title,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = wxDEFAULT_FRAME_STYLE,
                          const wxString& name = wxFrameNameStr,
                          int tabPosition = -1);
    // Bricsys change end : add tabPosition

    virtual ~wxAuiDocMDIChildFrame();

    // Bricsys change start : add tabPosition
    bool Create(wxDocument *doc,
                wxView *view,
                wxAuiDocMDIParentFrame *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr,
                int tabPosition = -1);
    // Bricsys change end : add tabPosition

    // Extend event processing to search the view's event table
    virtual bool ProcessEvent(wxEvent& event);

    inline wxDocument *GetDocument() const { return m_childDocument; }
    inline wxView *GetView(void) const { return m_childView; }
    inline void SetDocument(wxDocument *doc) { m_childDocument = doc; }
    inline void SetView(wxView *view) { m_childView = view; }
    bool Destroy() { m_childView = (wxView *)NULL; return wxAuiMDIChildFrame::Destroy(); }

protected:
    void Init();
    wxDocument*       m_childDocument;
    wxView*           m_childView;

public:
    void OnMenuHighlight(wxMenuEvent& evt);
    void OnActivate(wxActivateEvent& evt);
    void OnCloseWindow(wxCloseEvent& evt);

    void SetMDIParentFrame(wxAuiDocMDIParentFrame* parent);
    wxAuiDocMDIParentFrame* GetMDIParentFrame() const;

private:
    DECLARE_DYNAMIC_CLASS(wxAuiDocMDIChildFrame)
    DECLARE_EVENT_TABLE()

    friend class wxAuiMDIClientWindow;
};

#endif
    // wxUSE_AUI

#endif
    // _WX_AUITABDOCMDI_H_
