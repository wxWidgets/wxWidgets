///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     wxDropTarget, wxDropSource, wxDataObject implementation
// Author:      David Webster
// Modified by:
// Created:     10/21/99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dnd.h"
#endif

#define INCL_PM
#include <os2.h>
#include "wx/window.h"
#include "wx/app.h"
#include "wx/gdicmn.h"
#include "wx/dnd.h"

// ----------------------------------------------------------------------------
// global
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxDropTarget
// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget(
  wxDataObject*                     pDataObject
)
{
    // TODO:
};

wxDropTarget::~wxDropTarget()
{
};

void wxDropTarget::Register(
  WXHWND                            hwnd
)
{
    //TODO:
};

void wxDropTarget::Revoke(
  WXHWND                            hwnd
)
{
    //TODO:
};

wxDragResult wxDropTarget::OnDragOver(
  wxCoord                           x
, wxCoord                           y
, wxDragResult                      vDef
)
{
    //TODO:
    return vDef;
};

bool wxDropTarget::OnDrop(
  wxCoord                           x
, wxCoord                           y
)
{
    //TODO:
    return FALSE;
};

wxDragResult wxDropTarget::OnData(
  wxCoord                           x
, wxCoord                           y
, wxDragResult                      vResult
)
{
    //TODO:
    return (wxDragResult)0;
};

bool wxDropTarget::GetData()
{
    //TODO:
    return FALSE;
};

bool wxDropTarget::IsAcceptable(
  DRAGINFO*                         pInfo
)
{
    //TODO:
    return FALSE;
};

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

wxDropSource::wxDropSource(
  wxWindow*                         pWin
)
{
    // TODO
};

wxDropSource::wxDropSource(
  wxDataObject&                     rData
, wxWindow*                         pWin
)
{
    // TODO
};

wxDropSource::~wxDropSource()
{
    // TODO
};

wxDragResult wxDropSource::DoDragDrop(
  bool                              WXUNUSED(bAllowMove)
)
{
    // TODO
    return wxDragError;
};

void wxDropSource::Init()
{
    // TODO
};

