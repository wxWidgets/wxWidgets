/////////////////////////////////////////////////////////////////////////////
// Name:        hash.cpp
// Purpose:     wxHashTable implementation
// Author:      Julian Smart
// Modified by: VZ at 25.02.00: type safe hashes with WX_DECLARE_HASH()
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "hash.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/list.h"
#endif

#include "wx/hash.h"

#include <string.h>
#include <stdarg.h>

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxHashTable, wxObject)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxHashTablleBase for working with "void *" data
// ----------------------------------------------------------------------------

wxHashTableBase::wxHashTableBase()
{
    m_deleteContents = FALSE;
    m_hashTable = (wxListBase **)NULL;
    m_hashSize = 0;
    m_count = 0;
    m_keyType = wxKEY_NONE;
}

void wxHashTableBase::Create(wxKeyType keyType, size_t size)
{
    Destroy();

    m_hashSize = size;
    m_keyType = keyType;
    m_hashTable = new wxListBase *[size];
    for ( size_t n = 0; n < m_hashSize; n++ )
    {
        m_hashTable[n] = (wxListBase *) NULL;
    }
}

void wxHashTableBase::Destroy()
{
    if ( m_hashTable )
    {
        for ( size_t n = 0; n < m_hashSize; n++ )
        {
            delete m_hashTable[n];
        }

        delete [] m_hashTable;

        m_hashTable = (wxListBase **)NULL;

        m_count = 0;
    }
}

void wxHashTableBase::DeleteContents(bool flag)
{
    m_deleteContents = flag;
    for ( size_t n = 0; n < m_hashSize; n++ )
    {
        if ( m_hashTable[n] )
        {
            m_hashTable[n]->DeleteContents(flag);
        }
    }
}

wxNodeBase *wxHashTableBase::GetNode(long key, long value) const
{
    size_t slot = (size_t)abs(key % m_hashSize);

    wxNodeBase *node;
    if ( m_hashTable[slot] )
    {
        node = m_hashTable[slot]->Find(wxListKey(value));
    }
    else
    {
        node = (wxNodeBase *)NULL;
    }

    return node;
}

// ----------------------------------------------------------------------------
// wxHashTableLong
// ----------------------------------------------------------------------------

wxHashTableLong::~wxHashTableLong()
{
    Destroy();
}

void wxHashTableLong::Init(size_t size)
{
    m_hashSize = size;
    m_values = new wxArrayLong *[size];
    m_keys = new wxArrayLong *[size];

    for ( size_t n = 0; n < m_hashSize; n++ )
    {
        m_values[n] =
        m_keys[n] = (wxArrayLong *)NULL;
    }

    m_count = 0;
}

void wxHashTableLong::Destroy()
{
    for ( size_t n = 0; n < m_hashSize; n++ )
    {
        delete m_values[n];
        delete m_keys[n];
    }

    delete [] m_values;
    delete [] m_keys;
    m_hashSize = 0;
    m_count = 0;
}

void wxHashTableLong::Put(long key, long value)
{
    wxCHECK_RET( m_hashSize, _T("must call Create() first") );

    size_t slot = (size_t)abs(key % m_hashSize);

    if ( !m_keys[slot] )
    {
        m_keys[slot] = new wxArrayLong;
        m_values[slot] = new wxArrayLong;
    }

    m_keys[slot]->Add(key);
    m_values[slot]->Add(value);

    m_count++;
}

long wxHashTableLong::Get(long key) const
{
    wxCHECK_MSG( m_hashSize, wxNOT_FOUND, _T("must call Create() first") );

    size_t slot = (size_t)abs(key % m_hashSize);

    wxArrayLong *keys = m_keys[slot];
    if ( keys )
    {
        size_t count = keys->GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            if ( keys->Item(n) == key )
            {
                return m_values[slot]->Item(n);
            }
        }
    }

    return wxNOT_FOUND;
}

long wxHashTableLong::Delete(long key)
{
    wxCHECK_MSG( m_hashSize, wxNOT_FOUND, _T("must call Create() first") );

    size_t slot = (size_t)abs(key % m_hashSize);

    wxArrayLong *keys = m_keys[slot];
    if ( keys )
    {
        size_t count = keys->GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            if ( keys->Item(n) == key )
            {
                long val = m_values[slot]->Item(n);

                keys->RemoveAt(n);
                m_values[slot]->RemoveAt(n);

                m_count--;

                return val;
            }
        }
    }

    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// old not type safe wxHashTable
// ----------------------------------------------------------------------------

wxHashTable::wxHashTable (int the_key_type, int size)
{
  n = 0;
  hash_table = (wxList**) NULL;
  Create(the_key_type, size);
  m_count = 0;
  m_deleteContents = FALSE;
/*
  n = size;
  current_position = -1;
  current_node = (wxNode *) NULL;

  key_type = the_key_type;
  hash_table = new wxList *[size];
  int i;
  for (i = 0; i < size; i++)
    hash_table[i] = (wxList *) NULL;
*/
}

wxHashTable::~wxHashTable ()
{
  Destroy();
}

void wxHashTable::Destroy()
{
  if (!hash_table) return;
  int i;
  for (i = 0; i < n; i++)
    if (hash_table[i])
      delete hash_table[i];
  delete[] hash_table;
  hash_table = NULL;
}

bool wxHashTable::Create(int the_key_type, int size)
{
  Destroy();

  n = size;
  current_position = -1;
  current_node = (wxNode *) NULL;

  key_type = the_key_type;
  hash_table = new wxList *[size];
  int i;
  for (i = 0; i < size; i++)
    hash_table[i] = (wxList *) NULL;
  return TRUE;
}


void wxHashTable::DoCopy(const wxHashTable& table)
{
  n = table.n;
  current_position = table.current_position;
  current_node = NULL; // doesn't matter - Next() will reconstruct it
  key_type = table.key_type;

  hash_table = new wxList *[n];
  for (int i = 0; i < n; i++) {
    if (table.hash_table[i] == NULL)
      hash_table[i] = NULL;
    else {
      hash_table[i] = new wxList(key_type);
      *(hash_table[i]) = *(table.hash_table[i]);
    }
  }
}

void wxHashTable::Put (long key, long value, wxObject * object)
{
  // Should NEVER be
  long k = (long) key;

  int position = (int) (k % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
  {
    hash_table[position] = new wxList (wxKEY_INTEGER);
    if (m_deleteContents) hash_table[position]->DeleteContents(TRUE);
  }

  hash_table[position]->Append (value, object);
  m_count++;
}

void wxHashTable::Put (long key, const wxChar *value, wxObject * object)
{
  // Should NEVER be
  long k = (long) key;

  int position = (int) (k % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
  {
    hash_table[position] = new wxList (wxKEY_INTEGER);
    if (m_deleteContents) hash_table[position]->DeleteContents(TRUE);
  }

  hash_table[position]->Append (value, object);
  m_count++;
}

void wxHashTable::Put (long key, wxObject * object)
{
  // Should NEVER be
  long k = (long) key;

  int position = (int) (k % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
  {
    hash_table[position] = new wxList (wxKEY_INTEGER);
    if (m_deleteContents) hash_table[position]->DeleteContents(TRUE);
  }

  hash_table[position]->Append (k, object);
  m_count++;
}

void wxHashTable::Put (const wxChar *key, wxObject * object)
{
  int position = (int) (MakeKey (key) % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
  {
    hash_table[position] = new wxList (wxKEY_STRING);
    if (m_deleteContents) hash_table[position]->DeleteContents(TRUE);
  }

  hash_table[position]->Append (key, object);
  m_count++;
}

wxObject *wxHashTable::Get (long key, long value) const
{
  // Should NEVER be
  long k = (long) key;

  int position = (int) (k % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (value);
      if (node)
        return node->Data ();
      else
        return (wxObject *) NULL;
    }
}

wxObject *wxHashTable::Get (long key, const wxChar *value) const
{
  // Should NEVER be
  long k = (long) key;

  int position = (int) (k % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (value);
      if (node)
        return node->Data ();
      else
        return (wxObject *) NULL;
    }
}

wxObject *wxHashTable::Get (long key) const
{
  // Should NEVER be
  long k = (long) key;

  int position = (int) (k % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (k);
      return node ? node->Data () : (wxObject*)NULL;
    }
}

wxObject *wxHashTable::Get (const wxChar *key) const
{
  int position = (int) (MakeKey (key) % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (key);
      return node ? node->Data () : (wxObject*)NULL;
    }
}

wxObject *wxHashTable::Delete (long key)
{
  // Should NEVER be
  long k = (long) key;

  int position = (int) (k % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (k);
      if (node)
        {
          wxObject *data = node->Data ();
          delete node;
          m_count--;
          return data;
        }
      else
        return (wxObject *) NULL;
    }
}

wxObject *wxHashTable::Delete (const wxChar *key)
{
  int position = (int) (MakeKey (key) % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (key);
      if (node)
        {
          wxObject *data = node->Data ();
          delete node;
          m_count--;
          return data;
        }
      else
        return (wxObject *) NULL;
    }
}

wxObject *wxHashTable::Delete (long key, int value)
{
  // Should NEVER be
  long k = (long) key;

  int position = (int) (k % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (value);
      if (node)
        {
          wxObject *data = node->Data ();
          delete node;
          m_count--;
          return data;
        }
      else
        return (wxObject *) NULL;
    }
}

wxObject *wxHashTable::Delete (long key, const wxChar *value)
{
  int position = (int) (key % n);
  if (position < 0) position = -position;

  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (value);
      if (node)
        {
          wxObject *data = node->Data ();
          delete node;
          m_count--;
          return data;
        }
      else
        return (wxObject *) NULL;
    }
}

long wxHashTable::MakeKey (const wxChar *string) const
{
  long int_key = 0;

  while (*string)
    int_key += (wxUChar) *string++;

  return int_key;
}

void wxHashTable::BeginFind ()
{
  current_position = -1;
  current_node = (wxNode *) NULL;
}

wxNode *wxHashTable::Next ()
{
  wxNode *found = (wxNode *) NULL;
  bool end = FALSE;
  while (!end && !found)
    {
      if (!current_node)
        {
          current_position++;
          if (current_position >= n)
            {
              current_position = -1;
              current_node = (wxNode *) NULL;
              end = TRUE;
            }
          else
            {
              if (hash_table[current_position])
                {
                  current_node = hash_table[current_position]->First ();
                  found = current_node;
                }
            }
        }
      else
        {
          current_node = current_node->Next ();
          found = current_node;
        }
    }
  return found;
}

void wxHashTable::DeleteContents (bool flag)
{
  int i;
  m_deleteContents = flag;
  for (i = 0; i < n; i++)
    {
      if (hash_table[i])
        hash_table[i]->DeleteContents (flag);
    }
}

void wxHashTable::Clear ()
{
  int i;
  for (i = 0; i < n; i++)
    {
      if (hash_table[i])
        hash_table[i]->Clear ();
    }
  m_count = 0;
}

