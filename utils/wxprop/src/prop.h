/////////////////////////////////////////////////////////////////////////////
// Name:        prop.h
// Purpose:     Property sheet classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _PROP_H_
#define _PROP_H_

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
class wxPropertySheet: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxPropertySheet)
 protected:
  wxObject *viewedObject;
  wxList properties;
  wxPropertyView *propertyView;

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
  inline virtual wxList& GetProperties(void) { return properties; }
  
  // Sets/clears the modified flag for each property value
  virtual void SetAllModified(bool flag = TRUE);
};


// Base class for property sheet views. There are currently two directly derived
// classes: wxPropertyListView, and wxPropertyFormView.
class wxPropertyView: public wxEvtHandler
{
 DECLARE_DYNAMIC_CLASS(wxPropertyView)
 protected:
  long buttonFlags;
  wxPropertySheet *propertySheet;
  wxProperty *currentProperty;
  wxList validatorRegistryList;
  wxPropertyValidator *currentValidator;
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
  inline virtual wxList& GetRegistryList(void)
   { return validatorRegistryList; }

  virtual wxPropertyValidator *FindPropertyValidator(wxProperty *property);
  inline virtual void SetPropertySheet(wxPropertySheet *sheet) { propertySheet = sheet; }
  inline virtual wxPropertySheet *GetPropertySheet(void) { return propertySheet; }

  virtual void OnOk(void) {};
  virtual void OnCancel(void) {};
  virtual void OnHelp(void) {};

  inline virtual bool OnClose(void) { return FALSE; }
  inline long GetFlags(void) { return buttonFlags; }
};


class wxPropertyValidator: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxPropertyValidator)
 protected:
  long validatorFlags;
  wxProperty *validatorProperty;
 public:
  wxPropertyValidator(long flags = 0);
  ~wxPropertyValidator(void);

  inline long GetFlags(void) { return validatorFlags; }
  inline void SetValidatorProperty(wxProperty *prop) { validatorProperty = prop; }
  inline wxProperty *GetValidatorProperty(void) { return validatorProperty; }

  virtual bool StringToFloat (char *s, float *number);
  virtual bool StringToDouble (char *s, double *number);
  virtual bool StringToInt (char *s, int *number);
  virtual bool StringToLong (char *s, long *number);
  virtual char *FloatToString (float number);
  virtual char *DoubleToString (double number);
  virtual char *IntToString (int number);
  virtual char *LongToString (long number);
};


// extern wxPropertyValidator *wxDefaultPropertyValidator;

class wxPropertyValidatorRegistry: public wxHashTable
{
  DECLARE_DYNAMIC_CLASS(wxPropertyValidatorRegistry)
 public:
  wxPropertyValidatorRegistry(void);
  ~wxPropertyValidatorRegistry(void);

  virtual void RegisterValidator(wxString& roleName, wxPropertyValidator *validator);
  virtual wxPropertyValidator *GetValidator(wxString& roleName);
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

class wxPropertyValue: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPropertyValue)
 public:
  wxObject *client_data;
  wxPropertyValueType type;
  bool modifiedFlag;

  union {
    long integer; // Also doubles as bool
    char *string;
    float real;
    long *integerPtr;
    bool *boolPtr;
    char **stringPtr;
    float *realPtr;
    wxPropertyValue *first;  // If is a list expr, points to the first node
    } value;

  wxPropertyValue *next;     // If this is a node in a list, points to the next node
  wxPropertyValue *last;     // If is a list expr, points to the last node

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

  virtual inline wxPropertyValueType Type(void) { return type; }
  virtual inline void SetType(wxPropertyValueType typ) { type = typ; }
  virtual long IntegerValue(void);
  virtual float RealValue(void);
  virtual bool BoolValue(void);
  virtual char *StringValue(void);
  virtual long *IntegerValuePtr(void);
  virtual float *RealValuePtr(void);
  virtual bool *BoolValuePtr(void);
  virtual char **StringValuePtr(void);

  // Get nth arg of clause (starting from 1)
  virtual wxPropertyValue *Arg(wxPropertyValueType type, int arg);

  // Return nth argument of a list expression (starting from zero)
  virtual wxPropertyValue *Nth(int arg);
  // Returns the number of elements in a list expression
  virtual int Number(void);

  virtual wxPropertyValue *NewCopy(void);
  virtual void Copy(wxPropertyValue& copyFrom);

  virtual void WritePropertyClause(ostream& stream);  // Write this expression as a top-level clause
  virtual void WritePropertyType(ostream& stream);    // Write as any other subexpression

  // Append an expression to a list
  virtual void Append(wxPropertyValue *expr);
  // Insert at beginning of list
  virtual void Insert(wxPropertyValue *expr);

  // Get first expr in list
  virtual inline wxPropertyValue *GetFirst(void) 
    { return ((type == wxPropertyValueList) ? value.first : (wxPropertyValue*)NULL); }

  // Get next expr if this is a node in a list
  virtual inline wxPropertyValue *GetNext(void) 
    { return next; }

  // Get last expr in list
  virtual inline wxPropertyValue *GetLast(void) 
    { return ((type == wxPropertyValueList) ? last : (wxPropertyValue*)NULL); }
  
  // Delete this node from the list
  virtual void Delete(wxPropertyValue *node);

  // Clear list
  virtual void ClearList(void);

  virtual inline void SetClientData(wxObject *data) { client_data = data; }
  virtual inline wxObject *GetClientData(void) { return client_data; }

  virtual wxString GetStringRepresentation(void);
  
  inline void SetModified(bool flag = TRUE) { modifiedFlag = flag; }
  inline bool GetModified(void) { return modifiedFlag; }

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
};

/*
 * Property class: contains a name and a value.
 */

class wxProperty: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxProperty)
 protected:
  bool enabled;
 public:
  wxPropertyValue value;
  wxString name;
  wxString propertyRole;
  wxPropertyValidator *propertyValidator;
  wxWindow *propertyWindow; // Usually a panel item, if anything

  wxProperty(void);
  wxProperty(wxProperty& copyFrom);
  wxProperty(wxString name, wxString role, wxPropertyValidator *ed = NULL);
  wxProperty(wxString name, const wxPropertyValue& val, wxString role, wxPropertyValidator *ed = NULL);
  ~wxProperty(void);

  virtual wxPropertyValue& GetValue(void);
  virtual wxPropertyValidator *GetValidator(void);
  virtual wxString& GetName(void);
  virtual wxString& GetRole(void);
  virtual void SetValue(const wxPropertyValue& val);
  virtual void SetValidator(wxPropertyValidator *v);
  virtual void SetName(wxString& nm);
  virtual void SetRole(wxString& role);
  void operator=(const wxPropertyValue& val);
  virtual inline void SetWindow(wxWindow *win) { propertyWindow = win; }
  virtual inline wxWindow *GetWindow(void) { return propertyWindow; }
  
  inline void Enable(bool en) { enabled = en; }
  inline bool IsEnabled(void) { return enabled; }
};

#endif

