////////////////////////////////////////////////////////////////////////////////
// Name:       mmsolve.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation
#endif
#include "mmsolve.h"

wxMMediaFile *wxMediaFileSolve::ByExtension(const wxString& filename)
{
  wxMFileList *list = m_first;
  wxString tmp, f_ext;
  int pos = filename.Find('.', TRUE)+1;

  tmp = filename;
  f_ext = tmp(pos, filename.Length()-pos);

  printf("f_ext = %s\n", f_ext.GetData());
  while (list) {
    printf("list->ext = %s\n", list->ext.GetData());
    if (list->ext.CompareTo(f_ext) == 0) {
      wxMMediaFile *mmf = list->creator();
      return mmf;
    }
    list = list->next;
  }
  return NULL;
}

wxMMediaFile *wxMediaFileSolve::ByName(const wxString& name)
{
  wxMFileList *list = m_first;

  while (list) {
    if (list->name == name)
      return (wxMMediaFile *)(list->creator());
    list = list->next;
  }
  return NULL;
}

bool wxMatchMimeType(const wxString& mime_obj, const wxString& type)
{
#ifdef USE_GNU_WXSTRING
  wxString mime2_obj = mime_obj;
  wxString type2 = type;
#define mime_obj mime2_obj
#define type type2
#endif

  if (mime_obj.Find('*') != -1) {
    wxString part_str1[2], part_str2[2];

    part_str1[0] = mime_obj.Left('/');
    part_str1[1] = mime_obj.After('/');
   
    part_str2[0] = type.Left('/');
    part_str2[1] = type.After('/');

    if (part_str1[0] == "*" && part_str1[1] == "*")
      return TRUE;

    if (part_str1[0] == "*" && part_str1[1] == part_str2[1])
      return TRUE;

    if (part_str1[1] == "*" && part_str1[0] == part_str2[1])
      return TRUE;
      
    return FALSE;
  }
  if (mime_obj == type)
    return TRUE;
  return FALSE;
}

wxMMediaFile *wxMediaFileSolve::ByType(const wxString& type)
{
  wxMFileList *list = m_first;

  while (list) {
    if (wxMatchMimeType(*(list->mime_type), type))
      return (wxMMediaFile *)(list->creator());
    list = list->next;
  }
  return NULL;
}

void wxMediaFileSolve::ListMDevice(wxString*& names, wxUint8& devices)
{
  wxMFileList *list = m_first;
  wxUint8 d = 0;

  if (!m_devnum) {
    names = NULL;
    return;
  }
  devices = m_devnum;
  names = new wxString[devices];

  while (list) {
    names[d++] = list->name;
    list = list->next;
  }
}

wxMMDfileRegister::wxMMDfileRegister(wxMediaFileCreator cbk,
				     char *mtype, char *ext, char *name)
{
  wxMediaFileSolve::wxMFileList *entry = new wxMediaFileSolve::wxMFileList;

  entry->next = wxMediaFileSolve::m_first;
  entry->creator = cbk;
  entry->mime_type = wxString(mtype);
  entry->name = wxString(name);
  entry->ext = wxString(ext);
  wxMediaFileSolve::m_devnum++;
  wxMediaFileSolve::m_first = entry;
}
