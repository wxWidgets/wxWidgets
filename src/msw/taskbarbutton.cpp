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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/toplevel.h"
#endif

#if wxUSE_TASKBARBUTTON

#include "wx/msw/private.h"
#include "wx/msw/taskbarbutton.h"

#include <shlwapi.h>
#include <initguid.h>

WINOLEAPI PropVariantClear(PROPVARIANT* pvar);

#ifndef PropVariantInit
#define PropVariantInit(pvar) memset ( (pvar), 0, sizeof(PROPVARIANT) )
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

typedef struct _tagpropertykey
{
    GUID fmtid;
    DWORD pid;
} PROPERTYKEY;

#define REFPROPERTYKEY const PROPERTYKEY &

typedef struct tagPROPVARIANT PROPVARIANT;
#define REFPROPVARIANT const PROPVARIANT &

#define DEFINE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) \
    const PROPERTYKEY name  = \
    { { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }, pid }

DEFINE_PROPERTYKEY(PKEY_Title,
    0xf29f85e0, 0x4ff9, 0x1068, 0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9, 2);
DEFINE_PROPERTYKEY(PKEY_AppUserModel_IsDestListSeparator,
    0x9f4c2855, 0x9f79, 0x4b39, 0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3, 6);
DEFINE_PROPERTYKEY(PKEY_Link_Arguments,
    0x436f2667, 0x14e2, 0x4feb, 0xb3, 0x0a, 0x14, 0x6c, 0x53, 0xb5, 0xb6, 0x74, 100);

class ITaskbarList : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL HrInit() = 0;
    virtual HRESULT wxSTDCALL AddTab(HWND) = 0;
    virtual HRESULT wxSTDCALL DeleteTab(HWND) = 0;
    virtual HRESULT wxSTDCALL ActivateTab(HWND) = 0;
    virtual HRESULT wxSTDCALL SetActiveAlt(HWND) = 0;
};

class ITaskbarList2 : public ITaskbarList
{
public:
    virtual HRESULT wxSTDCALL MarkFullscreenWindow(HWND, BOOL) = 0;
};

class ITaskbarList3 : public ITaskbarList2
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
    virtual HRESULT wxSTDCALL ThumbBarSetImageList(HWND, HIMAGELIST) = 0;
    virtual HRESULT wxSTDCALL SetOverlayIcon(HWND, HICON, LPCWSTR) = 0;
    virtual HRESULT wxSTDCALL SetThumbnailTooltip(HWND, LPCWSTR pszTip) = 0;
    virtual HRESULT wxSTDCALL SetThumbnailClip(HWND, RECT *) = 0;
};

class IShellLinkA : public IUnknown
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

class IShellLinkW : public IUnknown
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

#ifdef wxUSE_UNICODE
#define IShellLink      ::IShellLinkW

DEFINE_GUID(wxIID_IShellLink,
    0x000214F9, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
#else
#define IShellLink      ::IShellLinkA

DEFINE_GUID(wxIID_IShellLink,
    0x000214EE, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
#endif  // wxUSE_UNICODE


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
    virtual HRESULT wxSTDCALL SetValue(REFPROPERTYKEY, REFPROPVARIANT) = 0;
    virtual HRESULT wxSTDCALL Commit() = 0;
};

typedef enum KNOWNDESTCATEGORY
{
    KDC_FREQUENT    = 1,
    KDC_RECENT  = ( KDC_FREQUENT + 1 )
} KNOWNDESTCATEGORY;

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

typedef enum APPDOCLISTTYPE
{
    ADLT_RECENT   = 0,
    ADLT_FREQUENT = ( ADLT_RECENT + 1 )
} APPDOCLISTTYPE;

class IApplicationDocumentLists : public IUnknown
{
public:
    virtual HRESULT wxSTDCALL SetAppID(LPCWSTR) = 0;
    virtual HRESULT wxSTDCALL GetList(APPDOCLISTTYPE, UINT, REFIID, void**) = 0;
};

inline HRESULT InitPropVariantFromBoolean(BOOL fVal, PROPVARIANT *ppropvar)
{
    ppropvar->vt = VT_BOOL;
    ppropvar->boolVal = fVal ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}

inline HRESULT InitPropVariantFromString(PCWSTR psz, PROPVARIANT *ppropvar)
{
    ppropvar->vt = VT_LPWSTR;
    HRESULT hr = SHStrDupW(psz, &ppropvar->pwszVal);
    if (FAILED(hr))
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

bool AddShellLink(IObjectCollection *collection, const wxJumpListItem& item)
{
    IShellLink* shellLink = NULL;
    IPropertyStore* propertyStore = NULL;

    HRESULT hr = CoCreateInstance
                 (
                     wxCLSID_ShellLink,
                     NULL,
                     CLSCTX_INPROC_SERVER,
                     wxIID_IShellLink,
                     reinterpret_cast<void**> (&(shellLink))
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError("CoCreateInstance(wxCLSID_ShellLink)", hr);
        return false;
    }

    if ( item.GetType() == wxJUMP_LIST_TASK ||
         item.GetType() == wxJUMP_LIST_DESTIONATION )
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
        shellLink->Release();
        return false;
    }

    PROPVARIANT pv;
    if ( item.GetType() == wxJUMP_LIST_TASK ||
         item.GetType() == wxJUMP_LIST_DESTIONATION )
    {
        hr = InitPropVariantFromString(item.GetTitle().wc_str(), &pv);
        if ( SUCCEEDED(hr) )
        {
            hr = propertyStore->SetValue(PKEY_Title, pv);
        }
    }
    else if ( item.GetType() == wxJUMP_LIST_SEPARATOR )
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
    propertyStore->Release();
    PropVariantClear(&pv);

    // Add this IShellLink object to the given collection.
    hr = collection->AddObject(shellLink);

    shellLink->Release();
    return SUCCEEDED(hr);
}

wxJumpListItem* GetItemFromIShellLink(IShellLink* link)
{
    if ( !link )
        return NULL;

    wxJumpListItem *item = new wxJumpListItem(wxJUMP_LIST_DESTIONATION);

    IPropertyStore *linkProps;
    HRESULT hr = link->QueryInterface
                 (
                     wxIID_IPropertyStore,
                     reinterpret_cast<void **>(&linkProps)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError("IShellLink::QueryInterface", hr);
        return NULL;
    }

    PROPVARIANT var;
    linkProps->GetValue(PKEY_Link_Arguments, &var);
    item->SetArguments(wxString(var.pwszVal));
    PropVariantClear(&var);
    linkProps->Release();

    const int bufferSize = 2048;
    wchar_t buffer[bufferSize];

    link->GetDescription(buffer, INFOTIPSIZE);
    item->SetTooltip(wxString(buffer));

    int dummyIndex;
    link->GetIconLocation(buffer, bufferSize - 1, &dummyIndex);
    item->SetIconPath(wxString(buffer));

    link->GetPath(buffer, bufferSize - 1, NULL, NULL);
    item->SetFilePath(wxString(buffer));

    return item;
}

wxJumpListItem* GetItemFromIShellItem(IShellItem *shellItem)
{
    if ( !shellItem )
        return NULL;

    wxJumpListItem *item = new wxJumpListItem(wxJUMP_LIST_DESTIONATION);

    wchar_t *name;
    shellItem->GetDisplayName(SIGDN_FILESYSPATH, &name);
    item->SetFilePath(wxString(name));
    CoTaskMemFree(name);
    return item;
}

IObjectCollection* CreateObjectCollection()
{
    IObjectCollection* collection;

    HRESULT hr;
    hr = CoCreateInstance
         (
             wxCLSID_EnumerableObjectCollection,
             NULL,
             CLSCTX_INPROC,
             wxIID_IObjectCollection,
             reinterpret_cast<void**>(&(collection))
         );
    if ( FAILED(hr) )
    {
        wxLogApiError("CoCreateInstance(wxCLSID_EnumerableObjectCollection)",
                      hr);
        return NULL;
    }

    return collection;
}

} // namespace

IMPLEMENT_DYNAMIC_CLASS(wxThumbBarButton, wxObject)

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
      m_taskBarButtonParent(NULL)
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

wxTaskBarButtonImpl::wxTaskBarButtonImpl(WXWidget parent)
    : m_hwnd(parent),
      m_taskbarList(NULL),
      m_progressRange(0),
      m_hasInitThumbnailToolbar(false)
{
    HRESULT hr = CoCreateInstance
                 (
                    wxCLSID_TaskbarList,
                    NULL,
                    CLSCTX_INPROC_SERVER,
                    wxIID_ITaskbarList3,
                    reinterpret_cast<void **>(&m_taskbarList)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError(wxT("CoCreateInstance(wxCLSID_TaskbarList)"), hr);
        return;
    }

    hr = m_taskbarList->HrInit();
    if ( FAILED(hr) )
    {
        wxLogApiError(wxT("ITaskbarList3::Init"), hr);
        return;
    }
}

wxTaskBarButtonImpl::~wxTaskBarButtonImpl()
{
    if ( m_taskbarList )
      m_taskbarList->Release();

    for ( wxThumbBarButtons::iterator iter = m_thumbBarButtons.begin();
          iter != m_thumbBarButtons.end();
          ++iter)
    {
        delete (*iter);
    }
    m_thumbBarButtons.clear();
}

void wxTaskBarButtonImpl::SetProgressRange(int range)
{
    m_progressRange = range;
    if ( m_progressRange == 0 )
        SetProgressState(wxTASKBAR_BUTTON_NO_PROGRESS);
}

void wxTaskBarButtonImpl::SetProgressValue(int value)
{
    m_taskbarList->SetProgressValue(m_hwnd, value, m_progressRange);
}

void wxTaskBarButtonImpl::PulseProgress()
{
    SetProgressState(wxTASKBAR_BUTTON_INDETERMINATE);
}

void wxTaskBarButtonImpl::Show(bool show)
{
    if ( show )
        m_taskbarList->AddTab(m_hwnd);
    else
        m_taskbarList->DeleteTab(m_hwnd);
}

void wxTaskBarButtonImpl::Hide()
{
    Show(false);
}

void wxTaskBarButtonImpl::SetThumbnailTooltip(const wxString& tooltip)
{
    m_taskbarList->SetThumbnailTooltip(m_hwnd, tooltip.wc_str());
}

void wxTaskBarButtonImpl::SetProgressState(wxTaskBarButtonState state)
{
    m_taskbarList->SetProgressState(m_hwnd, static_cast<TBPFLAG>(state));
}

void wxTaskBarButtonImpl::SetOverlayIcon(const wxIcon& icon,
                                         const wxString& description)
{
    m_taskbarList->SetOverlayIcon(m_hwnd,
                                  GetHiconOf(icon),
                                  description.wc_str());
}

void wxTaskBarButtonImpl::SetThumbnailClip(const wxRect& rect)
{
    RECT rc;
    wxCopyRectToRECT(rect, rc);
    m_taskbarList->SetThumbnailClip(m_hwnd, rect.IsEmpty() ? NULL : &rc);
}

void wxTaskBarButtonImpl::SetThumbnailContents(const wxWindow *child)
{
    SetThumbnailClip(child->GetRect());
}

bool wxTaskBarButtonImpl::AppendThumbBarButton(wxThumbBarButton *button)
{
    wxASSERT_MSG( m_thumbBarButtons.size() < MAX_BUTTON_COUNT,
                  "Number of ThumbBarButtons and separators is limited to 7" );

    button->SetParent(this);
    m_thumbBarButtons.push_back(button);
    return InitOrUpdateThumbBarButtons();
}

bool wxTaskBarButtonImpl::AppendSeparatorInThumbBar()
{
    wxASSERT_MSG( m_thumbBarButtons.size() < MAX_BUTTON_COUNT,
                  "Number of ThumbBarButtons and separators is limited to 7" );

    // Append a disable ThumbBarButton without background can simulate the
    // behavior of appending a separator.
    wxThumbBarButton *separator = new wxThumbBarButton(wxID_ANY,
                                                       wxNullIcon,
                                                       wxEmptyString,
                                                       false,
                                                       false,
                                                       false);
    m_thumbBarButtons.push_back(separator);
    return InitOrUpdateThumbBarButtons();
}

bool wxTaskBarButtonImpl::InsertThumbBarButton(size_t pos,
                                               wxThumbBarButton *button)
{
    wxASSERT_MSG( m_thumbBarButtons.size() < MAX_BUTTON_COUNT,
                  "Number of ThumbBarButtons and separators is limited to 7" );
    wxASSERT_MSG( pos <= m_thumbBarButtons.size(),
                  "Invalid index when inserting the button" );

    button->SetParent(this);
    m_thumbBarButtons.insert(m_thumbBarButtons.begin() + pos, button);
    return InitOrUpdateThumbBarButtons();
}

wxThumbBarButton* wxTaskBarButtonImpl::RemoveThumbBarButton(
    wxThumbBarButton *button)
{
    for ( wxThumbBarButtons::iterator iter = m_thumbBarButtons.begin();
          iter != m_thumbBarButtons.end();
          ++iter )
    {
        if ( button == *iter )
        {
            m_thumbBarButtons.erase(iter);
            button->SetParent(NULL);
            InitOrUpdateThumbBarButtons();
            return *iter;
        }
    }

    return NULL;
}

wxThumbBarButton* wxTaskBarButtonImpl::RemoveThumbBarButton(int id)
{
    for ( wxThumbBarButtons::iterator iter = m_thumbBarButtons.begin();
          iter != m_thumbBarButtons.end();
          ++iter )
    {
        if ( id == (*iter)->GetID() )
        {
            m_thumbBarButtons.erase(iter);
            (*iter)->SetParent(NULL);
            InitOrUpdateThumbBarButtons();
            return *iter;
        }
    }

    return NULL;
}

bool wxTaskBarButtonImpl::InitOrUpdateThumbBarButtons()
{
    THUMBBUTTON buttons[MAX_BUTTON_COUNT];
    HRESULT hr;

    for ( size_t i = 0; i < MAX_BUTTON_COUNT; ++i )
    {
        memset(&buttons[i], 0, sizeof buttons[i]);
        buttons[i].iId = i;
        buttons[i].dwFlags = THBF_HIDDEN;
        buttons[i].dwMask = static_cast<THUMBBUTTONMASK>(THB_FLAGS);
    }

    for ( size_t i = 0; i < m_thumbBarButtons.size(); ++i )
    {
        buttons[i].hIcon = GetHiconOf(m_thumbBarButtons[i]->GetIcon());
        buttons[i].dwFlags = GetNativeThumbButtonFlags(*m_thumbBarButtons[i]);
        buttons[i].dwMask = static_cast<THUMBBUTTONMASK>(THB_ICON | THB_FLAGS);
        wxString tooltip = m_thumbBarButtons[i]->GetTooltip();
        if ( tooltip.empty() )
            continue;

        // Truncate the tooltip if its length longer than szTip(THUMBBUTTON)
        // allowed length (260).
        tooltip.Truncate(260);
        wxStrlcpy(buttons[i].szTip, tooltip.t_str(), tooltip.length());
        buttons[i].dwMask =
            static_cast<THUMBBUTTONMASK>(buttons[i].dwMask | THB_TOOLTIP);
    }

    if ( !m_hasInitThumbnailToolbar )
    {
        hr = m_taskbarList->ThumbBarAddButtons(m_hwnd,
                                               MAX_BUTTON_COUNT,
                                               buttons);
        if ( FAILED(hr) )
        {
            wxLogApiError(wxT("ITaskbarList3::ThumbBarAddButtons"), hr);
        }
        m_hasInitThumbnailToolbar = true;
    }
    else
    {
        hr = m_taskbarList->ThumbBarUpdateButtons(m_hwnd,
                                                  MAX_BUTTON_COUNT,
                                                  buttons);
        if ( FAILED(hr) )
        {
            wxLogApiError(wxT("ITaskbarList3::ThumbBarUpdateButtons"), hr);
        }
    }

    return SUCCEEDED(hr);
}

wxThumbBarButton* wxTaskBarButtonImpl::GetThumbBarButtonByIndex(size_t index)
{
    if ( index >= m_thumbBarButtons.size() )
        return NULL;

    return m_thumbBarButtons[index];
}

wxAppProgressIndicator::wxAppProgressIndicator(wxTopLevelWindow *parent, int maxValue)
    : m_parent(parent), m_maxValue(maxValue)
{
}

wxAppProgressIndicator::~wxAppProgressIndicator()
{
    m_taskBarButton->SetProgressState(wxTASKBAR_BUTTON_NO_PROGRESS);
}

bool wxAppProgressIndicator::Update(int value)
{
    wxASSERT_MSG( value <= m_maxValue, wxT("invalid progress value") );
    Init();

    m_taskBarButton->SetProgressValue(value);
    return true;
}

bool wxAppProgressIndicator::Pulse()
{
    Init();
    m_taskBarButton->PulseProgress();
    return true;
}

void wxAppProgressIndicator::Init()
{
    if ( m_taskBarButton.get() == NULL )
    {
        // Sleep 100 milliseconds to wait for creation of taskbar button.
        // TODO(zhchbin): Do not use sleep since it will block the UI thread.
        // Currently it is used to make sure the API works correctlly.
        wxMilliSleep(100);
        m_taskBarButton.reset(new wxTaskBarButtonImpl(m_parent->GetHandle()));
        m_taskBarButton->SetProgressRange(m_maxValue);
    }
}

wxJumpListItem::wxJumpListItem(wxJumpListItemType type,
                               const wxString& title,
                               const wxString& filePath,
                               const wxString& arguments,
                               const wxString& tooltip,
                               const wxString& iconPath,
                               int iconIndex)
    : m_type(type),
      m_title(title),
      m_filePath(filePath),
      m_arguments(arguments),
      m_tooltip(tooltip),
      m_iconPath(iconPath),
      m_iconIndex(iconIndex)
{
}


wxJumpListItemType wxJumpListItem::GetType() const
{
    return m_type;
}

void wxJumpListItem::SetType(wxJumpListItemType type)
{
    m_type = type;
}

const wxString& wxJumpListItem::GetTitle() const
{
    return m_title;
}

void wxJumpListItem::SetTitle(const wxString& title)
{
    m_title = title;
}

const wxString& wxJumpListItem::GetFilePath() const
{
    return m_filePath;
}

void wxJumpListItem::SetFilePath(const wxString& filePath)
{
    m_filePath = filePath;
}

const wxString& wxJumpListItem::GetArguments() const
{
    return m_arguments;
}

void wxJumpListItem::SetArguments(const wxString& arguments)
{
    m_arguments = arguments;
}

const wxString& wxJumpListItem::GetTooltip() const
{
    return m_tooltip;
}

void wxJumpListItem::SetTooltip(const wxString& tooltip)
{
    m_tooltip = tooltip;
}

const wxString& wxJumpListItem::GetIconPath() const
{
    return m_iconPath;
}

void wxJumpListItem::SetIconPath(const wxString& iconPath)
{
    m_iconPath = iconPath;
}

int wxJumpListItem::GetIconIndex() const
{
    return m_iconIndex;
}

void wxJumpListItem::SetIconIndex(int iconIndex)
{
    m_iconIndex = iconIndex;
}

wxJumpListCategory::wxJumpListCategory(const wxString& title)
    : m_title(title)
{
}

wxJumpListCategory::~wxJumpListCategory()
{
    for (wxJumpListItems::iterator it = m_items.begin();
         it != m_items.end();
         ++it)
    {
        delete *it;
    }
}

wxJumpListItem* wxJumpListCategory::Append(wxJumpListItem *item)
{
    m_items.push_back(item);
    return item;
}

void wxJumpListCategory::Delete(wxJumpListItem *item)
{
    item = Remove(item);
    if ( item )
        delete item;
}

wxJumpListItem* wxJumpListCategory::Remove(wxJumpListItem *item)
{
    for (wxJumpListItems::iterator it = m_items.begin();
         it != m_items.end();
         ++it)
    {
        if ( *it == item )
        {
            m_items.erase(it);
            return item;
        }
    }

    return NULL;
}

wxJumpListItem* wxJumpListCategory::FindItemByPosition(size_t pos) const
{
    wxASSERT_MSG(pos < m_items.size(), "invalid pos.");
    return m_items[pos];
}

wxJumpListItem* wxJumpListCategory::Insert(size_t pos, wxJumpListItem *item)
{
    wxASSERT_MSG(pos <= m_items.size(), "invalid pos.");
    m_items.insert(m_items.begin() + pos, item);
    return item;
}

wxJumpListItem* wxJumpListCategory::Prepend(wxJumpListItem *item)
{
    return Insert(0, item);
}

void wxJumpListCategory::SetTitle(const wxString& title)
{
    m_title = title;
}

const wxString& wxJumpListCategory::GetTitle() const
{
    return m_title;
}

const wxJumpListItems& wxJumpListCategory::GetItems() const
{
    return m_items;
}

wxJumpList::wxJumpList() : m_destinationList(NULL)
{

    HRESULT hr = CoCreateInstance
                 (
                    wxCLSID_DestinationList,
                    NULL,
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

wxJumpList::~wxJumpList()
{
    if ( m_destinationList )
        m_destinationList->Release();

    for ( wxJumpListCategories::iterator it = m_customCategories.begin();
          it != m_customCategories.end();
          ++it )
    {
        delete *it;
    }
}

void wxJumpList::Update()
{
    if ( !BeginUpdate() )
        return;

    AddTasksToDestinationList();
    AddCustomCategoriesToDestionationList();
    if ( m_recent_visible )
        m_destinationList->AppendKnownCategory(KDC_RECENT);
    if ( m_frequent_visible )
        m_destinationList->AppendKnownCategory(KDC_FREQUENT);
    CommitUpdate();
}

wxJumpListCategory* wxJumpList::GetTasks()
{
    if ( m_tasks.get() == NULL )
        m_tasks.reset(new wxJumpListCategory(wxT("Tasks")));

    return m_tasks.get();
}

void wxJumpList::ShowRecentCategory(bool shown)
{
    m_recent_visible = shown;
}

void wxJumpList::HideRecentCategory()
{
    ShowRecentCategory(false);
}

void wxJumpList::ShowFrequentCategory(bool shown)
{
    m_frequent_visible = shown;
}

void wxJumpList::HideFrequentCategory()
{
    ShowFrequentCategory(false);
}

const wxJumpListCategory* wxJumpList::GetFrequentCategory()
{
    wxString title = wxT("Frequent");
    if ( m_frequent.get() == NULL )
        m_frequent.reset(new wxJumpListCategory(title));
    LoadKnownCategory(title);
    return m_frequent.get();
}

const wxJumpListCategory* wxJumpList::GetRecentCategory()
{
    wxString title = wxT("Recent");
    if ( m_recent.get() == NULL )
        m_recent.reset(new wxJumpListCategory(title));
    LoadKnownCategory(title);
    return m_recent.get();
}

const wxJumpListCategories& wxJumpList::GetCustomCategories()
{
    return m_customCategories;
}

void wxJumpList::AddCategory(wxJumpListCategory *catalog)
{
    m_customCategories.push_back(catalog);
}

wxJumpListCategory* wxJumpList::RemoveCategory(const wxString& title)
{
    for ( wxJumpListCategories::iterator it = m_customCategories.begin();
          it != m_customCategories.end();
          ++it )
    {
        if ( (*it)->GetTitle() == title )
        {
            m_customCategories.erase(it);
            return *it;
        }
    }

    return NULL;
}

void wxJumpList::DeleteCategory(const wxString& title)
{
    wxJumpListCategory* category = RemoveCategory(title);
    if ( category )
        delete category;
}

bool wxJumpList::BeginUpdate()
{
    if ( m_destinationList == NULL )
        return false;

    unsigned int max_count = 0;
    HRESULT hr = m_destinationList->BeginList(&max_count,
        wxIID_IObjectArray, reinterpret_cast<void**>(&(m_objectArray)));

    return SUCCEEDED(hr);
}

bool wxJumpList::CommitUpdate()
{
    m_objectArray->Release();
    return SUCCEEDED(m_destinationList->CommitList());
}

void wxJumpList::AddTasksToDestinationList()
{
    IObjectCollection* collection = CreateObjectCollection();
    if ( !collection )
        return;

    const wxJumpListItems& tasks = m_tasks->GetItems();
    for ( wxJumpListItems::const_iterator it = tasks.begin();
          it != tasks.end();
          ++it )
    {
        AddShellLink(collection, *(*it));
    }
    m_destinationList->AddUserTasks(collection);
    collection->Release();
}

void wxJumpList::AddCustomCategoriesToDestionationList()
{
    for ( wxJumpListCategories::iterator iter = m_customCategories.begin();
          iter != m_customCategories.end();
          ++iter )
    {
        IObjectCollection* collection = CreateObjectCollection();
        if ( !collection )
            continue;

        const wxJumpListItems& tasks = (*iter)->GetItems();
        for ( wxJumpListItems::const_iterator it = tasks.begin();
              it != tasks.end();
              ++it )
        {
            AddShellLink(collection, *(*it));
        }
        m_destinationList->AppendCategory((*iter)->GetTitle().wc_str(),
                                          collection);
        collection->Release();
    }
}

void wxJumpList::LoadKnownCategory(const wxString& title)
{
    IApplicationDocumentLists *docList = 0;
    HRESULT hr = CoCreateInstance
                 (
                    wxCLSID_ApplicationDocumentLists,
                    NULL,
                    CLSCTX_INPROC_SERVER,
                    wxIID_IApplicationDocumentLists,
                    reinterpret_cast<void **>(&docList)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError("CoCreateInstance(wxCLSID_ApplicationDocumentLists)", hr);
        return;
    }

    IObjectArray *array = NULL;
    wxASSERT_MSG(title == "Recent" || title == "Frequent", "Invalid title.");
    hr = docList->GetList
                 (
                     title == "Recent" ? ADLT_RECENT : ADLT_FREQUENT,
                     NULL,
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
        IUnknown *collectionItem = NULL;
        hr = array->GetAt(i, wxIID_IUnknown,
                          reinterpret_cast<void **>(&collectionItem));
        if ( FAILED(hr) )
        {
            wxLogApiError("IObjectArray::GetAt", hr);
            continue;
        }

        IShellLink *shellLink = NULL;
        IShellItem *shellItem = NULL;
        wxJumpListItem* item = NULL;

        if ( SUCCEEDED(collectionItem->QueryInterface(
                 wxIID_IShellLink, reinterpret_cast<void**>(&shellLink))) )
        {
            item = GetItemFromIShellLink(shellLink);
            shellLink->Release();
        }
        else if ( SUCCEEDED(collectionItem->QueryInterface(
                      wxIID_IShellItem, reinterpret_cast<void**>(&shellItem))) )
        {
            item = GetItemFromIShellItem(shellItem);
            shellItem->Release();
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
        collectionItem->Release();
    }

    array->Release();
    docList->Release();
}

#endif // wxUSE_TASKBARBUTTON
