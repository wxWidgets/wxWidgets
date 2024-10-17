/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/notifmsgrt.cpp
// Purpose:     WinRT implementation of wxNotificationMessageImpl
// Author:      Tobias Taschner
// Created:     2015-09-13
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/module.h"
    #include "wx/string.h"
#endif // WX_PRECOMP

#include "wx/msw/rt/private/notifmsg.h"

#include "wx/notifmsg.h"
#include "wx/msw/rt/utils.h"
#include "wx/msw/private/comptr.h"
#include "wx/msw/wrapshl.h"

#include "wx/filename.h"
#include "wx/stdpaths.h"

#include <roapi.h>
#include <windows.ui.notifications.h>
#include <functiondiscoverykeys.h>
#include <propvarutil.h>
#include <wrl/implements.h>

using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;

namespace rt = wxWinRT;

typedef ABI::Windows::Foundation::ITypedEventHandler<ToastNotification *, ::IInspectable *> DesktopToastActivatedEventHandler;
typedef ABI::Windows::Foundation::ITypedEventHandler<ToastNotification *, ToastDismissedEventArgs *> DesktopToastDismissedEventHandler;
typedef ABI::Windows::Foundation::ITypedEventHandler<ToastNotification *, ToastFailedEventArgs *> DesktopToastFailedEventHandler;

class wxToastNotifMsgImpl;

class wxToastEventHandler :
    public Microsoft::WRL::Implements<DesktopToastActivatedEventHandler, DesktopToastDismissedEventHandler, DesktopToastFailedEventHandler>
{
public:
    explicit wxToastEventHandler(wxToastNotifMsgImpl* toastImpl) :
        m_impl(toastImpl)
    {

    }

    void Detach()
    {
        m_impl = nullptr;
    }

    // DesktopToastActivatedEventHandler
    IFACEMETHODIMP Invoke(IToastNotification *sender, IInspectable* args);

    // DesktopToastDismissedEventHandler
    IFACEMETHODIMP Invoke(IToastNotification *sender, IToastDismissedEventArgs *e);

    // DesktopToastFailedEventHandler
    IFACEMETHODIMP Invoke(IToastNotification *sender, IToastFailedEventArgs *e);

    // IUnknown
    STDMETHODIMP_(ULONG) AddRef() override
    {
        return ++m_cRef;
    }

    STDMETHODIMP_(ULONG) Release() override
    {
        if ( !--m_cRef )
        {
            delete this;
            return 0;
        }

        return m_cRef;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) override
    {
        if ( riid == IID_IUnknown || riid == __uuidof(DesktopToastActivatedEventHandler) )
        {
            DesktopToastActivatedEventHandler* const p = this;
            *ppv = p;
        }
        else if ( riid == __uuidof(DesktopToastDismissedEventHandler) )
        {
            DesktopToastDismissedEventHandler* const p = this;
            *ppv = p;
        }
        else if ( riid == __uuidof(DesktopToastFailedEventHandler) )
        {
            DesktopToastFailedEventHandler* const p = this;
            *ppv = p;
        }
        else
        {
            *ppv = nullptr;

            return E_NOINTERFACE;
        }

        AddRef();

        return S_OK;
    }

private:
    ULONG m_cRef = 0;

    wxToastNotifMsgImpl* m_impl;
};

class wxToastNotifMsgImpl : public wxNotificationMessageImpl
{
public:
    wxToastNotifMsgImpl(wxNotificationMessageBase* notification) :
        wxNotificationMessageImpl(notification),
        m_toastEventHandler(nullptr)
    {

    }

    virtual ~wxToastNotifMsgImpl()
    {
        if ( m_toastEventHandler )
            m_toastEventHandler->Detach();
    }

    virtual bool Show(int WXUNUSED(timeout)) override
    {
        wxCOMPtr<IXmlDocument> toastXml;
        HRESULT hr = CreateToastXML(&toastXml);
        if ( SUCCEEDED(hr) )
        {
            hr = CreateToast(toastXml);
        }

        return SUCCEEDED(hr);
    }

    virtual bool Close() override
    {
        if ( m_notifier.get() && m_toast.get() )
        {
            bool success = SUCCEEDED(m_notifier->Hide(m_toast));
            ReleaseToast();
            return success;
        }
        else
            return false;
    }

    virtual void SetTitle(const wxString& title) override
    {
        m_title = title;
    }

    virtual void SetMessage(const wxString& message) override
    {
        m_message = message;
    }

    virtual void SetParent(wxWindow *WXUNUSED(parent)) override
    {

    }

    virtual void SetFlags(int WXUNUSED(flags)) override
    {

    }

    virtual void SetIcon(const wxIcon& WXUNUSED(icon)) override
    {
        // Icon would have to be saved to disk (temporarily?)
        // to be used as a file:// url in the notifications XML
    }

    virtual bool AddAction(wxWindowID WXUNUSED(actionid), const wxString &WXUNUSED(label)) override
    {
        return false;
    }

    void ReleaseToast()
    {
        if ( m_toastEventHandler )
            m_toastEventHandler->Detach();
        m_notifier = nullptr;
        m_toast = nullptr;
    }

    HRESULT CreateToast(IXmlDocument *xml)
    {
        HRESULT hr = ms_toastMgr->CreateToastNotifierWithId(rt::TempStringRef(ms_appId), &m_notifier);
        if ( SUCCEEDED(hr) )
        {
            wxCOMPtr<IToastNotificationFactory> factory;
            hr = rt::GetActivationFactory(RuntimeClass_Windows_UI_Notifications_ToastNotification,
                IID_IToastNotificationFactory, reinterpret_cast<void**>(&factory));
            if ( SUCCEEDED(hr) )
            {
                hr = factory->CreateToastNotification(xml, &m_toast);
                if ( SUCCEEDED(hr) )
                {
                    // Register the event handlers
                    EventRegistrationToken activatedToken, dismissedToken, failedToken;
                    m_toastEventHandler = new wxToastEventHandler(this);
                    wxCOMPtr<wxToastEventHandler> eventHandler(m_toastEventHandler);

                    hr = m_toast->add_Activated(eventHandler, &activatedToken);
                    if ( SUCCEEDED(hr) )
                    {
                        hr = m_toast->add_Dismissed(eventHandler, &dismissedToken);
                        if ( SUCCEEDED(hr) )
                        {
                            hr = m_toast->add_Failed(eventHandler, &failedToken);
                            if ( SUCCEEDED(hr) )
                            {
                                hr = m_notifier->Show(m_toast);
                            }
                        }
                    }
                }
            }
        }

        if ( FAILED(hr) )
            ReleaseToast();

        return hr;
    }

    HRESULT CreateToastXML(IXmlDocument** toastXml) const
    {
        HRESULT hr = ms_toastMgr->GetTemplateContent(ToastTemplateType_ToastText02, toastXml);
        if ( SUCCEEDED(hr) )
        {
            wxCOMPtr<IXmlNodeList> nodeList;
            hr = (*toastXml)->GetElementsByTagName(rt::TempStringRef("text"), &nodeList);
            if ( SUCCEEDED(hr) )
            {
                hr = SetNodeListValueString(0, m_title, nodeList, *toastXml);
                if ( SUCCEEDED(hr) )
                    hr = SetNodeListValueString(1, m_message, nodeList, *toastXml);
            }
        }

        return hr;
    }

    static HRESULT SetNodeListValueString(UINT32 index, const wxString& str, IXmlNodeList* nodeList, IXmlDocument *toastXml)
    {
        wxCOMPtr<IXmlNode> textNode;
        // Set title node
        HRESULT hr = nodeList->Item(index, &textNode);
        if ( SUCCEEDED(hr) )
        {
            hr = SetNodeValueString(str, textNode, toastXml);
        }

        return hr;
    }

    static HRESULT SetNodeValueString(const wxString& str, IXmlNode *node, IXmlDocument *xml)
    {
        wxCOMPtr<IXmlText> inputText;

        HRESULT hr = xml->CreateTextNode(rt::TempStringRef(str), &inputText);
        if ( SUCCEEDED(hr) )
        {
            wxCOMPtr<IXmlNode> inputTextNode;

            hr = inputText->QueryInterface(IID_IXmlNode, reinterpret_cast<void**>(&inputTextNode));
            if ( SUCCEEDED(hr) )
            {
                wxCOMPtr<IXmlNode> pAppendedChild;
                hr = node->AppendChild(inputTextNode, &pAppendedChild);
            }
        }

        return hr;
    }

    static bool IsEnabled()
    {
        return ms_enabled;
    }

    static wxString BuildAppId()
    {
        // Build a Application User Model IDs based on app info
        wxString vendorId = wxTheApp->GetVendorName();
        if ( vendorId.empty() )
            vendorId = "wxWidgetsApp";
        wxString appId = vendorId + "." + wxTheApp->GetAppName();
        // Remove potential spaces
        appId.Replace(" ", "", true);
        return appId;
    }

    static bool CheckShortcut(const wxFileName& filename)
    {
        // Prepare interfaces
        wxCOMPtr<IShellLink> shellLink;
        if ( FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
            IID_IShellLinkW, reinterpret_cast<void**>(&shellLink))) )
            return false;
        wxCOMPtr<IPersistFile> persistFile;
        if ( FAILED(shellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&persistFile))) )
            return false;
        wxCOMPtr<IPropertyStore> propertyStore;
        if ( FAILED(shellLink->QueryInterface(IID_IPropertyStore, reinterpret_cast<void**>(&propertyStore))) )
            return false;

        bool writeShortcut = false;

        if ( filename.Exists() )
        {
            // Check existing shortcut for application id
            if ( SUCCEEDED(persistFile->Load(filename.GetFullPath().wc_str(), 0)) )
            {
                PROPVARIANT appIdPropVar;
                if ( SUCCEEDED(propertyStore->GetValue(PKEY_AppUserModel_ID, &appIdPropVar)) )
                {
                    wxString appId;
                    if ( appIdPropVar.vt == VT_LPWSTR )
                        appId = appIdPropVar.pwszVal;
                    if ( appId.empty() || (!ms_appId.empty() && ms_appId != appId) )
                    {
                        // Update shortcut if app id does not match or is empty
                        writeShortcut = true;
                    }
                    else if ( ms_appId.empty() )
                    {
                        // Use if no app id has been set
                        ms_appId = appId;
                    }
                }
            }
            else
                return false;
        }
        else
        {
            // Create new shortcut
            if ( FAILED(shellLink->SetPath(wxStandardPaths::Get().GetExecutablePath().t_str())) )
                return false;
            if ( FAILED(shellLink->SetArguments(wxT(""))) )
                return false;

            writeShortcut = true;
        }

        if ( writeShortcut )
        {
            if ( ms_appId.empty() )
                ms_appId = BuildAppId();

            // Set application id in shortcut
            PROPVARIANT appIdPropVar;
            if ( FAILED(InitPropVariantFromString(ms_appId.wc_str(), &appIdPropVar)) )
                return false;
            if ( FAILED(propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar)) )
                return false;
            if ( FAILED(propertyStore->Commit()) )
                return false;
            if ( FAILED(persistFile->Save(filename.GetFullPath().wc_str(), TRUE)) )
                return false;
        }

        return true;
    }

    static bool UseToasts(
        const wxString& shortcutPath,
        const wxString& appId)
    {
        ms_enabled = false;

        // WinRT runtime is required (available since Win8)
        if ( !rt::IsAvailable() )
            return false;

        // Toast notification manager has to be available
        if ( ms_toastStaticsInitialized == -1 )
        {
            if ( SUCCEEDED(rt::GetActivationFactory(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager,
                IID_IToastNotificationManagerStatics, reinterpret_cast<void**>(&ms_toastMgr))) )
            {
                ms_toastStaticsInitialized = 1;
            }
            else
                ms_toastStaticsInitialized = 0;
        }

        if ( ms_toastStaticsInitialized != 1 )
            return false;

        // Build/complete shortcut path
        wxFileName shortcutFilename(shortcutPath);
        if ( !shortcutFilename.HasName() )
            shortcutFilename.SetName(wxTheApp->GetAppDisplayName());
        if ( !shortcutFilename.HasExt() )
            shortcutFilename.SetExt("lnk");
        if ( shortcutFilename.IsRelative() )
            shortcutFilename.MakeAbsolute(wxStandardPaths::MSWGetShellDir(CSIDL_STARTMENU));

        ms_appId = appId;

        if ( CheckShortcut(shortcutFilename) )
            ms_enabled = true;

        return ms_enabled;
    }

    static void Uninitalize()
    {
        if (ms_toastStaticsInitialized == 1)
        {
            ms_toastMgr = nullptr;
            ms_toastStaticsInitialized = -1;
        }
    }

private:
    wxString m_title;
    wxString m_message;
    wxCOMPtr<IToastNotifier> m_notifier;
    wxCOMPtr<IToastNotification> m_toast;
    wxToastEventHandler* m_toastEventHandler;

    static bool ms_enabled;
    static wxString ms_appId;
    static int ms_toastStaticsInitialized;
    static wxCOMPtr<IToastNotificationManagerStatics> ms_toastMgr;

    friend class wxToastEventHandler;
};

bool wxToastNotifMsgImpl::ms_enabled = false;
int wxToastNotifMsgImpl::ms_toastStaticsInitialized = -1;
wxString wxToastNotifMsgImpl::ms_appId;
wxCOMPtr<IToastNotificationManagerStatics> wxToastNotifMsgImpl::ms_toastMgr;

HRESULT wxToastEventHandler::Invoke(
    IToastNotification *WXUNUSED(sender),
    IInspectable *WXUNUSED(args))
{
    if ( m_impl )
    {
        wxCommandEvent evt(wxEVT_NOTIFICATION_MESSAGE_CLICK);
        m_impl->ProcessNotificationEvent(evt);
    }

    return S_OK;
}

HRESULT wxToastEventHandler::Invoke(
    IToastNotification *WXUNUSED(sender),
    IToastDismissedEventArgs *e)
{
    if ( m_impl )
    {
        wxCommandEvent evt(wxEVT_NOTIFICATION_MESSAGE_DISMISSED);
        ABI::Windows::UI::Notifications::ToastDismissalReason nativeReason;
        auto reason = wxNotificationMessage::DismissalReason::Unknown;
        if ( SUCCEEDED(e->get_Reason(&nativeReason)) )
        {
            switch ( nativeReason )
            {
                case ABI::Windows::UI::Notifications::ToastDismissalReason_UserCanceled:
                    reason = wxNotificationMessage::DismissalReason::ByUser;
                    break;
                case ABI::Windows::UI::Notifications::ToastDismissalReason_ApplicationHidden:
                    reason = wxNotificationMessage::DismissalReason::ByApp;
                    break;
                case ABI::Windows::UI::Notifications::ToastDismissalReason_TimedOut:
                    reason = wxNotificationMessage::DismissalReason::TimedOut;
                    break;
            }
        }
        evt.SetInt(static_cast<int>(reason));
        m_impl->ProcessNotificationEvent(evt);
    }

    return S_OK;
}

HRESULT wxToastEventHandler::Invoke(IToastNotification *WXUNUSED(sender),
    IToastFailedEventArgs *WXUNUSED(e))
{
    //TODO: Handle toast failed event
    return S_OK;
}

//
// wxToastNotifMsgModule
//

class wxToastNotifMsgModule : public wxModule
{
public:
    wxToastNotifMsgModule()
    {
        // Using RT API requires OLE and, importantly, we must ensure our
        // OnExit() runs before it is uninitialized.
        AddDependency("wxOleInitModule");
    }

    virtual bool OnInit() override
    {
        return true;
    }

    virtual void OnExit() override
    {
        wxToastNotifMsgImpl::Uninitalize();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxToastNotifMsgModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxToastNotifMsgModule, wxModule);

//
// wxToastNotificationHelper
//

bool wxToastNotificationHelper::UseToasts(const wxString& shortcutPath,
    const wxString& appId)
{
#if wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT
    return wxToastNotifMsgImpl::UseToasts(shortcutPath, appId);
#else
    wxUnusedVar(shortcutPath);
    wxUnusedVar(appId);
    return false;
#endif
}

bool wxToastNotificationHelper::IsEnabled()
{
#if wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT
    return wxToastNotifMsgImpl::IsEnabled();
#else
    return false;
#endif
}

wxNotificationMessageImpl* wxToastNotificationHelper::CreateInstance(wxNotificationMessageBase* notification)
{
#if wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT
    return new wxToastNotifMsgImpl(notification);
#else
    wxUnusedVar(notification);
    return nullptr;
#endif
}

#endif // wxUSE_NOTIFICATION_MESSAGE && wxUSE_WINRT
