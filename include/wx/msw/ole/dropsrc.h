///////////////////////////////////////////////////////////////////////////////
// Name:        ole/dropsrc.h
// Purpose:     declaration of the wxDropSource class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.03.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_OLEDROPSRC_H
#define   _WX_OLEDROPSRC_H

#ifdef __GNUG__
    #pragma interface
#endif

#if !wxUSE_DRAG_AND_DROP
  #error  "You should #define wxUSE_DRAG_AND_DROP to 1 to compile this file!"
#endif  //WX_DRAG_DROP

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class wxIDropSource;
class WXDLLEXPORT wxDataObject;
class WXDLLEXPORT wxWindow;

// ----------------------------------------------------------------------------
// wxDropSource is used to start the drag-&-drop operation on associated
// wxDataObject object. It's responsible for giving UI feedback while dragging.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDropSource
{
public:
    // ctors: if you use default ctor you must call SetData() later!
    //
    // NB: the "wxWindow *win" parameter is unused and is here only for wxGTK
    //     compatibility, as well as both icon parameters
    wxDropSource(wxWindow *win = NULL,
            const wxIcon &go = wxNullIcon,
            const wxIcon &stop = wxNullIcon );
    wxDropSource(wxDataObject& data,
            wxWindow *win = NULL,
            const wxIcon &go = wxNullIcon,
            const wxIcon &stop = wxNullIcon );

    virtual ~wxDropSource();

    // do it (call this in response to a mouse button press, for example)
    // params: if bAllowMove is false, data can be only copied
    virtual wxDragResult DoDragDrop(bool bAllowMove = FALSE);

    // overridable: you may give some custom UI feedback during d&d operation
    // in this function (it's called on each mouse move, so it shouldn't be
    // too slow). Just return false if you want default feedback.
    virtual bool GiveFeedback(wxDragResult effect, bool bScrolling);

protected:
    void Init();

private:
    wxIDropSource *m_pIDropSource;  // the pointer to COM interface
};

#endif  //_WX_OLEDROPSRC_H
