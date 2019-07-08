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

#include "wx/generic/stattextg.h"

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
        "label with an && (ampersand)",
        "label with an && (&ampersand)",
        "", // empty label should work too
    };

    for ( unsigned int s = 0; s < WXSIZEOF(testLabelArray); s++ )
    {
        const wxString& l = testLabelArray[s];

        // GetLabel() should always return the string passed to SetLabel()
        c->SetLabel(l);
        CHECK( c->GetLabel() == l );

        // GetLabelText() should always return unescaped version of the label
        CHECK( c->GetLabelText() == wxControl::RemoveMnemonics(l) );

        // GetLabelText() should always return the string passed to SetLabelText()
        c->SetLabelText(l);
        CHECK( c->GetLabelText() == l );

        // And GetLabel() should be the escaped version of the text
        CHECK( l == wxControl::RemoveMnemonics(c->GetLabel()) );
    }

    // Check that both "&" and "&amp;" work in markup.
#if wxUSE_MARKUP
    c->SetLabelMarkup("mnemonic in &amp;markup");
    CHECK( c->GetLabel() == "mnemonic in &markup" );
    CHECK( c->GetLabelText() == "mnemonic in markup" );

    c->SetLabelMarkup("mnemonic in &markup");
    CHECK( c->GetLabel() == "mnemonic in &markup" );
    CHECK( c->GetLabelText() == "mnemonic in markup" );

    c->SetLabelMarkup("&amp;&amp; finally");
    CHECK( c->GetLabel() == "&& finally" );
    CHECK( c->GetLabelText() == "& finally" );

    c->SetLabelMarkup("&& finally");
    CHECK( c->GetLabel() == "&& finally" );
    CHECK( c->GetLabelText() == "& finally" );
#endif // wxUSE_MARKUP
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

    SECTION("wxStaticText/ellipsized")
    {
        const wxScopedPtr<wxStaticText>
            st(new wxStaticText(wxTheApp->GetTopWindow(), wxID_ANY, ORIGINAL_LABEL,
                                wxDefaultPosition, wxDefaultSize,
                                wxST_ELLIPSIZE_START));
        DoTestLabel(st.get());
    }

    SECTION("wxGenericStaticText")
    {
        const wxScopedPtr<wxGenericStaticText>
            gst(new wxGenericStaticText(wxTheApp->GetTopWindow(), wxID_ANY, ORIGINAL_LABEL));
        DoTestLabel(gst.get());
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

TEST_CASE("wxControl::FindAccelIndex", "[wxControl][label][mnemonics]")
{
    CHECK( wxControl::FindAccelIndex("foo") == wxNOT_FOUND );
    CHECK( wxControl::FindAccelIndex("&foo") == 0 );
    CHECK( wxControl::FindAccelIndex("f&oo") == 1 );
    CHECK( wxControl::FindAccelIndex("foo && bar") == wxNOT_FOUND );
    CHECK( wxControl::FindAccelIndex("foo && &bar") == 6 );
}
