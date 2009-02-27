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

    virtual bool TryBefore(wxEvent& event);

    wxDocManager *m_docManager;

private:
    DECLARE_CLASS(wxDocMDIParentFrame)
    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(wxDocMDIParentFrame);
};

// ----------------------------------------------------------------------------
// An MDI document child frame: we need to define it as a class just for wxRTTI,
// otherwise we could simply typedef it
// ----------------------------------------------------------------------------

#ifdef __VISUALC6__
    // "non dll-interface class 'wxDocChildFrameAny<>' used as base interface
    // for dll-interface class 'wxDocMDIChildFrame'" -- this is bogus as the
    // template will be DLL-exported but only once it is used as base class
    // here!
    #pragma warning (disable:4275)
#endif

typedef
  wxDocChildFrameAny<wxMDIChildFrame, wxMDIParentFrame> wxDocMDIChildFrameBase;

class WXDLLIMPEXP_CORE wxDocMDIChildFrame : public wxDocMDIChildFrameBase
{
public:
    wxDocMDIChildFrame(wxDocument *doc,
                       wxView *view,
                       wxMDIParentFrame *parent,
                       wxWindowID id,
                       const wxString& title,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxDEFAULT_FRAME_STYLE,
                       const wxString& name = wxFrameNameStr)
        : wxDocMDIChildFrameBase(doc, view,
                                 parent, id, title, pos, size, style, name)
    {
    }

private:
    DECLARE_CLASS(wxDocMDIChildFrame)
    wxDECLARE_NO_COPY_CLASS(wxDocMDIChildFrame);
};

#ifdef __VISUALC6__
    #pragma warning (default:4275)
#endif

#endif // wxUSE_MDI_ARCHITECTURE

#endif // _WX_DOCMDI_H_
