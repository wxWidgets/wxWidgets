/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ============================================================================
// headers & declarations
// ============================================================================

// wxWindows headers
// -----------------

#ifdef __GNUG__
#pragma implementation "menu.h"
#pragma implementation "menuitem.h"
#endif

#include "wx/menu.h"
#include "wx/menuitem.h"
#include "wx/log.h"
#include "wx/utils.h"

// other standard headers
// ----------------------
#include <string.h>

IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxEvtHandler)

// ============================================================================
// implementation
// ============================================================================

// Menus

// Construct a menu with optional title (then use append)
wxMenu::wxMenu(const wxString& title, const wxFunction func)
{
    m_title = title;
    m_parent = NULL;
    m_eventHandler = this;
    m_noItems = 0;
    m_menuBar = NULL;
    m_clientData = (void*) NULL;
    if (m_title != "")
    {
        Append(-2, m_title) ;
        AppendSeparator() ;
    }

    Callback(func);

    // TODO create menu
}

// The wxWindow destructor will take care of deleting the submenus.
wxMenu::~wxMenu()
{
    // TODO destroy menu and children

    wxNode *node = m_menuItems.First();
    while (node)
    {
        wxMenuItem *item = (wxMenuItem *)node->Data();

        // Delete child menus.
        // Beware: they must not be appended to children list!!!
        // (because order of delete is significant)
        if (item->GetSubMenu())
            item->DeleteSubMenu();

        wxNode *next = node->Next();
        delete item;
        delete node;
        node = next;
    }
}

void wxMenu::Break()
{
    // TODO
}

// function appends a new item or submenu to the menu
void wxMenu::Append(wxMenuItem *pItem)
{
    // TODO

    wxCHECK_RET( pItem != NULL, "can't append NULL item to the menu" );

    m_menuItems.Append(pItem);

    m_noItems++;
}

void wxMenu::AppendSeparator()
{
    // TODO
    Append(new wxMenuItem(this, ID_SEPARATOR));
}

// Pullright item
void wxMenu::Append(int Id, const wxString& label, wxMenu *SubMenu, 
                    const wxString& helpString)
{
    Append(new wxMenuItem(this, Id, label, helpString, FALSE, SubMenu));
}

// Ordinary menu item
void wxMenu::Append(int Id, const wxString& label, 
                    const wxString& helpString, bool checkable)
{
  // 'checkable' parameter is useless for Windows.
    Append(new wxMenuItem(this, Id, label, helpString, checkable));
}

void wxMenu::Delete(int id)
{
    wxNode *node;
    wxMenuItem *item;
    int pos;

    for (pos = 0, node = m_menuItems.First(); node; node = node->Next(), pos++) {
	 item = (wxMenuItem *)node->Data();
	 if (item->GetId() == id)
		break;
    }

    if (!node)
	return;

    m_menuItems.DeleteNode(node);
    delete item;

    // TODO
}

void wxMenu::Enable(int Id, bool Flag)
{
    wxMenuItem *item = FindItemForId(Id);
    wxCHECK_RET( item != NULL, "can't enable non-existing menu item" );

    item->Enable(Flag);
}

bool wxMenu::Enabled(int Id) const
{
    wxMenuItem *item = FindItemForId(Id);
    wxCHECK( item != NULL, FALSE );

    return item->IsEnabled();
}

void wxMenu::Check(int Id, bool Flag)
{
    wxMenuItem *item = FindItemForId(Id);
    wxCHECK_RET( item != NULL, "can't get status of non-existing menu item" );

    item->Check(Flag);
}

bool wxMenu::Checked(int Id) const
{
    wxMenuItem *item = FindItemForId(Id);
    wxCHECK( item != NULL, FALSE );

    return item->IsChecked();
}

void wxMenu::SetTitle(const wxString& label)
{
    m_title = label ;
    // TODO
}

const wxString wxMenu::GetTitle() const
{
    return m_title;
}

void wxMenu::SetLabel(int id, const wxString& label)
{
    wxMenuItem *item = FindItemForId(id) ;
    if (item==NULL)
        return;

    if (item->GetSubMenu()==NULL)
    {
        // TODO
    }
    else
    {
        // TODO
    }
    item->SetName(label);
}

wxString wxMenu::GetLabel(int Id) const
{
    // TODO
    return wxString("") ;
}

// Finds the item id matching the given string, -1 if not found.
int wxMenu::FindItem (const wxString& itemString) const
{
    char buf1[200];
    char buf2[200];
    wxStripMenuCodes ((char *)(const char *)itemString, buf1);

    for (wxNode * node = m_menuItems.First (); node; node = node->Next ())
    {
      wxMenuItem *item = (wxMenuItem *) node->Data ();
      if (item->GetSubMenu())
      {
        int ans = item->GetSubMenu()->FindItem(itemString);
        if (ans > -1)
          return ans;
      }
      if ( !item->IsSeparator() )
      {
        wxStripMenuCodes((char *)item->GetName().c_str(), buf2);
        if (strcmp(buf1, buf2) == 0)
          return item->GetId();
      }
    }

    return -1;
}

wxMenuItem *wxMenu::FindItemForId(int itemId, wxMenu ** itemMenu) const
{
    if (itemMenu)
        *itemMenu = NULL;
    for (wxNode * node = m_menuItems.First (); node; node = node->Next ())
    {
        wxMenuItem *item = (wxMenuItem *) node->Data ();

        if (item->GetId() == itemId)
        {
            if (itemMenu)
                *itemMenu = (wxMenu *) this;
            return item;
        }

        if (item->GetSubMenu())
        {
            wxMenuItem *ans = item->GetSubMenu()->FindItemForId (itemId, itemMenu);
            if (ans)
                return ans;
        }
    }

    if (itemMenu)
        *itemMenu = NULL;
    return NULL;
}

void wxMenu::SetHelpString(int itemId, const wxString& helpString)
{
    wxMenuItem *item = FindItemForId (itemId);
    if (item)
        item->SetHelp(helpString);
}

wxString wxMenu::GetHelpString (int itemId) const
{
    wxMenuItem *item = FindItemForId (itemId);
    wxString str("");
    return (item == NULL) ? str : item->GetHelp();
}

void wxMenu::ProcessCommand(wxCommandEvent & event)
{
    bool processed = FALSE;

    // Try a callback
    if (m_callback)
    {
            (void) (*(m_callback)) (*this, event);
            processed = TRUE;
    }

    // Try the menu's event handler
    if ( !processed && GetEventHandler())
    {
            processed = GetEventHandler()->ProcessEvent(event);
    }
/* TODO
    // Try the window the menu was popped up from (and up
    // through the hierarchy)
    if ( !processed && GetInvokingWindow())
        processed = GetInvokingWindow()->ProcessEvent(event);
*/
}

// Update a menu and all submenus recursively.
// source is the object that has the update event handlers
// defined for it. If NULL, the menu or associated window
// will be used.
void wxMenu::UpdateUI(wxEvtHandler* source)
{
  if (!source && GetInvokingWindow())
    source = GetInvokingWindow()->GetEventHandler();
  if (!source)
    source = GetEventHandler();
  if (!source)
    source = this;

  wxNode* node = GetItems().First();
  while (node)
  {
    wxMenuItem* item = (wxMenuItem*) node->Data();
    if ( !item->IsSeparator() )
    {
      wxWindowID id = item->GetId();
      wxUpdateUIEvent event(id);
      event.SetEventObject( source );

      if (source->ProcessEvent(event))
      {
        if (event.GetSetText())
          SetLabel(id, event.GetText());
        if (event.GetSetChecked())
          Check(id, event.GetChecked());
        if (event.GetSetEnabled())
          Enable(id, event.GetEnabled());
      }

      if (item->GetSubMenu())
        item->GetSubMenu()->UpdateUI(source);
    }
    node = node->Next();
  }
}

bool wxWindow::PopupMenu(wxMenu *menu, int x, int y)
{
    menu->SetInvokingWindow(this);
    menu->UpdateUI();

    // TODO
    return FALSE;
}

// Menu Bar
wxMenuBar::wxMenuBar()
{
    m_eventHandler = this;
    m_menuCount = 0;
    m_menus = NULL;
    m_titles = NULL;
    m_menuBarFrame = NULL;

    // TODO
}

wxMenuBar::wxMenuBar(int n, wxMenu *menus[], const wxString titles[])
{
    m_eventHandler = this;
    m_menuCount = n;
    m_menus = menus;
    m_titles = new wxString[n];
    int i;
    for ( i = 0; i < n; i++ )
	m_titles[i] = titles[i];
    m_menuBarFrame = NULL;

    // TODO
}

wxMenuBar::~wxMenuBar()
{
    int i;
    for (i = 0; i < m_menuCount; i++)
    {
        delete m_menus[i];
    }
    delete[] m_menus;
    delete[] m_titles;

    // TODO
}

// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus to enable/disable items
void wxMenuBar::Enable(int id, bool flag)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;
    if (!item)
        return;

    // TODO
}

void wxMenuBar::EnableTop(int pos, bool flag)
{
    // TODO
}

// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus
void wxMenuBar::Check(int id, bool flag)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;
    if (!item)
        return;

    if (!item->IsCheckable())
        return ;

    // TODO
}

bool wxMenuBar::Checked(int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;
    if (!item)
        return FALSE;

    // TODO
    return FALSE;
}

bool wxMenuBar::Enabled(int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;
    if (!item)
        return FALSE;

    // TODO
    return FALSE ;
}


void wxMenuBar::SetLabel(int id, const wxString& label)
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    if (!item)
        return;

    // TODO
}

wxString wxMenuBar::GetLabel(int id) const
{
    wxMenu *itemMenu = NULL;
    wxMenuItem *item = FindItemForId(id, &itemMenu) ;

    if (!item)
        return wxString("");

    // TODO
    return wxString("") ;
}

void wxMenuBar::SetLabelTop(int pos, const wxString& label)
{
    // TODO
}

wxString wxMenuBar::GetLabelTop(int pos) const
{
    // TODO
    return wxString("");
}

bool wxMenuBar::OnDelete(wxMenu *a_menu, int pos)
{
    // TODO
    return FALSE;
}

bool wxMenuBar::OnAppend(wxMenu *a_menu, const char *title)
{
    // TODO
    return FALSE;
}

void wxMenuBar::Append (wxMenu * menu, const wxString& title)
{
    if (!OnAppend(menu, title))
        return;

    m_menuCount ++;
    wxMenu **new_menus = new wxMenu *[m_menuCount];
    wxString *new_titles = new wxString[m_menuCount];
    int i;

    for (i = 0; i < m_menuCount - 1; i++)
	{
        new_menus[i] = m_menus[i];
        m_menus[i] = NULL;
        new_titles[i] = m_titles[i];
        m_titles[i] = "";
    }
    if (m_menus)
    {
        delete[]m_menus;
        delete[]m_titles;
    }
    m_menus = new_menus;
    m_titles = new_titles;

    m_menus[m_menuCount - 1] = (wxMenu *)menu;
    m_titles[m_menuCount - 1] = title;

    // TODO
}

void wxMenuBar::Delete(wxMenu * menu, int i)
{
    int j;
    int ii = (int) i;

    if (menu != 0)
    {
	    for (ii = 0; ii < m_menuCount; ii++)
        {
            if (m_menus[ii] == menu)
            break;
	    }
        if (ii >= m_menuCount)
            return;
    } else
    {
        if (ii < 0 || ii >= m_menuCount)
            return;
        menu = m_menus[ii];
    }

    if (!OnDelete(menu, ii))
        return;

    menu->SetParent(NULL);

    -- m_menuCount;
    for (j = ii; j < m_menuCount; j++)
    {
        m_menus[j] = m_menus[j + 1];
        m_titles[j] = m_titles[j + 1];
    }
}

// Find the menu menuString, item itemString, and return the item id.
// Returns -1 if none found.
int wxMenuBar::FindMenuItem (const wxString& menuString, const wxString& itemString) const
{
    char buf1[200];
    char buf2[200];
    wxStripMenuCodes ((char *)(const char *)menuString, buf1);
    int i;
    for (i = 0; i < m_menuCount; i++)
    {
        wxStripMenuCodes ((char *)(const char *)m_titles[i], buf2);
        if (strcmp (buf1, buf2) == 0)
            return m_menus[i]->FindItem (itemString);
    }
    return -1;
}

wxMenuItem *wxMenuBar::FindItemForId (int Id, wxMenu ** itemMenu) const
{
    if (itemMenu)
        *itemMenu = NULL;

    wxMenuItem *item = NULL;
    int i;
    for (i = 0; i < m_menuCount; i++)
        if ((item = m_menus[i]->FindItemForId (Id, itemMenu)))
            return item;
    return NULL;
}

void wxMenuBar::SetHelpString (int Id, const wxString& helpString)
{
    int i;
    for (i = 0; i < m_menuCount; i++)
    {
        if (m_menus[i]->FindItemForId (Id))
        {
            m_menus[i]->SetHelpString (Id, helpString);
            return;
        }
    }
}

wxString wxMenuBar::GetHelpString (int Id) const
{
    int i;
    for (i = 0; i < m_menuCount; i++)
    {
        if (m_menus[i]->FindItemForId (Id))
            return wxString(m_menus[i]->GetHelpString (Id));
    }
    return wxString("");
}


