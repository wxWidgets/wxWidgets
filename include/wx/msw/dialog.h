/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.h
// Purpose:     wxDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DIALOGH__
#define __DIALOGH__

#ifdef __GNUG__
#pragma interface "dialog.h"
#endif

#include "wx/panel.h"

WXDLLEXPORT_DATA(extern const char*) wxDialogNameStr;

// Dialog boxes
class WXDLLEXPORT wxDialog: public wxPanel
{
  DECLARE_DYNAMIC_CLASS(wxDialog)
 protected:
  bool m_modalShowing;
public:

  wxDialog(void);

  // Constructor with a modal flag, but no window id - the old convention
  inline wxDialog(wxWindow *parent,
           const wxString& title, bool modal,
           const int x = -1, const int y= -1, const int width = 500, const int height = 500,
           const long style = wxDEFAULT_DIALOG_STYLE,
           const wxString& name = wxDialogNameStr)
  {
      long modalStyle = modal ? wxDIALOG_MODAL : wxDIALOG_MODELESS ;
      Create(parent, -1, title, wxPoint(x, y), wxSize(width, height), style|modalStyle, name);
  }

  // Constructor with no modal flag - the new convention.
  inline wxDialog(wxWindow *parent, const wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxDEFAULT_DIALOG_STYLE,
           const wxString& name = wxDialogNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id,
           const wxString& title, // bool modal = FALSE, // TODO make this a window style?
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxDEFAULT_DIALOG_STYLE,
           const wxString& name = wxDialogNameStr);

  ~wxDialog(void);

  virtual bool Destroy(void);
  void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);
  void SetClientSize(const int width, const int height);
  void GetPosition(int *x, int *y) const;
  bool Show(const bool show);
  bool IsShown(void) const ;
  void Iconize(const bool iconize);

#if WXWIN_COMPATIBILITY
  inline bool Iconized(void) const { return IsIconized(); };
#endif

  virtual bool IsIconized(void) const;
  void Fit(void);

  void SetTitle(const wxString& title);
  wxString GetTitle(void) const ;

  bool OnClose(void);
  void OnCharHook(wxKeyEvent& event);
  void OnPaint(wxPaintEvent& event);
  void OnCloseWindow(wxCloseEvent& event);

  void SetModal(const bool flag);

  virtual void Centre(const int direction = wxBOTH);
  virtual bool IsModal(void) const { return ((GetWindowStyleFlag() & wxDIALOG_MODAL) == wxDIALOG_MODAL); }

  // For now, same as Show(TRUE) but returns return code
  virtual int ShowModal(void);
  virtual void EndModal(int retCode);

  // Standard buttons
  void OnOK(wxCommandEvent& event);
  void OnApply(wxCommandEvent& event);
  void OnCancel(wxCommandEvent& event);

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

  // IMPLEMENTATION
  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
  virtual long MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
  virtual bool MSWProcessMessage(WXMSG* pMsg);
//  virtual bool MSWOnEraseBkgnd(WXHDC pDC);
  virtual bool MSWOnClose(void);
  inline bool IsModalShowing() const { return m_modalShowing ; }
  virtual WXHBRUSH OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
  			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

DECLARE_EVENT_TABLE()
};

#endif
    // __DIALOGH__
