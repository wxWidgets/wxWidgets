/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/systhemectrl.cpp
// Purpose:     wxMSW implementation of wxSystemThemedControl
// Author:      Tobias Taschner
// Created:     2015-09-15
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#include "wx/systhemectrl.h"

#include "wx/msw/private.h"
#include "wx/msw/uxtheme.h"

#ifdef wxHAS_SYSTEM_THEMED_CONTROL

void wxSystemThemedControlBase::DoEnableSystemTheme(bool enable, wxWindow* window)
{
    if ( wxGetWinVersion() >= wxWinVersion_Vista && wxUxThemeIsActive() )
    {
        // It's possible to call EnableSystemTheme(false) before creating the
        // window, just don't do anything in this case.
        if ( window->GetHandle() )
        {
            const wchar_t* const sysThemeId = enable ? L"EXPLORER" : nullptr;
            ::SetWindowTheme(GetHwndOf(window), sysThemeId, nullptr);
        }

        m_systemThemeDisabled = !enable;
    }
}

#endif // wxHAS_SYSTEM_THEMED_CONTROL
