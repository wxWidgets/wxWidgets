/////////////////////////////////////////////////////////////////////////////
// Name:        hash.cpp
// Purpose:     wxHashTable implementation
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxHashTable, wxObject)
#endif

wxHashTable::wxHashTable (int the_key_type, int size)
{
  n = size;
  current_position = -1;
  current_node = (wxNode *) NULL;

  key_type = the_key_type;
  hash_table = new wxList *[size];
  int i;
  for (i = 0; i < size; i++)
    hash_table[i] = (wxList *) NULL;
}

wxHashTable::~wxHashTable (void)
{
  Destroy();
}

void wxHashTable::Destroy(void)
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
  n = size;
  current_position = -1;
  current_node = (wxNode *) NULL;

  key_type = the_key_type;
  if (hash_table)
    delete[] hash_table;
  hash_table = new wxList *[size];
  int i;
  for (i = 0; i < size; i++)
    hash_table[i] = (wxList *) NULL;
  return TRUE;
}

void wxHashTable::Put (long key, long value, wxObject * object)
{
  // Should NEVER be
  long k = (long) key;
  if (k < 0)
    k = -k;

  int position = (int) (k % n);
  if (!hash_table[position])
    hash_table[position] = new wxList (wxKEY_INTEGER);

  hash_table[position]->Append (value, object);
}

void wxHashTable::Put (long key, const wxChar *value, wxObject * object)
{
  // Should NEVER be
  long k = (long) key;
  if (k < 0)
    k = -k;

  int position = (int) (k % n);
  if (!hash_table[position])
    hash_table[position] = new wxList (wxKEY_INTEGER);

  hash_table[position]->Append (value, object);
}

void wxHashTable::Put (long key, wxObject * object)
{
  // Should NEVER be
  long k = (long) key;
  if (k < 0)
    k = -k;

  int position = (int) (k % n);
  if (!hash_table[position])
    hash_table[position] = new wxList (wxKEY_INTEGER);

  hash_table[position]->Append (k, object);
}

void wxHashTable::Put (const wxChar *key, wxObject * object)
{
  int position = (int) (MakeKey (key) % n);

  if (!hash_table[position])
    hash_table[position] = new wxList (wxKEY_STRING);

  hash_table[position]->Append (key, object);
}

wxObject *wxHashTable::Get (long key, long value) const
{
  // Should NEVER be
  long k = (long) key;
  if (k < 0)
    k = -k;

  int position = (int) (k % n);
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
  if (k < 0)
    k = -k;

  int position = (int) (k % n);
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
  if (k < 0)
    k = -k;

  int position = (int) (k % n);
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
  if (k < 0)
    k = -k;

  int position = (int) (k % n);
  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (k);
      if (node)
	{
	  wxObject *data = node->Data ();
	  delete node;
	  return data;
	}
      else
	return (wxObject *) NULL;
    }
}

wxObject *wxHashTable::Delete (const wxChar *key)
{
  int position = (int) (MakeKey (key) % n);
  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (key);
      if (node)
	{
	  wxObject *data = node->Data ();
	  delete node;
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
  if (k < 0)
    k = -k;

  int position = (int) (k % n);
  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (value);
      if (node)
	{
	  wxObject *data = node->Data ();
	  delete node;
	  return data;
	}
      else
	return (wxObject *) NULL;
    }
}

wxObject *wxHashTable::Delete (long key, const wxChar *value)
{
  int position = (int) (key % n);
  if (!hash_table[position])
    return (wxObject *) NULL;
  else
    {
      wxNode *node = hash_table[position]->Find (value);
      if (node)
	{
	  wxObject *data = node->Data ();
	  delete node;
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

void wxHashTable::BeginFind (void)
{
  current_position = -1;
  current_node = (wxNode *) NULL;
}

wxNode *wxHashTable::Next (void)
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
  for (i = 0; i < n; i++)
    {
      if (hash_table[i])
	hash_table[i]->DeleteContents (flag);
    }
}

void wxHashTable::Clear (void)
{
  int i;
  for (i = 0; i < n; i++)
    {
      if (hash_table[i])
	hash_table[i]->Clear ();
    }
}

