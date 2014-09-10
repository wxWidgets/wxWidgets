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

#if wxUSE_TASKBARBUTTON

#include "wx/msw/wrapshl.h"
#include "wx/msw/private.h"
#include "wx/taskbarbutton.h"

#include <Shobjidl.h>

wxThumbBarButton::wxThumbBarButton(int id,
                                   const wxIcon& icon,
                                   const wxString& tooltip)
    : m_id(id), m_icon(icon), m_tooltip(tooltip)
{
}

wxTaskBarButtonImpl::wxTaskBarButtonImpl(WXWidget parent)
    : m_hwnd(parent), m_taskbarList(NULL), m_hasShownThumbnailToolbar(false)
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
        wxLogApiError(wxT("ITaskbarButtonList3::Init"), hr);
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

void wxTaskBarButtonImpl::SetProgressValue(int value)
{
    wxCHECK_RET( value >= 0 && value <= 100,
                 wxT("Invalid value, must be in the range of [0, 100].") );

    m_taskbarList->SetProgressValue(m_hwnd, value, 100);
}

void wxTaskBarButtonImpl::Show()
{
    m_taskbarList->AddTab(m_hwnd);
}

void wxTaskBarButtonImpl::Hide()
{
    m_taskbarList->DeleteTab(m_hwnd);
}

void wxTaskBarButtonImpl::SetThumbnailTooltip(const wxString& tooltip)
{
    m_taskbarList->SetThumbnailTooltip(m_hwnd, tooltip.wc_str());
}

void wxTaskBarButtonImpl::SetProgressState(wxTaskBarButtonState state)
{
    m_taskbarList->SetProgressState(m_hwnd, static_cast<TBPFLAG>(state));
}

void wxTaskBarButtonImpl::SetOverlayIcon(const wxIcon& icon)
{
    m_taskbarList->SetOverlayIcon(m_hwnd, GetHiconOf(icon), NULL);
}

void wxTaskBarButtonImpl::SetThumbnailClip(const wxRect& rect)
{
    RECT rc;
    wxCopyRectToRECT(rect, rc);
    m_taskbarList->SetThumbnailClip(m_hwnd, rect.IsEmpty() ? NULL : &rc);
}

bool wxTaskBarButtonImpl::AddThumbBarButton(wxThumbBarButton *button)
{
    wxCHECK( button != NULL, wxT("Can't add invalid wxThumbBarButton.") );
    if (m_thumbBarButtons.size() >= 7)
        return false;

    m_thumbBarButtons.push_back(button);
    return true;
}

void wxTaskBarButtonImpl::ShowThumbnailToolbar()
{
    if ( m_hasShownThumbnailToolbar || m_thumbBarButtons.empty() )
        return;

    THUMBBUTTON buttons[7];
    size_t i;
    for ( i = 0; i < m_thumbBarButtons.size(); ++i )
    {
        buttons[i].iId = m_thumbBarButtons[i]->GetID();
        buttons[i].hIcon = GetHiconOf(m_thumbBarButtons[i]->GetIcon());
        buttons[i].dwFlags = THBF_ENABLED;
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

    m_taskbarList->ThumbBarAddButtons(m_hwnd,
                                      m_thumbBarButtons.size(),
                                      buttons);
    m_hasShownThumbnailToolbar = true;
}

#endif // wxUSE_TASKBARBUTTON
