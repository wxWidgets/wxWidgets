/////////////////////////////////////////////////////////////////////////////
// Name:        wxexpr.h
// Purpose:     Prolog-like file I/O, used by resource system.
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXEXPRH__
#define _WX_WXEXPRH__

#ifdef __GNUG__
#pragma interface "wxexpr.h"
#endif

#include <stdio.h>

#include "wx/defs.h"
#include "wx/string.h"

#include "wx/ioswrap.h"

#include "wx/list.h"
#include "wx/hash.h"

#include "wx/expr.h"

// Compatibility
#define PrologExpr wxExpr
#define PrologDatabase wxExprDatabase
#define proioErrorHandler wxExprErrorHandler
#define PROIO_ERROR_GENERAL 1
#define PROIO_ERROR_SYNTAX  2
#define PrologNull wxExprNull
#define PrologInteger wxExprInteger
#define PrologReal wxExprReal
#define PrologWord wxExprWord
#define PrologString wxExprString
#define PrologList wxExprList
#define PrologType wxExprType

// Error types
#define WXEXPR_ERROR_GENERAL 1
#define WXEXPR_ERROR_SYNTAX  2

// Error handler function definition. If app returns TRUE,
// carry on processing.
typedef bool (*wxExprErrorHandler) (int errorType, char *msg);

WXDLLEXPORT_DATA(extern wxExprErrorHandler) currentwxExprErrorHandler;

extern "C" WXDLLEXPORT_DATA(FILE*) yyin;

extern "C" WXDLLEXPORT int yyparse(void);

typedef enum {
    wxExprNull,
    wxExprInteger,
    wxExprReal,
    wxExprWord,
    wxExprString,
    wxExprList
} wxExprType;

class WXDLLEXPORT wxExprDatabase;

class WXDLLEXPORT wxExpr
{
 public:
  wxObject *client_data;
  wxExprType type;
  union {
    long integer;
    char *word;
    char *string;
    double real;
    wxExpr *first;  // If is a list expr, points to the first node
    } value;

  wxExpr *next;     // If this is a node in a list, points to the next node
  wxExpr *last;     // If is a list expr, points to the last node

  wxExpr(wxExprType the_type, char *word_or_string, bool allocate);
  wxExpr(const wxString& functor);      // Assume this is a new clause - pass functor
  wxExpr(wxExprType the_type, const wxString& word_or_string = "");
  wxExpr(long the_integer);
  wxExpr(double the_real);
  wxExpr(wxList *the_list);
  ~wxExpr(void);

  inline wxExprType Type(void) const { return type; }
  inline long IntegerValue(void) const
  {
    if (type == wxExprInteger)
      return value.integer;
    else if (type == wxExprReal)
      return (long)value.real;
    else return 0;
  }

  inline double RealValue(void) const {
    if (type == wxExprReal)
      return value.real;
    else if (type == wxExprInteger)
      return (double)value.integer;
    else return (double)0.0;
  }

  inline wxString WordValue(void) const {
    if (type == wxExprWord)
      return value.word;
    else if (type == wxExprString)
      return wxString(value.string);
    else return wxString("");
  }

  inline wxString StringValue(void) const {
    if (type == wxExprString)
      return wxString(value.string);
    else if (type == wxExprWord)
      return wxString(value.word);
    else return wxString("");
  }

  // Get nth arg of clause (starting from 1)
  wxExpr *Arg(wxExprType type, int arg) const;

  // Return nth argument of a list expression (starting from zero)
  wxExpr *Nth(int arg) const;

  // Returns the number of elements in a list expression
  int Number(void) const;

  // Make a clone
  wxExpr *Copy(void) const;

  wxExpr *GetAttributeValueNode(const wxString& word) const;  // Use only for a clause or list
  wxExpr *AttributeValue(const wxString& word) const;  // Use only for a clause
  wxString Functor(void) const;                     // Only for a clause
  bool IsFunctor(const wxString& s) const;                     // Only for a clause
  void WriteClause(ostream& stream);  // Write this expression as a top-level clause
  void WriteExpr(ostream& stream);    // Write as any other subexpression
  void WriteLispExpr(ostream& stream);

  // Append an expression to a list
  void Append(wxExpr *expr);
  // Insert at beginning of list
  void Insert(wxExpr *expr);

  // Get first expr in list
  inline wxExpr *GetFirst(void) const { return ((type == wxExprList) ? value.first : (wxExpr*)NULL); }

  // Get next expr if this is a node in a list
  inline wxExpr *GetNext(void) const { return next; }

  // Get last expr in list
  inline wxExpr *GetLast(void) const { return ((type == wxExprList) ? last : (wxExpr*)NULL); }

  // This should really be called SetAttributeValue since any existing
  // attribute-value is deleted first.
  void AddAttributeValue(const wxString& attribute, long value);
  void AddAttributeValue(const wxString& attribute, double value);
  void AddAttributeValueWord(const wxString& attribute, const wxString& value);
  void AddAttributeValueString(const wxString& attribute, const wxString& value);
  void AddAttributeValue(const wxString& attribute, wxList *value);
  void AddAttributeValue(const wxString& attribute, wxExpr *value);
  void AddAttributeValueStringList(const wxString& attribute, wxList *string_list);

  void DeleteAttributeValue(const wxString& attribute);

  bool GetAttributeValue(const wxString& att, int& var) const;
  bool GetAttributeValue(const wxString& att, long& var) const;
  bool GetAttributeValue(const wxString& att, float& var) const;
  bool GetAttributeValue(const wxString& att, double& var) const;
  bool GetAttributeValue(const wxString& att, wxString& var) const;  // Word OR string -> string
  bool GetAttributeValue(const wxString& att, wxExpr **var) const;

  // Compatibility with old PrologIO
  inline void AssignAttributeValue(char *att, int *var) const { GetAttributeValue(att, *var); }
  inline void AssignAttributeValue(char *att, long *var) const { GetAttributeValue(att, *var); }
  inline void AssignAttributeValue(char *att, float *var) const { GetAttributeValue(att, *var); }
  inline void AssignAttributeValue(char *att, double *var) const { GetAttributeValue(att, *var); }
  inline void AssignAttributeValue(char *att, wxExpr **var) const { GetAttributeValue(att, var); }
  void AssignAttributeValue(char *att, char **var) const ;  // Word OR string -> string

  // Add string items to list if the list attribute exists
  bool GetAttributeValueStringList(const wxString& att, wxList *var) const;

  // Associate other data with this expression, e.g. when reading in a
  // number of linked items - store C++ object pointer with the expression
  // so we can index into the wxExpr database and fish out the pointer.
  inline void SetClientData(wxObject *data) { client_data = data; }
  inline wxObject *GetClientData(void) const { return client_data; }
};

class WXDLLEXPORT wxExprDatabase: public wxList
{
private:
    wxNode *position;              // Where we are in a search
    wxHashTable *hash_table;
    wxString attribute_to_hash;

public:
    int noErrors;

    wxExprDatabase(wxExprErrorHandler handler = 0);

    // Use hashing on both the functor, and the attribute of
    // specified type (wxExprString or wxExprInteger) and name.
    // So to find node 45
    // (i.e. match the clause node(id=45, ...))
    // it usually requires 1 look-up: the keys for functor and attribute
    // are added together.
    // Obviously if the attribute was missing in a clause, it would
    // fail to be found by this method, but could be retrieved by a
    // linear search using BeginFind and FindClauseByFunctor,
    // or just searching through the list as per usual.

    wxExprDatabase(wxExprType type, const wxString& attribute, int size = 500,
            wxExprErrorHandler handler = 0);

    ~wxExprDatabase(void);

    void BeginFind(void) ;          // Initialise a search
    wxExpr *FindClause(long id) ;  // Find a term based on an integer id attribute
    // e.g. node(id=23, type=rectangle, ....).

    // Find on basis of attribute/value pairs, e.g. type=rectangle
    // This doesn't use hashing; it's a linear search.
    wxExpr *FindClause(const wxString& word, const wxString& value);
    wxExpr *FindClause(const wxString& word, long value);
    wxExpr *FindClause(const wxString& word, double value);
    wxExpr *FindClauseByFunctor(const wxString& functor);

    wxExpr *HashFind(const wxString& functor, const wxString& value) const;
    wxExpr *HashFind(const wxString& functor, long value) const;

    void Append(wxExpr *expr);  // Does cleverer things if hashing is on
    void ClearDatabase(void);
    inline int GetErrorCount() const { return noErrors; }
    bool Read(const wxString& filename);
    bool ReadFromString(const wxString& buffer);
    bool Write(const wxString& fileName);
    bool Write(ostream& stream);
    void WriteLisp(ostream& stream);

    // Compatibility
    inline bool ReadProlog(char *filename) { return Read(wxString(filename)); }
    inline bool ReadPrologFromString(char *buffer) { return ReadFromString(wxString(buffer)); }
    inline void WriteProlog(ostream& stream) { Write(stream); }

private:
    DECLARE_DYNAMIC_CLASS(wxExprDatabase)
};

// Function call-style interface - some more convenience wrappers/unwrappers

// Make a call
WXDLLEXPORT wxExpr* wxExprMakeCall(const wxString& functor ...);

#define wxExprMakeInteger(x) (new wxExpr((long)x))
#define wxExprMakeReal(x) (new wxExpr((double)x))
#define wxExprMakeString(x) (new wxExpr(wxExprString, x))
#define wxExprMakeWord(x)   (new wxExpr(wxExprWord, x))
#define wxExprMake(x)       (new wxExpr(x))

// Checks functor
WXDLLEXPORT bool wxExprIsFunctor(wxExpr *expr, const wxString& functor);

// Temporary variable for communicating between wxexpr.cpp and YACC/LEX
WXDLLEXPORT_DATA(extern wxExprDatabase*) thewxExprDatabase;

// YACC/LEX can leave memory lying around...
extern "C" WXDLLEXPORT int wxExprCleanUp();

#endif

