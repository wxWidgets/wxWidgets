/////////////////////////////////////////////////////////////////////////////
// Name:        prop.cpp
// Purpose:     Propert sheet classes implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "prop.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(__WXMSW__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "wx/window.h"
#include "wx/utils.h"
#include "wx/list.h"
#include "prop.h"

IMPLEMENT_DYNAMIC_CLASS(wxPropertyValue, wxObject)

wxPropertyValue::wxPropertyValue(void)
{
  type = wxPropertyValueNull;
  next = NULL;
  last = NULL;
  value.first = NULL;
  client_data = NULL;
  modifiedFlag = FALSE;
}

wxPropertyValue::wxPropertyValue(const wxPropertyValue& copyFrom)
{
  modifiedFlag = FALSE;
  Copy((wxPropertyValue& )copyFrom);
}

wxPropertyValue::wxPropertyValue(const char *val)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueString;

  value.string = copystring(val);
  client_data = NULL;
  next = NULL;
  last = NULL;
}

wxPropertyValue::wxPropertyValue(const wxString& val)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueString;

  value.string = copystring((const char *)val);
  client_data = NULL;
  next = NULL;
  last = NULL;
}

wxPropertyValue::wxPropertyValue(long the_integer)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueInteger;
  value.integer = the_integer;
  client_data = NULL;
  next = NULL;
}

wxPropertyValue::wxPropertyValue(bool val)
{
  modifiedFlag = FALSE;
  type = wxPropertyValuebool;
  value.integer = val;
  client_data = NULL;
  next = NULL;
}

wxPropertyValue::wxPropertyValue(float the_real)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueReal;
  value.real = the_real;
  client_data = NULL;
  next = NULL;
}

wxPropertyValue::wxPropertyValue(double the_real)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueReal;
  value.real = (float)the_real;
  client_data = NULL;
  next = NULL;
}

// Pointer versions: we have a pointer to the real C++ value.
wxPropertyValue::wxPropertyValue(char **val)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueStringPtr;

  value.stringPtr = val;
  client_data = NULL;
  next = NULL;
  last = NULL;
}

wxPropertyValue::wxPropertyValue(long *val)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueIntegerPtr;
  value.integerPtr = val;
  client_data = NULL;
  next = NULL;
}

wxPropertyValue::wxPropertyValue(bool *val)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueboolPtr;
  value.boolPtr = val;
  client_data = NULL;
  next = NULL;
}

wxPropertyValue::wxPropertyValue(float *val)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueRealPtr;
  value.realPtr = val;
  client_data = NULL;
  next = NULL;
}

wxPropertyValue::wxPropertyValue(wxList *the_list)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueList;
  client_data = NULL;
  last = NULL;
  value.first = NULL;

  wxNode *node = the_list->First();
  while (node)
  {
    wxPropertyValue *expr = (wxPropertyValue *)node->Data();
    Append(expr);
    node = node->Next();
  }

  delete the_list;
}

wxPropertyValue::wxPropertyValue(wxStringList *the_list)
{
  modifiedFlag = FALSE;
  type = wxPropertyValueList;
  client_data = NULL;
  last = NULL;
  value.first = NULL;

  wxNode *node = the_list->First();
  while (node)
  {
    char *s = (char *)node->Data();
    Append(new wxPropertyValue(s));
    node = node->Next();
  }
  delete the_list;
}

wxPropertyValue::~wxPropertyValue(void)
{
  switch (type)
  {
    case wxPropertyValueInteger:
    case wxPropertyValuebool:
    case wxPropertyValueReal:
    {
     break;
    }
   case wxPropertyValueString:
   {
     delete value.string;
     break;
   }
   case wxPropertyValueList:
   {
     wxPropertyValue *expr = value.first;
     while (expr)
     {
       wxPropertyValue *expr1 = expr->next;

       delete expr;
       expr = expr1;
     }
     break;
   }
   default:
   case wxPropertyValueNull: break;
  }
}

void wxPropertyValue::Append(wxPropertyValue *expr)
{
  modifiedFlag = TRUE;
  if (!value.first)
    value.first = expr;

  if (last)
    last->next = expr;
  last = expr;
}

void wxPropertyValue::Insert(wxPropertyValue *expr)
{
  modifiedFlag = TRUE;
  expr->next = value.first;
  value.first = expr;

  if (!last)
    last = expr;
}

// Delete from list
void wxPropertyValue::Delete(wxPropertyValue *node)
{
  wxPropertyValue *expr = GetFirst();
  
  wxPropertyValue *previous = NULL;
  while (expr && (expr != node))
  {
    previous = expr;
    expr = expr->GetNext();
  }

  if (expr)
  {
    if (previous)
      previous->next = expr->next;
      
    // If node was the first in the list,
    // make the list point to the NEXT one.
    if (GetFirst() == expr)
    {
      value.first = expr->next;
    }

    // If node was the last in the list,
    // make the list 'last' pointer point to the PREVIOUS one.
    if (GetLast() == expr)
    {
      if (previous)
        last = previous;
      else
        last = NULL;
    }
    modifiedFlag = TRUE;
    delete expr;
  }

}

void wxPropertyValue::ClearList(void)
{
  wxPropertyValue *val = GetFirst();
  if (val)
    modifiedFlag = TRUE;
  
  while (val)
  {
    wxPropertyValue *next = val->GetNext();
    delete val;
    val = next;
  }
  value.first = NULL;
  last = NULL;
}

wxPropertyValue *wxPropertyValue::NewCopy(void)
{
  switch (type)
  {
    case wxPropertyValueInteger:
      return new wxPropertyValue(value.integer);
    case wxPropertyValuebool:
      return new wxPropertyValue((bool) (value.integer != 0));
    case wxPropertyValueReal:
      return new wxPropertyValue(value.real);
    case wxPropertyValueString:
      return new wxPropertyValue(value.string);
    case wxPropertyValueList:
    {
      wxPropertyValue *expr = value.first;
      wxPropertyValue *new_list = new wxPropertyValue;
      new_list->SetType(wxPropertyValueList);
      while (expr)
      {
        wxPropertyValue *expr2 = expr->NewCopy();
        new_list->Append(expr2);
        expr = expr->next;
      }
      return new_list;
    }
   case wxPropertyValueIntegerPtr:
     return new wxPropertyValue(value.integerPtr);
   case wxPropertyValueRealPtr:
     return new wxPropertyValue(value.realPtr);
   case wxPropertyValueboolPtr:
     return new wxPropertyValue(value.boolPtr);
   case wxPropertyValueStringPtr:
     return new wxPropertyValue(value.stringPtr);

   case wxPropertyValueNull: 
#ifdef __X__
    cerr << "Should never get here!\n";
#endif
    break;
  }
  return NULL;
}

void wxPropertyValue::Copy(wxPropertyValue& copyFrom)
{
  type = copyFrom.Type();
  
  switch (type)
  {
    case wxPropertyValueInteger:
      (*this) = copyFrom.IntegerValue();
      return ;

    case wxPropertyValueReal:
      (*this) = copyFrom.RealValue();
      return ;
      
    case wxPropertyValueString:
      (*this) = wxString(copyFrom.StringValue());
      return ;
      
    case wxPropertyValuebool:
      (*this) = copyFrom.BoolValue();
      return ;

    // Pointers
    case wxPropertyValueboolPtr:
      (*this) = copyFrom.BoolValuePtr();
      return ;
    case wxPropertyValueRealPtr:
      (*this) = copyFrom.RealValuePtr();
      return ;
    case wxPropertyValueIntegerPtr:
      (*this) = copyFrom.IntegerValuePtr();
      return ;
    case wxPropertyValueStringPtr:
    {
      char** s = copyFrom.StringValuePtr();
      (*this) = s;
      return ;
    }
      
    case wxPropertyValueList:
    {
      value.first = NULL;
      next = NULL;
      last = NULL;
      wxPropertyValue *expr = copyFrom.value.first;
      while (expr)
      {
        wxPropertyValue *expr2 = expr->NewCopy();
        Append(expr2);
        expr = expr->next;
      }
      return;
    }
   case wxPropertyValueNull: 
#ifdef __X__
    cerr << "Should never get here!\n";
#endif
    break;
  }
}

// Return nth argument of a clause (starting from 1)
wxPropertyValue *wxPropertyValue::Arg(wxPropertyValueType type, int arg)
{
  wxPropertyValue *expr = value.first;
  for (int i = 1; i < arg; i++)
    if (expr)
      expr = expr->next;

  if (expr && (expr->type == type))
    return expr;
  else
    return NULL;
}

// Return nth argument of a list expression (starting from zero)
wxPropertyValue *wxPropertyValue::Nth(int arg)
{
  if (type != wxPropertyValueList)
    return NULL;

  wxPropertyValue *expr = value.first;
  for (int i = 0; i < arg; i++)
    if (expr)
      expr = expr->next;
    else return NULL;

  if (expr)
    return expr;
  else
    return NULL;
}

  // Returns the number of elements in a list expression
int wxPropertyValue::Number(void)
{
  if (type != wxPropertyValueList)
    return 0;

  int i = 0;
  wxPropertyValue *expr = value.first;
  while (expr)
  {
    expr = expr->next;
    i ++;
  }
  return i;
}

void wxPropertyValue::WritePropertyClause(ostream& stream)  // Write this expression as a top-level clause
{
  if (type != wxPropertyValueList)
    return;

  wxPropertyValue *node = value.first;
  if (node)
  {
    node->WritePropertyType(stream);
    stream << "(";
    node = node->next;
    bool first = TRUE;
    while (node)
    {
      if (!first)
        stream << "  ";
      node->WritePropertyType(stream);
      node = node->next;
      if (node) stream << ",\n";
      first = FALSE;
    }
    stream << ").\n\n";
  }
}

void wxPropertyValue::WritePropertyType(ostream& stream)    // Write as any other subexpression
{
  switch (type)
  {
    case wxPropertyValueInteger:
    {
      stream << value.integer;
      break;
    }
    case wxPropertyValueIntegerPtr:
    {
      stream << *value.integerPtr;
      break;
    }
    case wxPropertyValuebool:
    {
      if (value.integer)
        stream << "True";
      else
        stream << "False";
      break;
    }
    case wxPropertyValueboolPtr:
    {
      if (*value.integerPtr)
        stream << "True";
      else
        stream << "False";
      break;
    }
    case wxPropertyValueReal:
    {
      float f = value.real;
      sprintf(wxBuffer, "%.6g", (double)f);
      stream << wxBuffer;
      break;
    }
    case wxPropertyValueRealPtr:
    {
      float f = *value.realPtr;
/* Now the parser can cope with this.
      // Prevent printing in 'e' notation. Any better way?
      if (fabs(f) < 0.00001)
        f = 0.0;
*/
      sprintf(wxBuffer, "%.6g", f);
      stream << wxBuffer;
      break;
    }
    case wxPropertyValueString:
    {
//      stream << "\"";
      int i;
      int len = strlen(value.string);
      for (i = 0; i < len; i++)
      {
        char ch = value.string[i];
//        if (ch == '"' || ch == '\\')
//          stream << "\\";
        stream << ch;
      }

//      stream << "\"";
      break;
    }
    case wxPropertyValueStringPtr:
    {
      int i;
      int len = strlen(*(value.stringPtr));
      for (i = 0; i < len; i++)
      {
        char ch = *(value.stringPtr)[i];

      }
      break;
    }
    case wxPropertyValueList:
    {
      if (!value.first)
        stream << "[]";
      else
      {
        wxPropertyValue *expr = value.first;

        stream << "[";
        while (expr)
        {
          expr->WritePropertyType(stream);
          expr = expr->next;
          if (expr) stream << ", ";
        }
        stream << "]";
      }
      break;
    }
   case wxPropertyValueNull: break;
  }
}

wxString wxPropertyValue::GetStringRepresentation(void)
{
  char buf[500];
  buf[0] = 0;
  
  ostrstream str((char *)buf, (int)500, ios::out);
  WritePropertyType(str);
  str << '\0';
  str.flush();

  wxString theString(buf);
  return theString;
}

void wxPropertyValue::operator=(const wxPropertyValue& val)
{
  modifiedFlag = TRUE;
  Copy((wxPropertyValue&)val);
}

// void wxPropertyValue::operator=(const char *val)
void wxPropertyValue::operator=(const wxString& val1)
{
  const char *val = (const char *)val1;

  modifiedFlag = TRUE;
  if (type == wxPropertyValueNull)
    type = wxPropertyValueString;

  if (type == wxPropertyValueString)
  {
    if (val)
      value.string = copystring(val);
    else
      value.string = NULL;
  }
  else if (type == wxPropertyValueStringPtr)
  {
    if (*value.stringPtr)
      delete[] *value.stringPtr;
    if (val)
      *value.stringPtr = copystring(val);
    else
      *value.stringPtr = NULL;
  }
  
  client_data = NULL;
  next = NULL;
  last = NULL;

}

void wxPropertyValue::operator=(const long val)
{
  modifiedFlag = TRUE;
  if (type == wxPropertyValueNull)
    type = wxPropertyValueInteger;

  if (type == wxPropertyValueInteger)
    value.integer = val;
  else if (type == wxPropertyValueIntegerPtr)
    *value.integerPtr = val;
  else if (type == wxPropertyValueReal)
    value.real = (float)val;
  else if (type == wxPropertyValueRealPtr)
    *value.realPtr = (float)val;
  
  client_data = NULL;
  next = NULL;
}

void wxPropertyValue::operator=(const bool val)
{
  modifiedFlag = TRUE;
  if (type == wxPropertyValueNull)
    type = wxPropertyValuebool;

  if (type == wxPropertyValuebool)
    value.integer = (long)val;
  else if (type == wxPropertyValueboolPtr)
    *value.boolPtr = val;

  client_data = NULL;
  next = NULL;
}

void wxPropertyValue::operator=(const float val)
{
  modifiedFlag = TRUE;
  if (type == wxPropertyValueNull)
    type = wxPropertyValueReal;

  if (type == wxPropertyValueInteger)
    value.integer = (long)val;
  else if (type == wxPropertyValueIntegerPtr)
    *value.integerPtr = (long)val;
  else if (type == wxPropertyValueReal)
    value.real = val;
  else if (type == wxPropertyValueRealPtr)
    *value.realPtr = val;
    
  client_data = NULL;
  next = NULL;
}

void wxPropertyValue::operator=(const char **val)
{
  modifiedFlag = TRUE;
  type = wxPropertyValueStringPtr;

  if (val)
    value.stringPtr = (char **)val;
  else
    value.stringPtr = NULL;
  client_data = NULL;
  next = NULL;
  last = NULL;

}

void wxPropertyValue::operator=(const long *val)
{
  modifiedFlag = TRUE;
  type = wxPropertyValueIntegerPtr;
  value.integerPtr = (long *)val;
  client_data = NULL;
  next = NULL;
}

void wxPropertyValue::operator=(const bool *val)
{
  modifiedFlag = TRUE;
  type = wxPropertyValueboolPtr;
  value.boolPtr = (bool *)val;
  client_data = NULL;
  next = NULL;
}

void wxPropertyValue::operator=(const float *val)
{
  modifiedFlag = TRUE;
  type = wxPropertyValueRealPtr;
  value.realPtr = (float *)val;
  client_data = NULL;
  next = NULL;
}

long wxPropertyValue::IntegerValue(void)
  {
    if (type == wxPropertyValueInteger)
      return value.integer;
    else if (type == wxPropertyValueReal)
      return (long)value.real;
    else if (type == wxPropertyValueIntegerPtr)
      return *value.integerPtr;
    else if (type == wxPropertyValueRealPtr)
      return (long)(*value.realPtr);
    else return 0;
  }

long *wxPropertyValue::IntegerValuePtr(void)
{
  return value.integerPtr;
}

float wxPropertyValue::RealValue(void) {
    if (type == wxPropertyValueReal)
      return value.real;
    else if (type == wxPropertyValueRealPtr)
      return *value.realPtr;
    else if (type == wxPropertyValueInteger)
      return (float)value.integer;
    else if (type == wxPropertyValueIntegerPtr)
      return (float)*(value.integerPtr);
    else return 0.0;
  }

float *wxPropertyValue::RealValuePtr(void)
{
  return value.realPtr;
}

bool wxPropertyValue::BoolValue(void) {
    if (type == wxPropertyValueReal)
      return (value.real != 0.0);
    if (type == wxPropertyValueRealPtr)
      return (*(value.realPtr) != 0.0);
    else if (type == wxPropertyValueInteger)
      return (value.integer != 0);
    else if (type == wxPropertyValueIntegerPtr)
      return (*(value.integerPtr) != 0);
    else if (type == wxPropertyValuebool)
      return (value.integer != 0);
    else if (type == wxPropertyValueboolPtr)
      return (*(value.boolPtr) != 0);
    else return FALSE;
  }

bool *wxPropertyValue::BoolValuePtr(void)
{
  return value.boolPtr;
}

char *wxPropertyValue::StringValue(void) {
    if (type == wxPropertyValueString)
      return value.string;
    else if (type == wxPropertyValueStringPtr)
      return *(value.stringPtr);
    else return NULL;
  }

char **wxPropertyValue::StringValuePtr(void)
{
  return value.stringPtr;
}

/*
 * A property (name plus value)
 */
 
IMPLEMENT_DYNAMIC_CLASS(wxProperty, wxObject)

wxProperty::wxProperty(void)
{
  propertyRole = (char *)NULL;
  propertyValidator = NULL;
  propertyWindow = NULL;
  enabled = TRUE;
}

wxProperty::wxProperty(wxProperty& copyFrom)
{
  value = copyFrom.GetValue();
  name = copyFrom.GetName();
  propertyRole = copyFrom.GetRole();
  propertyValidator = copyFrom.GetValidator();
  enabled = copyFrom.IsEnabled();
  propertyWindow = NULL;
}

wxProperty::wxProperty(wxString nm, wxString role, wxPropertyValidator *ed):name(nm), propertyRole(role)
{
  propertyValidator = ed;
  propertyWindow = NULL;
  enabled = TRUE;
}

wxProperty::wxProperty(wxString nm, const wxPropertyValue& val, wxString role, wxPropertyValidator *ed):
  name(nm), value(val), propertyRole(role)
{
  propertyValidator = ed;
  propertyWindow = NULL;
  enabled = TRUE;
}

wxProperty::~wxProperty(void)
{
  if (propertyValidator)
    delete propertyValidator;
}

wxPropertyValue& wxProperty::GetValue(void)
{
  return value;
}

wxPropertyValidator *wxProperty::GetValidator(void)
{
  return propertyValidator;
}

wxString& wxProperty::GetName(void)
{
  return name;
}

wxString& wxProperty::GetRole(void)
{
  return propertyRole;
}

void wxProperty::SetValue(const wxPropertyValue& val)
{
  value = val;
}

void wxProperty::SetValidator(wxPropertyValidator *ed)
{
  propertyValidator = ed;
}

void wxProperty::SetRole(wxString& role)
{
  propertyRole = role;
}

void wxProperty::SetName(wxString& nm)
{
  name = nm;
}

void wxProperty::operator=(const wxPropertyValue& val)
{
  value = val;
}

/*
 * Base property view class
 */
 
IMPLEMENT_DYNAMIC_CLASS(wxPropertyView, wxEvtHandler)

wxPropertyView::wxPropertyView(long flags)
{
  buttonFlags = flags;
  propertySheet = NULL;
  currentValidator = NULL;
  currentProperty = NULL;
}

wxPropertyView::~wxPropertyView(void)
{
}

void wxPropertyView::AddRegistry(wxPropertyValidatorRegistry *registry)
{
  validatorRegistryList.Append(registry);
}

wxPropertyValidator *wxPropertyView::FindPropertyValidator(wxProperty *property)
{
  if (property->GetValidator())
    return property->GetValidator();
    
  wxNode *node = validatorRegistryList.First();
  while (node)
  {
    wxPropertyValidatorRegistry *registry = (wxPropertyValidatorRegistry *)node->Data();
    wxPropertyValidator *validator = registry->GetValidator(property->GetRole());
    if (validator)
      return validator;
    node = node->Next();
  }
  return NULL;
/*
  if (!wxDefaultPropertyValidator)
    wxDefaultPropertyValidator = new wxPropertyListValidator;
  return wxDefaultPropertyValidator;
*/
}

/*
 * Property sheet
 */

IMPLEMENT_DYNAMIC_CLASS(wxPropertySheet, wxObject)

wxPropertySheet::wxPropertySheet(void):properties(wxKEY_STRING)
{
}

wxPropertySheet::~wxPropertySheet(void)
{
  Clear();
}

bool wxPropertySheet::Save( ostream& WXUNUSED(str) )
{
  return FALSE;
}

bool wxPropertySheet::Load( ostream& WXUNUSED(str) )
{
  return FALSE;
}

void wxPropertySheet::UpdateAllViews( wxPropertyView *WXUNUSED(thisView) )
{
}

// Add a property
void wxPropertySheet::AddProperty(wxProperty *property)
{
  properties.Append(property->GetName().GetData(), property);
}

// Get property by name
wxProperty *wxPropertySheet::GetProperty(wxString name)
{
  wxNode *node = properties.Find(name.GetData());
  if (!node)
    return NULL;
  else
    return (wxProperty *)node->Data();
}

// Clear all properties
void wxPropertySheet::Clear(void)
{
  wxNode *node = properties.First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    wxNode *next = node->Next();
    delete prop;
    delete node;
    node = next;
  }
}

// Sets/clears the modified flag for each property value
void wxPropertySheet::SetAllModified(bool flag)
{
  wxNode *node = properties.First();
  while (node)
  {
    wxProperty *prop = (wxProperty *)node->Data();
    prop->GetValue().SetModified(flag);
    node = node->Next();
  }  
}

/*
 * Property validator registry
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxPropertyValidatorRegistry, wxHashTable)

wxPropertyValidatorRegistry::wxPropertyValidatorRegistry(void):wxHashTable(wxKEY_STRING)
{
}

wxPropertyValidatorRegistry::~wxPropertyValidatorRegistry(void)
{
  ClearRegistry();
}

void wxPropertyValidatorRegistry::RegisterValidator(wxString& typeName, wxPropertyValidator *validator)
{
  Put(typeName.GetData(), validator);
}

wxPropertyValidator *wxPropertyValidatorRegistry::GetValidator(wxString& typeName)
{
  return (wxPropertyValidator *)Get(typeName.GetData());
}

void wxPropertyValidatorRegistry::ClearRegistry(void)
{
  BeginFind();
  wxNode *node;
  while (node = Next())
  {
    delete (wxPropertyValidator *)node->Data();
  }
}

 /*
  * Property validator
  */


IMPLEMENT_ABSTRACT_CLASS(wxPropertyValidator, wxEvtHandler)

wxPropertyValidator::wxPropertyValidator(long flags)
{
  validatorFlags = flags;
  validatorProperty = NULL;
}

wxPropertyValidator::~wxPropertyValidator(void)
{}

bool wxPropertyValidator::StringToFloat (char *s, float *number) {
	double num;
	bool ok = StringToDouble (s, &num);
	*number = (float) num;
	return ok;
}

bool wxPropertyValidator::StringToDouble (char *s, double *number) {
    bool ok = TRUE;
    char *value_ptr;
    *number = strtod (s, &value_ptr);
    if (value_ptr) {
		int len = strlen (value_ptr);
		for (int i = 0; i < len; i++) {
			ok = (isspace (value_ptr[i]) != 0);
			if (!ok) return FALSE;
		}
    }
    return ok;
}

bool wxPropertyValidator::StringToInt (char *s, int *number) {
	long num;
	bool ok = StringToLong (s, &num);
	*number = (int) num;
	return ok;
}

bool wxPropertyValidator::StringToLong (char *s, long *number) {
    bool ok = TRUE;
    char *value_ptr;
    *number = strtol (s, &value_ptr, 10);
    if (value_ptr) {
		int len = strlen (value_ptr);
		for (int i = 0; i < len; i++) {
			ok = (isspace (value_ptr[i]) != 0);
			if (!ok) return FALSE;
		}
    }
    return ok;
}

char *wxPropertyValidator::FloatToString (float number) {
	static char buf[20];
	sprintf (buf, "%.6g", number);
	return buf;
}

char *wxPropertyValidator::DoubleToString (double number) {
	static char buf[20];
	sprintf (buf, "%.6g", number);
	return buf;
}

char *wxPropertyValidator::IntToString (int number) {
	return ::IntToString (number);
}

char *wxPropertyValidator::LongToString (long number) {
	return ::LongToString (number);
  }


