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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/setup.h"
#endif

#if wxUSE_SERIAL

#include "wx/object.h"
#include "wx/objstrm.h"
#include "wx/datstrm.h"

#define WXOBJ_BEGIN "OBEGIN"
#define WXOBJ_BEG_LEN 6

#define TAG_EMPTY_OBJECT "NULL"
#define TAG_DUPLICATE_OBJECT "DUPLIC"

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

  if (info.duplicate) {
    data_s.WriteString(TAG_DUPLICATE_OBJECT);
    data_s.WriteString(GetObjectName(info.object));
    printf("info.object (dup %s)\n", info.object->GetClassInfo()->GetClassName());
    return;
  }

  if (info.object) {
    data_s.WriteString(info.object->GetClassInfo()->GetClassName());
    printf("info.object (%s)\n", info.object->GetClassInfo()->GetClassName());
  } else {
    data_s.WriteString(TAG_EMPTY_OBJECT);
    printf("info.object (NULL)\n");
    return;
  }

  data_s.WriteString(GetObjectName(info.object));

  // I assume an object will not have millions of children
  // Hmmm ... it could have (for example wxGrid)
  data_s.Write32(info.children.Number());
}

void wxObjectOutputStream::AddChild(wxObject *obj)
{
  wxObjectStreamInfo *info;

  if (!FirstStage())
    return;

  info = new wxObjectStreamInfo;

  if (m_saved_objs.Member(obj) != NULL) {
    info->duplicate = TRUE;
  } else {
    info->duplicate = FALSE;
    m_saved_objs.Append(obj);
  }
  if (!obj)
    info->duplicate = FALSE;

  info->n_children = 0;
  info->object = obj;
  info->parent = m_current_info; // Not useful here.
  m_current_info->n_children++;
  m_current_info->children.Append(info);
}

void wxObjectOutputStream::ProcessObjectDef(wxObjectStreamInfo *info)
{
  wxNode *node;

  m_current_info = info;
  // First stage: get children of obj
  if (info->object && !info->duplicate)
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

  if (info->object && !info->duplicate)
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
  info.duplicate = FALSE;
  ProcessObjectDef(&info);

  m_stage = 1;
  ProcessObjectData(&info);

  info.children.Clear();
  m_saved_objs.Clear();

  m_saving = FALSE;

  return TRUE;
}

// ----------------------------------------------------------------------------
// wxObjectInputStream
// ----------------------------------------------------------------------------

wxObjectInputStream::wxObjectInputStream(wxInputStream& s)
  : wxFilterInputStream(s)
{
  m_secondcall = FALSE;
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
  return (wxObject *) NULL;
}

wxObject *wxObjectInputStream::GetParent() const
{
  if (!m_current_info->parent)
    return (wxObject *) NULL;

  return m_current_info->parent->object;
}

wxObject *wxObjectInputStream::GetChild()
{
  wxObject *obj = GetChild(0);

  m_current_info->children_removed++;

  return obj;
}

wxObject *wxObjectInputStream::GetChild(int no) const
{
  wxNode *node;
  wxObjectStreamInfo *info;

  if (m_current_info->children_removed >= m_current_info->n_children)
    return (wxObject *) NULL;

  node = m_current_info->children.Nth(m_current_info->children_removed+no);

  if (!node)
    return (wxObject *) NULL;

  info = (wxObjectStreamInfo *)node->Data();

  return info->object;
}

void wxObjectInputStream::RemoveChildren(int nb)
{
  m_current_info->children_removed += nb;
}

bool wxObjectInputStream::ReadObjectDef(wxObjectStreamInfo *info)
{
  wxDataInputStream data_s(*this);
  char sig[WXOBJ_BEG_LEN+1];
  wxString class_name;

  Read(sig, WXOBJ_BEG_LEN);
  sig[WXOBJ_BEG_LEN] = 0;
  if (wxString(sig) != WXOBJ_BEGIN)
    return FALSE;

  class_name = data_s.ReadString();
  info->children_removed = 0;
  info->n_children = 0;

  if (class_name == TAG_EMPTY_OBJECT)
    info->object = (wxObject *) NULL;
  else if (class_name == TAG_DUPLICATE_OBJECT) {
    info->object_name = data_s.ReadString();
    info->object = SolveName(info->object_name);
  } else {
    info->object_name = data_s.ReadString();
    info->object = wxCreateDynamicObject( WXSTRINGCAST class_name);
    info->n_children = data_s.Read32();
  }
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
    return (wxObjectStreamInfo *) NULL;

  for (c=0;c<info->n_children;c++) {
    c_info = ProcessObjectDef(info);
    if (!c_info)
      return (wxObjectStreamInfo *) NULL;
    info->children.Append(c_info);
  }

  return info;
}

void wxObjectInputStream::ProcessObjectData(wxObjectStreamInfo *info)
{
  wxNode *node = info->children.First();

  m_current_info = info;

  if (info->object)
    info->object->LoadObject(*this);
  while (node) {
    ProcessObjectData((wxObjectStreamInfo *)node->Data());
    node = node->Next();
  }

  m_current_info = info;

  if (info->recall) {
    m_secondcall = TRUE;
    info->object->LoadObject(*this);
    m_secondcall = FALSE;
  }
}

wxObject *wxObjectInputStream::LoadObject()
{
  wxObjectStreamInfo *info;
  wxObject *object;

  info = ProcessObjectDef((wxObjectStreamInfo *) NULL);
  if (!info)
    return (wxObject *) NULL;
  ProcessObjectData(info);

  object = info->object;

  delete info; // It's magic ! The whole tree is destroyed.

  return object;
}

#endif

