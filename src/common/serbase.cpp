/////////////////////////////////////////////////////////////////////////////
// Name:        serbase.cpp
// Purpose:     wxStream base classes
// Author:      Robert Roebling
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "serbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/serbase.h"
#include "wx/datstrm.h"
#include "wx/objstrm.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_SERIAL

// ----------------------------------------------------------------------------
// wxObject_Serialize
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxObject_Serialize,wxObject)

#if !USE_SHARED_LIBRARY
IMPLEMENT_SERIAL_CLASS(wxList, wxObject)
IMPLEMENT_SERIAL_CLASS(wxHashTable, wxObject)
#endif

void WXSERIAL(wxList)::StoreObject(wxObjectOutputStream& s)
{
  wxList *lst_object = (wxList *)Object();
  wxNode *node = lst_object->First();

  if (s.FirstStage()) {
    while (node) {
      s.AddChild(node->Data());
      node = node->Next(); 
    }
    return;
  }

  wxDataOutputStream data_s(s);

  data_s.Write8(lst_object->GetDeleteContents());
  data_s.Write8(lst_object->GetKeyType());
  data_s.Write32( lst_object->Number() );

  if (lst_object->GetKeyType() == wxKEY_INTEGER) {
    while (node) {
      data_s.Write32(node->GetKeyInteger());
      node = node->Next();
    }
  } else {
    while (node) {
      data_s.WriteString(node->GetKeyString());
      node = node->Next();
    }
  }
}

void WXSERIAL(wxList)::LoadObject(wxObjectInputStream& s)
{
  wxDataInputStream data_s(s);
  wxList *list = (wxList *)Object();
  int number, i;

  list->DeleteContents( data_s.Read8() );
  list->SetKeyType( (wxKeyType) data_s.Read8() );
  number = data_s.Read32();

  if (list->GetKeyType() == wxKEY_INTEGER) {
    for (i=0;i<number;i++)
      list->Append( data_s.Read32(), s.GetChild() );
  } else {
    for (i=0;i<number;i++)
      list->Append( data_s.ReadString(), s.GetChild() );
  }
}

// ----------------------------------------------------------------------------

void WXSERIAL(wxHashTable)::StoreObject(wxObjectOutputStream& s)
{
  wxHashTable *table = (wxHashTable *)Object();
  int i;

  if (s.FirstStage()) {
    for (i=0;i<table->n;i++)
      s.AddChild(table->hash_table[i]);
    return;
  }

  wxDataOutputStream data_s(s);

  data_s.Write8(table->key_type);
  data_s.Write32(table->n);
}

void WXSERIAL(wxHashTable)::LoadObject(wxObjectInputStream& s)
{
  wxHashTable *table = (wxHashTable *)Object();
  wxDataInputStream data_s(s);
  int i, key, n;

  key = data_s.Read8();
  n = data_s.Read32();

  table->Create(key, n);

  for (i=0;i<n;i++)
    table->hash_table[i] = (wxList *)s.GetChild();
}

#endif // wxUSE_SERIAL
