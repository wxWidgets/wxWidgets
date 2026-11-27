/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/access.mm
// Purpose:     implementation of wxAccessible on macOS
// Copyright:   (c) 2026 Joshua Lee Ockert
// Licence:     wxWindows licence
// SPDX-License-Identifier: wxWindows OR LGPL-2.0-or-later WITH WxWindows-exception-3.1
// Dedicated in loving memory to Mark John Ockert 1961-2025
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_ACCESSIBILITY
// Use wxUSE_ACCESSIBILITY guard for the whole source file.  This can allow
// unconditionally compiling the file into the target.

#include "wx/access.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
#endif

#include "wx/osx/private.h"



//////////////////////////////////////////////////////////////////////////////
//     Mapping function from wxAccessible roles to NSAccessibility roles    //
//////////////////////////////////////////////////////////////////////////////
NSAccessibilityRole wxOSXAccessibilityRoleFromWx(wxAccRole role)
{
    switch (role)
    {
        case wxROLE_SYSTEM_BUTTONDROPDOWN:
        case wxROLE_SYSTEM_BUTTONMENU:
            return NSAccessibilityPopUpButtonRole;
        case wxROLE_SYSTEM_CELL:
            return NSAccessibilityCellRole;
        case wxROLE_SYSTEM_CHECKBUTTON:
            return NSAccessibilityCheckBoxRole;
        case wxROLE_SYSTEM_COLUMNHEADER:
            return NSAccessibilityColumnRole;
        case wxROLE_SYSTEM_COMBOBOX:
            return NSAccessibilityComboBoxRole;
        case wxROLE_SYSTEM_DOCUMENT:
        case wxROLE_SYSTEM_TEXT:
            return NSAccessibilityTextAreaRole;
        case wxROLE_SYSTEM_GRAPHIC:
            return NSAccessibilityImageRole;
        case wxROLE_SYSTEM_GROUPING:
        case wxROLE_SYSTEM_CLIENT:
        case wxROLE_SYSTEM_PANE:
        case wxROLE_SYSTEM_PROPERTYPAGE:
            return NSAccessibilityGroupRole;
        case wxROLE_SYSTEM_LINK:
            return NSAccessibilityLinkRole;
        case wxROLE_SYSTEM_LIST:
            return NSAccessibilityListRole;
        case wxROLE_SYSTEM_LISTITEM:
            return NSAccessibilityStaticTextRole;
        case wxROLE_SYSTEM_MENUBAR:
            return NSAccessibilityMenuBarRole;
        case wxROLE_SYSTEM_MENUITEM:
            return NSAccessibilityMenuItemRole;
        case wxROLE_SYSTEM_MENUPOPUP:
            return NSAccessibilityMenuRole;
        case wxROLE_SYSTEM_OUTLINE:
            return NSAccessibilityOutlineRole;
        case wxROLE_SYSTEM_OUTLINEITEM:
            return NSAccessibilityRowRole;
        case wxROLE_SYSTEM_PAGETAB:
            return NSAccessibilityRadioButtonRole;
        case wxROLE_SYSTEM_PAGETABLIST:
            return NSAccessibilityTabGroupRole;
        case wxROLE_SYSTEM_PROGRESSBAR:
            return NSAccessibilityProgressIndicatorRole;
        case wxROLE_SYSTEM_PUSHBUTTON:
            return NSAccessibilityButtonRole;
        case wxROLE_SYSTEM_RADIOBUTTON:
            return NSAccessibilityRadioButtonRole;
        case wxROLE_SYSTEM_ROWHEADER:
            return NSAccessibilityRowRole;
        case wxROLE_SYSTEM_SCROLLBAR:
            return NSAccessibilityScrollBarRole;
        case wxROLE_SYSTEM_SEPARATOR:
            return NSAccessibilitySplitterRole;
        case wxROLE_SYSTEM_SLIDER:
            return NSAccessibilitySliderRole;
        case wxROLE_SYSTEM_SPINBUTTON:
            return NSAccessibilityIncrementorRole;
        case wxROLE_SYSTEM_STATICTEXT:
            return NSAccessibilityStaticTextRole;
        case wxROLE_SYSTEM_TABLE:
            return NSAccessibilityTableRole;
        case wxROLE_SYSTEM_TOOLBAR:
            return NSAccessibilityToolbarRole;
        case wxROLE_SYSTEM_TOOLTIP:
            return NSAccessibilityHelpTagRole;
        case wxROLE_SYSTEM_WINDOW:
        case wxROLE_SYSTEM_DIALOG:
        case wxROLE_SYSTEM_APPLICATION:
            return NSAccessibilityWindowRole;
        default:
            return nil;
    }
}



//////////////////////////////////////////////////////////////////////////////
//   Mapping function from wxAccessible events to NSAccessibility events    //
//////////////////////////////////////////////////////////////////////////////
static NSAccessibilityNotificationName wxOSXNotificationFromWx(int eventType)
{
    switch (eventType)
    {
        case wxACC_EVENT_OBJECT_CREATE:
            return NSAccessibilityCreatedNotification;
        case wxACC_EVENT_OBJECT_DESTROY:
            return NSAccessibilityUIElementDestroyedNotification;
        case wxACC_EVENT_OBJECT_FOCUS:
            return NSAccessibilityFocusedUIElementChangedNotification;
        case wxACC_EVENT_OBJECT_NAMECHANGE:
            return NSAccessibilityTitleChangedNotification;
        case wxACC_EVENT_OBJECT_VALUECHANGE:
        case wxACC_EVENT_OBJECT_STATECHANGE:
            return NSAccessibilityValueChangedNotification;
        case wxACC_EVENT_OBJECT_SELECTION:
        case wxACC_EVENT_OBJECT_SELECTIONADD:
        case wxACC_EVENT_OBJECT_SELECTIONREMOVE:
        case wxACC_EVENT_OBJECT_SELECTIONWITHIN:
            return NSAccessibilitySelectedChildrenChangedNotification;
        case wxACC_EVENT_OBJECT_LOCATIONCHANGE:
            return NSAccessibilityMovedNotification;
        default:
            return nil;
    }
}



//////////////////////////////////////////////////////////////////////////////
// Navigate the pointer chain from the given NSView to the wxWindowMac peer //
// in order to get the wxAccessible object. This is used heavily by the     //
// wxNSView class, which responds to NSAccessibility messages by getting    //
// the requested information from the wxAccessible object (if any).         //
// MANY widget types are subclasses of wxNSView, so this should cover the   //
// vast majority. We may need to add others in the future.                  //
//////////////////////////////////////////////////////////////////////////////
wxAccessible *wxOSXGetAccessibleFromView(NSView *view)
{
    wxWidgetImpl *impl = wxWidgetImpl::FindFromWXWidget(view);
    if (!impl)
        return nullptr;

    wxWindowMac *peer = impl->GetWXPeer();
    if (!peer)
        return nullptr;

    return peer->GetAccessible();
}



//////////////////////////////////////////////////////////////////////////////
//               Basic wxAccessible implementation for macOS                //
//////////////////////////////////////////////////////////////////////////////
wxAccessible::wxAccessible(wxWindow *win) : wxAccessibleBase(win)
{
}

wxAccessible::~wxAccessible()
{
}

//////////////////////////////////////////////////////////////////////////////
//                          Send notification events                        //
// API is compatible with the same for Windows, but not all args are used   //
// in the macOS implementation.  This is because the wxAccessible API is    //
// based on the underlying Windows API:                                     //
//     NotifyWinEvent(eventType, window->GetHWND(), objectType, objectId)   //
// And indeed, that is what the wxAccessible implementation on MSW uses.    //
// However, on macOS, the equivalent system API is                          //
//     void NSAccessibilityPostNotification(id element, NSString *notifn)   //
// where the `id` type is a pointer to any Objective-C object.  So long as  //
// we can get the pointer from window->GetHandle(), we can pass that. When  //
// objectId > 0 (i.e., it isn't wxACC_SELF), we try to find the child       //
// object and get the `id` pointer to that; otherwise we fall back to using //
// the parent object.                                                       //
//////////////////////////////////////////////////////////////////////////////
void wxAccessible::NotifyEvent(int eventType, wxWindow *window,
                        wxAccObject WXUNUSED(objectType), int objectId)
{
    id view = (window) ? window->GetHandle() : nil;
    NSAccessibilityNotificationName notification = wxOSXNotificationFromWx(eventType);

    if (!view || !notification)
        return;

    if (objectId != wxACC_SELF) {
        wxAccessible *viewacc = window->GetAccessible();
        wxAccessible *childacc = nullptr;
        wxWindow *childwin = nullptr;
        wxAccStatus getchildstatus = wxACC_FAIL;
        id childview = nil;

        if (viewacc)
            getchildstatus = viewacc->GetChild(objectId, &childacc);
        if (childacc && getchildstatus == wxACC_OK)
            childwin = childacc->GetWindow();
        if (childwin)
            childview = childwin->GetHandle();
        if (childview)
            view = childview;
    }

    id retainedview = [view retain];
    auto postnotification = [retainedview, notification]() {
        NSAccessibilityPostNotification(retainedview, notification);
        [retainedview release];
    };

    if (wxTheApp)
        wxTheApp->CallAfter(postnotification);
    else
        postnotification();
}

#endif // wxUSE_ACCESSIBILITY
