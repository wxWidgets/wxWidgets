/////////////////////////////////////////////////////////////////////////////
// Name:       	taskbar.cpp
// Purpose:     wxTaskBarIcon OSX Implementation
// Author:      Ryan Norton
// Modified by:
// Created:     09/25/2004
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/defs.h"

#ifdef wxHAS_TASK_BAR_ICON

#include "wx/mac/private.h"

#include "wx/taskbar.h"
#include "wx/menu.h"
#include "wx/icon.h"

#if 0

#include "wx/frame.h"
#include "wx/dialog.h"

#endif

IMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler)

pascal OSStatus wxDockEventHandler(	EventHandlerCallRef inHandlerCallRef,
									EventRef inEvent, void* pData)
{
    wxTaskBarIcon*& pTB = (wxTaskBarIcon*&) pData;

    const UInt32 eventClass = GetEventClass(inEvent);
    const UInt32 eventKind = GetEventKind(inEvent);
            
    if (eventClass == kEventClassCommand && eventKind == kEventCommandProcess) 
    {
	MenuRef hMenu = MAC_WXHMENU(pTB->GetCurrentMenu()->GetHMenu());
    OSStatus result = eventNotHandledErr ;

    HICommand command ;
    OSErr err;
    
    err =    GetEventParameter(inEvent, kEventParamDirectObject, typeHICommand, 	
                            NULL, sizeof(HICommand), NULL, &command);
    wxASSERT(err == noErr);
                
    MenuItemIndex menuItemIndex;
    err = GetIndMenuItemWithCommandID(hMenu, command.commandID, 1, NULL, &menuItemIndex);
    wxASSERT(err == noErr);
                
                    
    MenuCommand id = command.commandID ;
    wxMenuItem* item = NULL;
    // for items we don't really control
    if ( id == kHICommandPreferences )
    {
        id = wxApp::s_macPreferencesMenuItemId ;
        
        wxMenuBar* mbar = wxMenuBar::MacGetInstalledMenuBar() ;
        if ( mbar )
        {
            wxMenu* menu = NULL ;
            item = mbar->FindItem( id , &menu ) ;
        }
    }
   else if (id != 0)
        GetMenuItemRefCon( hMenu , menuItemIndex , (UInt32*) &item ) ;

    if ( item )
    {
                if (item->IsCheckable())
                {
                    item->Check( !item->IsChecked() ) ;
                }

                item->GetMenu()->SendEvent( id , item->IsCheckable() ? item->IsChecked() : -1 ) ;
                result = noErr ;
    }
    return result ;
    }
    
    wxASSERT(eventClass == kEventClassApplication && eventKind == kEventAppGetDockTileMenu);
	
	//set the internal event
	pTB->SetInternalEvent(inEvent);
	
	//process the right click event
	wxTaskBarIconEvent evt(wxEVT_TASKBAR_RIGHT_UP,NULL);
	pTB->ProcessEvent(evt);
		
	//set the internal event
	pTB->SetInternalEvent(NULL);

	return noErr;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxDockEventHandler );

wxTaskBarIcon::wxTaskBarIcon(const wxTaskBarIconType& nType)
    : m_nType(nType), m_pEvent(NULL), m_pMenu(NULL), m_iconAdded(false)
{
    //Register the events that will return the dock menu
	EventTypeSpec tbEventList[] = { { kEventClassCommand, kEventProcessCommand },
                                    { kEventClassApplication, kEventAppGetDockTileMenu } };
	
	OSStatus err = InstallApplicationEventHandler(
            GetwxDockEventHandlerUPP(),
            GetEventTypeCount(tbEventList), tbEventList, 
			this, NULL);
			
	wxASSERT(err == noErr);
}
wxTaskBarIcon::~wxTaskBarIcon()
{
	//TODO:uninstall event handler
}

void wxTaskBarIcon::SetInternalEvent(void* pEvent)
{
	m_pEvent = pEvent;
}

wxMenu* wxTaskBarIcon::GetCurrentMenu()
{
    return m_pMenu;
}

// Operations:
bool wxTaskBarIcon::SetIcon(const wxIcon& icon, const wxString& tooltip)
{
    #if 0
    wxASSERT(wxTheApp);
    wxWindow* pTopWindow = wxTheApp->GetTopWindow();
    
    wxASSERT(pTopWindow);
    
    if(pTopWindow->IsKindOf(CLASSINFO(wxDialog)))
        ((wxDialog*)pTopWindow)->SetIcon(icon);
    else
    {
        wxASSERT(pTopWindow->IsKindOf(CLASSINFO(wxFrame)));
        ((wxFrame*)pTopWindow)->SetIcon(icon);
    }
    
    return true;
    #else
    //TODO: Educated guess
    
	CGImageRef pImage;
	//create the icon from the bitmap and mask bitmap contained within
	OSStatus err = CreateCGImageFromPixMaps(
		GetGWorldPixMap(MAC_WXHBITMAP(icon.GetHBITMAP())),
		GetGWorldPixMap(MAC_WXHBITMAP(icon.GetMask()->GetMaskBitmap())),
		&pImage
		);
		
	wxASSERT(err == 0);
	
	err = SetApplicationDockTileImage(pImage);
		
	wxASSERT(err == 0);

        m_iconAdded = true;
	return true;
    #endif
}
	
bool wxTaskBarIcon::RemoveIcon()
{
	//TODO:  Not tested
	OSStatus err = RestoreApplicationDockTileImage();
	wxASSERT(err == 0);
        m_iconAdded = false;

	return true;
}
	
bool wxTaskBarIcon::PopupMenu(wxMenu *menu)
{
	wxASSERT(m_pEvent != NULL);
    
    if (m_pMenu)
        delete m_pMenu;
        
    m_pMenu = menu;
    menu->SetEventHandler(this);

	//note to self - a MenuRef IS A MenuHandle
	MenuRef hMenu = MAC_WXHMENU(menu->GetHMenu());

	//When we call SetEventParameter it will decrement
	//the reference count of the menu - we need to make
	//sure it stays around in the wxMenu class here
	RetainMenu(hMenu);

	//set the actual dock menu
	OSStatus err = SetEventParameter((EventRef) m_pEvent, kEventParamMenuRef, 
					typeMenuRef, sizeof(MenuRef), 
					 &hMenu);
	wxASSERT(err == 0);
	
	return true;
}

#endif //wxHAS_TASK_BAR_ICON
