/////////////////////////////////////////////////////////////////////////////
// Name:        propform.h
// Purpose:     Property form classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPFORM_H_
#define _WX_PROPFORM_H_

#ifdef __GNUG__
#pragma interface "propform.h"
#endif

#include "wx/prop.h"

////
//// Property form classes: for using an existing dialog or panel
////

#define wxID_PROP_REVERT        3100
#define wxID_PROP_UPDATE        3101

// Mediates between a physical panel and the property sheet
class WXDLLEXPORT wxPropertyFormView: public wxPropertyView
{
 DECLARE_DYNAMIC_CLASS(wxPropertyFormView)
 public:
  wxPropertyFormView(wxWindow *propPanel = NULL, long flags = 0);
  ~wxPropertyFormView(void);

  // Associates and shows the view
  virtual void ShowView(wxPropertySheet *propertySheet, wxWindow *panel);

  // Update this view of the viewed object, called e.g. by
  // the object itself.
  virtual bool OnUpdateView(void);

  // Transfer values from property sheet to dialog
  virtual bool TransferToDialog(void);

  // Transfer values from dialog to property sheet
  virtual bool TransferToPropertySheet(void);

  // Check that all the values are valid
  virtual bool Check(void);

  // Give each property in the sheet a panel item, by matching
  // the name of the property to the name of the panel item.
  // The user doesn't always want to call this; sometimes, it
  // will have been done explicitly (e.g., no matching names).
  virtual bool AssociateNames(void);

  void OnOk(wxCommandEvent& event);
  void OnCancel(wxCommandEvent& event);
  void OnHelp(wxCommandEvent& event);
  void OnUpdate(wxCommandEvent& event);
  void OnRevert(wxCommandEvent& event);

  virtual bool OnClose();
  virtual void OnDoubleClick(wxControl *item);

  // TODO: does OnCommand still get called...???
  virtual void OnCommand(wxWindow& win, wxCommandEvent& event);

  inline virtual void AssociatePanel(wxWindow *win) { m_propertyWindow = win; }
  inline virtual wxWindow *GetPanel(void) const { return m_propertyWindow; }

  inline virtual void SetManagedWindow(wxWindow *win) { m_managedWindow = win; }
  inline virtual wxWindow *GetManagedWindow(void) const { return m_managedWindow; }

  inline virtual wxButton *GetWindowCloseButton() const { return m_windowCloseButton; }
  inline virtual wxButton *GetWindowCancelButton() const { return m_windowCancelButton; }
  inline virtual wxButton *GetHelpButton() const { return m_windowHelpButton; }

public:
  static bool sm_dialogCancelled;

 protected:
  bool              m_detailedEditing;     // E.g. using listbox for choices

  wxWindow*         m_propertyWindow; // Panel that the controls will appear on
  wxWindow*         m_managedWindow; // Frame or dialog
  
  wxButton*         m_windowCloseButton; // Or OK
  wxButton*         m_windowCancelButton;
  wxButton*         m_windowHelpButton;

DECLARE_EVENT_TABLE()

};
 
/*
 * The type of validator used for forms (wxForm style but using an existing panel
 * or dialog box).
 * Classes derived from this know how to map from whatever widget they
 * find themselves paired with, to the wxProperty and vice versa.
 * Should the widget pointer be stored with the validator, or
 * the wxProperty? If with the property, we don't have to supply
 * a validator for every property. Otherwise, there ALWAYS needs
 * to be a validator. On the other hand, not storing a wxWindow pointer
 * in the wxProperty is more elegant. Perhaps.
 * I think on balance, should put wxWindow pointer into wxProperty.
 * After all, wxProperty will often be used to represent the data
 * assocated with a window. It's that kinda thing.
 */

class WXDLLEXPORT wxPropertyFormValidator: public wxPropertyValidator
{
  DECLARE_DYNAMIC_CLASS(wxPropertyFormValidator)
 protected:
 public:
  wxPropertyFormValidator(long flags = 0): wxPropertyValidator(flags) { }
  ~wxPropertyFormValidator(void) {}
  
   // Called to check value is OK (e.g. when OK is pressed)
   // Return FALSE if value didn't check out; signal to restore old value.
   virtual bool OnCheckValue( wxProperty *WXUNUSED(property), wxPropertyFormView *WXUNUSED(view), 
     wxWindow *WXUNUSED(parentWindow) ) { return TRUE; }

   // Does the transferance from the property editing area to the property itself.
   // Called by the view, e.g. when closing the window.
   virtual bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow) = 0;

   // Called by the view to transfer the property to the window.
   virtual bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow) = 0;

  virtual void OnDoubleClick( wxProperty *WXUNUSED(property), wxPropertyFormView *WXUNUSED(view), 
     wxWindow *WXUNUSED(parentWindow) ) { } 
  virtual void OnSetFocus( wxProperty *WXUNUSED(property), wxPropertyFormView *WXUNUSED(view), 
     wxWindow *WXUNUSED(parentWindow) ) { }
  virtual void OnKillFocus( wxProperty *WXUNUSED(property), wxPropertyFormView *WXUNUSED(view), 
     wxWindow *WXUNUSED(parentWindow) ) { }
  virtual void OnCommand( wxProperty *WXUNUSED(property), wxPropertyFormView *WXUNUSED(view), 
     wxWindow *WXUNUSED(parentWindow), wxCommandEvent& WXUNUSED(event) ) {}
};

/*
 * Some default validators
 */
 
class WXDLLEXPORT wxRealFormValidator: public wxPropertyFormValidator
{
  DECLARE_DYNAMIC_CLASS(wxRealFormValidator)
 public:
   // 0.0, 0.0 means no range
   wxRealFormValidator(float min = 0.0, float max = 0.0, long flags = 0):wxPropertyFormValidator(flags)
   {
     m_realMin = min; m_realMax = max;
   }
   ~wxRealFormValidator(void) {}

   bool OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   // Called by the view to transfer the property to the window.
   bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);

 protected:
  float         m_realMin;
  float         m_realMax;
};

class WXDLLEXPORT wxIntegerFormValidator: public wxPropertyFormValidator
{
  DECLARE_DYNAMIC_CLASS(wxIntegerFormValidator)
 public:
   // 0, 0 means no range
   wxIntegerFormValidator(long min = 0, long max = 0, long flags = 0):wxPropertyFormValidator(flags)
   {
     m_integerMin = min; m_integerMax = max;
   }
   ~wxIntegerFormValidator(void) {}

   bool OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);

 protected:
  long          m_integerMin;
  long          m_integerMax;
};

class WXDLLEXPORT wxBoolFormValidator: public wxPropertyFormValidator
{
  DECLARE_DYNAMIC_CLASS(wxBoolFormValidator)
 protected:
 public:
   wxBoolFormValidator(long flags = 0):wxPropertyFormValidator(flags)
   {
   }
   ~wxBoolFormValidator(void) {}

   bool OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
};

class WXDLLEXPORT wxStringFormValidator: public wxPropertyFormValidator
{
  DECLARE_DYNAMIC_CLASS(wxStringFormValidator)
 public:
   wxStringFormValidator(wxStringList *list = NULL, long flags = 0);

   ~wxStringFormValidator(void)
   {
     if (m_strings)
       delete m_strings;
   }

   bool OnCheckValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   bool OnRetrieveValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);
   bool OnDisplayValue(wxProperty *property, wxPropertyFormView *view, wxWindow *parentWindow);

 protected:
  wxStringList*     m_strings;
};

/*
 * A default dialog box class to use.
 */
 
class WXDLLEXPORT wxPropertyFormDialog: public wxDialog
{
  DECLARE_CLASS(wxPropertyFormDialog)
 public:
  wxPropertyFormDialog(wxPropertyFormView *v, wxWindow *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = "dialogBox");

  void OnCloseWindow(wxCloseEvent& event);
  void OnDefaultAction(wxControl *item);
  void OnCommand(wxWindow& win, wxCommandEvent& event);

  // Extend event processing to search the view's event table
  virtual bool ProcessEvent(wxEvent& event);

 private:
  wxPropertyFormView*       m_view;

DECLARE_EVENT_TABLE()
};

/*
 * A default panel class to use.
 */
 
class WXDLLEXPORT wxPropertyFormPanel: public wxPanel
{
  DECLARE_CLASS(wxPropertyFormPanel)
 public:
  wxPropertyFormPanel(wxPropertyFormView *v, wxWindow *parent, const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "panel"):
     wxPanel(parent, -1, pos, size, style, name)
  {
    m_view = v;
  }
  void OnDefaultAction(wxControl *item);
  void OnCommand(wxWindow& win, wxCommandEvent& event);

  // Extend event processing to search the view's event table
  virtual bool ProcessEvent(wxEvent& event);

 private:
  wxPropertyFormView*       m_view;
};

/*
 * A default frame class to use.
 */
 
class WXDLLEXPORT wxPropertyFormFrame: public wxFrame
{
  DECLARE_CLASS(wxPropertyFormFrame)
 public:
  wxPropertyFormFrame(wxPropertyFormView *v, wxFrame *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame"):
     wxFrame(parent, -1, title, pos, size, style, name)
  {
    m_view = v;
    m_propertyPanel = NULL;
  }
  void OnCloseWindow(wxCloseEvent& event);

  // Must call this to create panel and associate view
  virtual bool Initialize(void);
  virtual wxPanel *OnCreatePanel(wxFrame *parent, wxPropertyFormView *v);
  inline virtual wxPanel *GetPropertyPanel(void) const { return m_propertyPanel; }

 private:
  wxPropertyFormView*       m_view;
  wxPanel*                  m_propertyPanel;

DECLARE_EVENT_TABLE()
};

#endif
  // _WX_PROPFORM_H_
