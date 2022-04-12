/////////////////////////////////////////////////////////////////////////////
// Name:        src/uwp/menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      Yann Clotioloman Yéo
// Modified by:
// Created:     02/01/2022
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_MENUS

#include "wx/menu.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/image.h"
#endif

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

#include "wx/scopedarray.h"
#include "wx/msw/private.h"
#include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
#include "wx/private/menuradio.h" // for wxMenuRadioItemsData

// other standard headers
#include <string.h>

#include "wx/dynlib.h"

// ---------------------------------------------------------------------------
// wxMenu construction, adding and removing menu items
// ---------------------------------------------------------------------------


// The wxWindow destructor will take care of deleting the submenus.
wxMenu::~wxMenu()
{

}

void wxMenu::Break()
{
}

wxMenuItem* wxMenu::DoAppend(wxMenuItem *item)
{
    return NULL;
}

wxMenuItem* wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    return NULL;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    return wxMenuBase::DoRemove(item);
}

// ---------------------------------------------------------------------------
// Menu Bar
// ---------------------------------------------------------------------------


wxMenuBar::wxMenuBar()
{
    Init();
}

wxMenuBar::wxMenuBar( long WXUNUSED(style) )
{

}

wxMenuBar::wxMenuBar(size_t count, wxMenu *menus[], const wxString titles[], long WXUNUSED(style))
{

}

wxMenuBar::~wxMenuBar()
{

}

// ---------------------------------------------------------------------------
// wxMenuBar helpers
// ---------------------------------------------------------------------------

WXHMENU wxMenuBar::Create()
{
    return NULL;
}


// ---------------------------------------------------------------------------
// wxMenuBar functions to work with the top level submenus
// ---------------------------------------------------------------------------

// NB: we don't support owner drawn top level items for now, if we do these
//     functions would have to be changed to use wxMenuItem as well

void wxMenuBar::EnableTop(size_t pos, bool enable)
{

}

bool wxMenuBar::IsEnabledTop(size_t pos) const
{
    return false;
}

void wxMenuBar::SetMenuLabel(size_t pos, const wxString& label)
{
}

wxString wxMenuBar::GetMenuLabel(size_t pos) const
{
    return "";
}

// ---------------------------------------------------------------------------
// wxMenuBar construction
// ---------------------------------------------------------------------------

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    return NULL;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    return false;
}

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    return false;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    return NULL;
}


void wxMenuBar::Attach(wxFrame *frame)
{
    wxMenuBarBase::Attach(frame);
    return wxNOT_FOUND;
}

#endif // wxUSE_MENUS
