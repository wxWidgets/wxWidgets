/////////////////////////////////////////////////////////////////////////////
// Name:        prop.h
// Purpose:     Property sheet classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROP_H_
#define _WX_PROP_H_

#ifdef __GNUG__
#pragma interface "prop.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/hash.h"
#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/button.h"
#include "wx/listbox.h"
#include "wx/textctrl.h"
#include "wx/gdicmn.h"
#include "wx/layout.h"

class wxWindow;
class wxProperty;
class wxPropertyValue;
class wxPropertySheet;
class wxPropertyView;
class wxPropertyValidator;
class wxPropertyValidatorRegistry;

#define wxPROPERTY_VERSION 2.0

// A storable sheet of values
class WXDLLEXPORT wxPropertySheet: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxPropertySheet)
 public:
  wxPropertySheet(void);
  ~wxPropertySheet(void);

  // Add a property
  virtual void AddProperty(wxProperty *property);

  // Get property by name
  virtual wxProperty *GetProperty(wxString name);

  // Clear all properties
  virtual void Clear(void);

  virtual bool Save(ostream& str);
  virtual bool Load(ostream& str);

  virtual void UpdateAllViews(wxPropertyView *thisView = NULL);
  inline virtual wxList& GetProperties(void) const { return (wxList&) m_properties; }
  
  // Sets/clears the modified flag for each property value
  virtual void SetAllModified(bool flag = TRUE);

 protected:
  wxObject*         m_viewedObject;
  wxList            m_properties;
  wxPropertyView*   m_propertyView;

};


// Base class for property sheet views. There are currently two directly derived
// classes: wxPropertyListView, and wxPropertyFormView.
class WXDLLEXPORT wxPropertyView: public wxEvtHandler
{
 DECLARE_DYNAMIC_CLASS(wxPropertyView)
 public:
  wxPropertyView(long flags = 0);
  ~wxPropertyView(void);

  // Associates and shows the view
  virtual void ShowView(wxPropertySheet *WXUNUSED(propertySheet), wxWindow *WXUNUSED(panel)) {}

  // Update this view of the viewed object, called e.g. by
  // the object itself.
  virtual bool OnUpdateView(void) {return FALSE;};

  // Override this to do something as soon as the property changed,
  // if the view and validators support it.
  virtual void OnPropertyChanged(wxProperty *WXUNUSED(property)) {}

  virtual void AddRegistry(wxPropertyValidatorRegistry *registry);
  inline virtual wxList& GetRegistryList(void) const
   { return (wxList&) m_validatorRegistryList; }

  virtual wxPropertyValidator *FindPropertyValidator(wxProperty *property);
  inline virtual void SetPropertySheet(wxPropertySheet *sheet) { m_propertySheet = sheet; }
  inline virtual wxPropertySheet *GetPropertySheet(void) const { return m_propertySheet; }

  virtual void OnOk(void) {};
  virtual void OnCancel(void) {};
  virtual void OnHelp(void) {};

  inline virtual bool OnClose(void) { return FALSE; }
  inline long GetFlags(void) { return m_buttonFlags; }

 protected:
  long                  m_buttonFlags;
  wxPropertySheet*      m_propertySheet;
  wxProperty*           m_currentProperty;
  wxList                m_validatorRegistryList;
  wxPropertyValidator*  m_currentValidator;
};


class WXDLLEXPORT wxPropertyValidator: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxPropertyValidator)
 public:
  wxPropertyValidator(long flags = 0);
  ~wxPropertyValidator(void);

  inline long GetFlags(void) const { return m_validatorFlags; }
  inline void SetValidatorProperty(wxProperty *prop) { m_validatorProperty = prop; }
  inline wxProperty *GetValidatorProperty(void) const { return m_validatorProperty; }

  virtual bool StringToFloat (char *s, float *number);
  virtual bool StringToDouble (char *s, double *number);
  virtual bool StringToInt (char *s, int *number);
  virtual bool StringToLong (char *s, long *number);
  virtual char *FloatToString (float number);
  virtual char *DoubleToString (double number);
  virtual char *IntToString (int number);
  virtual char *LongToString (long number);

 protected:
  long          m_validatorFlags;
  wxProperty*   m_validatorProperty;
};


// extern wxPropertyValidator *wxDefaultPropertyValidator;

class WXDLLEXPORT wxPropertyValidatorRegistry: public wxHashTable
{
  DECLARE_DYNAMIC_CLASS(wxPropertyValidatorRegistry)
 public:
  wxPropertyValidatorRegistry(void);
  ~wxPropertyValidatorRegistry(void);

  virtual void RegisterValidator(const wxString& roleName, wxPropertyValidator *validator);
  virtual wxPropertyValidator *GetValidator(const wxString& roleName);
  void ClearRegistry(void);
};

/*
 * Property value class
 */

typedef enum {
    wxPropertyValueNull,
    wxPropertyValueInteger,
    wxPropertyValueReal,
    wxPropertyValuebool,
    wxPropertyValueString,
    wxPropertyValueList,
    wxPropertyValueIntegerPtr,
    wxPropertyValueRealPtr,
    wxPropertyValueboolPtr,
    wxPropertyValueStringPtr
} wxPropertyValueType;

class WXDLLEXPORT wxPropertyValue: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPropertyValue)

  wxPropertyValue(void);                       // Unknown type
  wxPropertyValue(const wxPropertyValue& copyFrom);  // Copy constructor
  wxPropertyValue(const char *val);
  wxPropertyValue(const wxString& val);
  wxPropertyValue(long val);
  wxPropertyValue(bool val);
  wxPropertyValue(float val);
  wxPropertyValue(double the_real);
  wxPropertyValue(wxList *val);
  wxPropertyValue(wxStringList *val);
  // Pointer versions
  wxPropertyValue(char **val);
  wxPropertyValue(long *val);
  wxPropertyValue(bool *val);
  wxPropertyValue(float *val);

  ~wxPropertyValue(void);

  virtual inline wxPropertyValueType Type(void) const { return m_type; }
  virtual inline void SetType(wxPropertyValueType typ) { m_type = typ; }
  virtual long IntegerValue(void) const;
  virtual float RealValue(void) const;
  virtual bool BoolValue(void) const;
  virtual char *StringValue(void) const;
  virtual long *IntegerValuePtr(void) const;
  virtual float *RealValuePtr(void) const;
  virtual bool *BoolValuePtr(void) const;
  virtual char **StringValuePtr(void) const;

  // Get nth arg of clause (starting from 1)
  virtual wxPropertyValue *Arg(wxPropertyValueType type, int arg) const;

  // Return nth argument of a list expression (starting from zero)
  virtual wxPropertyValue *Nth(int arg) const;
  // Returns the number of elements in a list expression
  virtual int Number(void) const;

  virtual wxPropertyValue *NewCopy(void) const;
  virtual void Copy(wxPropertyValue& copyFrom);

  virtual void WritePropertyClause(ostream& stream);  // Write this expression as a top-level clause
  virtual void WritePropertyType(ostream& stream);    // Write as any other subexpression

  // Append an expression to a list
  virtual void Append(wxPropertyValue *expr);
  // Insert at beginning of list
  virtual void Insert(wxPropertyValue *expr);

  // Get first expr in list
  virtual inline wxPropertyValue *GetFirst(void) const
    { return ((m_type == wxPropertyValueList) ? m_value.first : (wxPropertyValue*)NULL); }

  // Get next expr if this is a node in a list
  virtual inline wxPropertyValue *GetNext(void) const
    { return m_next; }

  // Get last expr in list
  virtual inline wxPropertyValue *GetLast(void) const
    { return ((m_type == wxPropertyValueList) ? m_last : (wxPropertyValue*)NULL); }
  
  // Delete this node from the list
  virtual void Delete(wxPropertyValue *node);

  // Clear list
  virtual void ClearList(void);

  virtual inline void SetClientData(wxObject *data) { m_clientData = data; }
  virtual inline wxObject *GetClientData(void) { return m_clientData; }

  virtual wxString GetStringRepresentation(void);
  
  inline void SetModified(bool flag = TRUE) { m_modifiedFlag = flag; }
  inline bool GetModified(void) { return m_modifiedFlag; }

  // Operators
  void operator=(const wxPropertyValue& val);
//  void operator=(const char *val);
  void operator=(const wxString& val);
  void operator=(const long val);
  void operator=(const bool val);
  void operator=(const float val);
  void operator=(const char **val);
  void operator=(const long *val);
  void operator=(const bool *val);
  void operator=(const float *val);

 public:
  wxObject*             m_clientData;
  wxPropertyValueType   m_type;
  bool                  m_modifiedFlag;

  union {
    long integer; // Also doubles as bool
    char *string;
    float real;
    long *integerPtr;
    bool *boolPtr;
    char **stringPtr;
    float *realPtr;
    wxPropertyValue *first;  // If is a list expr, points to the first node
    } m_value;

  wxPropertyValue*      m_next;     // If this is a node in a list, points to the next node
  wxPropertyValue*      m_last;     // If is a list expr, points to the last node

};

/*
 * Property class: contains a name and a value.
 */

class WXDLLEXPORT wxProperty: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxProperty)
 protected:
  bool                  m_enabled;
 public:
  wxPropertyValue       m_value;
  wxString              m_name;
  wxString              m_propertyRole;
  wxPropertyValidator*  m_propertyValidator;
  wxWindow*             m_propertyWindow; // Usually a panel item, if anything

  wxProperty(void);
  wxProperty(wxProperty& copyFrom);
  wxProperty(wxString name, wxString role, wxPropertyValidator *ed = NULL);
  wxProperty(wxString name, const wxPropertyValue& val, wxString role, wxPropertyValidator *ed = NULL);
  ~wxProperty(void);

  virtual wxPropertyValue& GetValue(void) const;
  virtual wxPropertyValidator *GetValidator(void) const;
  virtual wxString& GetName(void) const;
  virtual wxString& GetRole(void) const;
  virtual void SetValue(const wxPropertyValue& val);
  virtual void SetValidator(wxPropertyValidator *v);
  virtual void SetName(wxString& nm);
  virtual void SetRole(wxString& role);
  void operator=(const wxPropertyValue& val);
  virtual inline void SetWindow(wxWindow *win) { m_propertyWindow = win; }
  virtual inline wxWindow *GetWindow(void) const { return m_propertyWindow; }
  
  inline void Enable(bool en) { m_enabled = en; }
  inline bool IsEnabled(void) const { return m_enabled; }
};

#endif
  // _WX_PROP_H_
