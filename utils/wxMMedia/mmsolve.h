// /////////////////////////////////////////////////////////////////////////////
// Name:       mmsolve.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
#ifndef __MMD_solve_H__
#define __MMD_solve_H__
#ifdef __GNUG__
#pragma interface
#endif

#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#else
#include "wx/wx.h"
#endif
#include "mmfile.h"

/** @name Solver classes */
//@{

typedef wxMMediaFile *(*wxMediaFileCreator)();

/** wxMediaFileSolve is a class to do name resolution on multimedia files
  * @memo Multimedia file solver
  * @author Guilhem Lavaux
  */
class WXDLLEXPORT wxMediaFileSolve : public wxObject {
protected:
  typedef struct wxMFileList {
    wxMediaFileCreator creator;
    wxString mime_type, name, ext;
    wxMFileList *next;
  } wxMFileList;

  static wxMFileList *m_first;
  static wxUint8 m_devnum;

  friend class wxMMDfileRegister;
public:
  wxMediaFileSolve() : wxObject() {}
  ~wxMediaFileSolve() {}

  /** It resolves using the extension of the specified filename
    * @memo
    * @return the multimedia codec
    * @param filename
    */
  static wxMMediaFile *ByExtension(const wxString& filename);

  /** It resolves using the real name of a codec
    * @memo
    * @return the multimedia codec
    * @param devname
    */
  static wxMMediaFile *ByName(const wxString& devname);

  /** It resolves using a mime type
    * @memo
    * @return the multimedia codec
    * @param mimetype
    */
  static wxMMediaFile *ByType(const wxString& mimetype);

  /** It lists all codecs currently registered in "names". "names" is allocated
    * by it and devices returns the number of codecs the list contains
    * @memo
    * @return nothing
    * @param names an array
    * @param devices
    */
  static void ListMDevice(wxString*& names, wxUint8& devices);
};

///
class wxMMDfileRegister {
public:
  ///
  wxMMDfileRegister(wxMediaFileCreator cbk, char *mtype, char *ext, char *name);
};

#define MMD_REGISTER_FILE(mtype, name, class, ext) \
static wxMMediaFile *wxMediaFileConstructor_##class() { return new class(); } \
wxMMDfileRegister mmd_##class(wxMediaFileConstructor_##class, mtype, ext, name);

//@}

#endif
