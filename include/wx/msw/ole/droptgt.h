///////////////////////////////////////////////////////////////////////////////
// Name:        ole/droptgt.h
// Purpose:     declaration of the wxDropTarget class
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     06.03.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// prolog
// ============================================================================
#ifndef   _WX_OLEDROPTGT_H
#define   _WX_OLEDROPTGT_H

#ifdef __GNUG__
#pragma interface "droptgt.h"
#endif

#if !USE_DRAG_AND_DROP
  #error  "You should #define wxUSE_DRAG_AND_DROP to 1 to compile this file!"
#endif  //WX_DRAG_DROP

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------
class  wxIDropTarget;
struct IDataObject;

// ----------------------------------------------------------------------------
// An instance of the class wxDropTarget may be associated with any wxWindow
// derived object via SetDropTarget() function. If this is done, the virtual
// methods of wxDropTarget are called when something is dropped on the window.
// 
// Note that wxDropTarget is an abstract base class (ABC) and you should derive
// your own class from it implementing pure virtual function in order to use it
// (all of them, including protected ones which are called by the class itself)
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxDropTarget
{
public:
  // ctor & dtor
  wxDropTarget();
  virtual ~wxDropTarget();

  // normally called by wxWindow on window creation/destruction, but might be
  // called `manually' as well. Register() returns true on success.
  bool Register(WXHWND hwnd);
  void Revoke(WXHWND hwnd);

  // do we accept this kind of data?
  virtual bool IsAcceptedData(IDataObject *pIDataSource) const;

  // called when mouse enters/leaves the window: might be used to give
  // some visual feedback to the user
  virtual void OnEnter() { }
  virtual void OnLeave() { }

  // this function is called when data is dropped.
  // (x, y) are the coordinates of the drop
  virtual bool OnDrop(long x, long y, const void *pData) = 0;

protected:
  // Override these to indicate what kind of data you support: the first
  // format to which data can be converted is used. The classes below show
  // how it can be done in the simplest cases.
    // how many different (clipboard) formats do you support?
  virtual size_t GetFormatCount() const = 0;
    // return the n-th supported format
  virtual wxDataFormat GetFormat(size_t n) const = 0;

private:
  wxIDropTarget    *m_pIDropTarget; // the pointer to COM interface
};

// ----------------------------------------------------------------------------
// A simple wxDropTarget derived class for text data: you only need to
// override OnDropText() to get something working
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxTextDropTarget : public wxDropTarget
{
public:
  virtual bool OnDrop(long x, long y, const void *pData);
  virtual bool OnDropText(long x, long y, const char *psz) = 0;

protected:
  virtual size_t GetFormatCount() const;
  virtual wxDataFormat GetFormat(size_t n) const;
};

// ----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxFileDropTarget : public wxDropTarget
{
public:
  virtual bool OnDrop(long x, long y, const void *pData);

  // params: the number of files and the array of file names
  virtual bool OnDropFiles(long x, long y, 
                           size_t nFiles, const char * const aszFiles[]) = 0;

protected:
  virtual size_t GetFormatCount() const;
  virtual wxDataFormat GetFormat(size_t n) const;
};

// ============================================================================
#endif  //_WX_OLEDROPTGT_H
