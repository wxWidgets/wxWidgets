///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/notifmsg.cpp
// Purpose:     implementation of wxNotificationMessage for OSX
// Author:      Tobias Taschner
// Created:     2015-08-06
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/notifmsg.h"

#if wxUSE_NOTIFICATION_MESSAGE && defined(wxHAS_NATIVE_NOTIFICATION_MESSAGE)

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"
#include "wx/private/notifmsg.h"
#include "wx/timer.h"
#include "wx/platinfo.h"
#include "wx/artprov.h"
#include "wx/vector.h"
#include "wx/stockitem.h"

#include "wx/utils.h"
#include <map>

@interface wxUserNotificationHandler : NSObject <NSUserNotificationCenterDelegate>

@end

// ----------------------------------------------------------------------------
// wxUserNotificationMsgImpl
// ----------------------------------------------------------------------------

class wxUserNotificationMsgImpl : public wxNotificationMessageImpl
{
public:
    wxUserNotificationMsgImpl(wxNotificationMessageBase* notification) :
        wxNotificationMessageImpl(notification)
    {
        UseHandler();
        m_notif = [[NSUserNotification alloc] init];
        
        // Build Id to unqiuely idendify this notification
        m_id = wxString::Format("%d_%d", (int)wxGetProcessId(), ms_notifIdBase++);
        
        // Register the notification
        ms_activeNotifications[m_id] = this;
        
        wxCFStringRef cfId(m_id);
        m_notif.userInfo = @{
                             @"wxId" : cfId.AsNSString()
                             };
    }

    virtual ~wxUserNotificationMsgImpl()
    {
        ms_activeNotifications[m_id] = nullptr;
        ReleaseHandler();
        [m_notif release];
    }

    virtual bool Show(int WXUNUSED(timeout)) override
    {
        NSUserNotificationCenter* nc = [NSUserNotificationCenter defaultUserNotificationCenter];
        [nc deliverNotification:m_notif];
        
        return true;
    }
    
    virtual bool Close() override
    {
        NSUserNotificationCenter* nc = [NSUserNotificationCenter defaultUserNotificationCenter];
        [nc removeDeliveredNotification:m_notif];
        
        return true;
    }
    
    virtual void SetTitle(const wxString& title) override
    {
        wxCFStringRef cftitle(title);
        m_notif.title = cftitle.AsNSString();
    }
    
    virtual void SetMessage(const wxString& message) override
    {
        wxCFStringRef cfmsg(message);
        m_notif.informativeText = cfmsg.AsNSString();
    }
    
    virtual void SetParent(wxWindow *WXUNUSED(parent)) override
    {
    }
    
    virtual void SetFlags(int WXUNUSED(flags)) override
    {
        // On OS X we do not add an icon based on the flags,
        // as this is primarily meant for custom icons
    }
    
    virtual void SetIcon(const wxIcon& icon) override
    {
        m_notif.contentImage = icon.GetNSImage();
    }
    
    virtual bool AddAction(wxWindowID actionid, const wxString &label) override
    {
        if (m_actions.size() >= 1) // Currently only 1 actions are supported
            return false;
        
        wxString strLabel = label;
        if (strLabel.empty())
            strLabel = wxGetStockLabel(actionid, wxSTOCK_NOFLAGS);
        wxCFStringRef cflabel(strLabel);
        
        m_actions.push_back(actionid);
        
        if (m_actions.size() == 1)
            m_notif.actionButtonTitle = cflabel.AsNSString();
        
        return true;
    }
    
    void Activated(NSUserNotificationActivationType activationType)
    {
        switch (activationType)
        {
            case NSUserNotificationActivationTypeNone:
            {
                wxCommandEvent evt(wxEVT_NOTIFICATION_MESSAGE_DISMISSED);
                // reason for dismissal not available on Mac
                evt.SetInt(static_cast<int>(wxNotificationMessage::DismissalReason::Unknown));
                ProcessNotificationEvent(evt);
                break;
            }
            case NSUserNotificationActivationTypeContentsClicked:
            {
                wxCommandEvent evt(wxEVT_NOTIFICATION_MESSAGE_CLICK);
                ProcessNotificationEvent(evt);
                Close();
                break;
            }
            case NSUserNotificationActivationTypeActionButtonClicked:
            {
                if (m_actions.empty())
                {
                    // Without actions the action button is handled as a message click
                    wxCommandEvent evt(wxEVT_NOTIFICATION_MESSAGE_CLICK);
                    ProcessNotificationEvent(evt);
                }
                else
                {
                    wxCommandEvent evt(wxEVT_NOTIFICATION_MESSAGE_ACTION);
                    evt.SetId(m_actions[0]);
                    ProcessNotificationEvent(evt);
                }
                Close();
                break;
            }
                
            default:
                break;
        };
    }
    
    static void NotificationActivated(const wxString& notificationId, NSUserNotificationActivationType activationType)
    {
        wxUserNotificationMsgImpl* impl = ms_activeNotifications[notificationId];
        if (impl)
            impl->Activated(activationType);
    }
    
    static void UseHandler()
    {
        if (!ms_handler)
        {
            ms_handler = [wxUserNotificationHandler alloc];
            [NSUserNotificationCenter defaultUserNotificationCenter].delegate = ms_handler;
        }
    }
    
    static void ReleaseHandler()
    {
        
    }

private:
    NSUserNotification* m_notif;
    wxString m_id;
    wxVector<wxWindowID> m_actions;
    
    static wxUserNotificationHandler* ms_handler;
    static std::map<wxString, wxUserNotificationMsgImpl*> ms_activeNotifications;
    static int ms_notifIdBase;
};

wxUserNotificationHandler* wxUserNotificationMsgImpl::ms_handler = nil;
std::map<wxString, wxUserNotificationMsgImpl*> wxUserNotificationMsgImpl::ms_activeNotifications;
int wxUserNotificationMsgImpl::ms_notifIdBase = 1000;

// ----------------------------------------------------------------------------
// wxUserNotificationHandler
// ----------------------------------------------------------------------------

@implementation wxUserNotificationHandler

- (void)userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification
{
    wxUnusedVar(center);
    NSString* notifId = [notification.userInfo objectForKey:@"wxId"];
    if (notifId)
        wxUserNotificationMsgImpl::NotificationActivated(wxCFStringRef::AsString(notifId), notification.activationType);
}

@end

// ============================================================================
// implementation
// ============================================================================


// ----------------------------------------------------------------------------
// wxNotificationMessage
// ----------------------------------------------------------------------------

void wxNotificationMessage::Init()
{
    m_impl = new wxUserNotificationMsgImpl(this);
}

#endif // wxUSE_NOTIFICATION_MESSAGE && defined(wxHAS_NATIVE_NOTIFICATION_MESSAGE)
