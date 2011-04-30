///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/bitmapcomboboxtest.cpp
// Purpose:     wxBitmapComboBox unit test
// Author:      Steven Lamerton
// Created:     2010-07-15
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_BITMAPCOMBOBOX

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/bmpcbox.h"
#include "wx/artprov.h"
#include "textentrytest.h"
#include "itemcontainertest.h"
#include "asserthelper.h"

//Test only if we are based off of wxComboBox
#ifndef wxGENERIC_BITMAPCOMBOBOX

class BitmapComboBoxTestCase : public TextEntryTestCase,
                               public ItemContainerTestCase,
                               public CppUnit::TestCase
{
public:
    BitmapComboBoxTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxTextEntry *GetTestEntry() const { return m_combo; }
    virtual wxWindow *GetTestWindow() const { return m_combo; }

    virtual wxItemContainer *GetContainer() const { return m_combo; }
    virtual wxWindow *GetContainerWindow() const { return m_combo; }

    virtual void CheckStringSelection(const char * WXUNUSED(sel))
    {
        // do nothing here, as explained in TextEntryTestCase comment, our
        // GetStringSelection() is the wxChoice, not wxTextEntry, one and there
        // is no way to return the selection contents directly
    }

    CPPUNIT_TEST_SUITE( BitmapComboBoxTestCase );
        wxTEXT_ENTRY_TESTS();
        wxITEM_CONTAINER_TESTS();
        CPPUNIT_TEST( Bitmap );
    CPPUNIT_TEST_SUITE_END();

    void Bitmap();

    wxBitmapComboBox *m_combo;

    DECLARE_NO_COPY_CLASS(BitmapComboBoxTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( BitmapComboBoxTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( BitmapComboBoxTestCase,
                                      "BitmapComboBoxTestCase" );

void BitmapComboBoxTestCase::setUp()
{
    m_combo = new wxBitmapComboBox(wxTheApp->GetTopWindow(), wxID_ANY);
}

void BitmapComboBoxTestCase::tearDown()
{
    wxDELETE(m_combo);
}

void BitmapComboBoxTestCase::Bitmap()
{
    wxArrayString items;
    items.push_back("item 0");
    items.push_back("item 1");

    //We need this otherwise MSVC complains as it cannot find a suitable append
    static_cast<wxComboBox*>(m_combo)->Append(items);

    CPPUNIT_ASSERT(!m_combo->GetItemBitmap(0).IsOk());

    wxBitmap bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER,
                                             wxSize(16, 16));

    m_combo->Append("item with bitmap", bitmap);

    CPPUNIT_ASSERT(m_combo->GetItemBitmap(2).IsOk());

    m_combo->Insert("item with bitmap", bitmap, 1);

    CPPUNIT_ASSERT(m_combo->GetItemBitmap(1).IsOk());

    m_combo->SetItemBitmap(0, bitmap);

    CPPUNIT_ASSERT(m_combo->GetItemBitmap(0).IsOk());

    CPPUNIT_ASSERT_EQUAL(wxSize(16, 16), m_combo->GetBitmapSize());
}

#endif //wxGENERIC_BITMAPCOMBOBOX

#endif //wxUSE_BITMAPCOMBOBOX
