///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/propgrid.cpp
// Purpose:     wxPropertyGrid unit tests
// Author:      Artur Wieczorek, Jaakko Salli
// Created:     2023-01-12
// Copyright:   (c) 2023 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_PROPGRID

#include "wx/app.h"
#include "wx/artprov.h"
#include "wx/dcmemory.h"
#if wxUSE_DATEPICKCTRL
#include "wx/datectrl.h"
#endif
#include "wx/stopwatch.h"

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/manager.h"
#include "wx/propgrid/advprops.h"

#include <random>
#include <vector>

// Prepare RNG
static std::random_device s_rd;
static std::default_random_engine s_rng(s_rd());

static void PopulateWithStandardItems(wxPropertyGridManager* pgManager)
{
    wxPropertyGridPage* pg = pgManager->GetPage("Standard Items");

    pg->Append(new wxPropertyCategory("Appearance", wxPG_LABEL));

    pg->Append(new wxStringProperty("Label", wxPG_LABEL, "PropertyGridTest"));
    pg->Append(new wxFontProperty("Font", wxPG_LABEL));
    pg->SetPropertyHelpString("Font", "Editing this will change font used in the property grid.");

    pg->Append(new wxSystemColourProperty("Margin Colour", wxPG_LABEL, pg->GetGrid()->GetMarginColour()));

    pg->Append(new wxSystemColourProperty("Cell Colour", wxPG_LABEL, pg->GetGrid()->GetCellBackgroundColour()));
    pg->Append(new wxSystemColourProperty("Cell Text Colour", wxPG_LABEL, pg->GetGrid()->GetCellTextColour()));
    pg->Append(new wxSystemColourProperty("Line Colour", wxPG_LABEL, pg->GetGrid()->GetLineColour()));
    static const wxString flags_prop_labels[] = {
        "wxICONIZE", "wxCAPTION", "wxMINIMIZE_BOX", "wxMAXIMIZE_BOX"
    };
    long flags_prop_values[] = { wxICONIZE, wxCAPTION, wxMINIMIZE_BOX, wxMAXIMIZE_BOX };
    wxPGChoices combinedFlags;
    combinedFlags.Add(WXSIZEOF(flags_prop_labels), flags_prop_labels, flags_prop_values);
    pg->Append(new wxFlagsProperty("Window Styles", wxPG_LABEL,
        combinedFlags, wxTheApp->GetTopWindow()->GetWindowStyle()));

    pg->Append(new wxCursorProperty("Cursor", wxPG_LABEL));

    pg->Append(new wxPropertyCategory("Position", "PositionCategory"));
    pg->SetPropertyHelpString("PositionCategory", "Change in items in this category will cause respective changes in frame.");

    pg->Append(new wxIntProperty("Height", wxPG_LABEL, 480L));
    pg->SetPropertyAttribute("Height", wxPG_ATTR_MIN, 10L);
    pg->SetPropertyAttribute("Height", wxPG_ATTR_MAX, 2048L);
    pg->SetPropertyAttribute("Height", wxPG_ATTR_UNITS, "Pixels");

    pg->SetPropertyValueUnspecified("Height");
    pg->SetPropertyAttribute("Height", wxPG_ATTR_HINT, "Enter new height for window");

    pg->SetPropertyHelpString("Height", "This property uses attributes \"Units\" and \"Hint\".");

    pg->Append(new wxIntProperty("Width", wxPG_LABEL, 640L));
    pg->SetPropertyAttribute("Width", wxPG_ATTR_MIN, 10L);
    pg->SetPropertyAttribute("Width", wxPG_ATTR_MAX, 2048L);
    pg->SetPropertyAttribute("Width", wxPG_ATTR_UNITS, "Pixels");

    pg->SetPropertyValueUnspecified("Width");
    pg->SetPropertyAttribute("Width", wxPG_ATTR_HINT, "Enter new width for window");
    pg->SetPropertyHelpString("Width", "This property uses attributes \"Units\" and \"Hint\".");

    pg->Append(new wxIntProperty("X", wxPG_LABEL, 10L));
    pg->SetPropertyAttribute("X", wxPG_ATTR_UNITS, "Pixels");
    pg->SetPropertyHelpString("X", "This property uses \"Units\" attribute.");

    pg->Append(new wxIntProperty("Y", wxPG_LABEL, 10L));
    pg->SetPropertyAttribute("Y", wxPG_ATTR_UNITS, "Pixels");
    pg->SetPropertyHelpString("Y", "This property uses \"Units\" attribute.");

    const wxString disabledHelpString = "This property is simply disabled.";

    pg->Append(new wxPropertyCategory("Environment", wxPG_LABEL));
    pg->Append(new wxStringProperty("Operating System", wxPG_LABEL, ::wxGetOsDescription()));

    pg->Append(new wxStringProperty("User Id", wxPG_LABEL, ::wxGetUserId()));
    pg->Append(new wxDirProperty("User Home", wxPG_LABEL, ::wxGetUserHome()));
    pg->Append(new wxStringProperty("User Name", wxPG_LABEL, ::wxGetUserName()));

    // Disable some of them
    pg->DisableProperty("Operating System");
    pg->DisableProperty("User Id");
    pg->DisableProperty("User Name");

    pg->SetPropertyHelpString("Operating System", disabledHelpString);
    pg->SetPropertyHelpString("User Id", disabledHelpString);
    pg->SetPropertyHelpString("User Name", disabledHelpString);

    pg->Append(new wxPropertyCategory("More Examples", wxPG_LABEL));

    pg->Append(new wxLongStringProperty("Information", wxPG_LABEL,
        "Editing properties will have immediate effect on this window, "
        "and vice versa (at least in most cases, that is)."
    ));
    pg->SetPropertyHelpString("Information", "This property is read-only.");

    pg->SetPropertyReadOnly("Information", true);

    // Set test information for cells in columns 3 and 4
    // (reserve column 2 for displaying units)
    wxBitmap bmp = wxArtProvider::GetBitmap(wxART_FOLDER);

    for ( auto it = pg->GetGrid()->GetIterator(); !it.AtEnd(); ++it )
    {
        wxPGProperty* p = *it;
        if ( p->IsCategory() )
            continue;

        pg->SetPropertyCell(p, 3, "Cell 3", bmp);
        pg->SetPropertyCell(p, 4, "Cell 4", wxBitmapBundle(), *wxWHITE, *wxBLACK);
    }
}

static void PopulateWithExamples(wxPropertyGridManager* pgManager)
{
    wxPropertyGridPage* pg = pgManager->GetPage("Examples");
    wxPGProperty* pid;
    wxPGProperty* prop;

    pg->Append(new wxIntProperty("IntProperty", wxPG_LABEL, 12345678L));

#if wxUSE_SPINBTN
    pg->Append(new wxIntProperty("SpinCtrl", wxPG_LABEL, 0L));

    pg->SetPropertyEditor("SpinCtrl", wxPGEditor_SpinCtrl);
    pg->SetPropertyAttribute("SpinCtrl", wxPG_ATTR_MIN, -2L);
    pg->SetPropertyAttribute("SpinCtrl", wxPG_ATTR_MAX, 16384L);
    pg->SetPropertyAttribute("SpinCtrl", wxPG_ATTR_SPINCTRL_STEP, 2L);
    pg->SetPropertyAttribute("SpinCtrl", wxPG_ATTR_SPINCTRL_MOTION, true);

    pg->SetPropertyHelpString("SpinCtrl",
        "This is regular wxIntProperty, which editor has been "
        "changed to wxPGEditor_SpinCtrl.");

#endif

    // Add bool property
    pg->Append(new wxBoolProperty("BoolProperty", wxPG_LABEL, false));

    // Add bool property with check box
    pg->Append(new wxBoolProperty("BoolProperty with CheckBox", wxPG_LABEL, false));
    pg->SetPropertyAttribute("BoolProperty with CheckBox", wxPG_BOOL_USE_CHECKBOX, true);

    pg->SetPropertyHelpString("BoolProperty with CheckBox",
        "Property attribute wxPG_BOOL_USE_CHECKBOX has been set to true.");

    prop = pg->Append(new wxFloatProperty("FloatProperty", wxPG_LABEL, 1234500.23));
    prop->SetAttribute(wxPG_ATTR_MIN, -100.12);

    pg->Append(new wxLongStringProperty("LongStringProperty", "LongStringProp",
        "This is much longer string than the first one. Edit it by clicking the button."));

    wxArrayString example_array;
    example_array.Add("String 1");
    example_array.Add("String 2");
    example_array.Add("String 3");
    pg->Append(new wxArrayStringProperty("ArrayStringProperty", wxPG_LABEL, example_array));

    prop = new wxFileProperty("FileProperty", "TextFile");
    pg->Append(prop);

    prop->SetAttribute(wxPG_FILE_WILDCARD, "Text Files (*.txt)|*.txt");
    prop->SetAttribute(wxPG_DIALOG_TITLE, "Custom File Dialog Title");
    prop->SetAttribute(wxPG_FILE_SHOW_FULL_PATH, false);

#ifdef __WXMSW__
    prop->SetAttribute(wxPG_FILE_SHOW_RELATIVE_PATH, "C:\\Windows");
    pg->SetPropertyValue(prop, "C:\\Windows\\System32\\msvcrt71.dll");
#endif

#if wxUSE_IMAGE
    pg->Append(new wxImageFileProperty("ImageFile", wxPG_LABEL));
#endif

    pid = pg->Append(new wxColourProperty("ColourProperty", wxPG_LABEL, *wxRED));
    pg->SetPropertyEditor("ColourProperty", wxPGEditor_ComboBox);
    pg->GetProperty("ColourProperty")->SetAutoUnspecified(true);
    pg->SetPropertyHelpString("ColourProperty",
        "wxPropertyGrid::SetPropertyEditor method has been used to change "
        "editor of this property to wxPGEditor_ComboBox)");

    pid = pg->Append(new wxColourProperty("ColourPropertyWithAlpha",
        wxPG_LABEL, wxColour(15, 200, 95, 128)));
    pg->SetPropertyAttribute("ColourPropertyWithAlpha", wxPG_COLOUR_HAS_ALPHA, true);
    pg->SetPropertyHelpString("ColourPropertyWithAlpha",
        "Attribute \"HasAlpha\" is set to true for this property.");

    pg->Append(new wxColourProperty("ColourProperty2", wxPG_LABEL, *wxGREEN));

    static const wxString enum_prop_labels[] = { "One Item",
        "Another Item", "One More", "This Is Last" };

    static long enum_prop_values[] = { 40, 80, 120, 160 };

    pg->Append(new wxEnumProperty("EnumProperty", wxPG_LABEL,
        wxArrayString(WXSIZEOF(enum_prop_labels), enum_prop_labels),
        wxArrayInt(enum_prop_values, enum_prop_values + WXSIZEOF(enum_prop_values)), 80));

    wxPGChoices soc;

    soc.Set(wxArrayString(WXSIZEOF(enum_prop_labels), enum_prop_labels),
        wxArrayInt(enum_prop_values, enum_prop_values + WXSIZEOF(enum_prop_values)));

    // add extra items
    soc.Add("Look, it continues", 200);
    soc.Add("Even More", 240);
    soc.Add("And More", 280);
    soc.Add(wxString(), 300);
    soc.Add("True End of the List", 320);

    soc[1].SetFgCol(*wxRED);
    soc[1].SetBgCol(*wxLIGHT_GREY);
    soc[2].SetFgCol(*wxGREEN);
    soc[2].SetBgCol(*wxLIGHT_GREY);
    soc[3].SetFgCol(*wxBLUE);
    soc[3].SetBgCol(*wxLIGHT_GREY);
    soc[4].SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER));

    pg->Append(new wxEnumProperty("EnumProperty 2", wxPG_LABEL, soc, 240));
    pg->GetProperty("EnumProperty 2")->AddChoice("Testing Extra", 360);

    pg->Append(new wxEnumProperty("EnumProperty 3", wxPG_LABEL, soc, 240));

    pg->GetProperty("EnumProperty 3")->SetAttribute(wxPG_ATTR_HINT, "Dummy Hint");

    pg->SetPropertyHelpString("EnumProperty 3", "This property uses \"Hint\" attribute.");

    pg->Append(new wxEnumProperty("EnumProperty 4", wxPG_LABEL, soc, 240));
    pg->GetProperty("EnumProperty 4")->AddChoice("4th only", 360);

    pg->SetPropertyHelpString("EnumProperty 4",
        "Should have one extra item when compared to EnumProperty 3");

    pg->Append(new wxEnumProperty("EnumProperty With Bitmap", "EnumProperty 5", soc, 280));
    pg->SetPropertyHelpString("EnumProperty 5", "Should have bitmap in front of the displayed value");
    wxBitmap bmpVal = wxArtProvider::GetBitmap(wxART_REMOVABLE);
    pg->SetPropertyImage("EnumProperty 5", bmpVal);

    pg->Append(new wxStringProperty("Password", wxPG_LABEL, "password"));
    pg->SetPropertyAttribute("Password", wxPG_STRING_PASSWORD, true);
    pg->SetPropertyHelpString("Password", "Has attribute wxPG_STRING_PASSWORD set to true");

    pg->Append(new wxDirProperty("DirProperty", wxPG_LABEL, ::wxGetUserHome()));
    pg->SetPropertyAttribute("DirProperty", wxPG_DIALOG_TITLE, "This is a custom dir dialog title");

    pg->Append(new wxStringProperty("StringProperty", wxPG_LABEL));
    pg->SetPropertyMaxLength("StringProperty", 6);
    pg->SetPropertyHelpString("StringProperty","Max length of this text has been limited to 6.");

    pg->SetPropertyValue("StringProperty", "some text");

    pg->Append(new wxStringProperty("StringProperty AutoComplete", wxPG_LABEL));

    wxArrayString autoCompleteStrings;
    autoCompleteStrings.Add("One choice");
    autoCompleteStrings.Add("Another choice");
    autoCompleteStrings.Add("Another choice, yeah");
    autoCompleteStrings.Add("Yet another choice");
    autoCompleteStrings.Add("Yet another choice, bear with me");
    pg->SetPropertyAttribute("StringProperty AutoComplete", wxPG_ATTR_AUTOCOMPLETE, autoCompleteStrings);

    pg->SetPropertyHelpString("StringProperty AutoComplete",
        "AutoComplete attribute has been set for this property.");

    pg->Append(new wxStringProperty("StringPropertyWithBitmap", wxPG_LABEL, "Test Text"));
    wxBitmap myTestBitmap1x(60, 15, 32);
    {
        wxMemoryDC mdc(myTestBitmap1x);
        mdc.SetBackground(*wxWHITE_BRUSH);
        mdc.Clear();
        mdc.SetPen(*wxBLACK_PEN);
        mdc.SetBrush(*wxWHITE_BRUSH);
        mdc.DrawRectangle(0, 0, 60, 15);
        mdc.DrawLine(0, 0, 59, 14);
        mdc.SetTextForeground(*wxBLACK);
        mdc.DrawText("x1", 0, 0);
    }
    wxBitmap myTestBitmap2x(120, 30, 32);
    {
        wxMemoryDC mdc(myTestBitmap2x);
        mdc.SetBackground(*wxWHITE_BRUSH);
        mdc.Clear();
        mdc.SetPen(wxPen(*wxBLUE, 2));
        mdc.SetBrush(*wxWHITE_BRUSH);
        mdc.DrawRectangle(0, 0, 120, 30);
        mdc.DrawLine(0, 0, 119, 31);
        mdc.SetTextForeground(*wxBLUE);
        wxFont f = mdc.GetFont();
        f.SetPixelSize(2 * f.GetPixelSize());
        mdc.SetFont(f);
        mdc.DrawText("x2", 0, 0);
    }
    myTestBitmap2x.SetScaleFactor(2);
    pg->SetPropertyImage("StringPropertyWithBitmap", wxBitmapBundle::FromBitmaps(myTestBitmap1x, myTestBitmap2x));

    // this value array would be optional if values matched string indexes
    static const wxString flags_prop_labels[] = {
        "wxICONIZE", "wxCAPTION", "wxMINIMIZE_BOX", "wxMAXIMIZE_BOX"
    };
    long flags_prop_values[] = { wxICONIZE, wxCAPTION, wxMINIMIZE_BOX, wxMAXIMIZE_BOX };
    wxPGChoices combinedFlags;
    combinedFlags.Add(WXSIZEOF(flags_prop_labels), flags_prop_labels, flags_prop_values);
    pg->Append(new wxFlagsProperty("FlagsProperty", wxPG_LABEL,
                   combinedFlags, wxTheApp->GetTopWindow()->GetWindowStyle() ) );
    pg->SetPropertyAttribute("FlagsProperty", wxPG_BOOL_USE_CHECKBOX, true, wxPGPropertyValuesFlags::Recurse);

    wxArrayString tchoices;
    tchoices.Add("Cabbage");
    tchoices.Add("Carrot");
    tchoices.Add("Onion");
    tchoices.Add("Potato");
    tchoices.Add("Strawberry");

    wxArrayString tchoicesValues;
    tchoicesValues.Add("Carrot");
    tchoicesValues.Add("Potato");

    pg->Append(new wxEnumProperty("EnumProperty X", wxPG_LABEL, tchoices));

    pg->Append(new wxMultiChoiceProperty("MultiChoiceProperty", wxPG_LABEL, tchoices, tchoicesValues));
    pg->SetPropertyAttribute("MultiChoiceProperty", wxPG_ATTR_MULTICHOICE_USERSTRINGMODE, 1);

#if wxUSE_LONGLONG
    pg->Append(new wxUIntProperty("UIntProperty", wxPG_LABEL, wxULongLong(wxULL(0xFEEEFEEEFEEE))));
#else
    pg->Append(new wxUIntProperty("UIntProperty", wxPG_LABEL, 0xFEEEFEEE));
#endif
    pg->SetPropertyAttribute("UIntProperty", wxPG_UINT_PREFIX, wxPG_PREFIX_NONE);
    pg->SetPropertyAttribute("UIntProperty", wxPG_UINT_BASE, wxPG_BASE_HEX);

    wxPGChoices eech;
    eech.Add("Choice 1");
    eech.Add("Choice 2");
    eech.Add("Choice 3");
    pg->Append(new wxEditEnumProperty("EditEnumProperty", wxPG_LABEL, eech, "Choice not in the list"));

    pg->GetProperty("EditEnumProperty")->SetAttribute(wxPG_ATTR_HINT, "Dummy Hint");

#if wxUSE_DATETIME
    //
    // wxDateTimeProperty
    pg->Append(new wxDateProperty("DateProperty", wxPG_LABEL, wxDateTime::Now()));

#if wxUSE_DATEPICKCTRL
    pg->SetPropertyAttribute("DateProperty", wxPG_DATE_PICKER_STYLE,
        (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY | wxDP_ALLOWNONE));

    pg->SetPropertyHelpString("DateProperty",
        "Attribute wxPG_DATE_PICKER_STYLE has been set to (long)"
        "(wxDP_DROPDOWN | wxDP_SHOWCENTURY | wxDP_ALLOWNONE).");
#endif

#endif

    wxPGProperty* carProp = pg->Append(new wxStringProperty("Car", wxPG_LABEL, "<composed>"));

    pg->AppendIn(carProp, new wxStringProperty("Model", wxPG_LABEL, "Lamborghini Diablo SV"));

    pg->AppendIn(carProp, new wxIntProperty("Engine Size (cc)", wxPG_LABEL, 5707L));

    wxPGProperty* speedsProp = pg->AppendIn(carProp, new wxStringProperty("Speeds", wxPG_LABEL, "<composed>"));

    pg->AppendIn(speedsProp, new wxIntProperty("Max. Speed (mph)", wxPG_LABEL, 290L));
    pg->AppendIn(speedsProp, new wxFloatProperty("0-100 mph (sec)", wxPG_LABEL, 3.9));
    pg->AppendIn(speedsProp, new wxFloatProperty("1/4 mile (sec)", wxPG_LABEL, 8.6));

    // This is how child property can be referred to by name
    pg->SetPropertyValue("Car.Speeds.Max. Speed (mph)", 300);

    pg->AppendIn(carProp, new wxIntProperty("Price ($)", wxPG_LABEL, 300000L));

    pg->AppendIn(carProp, new wxBoolProperty("Convertible", wxPG_LABEL, false));

    wxPGChoices bc;
    bc.Add("Wee", wxArtProvider::GetBitmap(wxART_CDROM, wxART_OTHER, wxSize(16, 16)));
    bc.Add("Not so wee", wxArtProvider::GetBitmap(wxART_FLOPPY, wxART_OTHER, wxSize(32, 32)));
    bc.Add("Friggin' huge", wxArtProvider::GetBitmap(wxART_HARDDISK, wxART_OTHER, wxSize(64, 64)));

    pg->Append(new wxEnumProperty("Variable Height Bitmaps", wxPG_LABEL, bc, 0));

    pid = new wxStringProperty("wxWidgets Traits", wxPG_LABEL, "<composed>");
    pg->SetPropertyReadOnly(pid);

    pid->AppendChild(new wxStringProperty("Latest Release", wxPG_LABEL, "3.2.1"));
    pid->AppendChild(new wxBoolProperty("Win API", wxPG_LABEL, true));

    pg->Append(pid);

    pg->AppendIn(pid, new wxBoolProperty("QT", wxPG_LABEL, true));
    pg->AppendIn(pid, new wxBoolProperty("Cocoa", wxPG_LABEL, true));
    pg->AppendIn(pid, new wxBoolProperty("Haiku", wxPG_LABEL, false));
    pg->AppendIn(pid, new wxStringProperty("Trunk Version", wxPG_LABEL, wxVERSION_NUM_DOT_STRING));
    pg->AppendIn(pid, new wxBoolProperty("GTK+", wxPG_LABEL, true));
}

static wxPropertyGridManager* CreateGrid(int style, int extraStyle)
{
    // This function creates the property grid in tests

    if ( style == -1 )
        style = // default style
        wxPG_BOLD_MODIFIED |
        wxPG_SPLITTER_AUTO_CENTER |
        wxPG_AUTO_SORT |
        wxPG_TOOLBAR;

    if ( extraStyle == -1 )
        // default extra style
        extraStyle = wxPG_EX_MODE_BUTTONS |
        wxPG_EX_MULTIPLE_SELECTION;

    wxPropertyGridManager* pgManager = new wxPropertyGridManager(wxTheApp->GetTopWindow(), wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    pgManager->SetSize(wxTheApp->GetTopWindow()->GetClientSize());
    pgManager->SetExtraStyle(extraStyle);

    // This is the default validation failure behaviour
    pgManager->SetValidationFailureBehavior(wxPGVFBFlags::MarkCell |
        wxPGVFBFlags::ShowMessageBox);

    wxPropertyGrid* pg = pgManager->GetGrid();
    // Set somewhat different unspecified value appearance
    wxPGCell cell;
    cell.SetText("Unspecified");
    cell.SetFgCol(*wxLIGHT_GREY);
    pg->SetUnspecifiedValueAppearance(cell);

    // Populate grid
    pgManager->AddPage("Standard Items");
    PopulateWithStandardItems(pgManager);
    pgManager->AddPage("Examples");
    PopulateWithExamples(pgManager);

    pgManager->Refresh();
    pgManager->Update();
    // Wait for update to be done
    wxStopWatch sw;
    while ( sw.Time() < 100 )
    {
        wxYield();
    }

    return pgManager;
}

static void ReplaceGrid(std::unique_ptr<wxPropertyGridManager>& pgManager, int style, int extraStyle)
{
    pgManager.reset(); // First destr0y previous instance
    pgManager.reset(CreateGrid(style, extraStyle));
    pgManager->SetFocus();
}

// Callback for testing property sorting
static int CustomPropertySortFunction(wxPropertyGrid*, wxPGProperty* p1, wxPGProperty* p2)
{
    // Reverse alphabetical order
    return p2->GetLabel().CmpNoCase(p1->GetBaseName());
}

static std::vector<wxPGProperty*> GetPropertiesInRandomOrder(wxPropertyGridInterface* props, int iterationFlags = wxPG_ITERATE_ALL)
{
    std::uniform_int_distribution<size_t> distrib(0, 10000);
    std::vector<wxPGProperty*> arr;

    for ( auto it = props->GetIterator(iterationFlags); !it.AtEnd(); ++it )
    {
        wxPGProperty* p = *it;
        size_t randomNumber = distrib(s_rng);
        p->SetClientData(reinterpret_cast<void*>(randomNumber));
        arr.push_back(p);
    }

    std::sort(arr.begin(), arr.end(), [](wxPGProperty* p1, wxPGProperty* p2) { return ((size_t)p1->GetClientData()) < ((size_t)p2->GetClientData()); });
    return arr;
}

TEST_CASE("PropertyGridTestCase", "[propgrid]")
{
    wxTheApp->GetTopWindow()->Raise();
#if defined(__WXGTK__)
    // Under wxGTK we need to have two children (at least) because if there
    // is one child its paint area is set to fill the whole parent frame.
    std::unique_ptr<wxWindow> win0(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY));
#endif // wxGTK
    std::unique_ptr<wxPropertyGridManager> pgManager(CreateGrid(-1, -1));

    SECTION("Iterator")
    {
        // Basic iterator tests

        int count = 0;
        for ( auto it = pgManager->GetVIterator(wxPG_ITERATE_PROPERTIES); !it.AtEnd(); it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            if ( p->IsCategory() )
                FAIL_CHECK(wxString::Format("'%s' is a category (non-private child property expected)", p->GetLabel()).c_str());
            else if ( p->GetParent()->HasFlag(wxPG_PROP_AGGREGATE) )
                FAIL_CHECK(wxString::Format("'%s' is a private child (non-private child property expected)", p->GetLabel()).c_str());
            count++;
        }
        INFO(wxString::Format("GetVIterator(wxPG_ITERATE_PROPERTIES) -> %i entries", count).c_str());

        count = 0;
        for ( auto it = pgManager->GetVIterator(wxPG_ITERATE_CATEGORIES); !it.AtEnd(); it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            if ( !p->IsCategory() )
                FAIL_CHECK(wxString::Format("'%s' is not a category (only category was expected)", p->GetLabel()).c_str());
            count++;
        }
        INFO(wxString::Format("GetVIterator(wxPG_ITERATE_CATEGORIES) -> %i entries", count).c_str());

        count = 0;
        for ( auto it = pgManager->GetVIterator(wxPG_ITERATE_PROPERTIES | wxPG_ITERATE_CATEGORIES); !it.AtEnd(); it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            if ( p->GetParent()->HasFlag(wxPG_PROP_AGGREGATE) )
                FAIL_CHECK(wxString::Format("'%s' is a private child (non-private child property or category expected)", p->GetLabel()).c_str());
            count++;
        }
        INFO(wxString::Format("GetVIterator(wxPG_ITERATE_PROPERTIES|wxPG_ITERATE_CATEGORIES) -> %i entries", count).c_str());

        count = 0;
        for ( auto it = pgManager->GetVIterator(wxPG_ITERATE_VISIBLE); !it.AtEnd(); it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            if ( (p->GetParent() != p->GetGrid()->GetRoot() && !p->GetParent()->IsExpanded()) )
                FAIL_CHECK(wxString::Format("'%s' had collapsed parent (only visible properties expected)", p->GetLabel()).c_str());
            else if ( p->HasFlag(wxPG_PROP_HIDDEN) )
                FAIL_CHECK(wxString::Format("'%s' was hidden (only visible properties expected)", p->GetLabel()).c_str());
            count++;
        }

        INFO(wxString::Format("GetVIterator(wxPG_ITERATE_VISIBLE) -> %i entries", count).c_str());
    }

    SECTION("Iterator_DeleteFirstPageThenLast")
    {
        // Get all properties from first page
        wxPGVIterator it;
        wxPropertyGridPage* pageFirst = pgManager->GetPage(0);
        std::vector<wxString> propertiesPageFirstInit;
        for ( it = pageFirst->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            propertiesPageFirstInit.push_back(it.GetProperty()->GetName());
        }

        // Get all properties from last page
        wxPropertyGridPage* pageLast = pgManager->GetPage(pgManager->GetPageCount() - 1);
        std::vector<wxString> propertiesPageLastInit;
        for ( it = pageLast->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            propertiesPageLastInit.push_back(it.GetProperty()->GetName());
        }

        int countAllPropertiesInit = 0;
        for ( it = pgManager->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            countAllPropertiesInit++;
        }

        // Delete all properties from first page
        pageFirst->Clear();

        it = pageFirst->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL));
        REQUIRE(it.AtEnd());

        std::vector<wxString> propertiesPageLast;
        for ( it = pageLast->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            propertiesPageLast.push_back(it.GetProperty()->GetName());
        }
        REQUIRE(propertiesPageLast == propertiesPageLastInit);

        size_t countAllProperties = 0;
        for ( it = pgManager->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            countAllProperties++;
        }
        REQUIRE(countAllProperties == countAllPropertiesInit - propertiesPageFirstInit.size());

        // Delete all properties from last page
        pageLast->Clear();

        it = pageFirst->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL));
        REQUIRE(it.AtEnd());

        it = pageLast->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL));
        REQUIRE(it.AtEnd());

        countAllProperties = 0;
        for ( it = pgManager->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            countAllProperties++;
        }
        REQUIRE(countAllProperties == countAllPropertiesInit - propertiesPageFirstInit.size() - propertiesPageLastInit.size());
    }

    SECTION("Iterator_DeleteLastPageThenFirst")
    {
        // Get all properties from first page
        wxPGVIterator it;
        wxPropertyGridPage* pageFirst = pgManager->GetPage(0);
        std::vector<wxString> propertiesPageFirstInit;
        for ( it = pageFirst->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            propertiesPageFirstInit.push_back(it.GetProperty()->GetName());
        }

        // Get all properties from last page
        wxPropertyGridPage* pageLast = pgManager->GetPage(pgManager->GetPageCount() - 1);
        std::vector<wxString> propertiesPageLastInit;
        for ( it = pageLast->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            propertiesPageLastInit.push_back(it.GetProperty()->GetName());
        }

        int countAllPropertiesInit = 0;
        for ( it = pgManager->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            countAllPropertiesInit++;
        }

        // Delete all properties from last page
        pageLast->Clear();

        it = pageLast->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL));
        REQUIRE(it.AtEnd());

        std::vector<wxString> propertiesPageFirst;
        for ( it = pageFirst->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            propertiesPageFirst.push_back(it.GetProperty()->GetName());
        }
        REQUIRE(propertiesPageFirst == propertiesPageFirstInit);

        size_t countAllProperties = 0;
        for ( it = pgManager->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            countAllProperties++;
        }
        REQUIRE(countAllProperties == countAllPropertiesInit - propertiesPageLastInit.size());

        // Delete all properties from first page
        pageFirst->Clear();

        it = pageFirst->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL));
        REQUIRE(it.AtEnd());

        it = pageLast->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL));
        REQUIRE(it.AtEnd());

        countAllProperties = 0;
        for ( it = pgManager->GetVIterator(wxPG_ITERATOR_FLAGS_ALL | wxPG_IT_CHILDREN(wxPG_ITERATOR_FLAGS_ALL)); !it.AtEnd(); it.Next() )
        {
            countAllProperties++;
        }
        REQUIRE(countAllProperties == countAllPropertiesInit - propertiesPageFirstInit.size() - propertiesPageLastInit.size());
    }

    SECTION("SelectProperty")
    {
        // Test that setting focus to properties does not crash things

        for ( size_t i = 0; i < pgManager->GetPageCount(); i++ )
        {
            wxPropertyGridPage* page = pgManager->GetPage(i);
            pgManager->SelectPage(page);

            for ( auto it = page->GetIterator(wxPG_ITERATE_VISIBLE); !it.AtEnd(); ++it )
            {
                wxPGProperty* p = *it;
                INFO(p->GetLabel().c_str());
                pgManager->GetGrid()->SelectProperty(p, true);
                ::wxMilliSleep(150);
                wxTheApp->GetTopWindow()->Update();
            }
        }
        SUCCEED();
    }

    SECTION("DeleteProperty")
    {
        // Delete everything in reverse order

        std::vector<wxPGProperty*> array;
        for ( auto it = pgManager->GetVIterator(wxPG_ITERATE_ALL & ~(wxPG_IT_CHILDREN(wxPG_PROP_AGGREGATE))); !it.AtEnd(); it.Next() )
        {
            array.push_back(it.GetProperty());
        }

        for ( std::vector<wxPGProperty*>::reverse_iterator it2 = array.rbegin(); it2 != array.rend(); ++it2 )
        {
            wxPGProperty* p = *it2;
            INFO(wxString::Format("Deleting '%s' ('%s')", p->GetLabel(), p->GetName()).c_str());
            pgManager->DeleteProperty(p);
        }

        // Check if grid is empty.
        auto it = pgManager->GetVIterator(wxPG_ITERATE_ALL & ~(wxPG_IT_CHILDREN(wxPG_PROP_AGGREGATE)));
        if ( !it.AtEnd() )
        {
            FAIL_CHECK("Not all properties are deleted");
        }

        pgManager->Refresh();
        pgManager->Update();
        // Wait for update to be done
        wxStopWatch sw;
        while ( sw.Time() < 100 )
        {
            wxYield();
        }
    }

    SECTION("Default_Values")
    {
        // Test property default values

        for ( auto it = pgManager->GetVIterator(wxPG_ITERATE_PROPERTIES); !it.AtEnd(); it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            pgManager->SetPropertyValue(p, p->GetDefaultValue());
        }
        SUCCEED();
    }

    SECTION("wxVariant_to_wxAny_conversion")
    {
        // Test wxAny<->wxVariant conversion

        wxPGProperty* prop;
        wxAny any;

#if wxUSE_DATETIME
        prop = pgManager->GetProperty("DateProperty");
        wxDateTime testTime = wxDateTime::Now();
        any = testTime;
        prop->SetValue(any);
        CHECK(prop->GetValue().GetAny().As<wxDateTime>() == testTime);
#endif

        prop = pgManager->GetProperty("IntProperty");
        int testInt = 25537983;
        any = testInt;
        prop->SetValue(any);
        CHECK(prop->GetValue().GetAny().As<int>() == testInt);
#ifdef wxLongLong_t
        CHECK(prop->GetValue().GetAny().As<wxLongLong_t>() == testInt);
#endif

        prop = pgManager->GetProperty("StringProperty");
        wxString testString = "asd934jfyn3";
        any = testString;
        prop->SetValue(any);
        CHECK(prop->GetValue().GetAny().As<wxString>() == testString);

        // Test with a type generated with IMPLEMENT_VARIANT_OBJECT()
        prop = pgManager->GetProperty("ColourProperty");
        wxColour testCol = *wxCYAN;
        any = testCol;
        prop->SetValue(any);
        CHECK(prop->GetValue().GetAny().As<wxColour>() == testCol);
    }

    SECTION("GetPropertyValues")
    {
        for ( size_t i = 0; i < pgManager->GetPageCount(); i++ )
        {
            wxPropertyGridPage* page = pgManager->GetPage(i);

            wxVariant values = page->GetPropertyValues();

            for ( size_t j = 0; j < values.GetCount(); j++ )
            {
                wxVariant& v = values[j];

                INFO(wxString::Format("%i: name=\"%s\"  type=\"%s\"\n", (int)j,
                    v.GetName(), v.GetType()).c_str());
            }
        }
        SUCCEED();
    }

    SECTION("SetPropertyValue_and_GetPropertyValue")
    {
        // In this section, mixed up usage of "propname" and "propname"
        // in wxPropertyGridInterface functions is intentional.
        // Purpose is to test wxPGPropArgCls ctors.

        wxArrayString test_arrstr_1;
        test_arrstr_1.Add("Apple");
        test_arrstr_1.Add("Orange");
        test_arrstr_1.Add("Lemon");

        wxArrayString test_arrstr_2;
        test_arrstr_2.Add("Potato");
        test_arrstr_2.Add("Cabbage");
        test_arrstr_2.Add("Cucumber");

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
        dt1.SetYear(dt1.GetYear() - 1);

        wxDateTime dt2 = wxDateTime::Now();
        dt2.SetYear(dt2.GetYear() - 10);
#endif

        wxColour colWithAlpha(1, 128, 254, 100);
        wxString colWithAlphaStr(colWithAlpha.GetAsString(wxC2S_CSS_SYNTAX));

        pgManager->SetPropertyValue("StringProperty", "Text1");
        pgManager->SetPropertyValue("IntProperty", 1024);
        pgManager->SetPropertyValue("FloatProperty", 1024.0000000001);
        pgManager->SetPropertyValue("BoolProperty", false);
        pgManager->SetPropertyValue("EnumProperty", 120);
        pgManager->SetPropertyValue("ArrayStringProperty", test_arrstr_1);
        wxColour emptyCol;
        pgManager->SetPropertyValue("ColourProperty", emptyCol);
        pgManager->SetPropertyValue("ColourProperty", const_cast<wxObject*>(static_cast<const wxObject*>(wxBLACK)));
        pgManager->SetPropertyValue("MultiChoiceProperty", test_arrint_1);
#if wxUSE_DATETIME
        pgManager->SetPropertyValue("DateProperty", dt1);
#endif

        pgManager->SelectPage(1);
        wxPropertyGrid* pg = pgManager->GetGrid();

        CHECK(pg->GetPropertyValueAsString("StringProperty") == "Text1");
        CHECK(pg->GetPropertyValueAsInt("IntProperty") == 1024);
        CHECK(pg->GetPropertyValueAsDouble("FloatProperty") == 1024.0000000001);
        CHECK(pg->GetPropertyValueAsBool("BoolProperty") == false);
        CHECK(pg->GetPropertyValueAsLong("EnumProperty") == 120);
        CHECK(pg->GetPropertyValueAsArrayString("ArrayStringProperty") == test_arrstr_1);
        wxColour col;
        col << pgManager->GetPropertyValue("ColourProperty");
        CHECK(col == *wxBLACK);
        CHECK(pg->GetPropertyValueAsArrayInt("MultiChoiceProperty") == test_arrint_1);
#if wxUSE_DATETIME
        CHECK(pg->GetPropertyValueAsDateTime("DateProperty") == dt1);
#endif

#if wxUSE_LONGLONG && defined(wxLongLong_t)
        pgManager->SetPropertyValue("IntProperty", wxLL(10000000000));
        CHECK(pg->GetPropertyValueAsLongLong("IntProperty") == wxLL(10000000000));
#else
        pgman->SetPropertyValue("IntProperty", 1000000000);
        CHECK(pg->GetPropertyValueAsLong("IntProperty") == 1000000000);
#endif

        pg->SetPropertyValue("StringProperty", "Text2");
        pg->SetPropertyValue("IntProperty", 512);
        pg->SetPropertyValue("FloatProperty", 512.0);
        pg->SetPropertyValue("BoolProperty", true);
        pg->SetPropertyValue("EnumProperty", 80);
        pg->SetPropertyValue("ArrayStringProperty", test_arrstr_2);
        pg->SetPropertyValue("ColourProperty", const_cast<wxObject*>(static_cast<const wxObject*>(wxWHITE)));
        pg->SetPropertyValue("MultiChoiceProperty", test_arrint_2);
#if wxUSE_DATETIME
        pg->SetPropertyValue("DateProperty", dt2);
#endif

        pgManager->SelectPage(0);

        CHECK(pgManager->GetPropertyValueAsString("StringProperty") == "Text2");
        CHECK(pgManager->GetPropertyValueAsInt("IntProperty") == 512);
        CHECK(pgManager->GetPropertyValueAsDouble("FloatProperty") == 512.0);
        CHECK(pgManager->GetPropertyValueAsBool("BoolProperty") == true);
        CHECK(pgManager->GetPropertyValueAsLong("EnumProperty") == 80);
        CHECK(pgManager->GetPropertyValueAsArrayString("ArrayStringProperty") == test_arrstr_2);
        col << pgManager->GetPropertyValue("ColourProperty");
        CHECK(col == *wxWHITE);
        CHECK(pgManager->GetPropertyValueAsArrayInt("MultiChoiceProperty") == test_arrint_2);
#if wxUSE_DATETIME
        CHECK(pgManager->GetPropertyValueAsDateTime("DateProperty") == dt2);
#endif

#if wxUSE_LONGLONG && defined(wxLongLong_t)
        pgManager->SetPropertyValue("IntProperty", wxLL(-80000000000));
        CHECK(pgManager->GetPropertyValueAsLongLong("IntProperty") == wxLL(-80000000000));
#else
        pgManager->SetPropertyValue("IntProperty", -1000000000);
        CHECK(pgManager->GetPropertyValueAsLong("IntProperty") == -1000000000);
#endif

        wxString nvs = "Lamborghini Diablo XYZ; 5707; [100; 3.9; 8.6] 3000002; Convertible";
        pgManager->SetPropertyValue("Car", nvs);

        CHECK(pgManager->GetPropertyValueAsString("Car.Model") == "Lamborghini Diablo XYZ");
        CHECK(pgManager->GetPropertyValueAsInt("Car.Speeds.Max. Speed (mph)") == 100);
        CHECK(pgManager->GetPropertyValueAsInt("Car.Price ($)") == 3000002);
        CHECK(pgManager->GetPropertyValueAsBool("Car.Convertible") == true);

        // SetPropertyValueString for special cases such as wxColour
        pgManager->SetPropertyValueString("ColourProperty", "(123,4,255)");
        col << pgManager->GetPropertyValue("ColourProperty");
        CHECK(col == wxColour(123, 4, 255));
        pgManager->SetPropertyValueString("ColourProperty", "#FE860B");
        col << pgManager->GetPropertyValue("ColourProperty");
        CHECK(col == wxColour(254, 134, 11));

        pgManager->SetPropertyValueString("ColourPropertyWithAlpha", "(10, 20, 30, 128)");
        col << pgManager->GetPropertyValue("ColourPropertyWithAlpha");
        CHECK(col == wxColour(10, 20, 30, 128));
        CHECK(pgManager->GetPropertyValueAsString("ColourPropertyWithAlpha") == "(10,20,30,128)");
    }

    SECTION("SetPropertyValueUnspecified")
    {
        // Null variant setter tests
        pgManager->SetPropertyValueUnspecified("StringProperty");
        pgManager->SetPropertyValueUnspecified("IntProperty");
        pgManager->SetPropertyValueUnspecified("FloatProperty");
        pgManager->SetPropertyValueUnspecified("BoolProperty");
        pgManager->SetPropertyValueUnspecified("EnumProperty");
        pgManager->SetPropertyValueUnspecified("ArrayStringProperty");
        pgManager->SetPropertyValueUnspecified("ColourProperty");
        pgManager->SetPropertyValueUnspecified("MultiChoiceProperty");
#if wxUSE_DATETIME
        pgManager->SetPropertyValueUnspecified("DateProperty");
#endif
        SUCCEED();
    }

    SECTION("Multiple_selection")
    {
        // Test multiple selection
        if ( !(pgManager->GetExtraStyle() & wxPG_EX_MULTIPLE_SELECTION) )
            ReplaceGrid(pgManager, -1, wxPG_EX_MULTIPLE_SELECTION);

        wxPropertyGrid* pg = pgManager->GetGrid();

        wxPGProperty* prop1 = pg->GetProperty("Label");
        wxPGProperty* prop2 = pg->GetProperty("Cell Text Colour");
        wxPGProperty* prop3 = pg->GetProperty("Height");
        wxPGProperty* catProp = pg->GetProperty("Appearance");

        REQUIRE(prop1);
        REQUIRE(prop2);
        REQUIRE(prop3);

        pg->ClearSelection();
        pg->AddToSelection(prop1);
        pg->AddToSelection(prop2);
        pg->AddToSelection(prop3);

        // Adding category to selection should fail silently
        pg->AddToSelection(catProp);

        wxArrayPGProperty selectedProperties = pg->GetSelectedProperties();

        CHECK(selectedProperties.size() == 3);
        CHECK(pg->IsPropertySelected(prop1));
        CHECK(pg->IsPropertySelected(prop2));
        CHECK(pg->IsPropertySelected(prop3));
        CHECK_FALSE(pg->IsPropertySelected(catProp));

        pg->RemoveFromSelection(prop1);
        wxArrayPGProperty selectedProperties2 = pg->GetSelectedProperties();

        CHECK(selectedProperties2.size() == 2);
        CHECK_FALSE(pg->IsPropertySelected(prop1));
        CHECK(pg->IsPropertySelected(prop2));
        CHECK(pg->IsPropertySelected(prop3));

        pg->ClearSelection();

        wxArrayPGProperty selectedProperties3 = pg->GetSelectedProperties();

        CHECK(selectedProperties3.size() == 0);
        CHECK_FALSE(pg->IsPropertySelected(prop1));
        CHECK_FALSE(pg->IsPropertySelected(prop2));
        CHECK_FALSE(pg->IsPropertySelected(prop3));

        pg->SelectProperty(prop2);

        CHECK_FALSE(pg->IsPropertySelected(prop1));
        CHECK(pg->IsPropertySelected(prop2));
        CHECK_FALSE(pg->IsPropertySelected(prop3));
    }

    SECTION("GetMainParent")
    {
        // Test retrieving main parent of the property

        // Simple properties
        wxPGProperty* prop = pgManager->GetProperty("DateProperty");
        wxPGProperty* parent = prop->GetMainParent();
        CHECK(parent->GetName() == "DateProperty");

        prop = pgManager->GetProperty("Label");
        parent = prop->GetMainParent();
        CHECK(parent->GetName() == "Label");

        // Properties with children
        prop = pgManager->GetProperty("Font");
        REQUIRE(prop);
        parent = prop->GetMainParent();
        REQUIRE(parent);
        CHECK(parent->GetName() == "Font");

        prop = pgManager->GetProperty("Font.Style");
        REQUIRE(prop);
        parent = prop->GetMainParent();
        REQUIRE(parent);
        CHECK(parent->GetName() == "Font");

        prop = pgManager->GetProperty("Car");
        REQUIRE(prop);
        parent = prop->GetMainParent();
        REQUIRE(parent);
        CHECK(parent->GetName() == "Car");

        prop = pgManager->GetProperty("Car.Model");
        REQUIRE(prop);
        parent = prop->GetMainParent();
        REQUIRE(parent);
        CHECK(parent->GetName() == "Car");

        prop = pgManager->GetProperty("Car.Speeds");
        REQUIRE(prop);
        parent = prop->GetMainParent();
        REQUIRE(parent);
        CHECK(parent->GetName() == "Car");
    }

    SECTION("Label_editing")
    {
        // Test label editing

        wxPropertyGrid* pg = pgManager->GetGrid();

        // Just mostly test that these won't crash
        pg->MakeColumnEditable(0, true);
        pg->MakeColumnEditable(2, true);
        pg->MakeColumnEditable(0, false);
        pg->MakeColumnEditable(2, false);
        pg->SelectProperty("Height");
        pg->BeginLabelEdit(0);
        pg->BeginLabelEdit(0);
        pg->EndLabelEdit(0);
        pg->EndLabelEdit(0);
        SUCCEED();
    }

    SECTION("Attributes")
    {
        wxPGProperty* prop = pgManager->GetProperty("StringProperty");
        prop->SetAttribute("Dummy Attribute", 15L);

        CHECK(prop->GetAttribute("Dummy Attribute").GetLong() == 15L);

        prop->SetAttribute("Dummy Attribute", wxVariant());

        CHECK(prop->GetAttribute("Dummy Attribute").IsNull());
    }

    SECTION("Attributes_with_PGManager")
    {
        const long val = 25;
        pgManager->SetPropertyAttribute("IntProperty", "Dummy Attribute", val);
        CHECK(pgManager->GetPropertyAttribute("IntProperty", "Dummy Attribute").GetLong() == val);

        pgManager->SetPropertyAttribute("IntProperty", "Dummy Attribute", wxVariant());
        CHECK(pgManager->GetPropertyAttribute("IntProperty", "Dummy Attribute").IsNull());
    }

    SECTION("Getting_list_of_attributes")
    {
        wxPGProperty* prop = pgManager->GetProperty("Height");
        const wxPGAttributeStorage& attrs1 = prop->GetAttributes();
        CHECK(attrs1.GetCount() >= 1);

        const wxPGAttributeStorage& attrs2 = pgManager->GetPropertyAttributes("Height");
        CHECK(attrs2.GetCount() == attrs1.GetCount());

        // Compare both lists
        wxVariant val1;
        wxVariant val2;
        wxPGAttributeStorage::const_iterator it = attrs1.StartIteration();
        while ( attrs1.GetNext(it, val1) )
        {
            val2 = attrs2.FindValue(val1.GetName());
            CHECK(val1 == val2);
        }
    }

    SECTION("Copying_list_of_attributes")
    {
        wxPGAttributeStorage attrs1(pgManager->GetPropertyAttributes("Height"));
        CHECK(attrs1.GetCount() >= 1);

        wxPGAttributeStorage attrs2;
        attrs2 = attrs1;
        CHECK(attrs2.GetCount() == attrs1.GetCount());

        // Compare both lists
        wxVariant val1;
        wxVariant val2;
        wxPGAttributeStorage::const_iterator it = attrs1.StartIteration();
        while ( attrs1.GetNext(it, val1) )
        {
            val2 = attrs2.FindValue(val1.GetName());
            CHECK(val1 == val2);
        }
    }

    SECTION("MaxLength")
    {
        wxPGProperty* prop1 = pgManager->GetProperty("StringProperty");
        wxString propSetValStr = "12345678901234567890";
        prop1->SetValueFromString(propSetValStr);
        CHECK(prop1->SetMaxLength(10));
        CHECK(prop1->GetMaxLength() == 10);
        wxString propGetValStr = prop1->GetValueAsString();
        CHECK(propGetValStr == propSetValStr);
        prop1->SetValueFromString(propSetValStr);
        propGetValStr = prop1->GetValueAsString();
        CHECK(propGetValStr == propSetValStr);

        CHECK(prop1->SetMaxLength(-1));
        CHECK(prop1->GetMaxLength() == 0);
        propGetValStr = prop1->GetValueAsString();
        CHECK(propGetValStr == propSetValStr);

        wxPGProperty* prop2 = pgManager->GetProperty("LongStringProp");
        propSetValStr = "123456789012345678901234567890";
        prop2->SetValueFromString(propSetValStr);
        CHECK(prop2->SetMaxLength(20));
        CHECK(prop2->GetMaxLength() == 20);
        propGetValStr = prop2->GetValueAsString();
        CHECK(propGetValStr == propSetValStr);
        prop2->SetValueFromString(propSetValStr);
        propGetValStr = prop2->GetValueAsString();
        CHECK(propGetValStr == propSetValStr);

        wxPGProperty* prop3 = pgManager->GetProperty("IntProperty");
        const long propSetValInt = 1234567;
        prop3->SetValueFromInt(propSetValInt);
        CHECK(prop3->SetMaxLength(4));
        CHECK(prop3->GetMaxLength() == 4);
        int propGetValInt = prop3->GetValue().GetLong();
        CHECK(propGetValInt == propSetValInt);
        prop3->SetValueFromInt(propSetValInt);
        propGetValInt = prop3->GetValue().GetLong();
        CHECK(propGetValInt == propSetValInt);

        wxPGProperty* prop4 = pgManager->GetProperty("ArrayStringProperty");
        const wxString arrStr[3]{ "01234567890", "abcdefghijk", "ABCDEFGHIJK" };
        wxArrayString propSetValArrStr(WXSIZEOF(arrStr), arrStr);
        prop4->SetValue(wxVariant(propSetValArrStr));
        CHECK(prop4->SetMaxLength(25));
        CHECK(prop4->GetMaxLength() == 25);
        wxArrayString propGetValArrStr = prop4->GetValue().GetArrayString();
        CHECK(propGetValArrStr == propSetValArrStr);
        prop4->SetValueFromString(wxVariant(propSetValArrStr));
        propGetValStr = prop4->GetValueAsString();
        CHECK(propGetValArrStr == propSetValArrStr);

        wxPGProperty* prop5 = pgManager->GetProperty("EnumProperty");
        CHECK_FALSE(prop5->SetMaxLength(50));

        wxPGProperty* prop6 = pgManager->GetProperty("BoolProperty");
        CHECK_FALSE(prop6->SetMaxLength(60));
    }

    SECTION("MaxLength_with_PG")
    {
        pgManager->SelectPage(1);
        wxPropertyGrid* pg = pgManager->GetGrid();

        wxPGProperty* prop1 = pgManager->GetProperty("StringProperty");
        wxString propSetValStr = "12345678901234567890";
        pg->SetPropertyValue(prop1, propSetValStr);
        CHECK(pg->SetPropertyMaxLength("StringProperty", 15));
        CHECK(prop1->GetMaxLength() == 15);
        wxString propGetValStr = pg->GetPropertyValueAsString(prop1);
        CHECK(propGetValStr == propSetValStr);
        pg->SetPropertyValue(prop1, propSetValStr);
        propGetValStr = pg->GetPropertyValueAsString(prop1);
        CHECK(propGetValStr == propSetValStr);

        CHECK(pg->SetPropertyMaxLength("StringProperty", -1));
        CHECK(prop1->GetMaxLength() == 0);
        propGetValStr = pg->GetPropertyValueAsString(prop1);
        CHECK(propGetValStr == propSetValStr);

        wxPGProperty* prop2 = pgManager->GetProperty("LongStringProp");
        propSetValStr = "123456789012345678901234567890";
        pg->SetPropertyValue(prop2, propSetValStr);
        CHECK(pg->SetPropertyMaxLength("LongStringProp", 25));
        CHECK(prop2->GetMaxLength() == 25);
        propGetValStr = pg->GetPropertyValueAsString(prop2);
        CHECK(propGetValStr == propSetValStr);
        pg->SetPropertyValue(prop2, propSetValStr);
        propGetValStr = pg->GetPropertyValueAsString(prop2);
        CHECK(propGetValStr == propSetValStr);

        wxPGProperty* prop3 = pgManager->GetProperty("FloatProperty");
        double propSetValFloat = 1234.567;
        pg->SetPropertyValue(prop3, propSetValFloat);
        CHECK(pg->SetPropertyMaxLength("FloatProperty", 5));
        CHECK(prop3->GetMaxLength() == 5);
        double propGetValFloat = pg->GetPropertyValueAsDouble(prop3);
        CHECK(propGetValFloat == propSetValFloat);

        CHECK(pg->SetPropertyMaxLength("FloatProperty", -1));
        CHECK(prop3->GetMaxLength() == 0);
        propGetValFloat = pg->GetPropertyValueAsDouble(prop3);
        CHECK(propGetValFloat == propSetValFloat);

        CHECK_FALSE(pg->SetPropertyMaxLength("ColourProperty", 35));

        CHECK_FALSE(pg->SetPropertyMaxLength("BoolProperty", 3));
    }

#if WXWIN_COMPATIBILITY_3_0
    SECTION("DoubleToString")
    {
        // Locale-specific decimal separator
        wxString sep = wxString::Format("%g", 1.1)[1];

        wxString s;

        CHECK(wxPropertyGrid::DoubleToString(s, 123.123, 2, true) ==
            wxString::Format("123%s12", sep));
        CHECK(wxPropertyGrid::DoubleToString(s, -123.123, 4, false) ==
            wxString::Format("-123%s1230", sep));
        CHECK(wxPropertyGrid::DoubleToString(s, -0.02, 1, false) ==
            wxString::Format("0%s0", sep));
        CHECK(wxPropertyGrid::DoubleToString(s, -0.000123, 3, true) == "0");
    }
#endif

    SECTION("GetPropertyValues")
    {
        wxPropertyGridPage* page1 = pgManager->GetPage(0);
        wxVariant pg1_values = page1->GetPropertyValues("Page1", nullptr, wxPGPropertyValuesFlags::KeepStructure);
        wxPropertyGridPage* page2 = pgManager->GetPage(1);
        wxVariant pg2_values = page2->GetPropertyValues("Page2", nullptr, wxPGPropertyValuesFlags::KeepStructure);
        SUCCEED();

        SECTION("SetPropertyValues")
        {
            page1->SetPropertyValues(pg2_values);
            page2->SetPropertyValues(pg1_values);
            SUCCEED();
        }
    }

    SECTION("Collapse_and_GetFirstCategory_and_GetNextCategory")
    {
        for ( size_t i = 0; i < pgManager->GetPageCount(); i++ )
        {
            wxPropertyGridPage* page = pgManager->GetPage(i);

            for ( auto it = page->GetIterator(wxPG_ITERATE_CATEGORIES); !it.AtEnd(); ++it )
            {
                wxPGProperty* p = *it;

                CHECK(page->IsPropertyCategory(p));

                INFO(wxString::Format("Collapsing: %s\n", page->GetPropertyLabel(p)).c_str());
                page->Collapse(p);
            }
        }
    }

    SECTION("Save_And_RestoreEditableState")
    {
        for ( size_t i = 0; i < pgManager->GetPageCount(); i++ )
        {
            pgManager->SelectPage(i);

            wxString stringState = pgManager->SaveEditableState();
            CHECK(pgManager->RestoreEditableState(stringState));
        }
    }

    SECTION("Expand_and_GetFirstCategory_and_GetNextCategory")
    {
        for ( size_t i = 0; i < pgManager->GetPageCount(); i++ )
        {
            wxPropertyGridPage* page = pgManager->GetPage(i);

            for ( auto it = page->GetIterator(wxPG_ITERATE_CATEGORIES); !it.AtEnd(); ++it )
            {
                wxPGProperty* p = *it;

                CHECK(page->IsPropertyCategory(p));

                INFO(wxString::Format("Expand: %s\n", page->GetPropertyLabel(p)).c_str());
                page->Expand(p);
            }
        }
    }

    SECTION("Choice_Manipulation")
    {
        wxPGProperty* enumProp = pgManager->GetProperty("EnumProperty");

        pgManager->SelectPage(1);
        pgManager->SelectProperty(enumProp);
        REQUIRE(pgManager->GetGrid()->GetSelection() == enumProp);

        const wxPGChoices& choices = enumProp->GetChoices();
        int ind = enumProp->InsertChoice("New Choice", choices.GetCount() / 2);
        enumProp->DeleteChoice(ind);
        SUCCEED();
    }

    //if ( !pgman->HasFlag(wxPG_HIDE_CATEGORIES) )
    SECTION("RandomCollapse")
    {
        // Select the most error prone page as visible.
        pgManager->SelectPage(1);

        for ( size_t i = 0; i < pgManager->GetPageCount(); i++ )
        {
            wxPropertyGridPage* page = pgManager->GetPage(i);

            std::vector<wxPGProperty*> arr;
            for ( auto it = page->GetIterator(wxPG_ITERATE_CATEGORIES); !it.AtEnd(); ++it )
            {
                arr.push_back(*it);
            }

            if ( !arr.empty() )
            {
                pgManager->Collapse(arr[0]);

                for ( size_t n = arr.size() - 1; n > 0; n-- )
                {
                    pgManager->Collapse(arr[n]);
                }
            }
        }
        SUCCEED();
    }

    SECTION("EnsureVisible")
    {
        pgManager->EnsureVisible("Cell Colour");
        SUCCEED();
    }

    SECTION("RemoveProperty")
    {
        wxPGProperty* p;

        wxPGProperty* origParent = pgManager->GetProperty("Window Styles")->GetParent();

        // For testing purposes, let's set some custom cell colours
        p = pgManager->GetProperty("Window Styles");
        p->SetCell(2, wxPGCell("style"));
        p = pgManager->RemoveProperty("Window Styles");
        pgManager->Refresh();
        pgManager->Update();

        pgManager->AppendIn(origParent, p);
        CHECK(p->GetCell(2).GetText() == "style");
        pgManager->Refresh();
        pgManager->Update();
    }

    SECTION("SortFunction")
    {
        wxPGProperty* p;

        // Make sure indexes are as supposed
        p = pgManager->GetProperty("User Name");
        CHECK(p->GetIndexInParent() == 3);

        p = pgManager->GetProperty("User Id");
        CHECK(p->GetIndexInParent() == 2);

        p = pgManager->GetProperty("User Home");
        CHECK(p->GetIndexInParent() == 1);

        p = pgManager->GetProperty("Operating System");
        CHECK(p->GetIndexInParent() == 0);

        pgManager->GetGrid()->SetSortFunction(CustomPropertySortFunction);

        pgManager->GetGrid()->SortChildren("Environment");

        // Make sure indexes have been reversed
        p = pgManager->GetProperty("User Name");
        CHECK(p->GetIndexInParent() == 0);

        p = pgManager->GetProperty("User Id");
        CHECK(p->GetIndexInParent() == 1);

        p = pgManager->GetProperty("User Home");
        CHECK(p->GetIndexInParent() == 2);

        p = pgManager->GetProperty("Operating System");
        CHECK(p->GetIndexInParent() == 3);
    }

    SECTION("Clear_with_PGManager")
    {
        // Manager clear
        pgManager->SelectProperty("Label");
        pgManager->Clear();

        CHECK(pgManager->GetPageCount() == 0);

        CHECK_FALSE(pgManager->GetGrid()->GetRoot()->HasAnyChild());
    }

    SECTION("Clear_with_PG")
    {
        // Grid clear
        pgManager->SelectProperty("Label");
        pgManager->GetGrid()->Clear();

        CHECK_FALSE(pgManager->GetGrid()->GetRoot()->HasAnyChild());
    }

    SECTION("SetPropertyBackgroundColour")
    {
        wxPropertyGrid* pg = pgManager->GetGrid();
        REQUIRE(pg->GetPropertyByName("Appearance"));
        REQUIRE(pg->GetPropertyByName("PositionCategory"));
        REQUIRE(pg->GetPropertyByName("Environment"));
        REQUIRE(pg->GetPropertyByName("More Examples"));

        pgManager->Freeze();
        // Set custom colours.
        pg->SetPropertyTextColour("Appearance", wxColour(255, 0, 0), wxPGPropertyValuesFlags::DontRecurse);
        pg->SetPropertyBackgroundColour("Appearance", wxColour(255, 255, 183));
        pg->SetPropertyTextColour("Appearance", wxColour(255, 0, 183));
        pg->SetPropertyTextColour("PositionCategory", wxColour(0, 255, 0), wxPGPropertyValuesFlags::DontRecurse);
        pg->SetPropertyBackgroundColour("PositionCategory", wxColour(255, 226, 190));
        pg->SetPropertyTextColour("PositionCategory", wxColour(255, 0, 190));
        pg->SetPropertyTextColour("Environment", wxColour(0, 0, 255), wxPGPropertyValuesFlags::DontRecurse);
        pg->SetPropertyBackgroundColour("Environment", wxColour(208, 240, 175));
        pg->SetPropertyTextColour("Environment", wxColour(255, 255, 255));
        pg->SetPropertyBackgroundColour("More Examples", wxColour(172, 237, 255));
        pg->SetPropertyTextColour("More Examples", wxColour(172, 0, 255));
        pgManager->Thaw();
        pgManager->Refresh();

        wxColour my_grey_1(212, 208, 200);
        wxColour my_grey_2(241, 239, 226);
        wxColour my_grey_3(113, 111, 100);
        pgManager->Freeze();
        pgManager->GetGrid()->SetMarginColour(*wxWHITE);
        pgManager->GetGrid()->SetCaptionBackgroundColour(*wxWHITE);
        pgManager->GetGrid()->SetCellBackgroundColour(my_grey_2);
        pgManager->GetGrid()->SetCellBackgroundColour(my_grey_2);
        pgManager->GetGrid()->SetCellTextColour(my_grey_3);
        pgManager->GetGrid()->SetLineColour(my_grey_1);
        pgManager->Thaw();
        SUCCEED();
    }

    SECTION("SetSplitterPosition")
    {
        const int trySplitterPos = 50;

        int style = wxPG_AUTO_SORT;  // wxPG_SPLITTER_AUTO_CENTER;
        ReplaceGrid(pgManager, style, -1);

        pgManager->SetSplitterPosition(trySplitterPos);

        CHECK(pgManager->GetGrid()->GetSplitterPosition() == trySplitterPos);

        wxSize origSz = wxTheApp->GetTopWindow()->GetSize();

        wxSize sz = origSz;
        sz.IncBy(5, 5);
        wxTheApp->GetTopWindow()->SetSize(sz);

        CHECK(pgManager->GetGrid()->GetSplitterPosition() == trySplitterPos);

        wxTheApp->GetTopWindow()->SetSize(origSz);
    }

    SECTION("HideProperty")
    {
        wxPropertyGridPage* page = pgManager->GetPage(0);

        std::vector<wxPGProperty*> arr1 = GetPropertiesInRandomOrder(page);
        for ( size_t i = 0; i < arr1.size(); i++ )
        {
            wxPGProperty* p = arr1[i];
            page->HideProperty(p, true);

            INFO(wxString::Format("HideProperty(%i, %s)", (int)i, p->GetLabel()).c_str());
            unsigned int h1 = page->GetVirtualHeight();
            unsigned int h2 = page->GetActualVirtualHeight();
            REQUIRE(h1 == h2);
        }

        std::vector<wxPGProperty*> arr2 = GetPropertiesInRandomOrder(page);
        for ( size_t i = 0; i < arr2.size(); i++ )
        {
            wxPGProperty* p = arr2[i];
            page->HideProperty(p, false);

            INFO(wxString::Format("ShowProperty(%i, %s)", (int)i, p->GetLabel()).c_str());
            unsigned int h1 = page->GetVirtualHeight();
            unsigned int h2 = page->GetActualVirtualHeight();
            REQUIRE(h1 == h2);
        }

        //
        // Let's do some more, for better consistency
        arr1 = GetPropertiesInRandomOrder(page);
        for ( size_t i = 0; i < arr1.size(); i++ )
        {
            wxPGProperty* p = arr1[i];
            page->HideProperty(p, true);

            INFO(wxString::Format("HideProperty(%i, %s)", (int)i, p->GetLabel()).c_str());
            unsigned int h1 = page->GetVirtualHeight();
            unsigned int h2 = page->GetActualVirtualHeight();
            REQUIRE(h1 == h2);
        }

        arr2 = GetPropertiesInRandomOrder(page);
        for ( size_t i = 0; i < arr2.size(); i++ )
        {
            wxPGProperty* p = arr2[i];
            page->HideProperty(p, false);

            INFO(wxString::Format("ShowProperty(%i, %s)", (int)i, p->GetLabel()).c_str());
            unsigned int h1 = page->GetVirtualHeight();
            unsigned int h2 = page->GetActualVirtualHeight();
            REQUIRE(h1 == h2);
        }

        // Ok, this time only hide half of them
        arr1 = GetPropertiesInRandomOrder(page);
        arr1.resize(arr1.size() / 2);
        for ( size_t i = 0; i < arr1.size(); i++ )
        {
            wxPGProperty* p = arr1[i];
            page->HideProperty(p, true);

            INFO(wxString::Format("HideProperty(%i, %s)", (int)i, p->GetLabel()).c_str());
            unsigned int h1 = page->GetVirtualHeight();
            unsigned int h2 = page->GetActualVirtualHeight();
            REQUIRE(h1 == h2);
        }

        arr2 = GetPropertiesInRandomOrder(page);
        for ( size_t i = 0; i < arr2.size(); i++ )
        {
            wxPGProperty* p = arr2[i];
            page->HideProperty(p, false);

            INFO(wxString::Format("ShowProperty(%i, %s)", (int)i, p->GetLabel()).c_str());
            unsigned int h1 = page->GetVirtualHeight();
            unsigned int h2 = page->GetActualVirtualHeight();
            REQUIRE(h1 == h2);
        }
    }

    SECTION("SetFlagsAsString_GetFlagsAsString")
    {
        std::uniform_int_distribution<int> distrib(0, 1);

        // Select the most error prone page as visible.
        pgManager->SelectPage(1);

        for ( size_t i = 0; i < pgManager->GetPageCount(); i++ )
        {
            wxPropertyGridPage* page = pgManager->GetPage(i);

            for ( auto it = page->GetIterator(wxPG_ITERATE_VISIBLE); !it.AtEnd(); ++it )
            {
                wxPGProperty* p = *it;

                // Save initial flags
                wxPGProperty::FlagType oldFlags = 0;
                if ( p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    oldFlags |= wxPG_PROP_COLLAPSED;
                }
                if ( p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    oldFlags |= wxPG_PROP_DISABLED;
                }
                if ( p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    oldFlags |= wxPG_PROP_HIDDEN;
                }
                if ( p->HasFlag(wxPG_PROP_NOEDITOR) )
                {
                    oldFlags |= wxPG_PROP_NOEDITOR;
                }

                wxString flags;

                if ( p->IsCategory() )
                {
                    if ( distrib(s_rng) != 0 ) // random Boolean value
                    {
                        if ( !flags.empty() )
                        {
                            flags.append("|");
                        }
                        flags.append("COLLAPSED");
                    }
                }

                if ( distrib(s_rng) != 0 ) // random Boolean value
                {
                    if ( !flags.empty() )
                    {
                        flags.append("|");
                    }
                    flags.append("DISABLED");
                }

                if ( distrib(s_rng) != 0 ) // random Boolean value
                {
                    if ( !flags.empty() )
                    {
                        flags.append("|");
                    }
                    flags.append("HIDDEN");
                }

                // Set flags
                p->SetFlagsFromString(flags);

                // Verify if flags have been properly set
                if ( flags.Find("COLLAPSED") != wxNOT_FOUND &&
                    !p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    FAIL_CHECK(wxString::Format("Error setting flag from string 'COLLAPSED' for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("COLLAPSED") == wxNOT_FOUND &&
                    p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    FAIL_CHECK(wxString::Format("Error resetting flag from string 'COLLAPSED'for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("DISABLED") != wxNOT_FOUND &&
                    !p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    FAIL_CHECK(wxString::Format("Error setting flag from string 'DISABLED' for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("DISABLED") == wxNOT_FOUND &&
                    p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    FAIL_CHECK(wxString::Format("Error resetting flag from string 'DISABLED' for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("HIDDEN") != wxNOT_FOUND &&
                    !p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    FAIL_CHECK(wxString::Format("Error setting flag from string 'HIDDEN' for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("HIDDEN") == wxNOT_FOUND &&
                    p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    FAIL_CHECK(wxString::Format("Error resetting flag from string 'HIDDEN' for property '%s'",
                        p->GetName()).c_str());
                }

                // Get individual flags
                bool ok;

                flags = p->GetFlagsAsString(wxPG_PROP_COLLAPSED);
                if ( p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    ok = (flags == "COLLAPSED");
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPG_PROP_COLLAPSED flag for property '%s'",
                        p->GetName()).c_str());
                }

                flags = p->GetFlagsAsString(wxPG_PROP_DISABLED);
                if ( p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    ok = (flags == "DISABLED");
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPG_PROP_DISABLED flag for property '%s'",
                        p->GetName()).c_str());
                }

                flags = p->GetFlagsAsString(wxPG_PROP_HIDDEN);
                if ( p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    ok = (flags == "HIDDEN");
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPG_PROP_HIDDEN flag for property '%s'",
                        p->GetName()).c_str());
                }

                flags = p->GetFlagsAsString(wxPG_PROP_NOEDITOR);
                if ( p->HasFlag(wxPG_PROP_NOEDITOR) )
                {
                    ok = (flags == "NOEDITOR");
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPG_PROP_NOEDITOR flag for property '%s'",
                        p->GetName()).c_str());
                }

                // Get all flags
                flags = p->GetFlagsAsString(wxPG_STRING_STORED_FLAGS);
                if ( p->HasFlag(wxPG_PROP_COLLAPSED) )
                {
                    ok = (flags.Find("COLLAPSED") != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find("COLLAPSED") == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPG_PROP_COLLAPSED flag for property '%s'",
                        p->GetName()).c_str());
                }

                if ( p->HasFlag(wxPG_PROP_DISABLED) )
                {
                    ok = (flags.Find("DISABLED") != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find("DISABLED") == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPG_PROP_DISBALED flag for property '%s'",
                        p->GetName()).c_str());
                }

                if ( p->HasFlag(wxPG_PROP_HIDDEN) )
                {
                    ok = (flags.Find("HIDDEN") != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find("HIDDEN") == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPG_PROP_HIDDEN flag for property '%s'",
                        p->GetName()).c_str());
                }

                if ( p->HasFlag(wxPG_PROP_NOEDITOR) )
                {
                    ok = (flags.Find("NOEDITOR") != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find("NOEDITOR") == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPG_PROP_NOEDITOR flag for property '%s'",
                        p->GetName()).c_str());
                }

                // Restore original flags
                p->ChangeFlag(wxPG_PROP_COLLAPSED, (oldFlags & wxPG_PROP_COLLAPSED) != 0);
                p->ChangeFlag(wxPG_PROP_DISABLED, (oldFlags & wxPG_PROP_DISABLED) != 0);
                p->ChangeFlag(wxPG_PROP_HIDDEN, (oldFlags & wxPG_PROP_HIDDEN) != 0);
                p->ChangeFlag(wxPG_PROP_NOEDITOR, (oldFlags & wxPG_PROP_NOEDITOR) != 0);
            }
        }
    }

    SECTION("MultipleColumns")
    {
        // Test with multiple columns
        for ( int i = 3; i < 12; i += 2 )
        {
            INFO(wxString::Format("%i columns", i).c_str());
            pgManager->SetColumnCount(i);
            pgManager->Refresh();
            pgManager->Update();
            wxMilliSleep(500);
        }
        SUCCEED();
    }

    SECTION("WindowStyles")
    {
        // Recreate grid with all possible (single) flags
        REQUIRE(wxPG_AUTO_SORT == 0x000000010);

        for ( int i = 4; i < 16; i++ )
        {
            int flag = 1 << i;
            INFO(wxString::Format("Style: 0x%X", flag).c_str());
            ReplaceGrid(pgManager, flag, -1);
            pgManager->Update();
            wxMilliSleep(500);
        }
        SUCCEED();

        REQUIRE(wxPG_EX_INIT_NOCAT == 0x00001000);

        for ( int i = 12; i < 27; i++ )
        {
            int flag = 1 << i;
            INFO(wxString::Format("ExStyle: 0x%X", flag).c_str());
            ReplaceGrid(pgManager, -1, flag);
            pgManager->Update();
            wxMilliSleep(500);
        }
        SUCCEED();
    }
}
#endif // wxUSE_PROPGRID
