#ifdef __GNUG__
#pragma implementation "sercore.h"
#endif
#include <wx/objstrm.h>
#include <wx/datstrm.h>
#include "sercore.h"

IMPLEMENT_SERIAL_CLASS(wxList,wxObject)

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

  data_s.Write8(lst_object->destroy_data);
  data_s.Write8(lst_object->key_type);
  data_s.Write32( lst_object->Number() );

  if (lst_object->key_type == wxKEY_INTEGER) {
    while (node) {
      data_s.Write32(node->key.integer);
      node = node->Next();
    }
  } else {
    while (node) {
      data_s.WriteString(node->key.string);
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
  list->key_type = data_s.Read8();
  number = data_s.Read32();

  if (list->key_type == wxKEY_INTEGER) {
    for (i=0;i<number;i++)
      list->Append( data_s.Read32(), s.GetChild(i) );
  } else {
    for (i=0;i<number;i++)
      list->Append( data_s.ReadString(), s.GetChild(i) );
  }
}
