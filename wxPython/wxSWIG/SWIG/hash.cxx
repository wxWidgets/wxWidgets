/*******************************************************************************
 * Simplified Wrapper and Interface Generator  (SWIG)
 * 
 * Author : David Beazley
 *
 * Department of Computer Science        
 * University of Chicago
 * 1100 E 58th Street
 * Chicago, IL  60637
 * beazley@cs.uchicago.edu
 *
 * Please read the file LICENSE for the copyright and terms by which SWIG
 * can be used and distributed.
 *******************************************************************************/

#include "internal.h"

/*******************************************************************************
 * $Header$
 *
 * File : hash.cxx
 *
 * A very simple Hash table class.  Could probably make it more elegant with
 * templates, but templates are pure evil...
 *******************************************************************************/

#define INIT_SIZE   119

// -----------------------------------------------------------------------------
// Hash::Hash()
//
// Constructor.  Creates a new hash table. 
// 
// Inputs : None
//
// Output : New Hash object.
//
// Side Effects : None
// -----------------------------------------------------------------------------

Hash::Hash() {
  int i;
  hashsize = INIT_SIZE;
  hashtable = new Node *[hashsize];
  for (i = 0; i < hashsize; i++) {
    hashtable[i] = 0;
  }
  index = -1;
  current = 0;
}

// -----------------------------------------------------------------------------
// Hash::~Hash()
//
// Destructor.
// 
// Inputs : None
//
// Output : None
//
// Side Effects : Total destruction.
// -----------------------------------------------------------------------------

Hash::~Hash() {
  int   i;
  Node *n,*next;

  for (i = 0; i < hashsize; i++) {
    if (hashtable[i]) {
      n = hashtable[i];
      while (n) {
	next = n->next;
	delete n;
	n = next;
      }
    }
  }
  delete [] hashtable;
}

// -----------------------------------------------------------------------------
// int Hash::h1(const char *key)
// 
// Hashing function.
//
// Inputs : ASCII character string.
//
// Output : Hash table index.
//
// Side Effects : None
// -----------------------------------------------------------------------------

int Hash::h1(const char *key) {
  int h = 0;
  const char *c;

  c = key;
  while (*c) {
    h = (128*h + *c) % hashsize;
    c++;
  }
  return h;
}

// -----------------------------------------------------------------------------
// int Hash::add(const char *k, void *obj)
// 
// Adds a new object to the hash table.
//
// Inputs : 
//          k     = Hash key
//          obj   = Pointer to an object
//
// Output : 0 on success, -1 if item is already in hash table.
//
// Side Effects : 
//          Makes a new hash table entry.
// -----------------------------------------------------------------------------

int Hash::add(const char *k, void *obj) {

  int  hv;
  Node *n,*prev;

  hv = h1(k);                                  // Get hash value
  n = hashtable[hv];
  prev = n;
  while (n) {
    if (strcmp(n->key,k) == 0) return -1;      // Already in hash table
    prev = n;
    n = n->next;
  }

  // Safe to add this to the table

  n = new Node(k,obj,0);
  if (prev) prev->next = n;
  else hashtable[hv] = n;
  return 0;
}

// -----------------------------------------------------------------------------
// int Hash::add(const char *k, void *obj, void (*d)(void *))
// 
// Adds a new object to the hash table.  Allows additional specification of
// a callback function for object deletion.
//
// Inputs :
//          k       = Hash key
//          obj     = Object pointer
//          d       = Deletion function
//
// Output : 0 on success, -1 if item is already in hash table.
//
// Side Effects :
//          Adds an entry to the hash table
// -----------------------------------------------------------------------------

int Hash::add(const char *k, void *obj, void (*d)(void *)) {

  int  hv;
  Node *n,*prev;

  hv = h1(k);                                  // Get hash value
  n = hashtable[hv];
  prev = n;
  while (n) {
    if (strcmp(n->key,k) == 0) return -1;      // Already in hash table
    prev = n;
    n = n->next;
  }

  // Safe to add this to the table

  n = new Node(k,obj,d);
  if (prev) prev->next = n;
  else hashtable[hv] = n;
  return 0;
}

// -----------------------------------------------------------------------------
// void *Hash::lookup(const char *k)
// 
// Looks up a value in the hash table.  Returns a pointer to the object if found.
//
// Inputs : k   = key value
//
// Output : Pointer to object or NULL if not found.
//
// Side Effects : None
// -----------------------------------------------------------------------------

void *Hash::lookup(const char *k) {
  int hv;
  Node *n;

  hv = h1(k);                                // Get hash value
  n = hashtable[hv];

  while (n) {
    if (strcmp(n->key,k) == 0) return n->object;
    n = n->next;
  }

  return 0;
}

// -----------------------------------------------------------------------------
// void Hash::remove(const char *k)
//
// Removes an item from the hash table.  Does nothing if item isn't in the
// hash table to begin with.
// 
// Inputs : k = Key value
//
// Output : None
//
// Side Effects : Deletes item from hash table.
// -----------------------------------------------------------------------------

void Hash::remove(const char *k) {

  int  hv;
  Node *n,*prev;

  hv = h1(k);                                  // Get hash value
  n = hashtable[hv];
  prev = 0;
  while (n) {
    if (strcmp(n->key,k) == 0) {
      // Found it, kill the thing
      if (prev) {
	prev->next = n->next;
      } else {
	hashtable[hv] = n->next;
      }
      delete n;
      return;
    }
    prev = n;
    n = n->next;
  }
}

// -----------------------------------------------------------------------------
// void *Hash::first()
//
// Gets the first item from the hash table or NULL if empty.
// 
// Inputs : None
//
// Output : First object in hash table or NULL if hash table is empty.
//
// Side Effects : Resets an internal iterator variable on the hash table.
// -----------------------------------------------------------------------------

void *Hash::first() {
  index = 0;
  current = 0;

  while (!hashtable[index] && (index < hashsize))
    index++;

  if (index >= hashsize) return 0;
  current = hashtable[index];
  return current->object;
}


// -----------------------------------------------------------------------------
// char *Hash::firstkey()
//
// Gets the first key from the hash table or NULL if empty.
// 
// Inputs : None
//
// Output : First key in hash table or NULL if hash table is empty.
//
// Side Effects : Resets an internal iterator variable on the hash table.
// -----------------------------------------------------------------------------

char *Hash::firstkey() {
  index = 0;
  current = 0;

  while ((index < hashsize) && (!hashtable[index]))
    index++;

  if (index >= hashsize) return 0;
  current = hashtable[index];
  return current->key;
}

// -----------------------------------------------------------------------------
// void *Hash::next()
// 
// Returns the next item in the hash table or NULL if there are no more entries.
// A call to first() should generally be made before using this function.
//
// Inputs : None
//
// Output : Pointer to next object or NULL if there are no more objects.
//
// Side Effects : Updates an iterator variable private to the hash table.
// -----------------------------------------------------------------------------

void *Hash::next() {
  if (index < 0) return first();
  
  // Try to move to the next entry

  current = current->next;

  if (current) {
    return current->object;
  } else {
    index++;
    while ((index < hashsize) && (!hashtable[index]))
      index++;
    if (index >= hashsize) return 0;
    current = hashtable[index];
    return current->object;
  }
}


// -----------------------------------------------------------------------------
// char *Hash::nextkey()
// 
// Returns the next key in the hash table or NULL if there are no more entries.
// A call to firstkey() should generally be made before using this function.
//
// Inputs : None
//
// Output : Pointer to next key or NULL if there are no more objects.
//
// Side Effects : Updates an iterator variable private to the hash table.
// -----------------------------------------------------------------------------

char *Hash::nextkey() {
  if (index < 0) return firstkey();
  
  // Try to move to the next entry

  current = current->next;

  if (current) {
    return current->key;
  } else {
    index++;
    while (!hashtable[index] && (index < hashsize))
      index++;
    if (index >= hashsize) return 0;
    current = hashtable[index];
    return current->key;
  }
}


