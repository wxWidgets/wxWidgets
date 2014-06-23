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
#endif

#if wxUSE_TASKBARBUTTON

#include "wx/msw/wrapshl.h"
#include "wx/msw/private.h"
#include "wx/taskbarbutton.h"

#include <Shobjidl.h>
#include <initguid.h>

namespace {

// The maximum number of thumbnail toolbar buttons allowed on windows is 7.
static const int MAX_BUTTON_COUNT = 7;

DEFINE_GUID(wxCLSID_TaskbarList,
    0x56fdf344, 0xfd6d, 0x11d0, 0x95, 0x8a, 0x0, 0x60, 0x97, 0xc9, 0xa0, 0x90);

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

void wxThumbBarButton::EnableInteractive(bool interactive)
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
                    IID_ITaskbarList3,
                    reinterpret_cast<void **>(&m_taskbarList)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError(wxT("CoCreateInstance(CLSID_TaskbarList)"), hr);
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
                  "Number of thumb buttons is limited to 7" );

    button->SetParent(this);
    m_thumbBarButtons.push_back(button);
    return InitOrUpdateThumbBarButtons();
}

bool wxTaskBarButtonImpl::InsertThumbBarButton(size_t pos,
                                               wxThumbBarButton *button)
{
    wxASSERT_MSG( m_thumbBarButtons.size() < MAX_BUTTON_COUNT,
                  "Number of thumb buttons is limited to 7" );
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

#endif // wxUSE_TASKBARBUTTON
