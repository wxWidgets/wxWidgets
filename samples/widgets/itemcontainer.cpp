/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        itemcontainer.cpp
// Purpose:     Part of the widgets sample showing wxComboBox
// Created:     20.07.07
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/event.h"
#endif

#include "wx/ctrlsub.h"
#include "itemcontainer.h"


// Help track client data objects in wxItemContainer instances.
class TrackedClientData : public wxClientData
{
public:

    TrackedClientData(ItemContainerWidgetsPage* tracker, int value)
        : m_tracker(tracker),
          m_value(value)
    {
        m_tracker->StartTrackingData();
    }

    virtual ~TrackedClientData()
    {
        m_tracker->StopTrackingData();
    }

    int GetValue() const
    {
        return m_value;
    }

private:
    ItemContainerWidgetsPage *m_tracker;
    int m_value;

    wxDECLARE_NO_COPY_CLASS(TrackedClientData);
};

// ============================================================================
// implementation
// ============================================================================

ItemContainerWidgetsPage::ItemContainerWidgetsPage(WidgetsBookCtrl *book,
                                                   wxImageList *image_list,
                                                   const char *const icon[])
: WidgetsPage(book, image_list, icon)
#if defined(__WXMSW__) || defined(__WXGTK__)
// Reference data needs to be sorted in a dictionary order
// since control's items are sorted in this order too.
, m_itemsSorted(wxDictionaryStringSortAscending)
#endif // __WXMSW__ || __WXGTK__
, m_trackedDataObjects(0)
{
    m_items.Add("This");
    m_items.Add("is");
    m_items.Add("a");
    m_items.Add("List");
    m_items.Add("of");
    m_items.Add("strings");
    m_itemsSorted = m_items;
}

ItemContainerWidgetsPage::~ItemContainerWidgetsPage()
{
}

wxClientData* ItemContainerWidgetsPage::CreateClientData(int value)
{
    return new TrackedClientData(this, value);
}

void ItemContainerWidgetsPage::StartTrackingData()
{
    ++m_trackedDataObjects;
}

void ItemContainerWidgetsPage::StopTrackingData()
{
    --m_trackedDataObjects;
}

bool ItemContainerWidgetsPage::VerifyAllClientDataDestroyed()
{
    if ( m_trackedDataObjects )
    {
        wxString message = "Bug in managing wxClientData: ";
        if ( m_trackedDataObjects > 0 )
            message << m_trackedDataObjects << " lost objects";
        else
            message << (-m_trackedDataObjects) << " extra deletes";
        wxFAIL_MSG(message);
        return false;
    }

    return true;
}

void ItemContainerWidgetsPage::StartTest(const wxString& label)
{
    m_container->Clear();
    wxLogMessage("Test - %s:", label);
}

void ItemContainerWidgetsPage::EndTest(const wxArrayString& items)
{
    const unsigned count = m_container->GetCount();

    bool ok = count == items.GetCount();
    if ( !ok )
    {
        wxFAIL_MSG("Item count does not match.");
    }
    else
    {
        for ( unsigned i = 0; i < count; ++i )
        {
            wxString str = m_container->GetString(i);
            if ( str != items[i] )
            {
                wxFAIL_MSG(wxString::Format(
                            "Wrong string \"%s\" at position %d (expected \"%s\")",
                           str, i, items[i]));
                ok = false;
                break;
            }

            if ( m_container->HasClientUntypedData() )
            {
                void *data = m_container->GetClientData(i);
                if ( data && !VerifyClientData((wxUIntPtr)data, str) )
                {
                    ok = false;
                    break;
                }
            }
            else if ( m_container->HasClientObjectData() )
            {
                TrackedClientData* obj = (TrackedClientData*)m_container->GetClientObject(i);
                if ( obj && !VerifyClientData(obj->GetValue(), str) )
                {
                    ok = false;
                    break;
                }
            }
        }

        if ( !ok )
        {
            wxLogMessage(DumpContainerData(items));
        }
    }

    m_container->Clear();
    ok &= VerifyAllClientDataDestroyed();

    wxLogMessage("...%s", ok ? "passed" : "failed");
}

wxString
ItemContainerWidgetsPage::DumpContainerData(const wxArrayString& expected) const
{
    wxString str;
    str << "Current content:\n";

    unsigned i;
    for ( i = 0; i < m_container->GetCount(); ++i )
    {
        str << " - " << m_container->GetString(i) << " [";
        if ( m_container->HasClientObjectData() )
        {
            TrackedClientData *
                obj = (TrackedClientData*)m_container->GetClientObject(i);
            if ( obj )
                str << obj->GetValue();
        }
        else if ( m_container->HasClientUntypedData() )
        {
            void *data = m_container->GetClientData(i);
            if ( data )
                str << (wxUIntPtr)data;
        }
        str << "]\n";
    }

    str << "Expected content:\n";
    for ( i = 0; i < expected.GetCount(); ++i )
    {
        const wxString& item = expected[i];
        str << " - " << item << "[";
        for( unsigned j = 0; j < m_items.GetCount(); ++j )
        {
            if ( m_items[j] == item )
                str << j;
        }
        str << "]\n";
    }

    return str;
}

bool ItemContainerWidgetsPage::VerifyClientData(wxUIntPtr i, const wxString& str)
{
    if ( i > m_items.GetCount() || m_items[i] != str )
    {
        wxLogMessage("Client data for '%s' does not match.", str);
        return false;
    }

    return true;
}

/* static */
wxArrayString
ItemContainerWidgetsPage::MakeArray(const wxSortedArrayString& sorted)
{
    wxArrayString a;

    const size_t count = sorted.size();
    a.reserve(count);
    for ( size_t n = 0; n < count; n++ )
        a.push_back(sorted[n]);

    return a;
}

void ItemContainerWidgetsPage::OnButtonTestItemContainer(wxCommandEvent&)
{
    m_container = GetContainer();
    wxASSERT_MSG(m_container, "Widget must have a test widget");

    wxLogMessage("wxItemContainer test for %s, %s:",
                 GetWidget()->GetClassInfo()->GetClassName(),
                 (m_container->IsSorted() ? "Sorted" : "Unsorted"));

    const wxArrayString
        expected_result = m_container->IsSorted() ? MakeArray(m_itemsSorted)
                                                  : m_items;

    StartTest("Append one item");
    wxString item = m_items[0];
    m_container->Append(item);
    EndTest(wxArrayString(1, &item));

    StartTest("Append some items");
    m_container->Append(m_items);
    EndTest(expected_result);

    StartTest("Append some items with data objects");
    wxClientData **objects = new wxClientData *[m_items.GetCount()];
    for ( unsigned i = 0; i < m_items.GetCount(); ++i )
        objects[i] = CreateClientData(i);
    m_container->Append(m_items, objects);
    EndTest(expected_result);
    delete[] objects;

    StartTest("Append some items with data");
    void **data = new void *[m_items.GetCount()];
    for ( unsigned i = 0; i < m_items.GetCount(); ++i )
        data[i] = wxUIntToPtr(i);
    m_container->Append(m_items, data);
    EndTest(expected_result);
    delete[] data;

    StartTest("Append some items with data, one by one");
    for ( unsigned i = 0; i < m_items.GetCount(); ++i )
        m_container->Append(m_items[i], wxUIntToPtr(i));
    EndTest(expected_result);

    StartTest("Append some items with data objects, one by one");
    for ( unsigned i = 0; i < m_items.GetCount(); ++i )
        m_container->Append(m_items[i], CreateClientData(i));
    EndTest(expected_result);

    if ( !m_container->IsSorted() )
    {
        StartTest("Insert in reverse order with data, one by one");
        for ( unsigned i = m_items.GetCount(); i; --i )
            m_container->Insert(m_items[i - 1], 0, wxUIntToPtr(i - 1));
        EndTest(expected_result);
    }
}

