/////////////////////////////////////////////////////////////////////////////
// Name:        list.h
// Purpose:     wxList, wxStringList classes
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __LISTH__
#define __LISTH__

#ifdef __GNUG__
#pragma interface "list.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"

class WXDLLEXPORT wxList;

#define wxKEY_NONE    0
#define wxKEY_INTEGER 1
#define wxKEY_STRING  2
class WXDLLEXPORT wxNode: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxNode)
 private:
 
  wxObject *data;
  wxNode *next;
  wxNode *previous;

 public:
  wxList *list;

  // Optional key stuff
  union
  {
    long integer;
    char *string;
  } key;

  wxNode(wxList *the_list = NULL, wxNode *last_one = NULL, wxNode *next_one = NULL, wxObject *object = NULL);
  wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one,
         wxObject *object, long the_key);
  wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one,
         wxObject *object, const char *the_key);
  ~wxNode(void);

  inline wxNode *Next(void) const { return next; }
  inline wxNode *Previous(void) const { return previous; }
  inline wxObject *Data(void) const { return (wxObject *)data; }
  inline void SetData(wxObject *the_data) { data = the_data; }
};

// type of compare function for list sort operation (as in 'qsort')
typedef int (*wxSortCompareFunction)(const void *elem1, const void *elem2);
typedef int (*wxListIterateFunction)(wxObject *o);

class WXDLLEXPORT wxList: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxList)

 public:
  int n;
  int destroy_data;
  wxNode *first_node;
  wxNode *last_node;
  unsigned int key_type;

  wxList(void);
  wxList(const unsigned int the_key_type);
  wxList(int N, wxObject *Objects[]);
  wxList(wxObject *object, ...);
  
#ifdef USE_STORABLE_CLASSES
  wxList( istream &stream, char *data );
  virtual void StoreObject( ostream &stream );
#endif
  
  ~wxList(void);

  inline int Number(void) const { return n; }

  // Append to end of list
  wxNode *Append(wxObject *object);

  // Insert at front of list
  wxNode *Insert(wxObject *object);

  // Insert before given node
  wxNode *Insert(wxNode *position, wxObject *object);

  // Keyed append
  wxNode *Append(long key, wxObject *object);
  wxNode *Append(const char *key, wxObject *object);

  bool DeleteNode(wxNode *node);
  bool DeleteObject(wxObject *object);  // Finds object pointer and
                                        // deletes node (and object if
                                        // DeleteContents is on)
  void Clear(void);                     // Delete all nodes

  inline wxNode *First(void) const { return first_node; }
  inline wxNode *Last(void) const { return last_node; }
  wxNode *Nth(int i) const;                  // nth node counting from 0

  // Keyed search
  wxNode *Find(long key) const;
  wxNode *Find(const char *key) const;

  wxNode *Member(wxObject *object) const;

  inline void DeleteContents(int destroy) { destroy_data = destroy; }
                                             // Instruct it to destroy user data
                                             // when deleting nodes
  // this function allows the sorting of arbitrary lists by giving
  // a function to compare two list elements.
  void Sort(const wxSortCompareFunction compfunc);

  wxObject *FirstThat(wxListIterateFunction func);
  void ForEach(wxListIterateFunction func);
  wxObject *LastThat(wxListIterateFunction func);
};

// String list class. N.B. this always copies strings
// with Add and deletes them itself.
class WXDLLEXPORT wxStringList: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxStringList)

 public:
  wxStringList(void);
  wxStringList(const char *first ...);
  ~wxStringList(void);

  virtual wxNode *Add(const char *s);
  virtual void Delete(const char *s);
  virtual char **ListToArray(bool new_copies = FALSE) const;
  virtual void Sort(void);
  virtual bool Member(const char *s) const;
};

#endif
    // __LISTH__
