///////////////////////////////////////////////////////////////////////////////
// Name:        ole/dropsrc.h
// Purpose:     declaration of the wxDropSource class
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     06.03.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef   _OLEDROPSRC_H
#define   _OLEDROPSRC_H

#ifdef __GNUG__
#pragma interface
#endif

#if !USE_DRAG_AND_DROP
  #error  "You should #define USE_DRAG_AND_DROP to 1 to compile this file!"
#endif  //WX_DRAG_DROP

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------
class wxIDropSource;
class wxDataObject;

// ----------------------------------------------------------------------------
// wxDropSource is used to start the drag-&-drop operation on associated
// wxDataObject object. It's responsible for giving UI feedback while dragging.
// ----------------------------------------------------------------------------
class wxDropSource
{
public:
  enum DragResult
  {
    Error,    // error prevented the d&d operation from completing
    None,     // drag target didn't accept the data
    Copy,     // the data was successfully copied
    Move,     // the data was successfully moved
    Cancel    // the operation was cancelled by user (not an error)
  };

  // ctors: if you use default ctor you must call SetData() later!
  wxDropSource();
  wxDropSource(wxDataObject& data);

  void SetData(wxDataObject& data);

  virtual ~wxDropSource();

  // do it (call this in response to a mouse button press, for example)
  // params: if bAllowMove is false, data can be only copied
  DragResult DoDragDrop(bool bAllowMove = FALSE);

  // overridable: you may give some custom UI feedback during d&d operation
  // in this function (it's called on each mouse move, so it shouldn't be too
  // slow). Just return false if you want default feedback.
  virtual bool GiveFeedback(DragResult effect, bool bScrolling);

protected:
  void Init();

  wxDataObject  *m_pData;         // pointer to associated data object

private:
  wxIDropSource *m_pIDropSource;  // the pointer to COM interface
};

#endif  //_OLEDROPSRC_H