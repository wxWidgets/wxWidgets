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

#ifndef _WX_DIALOG_H_
#define _WX_DIALOG_H_

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

  wxDialog();

  // Constructor with a modal flag, but no window id - the old convention
  inline wxDialog(wxWindow *parent,
           const wxString& title, bool modal,
           int x = -1, int y= -1, int width = 500, int height = 500,
           long style = wxDEFAULT_DIALOG_STYLE,
           const wxString& name = wxDialogNameStr)
  {
      long modalStyle = modal ? wxDIALOG_MODAL : wxDIALOG_MODELESS ;
      Create(parent, -1, title, wxPoint(x, y), wxSize(width, height), style|modalStyle, name);
  }

  // Constructor with no modal flag - the new convention.
  inline wxDialog(wxWindow *parent, wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_DIALOG_STYLE,
           const wxString& name = wxDialogNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& title, // bool modal = FALSE, // TODO make this a window style?
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_DIALOG_STYLE,
           const wxString& name = wxDialogNameStr);

  ~wxDialog();

  virtual bool Destroy();

  virtual void SetClientSize(int width, int height);

  virtual void GetPosition(int *x, int *y) const;

  bool Show(bool show);
  bool IsShown() const ;
  void Iconize(bool iconize);

#if WXWIN_COMPATIBILITY
  inline bool Iconized() const { return IsIconized(); };
#endif

  virtual bool IsIconized() const;
  void Fit();

  void SetTitle(const wxString& title);
  wxString GetTitle() const ;

  void OnSize(wxSizeEvent& event);
  bool OnClose();
  void OnCharHook(wxKeyEvent& event);
  void OnPaint(wxPaintEvent& event);
  void OnCloseWindow(wxCloseEvent& event);

  void SetModal(bool flag);

  virtual void Centre(int direction = wxBOTH);
  virtual bool IsModal() const { return ((GetWindowStyleFlag() & wxDIALOG_MODAL) == wxDIALOG_MODAL); }

  // For now, same as Show(TRUE) but returns return code
  virtual int ShowModal();
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
  virtual bool MSWOnClose();
  inline bool IsModalShowing() const { return m_modalShowing ; }
  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
  			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

protected:
  WXHWND m_hwndOldFocus;  // the window which had focus before we were shown

DECLARE_EVENT_TABLE()
};

#endif
    // _WX_DIALOG_H_
