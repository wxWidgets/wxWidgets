/////////////////////////////////////////////////////////////////////////////
// Name:        docmdi.h
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCMDI_H_
#define _WX_DOCMDI_H_

#include "wx/defs.h"

#if wxUSE_MDI_ARCHITECTURE

#include "wx/docview.h"
#include "wx/mdi.h"

/*
 * Use this instead of wxMDIParentFrame
 */

class WXDLLIMPEXP_CORE wxDocMDIParentFrame: public wxMDIParentFrame
{
public:
    wxDocMDIParentFrame();
    wxDocMDIParentFrame(wxDocManager *manager, wxFrame *parent, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr);

    bool Create(wxDocManager *manager, wxFrame *parent, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr);

    wxDocManager *GetDocumentManager(void) const { return m_docManager; }

    void OnExit(wxCommandEvent& event);
    void OnMRUFile(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

protected:
    void Init();

    virtual bool TryValidator(wxEvent& event);

    wxDocManager *m_docManager;

private:
    DECLARE_CLASS(wxDocMDIParentFrame)
    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(wxDocMDIParentFrame);
};

/*
 * Use this instead of wxMDIChildFrame
 */

class WXDLLIMPEXP_CORE wxDocMDIChildFrame: public wxMDIChildFrame
{
public:
    wxDocMDIChildFrame();
    wxDocMDIChildFrame(wxDocument *doc, wxView *view, wxMDIParentFrame *frame, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long type = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr);
    virtual ~wxDocMDIChildFrame();

    bool Create(wxDocument *doc,
                wxView *view,
                wxMDIParentFrame *frame,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long type = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    void OnActivate(wxActivateEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    inline wxDocument *GetDocument() const { return m_childDocument; }
    inline wxView *GetView(void) const { return m_childView; }
    inline void SetDocument(wxDocument *doc) { m_childDocument = doc; }
    inline void SetView(wxView *view) { m_childView = view; }
    bool Destroy() { m_childView = NULL; return wxMDIChildFrame::Destroy(); }

protected:
    void Init();

    virtual bool TryValidator(wxEvent& event);

    wxDocument*       m_childDocument;
    wxView*           m_childView;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(wxDocMDIChildFrame)
    wxDECLARE_NO_COPY_CLASS(wxDocMDIChildFrame);
};

#endif
    // wxUSE_MDI_ARCHITECTURE

#endif
    // _WX_DOCMDI_H_
