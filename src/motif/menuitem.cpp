///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

#include "wx/menu.h"
#include "wx/menuitem.h"
#include "wx/utils.h"
#include "wx/frame.h"

#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/CascadeBG.h>
#include <Xm/CascadeB.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>

#include "wx/motif/private.h"

void wxMenuItemCallback (Widget w, XtPointer clientData,
			 XtPointer ptr);
void wxMenuItemArmCallback (Widget w, XtPointer clientData,
			    XtPointer ptr);
void wxMenuItemDisarmCallback (Widget w, XtPointer clientData,
			       XtPointer ptr);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dynamic classes implementation
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)
#endif  //USE_SHARED_LIBRARY

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

// ctor & dtor
// -----------

wxMenuItem::wxMenuItem(wxMenu *pParentMenu, int id,
                       const wxString& strName, const wxString& strHelp,
                       bool bCheckable,
                       wxMenu *pSubMenu) :
                        m_strHelp(strHelp),
                        m_bCheckable(bCheckable),
                        m_strName(strName)
{
  wxASSERT( pParentMenu != NULL );

  m_pParentMenu = pParentMenu;
  m_pSubMenu    = pSubMenu;
  m_idItem      = id;
  m_bEnabled    = TRUE;
  m_bChecked    = FALSE;

  //// Motif-specific
  m_menuBar = NULL;
  m_buttonWidget = (WXWidget) NULL;
  m_topMenu = NULL;
}

wxMenuItem::~wxMenuItem() 
{
}

// misc
// ----

// delete the sub menu
void wxMenuItem::DeleteSubMenu()
{
  wxASSERT( m_pSubMenu != NULL );

  delete m_pSubMenu;
  m_pSubMenu = NULL;
}

// change item state
// -----------------

void wxMenuItem::Enable(bool bDoEnable)
{
  if ( m_bEnabled != bDoEnable )
  {
    if ( m_pSubMenu == NULL )
    {     // normal menu item
        if (m_buttonWidget)
            XtSetSensitive( (Widget) m_buttonWidget, (Boolean) bDoEnable);
    }
    else                            // submenu
    {
        // Maybe we should apply this to all items in the submenu?
        // Or perhaps it works anyway.
        if (m_buttonWidget)
            XtSetSensitive( (Widget) m_buttonWidget, (Boolean) bDoEnable);
    }

    m_bEnabled = bDoEnable;
  }
}

void wxMenuItem::Check(bool bDoCheck)
{
  wxCHECK_RET( IsCheckable(), "only checkable items may be checked" );

  if ( m_bChecked != bDoCheck )
  {
    if (m_buttonWidget && XtIsSubclass ((Widget) m_buttonWidget, xmToggleButtonGadgetClass))
    {
      XtVaSetValues ( (Widget) m_buttonWidget, XmNset, (Boolean) bDoCheck, NULL);
    }
    m_bChecked = bDoCheck;
  }
}

//// Motif-specific

void wxMenuItem::CreateItem (WXWidget menu, wxMenuBar * menuBar, wxMenu * topMenu)
{
  m_menuBar = menuBar;
  m_topMenu = topMenu;

  if (GetId() == -2)
    {
      // Id=-2 identifies a Title item.
      wxStripMenuCodes ((char*) (const char*) m_strName, wxBuffer);
      m_buttonWidget = (WXWidget) XtVaCreateManagedWidget (wxBuffer,
					    xmLabelGadgetClass, (Widget) menu, NULL);
    }
  else if ((!m_strName.IsNull() && m_strName != "") && (!m_pSubMenu))
    {
      wxStripMenuCodes ((char*) (const char*) m_strName, wxBuffer);
      if (IsCheckable())
	{
	  m_buttonWidget = (WXWidget) XtVaCreateManagedWidget (wxBuffer,
					    xmToggleButtonGadgetClass, (Widget) menu,
						  NULL);
	  XtVaSetValues ((Widget) m_buttonWidget, XmNset, (Boolean) IsChecked(), NULL);
	}
      else
	m_buttonWidget = (WXWidget) XtVaCreateManagedWidget (wxBuffer,
					      xmPushButtonGadgetClass, (Widget) menu,
						NULL);
      char mnem = wxFindMnemonic (m_strName);
      if (mnem != 0)
	XtVaSetValues ((Widget) m_buttonWidget, XmNmnemonic, mnem, NULL);

      //// TODO: proper accelerator treatment. What does wxFindAccelerator
      //// look for?
      strcpy(wxBuffer, (char*) (const char*) m_strName);
      char *accel = wxFindAccelerator (wxBuffer);
      if (accel)
	XtVaSetValues ((Widget) m_buttonWidget, XmNaccelerator, accel, NULL);

      // TODO: What does this do?
      strcpy(wxBuffer, (char*) (const char*) m_strName);
      XmString accel_str = wxFindAcceleratorText (wxBuffer);
      if (accel_str)
	{
	  XtVaSetValues ((Widget) m_buttonWidget, XmNacceleratorText, accel_str, NULL);
	  XmStringFree (accel_str);
	}

      if (IsCheckable())
	XtAddCallback ((Widget) m_buttonWidget,
		       XmNvalueChangedCallback,
		       (XtCallbackProc) wxMenuItemCallback,
		       (XtPointer) this);
      else
	XtAddCallback ((Widget) m_buttonWidget,
		       XmNactivateCallback,
		       (XtCallbackProc) wxMenuItemCallback,
		       (XtPointer) this);
      XtAddCallback ((Widget) m_buttonWidget,
		     XmNarmCallback,
		     (XtCallbackProc) wxMenuItemArmCallback,
		     (XtPointer) this);
      XtAddCallback ((Widget) m_buttonWidget,
		     XmNdisarmCallback,
		     (XtCallbackProc) wxMenuItemDisarmCallback,
		     (XtPointer) this);
    }
  else if (GetId() == -1)
    {
      m_buttonWidget = (WXWidget) XtVaCreateManagedWidget ("separator",
					xmSeparatorGadgetClass, (Widget) menu, NULL);
    }
  else if (m_pSubMenu)
    {
      m_buttonWidget = m_pSubMenu->CreateMenu (menuBar, menu, topMenu, m_strName, TRUE);
      m_pSubMenu->SetButtonWidget(m_buttonWidget);
      XtAddCallback ((Widget) m_buttonWidget,
		     XmNcascadingCallback,
		     (XtCallbackProc) wxMenuItemArmCallback,
		     (XtPointer) this);
    }
  if (m_buttonWidget)
    XtSetSensitive ((Widget) m_buttonWidget, (Boolean) IsEnabled());
}

void wxMenuItem::DestroyItem(bool full)
{
  if (GetId() == -2)
    {
      ;			// Nothing
      
    }
  else if ((!m_strName.IsNull() && (m_strName != "")) && !m_pSubMenu)
    {
      if (m_buttonWidget)
	{
	  if (IsCheckable())
	    XtRemoveCallback ((Widget) m_buttonWidget, XmNvalueChangedCallback,
			      wxMenuItemCallback, (XtPointer) this);
	  else
	    XtRemoveCallback ((Widget) m_buttonWidget, XmNactivateCallback,
			      wxMenuItemCallback, (XtPointer) this);
	  XtRemoveCallback ((Widget) m_buttonWidget, XmNarmCallback,
			    wxMenuItemArmCallback, (XtPointer) this);
	  XtRemoveCallback ((Widget) m_buttonWidget, XmNdisarmCallback,
			    wxMenuItemDisarmCallback, (XtPointer) this);
	}
    }
  else if (GetId() == -1)
    {
      ;			// Nothing
      
    }
  else if (GetSubMenu())
    {
      if (m_buttonWidget)
	{
	  XtRemoveCallback ((Widget) m_buttonWidget, XmNcascadingCallback,
			    wxMenuItemArmCallback, (XtPointer) this);
	}
      m_pSubMenu->DestroyMenu(full);
      if (full)
        m_buttonWidget = NULL;
    }

  if (m_buttonWidget && full)
    {
      XtDestroyWidget ((Widget) m_buttonWidget);
      m_buttonWidget = (WXWidget) 0;
    }
}

void wxMenuItem::SetLabel(const wxString& label)
{
  char mnem = wxFindMnemonic (label);
  wxStripMenuCodes ((char*) (const char*) label, wxBuffer);

  m_strName = label;

  if (m_buttonWidget)
    {
      XmString label_str = XmStringCreateSimple (wxBuffer);
      XtVaSetValues ((Widget) m_buttonWidget,
                     XmNlabelString, label_str,
                     NULL);
      XmStringFree (label_str);
      if (mnem != 0)
        XtVaSetValues ((Widget) m_buttonWidget, XmNmnemonic, mnem, NULL);
      strcpy(wxBuffer, (char*) (const char*) label);
      char *accel = wxFindAccelerator (wxBuffer);
      if (accel)
        XtVaSetValues ((Widget) m_buttonWidget, XmNaccelerator, accel, NULL);

      strcpy(wxBuffer, (char*) (const char*) label);
      XmString accel_str = wxFindAcceleratorText (wxBuffer);
      if (accel_str)
        {
          XtVaSetValues ((Widget) m_buttonWidget, XmNacceleratorText, accel_str, NULL);
          XmStringFree (accel_str);
        }
    }
}

void wxMenuItemCallback (Widget w, XtPointer clientData,
		    XtPointer ptr)
{
  wxMenuItem *item = (wxMenuItem *) clientData;
  if (item)
    {
      if (item->IsCheckable())
	{
	  Boolean isChecked = FALSE;
	  XtVaGetValues ((Widget) item->GetButtonWidget(), XmNset, & isChecked, NULL);
	  item->SetChecked(isChecked);
	}
      if (item->GetMenuBar() && item->GetMenuBar()->GetMenuBarFrame())
	{
          wxCommandEvent commandEvent(wxEVT_COMMAND_MENU_SELECTED, item->GetId());
          commandEvent.SetEventObject(item->GetMenuBar()->GetMenuBarFrame());
          commandEvent.SetInt( item->GetId() );

	  item->GetMenuBar()->GetMenuBarFrame()->GetEventHandler()->ProcessEvent(commandEvent);
	}
      else if (item->GetTopMenu())
	{
	  wxCommandEvent event (wxEVT_COMMAND_MENU_SELECTED, item->GetId());
	  event.SetEventObject(item->GetTopMenu());
          event.SetInt( item->GetId() );

	  item->GetTopMenu()->ProcessCommand (event);
	}
    }
}

void 
wxMenuItemArmCallback (Widget w, XtPointer clientData,
		       XtPointer ptr)
{
  wxMenuItem *item = (wxMenuItem *) clientData;
  if (item)
    {
      if (item->GetMenuBar() && item->GetMenuBar()->GetMenuBarFrame())
	{
          wxMenuEvent menuEvent(wxEVT_MENU_HIGHLIGHT, item->GetId());
          menuEvent.SetEventObject(item->GetMenuBar()->GetMenuBarFrame());

          item->GetMenuBar()->GetMenuBarFrame()->GetEventHandler()->ProcessEvent(menuEvent);
	}
    }
}

void 
wxMenuItemDisarmCallback (Widget w, XtPointer clientData,
			  XtPointer ptr)
{
  wxMenuItem *item = (wxMenuItem *) clientData;
  if (item)
    {
      if (item->GetMenuBar() && item->GetMenuBar()->GetMenuBarFrame())
	{
          // TODO: not sure this is correct, since -1 means something
          // special to event system
          wxMenuEvent menuEvent(wxEVT_MENU_HIGHLIGHT, -1);
          menuEvent.SetEventObject(item->GetMenuBar()->GetMenuBarFrame());

          item->GetMenuBar()->GetMenuBarFrame()->GetEventHandler()->ProcessEvent(menuEvent);
	}
    }
}

