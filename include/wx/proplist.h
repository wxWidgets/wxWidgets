/////////////////////////////////////////////////////////////////////////////
// Name:        proplist.h
// Purpose:     Property list classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

 /*

 TO DO:

 (1) Optional popup-help for each item, and an optional Help button
   for dialog.

 (2) Align Ok, Cancel, Help buttons properly.

 (3) Consider retrieving the rectangle on the panel that can be
 drawn into (where the value listbox is) and giving an example
 of editing graphically. May be too fancy.

 (4) Deriveable types for wxPropertyValue => may need to reorganise
 wxPropertyValue to use inheritance rather than present all-types-in-one
 scheme.

 (5) Optional popup panel for value list, perhaps.

 (6) Floating point checking routine still crashes with Floating
 point error for zany input.

 (7) Property sheet with choice (or listbox) to select alternative
 sheets... multiple views per panel, only one active. For this
 we really need a wxChoice that can be dynamically set: XView
 may be a problem; Motif?

 (8) More example validators, e.g. colour selector.
  */

#ifndef _WX_PROPLIST_H_
#define _WX_PROPLIST_H_

#ifdef __GNUG__
#pragma interface "proplist.h"
#endif

#include "wx/prop.h"

#define wxPROP_BUTTON_CLOSE       1
#define wxPROP_BUTTON_OK          2
#define wxPROP_BUTTON_CANCEL      4
#define wxPROP_BUTTON_CHECK_CROSS 8
#define wxPROP_BUTTON_HELP        16
#define wxPROP_DYNAMIC_VALUE_FIELD 32
#define wxPROP_PULLDOWN           64
#define wxPROP_SHOWVALUES         128

// Show OK/Cancel buttons on X-based systems where window management is
// more awkward
#if defined(__WXMOTIF__) || defined(__WXGTK__)
#define wxPROP_BUTTON_DEFAULT wxPROP_BUTTON_OK | wxPROP_BUTTON_CANCEL | wxPROP_BUTTON_CHECK_CROSS | wxPROP_PULLDOWN
#else
#define wxPROP_BUTTON_DEFAULT wxPROP_BUTTON_CHECK_CROSS | wxPROP_PULLDOWN | wxPROP_SHOWVALUES
#endif

#define wxID_PROP_CROSS     3000
#define wxID_PROP_CHECK     3001
#define wxID_PROP_EDIT      3002
#define wxID_PROP_TEXT      3003
#define wxID_PROP_SELECT    3004
#define wxID_PROP_VALUE_SELECT  3005

// Mediates between a physical panel and the property sheet
class wxPropertyListView: public wxPropertyView
{
 DECLARE_DYNAMIC_CLASS(wxPropertyListView)
 public:
  wxPropertyListView(wxPanel *propPanel = NULL, long flags = wxPROP_BUTTON_DEFAULT);
  ~wxPropertyListView(void);

  // Associates and shows the view
  virtual void ShowView(wxPropertySheet *propertySheet, wxPanel *panel);

  // Update this view of the viewed object, called e.g. by
  // the object itself.
  virtual bool OnUpdateView(void);

  wxString MakeNameValueString(wxString name, wxString value);

  // Update a single line in the list of properties
  virtual bool UpdatePropertyDisplayInList(wxProperty *property);

  // Update the whole list
  virtual bool UpdatePropertyList(bool clearEditArea = TRUE);

  // Find the wxListBox index corresponding to this property
  virtual int FindListIndexForProperty(wxProperty *property);

  // Select and show string representation in editor the given
  // property. NULL resets to show no property.
  virtual bool ShowProperty(wxProperty *property, bool select = TRUE);
  virtual bool EditProperty(wxProperty *property);

  // Update the display from the property
  virtual bool DisplayProperty(wxProperty *property);
  // Update the property from the display
  virtual bool RetrieveProperty(wxProperty *property);

  // Find appropriate validator and load property into value controls
  virtual bool BeginShowingProperty(wxProperty *property);
  // Find appropriate validator and unload property from value controls
  virtual bool EndShowingProperty(wxProperty *property);

  // Begin detailed editing (e.g. using value listbox)
  virtual void BeginDetailedEditing(void);

  // End detailed editing (e.g. using value listbox)
  virtual void EndDetailedEditing(void);

  // Called by the property listbox
  void OnPropertySelect(wxCommandEvent& event);

  // Called by the value listbox
  void OnValueListSelect(wxCommandEvent& event);

  virtual bool CreateControls(void);
  virtual void ShowTextControl(bool show);
  virtual void ShowListBoxControl(bool show);
  virtual void EnableCheck(bool show);
  virtual void EnableCross(bool show);

  void OnOk(wxCommandEvent& event);
  void OnCancel(wxCommandEvent& event);
  void OnHelp(wxCommandEvent& event);
  void OnPropertyDoubleClick(wxCommandEvent& event);
//  virtual void OnDoubleClick(void);

  void OnCheck(wxCommandEvent& event);
  void OnCross(wxCommandEvent& event);
  void OnEdit(wxCommandEvent& event);
  void OnText(wxCommandEvent& event);

  inline virtual wxListBox *GetPropertyScrollingList() const { return m_propertyScrollingList; }
  inline virtual wxListBox *GetValueList() const { return m_valueList; }
  inline virtual wxTextCtrl *GetValueText() const { return m_valueText; }
  inline virtual wxButton *GetConfirmButton() const { return m_confirmButton; }
  inline virtual wxButton *GetCancelButton() const { return m_cancelButton; }
  inline virtual wxButton *GetEditButton() const { return m_editButton; }
  inline virtual bool GetDetailedEditing(void) const { return m_detailedEditing; }

  inline virtual void AssociatePanel(wxPanel *win) { m_propertyWindow = win; }
  inline virtual wxPanel *GetPanel(void) const { return m_propertyWindow; }

  inline virtual void SetManagedWindow(wxWindow *win) { m_managedWindow = win; }
  inline virtual wxWindow *GetManagedWindow(void) const { return m_managedWindow; }

  inline virtual wxButton *GetWindowCloseButton() const { return m_windowCloseButton; }
  inline virtual wxButton *GetWindowCancelButton() const { return m_windowCancelButton; }
  inline virtual wxButton *GetHelpButton() const { return m_windowHelpButton; }
  
  bool OnClose(void);

public:
  static bool       sm_dialogCancelled;

 protected:
  wxListBox*        m_propertyScrollingList;
  wxListBox*        m_valueList;     // Should really be a combobox, but we don't have one.
  wxTextCtrl*       m_valueText;
  wxButton*         m_confirmButton;  // A tick, as in VB
  wxButton*         m_cancelButton;   // A cross, as in VB
  wxButton*         m_editButton;     // Invokes the custom validator, if any

  bool              m_detailedEditing;     // E.g. using listbox for choices

  static wxBitmap*  sm_tickBitmap;
  static wxBitmap*  sm_crossBitmap;
  
  wxPanel*          m_propertyWindow; // Panel that the controls will appear on
  wxWindow*         m_managedWindow; // Frame or dialog
  
  wxButton*         m_windowCloseButton; // Or OK
  wxButton*         m_windowCancelButton;
  wxButton*         m_windowHelpButton;

DECLARE_EVENT_TABLE()
};

class wxPropertyTextEdit: public wxTextCtrl
{
 DECLARE_CLASS(wxPropertyTextEdit)
 public:
  wxPropertyTextEdit(wxPropertyListView *v, wxWindow *parent, const wxWindowID id,
    const wxString& value, const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "text");
  void OnSetFocus(void);
  void OnKillFocus(void);

  wxPropertyListView*   m_view;
};

#define wxPROP_ALLOW_TEXT_EDITING           1

/*
 * The type of validator used for property lists (Visual Basic style)
 */
 
class wxPropertyListValidator: public wxPropertyValidator
{
  DECLARE_DYNAMIC_CLASS(wxPropertyListValidator)
 protected:
 public:
   wxPropertyListValidator(long flags = wxPROP_ALLOW_TEXT_EDITING): wxPropertyValidator(flags) { }
   ~wxPropertyListValidator(void) {}

   // Called when the property is selected or deselected: typically displays the value
   // in the edit control (having chosen a suitable control to display: (non)editable text or listbox)
   virtual bool OnSelect(bool select, wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property is double clicked. Extra functionality can be provided, such as
   // cycling through possible values.
   inline virtual bool OnDoubleClick( 
     wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow) )
     { return TRUE; }

   // Called when the value listbox is selected. Default behaviour is to copy
   // string to text control, and retrieve the value into the property.
   virtual bool OnValueListSelect(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property value is edited using standard text control
   inline virtual bool OnPrepareControls(
     wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow) )
     { return TRUE; }

   virtual bool OnClearControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

      // Called when the property is edited in detail
   inline virtual bool OnPrepareDetailControls(
     wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow) )
     { return TRUE; }

   // Called if focus lost, IF we're in a modeless property editing situation.
   inline virtual bool OnClearDetailControls(
     wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow) )
     { return TRUE; }

   // Called when the edit (...) button is pressed. The default implementation
   // calls view->BeginDetailedEditing; the filename validator (for example) overrides
   // this function to show the file selector.
   virtual void OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   inline virtual bool OnCheckValue(
     wxProperty *WXUNUSED(property), wxPropertyListView *WXUNUSED(view), wxWindow *WXUNUSED(parentWindow) )
     { return TRUE; }

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transferance from the property editing area to the property itself
   virtual bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   virtual bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

/*
 * A default dialog box class to use.
 */
 
class wxPropertyListDialog: public wxDialog
{
  DECLARE_CLASS(wxPropertyListDialog)
 public:
  wxPropertyListDialog(wxPropertyListView *v, wxWindow *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = "dialogBox");
  bool OnClose(void);
  void OnDefaultAction(wxControl *item);
  void OnCancel(wxCommandEvent& event);

  // Extend event processing to search the view's event table
  virtual bool ProcessEvent(wxEvent& event);

 private:
  wxPropertyListView*   m_view;

DECLARE_EVENT_TABLE()
};

/*
 * A default panel class to use.
 */
 
class wxPropertyListPanel: public wxPanel
{
  DECLARE_CLASS(wxPropertyListPanel)
 public:
  wxPropertyListPanel(wxPropertyListView *v, wxWindow *parent, const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = 0, const wxString& name = "panel"):
     wxPanel(parent, -1, pos, size, style, name)
  {
    m_view = v;
  }
  ~wxPropertyListPanel();
  void OnDefaultAction(wxControl *item);

  inline void SetView(wxPropertyListView* v) { m_view = v; }
  inline wxPropertyListView* GetView() const { return m_view; }

  // Extend event processing to search the view's event table
  virtual bool ProcessEvent(wxEvent& event);

  // Call Layout()
  void OnSize(wxSizeEvent& event);

 private:
  wxPropertyListView*   m_view;

DECLARE_EVENT_TABLE()
};

/*
 * A default frame class to use.
 */
 
class wxPropertyListFrame: public wxFrame
{
  DECLARE_CLASS(wxPropertyListFrame)
 public:
  wxPropertyListFrame(wxPropertyListView *v, wxFrame *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame"):
     wxFrame(parent, -1, title, pos, size, style, name)
  {
    m_view = v;
    m_propertyPanel = NULL;
  }
  bool OnClose(void);

  // Must call this to create panel and associate view
  virtual bool Initialize(void);
  virtual wxPropertyListPanel *OnCreatePanel(wxFrame *parent, wxPropertyListView *v);
  inline virtual wxPropertyListPanel *GetPropertyPanel(void) const { return m_propertyPanel; }

 private:
  wxPropertyListView*       m_view;
  wxPropertyListPanel*      m_propertyPanel;
};

/*
 * Some default validators
 */
 
class wxRealListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxRealListValidator)
 public:
   // 0.0, 0.0 means no range
   wxRealListValidator(float min = 0.0, float max = 0.0, long flags = wxPROP_ALLOW_TEXT_EDITING):wxPropertyListValidator(flags)
   {
     m_realMin = min; m_realMax = max;
   }
   ~wxRealListValidator(void) {}

   bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

 protected:
  float     m_realMin;
  float     m_realMax;
};

class wxIntegerListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxIntegerListValidator)
 public:
   // 0, 0 means no range
   wxIntegerListValidator(long min = 0, long max = 0, long flags = wxPROP_ALLOW_TEXT_EDITING):wxPropertyListValidator(flags)
   {
     m_integerMin = min; m_integerMax = max;
   }
   ~wxIntegerListValidator(void) {}

   bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

 protected:
  long m_integerMin;
  long m_integerMax;
};

class wxBoolListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxBoolListValidator)
 protected:
 public:
   wxBoolListValidator(long flags = 0):wxPropertyListValidator(flags)
   {
   }
   ~wxBoolListValidator(void) {}

   bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnPrepareDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnClearDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property is double clicked. Extra functionality can be provided,
   // cycling through possible values.
   virtual bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

class wxStringListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxStringListValidator)
 public:
   wxStringListValidator(wxStringList *list = NULL, long flags = 0);

   ~wxStringListValidator(void)
   {
     if (m_strings)
       delete m_strings;
   }

   bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnPrepareDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnClearDetailControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property is double clicked. Extra functionality can be provided,
   // cycling through possible values.
   bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

 protected:
  wxStringList*     m_strings;
};
 
class wxFilenameListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxFilenameListValidator)
 public:
   wxFilenameListValidator(wxString message = "Select a file", wxString wildcard = "*.*", long flags = 0);

   ~wxFilenameListValidator(void);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transferance from the property editing area to the property itself
   bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the edit (...) button is pressed.
   void OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

 protected:
  wxString  m_filenameWildCard;
  wxString  m_filenameMessage;
  
};

class wxColourListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxColourListValidator)
 protected:
 public:
   wxColourListValidator(long flags = 0);

   ~wxColourListValidator(void);

   bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the edit (...) button is pressed.
   void OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

class wxListOfStringsListValidator: public wxPropertyListValidator
{
  DECLARE_DYNAMIC_CLASS(wxListOfStringsListValidator)
 protected:
 public:
   wxListOfStringsListValidator(long flags = 0);

   ~wxListOfStringsListValidator(void)
   {
   }

   bool OnPrepareControls(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost.
   // Return FALSE if value didn't check out; signal to restore old value.
   bool OnCheckValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when TICK is pressed or focus is lost or view wants to update
   // the property list.
   // Does the transfer from the property editing area to the property itself
   bool OnRetrieveValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
   bool OnDisplayValue(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   // Called when the property is double clicked.
   bool OnDoubleClick(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);

   bool EditStringList(wxWindow *parent, wxStringList *stringList, const char *title = "String List Editor");

   // Called when the edit (...) button is pressed.
   void OnEdit(wxProperty *property, wxPropertyListView *view, wxWindow *parentWindow);
};

#endif
  // _WX_PROPLIST_H_
