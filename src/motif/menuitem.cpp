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
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "menuitem.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/menu.h"
#include "wx/menuitem.h"
#include "wx/utils.h"
#include "wx/frame.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/CascadeBG.h>
#include <Xm/CascadeB.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

// ----------------------------------------------------------------------------
// functions prototypes
// ----------------------------------------------------------------------------

static void wxMenuItemCallback(Widget w, XtPointer clientData, XtPointer ptr);
static void wxMenuItemArmCallback(Widget w, XtPointer clientData, XtPointer ptr);
static void wxMenuItemDisarmCallback(Widget w, XtPointer clientData, XtPointer ptr);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dynamic classes implementation
// ----------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

// ctor & dtor
// -----------

wxMenuItem::wxMenuItem(wxMenu *pParentMenu, int id,
                       const wxString& strName, const wxString& strHelp,
                       bool bCheckable,
                       wxMenu *pSubMenu)
{
    wxASSERT_MSG( pParentMenu != NULL, wxT("menuitem should have a menu") );

    // common init
    m_parentMenu  = pParentMenu;
    m_subMenu     = pSubMenu;
    m_id          = id;
    m_isEnabled   = TRUE;
    m_isChecked   = FALSE;
    m_help        = strHelp;
    m_isCheckable = bCheckable;
    m_text        = strName;

    // Motif-specific
    m_menuBar      = NULL;
    m_buttonWidget = (WXWidget) NULL;
    m_topMenu      = NULL;
}

wxMenuItem::~wxMenuItem()
{
}

// misc
// ----

// delete the sub menu
void wxMenuItem::DeleteSubMenu()
{
    wxASSERT( m_subMenu != NULL );

    delete m_subMenu;
    m_subMenu = NULL;
}

// change item state
// -----------------

void wxMenuItem::Enable(bool bDoEnable)
{
    if ( m_isEnabled != bDoEnable )
    {
        if ( !IsSubMenu() )
        {
            // normal menu item
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

        wxMenuItemBase::Enable(bDoEnable);
    }
}

void wxMenuItem::Check(bool bDoCheck)
{
    wxCHECK_RET( IsCheckable(), "only checkable items may be checked" );

    if ( m_isChecked != bDoCheck )
    {
        if ( m_buttonWidget )
        {
            wxASSERT_MSG( XtIsSubclass((Widget)m_buttonWidget,
                                       xmToggleButtonGadgetClass),
                          wxT("checkable menu item must be a toggle button") );

            XtVaSetValues((Widget)m_buttonWidget,
                          XmNset, (Boolean)bDoCheck,
                          NULL);
        }

        wxMenuItemBase::Check(bDoCheck);
    }
}

/* static */
wxString wxMenuItemBase::GetLabelFromText(const wxString& text)
{
    return wxStripMenuCodes(text);
}

// ----------------------------------------------------------------------------
// wxMenuItemBase
// ----------------------------------------------------------------------------

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int id,
                                const wxString& name,
                                const wxString& help,
                                bool isCheckable,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, isCheckable, subMenu);
}

// ----------------------------------------------------------------------------
// Motif-specific
// ----------------------------------------------------------------------------

void wxMenuItem::CreateItem (WXWidget menu, wxMenuBar * menuBar, wxMenu * topMenu)
{
    m_menuBar = menuBar;
    m_topMenu = topMenu;

    if (GetId() == -2)
    {
        // Id=-2 identifies a Title item.
        m_buttonWidget = (WXWidget) XtVaCreateManagedWidget
            (wxStripMenuCodes(m_text),
            xmLabelGadgetClass, (Widget) menu, NULL);
    }
    else if ((!m_text.IsNull() && m_text != "") && (!m_subMenu))
    {
        wxString strName = wxStripMenuCodes(m_text);
        if (IsCheckable())
        {
            m_buttonWidget = (WXWidget) XtVaCreateManagedWidget (strName,
                xmToggleButtonGadgetClass, (Widget) menu,
                NULL);
            XtVaSetValues ((Widget) m_buttonWidget, XmNset, (Boolean) IsChecked(), NULL);
        }
        else
            m_buttonWidget = (WXWidget) XtVaCreateManagedWidget (strName,
            xmPushButtonGadgetClass, (Widget) menu,
            NULL);
        char mnem = wxFindMnemonic (m_text);
        if (mnem != 0)
            XtVaSetValues ((Widget) m_buttonWidget, XmNmnemonic, mnem, NULL);

        //// TODO: proper accelerator treatment. What does wxFindAccelerator
        //// look for?
        strName = m_text;
        char *accel = wxFindAccelerator (strName);
        if (accel)
            XtVaSetValues ((Widget) m_buttonWidget, XmNaccelerator, accel, NULL);

        // TODO: What does this do?
        XmString accel_str = wxFindAcceleratorText (strName);
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
    else if (m_subMenu)
    {
        m_buttonWidget = m_subMenu->CreateMenu (menuBar, menu, topMenu, m_text, TRUE);
        m_subMenu->SetButtonWidget(m_buttonWidget);
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
        ;      // Nothing

    }
    else if ((!m_text.IsNull() && (m_text != "")) && !m_subMenu)
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
        ;      // Nothing

    }
    else if (GetSubMenu())
    {
        if (m_buttonWidget)
        {
            XtRemoveCallback ((Widget) m_buttonWidget, XmNcascadingCallback,
                wxMenuItemArmCallback, (XtPointer) this);
        }
        m_subMenu->DestroyMenu(full);
        if (full)
            m_buttonWidget = NULL;
    }

    if (m_buttonWidget && full)
    {
        XtDestroyWidget ((Widget) m_buttonWidget);
        m_buttonWidget = (WXWidget) 0;
    }
}

void wxMenuItem::SetText(const wxString& label)
{
    char mnem = wxFindMnemonic (label);
    wxString label2 = wxStripMenuCodes(label);

    m_text = label;

    if (m_buttonWidget)
    {
        wxXmString label_str(label2);
        XtVaSetValues ((Widget) m_buttonWidget,
            XmNlabelString, label_str(),
            NULL);
        if (mnem != 0)
            XtVaSetValues ((Widget) m_buttonWidget, XmNmnemonic, mnem, NULL);
        char *accel = wxFindAccelerator (label2);
        if (accel)
            XtVaSetValues ((Widget) m_buttonWidget, XmNaccelerator, accel, NULL);

        XmString accel_str = wxFindAcceleratorText (label2);
        if (accel_str)
        {
            XtVaSetValues ((Widget) m_buttonWidget, XmNacceleratorText, accel_str, NULL);
            XmStringFree (accel_str);
        }
    }
}

// ----------------------------------------------------------------------------
// Motif callbacks
// ----------------------------------------------------------------------------

void wxMenuItemCallback (Widget WXUNUSED(w), XtPointer clientData,
                         XtPointer WXUNUSED(ptr))
{
    wxMenuItem *item = (wxMenuItem *) clientData;
    if (item)
    {
        if (item->IsCheckable())
        {
            Boolean isChecked = FALSE;
            XtVaGetValues ((Widget) item->GetButtonWidget(), XmNset, & isChecked, NULL);

            // only set the flag, don't actually check anything
            item->wxMenuItemBase::Check(isChecked);
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

void wxMenuItemArmCallback (Widget WXUNUSED(w), XtPointer clientData,
                       XtPointer WXUNUSED(ptr))
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
wxMenuItemDisarmCallback (Widget WXUNUSED(w), XtPointer clientData,
                          XtPointer WXUNUSED(ptr))
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

