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

#include <memory>

class BitmapComboBoxTestCase : public TextEntryTestCase,
                               public ItemContainerTestCase
{
public:
    BitmapComboBoxTestCase();

protected:
    virtual wxTextEntry *GetTestEntry() const override
    { return m_combo.get(); }
    virtual wxWindow *GetTestWindow() const override { return m_combo.get(); }

    virtual wxItemContainer *GetContainer() const override
    { return m_combo.get(); }
    virtual wxWindow *GetContainerWindow() const override
    { return m_combo.get(); }

    virtual void CheckStringSelection(const char * WXUNUSED(sel)) override
    {
        // do nothing here, as explained in TextEntryTestCase comment, our
        // GetStringSelection() is the wxChoice, not wxTextEntry, one and there
        // is no way to return the selection contents directly
    }

    std::unique_ptr<wxBitmapComboBox> m_combo;

    wxDECLARE_NO_COPY_CLASS(BitmapComboBoxTestCase);
};

wxTEXT_ENTRY_TESTS(BitmapComboBoxTestCase, "BitmapComboBox",
                   "[bitmapcombobox][text-entry]");

wxITEM_CONTAINER_TESTS(BitmapComboBoxTestCase, "BitmapComboBox",
                       "[bitmapcombobox][item-container]");

BitmapComboBoxTestCase::BitmapComboBoxTestCase()
{
    m_combo = make_unique<wxBitmapComboBox>(wxTheApp->GetTopWindow(),
                                            wxID_ANY);
}


TEST_CASE_METHOD(BitmapComboBoxTestCase, "BitmapComboBox::Bitmap",
                 "[bitmapcombobox]")
{
    wxArrayString items;
    items.push_back("item 0");
    items.push_back("item 1");
    // TODO: Add wxBitmapComboBoxBase::Append(wxArrayString )
    for( unsigned int i = 0; i < items.size(); ++i )
        m_combo->Append(items[i]);

    CHECK(!m_combo->GetItemBitmap(0).IsOk());

    wxBitmap bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER,
                                             m_combo->FromDIP(wxSize(16, 16)));

    m_combo->Append("item with bitmap", bitmap);

    CHECK(m_combo->GetItemBitmap(2).IsOk());

    m_combo->Insert("item with bitmap", bitmap, 1);

    CHECK(m_combo->GetItemBitmap(1).IsOk());

    m_combo->SetItemBitmap(0, bitmap);

    CHECK(m_combo->GetItemBitmap(0).IsOk());

    CHECK(m_combo->GetBitmapSize() == m_combo->FromDIP(wxSize(16, 16)));

    m_combo->SetSelection( 1 );

    CHECK( "item with bitmap" == m_combo->GetStringSelection() );
}

#endif //wxUSE_BITMAPCOMBOBOX
