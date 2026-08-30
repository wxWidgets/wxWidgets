///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/pickerbasetest.cpp
// Purpose:     wxPickerBase unit test
// Author:      Steven Lamerton
// Created:     2010-08-07
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_COLOURPICKERCTRL || \
    wxUSE_DIRPICKERCTRL    || \
    wxUSE_FILEPICKERCTRL   || \
    wxUSE_FONTPICKERCTRL

#include "wx/pickerbase.h"
#include "pickerbasetest.h"

void PickerBaseTestCase::Margin()
{
    wxPickerBase* const base = GetBase();

    CHECK(base->HasTextCtrl());
    CHECK(base->GetInternalMargin() >= 0);

    base->SetInternalMargin(15);

    CHECK(base->GetInternalMargin() == 15);
}

void PickerBaseTestCase::Proportion()
{
    wxPickerBase* const base = GetBase();

    CHECK(base->HasTextCtrl());

    base->SetPickerCtrlProportion(1);
    base->SetTextCtrlProportion(1);

    CHECK(base->GetPickerCtrlProportion() == 1);
    CHECK(base->GetTextCtrlProportion() == 1);
}

void PickerBaseTestCase::Growable()
{
    wxPickerBase* const base = GetBase();

    CHECK(base->HasTextCtrl());

    base->SetPickerCtrlGrowable();
    base->SetTextCtrlGrowable();

    CHECK(base->IsPickerCtrlGrowable());
    CHECK(base->IsTextCtrlGrowable());

    base->SetPickerCtrlGrowable(false);
    base->SetTextCtrlGrowable(false);

    CHECK(!base->IsPickerCtrlGrowable());
    CHECK(!base->IsTextCtrlGrowable());
}

void PickerBaseTestCase::Controls()
{
    wxPickerBase* const base = GetBase();

    CHECK(base->HasTextCtrl());
    CHECK(base->GetTextCtrl() != nullptr);
    CHECK(base->GetPickerCtrl() != nullptr);
}

#endif
