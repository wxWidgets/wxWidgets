/////////////////////////////////////////////////////////////////////////////
// Name:        docmdi.h
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCMDI_H_
#define _WX_DOCMDI_H_

#ifdef __GNUG__
#pragma interface "docmdi.h"
#endif

#include "wx/docview.h"
#include "wx/mdi.h"

/*
 * Use this instead of wxMDIParentFrame
 */

class wxDocMDIParentFrame: public wxMDIParentFrame
{
  DECLARE_CLASS(wxDocMDIParentFrame)
 public:
  wxDocMDIParentFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name);

  bool OnClose(void);
  // Extend event processing to search the document manager's event table
  virtual bool ProcessEvent(wxEvent& event);

  wxDocManager *GetDocumentManager(void) const { return m_docManager; }

  void OnExit(wxCommandEvent& event);
  void OnMRUFile(wxCommandEvent& event);

 protected:
  wxDocManager *m_docManager;


DECLARE_EVENT_TABLE()
};

/*
 * Use this instead of wxMDIChildFrame
 */

class WXDLLEXPORT wxDocMDIChildFrame: public wxMDIChildFrame
{
  DECLARE_CLASS(wxDocMDIChildFrame)

 public:
  wxDocMDIChildFrame(wxDocument *doc, wxView *view, wxMDIParentFrame *frame, wxWindowID id, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long type = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame");
  ~wxDocMDIChildFrame(void);

  bool OnClose(void);
  // Extend event processing to search the view's event table
  virtual bool ProcessEvent(wxEvent& event);

  void OnActivate(wxActivateEvent& event);

  inline wxDocument *GetDocument(void) const { return m_childDocument; }
  inline wxView *GetView(void) const { return m_childView; }
  inline void SetDocument(wxDocument *doc) { m_childDocument = doc; }
  inline void SetView(wxView *view) { m_childView = view; }
 protected:
  wxDocument*       m_childDocument;
  wxView*           m_childView;

DECLARE_EVENT_TABLE()

};

#endif
    // _WX_DOCMDI_H_
