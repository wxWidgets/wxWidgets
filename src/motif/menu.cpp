/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "menu.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/menu.h"
#include "wx/menuitem.h"
#include "wx/log.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/frame.h"
#include "wx/settings.h"

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

// other standard headers
#include <string.h>

IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxEvtHandler)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Menus
// ----------------------------------------------------------------------------

// Construct a menu with optional title (then use append)
void wxMenu::Init()
{
    // Motif-specific members
    m_numColumns = 1;
    m_menuWidget = (WXWidget) NULL;
    m_popupShell = (WXWidget) NULL;
    m_buttonWidget = (WXWidget) NULL;
    m_menuId = 0;
    m_topLevelMenu  = (wxMenu*) NULL;
    m_ownedByMenuBar = FALSE;

    if ( !!m_title )
    {
        Append(wxID_SEPARATOR, m_title) ;
        AppendSeparator() ;
    }

    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_MENU);
    m_foregroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_MENUTEXT);
    m_font = wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT);
}

// The wxWindow destructor will take care of deleting the submenus.
wxMenu::~wxMenu()
{
    if (m_menuWidget)
    {
        if (m_menuParent)
            DestroyMenu(TRUE);
        else
            DestroyMenu(FALSE);
    }

    // Not sure if this is right
    if (m_menuParent && m_menuBar)
    {
        m_menuParent = NULL;
        //      m_menuBar = NULL;
    }
}

void wxMenu::Break()
{
    m_numColumns++;
}

// function appends a new item or submenu to the menu
bool wxMenu::DoAppend(wxMenuItem *pItem)
{
    if (m_menuWidget)
    {
        // this is a dynamic Append
        pItem->CreateItem(m_menuWidget, m_menuBar, m_topLevelMenu);
    }

    if ( pItem->IsSubMenu() )
    {
        pItem->GetSubMenu()->m_topLevelMenu = m_topLevelMenu;
    }

    return wxMenuBase::DoAppend(pItem);
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    item->DestroyItem(TRUE);

    return wxMenuBase::DoRemove(item);
}

bool wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if ( !wxMenuBase::DoInsert(pos, item) )
        return FALSE;

    wxFAIL_MSG(wxT("not implemented"));

    return FALSE;
}

void wxMenu::SetTitle(const wxString& label)
{
    m_title = label;

    wxMenuItemList::Node *node = GetMenuItems().GetFirst();
    if ( !node )
        return;

    wxMenuItem *item = node->GetData ();
    Widget widget = (Widget) item->GetButtonWidget();
    if ( !widget )
        return;

    wxXmString title_str(label);
    XtVaSetValues(widget,
                  XmNlabelString, title_str(),
                  NULL);
}

bool wxMenu::ProcessCommand(wxCommandEvent & event)
{
    bool processed = FALSE;

#if wxUSE_MENU_CALLBACK
    // Try a callback
    if (m_callback)
    {
        (void) (*(m_callback)) (*this, event);
        processed = TRUE;
    }
#endif // wxUSE_MENU_CALLBACK

    // Try the menu's event handler
    if ( !processed && GetEventHandler())
    {
        processed = GetEventHandler()->ProcessEvent(event);
    }
    // Try the window the menu was popped up from (and up
    // through the hierarchy)
    if ( !processed && GetInvokingWindow())
        processed = GetInvokingWindow()->ProcessEvent(event);

    return processed;
}

// ----------------------------------------------------------------------------
// Menu Bar
// ----------------------------------------------------------------------------

void wxMenuBar::Init()
{
    m_eventHandler = this;
    m_menuBarFrame = NULL;
    m_mainWidget = (WXWidget) NULL;
    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_MENU);
    m_foregroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_MENUTEXT);
    m_font = wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT);
}

wxMenuBar::wxMenuBar(int n, wxMenu *menus[], const wxString titles[])
{
    Init();

    for ( int i = 0; i < n; i++ )
    {
        m_menus.Append(menus[i]);
        m_titles.Add(titles[i]);
    }
}

wxMenuBar::~wxMenuBar()
{
    // nothing to do: wxMenuBarBase will delete the menus
}

void wxMenuBar::EnableTop(size_t WXUNUSED(pos), bool WXUNUSED(flag))
{
  //    wxFAIL_MSG("TODO");
//  wxLogWarning("wxMenuBar::EnableTop not yet implemented.");
}

void wxMenuBar::SetLabelTop(size_t pos, const wxString& label)
{
    wxMenu *menu = GetMenu(pos);
    if ( !menu )
        return;

    Widget w = (Widget)menu->GetButtonWidget();
    if (w)
    {
        wxXmString label_str(label);

        XtVaSetValues(w,
                      XmNlabelString, label_str(),
                      NULL);
    }
}

wxString wxMenuBar::GetLabelTop(size_t pos) const
{
    wxString str;

    wxMenu *menu = GetMenu(pos);
    if ( menu )
    {
        Widget w = (Widget)menu->GetButtonWidget();
        if (w)
        {
            XmString text;
            XtVaGetValues(w,
                          XmNlabelString, &text,
                          NULL);

            char *s;
            if ( XmStringGetLtoR(text, XmSTRING_DEFAULT_CHARSET, &s) )
            {
                str = s;

                XtFree(s);
            }
        }
    }

    return str;
}

bool wxMenuBar::Append(wxMenu * menu, const wxString& title)
{
    wxCHECK_MSG( menu, FALSE, wxT("invalid menu") );
    wxCHECK_MSG( !menu->GetParent() && !menu->GetButtonWidget(), FALSE,
                 wxT("menu already appended") );

    if ( m_menuBarFrame )
    {
        WXWidget w = menu->CreateMenu(this, GetMainWidget(), menu, title, TRUE);
        wxCHECK_MSG( w, FALSE, wxT("failed to create menu") );
        menu->SetButtonWidget(w);
    }

    menu->SetMenuBar(this);

    m_titles.Add(title);

    return wxMenuBarBase::Append(menu, title);
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return FALSE;

    wxFAIL_MSG(wxT("TODO"));

    return FALSE;
}

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Replace(pos, menu, title) )
        return FALSE;

    wxFAIL_MSG(wxT("TODO"));

    return NULL;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return NULL;

    if ( m_menuBarFrame )
        menu->DestroyMenu(TRUE);

    menu->SetMenuBar(NULL);

    m_titles.Remove(pos);

    return menu;
}

// Find the menu menuString, item itemString, and return the item id.
// Returns -1 if none found.
int wxMenuBar::FindMenuItem (const wxString& menuString, const wxString& itemString) const
{
    char buf1[200];
    char buf2[200];
    wxStripMenuCodes ((char *)(const char *)menuString, buf1);

    size_t menuCount = GetMenuCount();
    for (size_t i = 0; i < menuCount; i++)
    {
        wxStripMenuCodes ((char *)(const char *)m_titles[i], buf2);
        if (strcmp (buf1, buf2) == 0)
            return m_menus[i]->FindItem (itemString);
    }
    return -1;
}

wxMenuItem *wxMenuBar::FindItem(int id, wxMenu ** itemMenu) const
{
    if (itemMenu)
        *itemMenu = NULL;

    wxMenuItem *item = NULL;
    size_t menuCount = GetMenuCount();
    for (size_t i = 0; i < menuCount; i++)
        if ((item = m_menus[i]->FindItem(id, itemMenu)))
            return item;
        return NULL;
}

// Create menubar
bool wxMenuBar::CreateMenuBar(wxFrame* parent)
{
    if (m_mainWidget)
    {
        XtVaSetValues((Widget) parent->GetMainWidget(), XmNmenuBar, (Widget) m_mainWidget, NULL);
        /*
        if (!XtIsManaged((Widget) m_mainWidget))
        XtManageChild((Widget) m_mainWidget);
        */
        XtMapWidget((Widget) m_mainWidget);
        return TRUE;
    }

    Widget menuBarW = XmCreateMenuBar ((Widget) parent->GetMainWidget(), "MenuBar", NULL, 0);
    m_mainWidget = (WXWidget) menuBarW;

    size_t menuCount = GetMenuCount();
    for (size_t i = 0; i < menuCount; i++)
    {
        wxMenu *menu = GetMenu(i);
        wxString title(m_titles[i]);
        menu->SetButtonWidget(menu->CreateMenu (this, menuBarW, menu, title, TRUE));

        if (strcmp (wxStripMenuCodes(title), "Help") == 0)
            XtVaSetValues ((Widget) menuBarW, XmNmenuHelpWidget, (Widget) menu->GetButtonWidget(), NULL);

        // tear off menu support
#if (XmVersion >= 1002)
        if ( menu->IsTearOff() )
        {
            XtVaSetValues(GetWidget(menu),
                          XmNtearOffModel, XmTEAR_OFF_ENABLED,
                          NULL);
            Widget tearOff = XmGetTearOffControl(GetWidget(menu));
            wxDoChangeForegroundColour((Widget) tearOff, m_foregroundColour);
            wxDoChangeBackgroundColour((Widget) tearOff, m_backgroundColour, TRUE);
#endif
        }
    }

    SetBackgroundColour(m_backgroundColour);
    SetForegroundColour(m_foregroundColour);
    SetFont(m_font);

    XtVaSetValues((Widget) parent->GetMainWidget(), XmNmenuBar, (Widget) m_mainWidget, NULL);
    XtRealizeWidget ((Widget) menuBarW);
    XtManageChild ((Widget) menuBarW);
    SetMenuBarFrame(parent);

    return TRUE;
}

// Destroy menubar, but keep data structures intact so we can recreate it.
bool wxMenuBar::DestroyMenuBar()
{
    if (!m_mainWidget)
    {
        SetMenuBarFrame((wxFrame*) NULL);
        return FALSE;
    }

    XtUnmanageChild ((Widget) m_mainWidget);
    XtUnrealizeWidget ((Widget) m_mainWidget);

    size_t menuCount = GetMenuCount();
    for (size_t i = 0; i < menuCount; i++)
    {
        wxMenu *menu = GetMenu(i);
        menu->DestroyMenu(TRUE);

    }
    XtDestroyWidget((Widget) m_mainWidget);
    m_mainWidget = (WXWidget) 0;

    SetMenuBarFrame((wxFrame*) NULL);

    return TRUE;
}

//// Motif-specific
static XtWorkProcId WorkProcMenuId;

/* Since PopupMenu under Motif stills grab right mouse button events
* after it was closed, we need to delete the associated widgets to
* allow next PopUpMenu to appear...
*/

int PostDeletionOfMenu( XtPointer* clientData )
{
    XtRemoveWorkProc(WorkProcMenuId);
    wxMenu *menu = (wxMenu *)clientData;

    if (menu->GetMainWidget())
    {
        wxMenu *menuParent = menu->GetParent();
        if ( menuParent )
        {
            wxMenuItemList::Node *node = menuParent->GetMenuItems().GetFirst();
            while ( node )
            {
                if ( node->GetData()->GetSubMenu() == menu )
                {
                    menuParent->GetMenuItems().DeleteNode(node);

                    break;
                }

                node = node->GetNext();
            }
        }

        menu->DestroyMenu(TRUE);
    }

    // Mark as no longer popped up
    menu->m_menuId = -1;

    return TRUE;
}

void
wxMenuPopdownCallback(Widget WXUNUSED(w), XtPointer clientData,
                      XtPointer WXUNUSED(ptr))
{
    wxMenu *menu = (wxMenu *)clientData;

    // Added by JOREL Jean-Charles <jjorel@silr.ireste.fr>
    /* Since Callbacks of MenuItems are not yet processed, we put a
    * background job which will be done when system will be idle.
    * What awful hack!! :(
    */

    WorkProcMenuId = XtAppAddWorkProc(
        (XtAppContext) wxTheApp->GetAppContext(),
        (XtWorkProc) PostDeletionOfMenu,
        (XtPointer) menu );
    // Apparently not found in Motif headers
    //  XtVaSetValues( w, XmNpopupEnabled, XmPOPUP_DISABLED, NULL );
}

/*
* Create a popup or pulldown menu.
* Submenus of a popup will be pulldown.
*
*/

WXWidget wxMenu::CreateMenu (wxMenuBar * menuBar, WXWidget parent, wxMenu * topMenu, const wxString& title, bool pullDown)
{
    Widget menu = (Widget) 0;
    Widget buttonWidget = (Widget) 0;
    Arg args[5];
    XtSetArg (args[0], XmNnumColumns, m_numColumns);
    XtSetArg (args[1], XmNpacking, XmPACK_COLUMN);

    if (!pullDown)
    {
        menu = XmCreatePopupMenu ((Widget) parent, "popup", args, 2);
        XtAddCallback(menu,
            XmNunmapCallback,
            (XtCallbackProc)wxMenuPopdownCallback,
            (XtPointer)this);
    }
    else
    {
        char mnem = wxFindMnemonic (title);
        wxStripMenuCodes ((char*) (const char*) title, wxBuffer);

        menu = XmCreatePulldownMenu ((Widget) parent, "pulldown", args, 2);

        wxString title2(wxStripMenuCodes(title));
        wxXmString label_str(title2);
        buttonWidget = XtVaCreateManagedWidget(title2,
#if wxUSE_GADGETS
            xmCascadeButtonGadgetClass, (Widget) parent,
#else
            xmCascadeButtonWidgetClass, (Widget) parent,
#endif
            XmNlabelString, label_str(),
            XmNsubMenuId, menu,
            NULL);

        if (mnem != 0)
            XtVaSetValues (buttonWidget, XmNmnemonic, mnem, NULL);
    }

    m_menuWidget = (WXWidget) menu;

    m_menuBar = menuBar;
    m_topLevelMenu = topMenu;

    for ( wxMenuItemList::Node *node = GetMenuItems().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem *item = node->GetData();

        item->CreateItem(menu, menuBar, topMenu);
    }

    SetBackgroundColour(m_backgroundColour);
    SetForegroundColour(m_foregroundColour);
    SetFont(m_font);

    return buttonWidget;
}

// Destroys the Motif implementation of the menu,
// but maintains the wxWindows data structures so we can
// do a CreateMenu again.
void wxMenu::DestroyMenu (bool full)
{
    for ( wxMenuItemList::Node *node = GetMenuItems().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem *item = node->GetData();
        item->SetMenuBar((wxMenuBar*) NULL);

        item->DestroyItem(full);
    }

    if (m_buttonWidget)
    {
        if (full)
        {
            XtVaSetValues((Widget) m_buttonWidget, XmNsubMenuId, NULL, NULL);
            XtDestroyWidget ((Widget) m_buttonWidget);
            m_buttonWidget = (WXWidget) 0;
        }
    }
    if (m_menuWidget && full)
    {
        XtDestroyWidget((Widget) m_menuWidget);
        m_menuWidget = (WXWidget) NULL;
    }
}

WXWidget wxMenu::FindMenuItem (int id, wxMenuItem ** it) const
{
    if (id == m_menuId)
    {
        if (it)
            *it = (wxMenuItem*) NULL;
        return m_buttonWidget;
    }

    for ( wxMenuItemList::Node *node = GetMenuItems().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem *item = node->GetData ();
        if (item->GetId() == id)
        {
            if (it)
                *it = item;
            return item->GetButtonWidget();
        }

        if (item->GetSubMenu())
        {
            WXWidget w = item->GetSubMenu()->FindMenuItem (id, it);
            if (w)
            {
                return w;
            }
        }
    }

    if (it)
        *it = (wxMenuItem*) NULL;
    return (WXWidget) NULL;
}

void wxMenu::SetBackgroundColour(const wxColour& col)
{
    m_backgroundColour = col;
    if (m_menuWidget)
        wxDoChangeBackgroundColour(m_menuWidget, (wxColour&) col);
    if (m_buttonWidget)
        wxDoChangeBackgroundColour(m_buttonWidget, (wxColour&) col, TRUE);

    for ( wxMenuItemList::Node *node = GetMenuItems().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem* item = node->GetData();
        if (item->GetButtonWidget())
        {
            // This crashes because it uses gadgets
            //            wxDoChangeBackgroundColour(item->GetButtonWidget(), (wxColour&) col, TRUE);
        }
        if (item->GetSubMenu())
            item->GetSubMenu()->SetBackgroundColour((wxColour&) col);
    }
}

void wxMenu::SetForegroundColour(const wxColour& col)
{
    m_foregroundColour = col;
    if (m_menuWidget)
        wxDoChangeForegroundColour(m_menuWidget, (wxColour&) col);
    if (m_buttonWidget)
        wxDoChangeForegroundColour(m_buttonWidget, (wxColour&) col);

    for ( wxMenuItemList::Node *node = GetMenuItems().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem* item = node->GetData();
        if (item->GetButtonWidget())
        {
            // This crashes because it uses gadgets
            //            wxDoChangeForegroundColour(item->GetButtonWidget(), (wxColour&) col);
        }
        if (item->GetSubMenu())
            item->GetSubMenu()->SetForegroundColour((wxColour&) col);
    }
}

void wxMenu::ChangeFont(bool keepOriginalSize)
{
    // lesstif 0.87 hangs when setting XmNfontList
#ifndef LESSTIF_VERSION
    if (!m_font.Ok() || !m_menuWidget)
        return;

    XmFontList fontList = (XmFontList) m_font.GetFontList(1.0, XtDisplay((Widget) m_menuWidget));

    XtVaSetValues ((Widget) m_menuWidget,
        XmNfontList, fontList,
        NULL);
    if (m_buttonWidget)
    {
        XtVaSetValues ((Widget) m_buttonWidget,
            XmNfontList, fontList,
            NULL);
    }

    for ( wxMenuItemList::Node *node = GetMenuItems().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenuItem* item = node->GetData();
        if (m_menuWidget && item->GetButtonWidget() && m_font.Ok())
        {
            XtVaSetValues ((Widget) item->GetButtonWidget(),
                XmNfontList, fontList,
                NULL);
        }
        if (item->GetSubMenu())
            item->GetSubMenu()->ChangeFont(keepOriginalSize);
    }
#endif
}

void wxMenu::SetFont(const wxFont& font)
{
    m_font = font;
    ChangeFont();
}

bool wxMenuBar::SetBackgroundColour(const wxColour& col)
{
    m_backgroundColour = col;
    if (m_mainWidget)
        wxDoChangeBackgroundColour(m_mainWidget, (wxColour&) col);

    size_t menuCount = GetMenuCount();
    for (size_t i = 0; i < menuCount; i++)
        m_menus[i]->SetBackgroundColour((wxColour&) col);

    return TRUE;
}

bool wxMenuBar::SetForegroundColour(const wxColour& col)
{
    m_foregroundColour = col;
    if (m_mainWidget)
        wxDoChangeForegroundColour(m_mainWidget, (wxColour&) col);

    size_t menuCount = GetMenuCount();
    for (size_t i = 0; i < menuCount; i++)
        m_menus[i]->SetForegroundColour((wxColour&) col);

    return TRUE;
}

void wxMenuBar::ChangeFont(bool WXUNUSED(keepOriginalSize))
{
    // Nothing to do for menubar, fonts are kept in wxMenus
}

bool wxMenuBar::SetFont(const wxFont& font)
{
    m_font = font;
    ChangeFont();

    size_t menuCount = GetMenuCount();
    for (size_t i = 0; i < menuCount; i++)
        m_menus[i]->SetFont(font);

    return TRUE;
}

