/////////////////////////////////////////////////////////////////////////////
// Name:        mainfrm.h
// Purpose:     Studio main window class
// Author:      Julian Smart
// Modified by:
// Created:     27/7/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _STUDIO_MAINFRM_H_
#define _STUDIO_MAINFRM_H_

#include <wx/docmdi.h>

class wxSashLayoutWindow;
class wxSashEvent;

class csFrame: public wxDocMDIParentFrame
{
  public:
    csFrame(wxDocManager *manager, wxFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style);

    void OnCloseWindow(wxCloseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnSashDragPaletteWindow(wxSashEvent& event);
    void OnSashDragProjectWindow(wxSashEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);

    // General handler for disabling items
    void OnUpdateDisable(wxUpdateUIEvent& event);
    void OnSaveUpdate(wxUpdateUIEvent& event);

DECLARE_EVENT_TABLE()
};

class csMDIChildFrame: public wxDocMDIChildFrame
{
  public:
    csMDIChildFrame(wxDocument* doc, wxView* view, wxMDIParentFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style);

    void OnActivate(wxActivateEvent& event);

DECLARE_EVENT_TABLE()
};

#endif
  // _STUDIO_MAINFRM_H_

