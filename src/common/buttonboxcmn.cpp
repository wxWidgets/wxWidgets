/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/buttonboxcmn.cpp
// Purpose:     wxButtonBox common code
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     12.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BUTTONBOX

#include "wx/buttonbox.h"

const char wxButtonBoxNameStr[] = "buttonBox";

wxDEFINE_EVENT(wxEVT_BUTTONBOX, wxCommandEvent);

void wxButtonBoxBase::SendButtonBoxEvent(int index, bool selected)
{
    wxCommandEvent event(wxEVT_BUTTONBOX, GetId());
    event.SetEventObject(this);
    event.SetInt(index);
    event.SetExtraLong(selected ? 1 : 0);
    event.SetString(GetItemLabel((size_t)index));
    HandleWindowEvent(event);
}

#endif // wxUSE_BUTTONBOX
