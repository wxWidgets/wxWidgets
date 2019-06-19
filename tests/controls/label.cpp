///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/label.cpp
// Purpose:     wxControl and wxStaticText label tests
// Author:      Francesco Montorsi
// Created:     2010-3-21
// Copyright:   (c) 2010 Francesco Montorsi
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/checkbox.h"
#include "wx/control.h"
#include "wx/scopedptr.h"
#include "wx/stattext.h"

namespace
{

const char* const ORIGINAL_LABEL = "origin label";

// The actual testing function. It will change the label of the provided
// control, which is assumed to be ORIGINAL_LABEL initially.
void DoTestLabel(wxControl* c)
{
    CHECK( c->GetLabel() == ORIGINAL_LABEL );

    const wxString testLabelArray[] = {
        "label without mnemonics and markup",
        "label with &mnemonic",
        "label with <span foreground='blue'>some</span> <b>markup</b>",
        "label with <span foreground='blue'>some</span> <b>markup</b> and &mnemonic",
    };

    for ( unsigned int s = 0; s < WXSIZEOF(testLabelArray); s++ )
    {
        const wxString& l = testLabelArray[s];

        // GetLabel() should always return the string passed to SetLabel()
        c->SetLabel(l);
        CHECK( c->GetLabel() == l );

        // GetLabelText() should always return the string passed to SetLabelText()
        c->SetLabelText(l);
        CHECK( c->GetLabelText() == l );
    }


    // test calls to SetLabelText() and then to GetLabel()

    wxString testLabel = "label without mnemonics and markup";
    c->SetLabelText(testLabel);
    CHECK( c->GetLabel() == testLabel );

    c->SetLabelText("label with &mnemonic");
    CHECK( c->GetLabel() == "label with &&mnemonic" );

    testLabel = "label with <span foreground='blue'>some</span> <b>markup</b>";
    c->SetLabelText(testLabel);
    CHECK( c->GetLabel() == testLabel );

    c->SetLabelText("label with <span foreground='blue'>some</span> <b>markup</b> and &mnemonic");
    CHECK( c->GetLabel() == "label with <span foreground='blue'>some</span> <b>markup</b> and &&mnemonic" );

    // test calls to SetLabel() and then to GetLabelText()

    testLabel = "label without mnemonics and markup";
    c->SetLabel(testLabel);
    CHECK( c->GetLabelText() == testLabel );

    c->SetLabel("label with &mnemonic");
    CHECK( c->GetLabelText() == "label with mnemonic" );

    testLabel = "label with <span foreground='blue'>some</span> <b>markup</b>";
    c->SetLabel(testLabel);
    CHECK( c->GetLabelText() == testLabel );

    c->SetLabel("label with <span foreground='blue'>some</span> <b>markup</b> and &mnemonic");
    CHECK( c->GetLabelText() == "label with <span foreground='blue'>some</span> <b>markup</b> and mnemonic");
}

} // anonymous namespace

TEST_CASE("wxControl::Label", "[wxControl][label]")
{
    SECTION("wxStaticText")
    {
        const wxScopedPtr<wxStaticText>
            st(new wxStaticText(wxTheApp->GetTopWindow(), wxID_ANY, ORIGINAL_LABEL));
        DoTestLabel(st.get());
    }

    SECTION("wxCheckBox")
    {
        const wxScopedPtr<wxCheckBox>
            cb(new wxCheckBox(wxTheApp->GetTopWindow(), wxID_ANY, ORIGINAL_LABEL));
        DoTestLabel(cb.get());
    }
}

TEST_CASE("wxControl::RemoveMnemonics", "[wxControl][label][mnemonics]")
{
    CHECK( "mnemonic"  == wxControl::RemoveMnemonics("&mnemonic") );
    CHECK( "&mnemonic" == wxControl::RemoveMnemonics("&&mnemonic") );
    CHECK( "&mnemonic" == wxControl::RemoveMnemonics("&&&mnemonic") );
}
