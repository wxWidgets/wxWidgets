/////////////////////////////////////////////////////////////////////////////
// Name:        wxexpr.cpp
// Purpose:     wxExpr
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wxexpr.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_IOSTREAMH
    #include <fstream.h>
#else
    #include <fstream>
#endif

#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "wx/utils.h"

#include "wx/expr.h"
#include "wx/wxexpr.h"

extern "C" void add_expr(char *);
extern "C" void LexFromFile(FILE *fd);
extern "C" void LexFromString(char *buf);


wxExprDatabase *thewxExprDatabase = NULL;
wxExprErrorHandler currentwxExprErrorHandler;

wxExpr::wxExpr(const wxString& functor)
{
  type = wxExprList;
  next = NULL;
  last = NULL;
  value.first = NULL;

  wxExpr *pfunctor = new wxExpr(wxExprWord, functor);
  Append(pfunctor);
  client_data = NULL;
}

wxExpr::wxExpr(wxExprType the_type, const wxString& word_or_string)
{
  type = the_type;

  switch (the_type)
  {
   case wxExprWord:
    value.word = copystring((const wxChar *)word_or_string);
    break;
   case wxExprString:
    value.string = copystring((const wxChar *)word_or_string);
    break;
   case wxExprList:
    last = NULL;
    value.first = NULL;
    break;
   case wxExprReal:
   case wxExprInteger:
   case wxExprNull: 
    break;
  }
  client_data = NULL;
  next = NULL;
}

wxExpr::wxExpr(wxExprType the_type, wxChar *word_or_string, bool allocate)
{
  type = the_type;

  switch (the_type)
  {
   case wxExprWord:
    value.word = allocate ? copystring(word_or_string) : word_or_string;
    break;
   case wxExprString:
    value.string = allocate ? copystring(word_or_string) : word_or_string;
    break;
   case wxExprList:
    last = NULL;
    value.first = NULL;
    break;
   case wxExprReal:
   case wxExprInteger:
   case wxExprNull: 
    break;
  }
  client_data = NULL;
  next = NULL;
}

wxExpr::wxExpr(long the_integer)
{
  type = wxExprInteger;
  value.integer = the_integer;
  client_data = NULL;
  next = NULL;
}

wxExpr::wxExpr(double the_real)
{
  type = wxExprReal;
  value.real = the_real;
  client_data = NULL;
  next = NULL;
}

wxExpr::wxExpr(wxList *the_list)
{
  type = wxExprList;
  client_data = NULL;
  last = NULL;
  value.first = NULL;

  wxExpr *listExpr = new wxExpr(wxExprList);

  wxNode *node = the_list->First();
  while (node)
  {
    wxExpr *expr = (wxExpr *)node->Data();
    listExpr->Append(expr);
    node = node->Next();
  }
  Append(listExpr);

  delete the_list;
}

wxExpr::~wxExpr(void)
{
  switch (type)
  {
    case wxExprInteger:
    case wxExprReal:
    {
     break;
    }
   case wxExprString:
   {
     delete[] value.string;
     break;
   }
   case wxExprWord:
   {
     delete[] value.word;
     break;
   }
   case wxExprList:
   {
     wxExpr *expr = value.first;
     while (expr)
     {
       wxExpr *expr1 = expr->next;

       delete expr;
       expr = expr1;
     }
     break;
   }
   case wxExprNull: break;
  }
}

void wxExpr::Append(wxExpr *expr)
{
  if (!value.first)
    value.first = expr;

  if (last)
    last->next = expr;
  last = expr;
}

void wxExpr::Insert(wxExpr *expr)
{
  expr->next = value.first;
  value.first = expr;

  if (!last)
    last = expr;
}

wxExpr *wxExpr::Copy(void) const
{
  // This seems to get round an optimizer bug when
  // using Watcom C++ 10a in WIN32 compilation mode.
  // If these lines not present, the type seems to be
  // interpreted wrongly as an integer.
  // I don't want to turn optimization off since it's needed
  // for reading in files quickly.
#if defined(__WATCOMC__)
  char buf[2];
  sprintf(buf, "");
#endif

  switch (type)
  {
    case wxExprInteger:
      return new wxExpr(value.integer);
    case wxExprReal:
      return new wxExpr(value.real);
    case wxExprString:
      return new wxExpr(wxExprString, wxString(value.string));
    case wxExprWord:
      return new wxExpr(wxExprWord, wxString(value.word));
    case wxExprList:
    {
      wxExpr *expr = value.first;
      wxExpr *new_list = new wxExpr(wxExprList);
      while (expr)
      {
        wxExpr *expr2 = expr->Copy();
        new_list->Append(expr2);
        expr = expr->next;
      }
      return new_list;
    }
   case wxExprNull: 
    break;
  }
  return NULL;
}


// Get the wxExpr (containing (= wxExpr Value) form) for the given word
//  or string, assuming that we have Attribute=Value, ...
wxExpr *wxExpr::GetAttributeValueNode(const wxString& word) const // Use only for a clause or list
{
  if (type != wxExprList)
    return NULL;

  wxExpr *expr = value.first;
  while (expr)
  {
    if (expr->type == wxExprList)
    {
      wxExpr *firstNode = expr->value.first;
      if ((firstNode->type == wxExprWord) && (firstNode->value.word[0] == '='))
      {
        wxExpr *secondNode = firstNode->next;
        if ((secondNode->type == wxExprWord) && 
            (wxStrcmp((const wxChar *)word, secondNode->value.word) == 0))
        {
          return expr;
        }
      }
    }
    expr = expr->next;
  }
  return NULL;
}

// Get the value (in wxExpr form) for the given word or string, assuming
// that we have Attribute=Value, ...
wxExpr *wxExpr::AttributeValue(const wxString& word) const // Use only for a clause or list
{
  if (type != wxExprList)
    return NULL;

  wxExpr *attExpr = GetAttributeValueNode(word);
  if (attExpr && attExpr->value.first && attExpr->value.first->next)
    return attExpr->value.first->next->next;
  else return NULL;
}

wxString wxExpr::Functor(void) const // Use only for a clause
{
  if ((type != wxExprList) || !value.first)
    return wxString(_T(""));

  if (value.first->type == wxExprWord)
    return wxString(value.first->value.word);
  else
    return wxString(_T(""));
}

bool wxExpr::IsFunctor(const wxString& f) const  // Use only for a clause
{
  if ((type != wxExprList) || !value.first)
    return FALSE;

  return (value.first->type == wxExprWord && 
          (wxStrcmp((const wxChar *)f, value.first->value.word) == 0));
}

// Return nth argument of a clause (starting from 1)
wxExpr *wxExpr::Arg(wxExprType theType, int arg) const
{
  wxExpr *expr = value.first;
  int i;
  for (i = 1; i < arg; i++)
    if (expr)
      expr = expr->next;

  if (expr && (expr->type == theType))
    return expr;
  else
    return NULL;
}

// Return nth argument of a list expression (starting from zero)
wxExpr *wxExpr::Nth(int arg) const
{
  if (type != wxExprList)
    return NULL;

  wxExpr *expr = value.first;
  int i;
  for (i = 0; i < arg; i++)
    if (expr)
      expr = expr->next;
    else return NULL;

  if (expr)
    return expr;
  else
    return NULL;
}

  // Returns the number of elements in a list expression
int wxExpr::Number(void) const
{
  if (type != wxExprList)
    return 0;

  int i = 0;
  wxExpr *expr = value.first;
  while (expr)
  {
    expr = expr->next;
    i ++;
  }
  return i;
}

void wxExpr::DeleteAttributeValue(const wxString& attribute)
{
  if (type != wxExprList)
    return;

  wxExpr *expr = value.first;
  wxExpr *lastExpr = this;
  while (expr)
  {
    if (expr->type == wxExprList)
    {
      wxExpr *firstNode = expr->value.first;
      if ((firstNode->type == wxExprWord) && (firstNode->value.word[0] == '='))
      {
        wxExpr *secondNode = firstNode->next;
        if ((secondNode->type == wxExprWord) && 
            (wxStrcmp((const wxChar *)attribute, secondNode->value.word) == 0))
        {
          wxExpr *nextExpr = expr->next;
          delete expr;

          lastExpr->next = nextExpr;

          if (last == expr)
            last = lastExpr;

          return;
        }
      }
    }
    lastExpr = expr;
    expr = expr->next;
  }
  return;
}

void wxExpr::AddAttributeValue(const wxString& attribute, wxExpr *val)
{
  if (type != wxExprList)
  {
//    cout << "Error! tried to add an attribute-value pair to a nonlist wxExpr expression\n";
    return;
  }
  // Warning - existing code may assume that any existing value
  // is deleted first. For efficiency, we leave this to the application.
//  DeleteAttributeValue(attribute);

  wxExpr *patt = new wxExpr(wxExprWord, attribute);
  wxExpr *pequals = new wxExpr(wxExprWord, _T("="));

  wxExpr *listExpr = new wxExpr(wxExprList);

  listExpr->Append(pequals);
  listExpr->Append(patt);
  listExpr->Append(val);

  Append(listExpr);
}

void wxExpr::AddAttributeValue(const wxString& attribute, long val)
{
  if (type != wxExprList)
  {
//    cout << "Error! tried to add an attribute-value pair to a nonlist wxExpr expression\n";
    return;
  }
  // Warning - existing code may assume that any existing value
  // is deleted first. For efficiency, we leave this to the application.
//  DeleteAttributeValue(attribute);

  wxExpr *patt = new wxExpr(wxExprWord, attribute);
  wxExpr *pval = new wxExpr(val);
  wxExpr *pequals = new wxExpr(wxExprWord, _T("="));

  wxExpr *listExpr = new wxExpr(wxExprList);

  listExpr->Append(pequals);
  listExpr->Append(patt);
  listExpr->Append(pval);

  Append(listExpr);
}

void wxExpr::AddAttributeValue(const wxString& attribute, double val)
{
  if (type != wxExprList)
  {
//    cout << "Error! tried to add an attribute-value pair to a nonlist wxExpr expression\n";
    return;
  }

//  DeleteAttributeValue(attribute);
  wxExpr *patt = new wxExpr(wxExprWord, attribute);
  wxExpr *pval = new wxExpr(val);
  wxExpr *pequals = new wxExpr(wxExprWord, _T("="));

  wxExpr *listExpr = new wxExpr(wxExprList);

  listExpr->Append(pequals);
  listExpr->Append(patt);
  listExpr->Append(pval);

  Append(listExpr);
}

void wxExpr::AddAttributeValueString(const wxString& attribute, const wxString& val)
{
  if (type != wxExprList)
  {
//    cout << "Error! tried to add an attribute-value pair to a nonlist wxExpr expression\n";
    return;
  }

//  DeleteAttributeValue(attribute);

  wxExpr *patt = new wxExpr(wxExprWord, attribute);
  wxExpr *pval = new wxExpr(wxExprString, val);
  wxExpr *pequals = new wxExpr(wxExprWord, _T("="));

  wxExpr *listExpr = new wxExpr(wxExprList);

  listExpr->Append(pequals);
  listExpr->Append(patt);
  listExpr->Append(pval);

  Append(listExpr);
}

void wxExpr::AddAttributeValueWord(const wxString& attribute, const wxString& val)
{
  if (type != wxExprList)
  {
//    cout << "Error! tried to add an attribute-value pair to a nonlist wxExpr expression\n";
    return;
  }

//  DeleteAttributeValue(attribute);

  wxExpr *patt = new wxExpr(wxExprWord, attribute);
  wxExpr *pval = new wxExpr(wxExprWord, val);
  wxExpr *pequals = new wxExpr(wxExprWord, _T("="));

  wxExpr *listExpr = new wxExpr(wxExprList);

  listExpr->Append(pequals);
  listExpr->Append(patt);
  listExpr->Append(pval);

  Append(listExpr);
}

void wxExpr::AddAttributeValue(const wxString& attribute, wxList *val)
{
  if (type != wxExprList)
  {
//    cout << "Error! tried to add an attribute-value pair to a nonlist wxExpr expression\n";
    return;
  }
  if (!val)
    return;

//  DeleteAttributeValue(attribute);

  wxExpr *patt = new wxExpr(wxExprWord, attribute);
  wxExpr *pval = new wxExpr(val);
  wxExpr *pequals = new wxExpr(wxExprWord, _T("="));

  wxExpr *listExpr = new wxExpr(wxExprList);

  listExpr->Append(pequals);
  listExpr->Append(patt);
  listExpr->Append(pval);

  Append(listExpr);
}

void wxExpr::AddAttributeValueStringList(const wxString& attribute, wxList *string_list)
{
  if (type != wxExprList)
  {
//    cout << "Error! tried to add an attribute-value pair to a nonlist wxExpr expression\n";
    return;
  }
  if (!string_list)
    return;

//  DeleteAttributeValue(attribute);

  // First make a list of wxExpr strings
  wxExpr *listExpr = new wxExpr(wxExprList);
  wxNode *node = string_list->First();
  while (node)
  {
    char *string = (char *)node->Data();
    wxExpr *expr = new wxExpr(wxExprString, wxString(string));
    listExpr->Append(expr);
    node = node->Next();
  }

  // Now make an (=, Att, Value) triple
  wxExpr *patt = new wxExpr(wxExprWord, attribute);
  wxExpr *pequals = new wxExpr(wxExprWord, _T("="));

  wxExpr *listExpr2 = new wxExpr(wxExprList);

  listExpr2->Append(pequals);
  listExpr2->Append(patt);
  listExpr2->Append(listExpr);

  Append(listExpr2);
}

bool wxExpr::GetAttributeValue(const wxString& att, int& var) const
{
  wxExpr *expr = AttributeValue(att);

  if (expr && (expr->Type() == wxExprInteger || expr->Type() == wxExprReal))
  {
    var = (int)(expr->IntegerValue());
    return TRUE;
  }
  else
    return FALSE;
}

bool wxExpr::GetAttributeValue(const wxString& att, long& var) const
{
  wxExpr *expr = AttributeValue(att);

  if (expr && (expr->Type() == wxExprInteger || expr->Type() == wxExprReal))
  {
    var = expr->IntegerValue();
    return TRUE;
  }
  else
    return FALSE;
}

bool wxExpr::GetAttributeValue(const wxString& att, float& var) const
{
  wxExpr *expr = AttributeValue(att);
  if (expr && (expr->Type() == wxExprInteger || expr->Type() == wxExprReal))
  {
    var = (float) expr->RealValue();
    return TRUE;
  }
  else
    return FALSE;
}

bool wxExpr::GetAttributeValue(const wxString& att, double& var) const
{
  wxExpr *expr = AttributeValue(att);
  if (expr && (expr->Type() == wxExprInteger || expr->Type() == wxExprReal))
  {
    var = expr->RealValue();
    return TRUE;
  }
  else
    return FALSE;
}

bool wxExpr::GetAttributeValue(const wxString& att, wxString& var)  const // Word OR string -> string
{
  wxExpr *expr = AttributeValue(att);
  if (expr && expr->Type() == wxExprWord)
  {
    var = expr->WordValue();
    return TRUE;
  }
  else if (expr && expr->Type() == wxExprString)
  {
    var = expr->StringValue();
    return TRUE;
  }
  else
    return FALSE;
}

bool wxExpr::GetAttributeValue(const wxString& att, wxExpr **var) const
{
  wxExpr *expr = AttributeValue(att);
  if (expr)
  {
    *var = expr;
    return TRUE;
  }
  else
    return FALSE;
}

bool wxExpr::GetAttributeValueStringList(const wxString& att, wxList *var) const
{
  wxExpr *expr = AttributeValue(att);
  if (expr && expr->Type() == wxExprList)
  {
    wxExpr *string_expr = expr->value.first;
    while (string_expr)
    {
      if (string_expr->Type() == wxExprString)
        var->Append((wxObject *)copystring(string_expr->StringValue()));

      string_expr = string_expr->next;
    }
	return TRUE;
  }
  else
    return FALSE;
}

// Compatibility
void wxExpr::AssignAttributeValue(wxChar *att, wxChar **var) const
{
  wxString str;
  if (GetAttributeValue(att, str))
  {
    if (*var)
      delete[] *var;
    *var = copystring((const wxChar *) str);
  }
}

void wxExpr::WriteClause(ostream& stream)  // Write this expression as a top-level clause
{
  if (type != wxExprList)
    return;

  wxExpr *node = value.first;
  if (node)
  {
    node->WriteExpr(stream);
    stream << "(";
    node = node->next;
    bool first = TRUE;
    while (node)
    {
      if (!first)
        stream << "  ";
      node->WriteExpr(stream);
      node = node->next;
      if (node) stream << ",\n";
      first = FALSE;
    }
    stream << ").\n\n";
  }
}

void wxExpr::WriteExpr(ostream& stream)    // Write as any other subexpression
{
  // This seems to get round an optimizer bug when
  // using Watcom C++ 10a in WIN32 compilation mode.
  // If these lines not present, the type seems to be
  // interpreted wrongly as an integer.
  // I don't want to turn optimization off since it's needed
  // for reading in files quickly.
#if defined(__WATCOMC__)
  char buf[2];
  sprintf(buf, "");
#endif

  switch (type)
  {
    case wxExprInteger:
    {
      stream << value.integer;
      break;
    }
    case wxExprReal:
    {
      double f = value.real;
/* Now the parser can cope with this.
      // Prevent printing in 'e' notation. Any better way?
      if (fabs(f) < 0.00001)
        f = 0.0;
*/
      char buf[40];
      sprintf(buf, "%.6g", f);
      stream << buf;
      break;
    }
    case wxExprString:
    {
      stream << "\"";
      int i;
      wxWX2MBbuf val = wxConv_libc.cWX2MB(value.string);
      int len = strlen(val);
      for (i = 0; i < len; i++)
      {
        char ch = val[i];
        if (ch == '"' || ch == '\\')
          stream << "\\";
        stream << ch;
      }

      stream << "\"";
      break;
    }
    case wxExprWord:
    {
      bool quote_it = FALSE;
      wxWX2MBbuf val = wxConv_libc.cWX2MB(value.word);
      int len = strlen(val);
      if ((len == 0) || (len > 0 && (val[0] > 64 && val[0] < 91)))
        quote_it = TRUE;
      else
      {
        int i;
        for (i = 0; i < len; i++)
          if ((!isalpha(val[i])) && (!isdigit(val[i])) &&
              (val[i] != '_'))
            { quote_it = TRUE; i = len; }
      }

      if (quote_it)
        stream << "'";

      stream << val;

      if (quote_it)
        stream << "'";

      break;
    }
    case wxExprList:
    {
      if (!value.first)
        stream << "[]";
      else
      {
        wxExpr *expr = value.first;

        if ((expr->Type() == wxExprWord) && (wxStrcmp(expr->WordValue(), _T("=")) == 0))
        {
          wxExpr *arg1 = expr->next;
          wxExpr *arg2 = arg1->next;
          arg1->WriteExpr(stream);
          stream << " = ";
          arg2->WriteExpr(stream);
        }
        else
        {
          stream << "[";
          while (expr)
          {
            expr->WriteExpr(stream);
            expr = expr->next;
            if (expr) stream << ", ";
          }
          stream << "]";
        }
      }
      break;
    }
   case wxExprNull: break;
  }
}

void wxExpr::WriteLispExpr(ostream& stream)
{
  switch (type)
  {
    case wxExprInteger:
    {
      stream << value.integer;
      break;
    }
    case wxExprReal:
    {
      stream << value.real;
      break;
    }
    case wxExprString:
    {
      stream << "\"" << value.string << "\"";
      break;
    }
    case wxExprWord:
    {
      stream << value.word;
      break;
    }
    case wxExprList:
    {
      wxExpr *expr = value.first;

      stream << "(";
      while (expr)
      {
        expr->WriteLispExpr(stream);
        expr = expr->next;
        if (expr) stream << " ";
      }

      stream << ")";
      break;
    }
   case wxExprNull: break;
  }
}

/*
 * wxExpr 'database' (list of expressions)
 */
 
#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxExprDatabase, wxList)
#endif

wxExprDatabase::wxExprDatabase(wxExprErrorHandler handler)
{
  position = NULL;
  hash_table = NULL;
  currentwxExprErrorHandler = handler;
  noErrors = 0;
}

wxExprDatabase::wxExprDatabase(wxExprType type, const wxString& attribute, int size,
                               wxExprErrorHandler handler)
{
  position = NULL;
  attribute_to_hash = attribute;
  if (type == wxExprString)
    hash_table = new wxHashTable(wxKEY_STRING, size);
  else if (type == wxExprInteger)
    hash_table = new wxHashTable(wxKEY_INTEGER, size);
  else hash_table = NULL;

  currentwxExprErrorHandler = handler;
  noErrors = 0;
}

wxExprDatabase::~wxExprDatabase(void)
{
  ClearDatabase();
  if (hash_table)
    delete hash_table;
}

void wxExprDatabase::BeginFind(void)          // Initialise a search
{
  position = First();
}

wxExpr *wxExprDatabase::FindClause(long id)  // Find a term based on an integer id attribute
                                 // e.g. node(id=23, type=rectangle, ....).
{
  wxExpr *found = NULL;
  while (position && !found)
  {
    wxExpr *term = (wxExpr *)position->Data();
    
    if (term->Type() == wxExprList)
    {
      wxExpr *value = term->AttributeValue("id");
      if (value->Type() == wxExprInteger && value->IntegerValue() == id)
        found = term;
    }
    position = position->Next();
  }
  return found;
}

// Find on basis of attribute/value pairs, e.g. type=rectangle
wxExpr *wxExprDatabase::FindClause(const wxString& word, const wxString& val)
{
  wxExpr *found = NULL;
  while (position && !found)
  {
    wxExpr *term = (wxExpr *)position->Data();
    
    if (term->Type() == wxExprList)
    {
      wxExpr *value = term->AttributeValue(word);
      if ((value->Type() == wxExprWord && value->WordValue() == val) ||
          (value->Type() == wxExprString && value->StringValue() == val))
        found = term;
    }
    position = position->Next();
  }
  return found;
}

wxExpr *wxExprDatabase::FindClause(const wxString& word, long val)
{
  wxExpr *found = NULL;
  while (position && !found)
  {
    wxExpr *term = (wxExpr *)position->Data();
    
    if (term->Type() == wxExprList)
    {
      wxExpr *value = term->AttributeValue(word);
      if ((value->Type() == wxExprInteger) && (value->IntegerValue() == val))
        found = term;
    }
    position = position->Next();
  }
  return found;
}

wxExpr *wxExprDatabase::FindClause(const wxString& word, double val)
{
  wxExpr *found = NULL;
  while (position && !found)
  {
    wxExpr *term = (wxExpr *)position->Data();
    
    if (term->Type() == wxExprList)
    {
      wxExpr *value = term->AttributeValue(word);
      if ((value->Type() == wxExprReal) && (value->RealValue() == val))
        found = term;
    }
    position = position->Next();
  }
  return found;
}

wxExpr *wxExprDatabase::FindClauseByFunctor(const wxString& functor)
{
  wxExpr *found = NULL;
  while (position && !found)
  {
    wxExpr *term = (wxExpr *)position->Data();
    
    if (term->Type() == wxExprList)
    {
      if (term->Functor() == functor)
        found = term;
    }
    position = position->Next();
  }
  return found;
}

// If hashing is on, must store in hash table too
void wxExprDatabase::Append(wxExpr *clause)
{
  wxList::Append((wxObject *)clause);
  if (hash_table)
  {
    wxString functor(clause->Functor());
    wxExpr *expr = clause->AttributeValue(attribute_to_hash);
    if (expr)
    {
      long functor_key = hash_table->MakeKey(WXSTRINGCAST functor);
      long value_key = 0;
      if (expr && expr->Type() == wxExprString)
      {
        value_key = hash_table->MakeKey(WXSTRINGCAST expr->StringValue());
        hash_table->Put(functor_key + value_key, WXSTRINGCAST expr->StringValue(), (wxObject *)clause);
      }
      else if (expr && expr->Type() == wxExprInteger)
      {
        value_key = expr->IntegerValue();
        hash_table->Put(functor_key + value_key, expr->IntegerValue(), (wxObject *)clause);
      }

    }
  }
}

wxExpr *wxExprDatabase::HashFind(const wxString& functor, long value) const
{
  long key = hash_table->MakeKey(WXSTRINGCAST functor) + value;

  // The key alone isn't guaranteed to be unique:
  // must supply value too. Let's assume the value of the
  // id is going to be reasonably unique.
  return (wxExpr *)hash_table->Get(key, value);
}

wxExpr *wxExprDatabase::HashFind(const wxString& functor, const wxString& value) const
{
  long key = hash_table->MakeKey(WXSTRINGCAST functor) + hash_table->MakeKey(WXSTRINGCAST value);
  return (wxExpr *)hash_table->Get(key, WXSTRINGCAST value);
}

void wxExprDatabase::ClearDatabase(void)
{
  noErrors = 0;
  wxNode *node = First();
  while (node)
  {
    wxExpr *expr = (wxExpr *)node->Data();
    delete expr;
    delete node;
    node = First();
  }

  if (hash_table)
    hash_table->Clear();
}

bool wxExprDatabase::Read(const wxString& filename)
{
  noErrors = 0;

  FILE *f = fopen(filename.fn_str(), "r");
  if (f)
  {
    thewxExprDatabase = this;

    LexFromFile(f);
    yyparse();
    fclose(f);

    wxExprCleanUp();
    return (noErrors == 0);
  }
  else
  {
    return FALSE;
  }
}

bool wxExprDatabase::ReadFromString(const wxString& buffer)
{
  noErrors = 0;
  thewxExprDatabase = this;

  wxWX2MBbuf buf = buffer.mb_str();
  LexFromString(MBSTRINGCAST buf);
  yyparse();
  wxExprCleanUp();
  return (noErrors == 0);
}

bool wxExprDatabase::Write(const wxString& fileName)
{
  ofstream str(MBSTRINGCAST fileName.mb_str());
  if (str.bad())
    return FALSE;
  return Write(str);
}

bool wxExprDatabase::Write(ostream& stream)
{
  noErrors = 0;
  wxNode *node = First();
  while (node)
  {
    wxExpr *expr = (wxExpr *)node->Data();
    expr->WriteClause(stream);
    node = node->Next();
  }
  return (noErrors == 0);
}

void wxExprDatabase::WriteLisp(ostream& stream)
{
  noErrors = 0;
  wxNode *node = First();
  while (node)
  {
    wxExpr *expr = (wxExpr *)node->Data();
    expr->WriteLispExpr(stream);
    stream << "\n\n";
    node = node->Next();
  }
}

void add_expr(wxExpr * expr)
{
  thewxExprDatabase->Append(expr);
}

// Checks functor
bool wxExprIsFunctor(wxExpr *expr, const wxString& functor)
{
  if (expr && (expr->Type() == wxExprList))
  {
    wxExpr *first_expr = expr->value.first;

    if (first_expr && (first_expr->Type() == wxExprWord) &&
       (first_expr->WordValue() == functor))
      return TRUE;
    else 
      return FALSE;
  } 
  else 
    return FALSE;
}

/*
 * Called from parser
 *
 */

char *wxmake_integer(char *str)
{
  wxExpr *x = new wxExpr(atol(str));

  return (char *)x;
}

char *wxmake_real(char *str1, char *str2)
{
  char buf[50];

  sprintf(buf, "%s.%s", str1, str2);
  double f = (double)atof(buf);
  wxExpr *x = new wxExpr(f);

  return (char *)x;
}

// extern "C" double exp10(double);

char *wxmake_exp(char *str1, char *str2)
{
  double mantissa = (double)atoi(str1);
  double exponent = (double)atoi(str2);

  double d = mantissa * pow(10.0, exponent);

  wxExpr *x = new wxExpr(d);

  return (char *)x;
}

char *wxmake_exp2(char *str1, char *str2, char *str3)
{
  char buf[50];

  sprintf(buf, "%s.%s", str1, str2);
  double mantissa = (double)atof(buf);
  double exponent = (double)atoi(str3);

  double d = mantissa * pow(10.0, exponent);

  wxExpr *x = new wxExpr(d);

  return (char *)x;
}

char *wxmake_word(char *str)
{
  wxExpr *x = new wxExpr(wxExprWord, str);
  return (char *)x;
}

char *wxmake_string(char *str)
{
  wxChar *s, *t;
  int len, i;
  wxMB2WXbuf sbuf = wxConv_libc.cMB2WX(str);

  str++;			/* skip leading quote */
  len = wxStrlen(sbuf) - 1;	/* ignore trailing quote */
    
  s = new wxChar[len + 1];
    
  t = s;
  for(i=0; i<len; i++)
  {
    if (sbuf[i] == _T('\\') && sbuf[i+1] == _T('"'))
    {
      *t++ = _T('"');
      i ++;
    }
    else if (sbuf[i] == _T('\\') && sbuf[i+1] == _T('\\'))
    {
      *t++ = _T('\\');
      i ++;
    }
    else
      *t++ = sbuf[i];
  }

  *t = _T('\0');

  wxExpr *x = new wxExpr(wxExprString, s, FALSE);
  return (char *)x;
}

char *proio_cons(char * ccar, char * ccdr)
{
  wxExpr *car = (wxExpr *)ccar;
  wxExpr *cdr = (wxExpr *)ccdr;

  if (cdr == NULL)
  {
    cdr = new wxExpr(wxExprList);
  }
  if (car)
    cdr->Insert(car);
  return (char *)cdr;
}

void process_command(char * cexpr)
{
  wxExpr *expr = (wxExpr *)cexpr;
  add_expr(expr);
}

void syntax_error(char *WXUNUSED(s))
{
  if (currentwxExprErrorHandler)
    (void)(*(currentwxExprErrorHandler))(WXEXPR_ERROR_SYNTAX, "syntax error");
  if (thewxExprDatabase) thewxExprDatabase->noErrors += 1;
}

#if 0
#ifdef _WINDLL
// char *__cdecl strdup(const char *s)
WXDLLEXPORT char *strdup(const char *s)
{
  int len = strlen(s);
  char *new_s = (char *)malloc(sizeof(char)*(len+1));
  strcpy(new_s, s);
  return new_s;
}
#endif
#endif

