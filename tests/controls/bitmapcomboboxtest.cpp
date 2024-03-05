///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/bitmapcomboboxtest.cpp
// Purpose:     wxBitmapComboBox unit test
// Author:      Steven Lamerton
// Created:     2010-07-15
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_BITMAPCOMBOBOX


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/bmpcbox.h"
#include "wx/artprov.h"
#include "textentrytest.h"
#include "itemcontainertest.h"
#include "asserthelper.h"

class BitmapComboBoxTestCase : public TextEntryTestCase,
                               public ItemContainerTestCase,
                               public CppUnit::TestCase
{
public:
    BitmapComboBoxTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxTextEntry *GetTestEntry() const override { return m_combo; }
    virtual wxWindow *GetTestWindow() const override { return m_combo; }

    virtual wxItemContainer *GetContainer() const override { return m_combo; }
    virtual wxWindow *GetContainerWindow() const override { return m_combo; }

    virtual void CheckStringSelection(const char * WXUNUSED(sel)) override
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

    wxDECLARE_NO_COPY_CLASS(BitmapComboBoxTestCase);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(BitmapComboBoxTestCase,
                               "[BitmapComboBoxTestCase][item-container]");

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
    // TODO: Add wxBitmapComboBoxBase::Append(wxArrayString )
    for( unsigned int i = 0; i < items.size(); ++i )
        m_combo->Append(items[i]);

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

    m_combo->SetSelection( 1 );

    CPPUNIT_ASSERT_EQUAL( m_combo->GetStringSelection(), "item with bitmap" );
}

#endif //wxUSE_BITMAPCOMBOBOX
