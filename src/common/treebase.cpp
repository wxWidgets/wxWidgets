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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
  #pragma implementation "treebase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TREECTRL

#include "wx/treebase.h"
#include "wx/settings.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/arrimpl.cpp"
#include "wx/dcclient.h"


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
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP)

// ----------------------------------------------------------------------------
// Tree event
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent, wxNotifyEvent)


wxTreeEvent::wxTreeEvent(wxEventType commandType, int id)
           : wxNotifyEvent(commandType, id)
{
    m_itemOld = 0l;
    m_editCancelled = false;
}

wxTreeEvent::wxTreeEvent(const wxTreeEvent & event)
           : wxNotifyEvent(event)
{
    m_evtKey = event.m_evtKey;
    m_item = event.m_item;
    m_itemOld = event.m_itemOld;
    m_pointDrag = event.m_pointDrag;
    m_label = event.m_label;
    m_editCancelled = event.m_editCancelled;
}

#endif // wxUSE_TREECTRL

