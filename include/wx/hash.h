/////////////////////////////////////////////////////////////////////////////
// Name:        hash.h
// Purpose:     wxHashTable class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXHASHH__
#define __WXHASHH__

#ifdef __GNUG__
#pragma interface "hash.h"
#endif

#include "wx/object.h"
#include "wx/list.h"

/*
 * A hash table is an array of user-definable size with lists
 * of data items hanging off the array positions.  Usually there'll
 * be a hit, so no search is required; otherwise we'll have to run down
 * the list to find the desired item.
*/

class WXDLLEXPORT wxHashTable: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxHashTable)

 public:
  int n;
  int current_position;
  wxNode *current_node;

  unsigned int key_type;
  wxList **hash_table;

  wxHashTable(int the_key_type = wxKEY_INTEGER, int size = 1000);
  ~wxHashTable(void);

  bool Create(int the_key_type = wxKEY_INTEGER, int size = 1000);

  // Note that there are 2 forms of Put, Get.
  // With a key and a value, the *value* will be checked
  // when a collision is detected. Otherwise, if there are
  // 2 items with a different value but the same key,
  // we'll retrieve the WRONG ONE. So where possible,
  // supply the required value along with the key.
  // In fact, the value-only versions make a key, and still store
  // the value. The use of an explicit key might be required
  // e.g. when combining several values into one key.
  // When doing that, it's highly likely we'll get a collision,
  // e.g. 1 + 2 = 3, 2 + 1 = 3.

  // key and value are NOT necessarily the same
  void Put(long key, long value, wxObject *object);
  void Put(long key, const char *value, wxObject *object);

  // key and value are the same
  void Put(long value, wxObject *object);
  void Put(const char *value, wxObject *object);

  // key and value not the same
  wxObject *Get(long key, long value) const;
  wxObject *Get(long key, const char *value) const;

  // key and value are the same
  wxObject *Get(long value) const;
  wxObject *Get(const char *value) const;

  // Deletes entry and returns data if found
  wxObject *Delete(long key);
  wxObject *Delete(const char *key);

  wxObject *Delete(long key, int value);
  wxObject *Delete(long key, const char *value);

  // Construct your own integer key from a string, e.g. in case
  // you need to combine it with something
  long MakeKey(const char *string) const;

  // Way of iterating through whole hash table (e.g. to delete everything)
  // Not necessary, of course, if you're only storing pointers to
  // objects maintained separately

  void BeginFind(void);
  wxNode *Next(void);

  void DeleteContents(bool flag);
  void Clear(void);

};

#endif
    // __WXHASHH__
