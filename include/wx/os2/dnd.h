///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     declaration of the wxDropTarget class
// Author:      David Webster
// Modified by:
// Created:     10/21/99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 David Webster
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////


#ifndef __OS2DNDH__
#define __OS2DNDH__

#ifdef __GNUG__
#pragma interface
#endif

#if !wxUSE_DRAG_AND_DROP
    #error  "You should #define wxUSE_DRAG_AND_DROP to 1 to compile this file!"
#endif  //WX_DRAG_DROP

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDropSource: public wxDropSourceBase
{
public:
    /* constructor. set data later with SetData() */
    wxDropSource(wxWindow* pWin);

    /* constructor for setting one data object */
    wxDropSource( wxDataObject& rData,
                  wxWindow*     pWin
                );
    virtual ~wxDropSource();

    /* start drag action */
    virtual wxDragResult DoDragDrop(bool bAllowMove = FALSE);

protected:
    void Init(void);
    bool                            m_bLazyDrag;

    DRAGIMAGE*                      m_pDragImage;
    DRAGINFO*                       m_pDragInfo;
    DRAGTRANSFER*                   m_pDragTransfer;
};

//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDropTarget: public wxDropTargetBase
{
public:
    wxDropTarget(wxDataObject *dataObject = (wxDataObject*)NULL);
    virtual ~wxDropTarget();

    void Register(WXHWND hwnd);
    void Revoke(WXHWND hwnd);

    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDrop(wxCoord x, wxCoord y);
    virtual bool OnData(wxCoord x, wxCoord y);
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult vResult);
    virtual bool GetData();

  // implementation
protected:
    virtual bool IsAcceptable(DRAGINFO* pInfo);

    DRAGINFO*                       m_pDragInfo;
    DRAGTRANSFER*                   m_pDragTransfer;
};

// ----------------------------------------------------------------------------
// A simple wxDropTarget derived class for text data: you only need to
// override OnDropText() to get something working
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTextDropTarget : public wxDropTarget
{
public:
    wxTextDropTarget();
    virtual ~wxTextDropTarget();

    virtual bool OnDropText( wxCoord         x
                            ,wxCoord         y
                            ,const wxString& rText
                           ) = 0;

    virtual bool OnData( wxCoord x
                        ,wxCoord y
                       );
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult vResult);
};

// ----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFileDropTarget : public wxDropTarget
{
public:
    wxFileDropTarget();
    virtual ~wxFileDropTarget();

    // parameters are the number of files and the array of file names
    virtual bool OnDropFiles( wxCoord              x
                             ,wxCoord              y
                             ,const wxArrayString& rFilenames
                            ) = 0;

    virtual bool OnData( wxCoord x
                        ,wxCoord y
                       );
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult vResult);
};

#endif //__OS2DNDH__

