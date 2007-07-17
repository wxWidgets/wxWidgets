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

#if wxUSE_IOSTREAMH
#if defined(__WXMSW__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#else
#include <strstream>
#endif

#include "wx/window.h"
#include "wx/utils.h"
#include "wx/list.h"
#include "prop.h"

IMPLEMENT_DYNAMIC_CLASS(wxPropertyValue, wxObject)

wxPropertyValue::wxPropertyValue(void)
{
  m_type = wxPropertyValueNull;
  m_next = NULL;
  m_last = NULL;
  m_value.first = NULL;
  m_clientData = NULL;
  m_modifiedFlag = FALSE;
}

wxPropertyValue::wxPropertyValue(const wxPropertyValue& copyFrom)
{
  m_modifiedFlag = FALSE;
  Copy((wxPropertyValue& )copyFrom);
}

wxPropertyValue::wxPropertyValue(const char *val)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueString;

  m_value.string = copystring(val);
  m_clientData = NULL;
  m_next = NULL;
  m_last = NULL;
}

wxPropertyValue::wxPropertyValue(const wxString& val)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueString;

  m_value.string = copystring((const char *)val);
  m_clientData = NULL;
  m_next = NULL;
  m_last = NULL;
}

wxPropertyValue::wxPropertyValue(long the_integer)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueInteger;
  m_value.integer = the_integer;
  m_clientData = NULL;
  m_next = NULL;
}

wxPropertyValue::wxPropertyValue(bool val)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValuebool;
  m_value.integer = val;
  m_clientData = NULL;
  m_next = NULL;
}

wxPropertyValue::wxPropertyValue(float the_real)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueReal;
  m_value.real = the_real;
  m_clientData = NULL;
  m_next = NULL;
}

wxPropertyValue::wxPropertyValue(double the_real)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueReal;
  m_value.real = (float)the_real;
  m_clientData = NULL;
  m_next = NULL;
}

// Pointer versions: we have a pointer to the real C++ value.
wxPropertyValue::wxPropertyValue(char **val)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueStringPtr;

  m_value.stringPtr = val;
  m_clientData = NULL;
  m_next = NULL;
  m_last = NULL;
}

wxPropertyValue::wxPropertyValue(long *val)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueIntegerPtr;
  m_value.integerPtr = val;
  m_clientData = NULL;
  m_next = NULL;
}

wxPropertyValue::wxPropertyValue(bool *val)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueboolPtr;
  m_value.boolPtr = val;
  m_clientData = NULL;
  m_next = NULL;
}

wxPropertyValue::wxPropertyValue(float *val)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueRealPtr;
  m_value.realPtr = val;
  m_clientData = NULL;
  m_next = NULL;
}

wxPropertyValue::wxPropertyValue(wxList *the_list)
{
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueList;
  m_clientData = NULL;
  m_last = NULL;
  m_value.first = NULL;

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
  m_modifiedFlag = FALSE;
  m_type = wxPropertyValueList;
  m_clientData = NULL;
  m_last = NULL;
  m_value.first = NULL;

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
  switch (m_type)
  {
    case wxPropertyValueInteger:
    case wxPropertyValuebool:
    case wxPropertyValueReal:
    {
     break;
    }
   case wxPropertyValueString:
   {
     delete[] m_value.string;
     break;
   }
   case wxPropertyValueList:
   {
     wxPropertyValue *expr = m_value.first;
     while (expr)
     {
       wxPropertyValue *expr1 = expr->m_next;

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
  m_modifiedFlag = TRUE;
  if (!m_value.first)
    m_value.first = expr;

  if (m_last)
    m_last->m_next = expr;
  m_last = expr;
}

void wxPropertyValue::Insert(wxPropertyValue *expr)
{
  m_modifiedFlag = TRUE;
  expr->m_next = m_value.first;
  m_value.first = expr;

  if (!m_last)
    m_last = expr;
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
      previous->m_next = expr->m_next;
      
    // If node was the first in the list,
    // make the list point to the NEXT one.
    if (GetFirst() == expr)
    {
      m_value.first = expr->m_next;
    }

    // If node was the last in the list,
    // make the list 'last' pointer point to the PREVIOUS one.
    if (GetLast() == expr)
    {
      if (previous)
        m_last = previous;
      else
        m_last = NULL;
    }
    m_modifiedFlag = TRUE;
    delete expr;
  }

}

void wxPropertyValue::ClearList(void)
{
  wxPropertyValue *val = GetFirst();
  if (val)
    m_modifiedFlag = TRUE;

  while (val)
  {
    wxPropertyValue *next = val->GetNext();
    delete val;
    val = next;
  }
  m_value.first = NULL;
  m_last = NULL;
}

wxPropertyValue *wxPropertyValue::NewCopy(void) const
{
  switch (m_type)
  {
    case wxPropertyValueInteger:
      return new wxPropertyValue(m_value.integer);
    case wxPropertyValuebool:
      return new wxPropertyValue((bool) (m_value.integer != 0));
    case wxPropertyValueReal:
      return new wxPropertyValue(m_value.real);
    case wxPropertyValueString:
      return new wxPropertyValue(m_value.string);
    case wxPropertyValueList:
    {
      wxPropertyValue *expr = m_value.first;
      wxPropertyValue *new_list = new wxPropertyValue;
      new_list->SetType(wxPropertyValueList);
      while (expr)
      {
        wxPropertyValue *expr2 = expr->NewCopy();
        new_list->Append(expr2);
        expr = expr->m_next;
      }
      return new_list;
    }
   case wxPropertyValueIntegerPtr:
     return new wxPropertyValue(m_value.integerPtr);
   case wxPropertyValueRealPtr:
     return new wxPropertyValue(m_value.realPtr);
   case wxPropertyValueboolPtr:
     return new wxPropertyValue(m_value.boolPtr);
   case wxPropertyValueStringPtr:
     return new wxPropertyValue(m_value.stringPtr);

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
  m_type = copyFrom.Type();

  switch (m_type)
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
      (*this) = s != 0;
      return ;
    }
      
    case wxPropertyValueList:
    {
      m_value.first = NULL;
      m_next = NULL;
      m_last = NULL;
      wxPropertyValue *expr = copyFrom.m_value.first;
      while (expr)
      {
        wxPropertyValue *expr2 = expr->NewCopy();
        Append(expr2);
        expr = expr->m_next;
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
wxPropertyValue *wxPropertyValue::Arg(wxPropertyValueType type, int arg) const
{
  wxPropertyValue *expr = m_value.first;
  for (int i = 1; i < arg; i++)
    if (expr)
      expr = expr->m_next;

  if (expr && (expr->m_type == type))
    return expr;
  else
    return NULL;
}

// Return nth argument of a list expression (starting from zero)
wxPropertyValue *wxPropertyValue::Nth(int arg) const
{
  if (m_type != wxPropertyValueList)
    return NULL;

  wxPropertyValue *expr = m_value.first;
  for (int i = 0; i < arg; i++)
    if (expr)
      expr = expr->m_next;
    else return NULL;

  if (expr)
    return expr;
  else
    return NULL;
}

  // Returns the number of elements in a list expression
int wxPropertyValue::Number(void) const
{
  if (m_type != wxPropertyValueList)
    return 0;

  int i = 0;
  wxPropertyValue *expr = m_value.first;
  while (expr)
  {
    expr = expr->m_next;
    i ++;
  }
  return i;
}

void wxPropertyValue::WritePropertyClause(ostream& stream)  // Write this expression as a top-level clause
{
  if (m_type != wxPropertyValueList)
    return;

  wxPropertyValue *node = m_value.first;
  if (node)
  {
    node->WritePropertyType(stream);
    stream << "(";
    node = node->m_next;
    bool first = TRUE;
    while (node)
    {
      if (!first)
        stream << "  ";
      node->WritePropertyType(stream);
      node = node->m_next;
      if (node) stream << ",\n";
      first = FALSE;
    }
    stream << ").\n\n";
  }
}

void wxPropertyValue::WritePropertyType(ostream& stream)    // Write as any other subexpression
{
  switch (m_type)
  {
    case wxPropertyValueInteger:
    {
      stream << m_value.integer;
      break;
    }
    case wxPropertyValueIntegerPtr:
    {
      stream << *m_value.integerPtr;
      break;
    }
    case wxPropertyValuebool:
    {
      if (m_value.integer)
        stream << "True";
      else
        stream << "False";
      break;
    }
    case wxPropertyValueboolPtr:
    {
      if (*m_value.integerPtr)
        stream << "True";
      else
        stream << "False";
      break;
    }
    case wxPropertyValueReal:
    {
      float f = m_value.real;
      sprintf(wxBuffer, "%.6g", (double)f);
      stream << wxBuffer;
      break;
    }
    case wxPropertyValueRealPtr:
    {
      float f = *m_value.realPtr;
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
      int len = strlen(m_value.string);
      for (i = 0; i < len; i++)
      {
        char ch = m_value.string[i];
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
      int len = strlen(*(m_value.stringPtr));
      for (i = 0; i < len; i++)
      {
        char ch = *(m_value.stringPtr)[i];

      }
      break;
    }
    case wxPropertyValueList:
    {
      if (!m_value.first)
        stream << "[]";
      else
      {
        wxPropertyValue *expr = m_value.first;

        stream << "[";
        while (expr)
        {
          expr->WritePropertyType(stream);
          expr = expr->m_next;
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
  m_modifiedFlag = TRUE;
  Copy((wxPropertyValue&)val);
}

// void wxPropertyValue::operator=(const char *val)
void wxPropertyValue::operator=(const wxString& val1)
{
  const char *val = (const char *)val1;

  m_modifiedFlag = TRUE;
  if (m_type == wxPropertyValueNull)
    m_type = wxPropertyValueString;

  if (m_type == wxPropertyValueString)
  {
    if (val)
      m_value.string = copystring(val);
    else
      m_value.string = NULL;
  }
  else if (m_type == wxPropertyValueStringPtr)
  {
    if (*m_value.stringPtr)
      delete[] *m_value.stringPtr;
    if (val)
      *m_value.stringPtr = copystring(val);
    else
      *m_value.stringPtr = NULL;
  }
  
  m_clientData = NULL;
  m_next = NULL;
  m_last = NULL;

}

void wxPropertyValue::operator=(const long val)
{
  m_modifiedFlag = TRUE;
  if (m_type == wxPropertyValueNull)
    m_type = wxPropertyValueInteger;

  if (m_type == wxPropertyValueInteger)
    m_value.integer = val;
  else if (m_type == wxPropertyValueIntegerPtr)
    *m_value.integerPtr = val;
  else if (m_type == wxPropertyValueReal)
    m_value.real = (float)val;
  else if (m_type == wxPropertyValueRealPtr)
    *m_value.realPtr = (float)val;

  m_clientData = NULL;
  m_next = NULL;
}

void wxPropertyValue::operator=(const bool val)
{
  m_modifiedFlag = TRUE;
  if (m_type == wxPropertyValueNull)
    m_type = wxPropertyValuebool;

  if (m_type == wxPropertyValuebool)
    m_value.integer = (long)val;
  else if (m_type == wxPropertyValueboolPtr)
    *m_value.boolPtr = val;

  m_clientData = NULL;
  m_next = NULL;
}

void wxPropertyValue::operator=(const float val)
{
  m_modifiedFlag = TRUE;
  if (m_type == wxPropertyValueNull)
    m_type = wxPropertyValueReal;

  if (m_type == wxPropertyValueInteger)
    m_value.integer = (long)val;
  else if (m_type == wxPropertyValueIntegerPtr)
    *m_value.integerPtr = (long)val;
  else if (m_type == wxPropertyValueReal)
    m_value.real = val;
  else if (m_type == wxPropertyValueRealPtr)
    *m_value.realPtr = val;

  m_clientData = NULL;
  m_next = NULL;
}

void wxPropertyValue::operator=(const char **val)
{
  m_modifiedFlag = TRUE;
  m_type = wxPropertyValueStringPtr;

  if (val)
    m_value.stringPtr = (char **)val;
  else
    m_value.stringPtr = NULL;
  m_clientData = NULL;
  m_next = NULL;
  m_last = NULL;

}

void wxPropertyValue::operator=(const long *val)
{
  m_modifiedFlag = TRUE;
  m_type = wxPropertyValueIntegerPtr;
  m_value.integerPtr = (long *)val;
  m_clientData = NULL;
  m_next = NULL;
}

void wxPropertyValue::operator=(const bool *val)
{
  m_modifiedFlag = TRUE;
  m_type = wxPropertyValueboolPtr;
  m_value.boolPtr = (bool *)val;
  m_clientData = NULL;
  m_next = NULL;
}

void wxPropertyValue::operator=(const float *val)
{
  m_modifiedFlag = TRUE;
  m_type = wxPropertyValueRealPtr;
  m_value.realPtr = (float *)val;
  m_clientData = NULL;
  m_next = NULL;
}

long wxPropertyValue::IntegerValue(void) const
  {
    if (m_type == wxPropertyValueInteger)
      return m_value.integer;
    else if (m_type == wxPropertyValueReal)
      return (long)m_value.real;
    else if (m_type == wxPropertyValueIntegerPtr)
      return *m_value.integerPtr;
    else if (m_type == wxPropertyValueRealPtr)
      return (long)(*m_value.realPtr);
    else return 0;
  }

long *wxPropertyValue::IntegerValuePtr(void) const
{
  return m_value.integerPtr;
}

float wxPropertyValue::RealValue(void) const {
    if (m_type == wxPropertyValueReal)
      return m_value.real;
    else if (m_type == wxPropertyValueRealPtr)
      return *m_value.realPtr;
    else if (m_type == wxPropertyValueInteger)
      return (float)m_value.integer;
    else if (m_type == wxPropertyValueIntegerPtr)
      return (float)*(m_value.integerPtr);
    else return 0.0;
  }

float *wxPropertyValue::RealValuePtr(void) const
{
  return m_value.realPtr;
}

bool wxPropertyValue::BoolValue(void) const {
    if (m_type == wxPropertyValueReal)
      return (m_value.real != 0.0);
    if (m_type == wxPropertyValueRealPtr)
      return (*(m_value.realPtr) != 0.0);
    else if (m_type == wxPropertyValueInteger)
      return (m_value.integer != 0);
    else if (m_type == wxPropertyValueIntegerPtr)
      return (*(m_value.integerPtr) != 0);
    else if (m_type == wxPropertyValuebool)
      return (m_value.integer != 0);
    else if (m_type == wxPropertyValueboolPtr)
      return (*(m_value.boolPtr) != 0);
    else return FALSE;
  }

bool *wxPropertyValue::BoolValuePtr(void) const
{
  return m_value.boolPtr;
}

char *wxPropertyValue::StringValue(void) const {
    if (m_type == wxPropertyValueString)
      return m_value.string;
    else if (m_type == wxPropertyValueStringPtr)
      return *(m_value.stringPtr);
    else return NULL;
  }

char **wxPropertyValue::StringValuePtr(void) const
{
  return m_value.stringPtr;
}

/*
 * A property (name plus value)
 */
 
IMPLEMENT_DYNAMIC_CLASS(wxProperty, wxObject)

wxProperty::wxProperty(void)
{
  m_propertyRole = (char *)NULL;
  m_propertyValidator = NULL;
  m_propertyWindow = NULL;
  m_enabled = TRUE;
}

wxProperty::wxProperty(wxProperty& copyFrom)
{
  m_value = copyFrom.GetValue();
  m_name = copyFrom.GetName();
  m_propertyRole = copyFrom.GetRole();
  m_propertyValidator = copyFrom.GetValidator();
  m_enabled = copyFrom.IsEnabled();
  m_propertyWindow = NULL;
}

wxProperty::wxProperty(wxString nm, wxString role, wxPropertyValidator *ed):m_name(nm), m_propertyRole(role)
{
  m_propertyValidator = ed;
  m_propertyWindow = NULL;
  m_enabled = TRUE;
}

wxProperty::wxProperty(wxString nm, const wxPropertyValue& val, wxString role, wxPropertyValidator *ed):
  m_name(nm), m_value(val), m_propertyRole(role)
{
  m_propertyValidator = ed;
  m_propertyWindow = NULL;
  m_enabled = TRUE;
}

wxProperty::~wxProperty(void)
{
  if (m_propertyValidator)
    delete m_propertyValidator;
}

wxPropertyValue& wxProperty::GetValue(void) const
{
  return (wxPropertyValue&) m_value;
}

wxPropertyValidator *wxProperty::GetValidator(void) const
{
  return m_propertyValidator;
}

wxString& wxProperty::GetName(void) const
{
  return (wxString&) m_name;
}

wxString& wxProperty::GetRole(void) const
{
  return (wxString&) m_propertyRole;
}

void wxProperty::SetValue(const wxPropertyValue& val)
{
  m_value = val;
}

void wxProperty::SetValidator(wxPropertyValidator *ed)
{
  m_propertyValidator = ed;
}

void wxProperty::SetRole(wxString& role)
{
  m_propertyRole = role;
}

void wxProperty::SetName(wxString& nm)
{
  m_name = nm;
}

void wxProperty::operator=(const wxPropertyValue& val)
{
  m_value = val;
}

/*
 * Base property view class
 */
 
IMPLEMENT_DYNAMIC_CLASS(wxPropertyView, wxEvtHandler)

wxPropertyView::wxPropertyView(long flags)
{
  m_buttonFlags = flags;
  m_propertySheet = NULL;
  m_currentValidator = NULL;
  m_currentProperty = NULL;
}

wxPropertyView::~wxPropertyView(void)
{
}

void wxPropertyView::AddRegistry(wxPropertyValidatorRegistry *registry)
{
  m_validatorRegistryList.Append(registry);
}

wxPropertyValidator *wxPropertyView::FindPropertyValidator(wxProperty *property)
{
  if (property->GetValidator())
    return property->GetValidator();
    
  wxNode *node = m_validatorRegistryList.First();
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

wxPropertySheet::wxPropertySheet(void):m_properties(wxKEY_STRING)
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
  m_properties.Append((const char*) property->GetName(), property);
}

// Get property by name
wxProperty *wxPropertySheet::GetProperty(wxString name)
{
  wxNode *node = m_properties.Find((const char*) name);
  if (!node)
    return NULL;
  else
    return (wxProperty *)node->Data();
}

// Clear all properties
void wxPropertySheet::Clear(void)
{
  wxNode *node = m_properties.First();
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
  wxNode *node = m_properties.First();
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

void wxPropertyValidatorRegistry::RegisterValidator(const wxString& typeName, wxPropertyValidator *validator)
{
  Put((const char*) typeName, validator);
}

wxPropertyValidator *wxPropertyValidatorRegistry::GetValidator(const wxString& typeName)
{
  return (wxPropertyValidator *)Get((const char*) typeName);
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
  m_validatorFlags = flags;
  m_validatorProperty = NULL;
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


