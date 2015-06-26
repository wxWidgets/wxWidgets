/////////////////////////////////////////////////////////////////////////////
// Name:        samples/propgrid/tests.cpp
// Purpose:     wxPropertyGrid tests
// Author:      Jaakko Salli
// Modified by:
// Created:     2007-05-16
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>

#include "propgrid.h"
#include "sampleprops.h"


// -----------------------------------------------------------------------
// wxTestCustomFlagsProperty
// -----------------------------------------------------------------------

//
// Test customizing wxColourProperty via subclassing
//
// * Includes custom colour entry.
// * Includes extra custom entry.
//
class MyColourProperty : public wxColourProperty
{
public:
    MyColourProperty( const wxString& label = wxPG_LABEL,
                       const wxString& name = wxPG_LABEL,
                       const wxColour& value = *wxWHITE )
        : wxColourProperty(label, name, value)
    {
        wxPGChoices colours;
        colours.Add(wxT("White"));
        colours.Add(wxT("Black"));
        colours.Add(wxT("Red"));
        colours.Add(wxT("Green"));
        colours.Add(wxT("Blue"));
        colours.Add(wxT("Custom"));
        colours.Add(wxT("None"));
        m_choices = colours;
        SetIndex(0);
        wxVariant variant;
        variant << value;
        SetValue(variant);
    }

    virtual ~MyColourProperty()
    {
    }

    virtual wxColour GetColour( int index ) const wxOVERRIDE
    {
        switch (index)
        {
            case 0: return *wxWHITE;
            case 1: return *wxBLACK;
            case 2: return *wxRED;
            case 3: return *wxGREEN;
            case 4: return *wxBLUE;
            case 5:
                // Return current colour for the custom entry
                wxColour col;
                if ( GetIndex() == GetCustomColourIndex() )
                {
                    if ( m_value.IsNull() )
                        return col;
                    col << m_value;
                    return col;
                }
                return *wxWHITE;
        };
        return wxColour();
    }

    virtual wxString ColourToString( const wxColour& col,
                                     int index,
                                     int argFlags = 0 ) const wxOVERRIDE
    {
        if ( index == (int)(m_choices.GetCount()-1) )
            return wxEmptyString;

        return wxColourProperty::ColourToString(col, index, argFlags);
    }

    virtual int GetCustomColourIndex() const wxOVERRIDE
    {
        return m_choices.GetCount()-2;
    }
};


void FormMain::AddTestProperties( wxPropertyGridPage* pg )
{
    pg->Append( new MyColourProperty(wxT("CustomColourProperty"), wxPG_LABEL, *wxGREEN) );
    pg->GetProperty(wxT("CustomColourProperty"))->SetAutoUnspecified(true);
    pg->SetPropertyEditor( wxT("CustomColourProperty"), wxPGEditor_ComboBox );

    pg->SetPropertyHelpString(wxT("CustomColourProperty"),
        wxT("This is a MyColourProperty from the sample app. ")
        wxT("It is built by subclassing wxColourProperty."));
}

// -----------------------------------------------------------------------

void FormMain::OnDumpList( wxCommandEvent& WXUNUSED(event) )
{
    wxVariant values = m_pPropGridManager->GetPropertyValues(wxT("list"), wxNullProperty, wxPG_INC_ATTRIBUTES);
    wxString text = wxT("This only tests that wxVariant related routines do not crash.");
    wxString t;

    wxDialog* dlg = new wxDialog(this,wxID_ANY,wxT("wxVariant Test"),
        wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

    unsigned int i;
    for ( i = 0; i < (unsigned int)values.GetCount(); i++ )
    {
        wxVariant& v = values[i];

        wxString strValue = v.GetString();

#if wxCHECK_VERSION(2,8,0)
        if ( v.GetName().EndsWith(wxT("@attr")) )
#else
        if ( v.GetName().Right(5) == wxT("@attr") )
#endif
        {
            text += wxString::Format(wxT("Attributes:\n"));

            unsigned int n;
            for ( n = 0; n < (unsigned int)v.GetCount(); n++ )
            {
                wxVariant& a = v[n];

                t.Printf(wxT("  attribute %i: name=\"%s\"  (type=\"%s\"  value=\"%s\")\n"),(int)n,
                    a.GetName().c_str(),a.GetType().c_str(),a.GetString().c_str());
                text += t;
            }
        }
        else
        {
            t.Printf(wxT("%i: name=\"%s\"  type=\"%s\"  value=\"%s\"\n"),(int)i,
                v.GetName().c_str(),v.GetType().c_str(),strValue.c_str());
            text += t;
        }
    }

    // multi-line text editor dialog
    const int spacing = 8;
    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* rowsizer = new wxBoxSizer( wxHORIZONTAL );
    wxTextCtrl* ed = new wxTextCtrl(dlg, wxID_ANY, text,
                                    wxDefaultPosition, wxDefaultSize,
                                    wxTE_MULTILINE|wxTE_READONLY);
    rowsizer->Add( ed, wxSizerFlags(1).Expand().Border(wxALL, spacing));
    topsizer->Add( rowsizer, wxSizerFlags(1).Expand());
    rowsizer = new wxBoxSizer( wxHORIZONTAL );
    rowsizer->Add( new wxButton(dlg,wxID_OK,wxT("Ok")),
        wxSizerFlags(0).CentreHorizontal().CentreVertical().Border(wxBOTTOM|wxLEFT|wxRIGHT, spacing));
    topsizer->Add( rowsizer, wxSizerFlags().Right() );

    dlg->SetSizer( topsizer );
    topsizer->SetSizeHints( dlg );

    dlg->SetSize(400,300);
    dlg->Centre();
    dlg->ShowModal();
}

// -----------------------------------------------------------------------

class TestRunner
{
public:

    TestRunner( const wxString& name, wxPropertyGridManager* man, wxTextCtrl* ed, wxArrayString* errorMessages )
    {
        m_name = name;
        m_man = man;
        m_ed = ed;
        m_errorMessages = errorMessages;
#ifdef __WXDEBUG__
        m_preWarnings = wxPGGlobalVars->m_warnings;
#endif

        if ( name != wxT("none") )
            Msg(name+wxT("\n"));
    }

    ~TestRunner()
    {
#ifdef __WXDEBUG__
        int warningsOccurred = wxPGGlobalVars->m_warnings - m_preWarnings;
        if ( warningsOccurred )
        {
            wxString s = wxString::Format(wxT("%i warnings occurred during test '%s'"), warningsOccurred, m_name.c_str());
            m_errorMessages->push_back(s);
            Msg(s);
        }
#endif
    }

    void Msg( const wxString& text )
    {
        if ( m_ed )
        {
            m_ed->AppendText(text);
            m_ed->AppendText(wxT("\n"));
        }
        wxLogDebug(text);
    }

protected:
    wxPropertyGridManager* m_man;
    wxTextCtrl* m_ed;
    wxArrayString* m_errorMessages;
    wxString m_name;
#ifdef __WXDEBUG__
    int m_preWarnings;
#endif
};


#define RT_START_TEST(TESTNAME) \
    TestRunner tr(wxT(#TESTNAME), pgman, ed, &errorMessages);

#define RT_MSG(S) \
    tr.Msg(S);

#define RT_FAILURE() \
    { \
        wxString s1 = wxString::Format(wxT("Test failure in tests.cpp, line %i."),__LINE__-1); \
        errorMessages.push_back(s1); \
        wxLogDebug(s1); \
        failures++; \
    }

#define RT_ASSERT(COND) \
    if (!(COND)) \
        RT_FAILURE()

#define RT_FAILURE_MSG(MSG) \
    { \
        wxString s1 = wxString::Format(wxT("Test failure in tests.cpp, line %i."),__LINE__-1); \
        errorMessages.push_back(s1); \
        wxLogDebug(s1); \
        wxString s2 = wxString::Format(wxT("Message: %s"),MSG.c_str()); \
        errorMessages.push_back(s2); \
        wxLogDebug(s2); \
        failures++; \
    }

#define RT_VALIDATE_VIRTUAL_HEIGHT(PROPS, EXTRATEXT) \
    { \
        unsigned int h1_ = PROPS->GetVirtualHeight(); \
        unsigned int h2_ = PROPS->GetActualVirtualHeight(); \
        if ( h1_ != h2_ ) \
        { \
            wxString s_ = wxString::Format(wxT("VirtualHeight = %i, should be %i (%s)"), h1_, h2_, EXTRATEXT.c_str()); \
            RT_FAILURE_MSG(s_); \
            _failed_ = true; \
        } \
        else \
        { \
            _failed_ = false; \
        } \
    }

inline bool GetRandomBooleanVal()
{
    return (rand() % 2) != 0;
}

int gpiro_cmpfunc(const void* a, const void* b)
{
    const wxPGProperty* p1 = (const wxPGProperty*) a;
    const wxPGProperty* p2 = (const wxPGProperty*) b;
    return (int) (((size_t)p1->GetClientData()) - ((size_t)p2->GetClientData()));
}

wxArrayPGProperty GetPropertiesInRandomOrder( wxPropertyGridInterface* props, int iterationFlags = wxPG_ITERATE_ALL )
{
    wxArrayPGProperty arr;

    wxPropertyGridIterator it;

    for ( it = props->GetIterator(iterationFlags);
          !it.AtEnd();
          ++it )
    {
        wxPGProperty* p = *it;
        size_t randomNumber = rand();
        p->SetClientData(reinterpret_cast<void*>(randomNumber));
        arr.push_back(p);
    }

    wxPGProperty** firstEntry = &arr[0];
    qsort(firstEntry, arr.size(), sizeof(wxPGProperty*), gpiro_cmpfunc);

    return arr;
}

// Callback for testing property sorting
int MyPropertySortFunction(wxPropertyGrid* WXUNUSED(propGrid),
                           wxPGProperty* p1,
                           wxPGProperty* p2)
{
    // Reverse alphabetical order
    return p2->GetLabel().CmpNoCase( p1->GetBaseName() );
}

bool FormMain::RunTests( bool fullTest, bool interactive )
{
    wxString t;

    wxPropertyGridManager* pgman = m_pPropGridManager;
    wxPropertyGrid* pg;

    size_t i;

    pgman->ClearSelection();

    srand((unsigned int)(time(NULL) % UINT_MAX));

    int failures = 0;
    bool _failed_ = false;
    wxArrayString errorMessages;
    wxDialog* dlg = NULL;

    dlg = new wxDialog(this,wxID_ANY,wxT("wxPropertyGrid Regression Tests"),
        wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

    // multi-line text editor dialog
    const int spacing = 8;
    wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* rowsizer = new wxBoxSizer( wxHORIZONTAL );
    wxTextCtrl* ed = new wxTextCtrl(dlg, wxID_ANY, wxEmptyString,
                                    wxDefaultPosition, wxDefaultSize,
                                    wxTE_MULTILINE|wxTE_READONLY);
    rowsizer->Add( ed, wxSizerFlags(1).Expand().Border(wxALL, spacing));
    topsizer->Add( rowsizer, wxSizerFlags(1).Expand());
    rowsizer = new wxBoxSizer( wxHORIZONTAL );
    rowsizer->Add( new wxButton(dlg,wxID_OK,wxT("Ok")),
        wxSizerFlags(0).CentreHorizontal().CentreVertical().Border(wxBOTTOM|wxLEFT|wxRIGHT, spacing));
    topsizer->Add( rowsizer, wxSizerFlags().Right() );

    dlg->SetSizer( topsizer );
    topsizer->SetSizeHints( dlg );

    dlg->SetSize(400,300);
    dlg->Move(wxSystemSettings::GetMetric(wxSYS_SCREEN_X)-dlg->GetSize().x,
              wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)-dlg->GetSize().y);
    dlg->Show();

    {
        //
        // Basic iterator tests
        RT_START_TEST(GetIterator)

        wxPGVIterator it;
        int count;

        count = 0;
        for ( it = pgman->GetVIterator(wxPG_ITERATE_PROPERTIES);
              !it.AtEnd();
              it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            if ( p->IsCategory() )
                RT_FAILURE_MSG(wxString::Format(wxT("'%s' is a category (non-private child property expected)"),p->GetLabel().c_str()))
            else if ( p->GetParent()->HasFlag(wxPG_PROP_AGGREGATE) )
                RT_FAILURE_MSG(wxString::Format(wxT("'%s' is a private child (non-private child property expected)"),p->GetLabel().c_str()))
            count++;
        }

        RT_MSG(wxString::Format(wxT("GetVIterator(wxPG_ITERATE_PROPERTIES) -> %i entries"), count));

        count = 0;
        for ( it = pgman->GetVIterator(wxPG_ITERATE_CATEGORIES);
              !it.AtEnd();
              it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            if ( !p->IsCategory() )
                RT_FAILURE_MSG(wxString::Format(wxT("'%s' is not a category (only category was expected)"),p->GetLabel().c_str()))
            count++;
        }

        RT_MSG(wxString::Format(wxT("GetVIterator(wxPG_ITERATE_CATEGORIES) -> %i entries"), count));

        count = 0;
        for ( it = pgman->GetVIterator(wxPG_ITERATE_PROPERTIES|wxPG_ITERATE_CATEGORIES);
              !it.AtEnd();
              it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            if ( p->GetParent()->HasFlag(wxPG_PROP_AGGREGATE) )
                RT_FAILURE_MSG(wxString::Format(wxT("'%s' is a private child (non-private child property or category expected)"),p->GetLabel().c_str()))
            count++;
        }

        RT_MSG(wxString::Format(wxT("GetVIterator(wxPG_ITERATE_PROPERTIES|wxPG_ITERATE_CATEGORIES) -> %i entries"), count));

        count = 0;
        for ( it = pgman->GetVIterator(wxPG_ITERATE_VISIBLE);
              !it.AtEnd();
              it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            if ( (p->GetParent() != p->GetParentState()->DoGetRoot() && !p->GetParent()->IsExpanded()) )
                RT_FAILURE_MSG(wxString::Format(wxT("'%s' had collapsed parent (only visible properties expected)"),p->GetLabel().c_str()))
            else if ( p->HasFlag(wxPG_PROP_HIDDEN) )
                RT_FAILURE_MSG(wxString::Format(wxT("'%s' was hidden (only visible properties expected)"),p->GetLabel().c_str()))
            count++;
        }

        RT_MSG(wxString::Format(wxT("GetVIterator(wxPG_ITERATE_VISIBLE) -> %i entries"), count));
    }

    if ( fullTest )
    {
        // Test that setting focus to properties does not crash things
        RT_START_TEST(SelectProperty)

        wxPropertyGridIterator it;
        size_t ind;

        for ( ind=0; ind<pgman->GetPageCount(); ind++ )
        {
            wxPropertyGridPage* page = pgman->GetPage(ind);
            pgman->SelectPage(page);

            for ( it = page->GetIterator(wxPG_ITERATE_VISIBLE);
                  !it.AtEnd();
                  ++it )
            {
                wxPGProperty* p = *it;
                RT_MSG(p->GetLabel());
                pgman->GetGrid()->SelectProperty(p, true);
                ::wxMilliSleep(150);
                Update();
            }
        }
    }

    {
        //
        // Delete everything in reverse order
        RT_START_TEST(DeleteProperty)

        wxPGVIterator it;
        wxArrayPGProperty array;

        for ( it = pgman->GetVIterator(wxPG_ITERATE_ALL&~(wxPG_IT_CHILDREN(wxPG_PROP_AGGREGATE)));
              !it.AtEnd();
              it.Next() )
            array.push_back(it.GetProperty());

        wxArrayPGProperty::reverse_iterator it2;

        for ( it2 = array.rbegin(); it2 != array.rend(); ++it2 )
        {
            wxPGProperty* p = (wxPGProperty*)*it2;
            RT_MSG(wxString::Format(wxT("Deleting '%s' ('%s')"),p->GetLabel().c_str(),p->GetName().c_str()));
            pgman->DeleteProperty(p);
        }

        // Check if grid is empty.
        it = pgman->GetVIterator(wxPG_ITERATE_ALL&~(wxPG_IT_CHILDREN(wxPG_PROP_AGGREGATE)));
        if ( !it.AtEnd() )
        {
            RT_FAILURE_MSG(wxString(wxS("Not all properties are deleted")));
        }

        // Recreate grid
        CreateGrid( -1, -1 );
        pgman = m_pPropGridManager;
    }

    {
        //
        // Test property default values
        RT_START_TEST(Default_Values)

        wxPGVIterator it;

        for ( it = pgman->GetVIterator(wxPG_ITERATE_PROPERTIES);
              !it.AtEnd();
              it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            pgman->SetPropertyValue(p, p->GetDefaultValue());
        }

        // Recreate grid
        CreateGrid( -1, -1 );
        pgman = m_pPropGridManager;
    }

    {
        //
        // Test wxAny<->wxVariant conversion
        RT_START_TEST(WXVARIANT_TO_WXANY_CONVERSION)

        wxPGProperty* prop;
        wxAny any;

#if wxUSE_DATETIME
        prop = pgman->GetProperty(wxT("DateProperty"));
        wxDateTime testTime = wxDateTime::Now();
        any = testTime;
        prop->SetValue(any);
        if ( prop->GetValue().GetAny().As<wxDateTime>() != testTime )
            RT_FAILURE();
#endif

        prop = pgman->GetProperty(wxT("IntProperty"));
        int testInt = 25537983;
        any = testInt;
        prop->SetValue(any);
        if ( prop->GetValue().GetAny().As<int>() != testInt )
            RT_FAILURE();
#ifdef wxLongLong_t
        if ( prop->GetValue().GetAny().As<wxLongLong_t>() != testInt )
            RT_FAILURE();
#endif

        prop = pgman->GetProperty(wxT("StringProperty"));
        wxString testString = wxT("asd934jfyn3");
        any = testString;
        prop->SetValue(any);
        if ( prop->GetValue().GetAny().As<wxString>() != testString )
            RT_FAILURE();

        // Test with a type generated with IMPLEMENT_VARIANT_OBJECT()
        prop = pgman->GetProperty(wxT("ColourProperty"));
        wxColour testCol = *wxCYAN;
        any = testCol;
        prop->SetValue(any);
        if ( prop->GetValue().GetAny().As<wxColour>() != testCol )
            RT_FAILURE();

        // Test with a type with custom wxVariantData defined by
        // wxPG headers.
        prop = pgman->GetProperty(wxT("Position"));
        wxPoint testPoint(199, 199);
        any = testPoint;
        prop->SetValue(any);
        if ( prop->GetValue().GetAny().As<wxPoint>() != testPoint )
            RT_FAILURE();
    }

    {
        RT_START_TEST(GetPropertyValues)

        for ( i=0; i<3; i++ )
        {
            wxString text;

            wxPropertyGridPage* page = pgman->GetPage(i);

            wxVariant values = page->GetPropertyValues();

            unsigned int i;
            for ( i = 0; i < (unsigned int)values.GetCount(); i++ )
            {
                wxVariant& v = values[i];

                t.Printf(wxT("%i: name=\"%s\"  type=\"%s\"\n"),(int)i,
                    v.GetName().c_str(),v.GetType().c_str());

                text += t;
            }
            ed->AppendText(text);
        }
    }

    {
        RT_START_TEST(SetPropertyValue_and_GetPropertyValue)

        // In this section, mixed up usage of wxT("propname") and "propname"
        // in wxPropertyGridInterface functions is intentional.
        // Purpose is to test wxPGPropArgCls ctors.

        //pg = (wxPropertyGrid*) NULL;

        wxArrayString test_arrstr_1;
        test_arrstr_1.Add(wxT("Apple"));
        test_arrstr_1.Add(wxT("Orange"));
        test_arrstr_1.Add(wxT("Lemon"));

        wxArrayString test_arrstr_2;
        test_arrstr_2.Add(wxT("Potato"));
        test_arrstr_2.Add(wxT("Cabbage"));
        test_arrstr_2.Add(wxT("Cucumber"));

        wxArrayInt test_arrint_1;
        test_arrint_1.Add(1);
        test_arrint_1.Add(2);
        test_arrint_1.Add(3);

        wxArrayInt test_arrint_2;
        test_arrint_2.Add(0);
        test_arrint_2.Add(1);
        test_arrint_2.Add(4);

#if wxUSE_DATETIME
        wxDateTime dt1 = wxDateTime::Now();
        dt1.SetYear(dt1.GetYear()-1);

        wxDateTime dt2 = wxDateTime::Now();
        dt2.SetYear(dt2.GetYear()-10);
#endif

        wxColour colWithAlpha(1, 128, 254, 100);
        wxString colWithAlphaStr(colWithAlpha.GetAsString(wxC2S_CSS_SYNTAX));

#define FLAG_TEST_SET1 (wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxRESIZE_BORDER)
#define FLAG_TEST_SET2 (wxSTAY_ON_TOP|wxCAPTION|wxICONIZE|wxSYSTEM_MENU)

        pgman->SetPropertyValue(wxT("StringProperty"),wxT("Text1"));
        pgman->SetPropertyValue(wxT("IntProperty"),1024);
        pgman->SetPropertyValue(wxT("FloatProperty"),1024.0000000001);
        pgman->SetPropertyValue(wxT("BoolProperty"),false);
        pgman->SetPropertyValue(wxT("EnumProperty"),120);
        pgman->SetPropertyValue(wxT("ArrayStringProperty"),test_arrstr_1);
        wxColour emptyCol;
        pgman->SetPropertyValue(wxT("ColourProperty"),emptyCol);
        pgman->SetPropertyValue(wxT("ColourProperty"),(wxObject*)wxBLACK);
        pgman->SetPropertyValue(wxT("Size"),WXVARIANT(wxSize(150,150)));
        pgman->SetPropertyValue(wxT("Position"),WXVARIANT(wxPoint(150,150)));
        pgman->SetPropertyValue(wxT("MultiChoiceProperty"),test_arrint_1);
#if wxUSE_DATETIME
        pgman->SetPropertyValue(wxT("DateProperty"),dt1);
#endif

        pgman->SelectPage(2);
        pg = pgman->GetGrid();

        if ( pg->GetPropertyValueAsString(wxT("StringProperty")) != wxT("Text1") )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsInt(wxT("IntProperty")) != 1024 )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsDouble(wxT("FloatProperty")) != 1024.0000000001 )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsBool(wxT("BoolProperty")) != false )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsLong(wxT("EnumProperty")) != 120 )
            RT_FAILURE();
        if ( pg->GetPropertyValueAsArrayString(wxT("ArrayStringProperty")) != test_arrstr_1 )
            RT_FAILURE();
        wxColour col;
        col << pgman->GetPropertyValue(wxT("ColourProperty"));
        if ( col != *wxBLACK )
            RT_FAILURE();
        wxVariant varSize(pg->GetPropertyValue(wxT("Size")));
        if ( wxSizeRefFromVariant(varSize) != wxSize(150,150) )
            RT_FAILURE();
        wxVariant varPos(pg->GetPropertyValue(wxT("Position")));
        if ( wxPointRefFromVariant(varPos) != wxPoint(150,150) )
            RT_FAILURE();
        if ( !(pg->GetPropertyValueAsArrayInt(wxT("MultiChoiceProperty")) == test_arrint_1) )
            RT_FAILURE();
#if wxUSE_DATETIME
        if ( !(pg->GetPropertyValueAsDateTime(wxT("DateProperty")) == dt1) )
            RT_FAILURE();
#endif

#if wxUSE_LONGLONG && defined(wxLongLong_t)
        pgman->SetPropertyValue(wxT("IntProperty"),wxLL(10000000000));
        if ( pg->GetPropertyValueAsLongLong(wxT("IntProperty")) != wxLL(10000000000) )
            RT_FAILURE();
#else
        pgman->SetPropertyValue(wxT("IntProperty"),1000000000);
        if ( pg->GetPropertyValueAsLong(wxT("IntProperty")) != 1000000000 )
            RT_FAILURE();
#endif

        pg->SetPropertyValue(wxT("StringProperty"),wxT("Text2"));
        pg->SetPropertyValue(wxT("IntProperty"),512);
        pg->SetPropertyValue(wxT("FloatProperty"),512.0);
        pg->SetPropertyValue(wxT("BoolProperty"),true);
        pg->SetPropertyValue(wxT("EnumProperty"),80);
        pg->SetPropertyValue(wxT("ArrayStringProperty"),test_arrstr_2);
        pg->SetPropertyValue(wxT("ColourProperty"),(wxObject*)wxWHITE);
        pg->SetPropertyValue(wxT("Size"),WXVARIANT(wxSize(300,300)));
        pg->SetPropertyValue(wxT("Position"),WXVARIANT(wxPoint(300,300)));
        pg->SetPropertyValue(wxT("MultiChoiceProperty"),test_arrint_2);
#if wxUSE_DATETIME
        pg->SetPropertyValue(wxT("DateProperty"),dt2);
#endif

        //pg = (wxPropertyGrid*) NULL;

        pgman->SelectPage(0);

        if ( pgman->GetPropertyValueAsString(wxT("StringProperty")) != wxT("Text2") )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsInt(wxT("IntProperty")) != 512 )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsDouble(wxT("FloatProperty")) != 512.0 )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsBool(wxT("BoolProperty")) != true )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsLong(wxT("EnumProperty")) != 80 )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsArrayString(wxT("ArrayStringProperty")) != test_arrstr_2 )
            RT_FAILURE();
        col << pgman->GetPropertyValue(wxT("ColourProperty"));
        if ( col != *wxWHITE )
            RT_FAILURE();
        varSize = pgman->GetPropertyValue(wxT("Size"));
        if ( wxSizeRefFromVariant(varSize) != wxSize(300,300) )
            RT_FAILURE();
        varPos = pgman->GetPropertyValue(wxT("Position"));
        if ( wxPointRefFromVariant(varPos) != wxPoint(300,300) )
            RT_FAILURE();
        if ( !(pgman->GetPropertyValueAsArrayInt(wxT("MultiChoiceProperty")) == test_arrint_2) )
            RT_FAILURE();
#if wxUSE_DATETIME
        if ( !(pgman->GetPropertyValueAsDateTime(wxT("DateProperty")) == dt2) )
            RT_FAILURE();
#endif

#if wxUSE_LONGLONG && defined(wxLongLong_t)
        pgman->SetPropertyValue(wxT("IntProperty"),wxLL(-80000000000));
        if ( pgman->GetPropertyValueAsLongLong(wxT("IntProperty")) != wxLL(-80000000000) )
            RT_FAILURE();
#else
        pgman->SetPropertyValue(wxT("IntProperty"),-1000000000);
        if ( pgman->GetPropertyValueAsLong(wxT("IntProperty")) != -1000000000 )
            RT_FAILURE();
#endif

        // Make sure children of composite parent get updated as well
        // Original string value: "Lamborghini Diablo SV; 5707; [300; 3.9; 8.6] 300000; Not Convertible"

        //
        // This updates children as well
        wxString nvs = wxT("Lamborghini Diablo XYZ; 5707; [100; 3.9; 8.6] 3000002; Convertible");
        pgman->SetPropertyValue("Car", nvs);

        if ( pgman->GetPropertyValueAsString(wxT("Car.Model")) != wxT("Lamborghini Diablo XYZ") )
        {
            RT_FAILURE_MSG(wxString::Format(wxS("Did not match: Car.Model=%s"), pgman->GetPropertyValueAsString(wxS("Car.Model")).c_str()));
        }

        if ( pgman->GetPropertyValueAsInt(wxT("Car.Speeds.Max. Speed (mph)")) != 100 )
        {
            RT_FAILURE_MSG(wxString::Format(wxS("Did not match: Car.Speeds.Max. Speed (mph)=%s"), pgman->GetPropertyValueAsString(wxS("Car.Speeds.Max. Speed (mph)")).c_str()));
        }

        if ( pgman->GetPropertyValueAsInt(wxT("Car.Price ($)")) != 3000002 )
        {
            RT_FAILURE_MSG(wxString::Format(wxS("Did not match: Car.Price ($)=%s"), pgman->GetPropertyValueAsString(wxS("Car.Price ($)")).c_str()));
        }

        if ( !pgman->GetPropertyValueAsBool(wxT("Car.Convertible")) )
        {
            RT_FAILURE_MSG(wxString::Format(wxS("Did not match: Car.Convertible=%s"), pgman->GetPropertyValueAsString(wxS("Car.Convertible")).c_str()));
        }

        // SetPropertyValueString for special cases such as wxColour
        pgman->SetPropertyValueString(wxT("ColourProperty"), wxT("(123,4,255)"));
        col << pgman->GetPropertyValue(wxT("ColourProperty"));
        if ( col != wxColour(123, 4, 255) )
            RT_FAILURE();
        pgman->SetPropertyValueString(wxT("ColourProperty"), wxT("#FE860B"));
        col << pgman->GetPropertyValue(wxT("ColourProperty"));
        if ( col != wxColour(254, 134, 11) )
            RT_FAILURE();

        pgman->SetPropertyValueString(wxT("ColourPropertyWithAlpha"),
                                      wxT("(10, 20, 30, 128)"));
        col << pgman->GetPropertyValue(wxT("ColourPropertyWithAlpha"));
        if ( col != wxColour(10, 20, 30, 128) )
            RT_FAILURE();
        if ( pgman->GetPropertyValueAsString(wxT("ColourPropertyWithAlpha"))
                != wxT("(10,20,30,128)") )
            RT_FAILURE();
    }

    {
        RT_START_TEST(SetPropertyValueUnspecified)

        // Null variant setter tests
        pgman->SetPropertyValueUnspecified(wxT("StringProperty"));
        pgman->SetPropertyValueUnspecified(wxT("IntProperty"));
        pgman->SetPropertyValueUnspecified(wxT("FloatProperty"));
        pgman->SetPropertyValueUnspecified(wxT("BoolProperty"));
        pgman->SetPropertyValueUnspecified(wxT("EnumProperty"));
        pgman->SetPropertyValueUnspecified(wxT("ArrayStringProperty"));
        pgman->SetPropertyValueUnspecified(wxT("ColourProperty"));
        pgman->SetPropertyValueUnspecified(wxT("Size"));
        pgman->SetPropertyValueUnspecified(wxT("Position"));
        pgman->SetPropertyValueUnspecified(wxT("MultiChoiceProperty"));
#if wxUSE_DATETIME
        pgman->SetPropertyValueUnspecified(wxT("DateProperty"));
#endif
    }

    {
        //
        // Test multiple selection
        RT_START_TEST(MULTIPLE_SELECTION)
        if ( !(pgman->GetExtraStyle() & wxPG_EX_MULTIPLE_SELECTION) )
            CreateGrid( -1, wxPG_EX_MULTIPLE_SELECTION);
        pgman = m_pPropGridManager;

        wxPropertyGrid* pg = pgman->GetGrid();

        wxPGProperty* prop1 = pg->GetProperty(wxT("Label"));
        wxPGProperty* prop2 = pg->GetProperty(wxT("Cell Text Colour"));
        wxPGProperty* prop3 = pg->GetProperty(wxT("Height"));
        wxPGProperty* catProp = pg->GetProperty(wxT("Appearance"));

        RT_ASSERT( prop1 && prop2 && prop3 );

        pg->ClearSelection();
        pg->AddToSelection(prop1);
        pg->AddToSelection(prop2);
        pg->AddToSelection(prop3);

        // Adding category to selection should fail silently
        pg->AddToSelection(catProp);

        wxArrayPGProperty selectedProperties = pg->GetSelectedProperties();

        RT_ASSERT( selectedProperties.size() == 3 )
        RT_ASSERT( pg->IsPropertySelected(prop1) )
        RT_ASSERT( pg->IsPropertySelected(prop2) )
        RT_ASSERT( pg->IsPropertySelected(prop3) )
        RT_ASSERT( !pg->IsPropertySelected(catProp) )

        pg->RemoveFromSelection(prop1);
        wxArrayPGProperty selectedProperties2 = pg->GetSelectedProperties();

        RT_ASSERT( selectedProperties2.size() == 2 )
        RT_ASSERT( !pg->IsPropertySelected(prop1) )
        RT_ASSERT( pg->IsPropertySelected(prop2) )
        RT_ASSERT( pg->IsPropertySelected(prop3) )

        pg->ClearSelection();

        wxArrayPGProperty selectedProperties3 = pg->GetSelectedProperties();

        RT_ASSERT( selectedProperties3.size() == 0 )
        RT_ASSERT( !pg->IsPropertySelected(prop1) )
        RT_ASSERT( !pg->IsPropertySelected(prop2) )
        RT_ASSERT( !pg->IsPropertySelected(prop3) )

        pg->SelectProperty(prop2);

        RT_ASSERT( !pg->IsPropertySelected(prop1) )
        RT_ASSERT( pg->IsPropertySelected(prop2) )
        RT_ASSERT( !pg->IsPropertySelected(prop3) )
    }

    {
        //
        // Test label editing
        RT_START_TEST(LABEL_EDITING)

        wxPropertyGrid* pg = pgman->GetGrid();

        // Just mostly test that these won't crash
        pg->MakeColumnEditable(0, true);
        pg->MakeColumnEditable(2, true);
        pg->MakeColumnEditable(0, false);
        pg->MakeColumnEditable(2, false);
        pg->SelectProperty(wxT("Height"));
        pg->BeginLabelEdit(0);
        pg->BeginLabelEdit(0);
        pg->EndLabelEdit(0);
        pg->EndLabelEdit(0);

        // Recreate grid
        CreateGrid( -1, -1 );
        pgman = m_pPropGridManager;
    }

    {
        RT_START_TEST(Attributes)

        wxPGProperty* prop = pgman->GetProperty(wxT("StringProperty"));
        prop->SetAttribute(wxT("Dummy Attribute"), (long)15);

        if ( prop->GetAttribute(wxT("Dummy Attribute")).GetLong() != 15 )
            RT_FAILURE();

        prop->SetAttribute(wxT("Dummy Attribute"), wxVariant());

        if ( !prop->GetAttribute(wxT("Dummy Attribute")).IsNull() )
            RT_FAILURE();
    }

#if WXWIN_COMPATIBILITY_3_0
    {
        RT_START_TEST(DoubleToString)

        // Locale-specific decimal separator
        wxString sep = wxString::Format(wxT("%g"), 1.1)[1];

        wxString s;

        if ( wxPropertyGrid::DoubleToString(s, 123.123, 2, true) !=
                wxString::Format(wxT("123%s12"), sep.c_str()) )
            RT_FAILURE();
        if ( wxPropertyGrid::DoubleToString(s, -123.123, 4, false) !=
                wxString::Format(wxT("-123%s1230"), sep.c_str()) )
            RT_FAILURE();
        if ( wxPropertyGrid::DoubleToString(s, -0.02, 1, false) !=
                wxString::Format(wxT("0%s0"), sep) )
            RT_FAILURE();
        if ( wxPropertyGrid::DoubleToString(s, -0.000123, 3, true) != wxT("0") )
            RT_FAILURE();
    }
#endif

    {
        wxPropertyGridPage* page1;
        wxPropertyGridPage* page2;
        wxPropertyGridPage* page3;
        wxVariant pg1_values;
        wxVariant pg2_values;
        wxVariant pg3_values;

        {
            RT_START_TEST(GetPropertyValues)

            page1 = pgman->GetPage(0);
            pg1_values = page1->GetPropertyValues(wxT("Page1"),NULL,wxPG_KEEP_STRUCTURE);
            page2 = pgman->GetPage(1);
            pg2_values = page2->GetPropertyValues(wxT("Page2"),NULL,wxPG_KEEP_STRUCTURE);
            page3 = pgman->GetPage(2);
            pg3_values = page3->GetPropertyValues(wxT("Page3"),NULL,wxPG_KEEP_STRUCTURE);
        }

        {
            RT_START_TEST(SetPropertyValues)

            page1->SetPropertyValues(pg3_values);
            page2->SetPropertyValues(pg1_values);
            page3->SetPropertyValues(pg2_values);
        }
    }

    if ( !pgman->HasFlag(wxPG_HIDE_CATEGORIES) )
    {
        RT_START_TEST(Collapse_and_GetFirstCategory_and_GetNextCategory)

        for ( i=0; i<3; i++ )
        {
            wxPropertyGridPage* page = pgman->GetPage(i);

            wxPropertyGridIterator it;

            for ( it = page->GetIterator( wxPG_ITERATE_CATEGORIES );
                  !it.AtEnd();
                  ++it )
            {
                wxPGProperty* p = *it;

                if ( !page->IsPropertyCategory(p) )
                    RT_FAILURE();

                page->Collapse( p );

                t.Printf(wxT("Collapsing: %s\n"),page->GetPropertyLabel(p).c_str());
                ed->AppendText(t);
            }
        }
    }

    {
        RT_START_TEST(Save_And_RestoreEditableState)

        for ( i=0; i<3; i++ )
        {
            pgman->SelectPage(i);

            wxString stringState = pgman->SaveEditableState();
            bool res = pgman->RestoreEditableState(stringState);
            if ( !res )
                RT_FAILURE();
        }
    }

    //if ( !pgman->HasFlag(wxPG_HIDE_CATEGORIES) )
    {
        RT_START_TEST(Expand_and_GetFirstCategory_and_GetNextCategory)

        for ( i=0; i<3; i++ )
        {
            wxPropertyGridPage* page = pgman->GetPage(i);

            wxPropertyGridIterator it;

            for ( it = page->GetIterator( wxPG_ITERATE_CATEGORIES );
                  !it.AtEnd();
                  ++it )
            {
                wxPGProperty* p = *it;

                if ( !page->IsPropertyCategory(p) )
                    RT_FAILURE();

                page->Expand( p );

                t.Printf(wxT("Expand: %s\n"),page->GetPropertyLabel(p).c_str());
                ed->AppendText(t);
            }
        }
    }

    {
        RT_START_TEST(Choice_Manipulation)

        wxPGProperty* enumProp = pgman->GetProperty(wxT("EnumProperty"));

        pgman->SelectPage(2);
        pgman->SelectProperty(enumProp);
        wxASSERT(pgman->GetGrid()->GetSelection() == enumProp);

        const wxPGChoices& choices = enumProp->GetChoices();
        int ind = enumProp->InsertChoice(wxT("New Choice"), choices.GetCount()/2);
        enumProp->DeleteChoice(ind);

        // Recreate the original grid
        CreateGrid( -1, -1 );
        pgman = m_pPropGridManager;
    }

    //if ( !pgman->HasFlag(wxPG_HIDE_CATEGORIES) )
    {
        RT_START_TEST(RandomCollapse)

        // Select the most error prone page as visible.
        pgman->SelectPage(1);

        for ( i=0; i<3; i++ )
        {
            wxArrayPtrVoid arr;

            wxPropertyGridPage* page = pgman->GetPage(i);

            wxPropertyGridIterator it;

            for ( it = page->GetIterator( wxPG_ITERATE_CATEGORIES );
                  !it.AtEnd();
                  ++it )
            {
                arr.Add((void*)*it);
            }

            if ( !arr.empty() )
            {
                size_t n;

                pgman->Collapse( (wxPGProperty*)arr.Item(0) );

                for ( n=arr.GetCount()-1; n>0; n-- )
                {
                    pgman->Collapse( (wxPGProperty*)arr.Item(n) );
                }
            }

        }
    }

    {
        RT_START_TEST(EnsureVisible)
        pgman->EnsureVisible(wxT("Cell Colour"));
    }

    {
        RT_START_TEST(RemoveProperty)

        wxPGProperty* p;

        wxPGProperty* origParent =
            pgman->GetProperty(wxT("Window Styles"))->GetParent();

        // For testing purposes, let's set some custom cell colours
        p = pgman->GetProperty(wxT("Window Styles"));
        p->SetCell(2, wxPGCell("style"));
        p = pgman->RemoveProperty(wxT("Window Styles"));
        pgman->Refresh();
        pgman->Update();

        pgman->AppendIn(origParent, p);
        wxASSERT( p->GetCell(2).GetText() == wxT("style"));
        pgman->Refresh();
        pgman->Update();
    }

    {
        RT_START_TEST(SortFunction)

        wxPGProperty* p;

        // Make sure indexes are as supposed

        p = pgman->GetProperty(wxT("User Name"));
        if ( p->GetIndexInParent() != 3 )
            RT_FAILURE();

        p = pgman->GetProperty(wxT("User Id"));
        if ( p->GetIndexInParent() != 2 )
            RT_FAILURE();

        p = pgman->GetProperty(wxT("User Home"));
        if ( p->GetIndexInParent() != 1 )
            RT_FAILURE();

        p = pgman->GetProperty(wxT("Operating System"));
        if ( p->GetIndexInParent() != 0 )
            RT_FAILURE();

        pgman->GetGrid()->SetSortFunction(MyPropertySortFunction);

        pgman->GetGrid()->SortChildren(wxT("Environment"));

        // Make sure indexes have been reversed
        p = pgman->GetProperty(wxT("User Name"));
        if ( p->GetIndexInParent() != 0 )
            RT_FAILURE();

        p = pgman->GetProperty(wxT("User Id"));
        if ( p->GetIndexInParent() != 1 )
            RT_FAILURE();

        p = pgman->GetProperty(wxT("User Home"));
        if ( p->GetIndexInParent() != 2 )
            RT_FAILURE();

        p = pgman->GetProperty(wxT("Operating System"));
        if ( p->GetIndexInParent() != 3 )
            RT_FAILURE();
    }

    {
        RT_START_TEST(SetPropertyBackgroundColour)
        wxCommandEvent evt;
        evt.SetInt(1); // IsChecked() will return true.
        evt.SetId(ID_COLOURSCHEME4);
        OnCatColours(evt);
        OnColourScheme(evt);
    }

    {
        RT_START_TEST(Clear)

        // Manager clear
        pgman->SelectProperty(wxT("Label"));
        pgman->Clear();

        if ( pgman->GetPageCount() )
            RT_FAILURE();

        if ( pgman->GetGrid()->GetRoot()->GetChildCount() )
            RT_FAILURE();

        // Recreate the original grid
        CreateGrid( -1, -1 );
        pgman = m_pPropGridManager;

        // Grid clear
        pgman->SelectProperty(wxT("Label"));
        pgman->GetGrid()->Clear();

        if ( pgman->GetGrid()->GetRoot()->GetChildCount() )
            RT_FAILURE();

        // Recreate the original grid
        CreateGrid( -1, -1 );
        pgman = m_pPropGridManager;
    }

    {
        RT_START_TEST(SetSplitterPosition)

        InitPanel();

        const int trySplitterPos = 50;

        int style = wxPG_AUTO_SORT;  // wxPG_SPLITTER_AUTO_CENTER;
        pgman = m_pPropGridManager =
            new wxPropertyGridManager(m_panel, wxID_ANY,
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      style );

        PopulateGrid();
        pgman->SetSplitterPosition(trySplitterPos);

        if ( pgman->GetGrid()->GetSplitterPosition() != trySplitterPos )
            RT_FAILURE_MSG(wxString::Format(wxT("Splitter position was %i (should have been %i)"),(int)pgman->GetGrid()->GetSplitterPosition(),trySplitterPos));

        m_topSizer->Add( m_pPropGridManager, wxSizerFlags(1).Expand());
        FinalizePanel();

        wxSize origSz = GetSize();

        wxSize sz = origSz;
        sz.IncBy(5, 5);
        SetSize(sz);

        if ( pgman->GetGrid()->GetSplitterPosition() != trySplitterPos )
            RT_FAILURE_MSG(wxString::Format(wxT("Splitter position was %i (should have been %i)"),(int)pgman->GetGrid()->GetSplitterPosition(),trySplitterPos));

        SetSize(origSz);

        // Recreate the original grid
        CreateGrid( -1, -1 );
        pgman = m_pPropGridManager;
    }

    {
        RT_START_TEST(HideProperty)

        wxPropertyGridPage* page = pgman->GetPage(0);

        wxArrayPGProperty arr1;

        arr1 = GetPropertiesInRandomOrder(page);

        if ( !_failed_ )
        {
            for ( i=0; i<arr1.size(); i++ )
            {
                wxPGProperty* p = arr1[i];
                page->HideProperty(p, true);

                wxString s = wxString::Format(wxT("HideProperty(%i, %s)"), (int)i, p->GetLabel().c_str());
                RT_VALIDATE_VIRTUAL_HEIGHT(page, s)
                if ( _failed_ )
                    break;
            }
        }

        if ( !_failed_ )
        {
            wxArrayPGProperty arr2 = GetPropertiesInRandomOrder(page);

            for ( i=0; i<arr2.size(); i++ )
            {
                wxPGProperty* p = arr2[i];
                page->HideProperty(p, false);

                wxString s = wxString::Format(wxT("ShowProperty(%i, %s)"), (int)i, p->GetLabel().c_str());
                RT_VALIDATE_VIRTUAL_HEIGHT(page, s)
                if ( _failed_ )
                    break;
            }
        }

        //
        // Let's do some more, for better consistency
        arr1 = GetPropertiesInRandomOrder(page);

        if ( !_failed_ )
        {
            for ( i=0; i<arr1.size(); i++ )
            {
                wxPGProperty* p = arr1[i];
                page->HideProperty(p, true);

                wxString s = wxString::Format(wxT("HideProperty(%i, %s)"), (int)i, p->GetLabel().c_str());
                RT_VALIDATE_VIRTUAL_HEIGHT(page, s)
                if ( _failed_ )
                    break;
            }
        }

        if ( !_failed_ )
        {
            wxArrayPGProperty arr2 = GetPropertiesInRandomOrder(page);

            for ( i=0; i<arr2.size(); i++ )
            {
                wxPGProperty* p = arr2[i];
                page->HideProperty(p, false);

                wxString s = wxString::Format(wxT("ShowProperty(%i, %s)"), (int)i, p->GetLabel().c_str());
                RT_VALIDATE_VIRTUAL_HEIGHT(page, s)
                if ( _failed_ )
                    break;
            }
        }

        //
        // Ok, this time only hide half of them
        arr1 = GetPropertiesInRandomOrder(page);
        arr1.resize(arr1.size()/2);

        if ( !_failed_ )
        {
            for ( i=0; i<arr1.size(); i++ )
            {
                wxPGProperty* p = arr1[i];
                page->HideProperty(p, true);

                wxString s = wxString::Format(wxT("HideProperty(%i, %s)"), (int)i, p->GetLabel().c_str());
                RT_VALIDATE_VIRTUAL_HEIGHT(page, s)
                if ( _failed_ )
                    break;
            }
        }

        if ( !_failed_ )
        {
            wxArrayPGProperty arr2 = GetPropertiesInRandomOrder(page);

            for ( i=0; i<arr2.size(); i++ )
            {
                wxPGProperty* p = arr2[i];
                page->HideProperty(p, false);

                wxString s = wxString::Format(wxT("ShowProperty(%i, %s)"), (int)i, p->GetLabel().c_str());
                RT_VALIDATE_VIRTUAL_HEIGHT(page, s)
                if ( _failed_ )
                    break;
            }
        }

        // Recreate the original grid
        CreateGrid( -1, -1 );
        pgman = m_pPropGridManager;
    }

    {
        RT_START_TEST(SetFlagsAsString and GetFlagsAsString)

        // Select the most error prone page as visible.
        pgman->SelectPage(1);

        for ( i = 0; i < 3; i++ )
        {
            wxPropertyGridPage* page = pgman->GetPage(i);

            wxPropertyGridIterator it;
            for ( it = page->GetIterator(wxPG_ITERATE_VISIBLE);
                  !it.AtEnd();
                  ++it )
            {
                wxPGProperty *p = *it;

                // Save initial flags
                wxPGProperty::FlagType oldFlags = 0;
                if( p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    oldFlags |= wxPG_PROP_COLLAPSED;
                }
                if( p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    oldFlags |= wxPG_PROP_DISABLED;
                }
                if( p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    oldFlags |= wxPG_PROP_HIDDEN;
                }
                if( p->HasFlag(wxPG_PROP_NOEDITOR) )
                {
                    oldFlags |= wxPG_PROP_NOEDITOR;
                }

                wxString flags;

                if ( p->IsCategory() )
                {
                    if ( GetRandomBooleanVal() )
                    {
                        if ( !flags.empty() )
                        {
                            flags.append(wxS("|"));
                        }
                        flags.append(wxS("COLLAPSED"));
                    }
                }

                if (GetRandomBooleanVal() )
                {
                    if ( !flags.empty() )
                    {
                        flags.append(wxS("|"));
                    }
                    flags.append(wxS("DISABLED"));
                }

                if ( GetRandomBooleanVal() )
                {
                    if ( !flags.empty() )
                    {
                        flags.append(wxS("|"));
                    }
                    flags.append(wxS("HIDDEN"));
                }

                // Set flags
                p->SetFlagsFromString(flags);

                // Verify if flags have been properly set
                if ( flags.Find(wxS("COLLAPSED")) != wxNOT_FOUND &&
                     !p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Error setting flag from string 'COLLAPSED' for property '%s'"),
                        p->GetName().c_str()));
                }
                if ( flags.Find(wxS("COLLAPSED")) == wxNOT_FOUND &&
                     p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Error resetting flag from string 'COLLAPSED'for property '%s'"),
                        p->GetName().c_str()));
                }
                if ( flags.Find(wxS("DISABLED")) != wxNOT_FOUND &&
                     !p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Error setting flag from string 'DISABLED' for property '%s'"),
                        p->GetName().c_str()));
                }
                if ( flags.Find(wxS("DISABLED")) == wxNOT_FOUND &&
                     p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Error resetting flag from string 'DISABLED' for property '%s'"),
                        p->GetName().c_str()));
                }
                if ( flags.Find(wxS("HIDDEN")) != wxNOT_FOUND &&
                     !p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Error setting flag from string 'HIDDEN' for property '%s'"),
                        p->GetName().c_str()));
                }
                if ( flags.Find(wxS("HIDDEN")) == wxNOT_FOUND &&
                     p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Error resetting flag from string 'HIDDEN' for property '%s'"),
                        p->GetName().c_str()));
                }

                // Get individual flags
                bool ok;

                flags = p->GetFlagsAsString(wxPG_PROP_COLLAPSED);
                if ( p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    ok = (flags == wxS("COLLAPSED"));
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Invalid string for wxPG_PROP_COLLAPSED flag for property '%s'"),
                        p->GetName().c_str()));
                }

                flags = p->GetFlagsAsString(wxPG_PROP_DISABLED);
                if ( p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    ok = (flags == wxS("DISABLED"));
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Invalid string for wxPG_PROP_DISABLED flag for property '%s'"),
                        p->GetName().c_str()));
                }

                flags = p->GetFlagsAsString(wxPG_PROP_HIDDEN);
                if ( p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    ok = (flags == wxS("HIDDEN"));
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Invalid string for wxPG_PROP_HIDDEN flag for property '%s'"),
                        p->GetName().c_str()));
                }

                flags = p->GetFlagsAsString(wxPG_PROP_NOEDITOR);
                if ( p->HasFlag(wxPG_PROP_NOEDITOR) )
                {
                    ok = (flags == wxS("NOEDITOR"));
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Invalid string for wxPG_PROP_NOEDITOR flag for property '%s'"),
                        p->GetName().c_str()));
                }

                // Get all flags
                flags = p->GetFlagsAsString(wxPG_STRING_STORED_FLAGS);
                if ( p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    ok = (flags.Find(wxS("COLLAPSED")) != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find(wxS("COLLAPSED")) == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Invalid string for wxPG_PROP_COLLAPSED flag for property '%s'"),
                        p->GetName().c_str()));
                }

                if ( p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    ok = (flags.Find(wxS("DISABLED")) != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find(wxS("DISABLED")) == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Invalid string for wxPG_PROP_DISBALED flag for property '%s'"),
                        p->GetName().c_str()));
                }

                if ( p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    ok = (flags.Find(wxS("HIDDEN")) != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find(wxS("HIDDEN")) == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Invalid string for wxPG_PROP_HIDDEN flag for property '%s'"),
                        p->GetName().c_str()));
                }

                if ( p->HasFlag(wxPG_PROP_NOEDITOR) )
                {
                    ok = (flags.Find(wxS("NOEDITOR")) != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find(wxS("NOEDITOR")) == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    RT_FAILURE_MSG(wxString::Format(wxS("Invalid string for wxPG_PROP_NOEDITOR flag for property '%s'"),
                        p->GetName().c_str()));
                }

                // Restore original flags
                p->ChangeFlag(wxPG_PROP_COLLAPSED, (oldFlags & wxPG_PROP_COLLAPSED) != 0);
                p->ChangeFlag(wxPG_PROP_DISABLED, (oldFlags & wxPG_PROP_DISABLED) != 0);
                p->ChangeFlag(wxPG_PROP_HIDDEN, (oldFlags & wxPG_PROP_HIDDEN) != 0);
                p->ChangeFlag(wxPG_PROP_NOEDITOR, (oldFlags & wxPG_PROP_NOEDITOR) != 0);
            }
        }
    }

    if ( fullTest )
    {
        RT_START_TEST(MultipleColumns)

        // Test with multiple columns
        CreateGrid( -1, -1 );
        FinalizeFramePosition();
        pgman = m_pPropGridManager;
        for ( i=3; i<12; i+=2 )
        {
            RT_MSG(wxString::Format(wxT("%i columns"),(int)i));
            pgman->SetColumnCount(i);
            Refresh();
            Update();
            wxMilliSleep(500);
        }
    }

    if ( fullTest )
    {
        RT_START_TEST(WindowStyles)

        // Recreate grid with all possible (single) flags
        wxASSERT(wxPG_AUTO_SORT == 0x000000010);

        for ( i=4; i<16; i++ )
        {
            int flag = 1<<i;
            RT_MSG(wxString::Format(wxT("Style: 0x%X"),flag));
            CreateGrid( flag, -1 );
            pgman = m_pPropGridManager;
            Update();
            wxMilliSleep(500);
        }

        wxASSERT(wxPG_EX_INIT_NOCAT == 0x00001000);

        for ( i=12; i<27; i++ )
        {
            int flag = 1<<i;
            RT_MSG(wxString::Format(wxT("ExStyle: 0x%X"),flag));
            CreateGrid( -1, flag );
            pgman = m_pPropGridManager;
            Update();
            wxMilliSleep(500);
        }

        // Recreate the original grid
        CreateGrid( -1, -1 );
        pgman = m_pPropGridManager;
    }

    // Restore original grid size
    FinalizeFramePosition();

    RT_START_TEST(none)

    bool retVal;

    if ( failures || !errorMessages.empty() )
    {
        retVal = false;

        wxString s;
#ifdef __WXDEBUG__
        if ( failures )
#endif
            s = wxString::Format(wxT("%i tests failed!!!"), failures);
#ifdef __WXDEBUG__
        else
            s = wxString::Format(wxT("All tests were successful, but there were %i warnings!"), wxPGGlobalVars->m_warnings);
#endif
        RT_MSG(s)
        for ( i=0; i<errorMessages.size(); i++ )
            RT_MSG(errorMessages[i])
        wxMessageBox(s, wxT("Some Tests Failed"));
    }
    else
    {
        RT_MSG(wxT("All tests successful"))
        retVal = true;

        if ( !interactive )
            dlg->Close();
    }

    pgman->SelectPage(0);

    // Test may screw up the toolbar, so we need to refresh it.
    wxToolBar* toolBar = pgman->GetToolBar();
    if ( toolBar )
        toolBar->Refresh();

    return retVal;
}

// -----------------------------------------------------------------------
