/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/taskbarbutton.cpp
// Purpose:     Implements wxTaskBarButtonImpl class for manipulating buttons on
//              the Windows taskbar.
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-06-01
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/toplevel.h"
#endif

#if wxUSE_TASKBARBUTTON

#ifdef _MSC_VER
    #pragma comment( lib, "shlwapi" )
#endif

#include "wx/msw/private.h"
#include "wx/event.h"
#include "wx/msw/taskbarbutton.h"
#include "wx/msw/private/taskbarbutton.h"
#include "wx/private/windowlifetime.h"
#include "wx/dynlib.h"
#include "wx/msw/private/comptr.h"
#include "wx/msw/private/cotaskmemptr.h"
#include "wx/weakref.h"

#include <shlwapi.h>
#include <initguid.h>

#include <array>
#include <memory>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// Redefine the interfaces: ITaskbarList3, IObjectCollection,
// ICustomDestinationList, IShellLink, IShellItem, IApplicationDocumentLists
// etc.
// ----------------------------------------------------------------------------

WINOLEAPI PropVariantClear(PROPVARIANT* pvar);

#ifndef PropVariantInit
#define PropVariantInit(pvar) memset ( (pvar), 0, sizeof(PROPVARIANT) )
#endif

#ifndef INFOTIPSIZE
#define INFOTIPSIZE 1024
#endif

namespace {

// The maximum number of thumbnail toolbar buttons allowed on windows is 7.
static const int MAX_BUTTON_COUNT = 7;

DEFINE_GUID(wxCLSID_TaskbarList,
    0x56fdf344, 0xfd6d, 0x11d0, 0x95, 0x8a, 0x0, 0x60, 0x97, 0xc9, 0xa0, 0x90);
DEFINE_GUID(wxCLSID_DestinationList,
    0x77f10cf0, 0x3db5, 0x4966, 0xb5, 0x20, 0xb7, 0xc5, 0x4f, 0xd3,0x5e, 0xd6);
DEFINE_GUID(wxCLSID_EnumerableObjectCollection,
    0x2d3468c1, 0x36a7, 0x43b6, 0xac, 0x24, 0xd3, 0xf0, 0x2f, 0xd9, 0x60, 0x7a);
DEFINE_GUID(wxCLSID_ShellLink,
    0x00021401, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
DEFINE_GUID(wxIID_ICustomDestinationList,
    0x6332debf, 0x87b5, 0x4670, 0x90, 0xc0, 0x5e, 0x57, 0xb4, 0x08, 0xa4, 0x9e);
DEFINE_GUID(wxIID_ITaskbarList3,
    0xea1afb91, 0x9e28, 0x4b86, 0x90, 0xe9, 0x9e, 0x9f, 0x8a, 0x5e, 0xef, 0xaf);
DEFINE_GUID(wxIID_IPropertyStore,
    0x886d8eeb, 0x8cf2, 0x4446, 0x8d, 0x02, 0xcd, 0xba, 0x1d, 0xbd, 0xcf, 0x99);
DEFINE_GUID(wxIID_IObjectArray,
    0x92ca9dcd, 0x5622, 0x4bba, 0xa8, 0x05, 0x5e, 0x9f, 0x54, 0x1b, 0xd8, 0xc9);
DEFINE_GUID(wxIID_IObjectCollection,
    0x5632b1a4, 0xe38a, 0x400a, 0x92, 0x8a, 0xd4, 0xcd, 0x63, 0x23, 0x02, 0x95);
DEFINE_GUID(wxIID_IApplicationDocumentLists,
    0x3c594f9f, 0x9f30, 0x47a1, 0x97, 0x9a, 0xc9, 0xe8, 0x3d, 0x3d, 0x0a, 0x06);
DEFINE_GUID(wxCLSID_ApplicationDocumentLists,
    0x86bec222, 0x30f2, 0x47e0, 0x9f, 0x25, 0x60, 0xd1, 0x1c, 0xd7, 0x5c, 0x28);
DEFINE_GUID(wxIID_IUnknown,
    0x00000000, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
DEFINE_GUID(wxIID_IShellItem,
    0x43826d1e, 0xe718, 0x42ee, 0xbc, 0x55, 0xa1, 0xe2, 0x61, 0xc3, 0x7b, 0xfe);

typedef IUnknown *HIMAGELIST;

typedef enum THUMBBUTTONFLAGS
{
    THBF_ENABLED    = 0,
    THBF_DISABLED   = 0x1,
    THBF_DISMISSONCLICK = 0x2,
    THBF_NOBACKGROUND   = 0x4,
    THBF_HIDDEN = 0x8,
    THBF_NONINTERACTIVE = 0x10
} THUMBBUTTONFLAGS;

typedef enum THUMBBUTTONMASK
{
    THB_BITMAP  = 0x1,
    THB_ICON    = 0x2,
    THB_TOOLTIP = 0x4,
    THB_FLAGS   = 0x8
} THUMBBUTTONMASK;

typedef struct THUMBBUTTON
{
    THUMBBUTTONMASK dwMask;
    UINT iId;
    UINT iBitmap;
    HICON hIcon;
    WCHAR szTip[260];
    THUMBBUTTONFLAGS dwFlags;
} THUMBBUTTON;

typedef struct THUMBBUTTON *LPTHUMBBUTTON;

typedef enum TBPFLAG
{
    TBPF_NOPROGRESS = 0,
    TBPF_INDETERMINATE  = 0x1,
    TBPF_NORMAL = 0x2,
    TBPF_ERROR  = 0x4,
    TBPF_PAUSED = 0x8
} TBPFLAG;

#ifndef PROPERTYKEY_DEFINED
typedef struct _tagpropertykey
{
    GUID fmtid;
    DWORD pid;
} PROPERTYKEY;
#endif // !PROPERTYKEY_DEFINED

#define REFPROPERTYKEY const PROPERTYKEY &

#define DEFINE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) \
    const PROPERTYKEY name  = \
    { { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }, pid }

DEFINE_PROPERTYKEY(PKEY_Title,
    0xf29f85e0, 0x4ff9, 0x1068, 0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9, 2);
DEFINE_PROPERTYKEY(PKEY_AppUserModel_IsDestListSeparator,
    0x9f4c2855, 0x9f79, 0x4b39, 0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3, 6);
DEFINE_PROPERTYKEY(PKEY_Link_Arguments,
    0x436f2667, 0x14e2, 0x4feb, 0xb3, 0x0a, 0x14, 0x6c, 0x53, 0xb5, 0xb6, 0x74, 100);

#define IShellLink      wxIShellLinkW

DEFINE_GUID(wxIID_IShellLink,
    0x000214F9, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);

typedef enum _SIGDN
{
    SIGDN_NORMALDISPLAY               = 0,
    SIGDN_PARENTRELATIVEPARSING       = (int)0x80018001,
    SIGDN_DESKTOPABSOLUTEPARSING      = (int)0x80028000,
    SIGDN_PARENTRELATIVEEDITING       = (int)0x80031001,
    SIGDN_DESKTOPABSOLUTEEDITING      = (int)0x8004c000,
    SIGDN_FILESYSPATH                 = (int)0x80058000,
    SIGDN_URL                         = (int)0x80068000,
    SIGDN_PARENTRELATIVEFORADDRESSBAR = (int)0x8007c001,
    SIGDN_PARENTRELATIVE              = (int)0x80080001
} SIGDN;

enum _SICHINTF
{
    SICHINT_DISPLAY                       = 0,
    SICHINT_ALLFIELDS                     = (int)0x80000000,
    SICHINT_CANONICAL                     = 0x10000000,
    SICHINT_TEST_FILESYSPATH_IF_NOT_EQUAL = 0x20000000
};

typedef DWORD SICHINTF;
typedef ULONG SFGAOF;

typedef enum KNOWNDESTCATEGORY
{
    KDC_FREQUENT    = 1,
    KDC_RECENT  = ( KDC_FREQUENT + 1 )
} KNOWNDESTCATEGORY;

typedef enum APPDOCLISTTYPE
{
    ADLT_RECENT   = 0,
    ADLT_FREQUENT = ( ADLT_RECENT + 1 )
} APPDOCLISTTYPE;

} // anonymous namespace

class wxITaskbarList : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL HrInit() = 0;
    virtual HRESULT wxSTDCALL AddTab(HWND) = 0;
    virtual HRESULT wxSTDCALL DeleteTab(HWND) = 0;
    virtual HRESULT wxSTDCALL ActivateTab(HWND) = 0;
    virtual HRESULT wxSTDCALL SetActiveAlt(HWND) = 0;
};

class wxITaskbarList2 : public wxITaskbarList
{
public:
    virtual HRESULT wxSTDCALL MarkFullscreenWindow(HWND, BOOL) = 0;
};

class wxIShellLinkA : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL GetPath(LPSTR, int, WIN32_FIND_DATAA*, DWORD) = 0;
    virtual HRESULT wxSTDCALL GetIDList(LPITEMIDLIST *ppidl) = 0;
    virtual HRESULT wxSTDCALL SetIDList(LPCITEMIDLIST pidl) = 0;
    virtual HRESULT wxSTDCALL GetDescription(LPSTR, int) = 0;
    virtual HRESULT wxSTDCALL SetDescription(LPCSTR) = 0;
    virtual HRESULT wxSTDCALL GetWorkingDirectory(LPSTR, int) = 0;
    virtual HRESULT wxSTDCALL SetWorkingDirectory(LPCSTR) = 0;
    virtual HRESULT wxSTDCALL GetArguments(LPSTR, int) = 0;
    virtual HRESULT wxSTDCALL SetArguments(LPCSTR) = 0;
    virtual HRESULT wxSTDCALL GetHotkey(WORD*) = 0;
    virtual HRESULT wxSTDCALL SetHotkey(WORD) = 0;
    virtual HRESULT wxSTDCALL GetShowCmd(int*) = 0;
    virtual HRESULT wxSTDCALL SetShowCmd(int) = 0;
    virtual HRESULT wxSTDCALL GetIconLocation(LPSTR, int, int*) = 0;
    virtual HRESULT wxSTDCALL SetIconLocation(LPCSTR, int) = 0;
    virtual HRESULT wxSTDCALL SetRelativePath(LPCSTR, DWORD) = 0;
    virtual HRESULT wxSTDCALL Resolve(HWND, DWORD) = 0;
    virtual HRESULT wxSTDCALL SetPath(LPCSTR) = 0;
};

class wxIShellLinkW : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL GetPath(LPWSTR, int, WIN32_FIND_DATAW*, DWORD) = 0;
    virtual HRESULT wxSTDCALL GetIDList(LPITEMIDLIST *ppidl) = 0;
    virtual HRESULT wxSTDCALL SetIDList(LPCITEMIDLIST pidl) = 0;
    virtual HRESULT wxSTDCALL GetDescription(LPWSTR, int) = 0;
    virtual HRESULT wxSTDCALL SetDescription(LPCWSTR) = 0;
    virtual HRESULT wxSTDCALL GetWorkingDirectory(LPWSTR, int) = 0;
    virtual HRESULT wxSTDCALL SetWorkingDirectory(LPCWSTR) = 0;
    virtual HRESULT wxSTDCALL GetArguments(LPWSTR, int) = 0;
    virtual HRESULT wxSTDCALL SetArguments(LPCWSTR) = 0;
    virtual HRESULT wxSTDCALL GetHotkey(WORD*) = 0;
    virtual HRESULT wxSTDCALL SetHotkey(WORD) = 0;
    virtual HRESULT wxSTDCALL GetShowCmd(int*) = 0;
    virtual HRESULT wxSTDCALL SetShowCmd(int) = 0;
    virtual HRESULT wxSTDCALL GetIconLocation(LPWSTR, int, int*) = 0;
    virtual HRESULT wxSTDCALL SetIconLocation(LPCWSTR, int) = 0;
    virtual HRESULT wxSTDCALL SetRelativePath(LPCWSTR, DWORD) = 0;
    virtual HRESULT wxSTDCALL Resolve(HWND, DWORD) = 0;
    virtual HRESULT wxSTDCALL SetPath(LPCWSTR) = 0;
};

class IShellItem : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL BindToHandler(IBindCtx*, REFGUID, REFIID, void **) = 0;
    virtual HRESULT wxSTDCALL GetParent(IShellItem **) = 0;
    virtual HRESULT wxSTDCALL GetDisplayName(SIGDN, LPWSTR*) = 0;
    virtual HRESULT wxSTDCALL GetAttributes(SFGAOF, SFGAOF*) = 0;
    virtual HRESULT wxSTDCALL Compare(IShellItem *, SICHINTF, int *) = 0;
};

class IObjectArray : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL GetCount(UINT*) = 0;
    virtual HRESULT wxSTDCALL GetAt(UINT, REFIID, void **) = 0;
};

class IObjectCollection : public IObjectArray
{
public:
    virtual HRESULT wxSTDCALL AddObject(IUnknown *) = 0;
    virtual HRESULT wxSTDCALL AddFromArray(IObjectArray *) = 0;
    virtual HRESULT wxSTDCALL RemoveObjectAt(UINT) = 0;
    virtual HRESULT wxSTDCALL Clear() = 0;
};

class IPropertyStore : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL GetCount(DWORD *) = 0;
    virtual HRESULT wxSTDCALL GetAt(DWORD, PROPERTYKEY *) = 0;
    virtual HRESULT wxSTDCALL GetValue(REFPROPERTYKEY, PROPVARIANT *) = 0;
    virtual HRESULT wxSTDCALL SetValue(REFPROPERTYKEY, const PROPVARIANT&) = 0;
    virtual HRESULT wxSTDCALL Commit() = 0;
};

class ICustomDestinationList : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL SetAppID(LPCWSTR) = 0;
    virtual HRESULT wxSTDCALL BeginList(UINT*, REFIID, void**) = 0;
    virtual HRESULT wxSTDCALL AppendCategory(LPCWSTR, IObjectArray *) = 0;
    virtual HRESULT wxSTDCALL AppendKnownCategory(KNOWNDESTCATEGORY) = 0;
    virtual HRESULT wxSTDCALL AddUserTasks(IObjectArray *) = 0;
    virtual HRESULT wxSTDCALL CommitList() = 0;
    virtual HRESULT wxSTDCALL GetRemovedDestinations(REFIID, void**) = 0;
    virtual HRESULT wxSTDCALL DeleteList(LPCWSTR) = 0;
    virtual HRESULT wxSTDCALL AbortList() = 0;
};

class IApplicationDocumentLists : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL SetAppID(LPCWSTR) = 0;
    virtual HRESULT wxSTDCALL GetList(APPDOCLISTTYPE, UINT, REFIID, void**) = 0;
};

namespace
{

inline HRESULT InitPropVariantFromBoolean(BOOL fVal, PROPVARIANT *ppropvar)
{
    ppropvar->vt = VT_BOOL;
    ppropvar->boolVal = fVal ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}

inline HRESULT InitPropVariantFromString(PCWSTR psz, PROPVARIANT *ppropvar)
{
    HRESULT hr = E_FAIL;
    ppropvar->vt = VT_LPWSTR;

    typedef HRESULT (WINAPI *SHStrDupW_t)(LPCWSTR, LPWSTR*);
    static SHStrDupW_t s_pfnSHStrDupW = nullptr;
    if ( !s_pfnSHStrDupW )
    {
        wxDynamicLibrary dll(wxT("shlwapi.dll"));
        if ( dll.IsLoaded() )
        {
            s_pfnSHStrDupW = (SHStrDupW_t)dll.GetSymbol(wxT("SHStrDupW"));
        }
    }

    if ( s_pfnSHStrDupW )
    {
        hr = s_pfnSHStrDupW(psz, &ppropvar->pwszVal);
    }

    if ( FAILED(hr) )
    {
        PropVariantInit(ppropvar);
    }
    return hr;
}

THUMBBUTTONFLAGS GetNativeThumbButtonFlags(const wxThumbBarButton& button)
{
    WXUINT flags = 0;
    flags |= (button.IsEnable() ? THBF_ENABLED : THBF_DISABLED);
    if ( button.IsDismissOnClick() )
        flags |= THBF_DISMISSONCLICK;
    if ( !button.HasBackground() )
        flags |= THBF_NOBACKGROUND;
    if ( !button.IsShown() )
        flags |= THBF_HIDDEN;
    if ( !button.IsInteractive() )
        flags |= THBF_NONINTERACTIVE;
    return static_cast<THUMBBUTTONFLAGS>(flags);
}

bool AddShellLink(IObjectCollection *collection,
                  const wxTaskBarJumpListItem& item)
{
    wxCOMPtr<IShellLink> shellLink;
    wxCOMPtr<IPropertyStore> propertyStore;

    HRESULT hr = CoCreateInstance
                 (
                     wxCLSID_ShellLink,
                     nullptr,
                     CLSCTX_INPROC_SERVER,
                     wxIID_IShellLink,
                     reinterpret_cast<void**> (&(shellLink))
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError("CoCreateInstance(wxCLSID_ShellLink)", hr);
        return false;
    }

    if ( item.GetType() == wxTASKBAR_JUMP_LIST_TASK ||
         item.GetType() == wxTASKBAR_JUMP_LIST_DESTINATION )
    {
        if ( !item.GetFilePath().IsEmpty() )
            shellLink->SetPath(item.GetFilePath().wc_str());
        if ( !item.GetArguments().IsEmpty() )
            shellLink->SetArguments(item.GetArguments().wc_str());
        if ( !item.GetIconPath().IsEmpty() )
        {
            shellLink->SetIconLocation(item.GetIconPath().wc_str(),
                                        item.GetIconIndex());
        }
        if ( !item.GetTooltip().IsEmpty() )
            shellLink->SetDescription(item.GetTooltip().wc_str());
    }

    hr = shellLink->QueryInterface(wxIID_IPropertyStore,
                                   reinterpret_cast<void**>(&(propertyStore)));
    if ( FAILED(hr) )
    {
        wxLogApiError("IShellLink(QueryInterface)", hr);
        return false;
    }

    PROPVARIANT pv;
    if ( item.GetType() == wxTASKBAR_JUMP_LIST_TASK ||
         item.GetType() == wxTASKBAR_JUMP_LIST_DESTINATION )
    {
        hr = InitPropVariantFromString(item.GetTitle().wc_str(), &pv);
        if ( SUCCEEDED(hr) )
        {
            hr = propertyStore->SetValue(PKEY_Title, pv);
        }
    }
    else if ( item.GetType() == wxTASKBAR_JUMP_LIST_SEPARATOR )
    {
        hr = InitPropVariantFromBoolean(TRUE, &pv);
        if ( SUCCEEDED(hr) )
        {
            hr = propertyStore->SetValue(PKEY_AppUserModel_IsDestListSeparator,
                                         pv);
        }
    }

    // Save the changes we made to the property store.
    propertyStore->Commit();
    PropVariantClear(&pv);

    // Add this IShellLink object to the given collection.
    hr = collection->AddObject(shellLink);

    return SUCCEEDED(hr);
}

wxTaskBarJumpListItem* GetItemFromIShellLink(IShellLink* link)
{
    if ( !link )
        return nullptr;

    wxTaskBarJumpListItem* item =
        new wxTaskBarJumpListItem(nullptr, wxTASKBAR_JUMP_LIST_DESTINATION);

    wxCOMPtr<IPropertyStore> linkProps;
    HRESULT hr = link->QueryInterface
                 (
                     wxIID_IPropertyStore,
                     reinterpret_cast<void **>(&linkProps)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError("IShellLink::QueryInterface", hr);
        return nullptr;
    }

    PROPVARIANT var;
    linkProps->GetValue(PKEY_Link_Arguments, &var);
    item->SetArguments(wxString(var.pwszVal));
    PropVariantClear(&var);

    const int bufferSize = 2048;
    wchar_t buffer[bufferSize];

    link->GetDescription(buffer, INFOTIPSIZE);
    item->SetTooltip(wxString(buffer));

    int dummyIndex;
    link->GetIconLocation(buffer, bufferSize - 1, &dummyIndex);
    item->SetIconPath(wxString(buffer));

    link->GetPath(buffer, bufferSize - 1, nullptr, 0x1);
    item->SetFilePath(wxString(buffer));

    return item;
}

wxTaskBarJumpListItem* GetItemFromIShellItem(IShellItem *shellItem)
{
    if ( !shellItem )
        return nullptr;

    wxTaskBarJumpListItem *item =
        new wxTaskBarJumpListItem(nullptr, wxTASKBAR_JUMP_LIST_DESTINATION);

    wxCoTaskMemPtr<wchar_t> name;
    shellItem->GetDisplayName(SIGDN_FILESYSPATH, &name);
    item->SetFilePath(wxString(name));
    return item;
}

IObjectCollection* CreateObjectCollection()
{
    IObjectCollection* collection;

    HRESULT hr;
    hr = CoCreateInstance
         (
             wxCLSID_EnumerableObjectCollection,
             nullptr,
             CLSCTX_INPROC,
             wxIID_IObjectCollection,
             reinterpret_cast<void**>(&(collection))
         );
    if ( FAILED(hr) )
    {
        wxLogApiError("CoCreateInstance(wxCLSID_EnumerableObjectCollection)",
                      hr);
        return nullptr;
    }

    return collection;
}

} // namespace

class wxITaskbarList3 : public wxITaskbarList2
{
public:
    virtual HRESULT wxSTDCALL SetProgressValue(HWND, ULONGLONG, ULONGLONG) = 0;
    virtual HRESULT wxSTDCALL SetProgressState(HWND, TBPFLAG) = 0;
    virtual HRESULT wxSTDCALL RegisterTab(HWND, HWND) = 0;
    virtual HRESULT wxSTDCALL UnregisterTab(HWND) = 0;
    virtual HRESULT wxSTDCALL SetTabOrder(HWND, HWND) = 0;
    virtual HRESULT wxSTDCALL SetTabActive(HWND, HWND, DWORD) = 0;
    virtual HRESULT wxSTDCALL ThumbBarAddButtons(HWND, UINT, LPTHUMBBUTTON) = 0;
    virtual
        HRESULT wxSTDCALL ThumbBarUpdateButtons(HWND, UINT, LPTHUMBBUTTON) = 0;
    virtual HRESULT wxSTDCALL ThumbBarSetImageList(HWND, ::HIMAGELIST) = 0;
    virtual HRESULT wxSTDCALL SetOverlayIcon(HWND, HICON, LPCWSTR) = 0;
    virtual HRESULT wxSTDCALL SetThumbnailTooltip(HWND, LPCWSTR pszTip) = 0;
    virtual HRESULT wxSTDCALL SetThumbnailClip(HWND, RECT *) = 0;
};

struct wxTaskBarButtonStateData;

namespace
{

class wxMSWTaskBarParentObserver final : public wxEvtHandler
{
public:
    explicit wxMSWTaskBarParentObserver(
        const std::weak_ptr<wxTaskBarButtonStateData>& state)
        : m_state(state)
    {
    }

    void OnDestroy(wxWindowDestroyEvent& event);

private:
    std::weak_ptr<wxTaskBarButtonStateData> m_state;
};

} // anonymous namespace

// All mutable shell state lives independently of the public wrapper. A shell
// seam is allowed to destroy the owning frame synchronously; the operation on
// the stack retains this object and its backend, observes the retired flag and
// never returns through a dangling wxWindow or COM pointer.
struct wxTaskBarButtonStateData
{
    wxWeakRef<wxWindow> parent;
    wxWindow* parentIdentity { nullptr };
    WXHWND hwnd { nullptr };
    unsigned long long hwndGeneration { 0 };
    std::shared_ptr<wxMSWTaskBarButtonNativeBackend> backend;
    std::unique_ptr<wxMSWTaskBarParentObserver> parentObserver;
    bool parentObserverBound { false };
    bool retired { false };
    unsigned long long identityRevision { 1 };
    unsigned long long shellEpoch { 1 };
    bool rebindInProgress { false };
    bool rebindPending { false };

    int desiredProgressRange { 0 };
    int desiredProgressValue { 0 };
    // Zero is a valid value, so it can't double as the sentinel used to decide
    // whether Realize() has a determinate value to replay.
    bool desiredProgressValueKnown { false };
    wxTaskBarButtonState desiredProgressState {
        wxTASKBAR_BUTTON_NO_PROGRESS
    };
    unsigned long long progressValueRevision { 0 };
    unsigned long long progressStateRevision { 0 };
    bool progressValueAppliedKnown { false };
    int appliedProgressRange { 0 };
    int appliedProgressValue { 0 };
    bool progressStateAppliedKnown { false };
    wxTaskBarButtonState appliedProgressState {
        wxTASKBAR_BUTTON_NO_PROGRESS
    };

    bool tabVisibilityDesiredKnown { false };
    bool desiredTabVisible { true };
    unsigned long long tabVisibilityRevision { 0 };
    bool tabVisibilityAppliedKnown { false };
    bool appliedTabVisible { true };

    wxString desiredThumbnailTooltip;
    unsigned long long tooltipRevision { 0 };
    bool tooltipAppliedKnown { false };

    wxIcon desiredOverlayIcon;
    wxString desiredOverlayDescription;
    unsigned long long overlayRevision { 0 };
    bool overlayAppliedKnown { false };

    wxRect desiredThumbnailClip;
    unsigned long long clipRevision { 0 };
    bool clipAppliedKnown { false };

    unsigned long long thumbRevision { 0 };
    std::vector<wxMSWTaskBarThumbButtonNative> desiredThumbButtons;
    std::size_t desiredThumbButtonCount { 0 };
    bool thumbButtonsInitialized { false };
    bool thumbButtonsAppliedKnown { false };
    std::size_t appliedThumbButtonCount { 0 };
    bool thumbApplyInProgress { false };
    bool thumbApplyPending { false };
};

namespace
{

const wchar_t wxTASKBAR_BUTTON_GENERATION_PROPERTY[] =
    L"wxWidgets.TaskBarButton.HwndGeneration";
ULONG_PTR gs_nextTaskBarButtonHwndGeneration = 0;
wxMSWTaskBarButtonNativeFactoryForTesting
    gs_taskBarButtonNativeFactoryForTesting = nullptr;

bool wxMSWTaskBarResultSucceeded(long result)
{
    return SUCCEEDED(static_cast<HRESULT>(result));
}

unsigned long long wxMSWTaskBarGetOrCreateHwndGeneration(
    wxWindow* window,
    WXHWND wxHwnd)
{
    const HWND hwnd = static_cast<HWND>(wxHwnd);
    if ( !window || !hwnd || !::IsWindow(hwnd) ||
         GetHwndOf(window) != hwnd || wxFindWinFromHandle(hwnd) != window )
    {
        return 0;
    }

    ULONG_PTR generation = reinterpret_cast<ULONG_PTR>(
        ::GetPropW(hwnd, wxTASKBAR_BUTTON_GENERATION_PROPERTY));
    if ( generation )
        return static_cast<unsigned long long>(generation);

    if ( ++gs_nextTaskBarButtonHwndGeneration == 0 )
        ++gs_nextTaskBarButtonHwndGeneration;
    generation = gs_nextTaskBarButtonHwndGeneration;
    if ( !::SetPropW(hwnd,
                     wxTASKBAR_BUTTON_GENERATION_PROPERTY,
                     reinterpret_cast<HANDLE>(generation)) )
    {
        return 0;
    }

    return ::IsWindow(hwnd) &&
                   wxFindWinFromHandle(hwnd) == window &&
                   GetHwndOf(window) == hwnd &&
                   reinterpret_cast<ULONG_PTR>(
                       ::GetPropW(
                           hwnd,
                           wxTASKBAR_BUTTON_GENERATION_PROPERTY)) == generation
        ? static_cast<unsigned long long>(generation)
        : 0;
}

bool wxMSWTaskBarHasExactIdentity(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state || state->retired || !state->backend ||
         !state->parentIdentity || !state->hwnd ||
         !state->hwndGeneration )
    {
        return false;
    }

    wxWindow* const parent = state->parent.get();
    const HWND hwnd = static_cast<HWND>(state->hwnd);
    return parent == state->parentIdentity &&
           !wxWindowIsUnavailableForCallbacks(parent) &&
           GetHwndOf(parent) == hwnd && ::IsWindow(hwnd) &&
           wxFindWinFromHandle(hwnd) == parent &&
           reinterpret_cast<ULONG_PTR>(
               ::GetPropW(hwnd,
                          wxTASKBAR_BUTTON_GENERATION_PROPERTY)) ==
               state->hwndGeneration;
}

void wxMSWTaskBarRetireState(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state || state->retired )
        return;

    // Invalidate every observable identity before releasing COM. A hostile
    // backend Release() can run arbitrary code and must already see this state
    // as unavailable.
    state->retired = true;
    state->parent.Release();
    state->parentIdentity = nullptr;
    state->hwnd = nullptr;
    state->hwndGeneration = 0;
    if ( ++state->identityRevision == 0 )
        ++state->identityRevision;
    state->progressValueAppliedKnown = false;
    state->progressStateAppliedKnown = false;
    state->tabVisibilityAppliedKnown = false;
    state->tooltipAppliedKnown = false;
    state->overlayAppliedKnown = false;
    state->clipAppliedKnown = false;
    state->thumbButtonsAppliedKnown = false;
    state->thumbButtonsInitialized = false;
    state->thumbApplyPending = false;
    state->rebindPending = false;
    state->backend.reset();
}

void wxMSWTaskBarParentObserver::OnDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state.lock();
    if ( state && event.GetWindow() == state->parentIdentity )
        wxMSWTaskBarRetireState(state);
}

void wxMSWTaskBarDisconnectParentObserver(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state || !state->parentObserverBound || !state->parentObserver )
        return;

    // Clear the fact before Unbind(): event-table teardown must see this as an
    // idempotent transaction even if it synchronously destroys the wrapper.
    state->parentObserverBound = false;
    wxWindow* const parent = state->parent.get();
    if ( parent && parent == state->parentIdentity &&
         !wxWindowItselfIsUnavailableForCallbacks(parent) )
    {
        parent->Unbind(wxEVT_DESTROY,
                       &wxMSWTaskBarParentObserver::OnDestroy,
                       state->parentObserver.get());
    }
}

struct wxMSWTaskBarOperationLease
{
    std::shared_ptr<wxMSWTaskBarButtonNativeBackend> backend;
    WXHWND hwnd { nullptr };
    unsigned long long hwndGeneration { 0 };
    unsigned long long identityRevision { 0 };
    unsigned long long shellEpoch { 0 };
};

bool wxMSWTaskBarAcquireOperation(
    const std::shared_ptr<wxTaskBarButtonStateData>& state,
    wxMSWTaskBarOperationLease* lease)
{
    if ( !lease || !wxMSWTaskBarHasExactIdentity(state) )
        return false;

    lease->backend = state->backend;
    lease->hwnd = state->hwnd;
    lease->hwndGeneration = state->hwndGeneration;
    lease->identityRevision = state->identityRevision;
    lease->shellEpoch = state->shellEpoch;
    return lease->backend && wxMSWTaskBarHasExactIdentity(state) &&
           state->backend == lease->backend &&
           state->hwnd == lease->hwnd &&
           state->hwndGeneration == lease->hwndGeneration &&
           state->identityRevision == lease->identityRevision;
}

bool wxMSWTaskBarOperationIsCurrent(
    const std::shared_ptr<wxTaskBarButtonStateData>& state,
    const wxMSWTaskBarOperationLease& lease)
{
    return wxMSWTaskBarHasExactIdentity(state) &&
           state->backend == lease.backend &&
           state->hwnd == lease.hwnd &&
           state->hwndGeneration == lease.hwndGeneration &&
           state->identityRevision == lease.identityRevision &&
           state->shellEpoch == lease.shellEpoch;
}

class wxMSWTaskBarButtonCOMBackend final
    : public wxMSWTaskBarButtonNativeBackend
{
public:
    ~wxMSWTaskBarButtonCOMBackend() override
    {
        if ( m_taskbarList )
            m_taskbarList->Release();
    }

    long Initialize() override
    {
        if ( m_taskbarList )
            return S_OK;

        HRESULT hr = CoCreateInstance(
            wxCLSID_TaskbarList,
            nullptr,
            CLSCTX_INPROC_SERVER,
            wxIID_ITaskbarList3,
            reinterpret_cast<void**>(&m_taskbarList));
        if ( FAILED(hr) )
            return hr;

        hr = m_taskbarList->HrInit();
        if ( FAILED(hr) )
        {
            m_taskbarList->Release();
            m_taskbarList = nullptr;
        }
        return hr;
    }

    long AddTab(WXHWND hwnd) override
    {
        return m_taskbarList
            ? m_taskbarList->AddTab(static_cast<HWND>(hwnd))
            : E_POINTER;
    }

    long DeleteTab(WXHWND hwnd) override
    {
        return m_taskbarList
            ? m_taskbarList->DeleteTab(static_cast<HWND>(hwnd))
            : E_POINTER;
    }

    long SetProgressValue(WXHWND hwnd,
                          unsigned long long value,
                          unsigned long long range) override
    {
        return m_taskbarList
            ? m_taskbarList->SetProgressValue(
                  static_cast<HWND>(hwnd), value, range)
            : E_POINTER;
    }

    long SetProgressState(WXHWND hwnd, unsigned state) override
    {
        return m_taskbarList
            ? m_taskbarList->SetProgressState(
                  static_cast<HWND>(hwnd), static_cast<TBPFLAG>(state))
            : E_POINTER;
    }

    long SetOverlayIcon(WXHWND hwnd,
                        void* icon,
                        const wxString& description) override
    {
        return m_taskbarList
            ? m_taskbarList->SetOverlayIcon(
                  static_cast<HWND>(hwnd),
                  static_cast<HICON>(icon),
                  description.wc_str())
            : E_POINTER;
    }

    long SetThumbnailTooltip(WXHWND hwnd,
                             const wxString& tooltip) override
    {
        return m_taskbarList
            ? m_taskbarList->SetThumbnailTooltip(
                  static_cast<HWND>(hwnd), tooltip.wc_str())
            : E_POINTER;
    }

    long SetThumbnailClip(WXHWND hwnd, const wxRect* rect) override
    {
        RECT nativeRect;
        RECT* nativeRectPtr = nullptr;
        if ( rect )
        {
            wxCopyRectToRECT(*rect, nativeRect);
            nativeRectPtr = &nativeRect;
        }
        return m_taskbarList
            ? m_taskbarList->SetThumbnailClip(
                  static_cast<HWND>(hwnd), nativeRectPtr)
            : E_POINTER;
    }

    long ThumbBarAddButtons(
        WXHWND hwnd,
        std::size_t count,
        const wxMSWTaskBarThumbButtonNative* buttons) override
    {
        return ThumbBarButtons(true, hwnd, count, buttons);
    }

    long ThumbBarUpdateButtons(
        WXHWND hwnd,
        std::size_t count,
        const wxMSWTaskBarThumbButtonNative* buttons) override
    {
        return ThumbBarButtons(false, hwnd, count, buttons);
    }

private:
    long ThumbBarButtons(
        bool add,
        WXHWND hwnd,
        std::size_t count,
        const wxMSWTaskBarThumbButtonNative* buttons)
    {
        if ( !m_taskbarList || !buttons || count > MAX_BUTTON_COUNT )
            return E_INVALIDARG;

        THUMBBUTTON nativeButtons[MAX_BUTTON_COUNT];
        for ( std::size_t i = 0; i < count; ++i )
        {
            memset(&nativeButtons[i], 0, sizeof(nativeButtons[i]));
            nativeButtons[i].iId = buttons[i].token;
            nativeButtons[i].dwFlags = buttons[i].occupied
                ? static_cast<THUMBBUTTONFLAGS>(buttons[i].flags)
                : THBF_HIDDEN;
            // Shell slots can't be removed after ThumbBarAddButtons(). Always
            // update icon and tooltip as well as flags, including with empty
            // values, so hiding/reusing a slot can't inherit stale contents.
            nativeButtons[i].hIcon = static_cast<HICON>(buttons[i].icon);
            nativeButtons[i].dwMask = static_cast<THUMBBUTTONMASK>(
                THB_FLAGS | THB_ICON | THB_TOOLTIP);
            if ( !buttons[i].tooltip.empty() )
            {
                wxStrlcpy(nativeButtons[i].szTip,
                          buttons[i].tooltip.wc_str(),
                          WXSIZEOF(nativeButtons[i].szTip));
            }
        }

        return add
            ? m_taskbarList->ThumbBarAddButtons(
                  static_cast<HWND>(hwnd),
                  static_cast<UINT>(count), nativeButtons)
            : m_taskbarList->ThumbBarUpdateButtons(
                  static_cast<HWND>(hwnd),
                  static_cast<UINT>(count), nativeButtons);
    }

    wxITaskbarList3* m_taskbarList { nullptr };
};

std::shared_ptr<wxMSWTaskBarButtonNativeBackend>
wxMSWCreateTaskBarButtonNativeBackend()
{
    return gs_taskBarButtonNativeFactoryForTesting
        ? gs_taskBarButtonNativeFactoryForTesting()
        : std::make_shared<wxMSWTaskBarButtonCOMBackend>();
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxTaskBarJumpListImpl: definition of class for internal taskbar jump list
// implementation.
// ----------------------------------------------------------------------------
class wxTaskBarJumpListImpl
{
public:
    wxTaskBarJumpListImpl(wxTaskBarJumpList *jumpList = nullptr,
                          const wxString& appID = wxEmptyString);
    virtual ~wxTaskBarJumpListImpl();
    void ShowRecentCategory(bool shown = true);
    void HideRecentCategory();
    void ShowFrequentCategory(bool shown = true);
    void HideFrequentCategory();

    wxTaskBarJumpListCategory& GetTasks();
    const wxTaskBarJumpListCategory& GetFrequentCategory();
    const wxTaskBarJumpListCategory& GetRecentCategory();
    const wxTaskBarJumpListCategories& GetCustomCategories() const;

    void AddCustomCategory(wxTaskBarJumpListCategory* category);
    wxTaskBarJumpListCategory* RemoveCustomCategory(const wxString& title);
    void DeleteCustomCategory(const wxString& title);
    void Update();

private:
    bool BeginUpdate();
    bool CommitUpdate();
    void AddTasksToDestinationList();
    void AddCustomCategoriesToDestinationList();
    void LoadKnownCategory(const wxString& title);

    wxTaskBarJumpList *m_jumpList;

    wxCOMPtr<ICustomDestinationList>    m_destinationList;
    wxCOMPtr<IObjectArray>              m_objectArray;

    std::unique_ptr<wxTaskBarJumpListCategory> m_tasks;
    std::unique_ptr<wxTaskBarJumpListCategory> m_frequent;
    std::unique_ptr<wxTaskBarJumpListCategory> m_recent;
    wxTaskBarJumpListCategories m_customCategories;
    bool m_recent_visible;
    bool m_frequent_visible;

    // Application User Model ID.
    wxString m_appID;
};

// ----------------------------------------------------------------------------
// wxThumbBarButton Implementation.
// ----------------------------------------------------------------------------
wxIMPLEMENT_DYNAMIC_CLASS(wxThumbBarButton, wxObject);

wxThumbBarButton::wxThumbBarButton(int id,
                                   const wxIcon& icon,
                                   const wxString& tooltip,
                                   bool enable,
                                   bool dismissOnClick,
                                   bool hasBackground,
                                   bool shown,
                                   bool interactive)
    : m_id(id),
      m_icon(icon),
      m_tooltip(tooltip),
      m_enable(enable),
      m_dismissOnClick(dismissOnClick),
      m_hasBackground(hasBackground),
      m_shown(shown),
      m_interactive(interactive),
      m_taskBarButtonParent(nullptr)
{
}

bool wxThumbBarButton::Create(int id,
                              const wxIcon& icon,
                              const wxString& tooltip,
                              bool enable,
                              bool dismissOnClick,
                              bool hasBackground,
                              bool shown,
                              bool interactive)
{
    m_id = id;
    m_icon = icon;
    m_tooltip = tooltip;
    m_enable = enable;
    m_dismissOnClick = dismissOnClick;
    m_hasBackground = hasBackground;
    m_shown = shown;
    m_interactive = interactive;
    return true;
}

void wxThumbBarButton::Enable(bool enable)
{
    if ( m_enable != enable )
    {
        m_enable = enable;
        UpdateParentTaskBarButton();
    }
}

void wxThumbBarButton::SetHasBackground(bool has)
{
    if ( m_hasBackground != has )
    {
        m_hasBackground = has;
        UpdateParentTaskBarButton();
    }
}

void wxThumbBarButton::EnableDismissOnClick(bool enable)
{
    if ( m_dismissOnClick != enable )
    {
        m_dismissOnClick = enable;
        UpdateParentTaskBarButton();
    }
}

void wxThumbBarButton::Show(bool shown)
{
    if ( m_shown != shown )
    {
        m_shown = shown;
        UpdateParentTaskBarButton();
    }
}

void wxThumbBarButton::SetInteractive(bool interactive)
{
    if ( m_interactive != interactive )
    {
        m_interactive = interactive;
        UpdateParentTaskBarButton();
    }
}

bool wxThumbBarButton::UpdateParentTaskBarButton()
{
    if ( !m_taskBarButtonParent )
        return false;

    return static_cast<wxTaskBarButtonImpl*>(
               m_taskBarButtonParent)->InitOrUpdateThumbBarButtons();
}

// ----------------------------------------------------------------------------
// wxTaskBarButtonImpl Implementation.
// ----------------------------------------------------------------------------

namespace
{

wxTaskBarButtonState wxMSWTaskBarStateAfterProgressValue(
    wxTaskBarButtonState state)
{
    // This is the documented ITaskbarList3 state transition: setting a value
    // starts a normal indicator from NOPROGRESS/INDETERMINATE, but deliberately
    // retains the ERROR and PAUSED visual states.
    return state == wxTASKBAR_BUTTON_NO_PROGRESS ||
                   state == wxTASKBAR_BUTTON_INDETERMINATE
        ? wxTASKBAR_BUTTON_NORMAL
        : state;
}

wxTaskBarButtonState wxMSWTaskBarDesiredStateAfterProgressValue(
    wxTaskBarButtonState state,
    int value,
    int range)
{
    // wxTaskBarButton's public contract dismisses a completed progress bar.
    // ITaskbarList3 first makes the value determinate, so the caller must
    // explicitly restore NOPROGRESS after the successful value write.
    if ( value == range )
        return wxTASKBAR_BUTTON_NO_PROGRESS;

    return wxMSWTaskBarStateAfterProgressValue(state);
}

bool wxMSWTaskBarApplyProgressValue(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state )
        return false;

    const unsigned long long revision = state->progressValueRevision;
    const unsigned long long stateRevision = state->progressStateRevision;
    const int range = state->desiredProgressRange;
    const int value = state->desiredProgressValue;
    const bool appliedStateWasKnown = state->progressStateAppliedKnown;
    const wxTaskBarButtonState appliedStateBefore =
        state->appliedProgressState;
    if ( !state->desiredProgressValueKnown || range <= 0 )
    {
        state->progressValueAppliedKnown = false;
        return false;
    }

    wxMSWTaskBarOperationLease lease;
    if ( !wxMSWTaskBarAcquireOperation(state, &lease) )
    {
        state->progressValueAppliedKnown = false;
        return false;
    }

    const long result = lease.backend->SetProgressValue(
        lease.hwnd,
        static_cast<unsigned long long>(value),
        static_cast<unsigned long long>(range));
    const bool operationCurrent =
        wxMSWTaskBarOperationIsCurrent(state, lease);
    const bool valueCurrent = operationCurrent &&
                              state->progressValueRevision == revision;
    const bool succeeded = wxMSWTaskBarResultSucceeded(result);
    if ( valueCurrent && succeeded )
    {
        state->appliedProgressRange = range;
        state->appliedProgressValue = value;
        state->progressValueAppliedKnown = true;
    }
    else if ( valueCurrent )
    {
        state->progressValueAppliedKnown = false;
    }

    // SetProgressValue changes the shell state too. Publish that side effect
    // only if no nested state write superseded this operation. When the prior
    // shell state was unknown, the result is still ambiguous (ERROR/PAUSED are
    // retained), and the caller will issue SetProgressState to reconcile it.
    const bool stateCurrent = operationCurrent &&
                              state->progressStateRevision == stateRevision;
    if ( stateCurrent )
    {
        if ( succeeded && appliedStateWasKnown )
        {
            state->appliedProgressState =
                wxMSWTaskBarStateAfterProgressValue(appliedStateBefore);
            state->progressStateAppliedKnown = true;
        }
        else
        {
            state->progressStateAppliedKnown = false;
        }
    }

    if ( !succeeded )
        wxLogApiError(wxT("ITaskbarList3::SetProgressValue"), result);

    return valueCurrent && succeeded;
}

void wxMSWTaskBarApplyProgressState(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state )
        return;

    const unsigned long long revision = state->progressStateRevision;
    const wxTaskBarButtonState desired = state->desiredProgressState;
    wxMSWTaskBarOperationLease lease;
    if ( !wxMSWTaskBarAcquireOperation(state, &lease) )
    {
        state->progressStateAppliedKnown = false;
        return;
    }

    const long result = lease.backend->SetProgressState(
        lease.hwnd, static_cast<unsigned>(desired));
    const bool current = wxMSWTaskBarOperationIsCurrent(state, lease) &&
                         state->progressStateRevision == revision;
    if ( current && wxMSWTaskBarResultSucceeded(result) )
    {
        state->appliedProgressState = desired;
        state->progressStateAppliedKnown = true;
    }
    else if ( current )
    {
        state->progressStateAppliedKnown = false;
    }

    if ( !wxMSWTaskBarResultSucceeded(result) )
        wxLogApiError(wxT("ITaskbarList3::SetProgressState"), result);
}

void wxMSWTaskBarApplyTabVisibility(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state || !state->tabVisibilityDesiredKnown )
        return;

    const unsigned long long revision = state->tabVisibilityRevision;
    const bool visible = state->desiredTabVisible;
    wxMSWTaskBarOperationLease lease;
    if ( !wxMSWTaskBarAcquireOperation(state, &lease) )
    {
        state->tabVisibilityAppliedKnown = false;
        return;
    }

    const long result = visible
        ? lease.backend->AddTab(lease.hwnd)
        : lease.backend->DeleteTab(lease.hwnd);
    const bool current = wxMSWTaskBarOperationIsCurrent(state, lease) &&
                         state->tabVisibilityRevision == revision;
    if ( current && wxMSWTaskBarResultSucceeded(result) )
    {
        state->appliedTabVisible = visible;
        state->tabVisibilityAppliedKnown = true;
    }
    else if ( current )
    {
        state->tabVisibilityAppliedKnown = false;
    }

    if ( !wxMSWTaskBarResultSucceeded(result) )
    {
        wxLogApiError(visible ? wxT("ITaskbarList3::AddTab")
                              : wxT("ITaskbarList3::DeleteTab"),
                      result);
    }
}

void wxMSWTaskBarApplyTooltip(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state )
        return;

    const unsigned long long revision = state->tooltipRevision;
    const wxString tooltip = state->desiredThumbnailTooltip;
    wxMSWTaskBarOperationLease lease;
    if ( !wxMSWTaskBarAcquireOperation(state, &lease) )
    {
        state->tooltipAppliedKnown = false;
        return;
    }

    const long result = lease.backend->SetThumbnailTooltip(
        lease.hwnd, tooltip);
    const bool current = wxMSWTaskBarOperationIsCurrent(state, lease) &&
                         state->tooltipRevision == revision;
    if ( current )
        state->tooltipAppliedKnown = wxMSWTaskBarResultSucceeded(result);

    if ( !wxMSWTaskBarResultSucceeded(result) )
        wxLogApiError(wxT("ITaskbarList3::SetThumbnailTooltip"), result);
}

void wxMSWTaskBarApplyOverlay(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state )
        return;

    const unsigned long long revision = state->overlayRevision;
    const wxIcon icon = state->desiredOverlayIcon;
    const wxString description = state->desiredOverlayDescription;
    wxMSWTaskBarOperationLease lease;
    if ( !wxMSWTaskBarAcquireOperation(state, &lease) )
    {
        state->overlayAppliedKnown = false;
        return;
    }

    const long result = lease.backend->SetOverlayIcon(
        lease.hwnd,
        icon.IsOk() ? static_cast<void*>(GetHiconOf(icon)) : nullptr,
        description);
    const bool current = wxMSWTaskBarOperationIsCurrent(state, lease) &&
                         state->overlayRevision == revision;
    if ( current )
        state->overlayAppliedKnown = wxMSWTaskBarResultSucceeded(result);

    if ( !wxMSWTaskBarResultSucceeded(result) )
        wxLogApiError(wxT("ITaskbarList3::SetOverlayIcon"), result);
}

void wxMSWTaskBarApplyClip(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state )
        return;

    const unsigned long long revision = state->clipRevision;
    const wxRect clip = state->desiredThumbnailClip;
    wxMSWTaskBarOperationLease lease;
    if ( !wxMSWTaskBarAcquireOperation(state, &lease) )
    {
        state->clipAppliedKnown = false;
        return;
    }

    const long result = lease.backend->SetThumbnailClip(
        lease.hwnd, clip.IsEmpty() ? nullptr : &clip);
    const bool current = wxMSWTaskBarOperationIsCurrent(state, lease) &&
                         state->clipRevision == revision;
    if ( current )
        state->clipAppliedKnown = wxMSWTaskBarResultSucceeded(result);

    if ( !wxMSWTaskBarResultSucceeded(result) )
        wxLogApiError(wxT("ITaskbarList3::SetThumbnailClip"), result);
}

bool wxMSWTaskBarApplyThumbButtonsOnce(
    const std::shared_ptr<wxTaskBarButtonStateData>& state,
    const std::vector<wxMSWTaskBarThumbButtonNative>& buttons,
    std::size_t desiredCount,
    unsigned long long revision)
{
    if ( !state || buttons.size() != MAX_BUTTON_COUNT )
        return false;

    if ( state->thumbRevision != revision )
        return false;

    const bool add = !state->thumbButtonsInitialized;
    wxMSWTaskBarOperationLease lease;
    if ( !wxMSWTaskBarAcquireOperation(state, &lease) )
    {
        state->thumbButtonsAppliedKnown = false;
        return false;
    }

    const long result = add
        ? lease.backend->ThumbBarAddButtons(
              lease.hwnd, buttons.size(), buttons.data())
        : lease.backend->ThumbBarUpdateButtons(
              lease.hwnd, buttons.size(), buttons.data());
    const bool operationCurrent =
        wxMSWTaskBarOperationIsCurrent(state, lease);
    const bool revisionCurrent = operationCurrent &&
                                 state->thumbRevision == revision;
    const bool succeeded = wxMSWTaskBarResultSucceeded(result);

    // Initialization is a shell fact independent from the contents revision.
    // If A succeeds and synchronously queues B, B must subsequently use Update,
    // not attempt a second Add which the Shell contract forbids.
    if ( operationCurrent && add && succeeded )
        state->thumbButtonsInitialized = true;

    if ( revisionCurrent && succeeded )
    {
        state->thumbButtonsAppliedKnown = true;
        state->appliedThumbButtonCount = desiredCount;
    }
    else if ( revisionCurrent )
    {
        // A failed Add was never initialized and must be retried as Add. A
        // failed Update preserves initialization but makes applied contents
        // unknown until the next reconciliation.
        if ( add )
            state->thumbButtonsInitialized = false;
        state->thumbButtonsAppliedKnown = false;
    }

    if ( !wxMSWTaskBarResultSucceeded(result) )
    {
        wxLogApiError(add ? wxT("ITaskbarList3::ThumbBarAddButtons")
                          : wxT("ITaskbarList3::ThumbBarUpdateButtons"),
                      result);
    }
    return revisionCurrent && succeeded;
}

bool wxMSWTaskBarRequestThumbApply(
    const std::shared_ptr<wxTaskBarButtonStateData>& state,
    const std::vector<wxMSWTaskBarThumbButtonNative>* initialButtons = nullptr,
    std::size_t initialCount = 0,
    unsigned long long initialRevision = 0)
{
    if ( !state || state->retired )
        return false;

    if ( state->thumbApplyInProgress )
    {
        state->thumbApplyPending = true;
        return false;
    }

    state->thumbApplyInProgress = true;
    bool applied = false;
    unsigned passes = 0;
    do
    {
        state->thumbApplyPending = false;
        const bool useInitial = passes == 0 && initialButtons;
        const unsigned long long revision = useInitial
            ? initialRevision
            : state->thumbRevision;
        const std::vector<wxMSWTaskBarThumbButtonNative> buttons = useInitial
            ? *initialButtons
            : state->desiredThumbButtons;
        const std::size_t desiredCount = useInitial
            ? initialCount
            : state->desiredThumbButtonCount;

        applied = wxMSWTaskBarApplyThumbButtonsOnce(
            state, buttons, desiredCount, revision);

        if ( !state->retired && state->thumbRevision != revision )
            state->thumbApplyPending = true;
        ++passes;
    }
    while ( !state->retired && state->thumbApplyPending && passes < 32 );

    state->thumbApplyInProgress = false;
    if ( state->thumbApplyPending )
    {
        // A deliberately hostile seam can keep replacing the desired toolbar
        // forever. Bound this synchronous convergence loop; the next mutation
        // or TaskbarCreated notification retries the latest snapshot.
        state->thumbButtonsAppliedKnown = false;
    }

    return applied && state->thumbButtonsAppliedKnown &&
           state->appliedThumbButtonCount ==
               state->desiredThumbButtonCount;
}

bool wxMSWTaskBarStateIsValid(wxTaskBarButtonState state)
{
    switch ( state )
    {
        case wxTASKBAR_BUTTON_NO_PROGRESS:
        case wxTASKBAR_BUTTON_INDETERMINATE:
        case wxTASKBAR_BUTTON_NORMAL:
        case wxTASKBAR_BUTTON_ERROR:
        case wxTASKBAR_BUTTON_PAUSED:
            return true;
    }

    return false;
}

void wxMSWTaskBarInvalidateAppliedState(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    state->progressValueAppliedKnown = false;
    state->progressStateAppliedKnown = false;
    state->tabVisibilityAppliedKnown = false;
    state->tooltipAppliedKnown = false;
    state->overlayAppliedKnown = false;
    state->clipAppliedKnown = false;
    state->thumbButtonsAppliedKnown = false;
    state->thumbButtonsInitialized = false;
}

void wxMSWTaskBarRealizeState(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !wxMSWTaskBarHasExactIdentity(state) )
        return;

    // Capture the thumb generation before the first native seam. The desired
    // snapshot itself lives in the sidecar and retains all icon owners. If an
    // earlier replay call causes mutation B, the thumb reconciler observes the
    // newer revision and never publishes stale snapshot A.
    const unsigned long long thumbRevisionBeforeReplay =
        state->thumbRevision;
    const std::vector<wxMSWTaskBarThumbButtonNative>
        thumbButtonsBeforeReplay = state->desiredThumbButtons;
    const std::size_t thumbCountBeforeReplay =
        state->desiredThumbButtonCount;

    if ( ++state->shellEpoch == 0 )
        ++state->shellEpoch;
    wxMSWTaskBarInvalidateAppliedState(state);

    // Explorer restart invalidates all applied facts. Reconcile the complete
    // desired state; each helper is sidecar-only and remains safe if an earlier
    // native call destroys the owning frame.
    // SetProgressValue itself forces NOPROGRESS/INDETERMINATE to NORMAL. Apply
    // the value first and the explicit desired state last so Pulse(), reset,
    // ERROR and PAUSED all survive an Explorer restart exactly.
    if ( state->desiredProgressValueKnown &&
         state->desiredProgressRange > 0 )
    {
        wxMSWTaskBarApplyProgressValue(state);
    }
    wxMSWTaskBarApplyProgressState(state);
    wxMSWTaskBarApplyTabVisibility(state);
    wxMSWTaskBarApplyTooltip(state);
    wxMSWTaskBarApplyOverlay(state);
    wxMSWTaskBarApplyClip(state);
    wxMSWTaskBarRequestThumbApply(
        state,
        &thumbButtonsBeforeReplay,
        thumbCountBeforeReplay,
        thumbRevisionBeforeReplay);
}

bool wxMSWTaskBarRequestRebind(
    const std::shared_ptr<wxTaskBarButtonStateData>& state)
{
    if ( !state || state->retired || !state->backend )
        return false;

    state->rebindPending = true;
    if ( state->rebindInProgress )
        return wxMSWTaskBarHasExactIdentity(state);

    state->rebindInProgress = true;
    unsigned passes = 0;
    do
    {
        state->rebindPending = false;
        wxWindow* const parent = state->parent.get();
        if ( parent != state->parentIdentity ||
             wxWindowIsUnavailableForCallbacks(parent) )
        {
            wxMSWTaskBarInvalidateAppliedState(state);
            break;
        }

        const WXHWND hwnd = parent->GetHWND();
        const unsigned long long hwndGeneration =
            wxMSWTaskBarGetOrCreateHwndGeneration(parent, hwnd);
        if ( !hwnd || !hwndGeneration )
        {
            state->hwnd = nullptr;
            state->hwndGeneration = 0;
            if ( ++state->identityRevision == 0 )
                ++state->identityRevision;
            wxMSWTaskBarInvalidateAppliedState(state);
            break;
        }

        if ( state->hwnd != hwnd ||
             state->hwndGeneration != hwndGeneration ||
             !wxMSWTaskBarHasExactIdentity(state) )
        {
            // Publish the new exact native identity before crossing the first
            // shell seam. Every stale operation lease then fails its identity
            // and epoch checks, while the same public wrapper retains all
            // desired state and thumb-button ownership.
            state->hwnd = hwnd;
            state->hwndGeneration = hwndGeneration;
            if ( ++state->identityRevision == 0 )
                ++state->identityRevision;
            wxMSWTaskBarRealizeState(state);
        }

        if ( !state->retired && !wxMSWTaskBarHasExactIdentity(state) )
            state->rebindPending = true;
        ++passes;
    }
    while ( !state->retired && state->rebindPending && passes < 32 );

    state->rebindInProgress = false;
    if ( state->rebindPending )
    {
        // A hostile native callback can invalidate every freshly published
        // generation. Bound synchronous convergence; the next accessor or
        // TaskbarButtonCreated notification retries the preserved desired
        // state on the then-current native identity.
        wxMSWTaskBarInvalidateAppliedState(state);
    }

    return wxMSWTaskBarHasExactIdentity(state);
}

} // anonymous namespace

void wxMSWTaskBarButtonSetNativeFactoryForTesting(
    wxMSWTaskBarButtonNativeFactoryForTesting factory)
{
    gs_taskBarButtonNativeFactoryForTesting = factory;
}

/* static */
wxTaskBarButton* wxTaskBarButton::New(wxWindow* parent)
{
    if ( !parent || wxWindowIsUnavailableForCallbacks(parent) )
        return nullptr;

    const wxWeakRef<wxWindow> parentLifetime(parent);
    const WXHWND hwnd = parent->GetHWND();
    const unsigned long long hwndGeneration =
        wxMSWTaskBarGetOrCreateHwndGeneration(parent, hwnd);
    if ( !hwnd || !hwndGeneration )
        return nullptr;

    const std::shared_ptr<wxMSWTaskBarButtonNativeBackend> backend =
        wxMSWCreateTaskBarButtonNativeBackend();
    if ( !backend || !wxMSWTaskBarResultSucceeded(backend->Initialize()) )
        return nullptr;

    wxWindow* const liveParent = parentLifetime.get();
    const HWND nativeHwnd = static_cast<HWND>(hwnd);
    if ( liveParent != parent ||
         wxWindowIsUnavailableForCallbacks(liveParent) ||
         GetHwndOf(liveParent) != nativeHwnd || !::IsWindow(nativeHwnd) ||
         wxFindWinFromHandle(nativeHwnd) != liveParent ||
         reinterpret_cast<ULONG_PTR>(
             ::GetPropW(nativeHwnd,
                        wxTASKBAR_BUTTON_GENERATION_PROPERTY)) !=
             hwndGeneration )
    {
        return nullptr;
    }

    wxTaskBarButtonImpl* const impl = new wxTaskBarButtonImpl(
        backend, liveParent, hwnd, hwndGeneration);
    if ( !impl->IsAvailable() )
    {
        delete impl;
        return nullptr;
    }
    return impl;
}

wxTaskBarButtonImpl::wxTaskBarButtonImpl(
    const std::shared_ptr<wxMSWTaskBarButtonNativeBackend>& backend,
    wxWindow* parent,
    WXHWND hwnd,
    unsigned long long hwndGeneration)
    : m_state(std::make_shared<wxTaskBarButtonStateData>())
{
    m_state->parent = parent;
    m_state->parentIdentity = parent;
    m_state->hwnd = hwnd;
    m_state->hwndGeneration = hwndGeneration;
    m_state->backend = backend;
    m_state->desiredThumbButtons = BuildNativeThumbButtons();

    m_state->parentObserver.reset(
        new wxMSWTaskBarParentObserver(m_state));
    parent->Bind(wxEVT_DESTROY,
                 &wxMSWTaskBarParentObserver::OnDestroy,
                 m_state->parentObserver.get());
    m_state->parentObserverBound = true;
    if ( !wxMSWTaskBarHasExactIdentity(m_state) )
        wxMSWTaskBarRetireState(m_state);
}

wxTaskBarButtonImpl::~wxTaskBarButtonImpl()
{
    wxMSWTaskBarDisconnectParentObserver(m_state);
    wxMSWTaskBarRetireState(m_state);

    for ( wxThumbBarButton* const button : m_thumbBarButtons )
    {
        if ( button )
        {
            button->SetParent(nullptr);
            delete button;
        }
    }
    m_thumbBarButtons.clear();
}

bool wxTaskBarButtonImpl::IsAvailable() const
{
    return wxMSWTaskBarHasExactIdentity(m_state);
}

bool wxTaskBarButtonImpl::Rebind()
{
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    return wxMSWTaskBarRequestRebind(state);
}

bool wxTaskBarButtonImpl::GetExactIdentity(
    WXHWND* hwnd,
    unsigned long long* hwndGeneration) const
{
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    if ( !hwnd || !hwndGeneration ||
         !wxMSWTaskBarHasExactIdentity(state) )
    {
        return false;
    }

    *hwnd = state->hwnd;
    *hwndGeneration = state->hwndGeneration;
    return true;
}

void wxTaskBarButtonImpl::Realize()
{
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    wxMSWTaskBarRealizeState(state);
}

void wxTaskBarButtonImpl::SetProgressRange(int range)
{
    wxCHECK_RET( range >= 0, "taskbar progress range must be non-negative" );
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    if ( !state || state->retired )
        return;

    state->desiredProgressRange = range;
    if ( range > 0 && state->desiredProgressValueKnown &&
         state->desiredProgressValue > range )
    {
        state->desiredProgressValue = range;
    }
    ++state->progressValueRevision;
    state->progressValueAppliedKnown = false;

    if ( range == 0 )
    {
        state->desiredProgressState = wxTASKBAR_BUTTON_NO_PROGRESS;
        ++state->progressStateRevision;
        state->progressStateAppliedKnown = false;
        wxMSWTaskBarApplyProgressState(state);
        return;
    }

    // Merely configuring the scale must not make a progress indicator visible.
    // The public contract requires a subsequent SetProgressValue() call.
}

void wxTaskBarButtonImpl::SetProgressValue(int value)
{
    wxCHECK_RET( value >= 0, "taskbar progress value must be non-negative" );
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    if ( !state || state->retired )
        return;
    wxCHECK_RET( state->desiredProgressRange > 0 &&
                     value <= state->desiredProgressRange,
                 "taskbar progress value requires a positive range and must "
                 "not exceed it" );

    state->desiredProgressValue = value;
    state->desiredProgressValueKnown = true;
    const wxTaskBarButtonState effectiveState =
        wxMSWTaskBarDesiredStateAfterProgressValue(
            state->desiredProgressState, value,
            state->desiredProgressRange);
    if ( effectiveState != state->desiredProgressState )
    {
        state->desiredProgressState = effectiveState;
        ++state->progressStateRevision;
    }
    ++state->progressValueRevision;
    state->progressValueAppliedKnown = false;
    const bool valueApplied = wxMSWTaskBarApplyProgressValue(state);

    // A successful value write has an ambiguous final state only when the
    // previous applied state was unknown: ERROR/PAUSED are preserved by the
    // shell. Resolve that ambiguity without retrying a failed value write.
    if ( valueApplied &&
         (!state->progressStateAppliedKnown ||
          state->appliedProgressState != state->desiredProgressState) )
    {
        wxMSWTaskBarApplyProgressState(state);
    }
}

void wxTaskBarButtonImpl::PulseProgress()
{
    SetProgressState(wxTASKBAR_BUTTON_INDETERMINATE);
}

void wxTaskBarButtonImpl::Show(bool show)
{
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    if ( !state || state->retired )
        return;
    state->tabVisibilityDesiredKnown = true;
    state->desiredTabVisible = show;
    ++state->tabVisibilityRevision;
    state->tabVisibilityAppliedKnown = false;
    wxMSWTaskBarApplyTabVisibility(state);
}

void wxTaskBarButtonImpl::Hide()
{
    Show(false);
}

void wxTaskBarButtonImpl::SetThumbnailTooltip(const wxString& tooltip)
{
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    if ( !state || state->retired )
        return;
    state->desiredThumbnailTooltip = tooltip;
    ++state->tooltipRevision;
    state->tooltipAppliedKnown = false;
    wxMSWTaskBarApplyTooltip(state);
}

void wxTaskBarButtonImpl::SetProgressState(wxTaskBarButtonState state)
{
    wxCHECK_RET( wxMSWTaskBarStateIsValid(state),
                 "invalid taskbar progress state" );
    const std::shared_ptr<wxTaskBarButtonStateData> data = m_state;
    if ( !data || data->retired )
        return;
    data->desiredProgressState = state;
    ++data->progressStateRevision;
    data->progressStateAppliedKnown = false;
    wxMSWTaskBarApplyProgressState(data);
}

void wxTaskBarButtonImpl::SetOverlayIcon(const wxIcon& icon,
                                         const wxString& description)
{
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    if ( !state || state->retired )
        return;
    state->desiredOverlayIcon = icon;
    state->desiredOverlayDescription = description;
    ++state->overlayRevision;
    state->overlayAppliedKnown = false;
    wxMSWTaskBarApplyOverlay(state);
}

void wxTaskBarButtonImpl::SetThumbnailClip(const wxRect& rect)
{
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    if ( !state || state->retired )
        return;
    state->desiredThumbnailClip = rect;
    ++state->clipRevision;
    state->clipAppliedKnown = false;
    wxMSWTaskBarApplyClip(state);
}

void wxTaskBarButtonImpl::SetThumbnailContents(const wxWindow *child)
{
    wxCHECK_RET( child, "null thumbnail content window" );
    SetThumbnailClip(child->GetRect());
}

bool wxTaskBarButtonImpl::AppendThumbBarButton(wxThumbBarButton *button)
{
    wxCHECK_MSG( button, false, "Cannot append a null ThumbBarButton" );
    wxCHECK_MSG( m_thumbBarButtons.size() < MAX_BUTTON_COUNT, false,
                 "Number of ThumbBarButtons and separators is limited to 7" );
    wxCHECK_MSG( !button->GetParent(), false,
                 "ThumbBarButton already belongs to a taskbar button" );

    button->SetParent(this);
    m_thumbBarButtons.push_back(button);
    return InitOrUpdateThumbBarButtons();
}

bool wxTaskBarButtonImpl::AppendSeparatorInThumbBar()
{
    wxCHECK_MSG( m_thumbBarButtons.size() < MAX_BUTTON_COUNT, false,
                 "Number of ThumbBarButtons and separators is limited to 7" );

    // Append a disable ThumbBarButton without background can simulate the
    // behavior of appending a separator.
    wxThumbBarButton *separator = new wxThumbBarButton(wxID_ANY,
                                                       wxNullIcon,
                                                       wxEmptyString,
                                                       false,
                                                       false,
                                                       false);
    separator->SetParent(this);
    m_thumbBarButtons.push_back(separator);
    return InitOrUpdateThumbBarButtons();
}

bool wxTaskBarButtonImpl::InsertThumbBarButton(size_t pos,
                                               wxThumbBarButton *button)
{
    wxCHECK_MSG( button, false, "Cannot insert a null ThumbBarButton" );
    wxCHECK_MSG( m_thumbBarButtons.size() < MAX_BUTTON_COUNT, false,
                 "Number of ThumbBarButtons and separators is limited to 7" );
    wxCHECK_MSG( pos <= m_thumbBarButtons.size(), false,
                 "Invalid index when inserting the button" );
    wxCHECK_MSG( !button->GetParent(), false,
                 "ThumbBarButton already belongs to a taskbar button" );

    button->SetParent(this);
    m_thumbBarButtons.insert(m_thumbBarButtons.begin() + pos, button);
    return InitOrUpdateThumbBarButtons();
}

wxThumbBarButton* wxTaskBarButtonImpl::RemoveThumbBarButton(
    wxThumbBarButton *button)
{
    if ( !button )
        return nullptr;

    for ( wxThumbBarButtons::iterator iter = m_thumbBarButtons.begin();
          iter != m_thumbBarButtons.end(); ++iter )
    {
        if ( *iter != button )
            continue;

        m_thumbBarButtons.erase(iter);
        button->SetParent(nullptr);
        InitOrUpdateThumbBarButtons();
        return button;
    }
    return nullptr;
}

wxThumbBarButton* wxTaskBarButtonImpl::RemoveThumbBarButton(int id)
{
    for ( wxThumbBarButtons::iterator iter = m_thumbBarButtons.begin();
          iter != m_thumbBarButtons.end();
          ++iter )
    {
        wxThumbBarButton* const button = *iter;
        if ( id == button->GetID() )
        {
            m_thumbBarButtons.erase(iter);
            button->SetParent(nullptr);
            InitOrUpdateThumbBarButtons();
            return button;
        }
    }

    return nullptr;
}

bool wxTaskBarButtonImpl::InitOrUpdateThumbBarButtons()
{
    const std::vector<wxMSWTaskBarThumbButtonNative> buttons =
        BuildNativeThumbButtons();
    const std::size_t desiredCount = m_thumbBarButtons.size();
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    if ( !state || state->retired )
        return false;
    ++state->thumbRevision;
    state->desiredThumbButtons = buttons;
    state->desiredThumbButtonCount = desiredCount;
    state->thumbButtonsAppliedKnown = false;
    return wxMSWTaskBarRequestThumbApply(state);
}

wxThumbBarButton* wxTaskBarButtonImpl::GetThumbBarButtonByIndex(size_t index)
{
    if ( !IsAvailable() || index >= MAX_BUTTON_COUNT )
        return nullptr;

    return index < m_thumbBarButtons.size()
        ? m_thumbBarButtons[index]
        : nullptr;
}

std::vector<wxMSWTaskBarThumbButtonNative>
wxTaskBarButtonImpl::BuildNativeThumbButtons() const
{
    std::vector<wxMSWTaskBarThumbButtonNative> buttons;
    buttons.reserve(MAX_BUTTON_COUNT);
    for ( unsigned slot = 0; slot < MAX_BUTTON_COUNT; ++slot )
    {
        wxMSWTaskBarThumbButtonNative native;
        native.token = slot;
        if ( slot < m_thumbBarButtons.size() )
        {
            wxThumbBarButton* const button = m_thumbBarButtons[slot];
            native.occupied = true;
            native.retainedIcon = button->GetIcon();
            native.icon = native.retainedIcon.IsOk()
                ? static_cast<void*>(GetHiconOf(native.retainedIcon))
                : nullptr;
            native.tooltip = button->GetTooltip();
            // THUMBBUTTON::szTip includes its trailing NUL.
            native.tooltip.Truncate(259);
            native.flags = static_cast<unsigned>(
                GetNativeThumbButtonFlags(*button));
        }
        // Empty slots deliberately retain empty icon/tooltip values. The COM
        // backend includes both masks to clear content from a prior occupant.
        buttons.push_back(std::move(native));
    }
    return buttons;
}

bool wxTaskBarButtonImpl::GetSnapshotForTesting(
    wxMSWTaskBarButtonSnapshot* snapshot) const
{
    if ( !snapshot || !m_state )
        return false;

    *snapshot = wxMSWTaskBarButtonSnapshot();
    const std::shared_ptr<wxTaskBarButtonStateData> state = m_state;
    snapshot->available = wxMSWTaskBarHasExactIdentity(state);
    snapshot->retired = state->retired;
    snapshot->hwnd = state->hwnd;
    snapshot->hwndGeneration = state->hwndGeneration;
    snapshot->shellEpoch = state->shellEpoch;
    snapshot->desiredProgressRange = state->desiredProgressRange;
    snapshot->desiredProgressValue = state->desiredProgressValue;
    snapshot->desiredProgressValueKnown =
        state->desiredProgressValueKnown;
    snapshot->desiredProgressState = state->desiredProgressState;
    snapshot->progressValueAppliedKnown =
        state->progressValueAppliedKnown;
    snapshot->appliedProgressRange = state->appliedProgressRange;
    snapshot->appliedProgressValue = state->appliedProgressValue;
    snapshot->progressStateAppliedKnown =
        state->progressStateAppliedKnown;
    snapshot->appliedProgressState = state->appliedProgressState;
    snapshot->thumbButtonsAppliedKnown =
        state->thumbButtonsAppliedKnown;
    snapshot->thumbButtonsInitialized =
        state->thumbButtonsInitialized;
    snapshot->desiredThumbButtonCount =
        state->desiredThumbButtonCount;
    snapshot->appliedThumbButtonCount =
        state->appliedThumbButtonCount;
    return true;
}

bool wxMSWTaskBarButtonGetSnapshotForTesting(
    wxTaskBarButton* button,
    wxMSWTaskBarButtonSnapshot* snapshot)
{
    // All instances returned by the only public factory are this MSW
    // implementation. Avoid C++ RTTI: wxWidgets is commonly built with /GR-.
    wxTaskBarButtonImpl* const impl =
        static_cast<wxTaskBarButtonImpl*>(button);
    return impl && impl->GetSnapshotForTesting(snapshot);
}

// ----------------------------------------------------------------------------
// wxTaskBarJumpListItem Implementation.
// ----------------------------------------------------------------------------
wxTaskBarJumpListItem::wxTaskBarJumpListItem(wxTaskBarJumpListCategory *parent,
                                             wxTaskBarJumpListItemType type,
                                             const wxString& title,
                                             const wxString& filePath,
                                             const wxString& arguments,
                                             const wxString& tooltip,
                                             const wxString& iconPath,
                                             int iconIndex)
    : m_parentCategory(parent),
      m_type(type),
      m_title(title),
      m_filePath(filePath),
      m_arguments(arguments),
      m_tooltip(tooltip),
      m_iconPath(iconPath),
      m_iconIndex(iconIndex)
{
}

wxTaskBarJumpListItemType wxTaskBarJumpListItem::GetType() const
{
    return m_type;
}

void wxTaskBarJumpListItem::SetType(wxTaskBarJumpListItemType type)
{
    m_type = type;
    if ( m_parentCategory )
        m_parentCategory->Update();
}

const wxString& wxTaskBarJumpListItem::GetTitle() const
{
    return m_title;
}

void wxTaskBarJumpListItem::SetTitle(const wxString& title)
{
    m_title = title;
    if ( m_parentCategory )
        m_parentCategory->Update();
}

const wxString& wxTaskBarJumpListItem::GetFilePath() const
{
    return m_filePath;
}

void wxTaskBarJumpListItem::SetFilePath(const wxString& filePath)
{
    m_filePath = filePath;
    if ( m_parentCategory )
        m_parentCategory->Update();
}

const wxString& wxTaskBarJumpListItem::GetArguments() const
{
    return m_arguments;
}

void wxTaskBarJumpListItem::SetArguments(const wxString& arguments)
{
    m_arguments = arguments;
    if ( m_parentCategory )
        m_parentCategory->Update();
}

const wxString& wxTaskBarJumpListItem::GetTooltip() const
{
    return m_tooltip;
}

void wxTaskBarJumpListItem::SetTooltip(const wxString& tooltip)
{
    m_tooltip = tooltip;
    if ( m_parentCategory )
        m_parentCategory->Update();
}

const wxString& wxTaskBarJumpListItem::GetIconPath() const
{
    return m_iconPath;
}

void wxTaskBarJumpListItem::SetIconPath(const wxString& iconPath)
{
    m_iconPath = iconPath;
    if ( m_parentCategory )
        m_parentCategory->Update();
}

int wxTaskBarJumpListItem::GetIconIndex() const
{
    return m_iconIndex;
}

void wxTaskBarJumpListItem::SetIconIndex(int iconIndex)
{
    m_iconIndex = iconIndex;
    if ( m_parentCategory )
        m_parentCategory->Update();
}

wxTaskBarJumpListCategory* wxTaskBarJumpListItem::GetCategory() const
{
    return m_parentCategory;
}

void wxTaskBarJumpListItem::SetCategory(wxTaskBarJumpListCategory *category)
{
    m_parentCategory = category;
}

// ----------------------------------------------------------------------------
// wxTaskBarJumpListCategory Implementation.
// ----------------------------------------------------------------------------
wxTaskBarJumpListCategory::wxTaskBarJumpListCategory(wxTaskBarJumpList *parent,
                                                     const wxString& title)
    : m_parent(parent),
      m_title(title)
{
}

wxTaskBarJumpListCategory::~wxTaskBarJumpListCategory()
{
    for ( wxTaskBarJumpListItems::iterator it = m_items.begin();
          it != m_items.end();
          ++it )
    {
        delete *it;
    }
}

wxTaskBarJumpListItem*
wxTaskBarJumpListCategory::Append(wxTaskBarJumpListItem *item)
{
    m_items.push_back(item);
    item->SetCategory(this);
    Update();

    return item;
}

void wxTaskBarJumpListCategory::Delete(wxTaskBarJumpListItem *item)
{
    item = Remove(item);
    item->SetCategory(nullptr);
    Update();

    if ( item )
        delete item;
}

wxTaskBarJumpListItem*
wxTaskBarJumpListCategory::Remove(wxTaskBarJumpListItem *item)
{
    for (wxTaskBarJumpListItems::iterator it = m_items.begin();
         it != m_items.end();
         ++it)
    {
        if ( *it == item )
        {
            m_items.erase(it);
            item->SetCategory(nullptr);
            Update();
            return item;
        }
    }

    return nullptr;
}

wxTaskBarJumpListItem*
wxTaskBarJumpListCategory::FindItemByPosition(size_t pos) const
{
    wxASSERT_MSG( pos < m_items.size(), "invalid pos." );
    return m_items[pos];
}

wxTaskBarJumpListItem*
wxTaskBarJumpListCategory::Insert(size_t pos, wxTaskBarJumpListItem *item)
{
    wxASSERT_MSG( pos <= m_items.size(), "invalid pos." );
    m_items.insert(m_items.begin() + pos, item);
    item->SetCategory(this);
    Update();

    return item;
}

wxTaskBarJumpListItem*
wxTaskBarJumpListCategory::Prepend(wxTaskBarJumpListItem *item)
{
    return Insert(0, item);
}

void wxTaskBarJumpListCategory::SetTitle(const wxString& title)
{
    m_title = title;
    Update();
}

const wxString& wxTaskBarJumpListCategory::GetTitle() const
{
    return m_title;
}

const wxTaskBarJumpListItems& wxTaskBarJumpListCategory::GetItems() const
{
    return m_items;
}

void wxTaskBarJumpListCategory::Update()
{
    if ( m_parent )
        m_parent->Update();
}

// ----------------------------------------------------------------------------
// wxTaskBarJumpList Implementation.
// ----------------------------------------------------------------------------
wxTaskBarJumpList::wxTaskBarJumpList(const wxString& appID)
    : m_jumpListImpl(new wxTaskBarJumpListImpl(this, appID))
{
}

wxTaskBarJumpList::~wxTaskBarJumpList()
{
    delete m_jumpListImpl;
}

wxTaskBarJumpListCategory& wxTaskBarJumpList::GetTasks() const
{
    return m_jumpListImpl->GetTasks();
}

void wxTaskBarJumpList::ShowRecentCategory(bool shown)
{
    m_jumpListImpl->ShowRecentCategory(shown);
}

void wxTaskBarJumpList::HideRecentCategory()
{
    m_jumpListImpl->HideRecentCategory();
}

void wxTaskBarJumpList::ShowFrequentCategory(bool shown)
{
    m_jumpListImpl->ShowFrequentCategory(shown);
}

void wxTaskBarJumpList::HideFrequentCategory()
{
    m_jumpListImpl->HideFrequentCategory();
}

const wxTaskBarJumpListCategory& wxTaskBarJumpList::GetFrequentCategory() const
{
    return m_jumpListImpl->GetFrequentCategory();
}

const wxTaskBarJumpListCategory& wxTaskBarJumpList::GetRecentCategory() const
{
    return m_jumpListImpl->GetRecentCategory();
}

const wxTaskBarJumpListCategories&
wxTaskBarJumpList::GetCustomCategories() const
{
    return m_jumpListImpl->GetCustomCategories();
}

void wxTaskBarJumpList::AddCustomCategory(wxTaskBarJumpListCategory* category)
{
    m_jumpListImpl->AddCustomCategory(category);
}

wxTaskBarJumpListCategory* wxTaskBarJumpList::RemoveCustomCategory(
    const wxString& title)
{
    return m_jumpListImpl->RemoveCustomCategory(title);
}

void wxTaskBarJumpList::DeleteCustomCategory(const wxString& title)
{
    m_jumpListImpl->DeleteCustomCategory(title);
}

void wxTaskBarJumpList::Update()
{
    m_jumpListImpl->Update();
}

// ----------------------------------------------------------------------------
// wxTaskBarJumpListImpl Implementation.
// ----------------------------------------------------------------------------
wxTaskBarJumpListImpl::wxTaskBarJumpListImpl(wxTaskBarJumpList *jumpList,
                                             const wxString& appID)
    : m_jumpList(jumpList),
      m_destinationList(nullptr)
{
    m_appID = appID;
    HRESULT hr = CoCreateInstance
                 (
                    wxCLSID_DestinationList,
                    nullptr,
                    CLSCTX_INPROC_SERVER,
                    wxIID_ICustomDestinationList,
                    reinterpret_cast<void**> (&(m_destinationList))
                );
    if ( FAILED(hr) )
    {
        wxLogApiError(wxT("CoCreateInstance(wxCLSID_DestinationList)"), hr);
        return;
    }
}

wxTaskBarJumpListImpl::~wxTaskBarJumpListImpl()
{
    for ( wxTaskBarJumpListCategories::iterator it = m_customCategories.begin();
          it != m_customCategories.end();
          ++it )
    {
        delete *it;
    }
}

void wxTaskBarJumpListImpl::Update()
{
    if ( !BeginUpdate() )
        return;

    AddTasksToDestinationList();
    AddCustomCategoriesToDestinationList();
    if ( m_recent_visible )
        m_destinationList->AppendKnownCategory(KDC_RECENT);
    if ( m_frequent_visible )
        m_destinationList->AppendKnownCategory(KDC_FREQUENT);
    CommitUpdate();
}

wxTaskBarJumpListCategory& wxTaskBarJumpListImpl::GetTasks()
{
    if ( m_tasks.get() == nullptr )
        m_tasks.reset(new wxTaskBarJumpListCategory(m_jumpList, wxT("Tasks")));

    return *(m_tasks.get());
}

void wxTaskBarJumpListImpl::ShowRecentCategory(bool shown)
{
    m_recent_visible = shown;
}

void wxTaskBarJumpListImpl::HideRecentCategory()
{
    ShowRecentCategory(false);
}

void wxTaskBarJumpListImpl::ShowFrequentCategory(bool shown)
{
    m_frequent_visible = shown;
}

void wxTaskBarJumpListImpl::HideFrequentCategory()
{
    ShowFrequentCategory(false);
}

const wxTaskBarJumpListCategory& wxTaskBarJumpListImpl::GetFrequentCategory()
{
    wxString title = wxT("Frequent");
    if ( m_frequent.get() == nullptr )
        m_frequent.reset(new wxTaskBarJumpListCategory(m_jumpList, title));
    LoadKnownCategory(title);

    return *m_frequent.get();
}

const wxTaskBarJumpListCategory& wxTaskBarJumpListImpl::GetRecentCategory()
{
    wxString title = wxT("Recent");
    if ( m_recent.get() == nullptr )
        m_recent.reset(new wxTaskBarJumpListCategory(m_jumpList, title));
    LoadKnownCategory(title);

    return *m_recent.get();
}

const wxTaskBarJumpListCategories&
wxTaskBarJumpListImpl::GetCustomCategories() const
{
    return m_customCategories;
}

void
wxTaskBarJumpListImpl::AddCustomCategory(wxTaskBarJumpListCategory *category)
{
    wxASSERT_MSG( category != nullptr, "Invalid category." );
    m_customCategories.push_back(category);
}

wxTaskBarJumpListCategory*
wxTaskBarJumpListImpl::RemoveCustomCategory(const wxString& title)
{
    for ( wxTaskBarJumpListCategories::iterator it = m_customCategories.begin();
          it != m_customCategories.end();
          ++it )
    {
        wxTaskBarJumpListCategory* tbJlCat = *it;
        if ( tbJlCat->GetTitle() == title )
        {
            m_customCategories.erase(it);
            return tbJlCat;
        }
    }

    return nullptr;
}

void wxTaskBarJumpListImpl::DeleteCustomCategory(const wxString& title)
{
    wxTaskBarJumpListCategory* category = RemoveCustomCategory(title);
    if ( category )
        delete category;
}

bool wxTaskBarJumpListImpl::BeginUpdate()
{
    if ( m_destinationList == nullptr )
        return false;

    unsigned int max_count = 0;
    m_objectArray = nullptr;
    HRESULT hr = m_destinationList->BeginList(&max_count,
        wxIID_IObjectArray, reinterpret_cast<void**>(&m_objectArray));
    if ( !m_appID.empty() )
        m_destinationList->SetAppID(m_appID.wc_str());

    return SUCCEEDED(hr);
}

bool wxTaskBarJumpListImpl::CommitUpdate()
{
    return SUCCEEDED(m_destinationList->CommitList());
}

void wxTaskBarJumpListImpl::AddTasksToDestinationList()
{
    if ( !m_tasks.get() )
        return;

    wxCOMPtr<IObjectCollection> collection(CreateObjectCollection());
    if ( !collection )
        return;

    const wxTaskBarJumpListItems& tasks = m_tasks->GetItems();
    for ( wxTaskBarJumpListItems::const_iterator it = tasks.begin();
          it != tasks.end();
          ++it )
    {
        wxASSERT_MSG( ((*it)->GetType() == wxTASKBAR_JUMP_LIST_TASK ||
                       (*it)->GetType() == wxTASKBAR_JUMP_LIST_SEPARATOR),
                      "Invalid task Item." );
        AddShellLink(collection, *(*it));
    }
    m_destinationList->AddUserTasks(collection);
}

void wxTaskBarJumpListImpl::AddCustomCategoriesToDestinationList()
{
    for ( wxTaskBarJumpListCategories::iterator it = m_customCategories.begin();
          it != m_customCategories.end();
          ++it )
    {
        wxCOMPtr<IObjectCollection> collection(CreateObjectCollection());
        if ( !collection )
            continue;

        const wxTaskBarJumpListItems& tasks = (*it)->GetItems();
        for ( wxTaskBarJumpListItems::const_iterator iter = tasks.begin();
              iter != tasks.end();
              ++iter )
        {
            wxASSERT_MSG(
                (*iter)->GetType() == wxTASKBAR_JUMP_LIST_DESTINATION,
                "Invalid category item." );
            AddShellLink(collection, *(*iter));
        }
        m_destinationList->AppendCategory((*it)->GetTitle().wc_str(),
                                          collection);
    }
}

void wxTaskBarJumpListImpl::LoadKnownCategory(const wxString& title)
{
    wxCOMPtr<IApplicationDocumentLists> docList;
    HRESULT hr = CoCreateInstance
                 (
                    wxCLSID_ApplicationDocumentLists,
                    nullptr,
                    CLSCTX_INPROC_SERVER,
                    wxIID_IApplicationDocumentLists,
                    reinterpret_cast<void **>(&docList)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError("CoCreateInstance(wxCLSID_ApplicationDocumentLists)", hr);
        return;
    }
    if ( !m_appID.empty() )
        docList->SetAppID(m_appID.wc_str());

    wxCOMPtr<IObjectArray> array;
    wxASSERT_MSG( title == "Recent" || title == "Frequent", "Invalid title." );
    hr = docList->GetList
                 (
                     title == "Recent" ? ADLT_RECENT : ADLT_FREQUENT,
                     0,
                     wxIID_IObjectArray,
                     reinterpret_cast<void **>(&array)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError("IApplicationDocumentLists::GetList", hr);
        return;
    }

    UINT count = 0;
    array->GetCount(&count);
    for (UINT i = 0; i < count; ++i)
    {
        wxCOMPtr<IUnknown> collectionItem;
        hr = array->GetAt(i, wxIID_IUnknown,
                          reinterpret_cast<void **>(&collectionItem));
        if ( FAILED(hr) )
        {
            wxLogApiError("IObjectArray::GetAt", hr);
            continue;
        }

        wxCOMPtr<IShellLink> shellLink;
        wxCOMPtr<IShellItem> shellItem;
        wxTaskBarJumpListItem* item = nullptr;

        if ( SUCCEEDED(collectionItem->QueryInterface(
                 wxIID_IShellLink, reinterpret_cast<void**>(&shellLink))) )
        {
            item = GetItemFromIShellLink(shellLink);
        }
        else if ( SUCCEEDED(collectionItem->QueryInterface(
                      wxIID_IShellItem, reinterpret_cast<void**>(&shellItem))) )
        {
            item = GetItemFromIShellItem(shellItem);
        }
        else
        {
            wxLogError("Can not query interfaces: IShellLink or IShellItem.");
        }

        if ( item )
        {
            if ( title == wxT("Frequent") )
                m_frequent->Append(item);
            else
                m_recent->Append(item);
        }
    }
}

#endif // wxUSE_TASKBARBUTTON
