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

namespace {

// The maximum number of thumbnail toolbar buttons allowed on windows is 7.
static const int LIMITED_BUTTON_SIZE = 7;

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
      m_interactive(interactive)
{
}

wxTaskBarButtonImpl::wxTaskBarButtonImpl(WXWidget parent)
    : m_hwnd(parent),
      m_taskbarList(NULL),
      m_progressRange(0),
      m_hasInitThumbnailToolbar(false)
{
    HRESULT hr = CoCreateInstance
                 (
                    CLSID_TaskbarList,
                    NULL,
                    CLSCTX_ALL,
                    IID_ITaskbarList3,
                    reinterpret_cast<void **>(&m_taskbarList)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError(wxT("CoCreateInstance(CLSID_TaskbarButtonList)"), hr);
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
    if ( m_thumbBarButtons.size() >= LIMITED_BUTTON_SIZE )
        return false;

    m_thumbBarButtons.push_back(button);
    return InitOrUpdateThumbBarButtons();
}

bool wxTaskBarButtonImpl::InsertThumbBarButton(size_t pos,
                                               wxThumbBarButton *button)
{
    if ( m_thumbBarButtons.size() >= LIMITED_BUTTON_SIZE ||
         m_thumbBarButtons.size() < pos )
        return false;

    m_thumbBarButtons.insert(m_thumbBarButtons.begin() + pos, button);
    return InitOrUpdateThumbBarButtons();
}

bool wxTaskBarButtonImpl::RemoveThumbBarButton(wxThumbBarButton *button)
{
    wxThumbBarButtons::iterator it;
    for ( it = m_thumbBarButtons.begin(); it != m_thumbBarButtons.end(); ++it )
    {
        if ( button == *it )
        {
            m_thumbBarButtons.erase(it);
            return InitOrUpdateThumbBarButtons();
        }
    }

    return false;
}

bool wxTaskBarButtonImpl::RemoveThumbBarButton(int id)
{
    wxThumbBarButtons::iterator it;
    for ( it = m_thumbBarButtons.begin(); it != m_thumbBarButtons.end(); ++it )
    {
        if ( id == (*it)->GetID() )
        {
            m_thumbBarButtons.erase(it);
            return InitOrUpdateThumbBarButtons();
        }
    }

    return false;
}

bool wxTaskBarButtonImpl::InitOrUpdateThumbBarButtons()
{
    THUMBBUTTON buttons[LIMITED_BUTTON_SIZE];
    size_t i;
    HRESULT hr;

    for ( i = 0; i < LIMITED_BUTTON_SIZE; ++i )
    {
        memset(&buttons[i], 0, sizeof buttons[i]);
        buttons[i].iId = i;
        buttons[i].dwFlags = THBF_HIDDEN;
        buttons[i].dwMask  = THB_FLAGS;
    }

    for ( i = 0; i < m_thumbBarButtons.size(); ++i )
    {
        buttons[i].hIcon = GetHiconOf(m_thumbBarButtons[i]->GetIcon());
        buttons[i].dwFlags = GetNativeThumbButtonFlags(*m_thumbBarButtons[i]);
        buttons[i].dwMask = THB_ICON | THB_FLAGS;
        wxString tooltip = m_thumbBarButtons[i]->GetTooltip();
        if ( tooltip.empty() )
            continue;

        // Truncate the tooltip if its length longer than szTip(THUMBBUTTON)
        // allowed length (260).
        if ( tooltip.length() > WXSIZEOF(buttons[i].szTip) )
            tooltip = tooltip.SubString(0, WXSIZEOF(buttons[i].szTip) - 1);
        wxStrlcpy(buttons[i].szTip, tooltip.t_str(), tooltip.length());
        buttons[i].dwMask |= THB_TOOLTIP;
    }

    if ( !m_hasInitThumbnailToolbar )
    {
        hr = m_taskbarList->ThumbBarAddButtons(m_hwnd,
                                               LIMITED_BUTTON_SIZE,
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
                                                  LIMITED_BUTTON_SIZE,
                                                  buttons);
        if ( FAILED(hr) )
        {
            wxLogApiError(wxT("ITaskbarList3::ThumbBarUpdateButtons"), hr);
        }
    }

    return SUCCEEDED(hr);
}

int wxTaskBarButtonImpl::GetThumbBarButtonID(size_t index)
{
    if ( index >= m_thumbBarButtons.size() )
        return -1;

    return m_thumbBarButtons[index]->GetID();
}

#endif // wxUSE_TASKBARBUTTON
