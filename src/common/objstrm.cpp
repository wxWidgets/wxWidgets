/////////////////////////////////////////////////////////////////////////////
// Name:        objstrm.cpp
// Purpose:     wxObjectStream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     16/07/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "objstrm.h"
#endif

#include "wx/object.h"
#include "wx/objstrm.h"
#include "wx/datstrm.h"

#define WXOBJ_BEGIN "OBEGIN"
#define WXOBJ_BEG_LEN 6

// ----------------------------------------------------------------------------
// wxObjectOutputStream
// ----------------------------------------------------------------------------

wxObjectOutputStream::wxObjectOutputStream(wxOutputStream& s)
  : wxFilterOutputStream(s)
{
  m_saving = FALSE;
}

wxString wxObjectOutputStream::GetObjectName(wxObject *obj)
{
  wxString name;

  name.Printf("%x", (unsigned long)obj);
  return name;
}

void wxObjectOutputStream::WriteObjectDef(wxObjectStreamInfo& info)
{
  wxDataOutputStream data_s(*this); 

  Write(WXOBJ_BEGIN, WXOBJ_BEG_LEN);
  data_s.WriteString(info.object->GetClassInfo()->GetClassName());
  data_s.WriteString(GetObjectName(info.object));
  // I assume an object will not have millions of children
  data_s.Write8(info.children.Number());
}

void wxObjectOutputStream::AddChild(wxObject *obj)
{
  wxObjectStreamInfo *info;

  if (!FirstStage())
    return;

  info = new wxObjectStreamInfo;
  info->n_children = 0;
  info->object = obj;
  info->parent = m_current_info->object; // Not useful here.
  m_current_info->n_children++;
  m_current_info->children.Append(info);
}

void wxObjectOutputStream::ProcessObjectDef(wxObjectStreamInfo *info)
{
  wxNode *node;

  m_current_info = info;
  // First stage: get children of obj
  info->object->StoreObject(*this);

  // Prepare and write the sub-entry about the child obj.
  WriteObjectDef(*info);

  node = info->children.First();
  
  while (node) {
    ProcessObjectDef((wxObjectStreamInfo *)node->Data());
    node = node->Next();
  }
}

void wxObjectOutputStream::ProcessObjectData(wxObjectStreamInfo *info)
{
  wxNode *node = info->children.First();

  m_current_info = info;

  info->object->StoreObject(*this);

  while (node) {
    ProcessObjectData((wxObjectStreamInfo *)node->Data());
    node = node->Next();
  }
}

bool wxObjectOutputStream::SaveObject(wxObject& obj)
{
  wxObjectStreamInfo info;

  if (m_saving)
    return FALSE;

  m_saving = TRUE;

  // First stage
  m_stage = 0;
  info.object = &obj;
  info.n_children = 0;
  ProcessObjectDef(&info);

  m_stage = 1;
  ProcessObjectData(&info);

  info.children.Clear();

  m_saving = FALSE;

  return TRUE;
}

// ----------------------------------------------------------------------------
// wxObjectInputStream
// ----------------------------------------------------------------------------

wxObjectInputStream::wxObjectInputStream(wxInputStream& s)
  : wxFilterInputStream(s)
{
}

wxObject *wxObjectInputStream::SolveName(const wxString& name) const
{
  wxNode *node = m_solver.First();
  wxObjectStreamInfo *info;

  while (node) {
    info = (wxObjectStreamInfo *)node->Data();
    if (info->object_name == name)
      return info->object;

    node = node->Next();
  }
  return NULL;
}

wxObject *wxObjectInputStream::GetChild(int no) const
{
  return m_current_info->children.Nth(no);
}

bool wxObjectInputStream::ReadObjectDef(wxObjectStreamInfo *info)
{
  wxDataInputStream data_s(*this);
  char sig[WXOBJ_BEG_LEN+1];

  Read(sig, WXOBJ_BEG_LEN);
  sig[WXOBJ_BEG_LEN] = 0;
  if (wxString(sig) != WXOBJ_BEGIN)
    return FALSE;
  info->object = wxCreateDynamicObject( WXSTRINGCAST data_s.ReadString());
  info->object_name = data_s.ReadString();
  info->n_children = data_s.Read8();
  info->children = wxList();

  return TRUE;
}

wxObjectStreamInfo *wxObjectInputStream::ProcessObjectDef(wxObjectStreamInfo *parent)
{
  wxObjectStreamInfo *info, *c_info;
  int c;

  info = new wxObjectStreamInfo;
  info->parent = parent;
  info->children.DeleteContents(TRUE);

  m_solver.Append(info);

  if (!ReadObjectDef(info))
    return NULL;

  for (c=0;c<info->n_children;c++) {
    c_info = ProcessObjectDef(info);
    if (!c_info)
      return NULL;
    info->children.Append(c_info);
  }

  return info;
}

void wxObjectInputStream::ProcessObjectData(wxObjectStreamInfo *info)
{
  wxNode *node = info->children.First();
  wxObjectStreamInfo *c_info;

  m_current_info = info;

  info->object->LoadObject(*this);
  while (node) {
    c_info = (wxObjectStreamInfo *)node->Data();
    c_info->object->LoadObject(*this);
    node = node->Next();
  }
}

wxObject *wxObjectInputStream::LoadObject()
{
  wxObjectStreamInfo *info;
  wxObject *object;

  info = ProcessObjectDef(NULL);
  if (!info)
    return NULL;
  ProcessObjectData(info);

  object = info->object;

  delete info; // It's magic ! The whole tree is destroyed.

  return object;
}
