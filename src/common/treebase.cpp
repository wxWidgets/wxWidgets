/////////////////////////////////////////////////////////////////////////////
// Name:        treebase.cpp
// Purpose:     Base wxTreeCtrl classes
// Author:      Julian Smart
// Created:     01/02/97
// Modified:
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart et al
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// =============================================================================
// declarations
// =============================================================================

// -----------------------------------------------------------------------------
// headers
// -----------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "treebase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/treebase.h"
#include "wx/settings.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/arrimpl.cpp"
#include "wx/dcclient.h"
#include "wx/msgdlg.h"


// ----------------------------------------------------------------------------
// events
// ----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_BEGIN_DRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_BEGIN_RDRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_END_LABEL_EDIT)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_DELETE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_GET_INFO)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_SET_INFO)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_ITEM_EXPANDED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_ITEM_EXPANDING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_ITEM_COLLAPSED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_ITEM_COLLAPSING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_SEL_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_SEL_CHANGING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_KEY_DOWN)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_ITEM_ACTIVATED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_END_DRAG)

// ----------------------------------------------------------------------------
// Tree event
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent, wxNotifyEvent)

wxTreeEvent::wxTreeEvent(wxEventType commandType, int id)
           : wxNotifyEvent(commandType, id)
{
    m_code = 0;
    m_itemOld = 0l;
}

