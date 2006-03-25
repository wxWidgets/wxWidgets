/////////////////////////////////////////////////////////////////////////////
// Name:        objstrm.h
// Purpose:     wxObjectStream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     16/07/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WXOBJSTRM_H__
#define _WX_WXOBJSTRM_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_STREAMS && wxUSE_SERIAL

#include "wx/object.h"
#include "wx/string.h"
#include "wx/stream.h"

class wxObjectStreamInfo : public wxObject {
 public:
  wxString object_name;
  int n_children, children_removed;
  wxList children;
  wxObjectStreamInfo *parent;
  wxObject *object;
  bool duplicate, recall;
};

class wxObjectOutputStream : public wxFilterOutputStream {
 public:
  wxObjectOutputStream(wxOutputStream& s);

  void AddChild(wxObject *obj);
  bool SaveObject(wxObject& obj);

  bool FirstStage() const { return m_stage == 0; }

  wxString GetObjectName(wxObject *obj);

 protected:
  void WriteObjectDef(wxObjectStreamInfo& info);
  void ProcessObjectDef(wxObjectStreamInfo *info);
  void ProcessObjectData(wxObjectStreamInfo *info);

 protected:
  int m_stage;
  bool m_saving;
  wxObjectStreamInfo *m_current_info;
  wxList m_saved_objs;
};

class wxObjectInputStream : public wxFilterInputStream {
 public:
  wxObjectInputStream(wxInputStream& s);

  bool SecondCall() const { return m_secondcall; }
  void Recall(bool on = TRUE) { m_current_info->recall = on; }

  wxObject *GetChild(int no) const;
  wxObject *GetChild();
  int NumberOfChildren() const { return m_current_info->n_children; }
  void RemoveChildren(int nb);
  wxObject *GetParent() const;
  wxObject *LoadObject();

  wxObject *SolveName(const wxString& objName) const;

 protected:
  bool ReadObjectDef(wxObjectStreamInfo *info);
  wxObjectStreamInfo *ProcessObjectDef(wxObjectStreamInfo *info);
  void ProcessObjectData(wxObjectStreamInfo *info);

 protected:
  bool m_secondcall;
  wxObjectStreamInfo *m_current_info;
  wxList m_solver;
};

#endif
  // wxUSE_STREAMS && wxUSE_SERIAL

#endif
// _WX_WXOBJSTRM_H__
