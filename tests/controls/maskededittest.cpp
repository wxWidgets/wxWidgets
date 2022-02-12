///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/maskededittest.cpp
// Purpose:     wxMaskedEdit controls unit test
// Author:      Manuel Martin
// Created:     2012-07-30
// RCS-ID:      $Id:
// Copyright:   (c)
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_MASKED_EDIT

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/generic/maskededit.h"

#include "testableframe.h"
#include "wx/uiaction.h"

class MaskedEditTestCase
{
public:
    MaskedEditTestCase()
    {
        m_maskedEdit = new wxMaskedEditText(wxTheApp->GetTopWindow(), wxID_ANY);
        m_maskedEdit->SetMask( "=a>A^-###++xx.#{4}" );
    }
    ~MaskedEditTestCase ()
    {
        delete m_maskedEdit;
    }

protected:
    wxMaskedEditText* m_maskedEdit;

    DECLARE_NO_COPY_CLASS(MaskedEditTestCase)
};

TEST_CASE_METHOD( MaskedEditTestCase, "MaskedEditTestCase::CreateControl", "[maskedtextctrl]" )
{
    // test empty value
    CHECK( m_maskedEdit->GetValue() == "=  -   ++  .    " );

    m_maskedEdit->SetValue( "=bc- 12++  .9876" );
    // 'c' should be changed to 'C' and '12' should be left re-aligned
    CHECK( m_maskedEdit->GetValue() == "=bC-12 ++  .9876" );

    m_maskedEdit->SetFieldFlags( 3, wxMaskedEditFieldFlags( wxALIGN_RIGHT ) );
    m_maskedEdit->SetPlainValue( "fg56" );
    m_maskedEdit->SetFieldValue( 3, "987" );
    CHECK( m_maskedEdit->GetValue() == "=fG-56 ++  . 987" );
}

TEST_CASE_METHOD( MaskedEditTestCase, "MaskedEditTestCase::TestSet", "[maskedtextctrl]" )
{
    m_maskedEdit->SetMask( "=a>A^-###++xx.#{4}" );
    // test empty value
    CHECK( m_maskedEdit->GetValue() == "=  -   ++  .    " );

    m_maskedEdit->SetValue( "=bc- 12++  .9876" );
    // 'c' should be changed to 'C' and '12' should be left re-aligned
    CHECK( m_maskedEdit->GetValue() == "=bC-12 ++  .9876" );

    m_maskedEdit->SetFieldFlags( 3, wxMaskedEditFieldFlags( wxALIGN_RIGHT ) );
    m_maskedEdit->SetPlainValue( "fg56" );
    m_maskedEdit->SetFieldValue( 3, "987" );
    CHECK( m_maskedEdit->GetValue() == "=fG-56 ++  . 987" );
}

#if wxUSE_UIACTIONSIMULATOR
TEST_CASE_METHOD( MaskedEditTestCase, "MaskedEditTestCase::TestUI", "[maskedtextctrl]" )
{
    wxUIActionSimulator sim;
    m_maskedEdit->SetFieldFlags( 3, wxMaskedEditFieldFlags( wxALIGN_RIGHT ) );
    m_maskedEdit->SetValue( "=fG-56 ++  . 987" );
    m_maskedEdit->SetFocus();

    sim.Char( WXK_END );
    wxYield();
    sim.Char( WXK_BACK );
    wxYield();
    CHECK( m_maskedEdit->GetValue() == "=fG-56 ++  .  98" );
    sim.Char( WXK_LEFT );
    sim.Char( WXK_LEFT );
    sim.Char( WXK_LEFT );
    sim.Char( 'd' );
    wxYield();
    CHECK( m_maskedEdit->GetValue() == "=fG-56 ++d .  98" );

    m_maskedEdit->SetInsertionPoint( 6 );
    sim.Char( WXK_RIGHT );
    sim.Char( '2' );
    wxYield();
    CHECK( m_maskedEdit->GetValue() == "=fG-56 ++2d.  98" );
}
#endif

TEST_CASE_METHOD( MaskedEditTestCase, "MaskedEditTestCase::TestFunc", "[maskedtextctrl]" )
{
    // Should fail because not all fields have all required chars
    CHECK( !m_maskedEdit->GetInvalidFieldIndex() );

    m_maskedEdit->ChangeValue( "=aB-123++  .5670" );
    CHECK( m_maskedEdit->GetInvalidFieldIndex() );

    wxRangeParams parms;
    parms.rmin = 70;
    parms.rmax = 100;
    m_maskedEdit->SetFieldFunction( 1, &wxMaskedRangeCheck, &parms );
    // Should fail because 123 > 100
    CHECK( !m_maskedEdit->GetInvalidFieldIndex() );
}

#endif // wxUSE_MASKED_EDIT
