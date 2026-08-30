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
#include "wx/frame.h"
#if wxUSE_HEADERCTRL
#include "wx/headerctrl.h"
#endif
#if wxUSE_DATEPICKCTRL
#include "wx/datectrl.h"
#endif
#if wxUSE_TOOLBAR
#include "wx/toolbar.h"
#endif

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/manager.h"
#include "wx/propgrid/advprops.h"
#include "wx/odcombo.h"
#include "wx/weakref.h"

#ifdef __WXMSW__
#include "wx/msw/wrapwin.h"
#if wxUSE_TOOLBAR && !defined(__WXUNIVERSAL__) && !defined(__WXWINUI__)
#include "wx/msw/wrapcctl.h"
#endif
#endif

#ifdef __WXQT__
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#endif

#include "waitfor.h"

#include <functional>
#include <limits>
#include <random>
#include <vector>

// Private seams exported by the propgrid library for deterministic tests.
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_PROPGRID,
                         wxEVT_PG_COLS_RESIZED,
                         wxPropertyGridEvent);

#if wxUSE_HEADERCTRL
WXDLLIMPEXP_PROPGRID bool
wxPGProcessHeaderResizeEventForTesting(wxPropertyGridManager* manager,
                                       wxHeaderCtrlEvent& event);
#endif

#if wxUSE_TOOLBAR
WXDLLIMPEXP_PROPGRID void
wxPGManagerFailToolbarRemovalForTesting(unsigned int ordinal);
WXDLLIMPEXP_PROPGRID void
wxPGManagerResetToolbarRemovalFailuresForTesting();

class ToolbarRemovalFailureReset final
{
public:
    ToolbarRemovalFailureReset()
    {
        wxPGManagerResetToolbarRemovalFailuresForTesting();
    }

    ~ToolbarRemovalFailureReset()
    {
        wxPGManagerResetToolbarRemovalFailuresForTesting();
    }
};
#endif

#ifdef __WXMSW__
WXDLLIMPEXP_PROPGRID void
wxPGMSWFailNextDirectEditorParkingForTesting();
WXDLLIMPEXP_PROPGRID void
wxPGMSWFailNextFallbackEditorParkingForTesting();
WXDLLIMPEXP_PROPGRID void
wxPGMSWResetEditorParkingFailuresForTesting();
WXDLLIMPEXP_PROPGRID unsigned int
wxPGMSWGetEditorParkingHostCountForTesting();
WXDLLIMPEXP_PROPGRID unsigned int
wxPGMSWGetDeferredEditorBatchCountForTesting();
WXDLLIMPEXP_PROPGRID bool
wxPGMSWIsEditorParkingHostForTesting(WXWidget hwnd);

static bool IsMessageOnlyWindow(HWND target)
{
    HWND child = nullptr;
    while ( (child = ::FindWindowEx(HWND_MESSAGE, child, nullptr, nullptr)) )
    {
        if ( child == target )
            return true;
    }

    return false;
}

static HWND FindPropertyGridEditorParkingHost()
{
    HWND child = nullptr;
    while ( (child = ::FindWindowEx(HWND_MESSAGE,
                                    child,
                                    nullptr,
                                    nullptr)) )
    {
        if ( wxPGMSWIsEditorParkingHostForTesting(
                 reinterpret_cast<WXWidget>(child)) )
        {
            return child;
        }
    }

    return nullptr;
}

static bool HaveEqualWindowDpiContexts(HWND first, HWND second)
{
    typedef WXDPI_AWARENESS_CONTEXT
        (WINAPI *GetWindowContextFn)(HWND);
    typedef BOOL
        (WINAPI *ContextsEqualFn)(WXDPI_AWARENESS_CONTEXT,
                                  WXDPI_AWARENESS_CONTEXT);

    const HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
    if ( !user32 )
        return true;

    const GetWindowContextFn getWindowContext =
        reinterpret_cast<GetWindowContextFn>(
            ::GetProcAddress(user32, "GetWindowDpiAwarenessContext"));
    const ContextsEqualFn contextsEqual =
        reinterpret_cast<ContextsEqualFn>(
            ::GetProcAddress(user32, "AreDpiAwarenessContextsEqual"));
    if ( !getWindowContext || !contextsEqual )
        return true;

    return contextsEqual(getWindowContext(first),
                         getWindowContext(second)) != FALSE;
}

class EditorParkingFailureReset final
{
public:
    EditorParkingFailureReset()
    {
        wxPGMSWResetEditorParkingFailuresForTesting();
    }

    ~EditorParkingFailureReset()
    {
        wxPGMSWResetEditorParkingFailuresForTesting();
    }
};
#endif

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

    pg->Append(new wxUIntProperty("UIntProperty", wxPG_LABEL, wxULongLong(wxULL(0xFEEEFEEEFEEE))));
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
    YieldForAWhile(100);

    return pgManager;
}

static void ReplaceGrid(std::unique_ptr<wxPropertyGridManager>& pgManager, int style, int extraStyle)
{
    pgManager.reset(); // First destr0y previous instance
    pgManager.reset(CreateGrid(style, extraStyle));
    pgManager->SetFocus();
}

static void SendPropertyGridMouseEvent(wxPropertyGrid* grid,
                                       wxEventType type,
                                       const wxPoint& position,
                                       bool leftIsDown = false)
{
    wxMouseEvent event(type);
    event.SetId(grid->GetId());
    event.SetEventObject(grid);
    event.SetPosition(position);
    event.SetLeftDown(leftIsDown);
    grid->ProcessWindowEvent(event);
}

static void SendPropertyGridKeyEvent(wxPropertyGrid* grid,
                                     int keyCode,
                                     bool controlDown = false,
                                     bool shiftDown = false)
{
    wxKeyEvent event(wxEVT_KEY_DOWN);
    event.SetId(grid->GetId());
    event.SetEventObject(grid);
    event.m_keyCode = keyCode;
    event.SetControlDown(controlDown);
    event.SetShiftDown(shiftDown);
    grid->ProcessWindowEvent(event);
}

#if wxUSE_HEADERCTRL
static bool ProcessPropertyGridHeaderEvent(wxPropertyGridManager* manager,
                                           wxHeaderCtrlEvent& event)
{
    return wxPGProcessHeaderResizeEventForTesting(manager, event);
}
#endif

class ReentrantPropertyGrid : public wxPropertyGrid
{
public:
    static constexpr size_t DragOffsetStorageSizeForTest()
    {
        return sizeof(m_dragOffset);
    }

    using wxPropertyGrid::wxPropertyGrid;

    void DrawItemsForTest(wxDC& dc, const wxRect& rect)
    {
        DrawItems(dc,
                  static_cast<unsigned int>(rect.GetTop()),
                  static_cast<unsigned int>(rect.GetBottom()),
                   &rect);
    }

    void UseSoftwareDoubleBufferForTest()
    {
        SetExtraStyle(GetExtraStyle() &
                      ~wxPG_EX_NATIVE_DOUBLE_BUFFERING);
        wxSizeEvent event(GetSize(), GetId());
        OnResize(event);
    }

    wxBitmap GetSoftwareDoubleBufferForTest() const
    {
        return m_doubleBuffer ? *m_doubleBuffer : wxBitmap();
    }

    void InvalidateSoftwareDoubleBufferForTest()
    {
        wxASSERT( m_doubleBuffer );
        *m_doubleBuffer = wxBitmap();
    }

    void ResizeAndReplaceSoftwareDoubleBufferForTest(const wxSize& size)
    {
        SetSize(size);
        wxSizeEvent event(GetSize(), GetId());
        OnResize(event);
    }

    wxSize GetBestSizeForTest() const
    {
        return DoGetBestSize();
    }

    bool SendChangingEventForTest(wxPGProperty* property,
                                  wxVariant* value)
    {
        return SendEvent(wxEVT_PG_CHANGING,
                         property,
                         value,
                         wxPGSelectPropertyFlags::Null);
    }

    void BeginLabelEditWithEvent(unsigned int column)
    {
        DoBeginLabelEdit(column);
    }

    void EndLabelEditWithEvent(bool commit)
    {
        DoEndLabelEdit(commit);
    }

    void RepositionEditorForTest()
    {
        CorrectEditorWidgetPosY();
    }

    bool SelectFromInputForTest(wxPGProperty* property,
                                unsigned int column,
                                wxMouseEvent* event)
    {
        return AddToSelectionFromInputEvent(property,
                                            column,
                                            event,
                                            wxPGSelectPropertyFlags::Null);
    }

    bool SelectAndEditForTest(wxPGProperty* property, unsigned int column)
    {
        return DoSelectAndEdit(property,
                               column,
                               wxPGSelectPropertyFlags::Null);
    }

    void SetEditorAppearanceForTest(const wxPGCell& cell)
    {
        SetEditorAppearance(cell, false);
    }

    bool ValidateEditorForTest()
    {
        return DoEditorValidate();
    }

    void SetCurrentFocusedForTest(wxWindow* window)
    {
        m_curFocused = window;
    }

    bool MoveSplitterFromKeyboardForTest(int splitter, int delta)
    {
        return MoveSplitterFromKeyboard(splitter, delta);
    }

    void DestroyOnNextExpand(
        std::unique_ptr<ReentrantPropertyGrid>* owner)
    {
        m_destroyOnExpandOwner = owner;
    }

    void FinishSplitterDragForTest(bool cancel)
    {
        FinishSplitterDrag(cancel);
    }

#ifndef __WXMSW__
    void AfterNextCaptureReleaseForTest(std::function<void()> callback)
    {
        m_afterCaptureRelease = std::move(callback);
    }
#endif

    bool MoveOverPropertyForTest(wxPGProperty* property, unsigned int column)
    {
        const wxRect rect = GetPropertyRect(property, property);
        const int x = column == 0 ? 4 : GetSplitterPosition() + 8;
        const int y = rect.GetTop() + GetRowHeight() / 2;
        wxMouseEvent event(wxEVT_MOTION);
        event.SetEventObject(this);
        event.SetPosition(wxPoint(x, y));
        return HandleMouseMove(x, static_cast<unsigned int>(y), event);
    }

    void DPIChangedForTest(wxDPIChangedEvent& event)
    {
        OnDPIChanged(event);
    }

protected:
#ifndef __WXMSW__
    void DoReleaseMouse() override
    {
        // wxEVT_MOUSE_CAPTURE_CHANGED is MSW-only. On other ports exercise
        // the same reentrant boundary after their real capture release.
        const auto callback = std::move(m_afterCaptureRelease);
        wxPropertyGrid::DoReleaseMouse();
        if ( callback )
            callback();
    }
#endif

    bool DoExpand(wxPGProperty* property, bool sendEvent = false) override
    {
        const bool result = wxPropertyGrid::DoExpand(property, sendEvent);
        std::unique_ptr<ReentrantPropertyGrid>* const owner =
            m_destroyOnExpandOwner;
        m_destroyOnExpandOwner = nullptr;
        if ( owner )
            owner->reset();
        return result;
    }

private:
#ifndef __WXMSW__
    std::function<void()> m_afterCaptureRelease;
#endif
    std::unique_ptr<ReentrantPropertyGrid>* m_destroyOnExpandOwner = nullptr;
};

static_assert(ReentrantPropertyGrid::DragOffsetStorageSizeForTest() ==
                  sizeof(signed char),
              "wxPropertyGrid::m_dragOffset must keep its legacy ABI type");

class ReentrantPropertyGridManager final : public wxPropertyGridManager
{
public:
    ReentrantPropertyGrid* GetReentrantGrid()
    {
        return static_cast<ReentrantPropertyGrid*>(GetGrid());
    }

    bool ProcessEventForTest(wxEvent& event)
    {
        return ProcessEvent(event);
    }

protected:
    wxPropertyGrid* CreatePropertyGrid() const override
    {
        return new ReentrantPropertyGrid;
    }
};

class DestroyingCellRenderer final : public wxPGCellRenderer
{
public:
    explicit DestroyingCellRenderer(bool* called)
        : m_called(called)
    {
    }

    void Arm(wxWindow* destroyTarget = nullptr)
    {
        m_destroyTarget = destroyTarget;
        m_armed = true;
    }

    bool Render(wxDC& WXUNUSED(dc),
                const wxRect& WXUNUSED(rect),
                const wxPropertyGrid* propertyGrid,
                wxPGProperty* WXUNUSED(property),
                int WXUNUSED(column),
                int WXUNUSED(item),
                int WXUNUSED(flags)) const override
    {
        if ( m_armed )
        {
            m_armed = false;
            *m_called = true;
            wxWindow* const destroyTarget = m_destroyTarget;
            m_destroyTarget = nullptr;
            if ( destroyTarget )
                destroyTarget->Destroy();
            else
                delete const_cast<wxPropertyGrid*>(propertyGrid);
        }
        return true;
    }

private:
    bool* m_called;
    mutable wxWindow* m_destroyTarget = nullptr;
    mutable bool m_armed = false;
};

class DestroyingRendererProperty final : public wxStringProperty
{
public:
    explicit DestroyingRendererProperty(bool* called)
        : wxStringProperty("Destroying renderer", wxPG_LABEL, "value")
        , m_renderer(called)
    {
    }

    void Arm(wxWindow* destroyTarget = nullptr)
    {
        m_renderer.Arm(destroyTarget);
    }

    wxPGCellRenderer* GetCellRenderer(int WXUNUSED(column)) const override
    {
        return &m_renderer;
    }

private:
    mutable DestroyingCellRenderer m_renderer;
};

class RemovingCellRenderer final : public wxPGCellRenderer
{
public:
    explicit RemovingCellRenderer(bool* called)
        : m_called(called)
    {
    }

    void Arm() { m_armed = true; }

    bool Render(wxDC& WXUNUSED(dc),
                const wxRect& WXUNUSED(rect),
                const wxPropertyGrid* propertyGrid,
                wxPGProperty* property,
                int WXUNUSED(column),
                int WXUNUSED(item),
                int WXUNUSED(flags)) const override
    {
        if ( m_armed )
        {
            m_armed = false;
            *m_called = true;
            const_cast<wxPropertyGrid*>(propertyGrid)
                ->DeleteProperty(property);
        }
        return true;
    }

private:
    bool* const m_called;
    mutable bool m_armed = false;
};

class RemovingRendererProperty final : public wxStringProperty
{
public:
    explicit RemovingRendererProperty(bool* called)
        : wxStringProperty("Removing renderer", wxPG_LABEL, "value")
        , m_renderer(called)
    {
    }

    void Arm() { m_renderer.Arm(); }

    wxPGCellRenderer* GetCellRenderer(int WXUNUSED(column)) const override
    {
        return &m_renderer;
    }

private:
    mutable RemovingCellRenderer m_renderer;
};

class ResizingCellRenderer final : public wxPGCellRenderer
{
public:
    ResizingCellRenderer(bool* called,
                         bool* bufferReplaced,
                         bool* dcWasValid)
        : m_called(called)
        , m_bufferReplaced(bufferReplaced)
        , m_dcWasValid(dcWasValid)
    {
    }

    void Arm(const wxBitmap& bufferBefore)
    {
        m_bufferBefore = bufferBefore;
        m_armed = true;
    }

    bool Render(wxDC& dc,
                const wxRect& WXUNUSED(rect),
                const wxPropertyGrid* propertyGrid,
                wxPGProperty* WXUNUSED(property),
                int WXUNUSED(column),
                int WXUNUSED(item),
                int WXUNUSED(flags)) const override
    {
        if ( m_armed )
        {
            m_armed = false;
            *m_called = true;
            if ( m_dcWasValid )
                *m_dcWasValid = dc.IsOk();

            ReentrantPropertyGrid* const grid =
                static_cast<ReentrantPropertyGrid*>(
                    const_cast<wxPropertyGrid*>(propertyGrid));
            grid->ResizeAndReplaceSoftwareDoubleBufferForTest(
                wxSize(720, 500));

            if ( m_bufferReplaced )
            {
                const wxBitmap bufferAfter =
                    grid->GetSoftwareDoubleBufferForTest();
                *m_bufferReplaced =
                    m_bufferBefore.IsOk() && bufferAfter.IsOk() &&
                    !m_bufferBefore.IsSameAs(bufferAfter);
            }
        }
        return true;
    }

private:
    bool* const m_called;
    bool* const m_bufferReplaced;
    bool* const m_dcWasValid;
    wxBitmap m_bufferBefore;
    mutable bool m_armed = false;
};

class ResizingRendererProperty final : public wxStringProperty
{
public:
    ResizingRendererProperty(bool* called,
                             bool* bufferReplaced,
                             bool* dcWasValid)
        : wxStringProperty("Resizing renderer", wxPG_LABEL, "value")
        , m_renderer(called, bufferReplaced, dcWasValid)
    {
    }

    void Arm(const wxBitmap& bufferBefore)
    {
        m_renderer.Arm(bufferBefore);
    }

    wxPGCellRenderer* GetCellRenderer(int WXUNUSED(column)) const override
    {
        return &m_renderer;
    }

private:
    mutable ResizingCellRenderer m_renderer;
};

class ColumnMutatingCellRenderer final : public wxPGCellRenderer
{
public:
    void Arm() { m_armed = true; }
    int GetCallsAfterArm() const { return m_callsAfterArm; }

    bool Render(wxDC& WXUNUSED(dc),
                const wxRect& WXUNUSED(rect),
                const wxPropertyGrid* propertyGrid,
                wxPGProperty* WXUNUSED(property),
                int WXUNUSED(column),
                int WXUNUSED(item),
                int WXUNUSED(flags)) const override
    {
        if ( m_armed )
        {
            ++m_callsAfterArm;
            if ( m_callsAfterArm == 1 )
            {
                const_cast<wxPropertyGrid*>(propertyGrid)
                    ->SetColumnCount(2);
            }
        }
        return true;
    }

private:
    mutable bool m_armed = false;
    mutable int m_callsAfterArm = 0;
};

class ColumnMutatingRendererProperty final : public wxStringProperty
{
public:
    explicit ColumnMutatingRendererProperty(const wxString& label)
        : wxStringProperty(label, wxPG_LABEL, "value")
    {
    }

    void Arm() { m_renderer.Arm(); }
    int GetCallsAfterArm() const { return m_renderer.GetCallsAfterArm(); }

    wxPGCellRenderer* GetCellRenderer(int WXUNUSED(column)) const override
    {
        return &m_renderer;
    }

private:
    mutable ColumnMutatingCellRenderer m_renderer;
};

class DestroyingValueStringProperty final : public wxStringProperty
{
public:
    using wxStringProperty::GetValueAsString;

    explicit DestroyingValueStringProperty(bool* called)
        : wxStringProperty("Destroying value", wxPG_LABEL, "value")
        , m_called(called)
    {
    }

    void Arm(wxPropertyGrid* grid, wxWindow* destroyTarget = nullptr)
    {
        m_grid = grid;
        m_destroyTarget = destroyTarget;
        m_armed = true;
    }

    wxString GetValueAsString(
        wxPGPropValFormatFlags flags =
            wxPGPropValFormatFlags::Null) const override
    {
        const wxString value =
            wxStringProperty::GetValueAsString(flags);
        if ( m_armed )
        {
            m_armed = false;
            *m_called = true;
            wxPropertyGrid* const grid = m_grid;
            wxWindow* const destroyTarget = m_destroyTarget;
            m_grid = nullptr;
            m_destroyTarget = nullptr;
            if ( destroyTarget )
                destroyTarget->Destroy();
            else
                delete grid;
        }
        return value;
    }

private:
    bool* m_called;
    mutable wxPropertyGrid* m_grid = nullptr;
    mutable wxWindow* m_destroyTarget = nullptr;
    mutable bool m_armed = false;
};

class DestroyingTextEditor : public wxPGEditor
{
public:
    void Arm()
    {
        m_armed = true;
    }

    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                          wxPGProperty* property,
                                          const wxPoint& pos,
                                          const wxSize& size) const override
    {
        return wxPGEditor_TextCtrl->CreateControls(propgrid,
                                                   property,
                                                   pos,
                                                   size);
    }

    virtual void UpdateControl(wxPGProperty* property,
                               wxWindow* ctrl) const override
    {
        wxPGEditor_TextCtrl->UpdateControl(property, ctrl);
    }

    virtual bool OnEvent(wxPropertyGrid* WXUNUSED(propgrid),
                         wxPGProperty* WXUNUSED(property),
                         wxWindow* primaryCtrl,
                         wxEvent& event) const override
    {
        // Selecting the property also forwards focus events to the custom
        // editor. Only destroy the control for the explicitly armed text
        // event below: destroying it from inside its own focus processing is
        // not a supported window-lifetime operation and doesn't exercise the
        // HandleCustomEditorEvent() path this test is about.
        if ( !m_armed || event.GetEventType() != wxEVT_TEXT )
            return false;

        m_armed = false;
        // This test invokes HandleCustomEditorEvent() directly, so the
        // property-grid event forwarder isn't active on the stack. Remove and
        // destroy it before deleting the window, just as FreeEditors() does;
        // wxWindow deliberately asserts if a pushed handler is left dangling.
        delete primaryCtrl->PopEventHandler(false);
        delete primaryCtrl;
        return true;
    }

    virtual bool GetValueFromControl(wxVariant& value,
                                     wxPGProperty* property,
                                     wxWindow* ctrl) const override
    {
        return wxPGEditor_TextCtrl->GetValueFromControl(value,
                                                        property,
                                                        ctrl);
    }

private:
    mutable bool m_armed = false;
};

void DestroyPropertyGridEditorControl(wxWindow* control)
{
    while ( control && control->GetEventHandler() != control )
        delete control->PopEventHandler(false);
    delete control;
}

class DestroyingFocusSink final : public wxTextCtrl
{
public:
    using wxTextCtrl::wxTextCtrl;

    void Arm(wxWindow* control, const std::function<void()>& afterDestroy)
    {
        m_control = control;
        m_afterDestroy = afterDestroy;
        m_called = false;
    }

    bool WasCalled() const { return m_called; }

    void SetFocus() override
    {
        wxWindow* const control = m_control;
        m_control = nullptr;
        if ( control )
        {
            m_called = true;
            DestroyPropertyGridEditorControl(control);
            m_afterDestroy();
        }
        wxTextCtrl::SetFocus();
    }

private:
    wxWindow* m_control = nullptr;
    std::function<void()> m_afterDestroy;
    bool m_called = false;
};

class AcceptingValidator final : public wxValidator
{
public:
    wxObject* Clone() const override { return new AcceptingValidator; }
    bool Validate(wxWindow*) override { return true; }
    bool TransferToWindow() override { return true; }
    bool TransferFromWindow() override { return true; }
};

class DestroyingResultValidator final : public wxValidator
{
public:
    wxObject* Clone() const override
    {
        return new DestroyingResultValidator;
    }

    void Arm(wxWindow* control,
             bool result,
             wxWindow* destroyTarget = nullptr)
    {
        m_control = control;
        m_destroyTarget = destroyTarget;
        m_result = result;
        m_called = false;
    }

    bool WasCalled() const { return m_called; }

    bool Validate(wxWindow*) override
    {
        m_called = true;
        wxWindow* const control = m_control;
        wxWindow* const destroyTarget = m_destroyTarget;
        m_control = nullptr;
        m_destroyTarget = nullptr;
        if ( destroyTarget )
            destroyTarget->Destroy();
        else
            DestroyPropertyGridEditorControl(control);
        return m_result;
    }

    bool TransferToWindow() override { return true; }
    bool TransferFromWindow() override { return true; }

private:
    wxWindow* m_control = nullptr;
    wxWindow* m_destroyTarget = nullptr;
    bool m_result = false;
    bool m_called = false;
};

class DestroyingValidationProperty final : public wxStringProperty
{
public:
    explicit DestroyingValidationProperty(const wxString& label)
        : wxStringProperty(label, wxPG_LABEL, "value")
    {
    }

    void ArmValidationFailure(wxWindow* control)
    {
        m_control = control;
        m_failValidation = true;
        m_failureCallbackCalled = false;
    }

    void ArmValidator(wxWindow* control,
                      bool result,
                      wxWindow* destroyTarget = nullptr)
    {
        m_validator.Arm(control, result, destroyTarget);
    }

    bool ValidatorWasCalled() const { return m_validator.WasCalled(); }
    bool FailureCallbackWasCalled() const
    {
        return m_failureCallbackCalled;
    }

    bool ValidateValue(wxVariant&,
                       wxPGValidationInfo&) const override
    {
        return !m_failValidation;
    }

    void OnValidationFailure(wxVariant&) override
    {
        m_failureCallbackCalled = true;
        m_failValidation = false;
        wxWindow* const control = m_control;
        m_control = nullptr;
        DestroyPropertyGridEditorControl(control);
    }

protected:
    wxValidator* DoGetValidator() const override
    {
        return const_cast<DestroyingResultValidator*>(&m_validator);
    }

private:
    mutable DestroyingResultValidator m_validator;
    mutable wxWindow* m_control = nullptr;
    mutable bool m_failValidation = false;
    mutable bool m_failureCallbackCalled = false;
};

class DestroyingLookupProperty final : public wxStringProperty
{
public:
    enum class Callback
    {
        EditorClass,
        Validator
    };

    DestroyingLookupProperty(
        const wxString& label,
        Callback callback,
        const wxPGEditor* editor = wxPGEditor_TextCtrl)
        : wxStringProperty(label, wxPG_LABEL, "value"),
          m_callback(callback),
          m_editor(editor)
    {
    }

    void Arm(wxWindow* control)
    {
        m_control = control;
        m_armed = true;
    }

    bool WasCalled() const { return m_called; }

    const wxPGEditor* DoGetEditorClass() const override
    {
        if ( m_callback == Callback::EditorClass )
            DestroyControlIfArmed();
        return m_editor;
    }

    wxValidator* DoGetValidator() const override
    {
        if ( m_callback == Callback::Validator )
            DestroyControlIfArmed();
        return &m_validator;
    }

private:
    void DestroyControlIfArmed() const
    {
        if ( !m_armed )
            return;

        m_armed = false;
        m_called = true;
        wxWindow* const control = m_control;
        m_control = nullptr;
        DestroyPropertyGridEditorControl(control);
    }

    Callback m_callback;
    const wxPGEditor* m_editor;
    mutable wxWindow* m_control = nullptr;
    mutable bool m_armed = false;
    mutable bool m_called = false;
    mutable AcceptingValidator m_validator;
};

class DestroyingGetValueEditor : public wxPGEditor
{
public:
    void Arm(wxWindow* destroyTarget = nullptr)
    {
        m_destroyTarget = destroyTarget;
        m_armed = true;
        m_called = false;
    }
    bool WasCalled() const { return m_called; }

    wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                  wxPGProperty* property,
                                  const wxPoint& pos,
                                  const wxSize& size) const override
    {
        return wxPGEditor_TextCtrl->CreateControls(propgrid,
                                                   property,
                                                   pos,
                                                   size);
    }

    void UpdateControl(wxPGProperty* property,
                       wxWindow* ctrl) const override
    {
        wxPGEditor_TextCtrl->UpdateControl(property, ctrl);
    }

    bool OnEvent(wxPropertyGrid* propgrid,
                 wxPGProperty* property,
                 wxWindow* primaryCtrl,
                 wxEvent& event) const override
    {
        return wxPGEditor_TextCtrl->OnEvent(propgrid,
                                            property,
                                            primaryCtrl,
                                            event);
    }

    bool GetValueFromControl(wxVariant& value,
                             wxPGProperty* WXUNUSED(property),
                             wxWindow* ctrl) const override
    {
        if ( m_armed )
        {
            m_armed = false;
            m_called = true;
            value = "destroyed editor";
            wxWindow* const destroyTarget = m_destroyTarget;
            m_destroyTarget = nullptr;
            if ( destroyTarget )
                destroyTarget->Destroy();
            else
                DestroyPropertyGridEditorControl(ctrl);
            return true;
        }
        return false;
    }

private:
    mutable bool m_armed = false;
    mutable bool m_called = false;
    mutable wxWindow* m_destroyTarget = nullptr;
};

class DestroyingCustomImageEditor final : public DestroyingGetValueEditor
{
public:
    void Arm(wxPropertyGrid* grid, wxPGProperty* property)
    {
        m_grid = grid;
        m_property = property;
        m_called = false;
    }

    bool WasCalled() const { return m_called; }

    bool CanContainCustomImage() const override
    {
        if ( m_grid )
        {
            m_called = true;
            wxPropertyGrid* const grid = m_grid;
            wxPGProperty* const property = m_property;
            m_grid = nullptr;
            m_property = nullptr;
            grid->DeleteProperty(property);
        }
        return true;
    }

private:
    mutable wxPropertyGrid* m_grid = nullptr;
    mutable wxPGProperty* m_property = nullptr;
    mutable bool m_called = false;
};

class RemovingOnShowEditor final : public DestroyingGetValueEditor
{
public:
    void Arm(wxPropertyGrid* grid, wxPGProperty* property)
    {
        m_grid = grid;
        m_property = property;
        m_called = false;
        m_primary.Release();
    }

    bool WasCalled() const { return m_called; }
    wxWindow* GetCreatedControl() const { return m_primary.get(); }

    wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                  wxPGProperty* property,
                                  const wxPoint& pos,
                                  const wxSize& size) const override
    {
        wxPGWindowList windows =
            DestroyingGetValueEditor::CreateControls(propgrid,
                                                     property,
                                                     pos,
                                                     size);
        if ( wxWindow* const primary = windows.GetPrimary() )
        {
            // Make the later Show(true) in DoSelectProperty() observable on
            // every backend, even if the native Create() shows controls.
            primary->Hide();
            m_primary = primary;
            primary->Bind(wxEVT_SHOW,
                          [this](wxShowEvent& event)
                          {
                              event.Skip();
                               if ( event.IsShown() && m_grid )
                               {
                                   m_called = true;
                                  wxPropertyGrid* const grid = m_grid;
                                  wxPGProperty* const property = m_property;
                                  m_grid = nullptr;
                                  m_property = nullptr;
                                  grid->DeleteProperty(property);
                              }
                          });
        }
        return windows;
    }

private:
    mutable wxPropertyGrid* m_grid = nullptr;
    mutable wxPGProperty* m_property = nullptr;
    mutable bool m_called = false;
    mutable wxWeakRef<wxWindow> m_primary;
};

class DestroyingSecondaryEditor final : public wxPGEditor
{
public:
    enum class Callback
    {
        Focus,
        Show
    };

    void Arm(Callback callback)
    {
        m_callback = callback;
        m_called = false;
        m_secondary.Release();
    }

    bool WasCalled() const { return m_called; }
    wxWindow* GetSecondary() const { return m_secondary.get(); }

    wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                  wxPGProperty* property,
                                  const wxPoint& pos,
                                  const wxSize& size) const override
    {
        wxPGWindowList windows =
            wxPGEditor_TextCtrlAndButton->CreateControls(propgrid,
                                                         property,
                                                         pos,
                                                         size);
        m_secondary = windows.GetSecondary();
        if ( m_callback == Callback::Show )
        {
            if ( wxWindow* const primary = windows.GetPrimary() )
            {
                primary->Hide();
                primary->Bind(wxEVT_SHOW,
                              [this](wxShowEvent& event)
                              {
                                  event.Skip();
                                  if ( event.IsShown() )
                                      DestroySecondary();
                              });
            }
        }
        return windows;
    }

    void UpdateControl(wxPGProperty* property,
                       wxWindow* ctrl) const override
    {
        wxPGEditor_TextCtrlAndButton->UpdateControl(property, ctrl);
    }

    bool OnEvent(wxPropertyGrid* propgrid,
                 wxPGProperty* property,
                 wxWindow* primaryCtrl,
                 wxEvent& event) const override
    {
        return wxPGEditor_TextCtrlAndButton->OnEvent(propgrid,
                                                     property,
                                                     primaryCtrl,
                                                     event);
    }

    bool GetValueFromControl(wxVariant& value,
                             wxPGProperty* property,
                             wxWindow* ctrl) const override
    {
        return wxPGEditor_TextCtrlAndButton->GetValueFromControl(value,
                                                                 property,
                                                                 ctrl);
    }

    void OnFocus(wxPGProperty* property, wxWindow* ctrl) const override
    {
        wxPGEditor_TextCtrlAndButton->OnFocus(property, ctrl);
        if ( m_callback == Callback::Focus )
            DestroySecondary();
    }

private:
    void DestroySecondary() const
    {
        wxWindow* const secondary = m_secondary.get();
        if ( !secondary )
            return;

        m_called = true;
        m_secondary.Release();
        DestroyPropertyGridEditorControl(secondary);
    }

    mutable wxWeakRef<wxWindow> m_secondary;
    Callback m_callback = Callback::Focus;
    mutable bool m_called = false;
};

#ifdef __WXMSW__
class DestroyingCreateControlsEditor : public wxPGEditor
{
public:
    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                          wxPGProperty* property,
                                          const wxPoint& pos,
                                          const wxSize& size) const override
    {
        wxPGWindowList windows =
            wxPGEditor_TextCtrl->CreateControls(propgrid,
                                                property,
                                                pos,
                                                size);
        wxWindow* const primary = windows.GetPrimary();
        m_createdEditor = wxWindowRef(primary);
        m_createdHwnd =
            primary
                ? reinterpret_cast<HWND>(primary->GetHandle())
                : nullptr;

        delete propgrid;

        // The DoSelectProperty callback epoch must keep the newly created,
        // not-yet-assigned control alive across nested idle processing.
        wxYield();
        m_survivedNestedYield = m_createdEditor.get() != nullptr;
        return windows;
    }

    virtual void UpdateControl(wxPGProperty* property,
                               wxWindow* ctrl) const override
    {
        wxPGEditor_TextCtrl->UpdateControl(property, ctrl);
    }

    virtual bool OnEvent(wxPropertyGrid* propgrid,
                         wxPGProperty* property,
                         wxWindow* primaryCtrl,
                         wxEvent& event) const override
    {
        return wxPGEditor_TextCtrl->OnEvent(
            propgrid, property, primaryCtrl, event);
    }

    virtual bool GetValueFromControl(wxVariant& value,
                                     wxPGProperty* property,
                                     wxWindow* ctrl) const override
    {
        return wxPGEditor_TextCtrl->GetValueFromControl(
            value, property, ctrl);
    }

    bool SurvivedNestedYield() const
    {
        return m_survivedNestedYield;
    }

    bool IsEditorAlive() const
    {
        return m_createdEditor.get() != nullptr;
    }

    HWND GetCreatedHwnd() const
    {
        return m_createdHwnd;
    }

private:
    mutable wxWindowRef m_createdEditor;
    mutable HWND m_createdHwnd = nullptr;
    mutable bool m_survivedNestedYield = false;
};

class SameWindowEditor : public wxPGEditor
{
public:
    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                          wxPGProperty* property,
                                          const wxPoint& pos,
                                          const wxSize& size) const override
    {
        wxPGWindowList windows =
            wxPGEditor_TextCtrl->CreateControls(propgrid,
                                                property,
                                                pos,
                                                size);
        return wxPGWindowList(windows.GetPrimary(),
                              windows.GetPrimary());
    }

    virtual void UpdateControl(wxPGProperty* property,
                               wxWindow* ctrl) const override
    {
        wxPGEditor_TextCtrl->UpdateControl(property, ctrl);
    }

    virtual bool OnEvent(wxPropertyGrid* propgrid,
                         wxPGProperty* property,
                         wxWindow* primaryCtrl,
                         wxEvent& event) const override
    {
        return wxPGEditor_TextCtrl->OnEvent(
            propgrid, property, primaryCtrl, event);
    }

    virtual bool GetValueFromControl(wxVariant& value,
                                     wxPGProperty* property,
                                     wxWindow* ctrl) const override
    {
        return wxPGEditor_TextCtrl->GetValueFromControl(
            value, property, ctrl);
    }
};
#endif

class SelfDeletingValidationGrid : public ReentrantPropertyGrid
{
public:
    explicit SelfDeletingValidationGrid(wxWindow* parent)
        : ReentrantPropertyGrid(parent,
                                wxID_ANY,
                                wxDefaultPosition,
                                wxSize(400, 240))
    {
    }

    virtual bool DoEditorValidate() override
    {
        delete this;
        return true;
    }
};

class DestroyingDialogAdapter : public wxPGEditorDialogAdapter
{
public:
    explicit DestroyingDialogAdapter(bool destroyGrid)
        : m_destroyGrid(destroyGrid)
    {
    }

    virtual bool DoShowDialog(wxPropertyGrid* propGrid,
                              wxPGProperty* WXUNUSED(property)) override
    {
        m_called = true;
        if ( m_destroyGrid )
            delete propGrid;
        else
            SetValue("dialog value");
        return true;
    }

    bool WasCalled() const { return m_called; }

private:
    bool m_destroyGrid;
    bool m_called = false;
};

class ReentrantMeasureProperty : public wxStringProperty
{
public:
    enum class Action
    {
        None,
        RemoveProperty,
        DestroyGrid,
        SwitchPage
    };

    explicit ReentrantMeasureProperty(const wxString& name)
        : wxStringProperty(name, wxPG_LABEL, "value")
    {
    }

    void Arm(wxPropertyGrid* grid, Action action)
    {
        m_grid = grid;
        m_action = action;
    }

    void Arm(wxPropertyGridManager* manager, int targetPage)
    {
        m_manager = manager;
        m_targetPage = targetPage;
        m_action = Action::SwitchPage;
    }

    bool WasCalled() const { return m_wasCalled; }

    virtual wxSize OnMeasureImage(int WXUNUSED(item) = -1) const override
    {
        wxPropertyGrid* const grid = m_grid;
        wxPropertyGridManager* const manager = m_manager;
        const int targetPage = m_targetPage;
        const Action action = m_action;
        m_grid = nullptr;
        m_manager = nullptr;
        m_targetPage = wxNOT_FOUND;
        m_action = Action::None;

        if ( action != Action::None )
            m_wasCalled = true;

        if ( action == Action::RemoveProperty )
            grid->DeleteProperty(const_cast<ReentrantMeasureProperty*>(this));
        else if ( action == Action::DestroyGrid )
            delete grid;
        else if ( action == Action::SwitchPage )
            manager->SelectPage(targetPage);

        return wxSize(16, 16);
    }

private:
    mutable wxPropertyGrid* m_grid = nullptr;
    mutable wxPropertyGridManager* m_manager = nullptr;
    mutable int m_targetPage = wxNOT_FOUND;
    mutable Action m_action = Action::None;
    mutable bool m_wasCalled = false;
};

class ReentrantShowPage : public wxPropertyGridPage
{
public:
    void Arm(wxPropertyGridManager* manager, int targetPage)
    {
        m_managerForTest = manager;
        m_targetPage = targetPage;
        m_armed = true;
    }

    virtual void OnShow() override
    {
        if ( m_armed )
        {
            m_armed = false;
            m_managerForTest->SelectPage(m_targetPage);
        }
    }

private:
    wxPropertyGridManager* m_managerForTest = nullptr;
    int m_targetPage = wxNOT_FOUND;
    bool m_armed = false;
};

class SameTargetShowPage : public wxPropertyGridPage
{
public:
    void Arm(wxPropertyGridManager* manager, int targetPage)
    {
        m_managerForTest = manager;
        m_targetPage = targetPage;
    }

    virtual void OnShow() override
    {
        ++m_showCount;
        if ( m_showCount < 32 )
            m_managerForTest->SelectPage(m_targetPage);
    }

    int GetShowCount() const { return m_showCount; }

private:
    wxPropertyGridManager* m_managerForTest = nullptr;
    int m_targetPage = wxNOT_FOUND;
    int m_showCount = 0;
};

class ClientWidthNotificationPage : public wxPropertyGridPage
{
public:
    void NotifyClientWidthChange(int newWidth, int widthChange)
    {
        OnClientWidthChange(newWidth, widthChange, true);
    }
};

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
            else if ( p->GetParent()->HasFlag(wxPGFlags::Aggregate) )
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
            if ( p->GetParent()->HasFlag(wxPGFlags::Aggregate) )
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
            else if ( p->HasFlag(wxPGFlags::Hidden) )
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
        for ( auto it = pgManager->GetVIterator(wxPG_ITERATE_ALL & ~(wxPG_IT_CHILDREN(wxPGFlags::Aggregate))); !it.AtEnd(); it.Next() )
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
        auto it = pgManager->GetVIterator(wxPG_ITERATE_ALL & ~(wxPG_IT_CHILDREN(wxPGFlags::Aggregate)));
        if ( !it.AtEnd() )
        {
            FAIL_CHECK("Not all properties are deleted");
        }

        pgManager->Refresh();
        pgManager->Update();
        // Wait for update to be done
        YieldForAWhile(100);
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
        CHECK(prop->GetValue().GetAny().As<wxLongLong_t>() == testInt);

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
        if ( dt1.GetMonth() == wxDateTime::Feb && dt1.GetDay() == 29 )
            dt1.SetDay(28);

        wxDateTime dt2 = dt1;
        dt1.SetYear(dt1.GetYear() - 1);
        dt2.SetYear(dt2.GetYear() - 10);
#endif

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

        pgManager->SetPropertyValue("IntProperty", wxLL(10000000000));
        CHECK(pg->GetPropertyValueAsLongLong("IntProperty") == wxLL(10000000000));

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

        pgManager->SetPropertyValue("IntProperty", wxLL(-80000000000));
        CHECK(pgManager->GetPropertyValueAsLongLong("IntProperty") == wxLL(-80000000000));

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

        // The first edit of an initially empty secondary cell must persist;
        // historically DoEndLabelEdit() only wrote cells that already had
        // text and silently discarded this value.
        pgManager->SetColumnCount(3);
        pg->MakeColumnEditable(2, true);
        wxPGProperty* const height = pgManager->GetProperty("Height");
        REQUIRE( height );
        REQUIRE( pg->SelectProperty(height) );
        pg->BeginLabelEdit(2);
        wxTextCtrl* const editor = pg->GetLabelEditor();
        REQUIRE( editor );
        editor->ChangeValue("secondary label");
        pg->EndLabelEdit(true);
        CHECK( height->GetCell(2).GetText() == "secondary label" );
    }

    SECTION("Label_editing_reentrancy")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Editable", wxPG_LABEL, "value"));
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property) );

        int beginEvents = 0;
        pg->Bind(
            wxEVT_PG_LABEL_EDIT_BEGIN,
            [pg, &beginEvents](wxPropertyGridEvent&)
            {
                ++beginEvents;
                // A nested begin request must not create a second editor.
                pg->BeginLabelEditWithEvent(2);
            });

        pg->BeginLabelEditWithEvent(0);
        CHECK( beginEvents == 1 );
        REQUIRE( pg->GetLabelEditor() );

        const wxString originalLabel = property->GetLabel();
        int endingEvents = 0;
        pg->Bind(
            wxEVT_PG_LABEL_EDIT_ENDING,
            [pg, &endingEvents](wxPropertyGridEvent&)
            {
                ++endingEvents;
                // Cancelling from the public ending callback destroys the
                // editor. The outer commit must detect that stale transaction.
                pg->EndLabelEditWithEvent(false);
            });

        pg->GetLabelEditor()->ChangeValue("must not be committed");
        pg->EndLabelEditWithEvent(true);
        CHECK( endingEvents == 1 );
        CHECK_FALSE( pg->GetLabelEditor() );
        CHECK( property->GetLabel() == originalLabel );
    }

    SECTION("Label_editing_destroy_during_callback")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Editable", wxPG_LABEL, "value"));
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property) );

        const wxWeakRef<wxWindow> weakGrid(pg);
        pg->Bind(
            wxEVT_PG_LABEL_EDIT_BEGIN,
            [&grid](wxPropertyGridEvent&)
            {
                grid.reset();
            });

        pg->BeginLabelEditWithEvent(0);
        CHECK_FALSE( weakGrid );
    }

    SECTION("Label_begin_removal_aborts_editor_creation")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Begin removal",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        REQUIRE( pg->SelectProperty(property) );
        pg->Bind(wxEVT_PG_LABEL_EDIT_BEGIN,
                 [pg, property](wxPropertyGridEvent&)
                 {
                     pg->DeleteProperty(property);
                 });

        pg->BeginLabelEditWithEvent(0);

        CHECK_FALSE( pg->GetLabelEditor() );
        CHECK( pg->GetPropertyByName("Begin removal") == nullptr );
    }

    SECTION("Label_ending_removal_never_commits_pending_property")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Ending removal",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        REQUIRE( pg->SelectProperty(property) );
        pg->BeginLabelEditWithEvent(0);
        REQUIRE( pg->GetLabelEditor() );
        pg->GetLabelEditor()->ChangeValue("must not commit");
        pg->Bind(wxEVT_PG_LABEL_EDIT_ENDING,
                 [pg, property](wxPropertyGridEvent& event)
                 {
                     pg->DeleteProperty(property);
                     event.Veto();
                 });

        pg->EndLabelEditWithEvent(true);

        CHECK_FALSE( pg->GetLabelEditor() );
        CHECK( pg->GetPropertyByName("Ending removal") == nullptr );
    }

    SECTION("Label_ending_veto_cannot_retain_destroying_editor")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Vetoed editor destruction",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        REQUIRE( pg->SelectProperty(property) );
        pg->BeginLabelEditWithEvent(0);
        wxTextCtrl* const editor = pg->GetLabelEditor();
        REQUIRE(editor);
        const wxWeakRef<wxWindow> weakEditor(editor);
        pg->Bind(wxEVT_PG_LABEL_EDIT_ENDING,
                 [editor](wxPropertyGridEvent& event)
                 {
                     editor->Destroy();
                     event.Veto();
                 });

        pg->EndLabelEditWithEvent(true);

        CHECK_FALSE( pg->GetLabelEditor() );
        CHECK( pg->GetSelection() == property );
        wxTheApp->ProcessIdle();
        REQUIRE( WaitFor("PropertyGrid vetoed label editor destruction",
                         [&weakEditor]() { return !weakEditor; }) );
    }

    SECTION("Editor_modified_during_changed_event")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPGProperty* const property = pgManager->GetProperty("Label");
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property, true) );

        wxTextCtrl* const editor = pg->GetEditorTextCtrl();
        REQUIRE( editor );

        int changedEvents = 0;
        bool nestedCommitResult = false;
        pgManager->Bind(
            wxEVT_PG_CHANGED,
            [pg, property, &changedEvents,
             &nestedCommitResult](wxPropertyGridEvent& event)
            {
                if ( event.GetProperty() != property )
                    return;

                ++changedEvents;
                if ( changedEvents == 1 )
                {
                    wxTextCtrl* const currentEditor = pg->GetEditorTextCtrl();
                    REQUIRE( currentEditor );
                    currentEditor->ChangeValue("second");
                    pg->EditorsValueWasModified();
                    nestedCommitResult = pg->CommitChangesFromEditor();
                }
            });

        editor->ChangeValue("first");
        pg->EditorsValueWasModified();
        REQUIRE( pg->CommitChangesFromEditor() );
        CHECK( property->GetValueAsString() == "first" );
        CHECK( pg->IsEditorsValueModified() );
        CHECK( nestedCommitResult );

        REQUIRE( pg->CommitChangesFromEditor() );
        CHECK( property->GetValueAsString() == "second" );
        CHECK_FALSE( pg->IsEditorsValueModified() );
        CHECK( changedEvents == 2 );
    }

    SECTION("Custom_editor_may_destroy_primary_control")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        static DestroyingTextEditor editor;
        wxPGProperty* const property =
            pgManager->Append(
                new wxStringProperty("Destroying editor",
                                     "DestroyingEditor",
                                     "value"));
        REQUIRE( property );
        property->SetEditor(&editor);
        REQUIRE( pg->SelectProperty(property, true) );

        wxWindow* const primary = pg->GetEditorControl();
        REQUIRE( primary );
        const wxWeakRef<wxWindow> weakPrimary(primary);
        wxTextCtrl* const text = wxDynamicCast(primary, wxTextCtrl);
        REQUIRE( text );

        text->ChangeValue("trigger destruction");
        editor.Arm();
        wxCommandEvent textEvent(wxEVT_TEXT, primary->GetId());
        textEvent.SetEventObject(primary);
        CHECK_FALSE( pg->HandleCustomEditorEvent(textEvent) );

        CHECK_FALSE( weakPrimary );
        CHECK( pg->GetEditorControl() == nullptr );
        CHECK( pg->GetSelection() == property );
    }

    SECTION("Commit_editor_class_callback_may_destroy_primary_control")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        DestroyingLookupProperty* const property =
            new DestroyingLookupProperty(
                "Destroying editor lookup",
                DestroyingLookupProperty::Callback::EditorClass);
        REQUIRE( pg->Append(property) == property );
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE(editor);
        const wxWeakRef<wxWindow> weakEditor(editor);
        int changedEvents = 0;
        pg->Bind(wxEVT_PG_CHANGED,
                 [&changedEvents](wxPropertyGridEvent&)
                 {
                     ++changedEvents;
                 });

        property->Arm(editor);
        pg->EditorsValueWasModified();
        CHECK_FALSE( pg->CommitChangesFromEditor() );

        CHECK( property->WasCalled() );
        CHECK_FALSE( weakEditor );
        CHECK( pg->GetSelection() == property );
        CHECK( pg->GetEditorControl() == nullptr );
        CHECK( property->GetValueAsString() == "value" );
        CHECK( changedEvents == 0 );
    }

    SECTION("Commit_editor_class_callback_may_destroy_secondary_control")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        DestroyingLookupProperty* const property =
            new DestroyingLookupProperty(
                "Destroying secondary lookup",
                DestroyingLookupProperty::Callback::EditorClass,
                wxPGEditor_TextCtrlAndButton);
        REQUIRE( pg->Append(property) == property );
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE(secondary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);

        property->Arm(secondary);
        pg->EditorsValueWasModified();
        CHECK_FALSE( pg->CommitChangesFromEditor() );

        CHECK( property->WasCalled() );
        CHECK_FALSE( weakSecondary );
        CHECK( pg->GetEditorControlSecondary() == nullptr );
        CHECK( pg->GetEditorControl() != nullptr );
        CHECK( property->GetValueAsString() == "value" );
    }

    SECTION("Commit_get_value_callback_may_destroy_primary_control")
    {
        static DestroyingGetValueEditor destroyingEditor;
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Destroying value editor",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        property->SetEditor(&destroyingEditor);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE(editor);
        const wxWeakRef<wxWindow> weakEditor(editor);
        int changedEvents = 0;
        pg->Bind(wxEVT_PG_CHANGED,
                 [&changedEvents](wxPropertyGridEvent&)
                 {
                     ++changedEvents;
                 });

        destroyingEditor.Arm();
        pg->EditorsValueWasModified();
        CHECK_FALSE( pg->CommitChangesFromEditor() );

        CHECK( destroyingEditor.WasCalled() );
        CHECK_FALSE( weakEditor );
        CHECK( pg->GetSelection() == property );
        CHECK( pg->GetEditorControl() == nullptr );
        CHECK( property->GetValueAsString() == "value" );
        CHECK( changedEvents == 0 );
    }

    SECTION("Validator_lookup_may_destroy_primary_control")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        DestroyingLookupProperty* const property =
            new DestroyingLookupProperty(
                "Destroying validator lookup",
                DestroyingLookupProperty::Callback::Validator);
        REQUIRE( pg->Append(property) == property );
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE(editor);
        const wxWeakRef<wxWindow> weakEditor(editor);

        property->Arm(editor);
        CHECK_FALSE( pg->ValidateEditorForTest() );

        CHECK( property->WasCalled() );
        CHECK_FALSE( weakEditor );
        CHECK( pg->GetSelection() == property );
        CHECK( pg->GetEditorControl() == nullptr );
        CHECK( pg->ValidateEditorForTest() );
    }

    SECTION("Validator_false_result_retires_destroyed_primary_control")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        DestroyingValidationProperty* const property =
            new DestroyingValidationProperty("False validator primary");
        REQUIRE( pg->Append(property) == property );
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const primary = pg->GetEditorControl();
        REQUIRE(primary);
        const wxWeakRef<wxWindow> weakPrimary(primary);

        property->ArmValidator(primary, false);
        CHECK_FALSE( pg->ValidateEditorForTest() );

        CHECK( property->ValidatorWasCalled() );
        CHECK_FALSE( weakPrimary );
        CHECK( pg->GetEditorControl() == nullptr );
        CHECK( pg->GetEditorControlSecondary() != nullptr );
    }

    SECTION("Validator_false_result_retires_destroyed_secondary_control")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        DestroyingValidationProperty* const property =
            new DestroyingValidationProperty("False validator secondary");
        REQUIRE( pg->Append(property) == property );
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE(secondary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);

        property->ArmValidator(secondary, false);
        CHECK_FALSE( pg->ValidateEditorForTest() );

        CHECK( property->ValidatorWasCalled() );
        CHECK_FALSE( weakSecondary );
        CHECK( pg->GetEditorControlSecondary() == nullptr );
        CHECK( pg->GetEditorControl() != nullptr );
    }

    SECTION("Custom_editor_validation_failure_retires_secondary_control")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        DestroyingValidationProperty* const property =
            new DestroyingValidationProperty("Event validator secondary");
        REQUIRE( pg->Append(property) == property );
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        REQUIRE( pg->SelectProperty(property, true) );
        wxTextCtrl* const primary =
            wxDynamicCast(pg->GetEditorControl(), wxTextCtrl);
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE(primary);
        REQUIRE(secondary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);
        primary->ChangeValue("invalid event value");
        property->ArmValidator(secondary, false);
        // A text change only marks the editor dirty. Enter requests validation
        // and exercises the false-result cleanup in HandleCustomEditorEvent().
        pg->EditorsValueWasModified();
        wxCommandEvent textEvent(wxEVT_TEXT_ENTER, primary->GetId());
        textEvent.SetEventObject(primary);

        CHECK( pg->HandleCustomEditorEvent(textEvent) );

        CHECK( property->ValidatorWasCalled() );
        CHECK_FALSE( weakSecondary );
        CHECK( pg->GetEditorControlSecondary() == nullptr );
        CHECK( pg->GetEditorControl() == primary );
        CHECK( pg->GetSelection() == property );
    }

    SECTION("Validation_failure_callback_may_destroy_secondary_control")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        DestroyingValidationProperty* const property =
            new DestroyingValidationProperty("Failure secondary");
        REQUIRE( pg->Append(property) == property );
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        pg->SetValidationFailureBehavior(wxPGVFBFlags::Null);
        REQUIRE( pg->SelectProperty(property, true) );
        wxTextCtrl* const primary =
            wxDynamicCast(pg->GetEditorControl(), wxTextCtrl);
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE(primary);
        REQUIRE(secondary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);

        property->ArmValidationFailure(secondary);
        primary->ChangeValue("invalid");
        pg->EditorsValueWasModified();
        CHECK_FALSE( pg->CommitChangesFromEditor() );

        CHECK( property->FailureCallbackWasCalled() );
        CHECK_FALSE( weakSecondary );
        CHECK( pg->GetEditorControlSecondary() == nullptr );
        CHECK( pg->GetEditorControl() == primary );
        CHECK( property->GetValueAsString() == "value" );
    }

    SECTION("Validation_focus_restore_may_destroy_secondary_control")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        DestroyingValidationProperty* const property =
            new DestroyingValidationProperty("Focus restore secondary");
        REQUIRE( pg->Append(property) == property );
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        pg->SetValidationFailureBehavior(wxPGVFBFlags::Null);
        REQUIRE( pg->SelectProperty(property, true) );
        wxTextCtrl* const primary =
            wxDynamicCast(pg->GetEditorControl(), wxTextCtrl);
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE(primary);
        REQUIRE(secondary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);
        DestroyingFocusSink focusSink(wxTheApp->GetTopWindow(), wxID_ANY);
        unsigned int reentrantFocusEvents = 0;
        focusSink.Arm(secondary, [&]()
        {
            REQUIRE_FALSE( weakSecondary );

            // Cocoa sends a synchronous focus loss from the surviving text
            // editor after the button has died, before SetFocus() returns.
            // Exercise the same forwarding boundary on every port, keeping
            // both the real deletion above and the native SetFocus() below.
            const wxEventType focusTypes[] =
                { wxEVT_KILL_FOCUS, wxEVT_SET_FOCUS };
            for ( wxEventType type : focusTypes )
            {
                wxFocusEvent event(type, primary->GetId());
                event.SetEventObject(primary);
                event.SetWindow(&focusSink);
                primary->GetEventHandler()->ProcessEvent(event);
                ++reentrantFocusEvents;
            }
        });
        pg->SetCurrentFocusedForTest(&focusSink);
        property->ArmValidationFailure(nullptr);
        primary->ChangeValue("invalid");
        pg->EditorsValueWasModified();

        CHECK_FALSE( pg->CommitChangesFromEditor() );

        CHECK( focusSink.WasCalled() );
        CHECK( reentrantFocusEvents == 2 );
        CHECK_FALSE( property->FailureCallbackWasCalled() );
        CHECK_FALSE( weakSecondary );
        CHECK( pg->GetEditorControlSecondary() == nullptr );
        CHECK( pg->GetEditorControl() == primary );
        CHECK( property->GetValueAsString() == "value" );
    }

    SECTION("Selection_focus_callback_may_remove_property")
    {
        class FocusCallbackTextCtrl final : public wxTextCtrl
        {
        public:
            using wxTextCtrl::wxTextCtrl;

            void Arm(wxPropertyGrid* grid, wxPGProperty* property, bool& called)
            {
                m_grid = grid;
                m_property = property;
                m_called = &called;
            }

            void SetFocus() override
            {
                if ( m_grid )
                {
                    wxPropertyGrid* const grid = m_grid;
                    wxPGProperty* const property = m_property;
                    bool* const called = m_called;
                    m_grid = nullptr;
                    m_property = nullptr;
                    m_called = nullptr;
                    *called = true;
                    grid->DeleteProperty(property);
                    return;
                }

                wxTextCtrl::SetFocus();
            }

        private:
            wxPropertyGrid* m_grid = nullptr;
            wxPGProperty* m_property = nullptr;
            bool* m_called = nullptr;
        };

        // A local wxPGTextCtrlEditor subclass would reset the built-in editor
        // pointer in its base destructor. Delegate without owning that editor.
        class FocusCallbackEditor final : public wxPGEditor
        {
        public:
            wxPGWindowList CreateControls(wxPropertyGrid* grid,
                                          wxPGProperty* property,
                                          const wxPoint& pos,
                                          const wxSize& size) const override
            {
                return wxPGWindowList(
                    new FocusCallbackTextCtrl(grid->GetPanel(), wxID_ANY,
                                               property->GetValueAsString(),
                                               pos, size, wxTE_PROCESS_ENTER));
            }

            void UpdateControl(wxPGProperty* property,
                               wxWindow* ctrl) const override
            {
                wxPGEditor_TextCtrl->UpdateControl(property, ctrl);
            }

            bool OnEvent(wxPropertyGrid* grid,
                         wxPGProperty* property,
                         wxWindow* ctrl,
                         wxEvent& event) const override
            {
                return wxPGEditor_TextCtrl->OnEvent(grid, property, ctrl, event);
            }

            bool GetValueFromControl(wxVariant& value,
                                     wxPGProperty* property,
                                     wxWindow* ctrl) const override
            {
                return wxPGEditor_TextCtrl->GetValueFromControl(value,
                                                               property, ctrl);
            }

            void OnFocus(wxPGProperty* property, wxWindow* ctrl) const override
            {
                wxPGEditor_TextCtrl->OnFocus(property, ctrl);
            }
        } focusEditor;

        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Focus removal",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        property->SetEditor(&focusEditor);
        REQUIRE( pg->SelectProperty(property, true) );
        FocusCallbackTextCtrl* const editor =
            static_cast<FocusCallbackTextCtrl*>(pg->GetEditorControl());
        REQUIRE(editor);
        bool focusSeen = false;
        int selectedEvents = 0;
        pg->Bind(wxEVT_PG_SELECTED,
                 [&selectedEvents](wxPropertyGridEvent&)
                 {
                     ++selectedEvents;
                 });
        // Exercise the common guard at the virtual SetFocus() boundary. This
        // does not qualify native focus delivery, which depends on the window
        // manager accepting focus and is a separate integration contract.
        editor->Arm(pg, property, focusSeen);

        CHECK_FALSE( pg->SelectProperty(property, true) );

        CHECK( focusSeen );
        CHECK( pg->GetPropertyByName("Focus removal") == nullptr );
        CHECK( pg->GetSelection() == nullptr );
        CHECK( pg->GetEditorControl() == nullptr );
        CHECK( selectedEvents == 0 );

        wxTheApp->ProcessIdle();
        CHECK( pg->GetRoot()->GetChildCount() == 0 );
    }

    SECTION("Editor_focus_callback_may_destroy_secondary_control")
    {
        static DestroyingSecondaryEditor focusEditor;
        focusEditor.Arm(DestroyingSecondaryEditor::Callback::Focus);
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Focus secondary",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        property->SetEditor(&focusEditor);

        CHECK_FALSE( pg->SelectProperty(property, true) );

        CHECK( focusEditor.WasCalled() );
        CHECK( focusEditor.GetSecondary() == nullptr );
        CHECK( pg->GetEditorControlSecondary() == nullptr );
        CHECK( pg->GetEditorControl() != nullptr );
        CHECK( pg->GetSelection() == property );
    }

    SECTION("Custom_image_capability_callback_may_remove_property")
    {
        static DestroyingCustomImageEditor imageEditor;
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Image capability removal",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        property->SetEditor(&imageEditor);
        property->SetValueImage(wxBitmap(16, 16));
        imageEditor.Arm(pg, property);
        int selectedEvents = 0;
        pg->Bind(wxEVT_PG_SELECTED,
                 [&selectedEvents](wxPropertyGridEvent&)
                 {
                     ++selectedEvents;
                 });

        CHECK_FALSE( pg->SelectProperty(property, true) );

        CHECK( imageEditor.WasCalled() );
        CHECK( pg->GetPropertyByName("Image capability removal") == nullptr );
        CHECK( pg->GetSelection() == nullptr );
        CHECK( pg->GetEditorControl() == nullptr );
        CHECK( selectedEvents == 0 );

        wxTheApp->ProcessIdle();
        CHECK( pg->GetRoot()->GetChildCount() == 0 );
    }

    SECTION("Editor_show_callback_may_remove_property")
    {
        static RemovingOnShowEditor showEditor;
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Show removal",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        property->SetEditor(&showEditor);
        showEditor.Arm(pg, property);
        int selectedEvents = 0;
        pg->Bind(wxEVT_PG_SELECTED,
                 [&selectedEvents](wxPropertyGridEvent&)
                 {
                     ++selectedEvents;
                 });

        CHECK_FALSE( pg->SelectProperty(property, true) );

        CHECK( showEditor.WasCalled() );
        CHECK( pg->GetPropertyByName("Show removal") == nullptr );
        CHECK( pg->GetSelection() == nullptr );
        CHECK( pg->GetEditorControl() == nullptr );
        CHECK( selectedEvents == 0 );
        const wxWeakRef<wxWindow> weakPrimary(showEditor.GetCreatedControl());
        wxTheApp->ProcessIdle();
        CHECK( pg->GetRoot()->GetChildCount() == 0 );
        REQUIRE( WaitFor("PropertyGrid abandoned shown editor destruction",
                         [&weakPrimary]() { return !weakPrimary; }) );
    }

    SECTION("Editor_show_callback_may_destroy_secondary_control")
    {
        static DestroyingSecondaryEditor showEditor;
        showEditor.Arm(DestroyingSecondaryEditor::Callback::Show);
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Show secondary",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        property->SetEditor(&showEditor);

        CHECK_FALSE( pg->SelectProperty(property, true) );

        CHECK( showEditor.WasCalled() );
        CHECK( showEditor.GetSecondary() == nullptr );
        CHECK( pg->GetEditorControlSecondary() == nullptr );
        CHECK( pg->GetEditorControl() != nullptr );
        CHECK( pg->GetSelection() == property );
    }

    SECTION("Ensure_visible_expand_callback_may_destroy_grid")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const category =
            pg->Append(new wxPropertyCategory("Collapsed parent"));
        wxPGProperty* const property =
            pg->AppendIn(category,
                         new wxStringProperty("Hidden child",
                                              wxPG_LABEL,
                                              "value"));
        REQUIRE(category);
        REQUIRE(property);
        REQUIRE( pg->Collapse(category) );
        REQUIRE_FALSE( property->IsVisible() );
        const wxWeakRef<wxWindow> weakGrid(pg);
        pg->DestroyOnNextExpand(&grid);

        CHECK_FALSE( pg->EnsureVisible(property) );

        CHECK_FALSE( weakGrid );
        CHECK_FALSE( grid );
    }

    SECTION("Image_measure_may_remove_edited_property")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        ReentrantMeasureProperty* const property =
            new ReentrantMeasureProperty("Measured property");
        REQUIRE( pg->Append(property) == property );
        property->SetValueImage(wxBitmap(16, 16));
        REQUIRE( pg->SelectProperty(property, true) );

        property->Arm(pg,
                      ReentrantMeasureProperty::Action::RemoveProperty);
        pg->RepositionEditorForTest();

        CHECK( pg->GetPropertyByName("Measured property") == nullptr );
        CHECK( pg->GetSelection() == nullptr );
    }

    SECTION("Image_measure_may_destroy_grid")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        ReentrantMeasureProperty* const property =
            new ReentrantMeasureProperty("Destroying measure");
        REQUIRE( pg->Append(property) == property );
        property->SetValueImage(wxBitmap(16, 16));
        REQUIRE( pg->SelectProperty(property, true) );

        const wxWeakRef<wxWindow> weakGrid(pg);
        property->Arm(pg, ReentrantMeasureProperty::Action::DestroyGrid);
        pg->RepositionEditorForTest();

        CHECK_FALSE( weakGrid );
    }

    SECTION("Get_image_size_stops_after_property_removal")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        ReentrantMeasureProperty* const property =
            new ReentrantMeasureProperty("Direct image measure");
        REQUIRE( pg->Append(property) == property );
        property->SetValueImage(wxBitmap(16, 16));

        property->Arm(pg,
                      ReentrantMeasureProperty::Action::RemoveProperty);
        CHECK( pg->GetImageSize(property) == wxSize(0, 0) );
        CHECK( pg->GetPropertyByName("Direct image measure") == nullptr );
    }

    SECTION("Get_image_size_stops_after_grid_destruction")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        ReentrantMeasureProperty* const property =
            new ReentrantMeasureProperty("Direct destroying measure");
        REQUIRE( pg->Append(property) == property );
        property->SetValueImage(wxBitmap(16, 16));

        const wxWeakRef<wxWindow> weakGrid(pg);
        property->Arm(pg, ReentrantMeasureProperty::Action::DestroyGrid);
        CHECK( pg->GetImageSize(property) == wxSize(0, 0) );
        CHECK_FALSE( weakGrid );
    }

    SECTION("Fit_columns_stops_after_image_measure_removal")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240)));
        ReentrantPropertyGrid* const pg = grid.get();
        ReentrantMeasureProperty* const property =
            new ReentrantMeasureProperty("Fit image measure");
        REQUIRE( pg->Append(property) == property );
        property->SetValueImage(wxBitmap(16, 16));

        property->Arm(pg,
                      ReentrantMeasureProperty::Action::RemoveProperty);
        pg->FitColumns();

        CHECK( pg->GetPropertyByName("Fit image measure") == nullptr );
    }

    SECTION("Dialog_adapter_stops_after_validation_destroys_grid")
    {
        SelfDeletingValidationGrid* const pg =
            new SelfDeletingValidationGrid(wxTheApp->GetTopWindow());
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Dialog property",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property) );
        const wxWeakRef<wxWindow> weakGrid(pg);
        DestroyingDialogAdapter adapter(false);

        CHECK_FALSE( adapter.ShowDialog(pg, property) );
        CHECK_FALSE( weakGrid );
        CHECK_FALSE( adapter.WasCalled() );
    }

    SECTION("Dialog_adapter_stops_after_dialog_destroys_grid")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Dialog property",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property) );
        const wxWeakRef<wxWindow> weakGrid(pg);
        DestroyingDialogAdapter adapter(true);

        CHECK_FALSE( adapter.ShowDialog(pg, property) );
        CHECK_FALSE( weakGrid );
        CHECK( adapter.WasCalled() );
    }

    SECTION("Nested_property_change_is_rejected_transactionally")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPGProperty* const property = pgManager->GetProperty("Label");
        wxPGProperty* const other = pgManager->GetProperty("X");
        REQUIRE( property );
        REQUIRE( other );
        REQUIRE( pg->SelectProperty(property, true) );

        wxTextCtrl* const editor = pg->GetEditorTextCtrl();
        REQUIRE( editor );

        const wxVariant originalOtherValue = other->GetValue();
        bool changingNestedResult = true;
        bool changedNestedResult = true;
        int changingEvents = 0;
        int changedEvents = 0;

        pgManager->Bind(
            wxEVT_PG_CHANGING,
            [pg, property, other, &changingNestedResult,
             &changingEvents](wxPropertyGridEvent& event)
            {
                if ( event.GetProperty() == property )
                {
                    ++changingEvents;
                    changingNestedResult =
                        pg->ChangePropertyValue(other, 41L);
                }
            });
        pgManager->Bind(
            wxEVT_PG_CHANGED,
            [pg, property, other, &changedNestedResult,
             &changedEvents](wxPropertyGridEvent& event)
            {
                if ( event.GetProperty() == property )
                {
                    ++changedEvents;
                    changedNestedResult =
                        pg->ChangePropertyValue(other, 42L);
                }
            });

        editor->ChangeValue("outer transaction");
        pg->EditorsValueWasModified();
        REQUIRE( pg->CommitChangesFromEditor() );

        CHECK( property->GetValueAsString() == "outer transaction" );
        CHECK( other->GetValue() == originalOtherValue );
        CHECK_FALSE( changingNestedResult );
        CHECK_FALSE( changedNestedResult );
        CHECK( changingEvents == 1 );
        CHECK( changedEvents == 1 );
    }

    SECTION("Composed_change_parent_removal_aborts_event_chain")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPGProperty* const leaf =
            pgManager->GetProperty("Car.Speeds.Max. Speed (mph)");
        wxPGProperty* const composedParent =
            pgManager->GetProperty("Car.Speeds");
        REQUIRE( leaf );
        REQUIRE( composedParent );

        SECTION("hidden owner page")
        {
            REQUIRE( pg->GetState() != leaf->GetParentState() );
        }
        SECTION("displayed owner page")
        {
            pgManager->SelectPage(1);
            REQUIRE( pg->GetState() == leaf->GetParentState() );
        }
        const int selectedPage = pgManager->GetSelectedPage();

        int changedEvents = 0;
        bool removedParent = false;
        wxVariant changedValue;
        pgManager->Bind(
            wxEVT_PG_CHANGED,
            [&](wxPropertyGridEvent& event)
            {
                ++changedEvents;
                changedValue = event.GetPropertyValue();
                if ( !removedParent )
                {
                    removedParent = true;
                    pgManager->DeleteProperty(composedParent);
                }
            });

        CHECK( pg->ChangePropertyValue(leaf, 301L) );
        CHECK( removedParent );
        CHECK( changedEvents == 1 );
        CHECK( changedValue == wxVariant(301L) );
        CHECK_FALSE( pgManager->GetProperty("Car.Speeds") );
        CHECK( pgManager->GetSelectedPage() == selectedPage );
    }

    SECTION("Columns_resized_handler_may_destroy_manager")
    {
        ClientWidthNotificationPage* const notificationPage =
            new ClientWidthNotificationPage;
        pgManager->AddPage("Width notification",
                           wxBitmapBundle(),
                           notificationPage);
        pgManager->SelectPage(
            static_cast<int>(pgManager->GetPageCount()) - 1);

        wxPropertyGrid* const pg = pgManager->GetGrid();
        const wxWeakRef<wxWindow> weakManager(pgManager.get());
        const wxWeakRef<wxWindow> weakGrid(pg);
        bool eventSeen = false;

        pgManager->Bind(
            wxEVT_PG_COLS_RESIZED,
            [&](wxPropertyGridEvent&)
            {
                eventSeen = true;
                pgManager.reset();
            });

        // Exercise the property-grid state transaction directly. Dispatching
        // a synthetic wxSizeEvent through wxScrolled's pushed helper and then
        // deleting that helper from its own ProcessEvent() frame is outside
        // wxWindow's lifetime contract and would test the scrolled-window
        // dispatcher instead of OnClientWidthChange().
        const int widthChange = pg->FromDIP(17);
        notificationPage->NotifyClientWidthChange(
            pg->GetClientSize().x + widthChange,
            widthChange);

        CHECK( eventSeen );
        CHECK_FALSE( weakManager );
        CHECK_FALSE( weakGrid );
    }

    SECTION("Page_switch_aborts_when_resize_callback_switches_state")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPropertyGridPage* const firstPage = pgManager->GetPage(0);
        wxPropertyGridPage* const secondPage = pgManager->GetPage(1);
        REQUIRE( firstPage );
        REQUIRE( secondPage );
        pgManager->SelectPage(0);

        int resizeEvents = 0;
        pgManager->Bind(
            wxEVT_PG_COLS_RESIZED,
            [&](wxPropertyGridEvent&)
            {
                ++resizeEvents;
                if ( resizeEvents == 1 )
                    pg->SwitchState(firstPage->GetStatePtr());
            });

        pgManager->SelectPage(1);

        CHECK( resizeEvents >= 1 );
        CHECK( pgManager->GetSelectedPage() == 0 );
        CHECK( pg->GetState() == firstPage->GetStatePtr() );
    }

    SECTION("Page_switch_rejects_target_removal_during_transaction")
    {
        REQUIRE( pgManager->GetPageCount() >= 2 );
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPropertyGridPage* const targetPage = pgManager->GetPage(1);
        REQUIRE( targetPage );
        pgManager->SelectPage(0);
        const size_t pageCount = pgManager->GetPageCount();

        int resizeEvents = 0;
        bool removeResult = false;
        pgManager->Bind(
            wxEVT_PG_COLS_RESIZED,
            [&](wxPropertyGridEvent&)
            {
                if ( pg->GetState() == targetPage->GetStatePtr() &&
                     pgManager->GetSelectedPage() == 0 )
                {
                    ++resizeEvents;
                    // Every attempt must be rejected, including any further
                    // size notifications while the page switch is pending.
                    removeResult = pgManager->RemovePage(1) || removeResult;
                }
            });

        pgManager->SelectPage(1);

        // Switching state and updating its scrollbars can each resize the
        // destination columns before the manager commits the selected page.
        CHECK( resizeEvents >= 1 );
        CHECK_FALSE( removeResult );
        CHECK( pgManager->GetPageCount() == pageCount );
        CHECK( pgManager->GetPage(1) == targetPage );
        CHECK( pgManager->GetSelectedPage() == 1 );
        CHECK( pg->GetState() == targetPage->GetStatePtr() );
    }

    SECTION("Remove_current_page_aborts_if_replacement_switch_fails")
    {
        REQUIRE( pgManager->GetPageCount() >= 2 );
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPropertyGridPage* const replacementPage = pgManager->GetPage(0);
        wxPropertyGridPage* const currentPage = pgManager->GetPage(1);
        REQUIRE( replacementPage );
        REQUIRE( currentPage );
        pgManager->SelectPage(1);
        const size_t pageCount = pgManager->GetPageCount();
#if wxUSE_TOOLBAR
        wxToolBar* const toolbar = pgManager->GetToolBar();
        REQUIRE( toolbar );
        std::vector<wxToolBarToolBase*> toolsBefore;
        for ( size_t i = 0; i < toolbar->GetToolsCount(); ++i )
            toolsBefore.push_back(toolbar->GetToolByPos(static_cast<int>(i)));
#endif

        int resizeEvents = 0;
        pgManager->Bind(
            wxEVT_PG_COLS_RESIZED,
            [&](wxPropertyGridEvent&)
            {
                if ( pg->GetState() == replacementPage->GetStatePtr() &&
                     pgManager->GetSelectedPage() == 1 )
                {
                    ++resizeEvents;
                    pg->SwitchState(currentPage->GetStatePtr());
                }
            });

        CHECK_FALSE( pgManager->RemovePage(1) );

        CHECK( resizeEvents == 1 );
        CHECK( pgManager->GetPageCount() == pageCount );
        CHECK( pgManager->GetPage(1) == currentPage );
        CHECK( pgManager->GetSelectedPage() == 1 );
        CHECK( pg->GetState() == currentPage->GetStatePtr() );
#if wxUSE_TOOLBAR
        REQUIRE( toolbar->GetToolsCount() == toolsBefore.size() );
        for ( size_t i = 0; i < toolsBefore.size(); ++i )
        {
            CHECK( toolbar->GetToolByPos(static_cast<int>(i)) ==
                   toolsBefore[i] );
        }
#endif
    }

#if wxUSE_TOOLBAR
    SECTION("Remove_last_page_rolls_back_each_toolbar_failure")
    {
        ToolbarRemovalFailureReset resetToolbarFailures;
        wxUnusedVar(resetToolbarFailures);

        std::unique_ptr<wxPropertyGridManager> manager(
            new wxPropertyGridManager(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220),
                                      wxPG_TOOLBAR));
        manager->SetExtraStyle(wxPG_EX_MODE_BUTTONS);
        wxPropertyGridPage* const onlyPage = manager->AddPage("Only page");
        REQUIRE( onlyPage );
        onlyPage->Append(new wxStringProperty("Retained property",
                                              wxPG_LABEL,
                                              "value"));
        REQUIRE( manager->GetSelectedPage() == 0 );

        wxToolBar* const toolbar = manager->GetToolBar();
        REQUIRE( toolbar );
        REQUIRE( toolbar->GetToolsCount() >= 4 );

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__) && !defined(__WXWINUI__)
        const auto nativeToolCount = [toolbar]()
        {
            return static_cast<size_t>(::SendMessage(
                static_cast<HWND>(toolbar->GetHandle()), TB_BUTTONCOUNT, 0, 0));
        };
        REQUIRE( nativeToolCount() == toolbar->GetToolsCount() );
#endif

        for ( unsigned int failureOrdinal : {1u, 2u} )
        {
            CAPTURE(failureOrdinal);
            const size_t toolCount = toolbar->GetToolsCount();
            // GetToolByPos() takes int, including on 32-bit targets where a
            // larger size_t index would become a negative iterator distance.
            REQUIRE( toolCount <=
                     static_cast<size_t>((std::numeric_limits<int>::max)()) );
            const int toolCountInt = static_cast<int>(toolCount);
            std::vector<wxToolBarToolBase*> toolsBefore;
            for ( int i = 0; i < toolCountInt; ++i )
            {
                toolsBefore.push_back(toolbar->GetToolByPos(i));
            }

            wxPGManagerFailToolbarRemovalForTesting(failureOrdinal);
            CHECK_FALSE( manager->RemovePage(0) );

            CHECK( manager->GetPageCount() == 1 );
            CHECK( manager->GetPage(0) == onlyPage );
            CHECK( manager->GetSelectedPage() == 0 );
            CHECK( manager->GetProperty("Retained property") != nullptr );
            REQUIRE( toolbar->GetToolsCount() == toolCount );
            for ( int i = 0; i < toolCountInt; ++i )
            {
                CHECK( toolbar->GetToolByPos(i) == toolsBefore[i] );
            }
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__) && !defined(__WXWINUI__)
            // Restoring only the wrappers leaves MSW's native toolbar missing
            // the page button until Realize(), breaking the next removal.
            CHECK( nativeToolCount() == toolCount );
#endif
        }

        const size_t toolCount = toolbar->GetToolsCount();
        CHECK( manager->RemovePage(0) );
        // The retained internal page is not counted as a managed page after
        // the final removal, but its identity must still be preserved.
        CHECK( manager->GetPageCount() == 0 );
        CHECK( manager->GetPage(0) == onlyPage );
        CHECK( manager->GetSelectedPage() == wxNOT_FOUND );
        CHECK( manager->GetProperty("Retained property") == nullptr );
        CHECK( toolbar->GetToolsCount() + 2 == toolCount );
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__) && !defined(__WXWINUI__)
        CHECK( nativeToolCount() == toolbar->GetToolsCount() );
#endif
    }
#endif

    SECTION("SetFont_aborts_if_selection_callback_switches_page")
    {
        REQUIRE( pgManager->GetPageCount() >= 2 );
        pgManager->SelectPage(0);

        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPGProperty* const property = pgManager->GetProperty("Label");
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property, true) );

        const wxFont oldManagerFont = pgManager->GetFont();
        const wxFont oldGridFont = pg->GetFont();
        wxFont requestedFont = oldManagerFont;
        requestedFont.SetPointSize(requestedFont.GetPointSize() + 1);
        REQUIRE( requestedFont != oldManagerFont );

        bool switchedPage = false;
        pgManager->Bind(
            wxEVT_PG_SELECTED,
            [&](wxPropertyGridEvent&)
            {
                if ( !switchedPage )
                {
                    switchedPage = true;
                    pgManager->SelectPage(1);
                }
            });

        CHECK_FALSE( pgManager->SetFont(requestedFont) );
        CHECK( switchedPage );
        CHECK( pgManager->GetSelectedPage() == 1 );
        CHECK( pg->GetState() == pgManager->GetPage(1)->GetStatePtr() );
        CHECK( pgManager->GetFont() == oldManagerFont );
        CHECK( pg->GetFont() == oldGridFont );
    }

    SECTION("Page_OnShow_nested_selection_wins_transaction")
    {
        REQUIRE( pgManager->GetPageCount() >= 2 );
        ReentrantShowPage* const recursivePage = new ReentrantShowPage;
        pgManager->AddPage("Recursive page", wxBitmapBundle(), recursivePage);
        const int recursiveIndex =
            static_cast<int>(pgManager->GetPageCount()) - 1;
        recursivePage->Arm(pgManager.get(), 1);

        pgManager->SelectPage(recursiveIndex);

        CHECK( pgManager->GetSelectedPage() == 1 );
        CHECK( pgManager->GetGrid()->GetState() ==
               pgManager->GetPage(1)->GetStatePtr() );
    }

    SECTION("Page_OnShow_same_target_is_coalesced")
    {
        SameTargetShowPage* const recursivePage = new SameTargetShowPage;
        pgManager->AddPage("Same-target page",
                           wxBitmapBundle(),
                           recursivePage);
        const int recursiveIndex =
            static_cast<int>(pgManager->GetPageCount()) - 1;
        recursivePage->Arm(pgManager.get(), recursiveIndex);

        pgManager->SelectPage(recursiveIndex);

        CHECK( recursivePage->GetShowCount() == 1 );
        CHECK( pgManager->GetSelectedPage() == recursiveIndex );
        CHECK( pgManager->GetGrid()->GetState() ==
               recursivePage->GetStatePtr() );
    }

    SECTION("Page_event_rejects_reentrant_page_removal")
    {
        std::unique_ptr<ReentrantPropertyGridManager> manager(
            new ReentrantPropertyGridManager);
        REQUIRE( manager->Create(wxTheApp->GetTopWindow(),
                                 wxID_ANY,
                                 wxDefaultPosition,
                                 wxSize(360, 220)) );
        ClientWidthNotificationPage* const page =
            new ClientWidthNotificationPage;
        REQUIRE( manager->AddPage("Dispatch page",
                                  wxBitmapBundle(),
                                  page) == page );
        wxPGProperty* const property =
            page->Append(new wxStringProperty("Dispatch property",
                                              wxPG_LABEL,
                                              "value"));
        REQUIRE( property );

        int callbackCount = 0;
        bool removalResult = true;
        page->Bind(wxEVT_PG_CHANGED,
                   [&](wxPropertyGridEvent&)
                   {
                       ++callbackCount;
                       removalResult = manager->RemovePage(0);
                   });

        wxPropertyGridEvent event(wxEVT_PG_CHANGED, manager->GetId());
        event.SetEventObject(manager->GetGrid());
        event.SetPropertyGrid(manager->GetGrid());
        event.SetProperty(property);
        manager->ProcessEventForTest(event);

        CHECK( callbackCount == 1 );
        CHECK_FALSE( removalResult );
        CHECK( manager->GetPageCount() == 1 );
        CHECK( manager->GetPage(0) == page );
        CHECK( manager->GetProperty("Dispatch property") == property );
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    SECTION("Retained_page_dispatch_stops_at_each_destroy_boundary")
    {
        enum class DestroyTarget
        {
            Grid,
            Manager,
            TopLevel
        };

        for ( DestroyTarget target : {DestroyTarget::Grid,
                                      DestroyTarget::Manager,
                                      DestroyTarget::TopLevel} )
        {
            CAPTURE(static_cast<int>(target));
            wxFrame* const frame = new wxFrame(nullptr, wxID_ANY, "PG owner");
            ReentrantPropertyGridManager* const manager =
                new ReentrantPropertyGridManager;
            REQUIRE( manager->Create(frame,
                                     wxID_ANY,
                                     wxDefaultPosition,
                                     wxSize(360, 220)) );
            ClientWidthNotificationPage* const page =
                new ClientWidthNotificationPage;
            REQUIRE( manager->AddPage("Retained dispatch",
                                      wxBitmapBundle(),
                                      page) == page );
            wxPGProperty* const property =
                page->Append(new wxStringProperty("Retained dispatch property",
                                                  wxPG_LABEL,
                                                  "value"));
            REQUIRE( property );
            wxPropertyGrid* const grid = manager->GetGrid();
            const wxWeakRef<wxWindow> weakFrame(frame);
            const wxWeakRef<wxWindow> weakManager(manager);
            const wxWeakRef<wxWindow> weakGrid(grid);

            int pageCallbacks = 0;
            int managerCallbacks = 0;
            page->Bind(wxEVT_PG_CHANGED,
                       [=, &pageCallbacks](wxPropertyGridEvent&)
                       {
                           ++pageCallbacks;
                           // Child Destroy() is synchronous, unlike TLW
                           // Destroy(). Explicitly retain these targets so
                           // this exercises scheduled, still-live objects
                           // without deleting the manager-owned grid first.
                           if ( target == DestroyTarget::Grid )
                               wxTheApp->ScheduleForDestruction(grid);
                           else if ( target == DestroyTarget::Manager )
                               wxTheApp->ScheduleForDestruction(manager);
                           else
                               frame->Destroy();
                       });
            manager->Bind(wxEVT_PG_CHANGED,
                          [&](wxPropertyGridEvent&)
                          {
                              ++managerCallbacks;
                          });

            wxPropertyGridEvent event(wxEVT_PG_CHANGED, manager->GetId());
            event.SetEventObject(grid);
            event.SetPropertyGrid(grid);
            event.SetProperty(property);
            manager->ProcessEventForTest(event);

            CHECK( pageCallbacks == 1 );
            CHECK( managerCallbacks == 0 );
            CHECK( weakGrid );
            CHECK( weakManager );
            CHECK( weakFrame );
            if ( target == DestroyTarget::Grid )
                CHECK( wxTheApp->IsScheduledForDestruction(grid) );
            else if ( target == DestroyTarget::Manager )
                CHECK( wxTheApp->IsScheduledForDestruction(manager) );

            // For the isolated grid case, retire its manager before draining:
            // the manager owns the internal grid pointer by contract.
            if ( target == DestroyTarget::Grid && weakManager )
                manager->Destroy();
            if ( target != DestroyTarget::TopLevel && weakFrame )
                frame->Destroy();

            wxTheApp->ProcessIdle();
            YieldForAWhile(20);
            CHECK_FALSE( weakGrid );
            CHECK_FALSE( weakManager );
            CHECK_FALSE( weakFrame );
        }
    }
#endif

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
        const int trySplitterPos = wxTheApp->GetTopWindow()->FromDIP(60);
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

    SECTION("Splitter_capture_lost_rolls_back")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPGProperty* const property = pgManager->GetProperty("Label");
        REQUIRE( property );

        const wxRect propertyRect = pg->GetPropertyRect(property, property);
        REQUIRE( propertyRect.height > 0 );
        const int startingPosition = pg->GetSplitterPosition();
        const wxPoint downPosition(startingPosition,
                                   propertyRect.y + propertyRect.height / 2);

        int beginEvents = 0;
        int draggingEvents = 0;
        int endEvents = 0;
        pgManager->Bind(wxEVT_PG_COL_BEGIN_DRAG,
                        [&beginEvents](wxPropertyGridEvent&)
                        {
                            ++beginEvents;
                        });
        pgManager->Bind(wxEVT_PG_COL_DRAGGING,
                        [&draggingEvents](wxPropertyGridEvent&)
                        {
                            ++draggingEvents;
                        });
        pgManager->Bind(wxEVT_PG_COL_END_DRAG,
                        [&endEvents](wxPropertyGridEvent&)
                        {
                            ++endEvents;
                        });

        SendPropertyGridMouseEvent(pg, wxEVT_LEFT_DOWN, downPosition);
        REQUIRE( pg->HasCapture() );

        SendPropertyGridMouseEvent(
            pg,
            wxEVT_MOTION,
            wxPoint(startingPosition + pg->FromDIP(20), downPosition.y),
            true);
        CHECK( pg->GetSplitterPosition() != startingPosition );

        wxMouseCaptureChangedEvent captureLost(pg->GetId());
        captureLost.SetEventObject(pg);
        pg->ProcessWindowEvent(captureLost);

        CHECK_FALSE( pg->HasCapture() );
        CHECK( pg->GetSplitterPosition() == startingPosition );
        CHECK( beginEvents == 1 );
        CHECK( draggingEvents == 1 );
        CHECK( endEvents == 1 );
    }

    SECTION("Splitter_resize_callback_may_destroy_grid")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Resize destruction",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE(editor);
        const wxWeakRef<wxWindow> weakGrid(pg);
        const wxWeakRef<wxWindow> weakEditor(editor);
        const int targetPosition =
            pg->GetSplitterPosition() + pg->FromDIP(24);
#ifdef __WXQT__
        const QPointer<QWidget> editorNativeWidget(editor->GetHandle());
#endif
        int resizeEvents = 0;
        bool positionWasPublished = false;
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXOSX_COCOA__) || \
    defined(__WXQT__)
        struct EditorLifetime
        {
            bool afterOwnerDeletion = false;
            bool afterNestedIdle = false;
            bool parentDetached = false;
            bool nativeControlAlive = false;
#ifdef __WXQT__
            bool nativeParentDetached = false;
            bool nativeHidden = false;
#endif
        } editorLifetime;
#endif
        editor->Bind(wxEVT_SIZE,
                     [pg, targetPosition, &resizeEvents,
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXOSX_COCOA__) || \
    defined(__WXQT__)
                      editor, &editorLifetime,
#endif
                      &positionWasPublished](wxSizeEvent& event)
                     {
                         event.Skip();
                         if ( resizeEvents++ == 0 )
                         {
                             positionWasPublished =
                                 pg->GetSplitterPosition() == targetPosition;
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXOSX_COCOA__) || \
    defined(__WXQT__)
                             // Keep everything used after delete pg on the
                             // stack, independently of the editor-owned
                             // callable whose lifetime is under test.
                             const wxWeakRef<wxWindow> survivor(editor);
                             const WXWidget nativeControl = editor->GetHandle();
                             EditorLifetime* const lifetime = &editorLifetime;
#ifdef __WXQT__
                             const QPointer<QWidget> nativeWidget(nativeControl);
#endif
#endif
                             delete pg;
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXOSX_COCOA__) || \
    defined(__WXQT__)
                             lifetime->afterOwnerDeletion = !!survivor;
                             lifetime->parentDetached =
                                 survivor && !survivor->GetParent();
                             wxTheApp->ProcessIdle();
                             lifetime->afterNestedIdle = !!survivor;
                             lifetime->nativeControlAlive =
                                 survivor &&
#ifdef __WXQT__
                                 nativeWidget &&
#endif
                                 survivor->GetHandle() == nativeControl &&
                                 survivor->GetSize().x >= 0;
#ifdef __WXQT__
                             lifetime->nativeParentDetached =
                                 nativeWidget && !nativeWidget->parentWidget();
                             lifetime->nativeHidden =
                                 nativeWidget && !nativeWidget->isVisible();
#endif
#endif
                         }
                     });

        pg->SetSplitterPosition(targetPosition);

        CHECK( resizeEvents == 1 );
        CHECK( positionWasPublished );
        CHECK_FALSE( weakGrid );
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXOSX_COCOA__) || \
    defined(__WXQT__)
        // These ports retain the real editor/native control until the whole
        // resize transaction returns, even across an explicitly nested idle.
        // No corresponding ownership guarantee is assumed for other ports.
        CHECK( editorLifetime.afterOwnerDeletion );
        CHECK( editorLifetime.afterNestedIdle );
        CHECK( editorLifetime.parentDetached );
        CHECK( editorLifetime.nativeControlAlive );
#ifdef __WXQT__
        CHECK( editorLifetime.nativeParentDetached );
        CHECK( editorLifetime.nativeHidden );
#endif
#endif
        wxTheApp->ProcessIdle();
        REQUIRE( WaitFor("PropertyGrid splitter editor destruction",
                         [&weakEditor]() { return !weakEditor; }) );
#ifdef __WXQT__
        CHECK( editorNativeWidget.isNull() );
#endif
    }

    SECTION("Splitter_rollback_resize_callback_may_destroy_grid")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Rollback destruction",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE(editor);
        const wxRect propertyRect = pg->GetPropertyRect(property, property);
        const int startingPosition = pg->GetSplitterPosition();
        const wxPoint downPosition(startingPosition,
                                   propertyRect.y + propertyRect.height / 2);
        SendPropertyGridMouseEvent(pg, wxEVT_LEFT_DOWN, downPosition);
        REQUIRE(pg->HasCapture());
        SendPropertyGridMouseEvent(
            pg,
            wxEVT_MOTION,
            wxPoint(startingPosition + pg->FromDIP(24), downPosition.y),
            true);
        REQUIRE(pg->GetSplitterPosition() != startingPosition);

        const wxWeakRef<wxWindow> weakGrid(pg);
        const wxWeakRef<wxWindow> weakEditor(editor);
        int resizeEvents = 0;
        int endEvents = 0;
        bool positionWasRestored = false;
        editor->Bind(wxEVT_SIZE,
                     [pg, startingPosition, &resizeEvents,
                      &positionWasRestored](wxSizeEvent& event)
                     {
                         event.Skip();
                         if ( resizeEvents++ == 0 )
                         {
                             positionWasRestored =
                                 pg->GetSplitterPosition() == startingPosition;
                             delete pg;
                         }
                     });
        pg->Bind(wxEVT_PG_COL_END_DRAG,
                 [&endEvents](wxPropertyGridEvent&)
                 {
                     ++endEvents;
                 });

        pg->FinishSplitterDragForTest(true);

        CHECK( resizeEvents == 1 );
        CHECK( positionWasRestored );
        CHECK_FALSE( weakGrid );
        CHECK( endEvents == 0 );
        wxTheApp->ProcessIdle();
        REQUIRE( WaitFor("PropertyGrid rollback editor destruction",
                         [&weakEditor]() { return !weakEditor; }) );
    }

    SECTION("Splitter_resize_removing_hover_aborts_drag_event")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPGProperty* const hover = pgManager->GetProperty("Label");
        REQUIRE(hover);
        REQUIRE( pg->ClearSelection() );
        const wxRect propertyRect = pg->GetPropertyRect(hover, hover);
        const int startingPosition = pg->GetSplitterPosition();
        const wxPoint downPosition(startingPosition,
                                   propertyRect.y + propertyRect.height / 2);
        SendPropertyGridMouseEvent(pg,
                                   wxEVT_MOTION,
                                   wxPoint(startingPosition - 4,
                                           downPosition.y));

        int resizedEvents = 0;
        int draggingEvents = 0;
        int endEvents = 0;
        bool endHadNullProperty = false;
        int endColumn = wxNOT_FOUND;
        pgManager->Bind(wxEVT_PG_COLS_RESIZED,
                        [pg, hover, &resizedEvents](wxPropertyGridEvent&)
                        {
                            ++resizedEvents;
                            pg->DeleteProperty(hover);
                        });
        pgManager->Bind(wxEVT_PG_COL_DRAGGING,
                        [&draggingEvents](wxPropertyGridEvent&)
                        {
                            ++draggingEvents;
                        });
        pgManager->Bind(wxEVT_PG_COL_END_DRAG,
                        [&endEvents, &endHadNullProperty,
                         &endColumn](wxPropertyGridEvent& event)
                        {
                            ++endEvents;
                            endHadNullProperty = !event.GetProperty();
                            endColumn = static_cast<int>(event.GetColumn());
                        });

        SendPropertyGridMouseEvent(pg, wxEVT_LEFT_DOWN, downPosition);
        REQUIRE(pg->HasCapture());
        SendPropertyGridMouseEvent(
            pg,
            wxEVT_MOTION,
            wxPoint(startingPosition + pg->FromDIP(24), downPosition.y),
            true);

        CHECK(resizedEvents == 1);
        CHECK(draggingEvents == 0);
        CHECK(endEvents == 1);
        CHECK(pgManager->GetProperty("Label") == nullptr);
        CHECK(pg->GetSplitterPosition() == startingPosition);
        CHECK(endHadNullProperty);
        CHECK(endColumn == 0);
        CHECK_FALSE(pg->HasCapture());
    }

    SECTION("Splitter_resize_page_switch_aborts_drag_event")
    {
        REQUIRE(pgManager->GetPageCount() >= 2);
        pgManager->SelectPage(0);
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPropertyGridPage* const sourcePage = pgManager->GetPage(0);
        REQUIRE(sourcePage);
        wxPGProperty* const hover = pgManager->GetProperty("Label");
        REQUIRE(hover);
        const wxRect propertyRect = pg->GetPropertyRect(hover, hover);
        const int startingPosition = pg->GetSplitterPosition();
        const wxPoint downPosition(startingPosition,
                                   propertyRect.y + propertyRect.height / 2);

        int resizedEvents = 0;
        int draggingEvents = 0;
        int endEvents = 0;
        bool destinationResizesAfterEnd = true;
        pgManager->Bind(wxEVT_PG_COLS_RESIZED,
                        [&pgManager, pg, sourcePage, &resizedEvents,
                         &endEvents, &destinationResizesAfterEnd]
                        (wxPropertyGridEvent&)
                        {
                            // Switching pages can synchronously resize the
                            // destination's columns. This is not another
                            // notification from the cancelled source drag.
                            if ( !sourcePage->IsDisplayed() )
                            {
                                destinationResizesAfterEnd =
                                    destinationResizesAfterEnd &&
                                    pgManager->GetPage(1)->IsDisplayed() &&
                                    endEvents == 1 && !pg->HasCapture();
                                return;
                            }

                            ++resizedEvents;
                            pgManager->SelectPage(1);
                        });
        pgManager->Bind(wxEVT_PG_COL_DRAGGING,
                        [&draggingEvents](wxPropertyGridEvent&)
                        {
                            ++draggingEvents;
                        });
        pgManager->Bind(wxEVT_PG_COL_END_DRAG,
                        [&endEvents](wxPropertyGridEvent&)
                        {
                            ++endEvents;
                        });

        SendPropertyGridMouseEvent(pg, wxEVT_LEFT_DOWN, downPosition);
        REQUIRE(pg->HasCapture());
        SendPropertyGridMouseEvent(
            pg,
            wxEVT_MOTION,
            wxPoint(startingPosition + pg->FromDIP(24), downPosition.y),
            true);

        CHECK(resizedEvents == 1);
        CHECK(draggingEvents == 0);
        CHECK(endEvents == 1);
        CHECK(destinationResizesAfterEnd);
        CHECK(pgManager->GetSelectedPage() == 1);
        CHECK(sourcePage->GetSplitterPosition() == startingPosition);
        CHECK_FALSE(pg->HasCapture());
    }

    SECTION("Splitter_resize_event_may_destroy_secondary_editor")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Resize secondary",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const primary = pg->GetEditorControl();
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE(primary);
        REQUIRE(secondary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);
        const wxRect propertyRect = pg->GetPropertyRect(property, property);
        const int startingPosition = pg->GetSplitterPosition();
        const wxPoint downPosition(startingPosition,
                                   propertyRect.y + propertyRect.height / 2);
        int resizedEvents = 0;
        int draggingEvents = 0;
        int endEvents = 0;
        pg->Bind(wxEVT_PG_COLS_RESIZED,
                 [secondary, &resizedEvents](wxPropertyGridEvent&)
                 {
                     ++resizedEvents;
                     DestroyPropertyGridEditorControl(secondary);
                 });
        pg->Bind(wxEVT_PG_COL_DRAGGING,
                 [&draggingEvents](wxPropertyGridEvent&)
                 {
                     ++draggingEvents;
                 });
        pg->Bind(wxEVT_PG_COL_END_DRAG,
                 [&endEvents](wxPropertyGridEvent&)
                 {
                     ++endEvents;
                 });

        SendPropertyGridMouseEvent(pg, wxEVT_LEFT_DOWN, downPosition);
        REQUIRE(pg->HasCapture());
        SendPropertyGridMouseEvent(
            pg,
            wxEVT_MOTION,
            wxPoint(startingPosition + pg->FromDIP(24), downPosition.y),
            true);

        CHECK(resizedEvents == 1);
        CHECK(draggingEvents == 0);
        CHECK(endEvents == 1);
        CHECK_FALSE(weakSecondary);
        CHECK(pg->GetEditorControlSecondary() == nullptr);
        CHECK(pg->GetEditorControl() == primary);
        CHECK(primary->IsShown());
        CHECK(pg->GetSplitterPosition() == startingPosition);
        CHECK_FALSE(pg->HasCapture());
        delete pg;
    }

    SECTION("Splitter_capture_release_may_destroy_secondary_editor")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Capture secondary",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const primary = pg->GetEditorControl();
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE(primary);
        REQUIRE(secondary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);
        const wxRect propertyRect = pg->GetPropertyRect(property, property);
        const int startingPosition = pg->GetSplitterPosition();
        const wxPoint downPosition(startingPosition,
                                   propertyRect.y + propertyRect.height / 2);
        int captureEvents = 0;
        int endEvents = 0;

        SendPropertyGridMouseEvent(pg, wxEVT_LEFT_DOWN, downPosition);
        REQUIRE(pg->HasCapture());
        SendPropertyGridMouseEvent(
            pg,
            wxEVT_MOTION,
            wxPoint(startingPosition + pg->FromDIP(24), downPosition.y),
            true);
        REQUIRE(pg->HasCapture());
#ifndef __WXMSW__
        // This is a deterministic release callback, not a claim that the
        // port emits the MSW-only capture-changed notification.
        pg->AfterNextCaptureReleaseForTest([secondary, &captureEvents]()
        {
            ++captureEvents;
            DestroyPropertyGridEditorControl(secondary);
        });
#else
        pg->Bind(wxEVT_MOUSE_CAPTURE_CHANGED,
                 [secondary, &captureEvents](wxMouseCaptureChangedEvent& event)
                 {
                     event.Skip();
                     if ( captureEvents++ == 0 )
                         DestroyPropertyGridEditorControl(secondary);
                 });
#endif
        pg->Bind(wxEVT_PG_COL_END_DRAG,
                 [&endEvents](wxPropertyGridEvent&)
                 {
                     ++endEvents;
                 });

        pg->FinishSplitterDragForTest(false);

        CHECK(captureEvents == 1);
        CHECK(endEvents == 1);
        CHECK_FALSE(weakSecondary);
        CHECK(pg->GetEditorControlSecondary() == nullptr);
        CHECK(pg->GetEditorControl() == primary);
        CHECK(primary->IsShown());
        CHECK_FALSE(pg->HasCapture());
        delete pg;
    }

    SECTION("Splitter_end_event_may_destroy_secondary_editor")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("End secondary",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const primary = pg->GetEditorControl();
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE(primary);
        REQUIRE(secondary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);
        const wxRect propertyRect = pg->GetPropertyRect(property, property);
        const wxPoint downPosition(
            pg->GetSplitterPosition(),
            propertyRect.y + propertyRect.height / 2);
        int endEvents = 0;

        SendPropertyGridMouseEvent(pg, wxEVT_LEFT_DOWN, downPosition);
        REQUIRE(pg->HasCapture());
        pg->Bind(wxEVT_PG_COL_END_DRAG,
                 [secondary, &endEvents](wxPropertyGridEvent&)
                 {
                     ++endEvents;
                     DestroyPropertyGridEditorControl(secondary);
                 });

        pg->FinishSplitterDragForTest(false);

        CHECK(endEvents == 1);
        CHECK_FALSE(weakSecondary);
        CHECK(pg->GetEditorControlSecondary() == nullptr);
        CHECK(pg->GetEditorControl() == primary);
        CHECK(primary->IsShown());
        CHECK_FALSE(pg->HasCapture());
        delete pg;
    }

    SECTION("Keyboard_splitter_event_may_destroy_secondary_editor")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Keyboard secondary",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE(property);
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const primary = pg->GetEditorControl();
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE(primary);
        REQUIRE(secondary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);
        const int startingPosition = pg->GetSplitterPosition();
        int resizedEvents = 0;
        int draggingEvents = 0;
        int endEvents = 0;
        pg->Bind(wxEVT_PG_COLS_RESIZED,
                 [secondary, &resizedEvents](wxPropertyGridEvent&)
                 {
                     ++resizedEvents;
                     DestroyPropertyGridEditorControl(secondary);
                 });
        pg->Bind(wxEVT_PG_COL_DRAGGING,
                 [&draggingEvents](wxPropertyGridEvent&)
                 {
                     ++draggingEvents;
                 });
        pg->Bind(wxEVT_PG_COL_END_DRAG,
                 [&endEvents](wxPropertyGridEvent&)
                 {
                     ++endEvents;
                 });

        CHECK( pg->MoveSplitterFromKeyboardForTest(0, pg->FromDIP(24)) );

        CHECK(resizedEvents == 1);
        CHECK(draggingEvents == 0);
        CHECK(endEvents == 1);
        CHECK_FALSE(weakSecondary);
        CHECK(pg->GetEditorControlSecondary() == nullptr);
        CHECK(pg->GetEditorControl() == primary);
        CHECK(pg->GetSplitterPosition() == startingPosition);
        delete pg;
    }

    SECTION("Splitter_begin_topology_change_aborts")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPGProperty* const property = pgManager->GetProperty("Label");
        REQUIRE( property );

        const wxRect propertyRect = pg->GetPropertyRect(property, property);
        REQUIRE( propertyRect.height > 0 );
        const wxPoint downPosition(
            pg->GetSplitterPosition(),
            propertyRect.y + propertyRect.height / 2);

        int beginEvents = 0;
        int endEvents = 0;
        pgManager->Bind(
            wxEVT_PG_COL_BEGIN_DRAG,
            [&pgManager, &beginEvents](wxPropertyGridEvent&)
            {
                ++beginEvents;
                pgManager->SetColumnCount(3);
            });
        pgManager->Bind(
            wxEVT_PG_COL_END_DRAG,
            [&endEvents](wxPropertyGridEvent&)
            {
                ++endEvents;
            });

        SendPropertyGridMouseEvent(pg, wxEVT_LEFT_DOWN, downPosition);

        CHECK( beginEvents == 1 );
        CHECK( endEvents == 1 );
        CHECK( pg->GetColumnCount() == 3 );
        CHECK_FALSE( pg->HasCapture() );
    }

    SECTION("Splitter_veto_with_topology_change_has_no_end")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPGProperty* const property = pgManager->GetProperty("Label");
        REQUIRE( property );

        const wxRect propertyRect = pg->GetPropertyRect(property, property);
        REQUIRE( propertyRect.height > 0 );
        const wxPoint downPosition(
            pg->GetSplitterPosition(),
            propertyRect.y + propertyRect.height / 2);

        int beginEvents = 0;
        int endEvents = 0;
        pgManager->Bind(
            wxEVT_PG_COL_BEGIN_DRAG,
            [&pgManager, &beginEvents](wxPropertyGridEvent& event)
            {
                ++beginEvents;
                event.Veto();
                pgManager->SetColumnCount(3);
            });
        pgManager->Bind(
            wxEVT_PG_COL_END_DRAG,
            [&endEvents](wxPropertyGridEvent&)
            {
                ++endEvents;
            });

        SendPropertyGridMouseEvent(pg, wxEVT_LEFT_DOWN, downPosition);

        CHECK( beginEvents == 1 );
        CHECK( endEvents == 0 );
        CHECK( pg->GetColumnCount() == 3 );
        CHECK_FALSE( pg->HasCapture() );
    }

#if wxUSE_HEADERCTRL
    SECTION("Header_resize_success_balances_transaction")
    {
        pgManager->ShowHeader();
        wxPropertyGrid* const pg = pgManager->GetGrid();
        const int oldSplitter = pg->GetSplitterPosition();

        int beginEvents = 0;
        int draggingEvents = 0;
        int endEvents = 0;
        int eventColumn = wxNOT_FOUND;
        pgManager->Bind(
            wxEVT_PG_COL_BEGIN_DRAG,
            [&](wxPropertyGridEvent& event)
            {
                ++beginEvents;
                eventColumn = static_cast<int>(event.GetColumn());
            });
        pgManager->Bind(
            wxEVT_PG_COL_DRAGGING,
            [&](wxPropertyGridEvent&) { ++draggingEvents; });
        pgManager->Bind(
            wxEVT_PG_COL_END_DRAG,
            [&](wxPropertyGridEvent&) { ++endEvents; });

        wxHeaderCtrlEvent begin(wxEVT_HEADER_BEGIN_RESIZE,
                                pgManager->GetId());
        begin.SetColumn(0);
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), begin) );
        CHECK( begin.IsAllowed() );

        wxHeaderCtrlEvent resizing(wxEVT_HEADER_RESIZING,
                                   pgManager->GetId());
        resizing.SetColumn(0);
        resizing.SetWidth(oldSplitter + pg->FromDIP(37));
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), resizing) );
        CHECK( resizing.IsAllowed() );

        wxHeaderCtrlEvent end(wxEVT_HEADER_END_RESIZE, pgManager->GetId());
        end.SetColumn(0);
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), end) );
        CHECK( end.IsAllowed() );

        CHECK( beginEvents == 1 );
        CHECK( draggingEvents == 1 );
        CHECK( endEvents == 1 );
        CHECK( eventColumn == 0 );
        CHECK( pg->GetSplitterPosition() != oldSplitter );
        CHECK( pg->GetState() == pgManager->GetCurrentPage()->GetStatePtr() );
    }

    SECTION("Header_resize_page_switch_balances_transaction")
    {
        pgManager->ShowHeader();
        REQUIRE( pgManager->GetPageCount() >= 2 );

        int beginEvents = 0;
        int endEvents = 0;
        pgManager->Bind(
            wxEVT_PG_COL_BEGIN_DRAG,
            [&](wxPropertyGridEvent&)
            {
                ++beginEvents;
                pgManager->SelectPage(1);
            });
        pgManager->Bind(
            wxEVT_PG_COL_END_DRAG,
            [&](wxPropertyGridEvent&)
            {
                ++endEvents;
            });

        wxHeaderCtrlEvent begin(wxEVT_HEADER_BEGIN_RESIZE,
                                pgManager->GetId());
        begin.SetColumn(0);
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), begin) );
        CHECK_FALSE( begin.IsAllowed() );

        wxHeaderCtrlEvent resizing(wxEVT_HEADER_RESIZING,
                                   pgManager->GetId());
        resizing.SetColumn(0);
        resizing.SetWidth(pgManager->GetGrid()->FromDIP(120));
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), resizing) );
        CHECK_FALSE( resizing.IsAllowed() );

        wxHeaderCtrlEvent end(wxEVT_HEADER_END_RESIZE, pgManager->GetId());
        end.SetColumn(0);
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), end) );
        CHECK( end.IsAllowed() );

        CHECK( beginEvents == 1 );
        CHECK( endEvents == 1 );
        CHECK( pgManager->GetSelectedPage() == 1 );
        CHECK( pgManager->GetGrid()->GetState() ==
               pgManager->GetPage(1)->GetStatePtr() );
    }

    SECTION("Header_resize_veto_with_topology_change_has_no_end")
    {
        pgManager->ShowHeader();

        int beginEvents = 0;
        int endEvents = 0;
        pgManager->Bind(
            wxEVT_PG_COL_BEGIN_DRAG,
            [&](wxPropertyGridEvent& event)
            {
                ++beginEvents;
                event.Veto();
                pgManager->SetColumnCount(3);
            });
        pgManager->Bind(
            wxEVT_PG_COL_END_DRAG,
            [&](wxPropertyGridEvent&)
            {
                ++endEvents;
            });

        wxHeaderCtrlEvent begin(wxEVT_HEADER_BEGIN_RESIZE,
                                pgManager->GetId());
        begin.SetColumn(0);
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), begin) );
        CHECK_FALSE( begin.IsAllowed() );

        wxHeaderCtrlEvent end(wxEVT_HEADER_END_RESIZE, pgManager->GetId());
        end.SetColumn(0);
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), end) );
        CHECK( end.IsAllowed() );

        CHECK( beginEvents == 1 );
        CHECK( endEvents == 0 );
        CHECK( pgManager->GetGrid()->GetColumnCount() == 3 );
    }

    SECTION("Header_nested_begin_is_rejected_without_end")
    {
        pgManager->ShowHeader();

        int beginEvents = 0;
        int endEvents = 0;
        bool nestedDispatched = false;
        bool nestedAllowed = true;
        pgManager->Bind(
            wxEVT_PG_COL_BEGIN_DRAG,
            [&](wxPropertyGridEvent& event)
            {
                ++beginEvents;
                wxHeaderCtrlEvent nested(wxEVT_HEADER_BEGIN_RESIZE,
                                         pgManager->GetId());
                nested.SetColumn(0);
                nestedDispatched =
                    ProcessPropertyGridHeaderEvent(pgManager.get(), nested);
                nestedAllowed = nested.IsAllowed();
                event.Veto();
            });
        pgManager->Bind(
            wxEVT_PG_COL_END_DRAG,
            [&](wxPropertyGridEvent&)
            {
                ++endEvents;
            });

        wxHeaderCtrlEvent begin(wxEVT_HEADER_BEGIN_RESIZE,
                                pgManager->GetId());
        begin.SetColumn(0);
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), begin) );
        CHECK_FALSE( begin.IsAllowed() );

        wxHeaderCtrlEvent end(wxEVT_HEADER_END_RESIZE, pgManager->GetId());
        end.SetColumn(0);
        REQUIRE( ProcessPropertyGridHeaderEvent(pgManager.get(), end) );
        CHECK( end.IsAllowed() );

        CHECK( beginEvents == 1 );
        CHECK( endEvents == 0 );
        CHECK( nestedDispatched );
        CHECK_FALSE( nestedAllowed );
    }
#endif

    SECTION("Splitter_keyboard_transaction")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        pg->SetColumnCount(3);
        pg->SetSplitterPosition(pg->FromDIP(100), 0);
        pg->SetSplitterPosition(pg->FromDIP(180), 1);

        int beginEvents = 0;
        int draggingEvents = 0;
        int endEvents = 0;
        int eventColumn = wxNOT_FOUND;
        pgManager->Bind(
            wxEVT_PG_COL_BEGIN_DRAG,
            [&](wxPropertyGridEvent& event)
            {
                ++beginEvents;
                eventColumn = static_cast<int>(event.GetColumn());
            });
        pgManager->Bind(wxEVT_PG_COL_DRAGGING,
                        [&](wxPropertyGridEvent&) { ++draggingEvents; });
        pgManager->Bind(wxEVT_PG_COL_END_DRAG,
                        [&](wxPropertyGridEvent&) { ++endEvents; });

        // Select the second boundary, then move it by the documented fine and
        // accelerated keyboard increments.
        SendPropertyGridKeyEvent(pg, WXK_DOWN, true);
        const int initial = pg->GetSplitterPosition(1);
        SendPropertyGridKeyEvent(pg, WXK_RIGHT, true);
        CHECK( pg->GetSplitterPosition(1) == initial + pg->FromDIP(1) );
        SendPropertyGridKeyEvent(pg, WXK_LEFT, true, true);
        CHECK( pg->GetSplitterPosition(1) == initial - pg->FromDIP(7) );

        CHECK( beginEvents == 2 );
        CHECK( draggingEvents == 2 );
        CHECK( endEvents == 2 );
        CHECK( eventColumn == 1 );
        CHECK_FALSE( pg->HasCapture() );
    }

    SECTION("Splitter_nested_keyboard_begin_is_coalesced")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        const int initial = pg->GetSplitterPosition();
        int beginEvents = 0;
        int endEvents = 0;
        bool nestedSent = false;
        pgManager->Bind(
            wxEVT_PG_COL_BEGIN_DRAG,
            [&](wxPropertyGridEvent&)
            {
                ++beginEvents;
                if ( !nestedSent )
                {
                    nestedSent = true;
                    SendPropertyGridKeyEvent(pg, WXK_RIGHT, true);
                }
            });
        pgManager->Bind(
            wxEVT_PG_COL_END_DRAG,
            [&](wxPropertyGridEvent&)
            {
                ++endEvents;
            });

        SendPropertyGridKeyEvent(pg, WXK_RIGHT, true);

        CHECK( beginEvents == 1 );
        CHECK( endEvents == 1 );
        CHECK( pg->GetSplitterPosition() == initial );
        CHECK_FALSE( pg->HasCapture() );
    }

    SECTION("DPI_scales_visible_and_hidden_page_geometry")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        REQUIRE( pgManager->GetPageCount() >= 2 );

        pgManager->SetPageSplitterPosition(0, pg->FromDIP(120));
        pgManager->SetPageSplitterPosition(1, pg->FromDIP(160));
        wxPropertyGridPage* const visiblePage = pgManager->GetPage(0);
        wxPropertyGridPage* const hiddenPage = pgManager->GetPage(1);
        const int visibleWidth =
            visiblePage->GetStatePtr()->GetColumnWidth(0);
        const int hiddenWidth =
            hiddenPage->GetStatePtr()->GetColumnWidth(0);

        wxDPIChangedEvent dpiUp(wxSize(96, 96), wxSize(144, 144));
        dpiUp.SetEventObject(pg);
        pg->ProcessWindowEvent(dpiUp);

#ifndef wxHAS_DPI_INDEPENDENT_PIXELS
        CHECK( visiblePage->GetStatePtr()->GetColumnWidth(0) ==
               dpiUp.ScaleX(visibleWidth) );
        CHECK( hiddenPage->GetStatePtr()->GetColumnWidth(0) ==
               dpiUp.ScaleX(hiddenWidth) );
#else
        CHECK( visiblePage->GetStatePtr()->GetColumnWidth(0) ==
               visibleWidth );
        CHECK( hiddenPage->GetStatePtr()->GetColumnWidth(0) ==
               hiddenWidth );
#endif

        wxDPIChangedEvent dpiDown(wxSize(144, 144), wxSize(96, 96));
        dpiDown.SetEventObject(pg);
        pg->ProcessWindowEvent(dpiDown);
        CHECK( visiblePage->GetStatePtr()->GetColumnWidth(0) ==
               visibleWidth );
        CHECK( hiddenPage->GetStatePtr()->GetColumnWidth(0) ==
               hiddenWidth );
    }

    SECTION("Custom_renderer_may_destroy_grid_during_draw")
    {
        bool rendererCalled = false;
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        DestroyingRendererProperty* const property =
            new DestroyingRendererProperty(&rendererCalled);
        REQUIRE( pg->Append(property) == property );

        wxBitmap bitmap(360, 220);
        wxMemoryDC dc(bitmap);
        const wxRect rect(0, 0, 360, 220);
        pg->DrawItemsForTest(dc, rect);

        const wxWeakRef<wxWindow> weakGrid(pg);
        property->Arm();
        pg->DrawItemsForTest(dc, rect);
        dc.SelectObject(wxNullBitmap);

        CHECK( rendererCalled );
        CHECK_FALSE( weakGrid );
        wxTheApp->ProcessIdle();
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    SECTION("Retained_renderer_stops_after_toplevel_destroy_request")
    {
        wxFrame* const frame = new wxFrame(nullptr, wxID_ANY, "PG renderer");
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(frame,
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        bool rendererCalled = false;
        DestroyingRendererProperty* const property =
            new DestroyingRendererProperty(&rendererCalled);
        REQUIRE( pg->Append(property) == property );

        wxBitmap bitmap(360, 220);
        wxMemoryDC dc(bitmap);
        const wxRect rect(0, 0, 360, 220);
        pg->DrawItemsForTest(dc, rect);
        const wxWeakRef<wxWindow> weakFrame(frame);
        const wxWeakRef<wxWindow> weakGrid(pg);

        property->Arm(frame);
        pg->DrawItemsForTest(dc, rect);
        dc.SelectObject(wxNullBitmap);

        CHECK( rendererCalled );
        wxTheApp->ProcessIdle();
        YieldForAWhile(20);
        CHECK_FALSE( weakGrid );
        CHECK_FALSE( weakFrame );
    }

    SECTION("Retained_property_callback_stops_after_grid_destroy_request")
    {
        wxFrame* const frame = new wxFrame(nullptr, wxID_ANY, "PG property");
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(frame,
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        bool propertyCalled = false;
        DestroyingValueStringProperty* const property =
            new DestroyingValueStringProperty(&propertyCalled);
        REQUIRE( pg->Append(property) == property );

        wxBitmap bitmap(360, 220);
        wxMemoryDC dc(bitmap);
        const wxRect rect(0, 0, 360, 220);
        pg->DrawItemsForTest(dc, rect);
        const wxWeakRef<wxWindow> weakFrame(frame);
        const wxWeakRef<wxWindow> weakGrid(pg);

        property->Arm(pg, pg);
        pg->DrawItemsForTest(dc, rect);
        dc.SelectObject(wxNullBitmap);

        CHECK( propertyCalled );
        if ( weakFrame )
            frame->Destroy();
        wxTheApp->ProcessIdle();
        YieldForAWhile(20);
        CHECK_FALSE( weakGrid );
        CHECK_FALSE( weakFrame );
    }

    SECTION("Retained_validator_stops_after_manager_destroy_request")
    {
        wxFrame* const frame = new wxFrame(nullptr, wxID_ANY, "PG validator");
        ReentrantPropertyGridManager* const manager =
            new ReentrantPropertyGridManager;
        REQUIRE( manager->Create(frame,
                                 wxID_ANY,
                                 wxDefaultPosition,
                                 wxSize(360, 220)) );
        wxPropertyGridPage* const page = manager->AddPage("Validator page");
        REQUIRE( page );
        DestroyingValidationProperty* const property =
            new DestroyingValidationProperty("Retained validator");
        REQUIRE( page->Append(property) == property );
        ReentrantPropertyGrid* const pg = manager->GetReentrantGrid();
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE( editor );
        const wxWeakRef<wxWindow> weakFrame(frame);
        const wxWeakRef<wxWindow> weakManager(manager);
        const wxWeakRef<wxWindow> weakGrid(pg);
        int changedEvents = 0;
        manager->Bind(wxEVT_PG_CHANGED,
                      [&](wxPropertyGridEvent&)
                      {
                          ++changedEvents;
                      });

        property->ArmValidator(editor, true, manager);
        CHECK_FALSE( pg->ValidateEditorForTest() );

        CHECK( property->ValidatorWasCalled() );
        CHECK( changedEvents == 0 );
        if ( weakFrame )
            frame->Destroy();
        wxTheApp->ProcessIdle();
        YieldForAWhile(20);
        CHECK_FALSE( weakGrid );
        CHECK_FALSE( weakManager );
        CHECK_FALSE( weakFrame );
    }

    SECTION("Retained_editor_stops_after_toplevel_destroy_request")
    {
        static DestroyingGetValueEditor editor;
        wxFrame* const frame = new wxFrame(nullptr, wxID_ANY, "PG editor");
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(frame,
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Retained editor",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        property->SetEditor(&editor);
        REQUIRE( pg->SelectProperty(property, true) );
        const wxWeakRef<wxWindow> weakFrame(frame);
        const wxWeakRef<wxWindow> weakGrid(pg);
        int changedEvents = 0;
        pg->Bind(wxEVT_PG_CHANGED,
                 [&](wxPropertyGridEvent&)
                 {
                     ++changedEvents;
                 });

        editor.Arm(frame);
        pg->EditorsValueWasModified();
        CHECK_FALSE( pg->CommitChangesFromEditor() );

        CHECK( editor.WasCalled() );
        CHECK( changedEvents == 0 );
        wxTheApp->ProcessIdle();
        YieldForAWhile(20);
        CHECK_FALSE( weakGrid );
        CHECK_FALSE( weakFrame );
    }
#endif

    SECTION("Custom_renderer_column_topology_change_aborts_draw")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        pg->SetColumnCount(3);
        ColumnMutatingRendererProperty* const property =
            new ColumnMutatingRendererProperty("Mutating renderer");
        REQUIRE( pg->Append(property) == property );
        REQUIRE( pg->Append(new wxStringProperty("Following property",
                                                 wxPG_LABEL,
                                                 "value")) );

        wxBitmap bitmap(360, 220);
        wxMemoryDC dc(bitmap);
        const wxRect rect(0, 0, 360, 220);
        pg->DrawItemsForTest(dc, rect);

        property->Arm();
        pg->DrawItemsForTest(dc, rect);
        dc.SelectObject(wxNullBitmap);

        CHECK( pg->GetColumnCount() == 2 );
        CHECK( property->GetCallsAfterArm() == 1 );
        delete pg;
        wxTheApp->ProcessIdle();
    }

    SECTION("Multiple_selection_add_stops_after_renderer_removes_property")
    {
        bool rendererCalled = false;
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220)));
        ReentrantPropertyGrid* const pg = grid.get();
        pg->SetExtraStyle(pg->GetExtraStyle() |
                          wxPG_EX_MULTIPLE_SELECTION);
        wxPGProperty* const first =
            pg->Append(new wxStringProperty("First selection"));
        RemovingRendererProperty* const removing =
            new RemovingRendererProperty(&rendererCalled);
        REQUIRE( first );
        REQUIRE( pg->Append(removing) == removing );
        REQUIRE( pg->SelectProperty(first, false) );

        removing->Arm();
        CHECK_FALSE( pg->AddToSelection(removing) );

        CHECK( rendererCalled );
        CHECK( pg->GetPropertyByName("Removing renderer") == nullptr );
        CHECK( pg->GetSelection() == first );
        wxTheApp->ProcessIdle();
    }

    SECTION("Multiple_selection_remove_stops_after_renderer_removes_property")
    {
        bool rendererCalled = false;
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220)));
        ReentrantPropertyGrid* const pg = grid.get();
        pg->SetExtraStyle(pg->GetExtraStyle() |
                          wxPG_EX_MULTIPLE_SELECTION);
        wxPGProperty* const first =
            pg->Append(new wxStringProperty("First selection"));
        RemovingRendererProperty* const removing =
            new RemovingRendererProperty(&rendererCalled);
        REQUIRE( first );
        REQUIRE( pg->Append(removing) == removing );
        REQUIRE( pg->SelectProperty(first, false) );
        REQUIRE( pg->AddToSelection(removing) );

        removing->Arm();
        CHECK_FALSE( pg->RemoveFromSelection(removing) );

        CHECK( rendererCalled );
        CHECK( pg->GetPropertyByName("Removing renderer") == nullptr );
        CHECK( pg->GetSelection() == first );
        wxTheApp->ProcessIdle();
    }

    SECTION("Select_and_edit_stops_when_add_removes_target")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220)));
        ReentrantPropertyGrid* const pg = grid.get();
        wxPGProperty* const first =
            pg->Append(new wxStringProperty("Old selection"));
        wxPGProperty* const target =
            pg->Append(new wxStringProperty("Label target"));
        REQUIRE( first );
        REQUIRE( target );
        pg->MakeColumnEditable(0);
        REQUIRE( pg->SelectProperty(first, false) );

        int labelEditBeginEvents = 0;
        pg->Bind(wxEVT_PG_LABEL_EDIT_BEGIN,
                 [&labelEditBeginEvents](wxPropertyGridEvent&)
                 {
                     ++labelEditBeginEvents;
                 });

        pg->Bind(wxEVT_PG_SELECTED,
                 [pg, target](wxPropertyGridEvent& event)
                 {
                     if ( event.GetProperty() == target )
                         pg->DeleteProperty(target);
                 });

        CHECK_FALSE( pg->SelectAndEditForTest(target, 0) );
        CHECK_FALSE( pg->GetLabelEditor() );
        CHECK( pg->GetPropertyByName("Label target") == nullptr );
        CHECK( labelEditBeginEvents == 0 );
    }

    SECTION("Shift_selection_stops_after_range_property_is_removed")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220)));
        ReentrantPropertyGrid* const pg = grid.get();
        pg->SetExtraStyle(pg->GetExtraStyle() |
                          wxPG_EX_MULTIPLE_SELECTION);
        wxPGProperty* const first =
            pg->Append(new wxStringProperty("Range 1"));
        wxPGProperty* const second =
            pg->Append(new wxStringProperty("Range 2"));
        wxPGProperty* const removed =
            pg->Append(new wxStringProperty("Range 3"));
        wxPGProperty* const target =
            pg->Append(new wxStringProperty("Range 4"));
        REQUIRE( first );
        REQUIRE( second );
        REQUIRE( removed );
        REQUIRE( target );
        REQUIRE( pg->SelectProperty(first, false) );

        int selectedEvents = 0;
        pg->Bind(wxEVT_PG_SELECTED,
                 [pg, second, removed, &selectedEvents](wxPropertyGridEvent& event)
                 {
                     ++selectedEvents;
                     if ( event.GetProperty() == second )
                         pg->DeleteProperty(removed);
                 });
        wxMouseEvent shiftClick(wxEVT_LEFT_DOWN);
        shiftClick.SetShiftDown(true);

        CHECK_FALSE( pg->SelectFromInputForTest(target, 1, &shiftClick) );
        CHECK( pg->IsPropertySelected(second) );
        CHECK_FALSE( pg->IsPropertySelected(target) );
        CHECK( pg->GetPropertyByName("Range 3") == nullptr );
        CHECK( pg->GetSelectedProperties().size() == 2 );
        CHECK( selectedEvents == 1 );
    }

    SECTION("Set_editor_appearance_value_callback_may_destroy_grid")
    {
        bool callbackCalled = false;
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        DestroyingValueStringProperty* const property =
            new DestroyingValueStringProperty(&callbackCalled);
        REQUIRE( pg->Append(property) == property );
        pg->Show();
        REQUIRE( pg->SelectProperty(property, true) );
        REQUIRE( pg->GetEditorControl() );

        wxPGCell explicitAppearance;
        // Returning from explicit text to an empty appearance must query the
        // property's value, which is the callback boundary under test.
        explicitAppearance.SetText("placeholder");
        pg->SetEditorAppearanceForTest(explicitAppearance);

        const wxWeakRef<wxWindow> weakGrid(pg);
        property->Arm(pg);
        wxPGCell defaultAppearance;
        defaultAppearance.SetEmptyData();
        pg->SetEditorAppearanceForTest(defaultAppearance);

        CHECK( callbackCalled );
        CHECK_FALSE( weakGrid );
        wxTheApp->ProcessIdle();
    }

#if wxUSE_TOOLTIPS
    SECTION("Tooltip_value_callback_may_destroy_grid")
    {
        bool callbackCalled = false;
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220),
                                      wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS);
        DestroyingValueStringProperty* const property =
            new DestroyingValueStringProperty(&callbackCalled);
        property->SetValue(wxString(200, 'x'));
        REQUIRE( pg->Append(property) == property );
        pg->Show();
        pg->Refresh();
        pg->Update();
        REQUIRE( pg->MoveOverPropertyForTest(property, 0) );

        const wxWeakRef<wxWindow> weakGrid(pg);
        property->Arm(pg);
        CHECK_FALSE( pg->MoveOverPropertyForTest(property, 1) );

        CHECK( callbackCalled );
        CHECK_FALSE( weakGrid );
        wxTheApp->ProcessIdle();
    }

    SECTION("Tooltip_image_measure_may_remove_property")
    {
        std::unique_ptr<ReentrantPropertyGrid> grid(
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220),
                                      wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS));
        ReentrantPropertyGrid* const pg = grid.get();
        ReentrantMeasureProperty* const property =
            new ReentrantMeasureProperty("Tooltip measure");
        REQUIRE( pg->Append(property) == property );
        property->SetValueImage(wxBitmap(16, 16));
        pg->Show();
        pg->Refresh();
        pg->Update();
        REQUIRE( pg->MoveOverPropertyForTest(property, 0) );

        property->Arm(pg, ReentrantMeasureProperty::Action::RemoveProperty);
        CHECK_FALSE( pg->MoveOverPropertyForTest(property, 1) );

        CHECK( pg->GetPropertyByName("Tooltip measure") == nullptr );
        wxTheApp->ProcessIdle();
    }

    SECTION("Tooltip_image_measure_page_switch_aborts_transaction")
    {
        std::unique_ptr<ReentrantPropertyGridManager> manager(
            new ReentrantPropertyGridManager);
        REQUIRE( manager->Create(wxTheApp->GetTopWindow(),
                                 wxID_ANY,
                                 wxDefaultPosition,
                                 wxSize(360, 220),
                                 wxPGMAN_DEFAULT_STYLE | wxPG_TOOLTIPS) );
        REQUIRE( manager->AddPage("Tooltip source") );
        ReentrantMeasureProperty* const property =
            new ReentrantMeasureProperty("Switching tooltip measure");
        REQUIRE( manager->Append(property) == property );
        property->SetValueImage(wxBitmap(16, 16));
        REQUIRE( manager->AddPage("Tooltip destination") );
        REQUIRE( manager->Append(new wxStringProperty("Destination")) );
        manager->SelectPage(0);

        ReentrantPropertyGrid* const pg = manager->GetReentrantGrid();
        REQUIRE( pg );
        manager->Show();
        manager->Refresh();
        manager->Update();
        REQUIRE( pg->MoveOverPropertyForTest(property, 0) );
        pg->SetToolTip("sentinel");

        property->Arm(manager.get(), 1);
        CHECK_FALSE( pg->MoveOverPropertyForTest(property, 1) );

        CHECK( property->WasCalled() );
        CHECK( manager->GetSelectedPage() == 1 );
        CHECK( pg->GetToolTipText() == "sentinel" );
        wxTheApp->ProcessIdle();
    }
#endif // wxUSE_TOOLTIPS

#ifdef __WXMSW__
    SECTION("Buffered_paint_renderer_may_resize_grid")
    {
        bool rendererCalled = false;
        bool bufferReplaced = false;
        bool dcWasValid = false;
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        ResizingRendererProperty* const property =
            new ResizingRendererProperty(&rendererCalled,
                                         &bufferReplaced,
                                         &dcWasValid);
        REQUIRE( pg->Append(property) == property );
        pg->UseSoftwareDoubleBufferForTest();
        const wxBitmap bufferBefore = pg->GetSoftwareDoubleBufferForTest();
        REQUIRE( bufferBefore.IsOk() );
        pg->Show();
        pg->Refresh();
        pg->Update();

        property->Arm(bufferBefore);
        pg->Refresh();
        YieldForAWhile(100);

        CHECK( rendererCalled );
        CHECK( dcWasValid );
        CHECK( bufferReplaced );
        CHECK( pg->GetSize() == wxSize(720, 500) );
        delete pg;
        wxTheApp->ProcessIdle();
    }

    SECTION("Invalid_software_buffer_falls_back_to_paint_dc")
    {
        bool rendererCalled = false;
        bool dcWasValid = false;
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        ResizingRendererProperty* const property =
            new ResizingRendererProperty(&rendererCalled,
                                         nullptr,
                                         &dcWasValid);
        REQUIRE( pg->Append(property) == property );
        pg->UseSoftwareDoubleBufferForTest();
        pg->Show();
        pg->Refresh();
        pg->Update();

        pg->InvalidateSoftwareDoubleBufferForTest();
        REQUIRE_FALSE( pg->GetSoftwareDoubleBufferForTest().IsOk() );
        property->Arm(wxBitmap());
        pg->Refresh();
        pg->Update();

        CHECK( rendererCalled );
        CHECK( dcWasValid );
        delete pg;
        wxTheApp->ProcessIdle();
    }
#endif

    SECTION("Default_renderer_callback_may_destroy_grid")
    {
        bool valueCallbackCalled = false;
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        DestroyingValueStringProperty* const property =
            new DestroyingValueStringProperty(&valueCallbackCalled);
        REQUIRE( pg->Append(property) == property );

        wxBitmap bitmap(360, 220);
        wxMemoryDC dc(bitmap);
        const wxRect rect(0, 0, 360, 220);
        pg->DrawItemsForTest(dc, rect);

        const wxWeakRef<wxWindow> weakGrid(pg);
        property->Arm(pg);
        pg->DrawItemsForTest(dc, rect);
        dc.SelectObject(wxNullBitmap);

        CHECK( valueCallbackCalled );
        CHECK_FALSE( weakGrid );
        wxTheApp->ProcessIdle();
    }

    SECTION("Best_size_callback_may_destroy_grid")
    {
        bool valueCallbackCalled = false;
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        DestroyingValueStringProperty* const property =
            new DestroyingValueStringProperty(&valueCallbackCalled);
        REQUIRE( pg->Append(property) == property );
        pg->GetBestSizeForTest();

        const wxWeakRef<wxWindow> weakGrid(pg);
        property->Arm(pg);
        const wxSize bestSize = pg->GetBestSizeForTest();

        CHECK( valueCallbackCalled );
        CHECK_FALSE( weakGrid );
        CHECK( bestSize == wxDefaultSize );
        wxTheApp->ProcessIdle();
    }

    SECTION("Changing_event_value_survives_grid_destruction")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Changing value",
                                            wxPG_LABEL,
                                            "before"));
        REQUIRE( property );

        const wxVariant pending("after");
        wxVariant beforeDeletion;
        wxVariant afterDeletion;
        bool handlerCalled = false;
        pg->Bind(wxEVT_PG_CHANGING,
                 [&](wxPropertyGridEvent& event)
                 {
                     handlerCalled = true;
                     beforeDeletion = event.GetPropertyValue();
                     // Deleting the grid also destroys this bound functor.
                     // Retain the destination on the stack before deletion,
                     // so the test doesn't read the freed lambda capture.
                     wxVariant* const resultAfterDeletion = &afterDeletion;
                     delete pg;
                     *resultAfterDeletion = event.GetPropertyValue();
                 });

        const wxWeakRef<wxWindow> weakGrid(pg);
        wxVariant value(pending);
        const bool vetoed =
            pg->SendChangingEventForTest(property, &value);

        CHECK( handlerCalled );
        CHECK_FALSE( vetoed );
        CHECK_FALSE( weakGrid );
        CHECK( beforeDeletion == pending );
        CHECK( afterDeletion == pending );
        wxTheApp->ProcessIdle();
    }

    SECTION("Property_grid_event_registration_is_idempotent")
    {
        ReentrantPropertyGrid* const first =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(240, 160));
        ReentrantPropertyGrid* const second =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(240, 160));
        wxPropertyGridEvent event(wxEVT_PG_SELECTED, wxID_ANY);

        event.SetPropertyGrid(first);
        event.SetPropertyGrid(first);
        event.SetPropertyGrid(second);
        event.SetPropertyGrid(second);
        delete first;
        event.SetPropertyGrid(nullptr);
        event.SetPropertyGrid(nullptr);
        delete second;
        SUCCEED();
    }

    SECTION("Changing_event_clone_owns_pending_value_snapshot")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(240, 160));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Changing clone",
                                            wxPG_LABEL,
                                            "before"));
        REQUIRE( property );

        std::unique_ptr<wxPropertyGridEvent> clone;
        pg->Bind(wxEVT_PG_CHANGING,
                 [&](wxPropertyGridEvent& event)
                 {
                     if ( !clone )
                     {
                         clone.reset(static_cast<wxPropertyGridEvent*>(
                             event.Clone()));
                     }
                 });

        wxVariant firstValue("first pending value");
        CHECK_FALSE( pg->SendChangingEventForTest(property, &firstValue) );
        REQUIRE( clone );
        CHECK( clone->GetPropertyValue() == firstValue );

        wxVariant secondValue("second pending value");
        CHECK_FALSE( pg->SendChangingEventForTest(property, &secondValue) );
        CHECK( clone->GetPropertyValue() == firstValue );

        delete pg;
        CHECK( clone->GetProperty() == nullptr );
        CHECK( clone->GetPropertyValue() == firstValue );
        clone.reset();
        wxTheApp->ProcessIdle();
    }

#ifdef __WXMSW__
    SECTION("CreateControls_destroying_grid_defers_unassigned_editor")
    {
        const unsigned int batchesBefore =
            wxPGMSWGetDeferredEditorBatchCountForTesting();
        const unsigned int hostsBefore =
            wxPGMSWGetEditorParkingHostCountForTesting();
        DestroyingCreateControlsEditor editor;
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("CreateControls deletion",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        pg->SetPropertyEditor(property, &editor);
        const wxWeakRef<wxWindow> weakGrid(pg);

        CHECK_FALSE( pg->SelectProperty(property, true) );
        CHECK_FALSE( weakGrid );
        CHECK( editor.SurvivedNestedYield() );
        CHECK( editor.IsEditorAlive() );
        CHECK( editor.GetCreatedHwnd() != nullptr );
        CHECK( ::IsWindow(editor.GetCreatedHwnd()) );
        const HWND createdParent =
            ::GetParent(editor.GetCreatedHwnd());
        const bool parkedDirectly =
            IsMessageOnlyWindow(editor.GetCreatedHwnd());
        const bool parkedUnderHost =
            createdParent &&
            wxPGMSWIsEditorParkingHostForTesting(
                reinterpret_cast<WXWidget>(createdParent));
        CHECK( (parkedDirectly || parkedUnderHost) );
        CHECK( wxPGMSWGetDeferredEditorBatchCountForTesting() ==
               batchesBefore + 1 );

        wxTheApp->ProcessIdle();
        CHECK_FALSE( editor.IsEditorAlive() );
        CHECK_FALSE( ::IsWindow(editor.GetCreatedHwnd()) );
        CHECK( wxPGMSWGetDeferredEditorBatchCountForTesting() ==
               batchesBefore );
        CHECK( wxPGMSWGetEditorParkingHostCountForTesting() ==
               hostsBefore );
    }

    SECTION("Ordinary_editor_teardown_restores_grid_focus")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Focused editor",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE( editor );
        editor->SetFocus();
        REQUIRE( WaitFor("PropertyGrid editor focus",
                         [editor]()
                         {
                             wxWindow* const focus = wxWindow::FindFocus();
                             return focus == editor ||
                                    (focus && editor->IsDescendant(focus));
                         }) );

        REQUIRE( pg->ClearSelection() );
        REQUIRE( WaitFor("PropertyGrid focus restoration",
                         [pg]()
                         {
                             return wxWindow::FindFocus() == pg;
                         }) );

        wxTheApp->ProcessIdle();
        delete pg;
    }

    SECTION("Duplicate_editor_identity_is_owned_once")
    {
        SameWindowEditor editor;
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Duplicate editor",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        pg->SetPropertyEditor(property, &editor);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const control = pg->GetEditorControl();
        REQUIRE( control );
        CHECK( pg->GetEditorControlSecondary() == nullptr );
        const wxWeakRef<wxWindow> weakControl(control);

        REQUIRE( pg->ClearSelection() );
        wxTheApp->ProcessIdle();
        CHECK_FALSE( weakControl );
        delete pg;
    }

    SECTION("Batch_survives_cross_member_destruction")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Two controls",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        pg->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const primary = pg->GetEditorControl();
        wxWindow* const secondary = pg->GetEditorControlSecondary();
        REQUIRE( primary );
        REQUIRE( secondary );
        const wxWeakRef<wxWindow> weakPrimary(primary);
        const wxWeakRef<wxWindow> weakSecondary(secondary);
        primary->Bind(
            wxEVT_DESTROY,
            [secondary](wxWindowDestroyEvent& event)
            {
                event.Skip();
                if ( !secondary->IsBeingDeleted() )
                    delete secondary;
            });

        REQUIRE( pg->ClearSelection() );
        wxTheApp->ProcessIdle();
        CHECK_FALSE( weakPrimary );
        CHECK_FALSE( weakSecondary );
        delete pg;
    }

    SECTION("Existing_pending_membership_preserves_app_handler_ownership")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Pending editor",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const control = pg->GetEditorControl();
        REQUIRE( control );
        const wxWeakRef<wxWindow> weakControl(control);

        wxEvtHandler* const firstAppHandler = new wxEvtHandler;
        wxEvtHandler* const secondAppHandler = new wxEvtHandler;
        control->PushEventHandler(firstAppHandler);
        control->PushEventHandler(secondAppHandler);
        bool appHandlersDetached = false;
        control->Bind(
            wxEVT_DESTROY,
            [control, firstAppHandler, secondAppHandler,
             &appHandlersDetached](wxWindowDestroyEvent& event)
            {
                if ( event.GetEventObject() == control )
                {
                    // These handlers belong to the application, not the
                    // grid. Unlink them here, but keep them alive until this
                    // destruction event has finished traversing the chain.
                    CHECK( control->GetEventHandler() == secondAppHandler );
                    CHECK( control->RemoveEventHandler(secondAppHandler) );
                    CHECK( control->RemoveEventHandler(firstAppHandler) );
                    CHECK( control->GetEventHandler() == control );
                    appHandlersDetached = true;
                }
                event.Skip();
            });
        wxPendingDelete.Append(control);
        wxPendingDelete.Append(control);

        REQUIRE( pg->ClearSelection() );
        CHECK_FALSE( wxPendingDelete.Member(control) );
        CHECK_FALSE( appHandlersDetached );
        CHECK( control->GetEventHandler() == secondAppHandler );
        CHECK( secondAppHandler->GetNextHandler() == firstAppHandler );
        CHECK( firstAppHandler->GetNextHandler() == control );

        wxTheApp->ProcessIdle();
        CHECK_FALSE( weakControl );
        CHECK( appHandlersDetached );
        CHECK( firstAppHandler->IsUnlinked() );
        CHECK( secondAppHandler->IsUnlinked() );
        delete firstAppHandler;
        delete secondAppHandler;
        delete pg;
    }

    SECTION("Double_parking_failure_with_live_grid_destroys_editor")
    {
        EditorParkingFailureReset resetParkingFailures;
        wxUnusedVar(resetParkingFailures);
        const unsigned int batchesBefore =
            wxPGMSWGetDeferredEditorBatchCountForTesting();
        const unsigned int hostsBefore =
            wxPGMSWGetEditorParkingHostCountForTesting();

        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Failed parking",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE( editor );
        const wxWeakRef<wxWindow> weakEditor(editor);
        const HWND editorHwnd =
            reinterpret_cast<HWND>(editor->GetHandle());

        wxPGMSWFailNextDirectEditorParkingForTesting();
        wxPGMSWFailNextFallbackEditorParkingForTesting();
        REQUIRE( pg->ClearSelection() );
        CHECK( weakEditor );
        CHECK( ::IsWindow(editorHwnd) );
        CHECK( wxPGMSWGetDeferredEditorBatchCountForTesting() ==
               batchesBefore + 1 );

        wxTheApp->ProcessIdle();
        CHECK_FALSE( weakEditor );
        CHECK_FALSE( ::IsWindow(editorHwnd) );
        CHECK( wxPGMSWGetDeferredEditorBatchCountForTesting() ==
               batchesBefore );
        CHECK( wxPGMSWGetEditorParkingHostCountForTesting() ==
               hostsBefore );
        delete pg;
    }

    SECTION("Boolean_choice_teardown_unlinks_owned_handler_once")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxBoolProperty("Boolean choice",
                                          wxPG_LABEL,
                                          true));
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE( editor );
        REQUIRE( wxDynamicCast(editor, wxOwnerDrawnComboBox) );
        const wxWeakRef<wxWindow> weakEditor(editor);

        REQUIRE( pg->ClearSelection() );
        wxTheApp->ProcessIdle();
        CHECK_FALSE( weakEditor );
        delete pg;
    }

    SECTION("Label_close_epoch_protects_active_editor_stack")
    {
        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(360, 220));
        wxPGProperty* const property =
            pg->Append(new wxStringProperty("Editable label",
                                            wxPG_LABEL,
                                            "value"));
        REQUIRE( property );
        REQUIRE( pg->SelectProperty(property) );
        pg->BeginLabelEditWithEvent(0);
        wxTextCtrl* const label = pg->GetLabelEditor();
        REQUIRE( label );
        label->SetFocus();
        REQUIRE( WaitFor("PropertyGrid label editor focus",
                         [label]()
                         {
                             return wxWindow::FindFocus() == label;
                         }) );
        const wxWeakRef<wxWindow> weakGrid(pg);
        const wxWeakRef<wxWindow> weakLabel(label);
        bool destroyed = false;
        bool survivedNestedYield = false;
        label->Bind(
            wxEVT_KILL_FOCUS,
            [pg, &destroyed, &survivedNestedYield, weakLabel](
                wxFocusEvent& event)
            {
                event.Skip();
                if ( !destroyed )
                {
                    destroyed = true;
                    delete pg;
                    wxYield();
                    survivedNestedYield = weakLabel.get() != nullptr;
                }
            });

        pg->EndLabelEditWithEvent(false);
        CHECK( destroyed );
        CHECK_FALSE( weakGrid );
        CHECK( survivedNestedYield );
        CHECK( weakLabel );
        wxTheApp->ProcessIdle();
        CHECK_FALSE( weakLabel );
    }
#endif

#ifndef wxHAS_DPI_INDEPENDENT_PIXELS
    SECTION("DPI_editor_resize_may_destroy_grid")
    {
#ifdef __WXMSW__
        enum class ParkingPath
        {
            Direct,
            HostFallback,
            DissociateFallback
        };

        EditorParkingFailureReset resetParkingFailures;
        wxUnusedVar(resetParkingFailures);

        const unsigned int parkingHostsBefore =
            wxPGMSWGetEditorParkingHostCountForTesting();
        const unsigned int batchesBefore =
            wxPGMSWGetDeferredEditorBatchCountForTesting();
        REQUIRE( FindPropertyGridEditorParkingHost() == nullptr );

        ParkingPath parkingPath = ParkingPath::Direct;
        SECTION("direct message-only parking")
        {
        }
        SECTION("dedicated DPI-compatible host")
        {
            parkingPath = ParkingPath::HostFallback;
            wxPGMSWFailNextDirectEditorParkingForTesting();
        }
        SECTION("selective dissociation after both parking paths fail")
        {
            parkingPath = ParkingPath::DissociateFallback;
            wxPGMSWFailNextDirectEditorParkingForTesting();
            wxPGMSWFailNextFallbackEditorParkingForTesting();
        }

        ReentrantPropertyGrid* const pg =
            new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                      wxID_ANY,
                                      wxDefaultPosition,
                                      wxSize(400, 240));
        wxArrayString labels;
        labels.Add("zero");
        labels.Add("one");
        labels.Add("two");
        wxPGChoices choices(labels);
        wxPGProperty* const property =
            pg->Append(new wxEnumProperty("DPI editor",
                                          wxPG_LABEL,
                                          choices,
                                          1));
        REQUIRE( property );
        pg->SetPropertyEditor(property, wxPGEditor_ComboBox);
        REQUIRE( pg->SelectProperty(property, true) );
        wxWindow* const editor = pg->GetEditorControl();
        REQUIRE( editor );
        wxOwnerDrawnComboBox* const combo =
            wxDynamicCast(editor, wxOwnerDrawnComboBox);
        REQUIRE( combo );
        wxTextCtrl* const textChild = combo->GetTextCtrl();
        REQUIRE( textChild );

        combo->ShowPopup();
        wxYield();
        wxWindow* const popup = combo->GetPopupWindow();
        REQUIRE( popup );

        const wxWeakRef<wxWindow> weakGrid(pg);
        const wxWeakRef<wxWindow> weakEditor(editor);
        const wxWeakRef<wxWindow> weakTextChild(textChild);
        const wxWeakRef<wxWindow> weakPopup(popup);
        const HWND gridHwnd = reinterpret_cast<HWND>(pg->GetHandle());
        const HWND editorHwnd =
            reinterpret_cast<HWND>(editor->GetHandle());
        const HWND textChildHwnd =
            reinterpret_cast<HWND>(textChild->GetHandle());
        const HWND textChildParent = ::GetParent(textChildHwnd);
        const HWND popupHwnd =
            reinterpret_cast<HWND>(popup->GetHandle());
        const HWND popupOwner = ::GetWindow(popupHwnd, GW_OWNER);
        REQUIRE( gridHwnd );
        REQUIRE( editorHwnd );
        REQUIRE( textChildHwnd );
        REQUIRE( popupHwnd );
        REQUIRE( ::IsWindow(gridHwnd) );
        REQUIRE( ::IsWindow(editorHwnd) );
        REQUIRE( ::IsWindow(textChildHwnd) );
        REQUIRE( ::IsWindow(popupHwnd) );
        REQUIRE( ::IsChild(editorHwnd, textChildHwnd) );
        REQUIRE( textChildParent != nullptr );
        REQUIRE( (::GetWindowLongPtr(popupHwnd, GWL_STYLE) & WS_POPUP) != 0 );
        REQUIRE_FALSE( ::IsChild(editorHwnd, popupHwnd) );

        bool destroyed = false;
        bool survivedNestedYield = false;
        bool destroyHandlerRan = false;
        bool hostWasAliveDuringEditorDestroy = false;
        HWND parkingHostHwnd = nullptr;

        editor->Bind(
            wxEVT_DESTROY,
            [&](wxWindowDestroyEvent& event)
            {
                event.Skip();
                destroyHandlerRan = true;
                wxTheApp->ProcessIdle();
                hostWasAliveDuringEditorDestroy =
                    !parkingHostHwnd ||
                    (::IsWindow(parkingHostHwnd) &&
                     wxPGMSWIsEditorParkingHostForTesting(
                         reinterpret_cast<WXWidget>(parkingHostHwnd)));
            });

        editor->Bind(
            wxEVT_SIZE,
            [&](wxSizeEvent& event)
            {
                event.Skip();
                if ( !destroyed )
                {
                    destroyed = true;
                    delete pg;

                    CHECK_FALSE( weakGrid );
                    CHECK( weakEditor );
                    CHECK( weakTextChild );
                    CHECK( weakPopup );
                    CHECK_FALSE( ::IsWindowVisible(popupHwnd) );
                    CHECK( ::GetWindow(popupHwnd, GW_OWNER) == popupOwner );
                    CHECK( wxPGMSWGetDeferredEditorBatchCountForTesting() ==
                           batchesBefore + 1 );

                    if ( parkingPath == ParkingPath::Direct )
                    {
                        CHECK( IsMessageOnlyWindow(editorHwnd) );
                        CHECK( ::GetParent(textChildHwnd) ==
                               textChildParent );
                        CHECK( wxPGMSWGetEditorParkingHostCountForTesting() ==
                               parkingHostsBefore );
                    }
                    else if ( parkingPath == ParkingPath::HostFallback )
                    {
                        parkingHostHwnd = ::GetParent(editorHwnd);
                        REQUIRE( parkingHostHwnd );
                        CHECK( wxPGMSWIsEditorParkingHostForTesting(
                                   reinterpret_cast<WXWidget>(
                                       parkingHostHwnd)) );
                        CHECK( ::GetParent(textChildHwnd) ==
                               textChildParent );
                        CHECK( HaveEqualWindowDpiContexts(editorHwnd,
                                                          parkingHostHwnd) );
                        CHECK( wxPGMSWGetEditorParkingHostCountForTesting() ==
                               parkingHostsBefore + 1 );
                    }
                    else
                    {
                        parkingHostHwnd =
                            FindPropertyGridEditorParkingHost();
                        REQUIRE( parkingHostHwnd );
                        CHECK( weakEditor->GetHandle() == nullptr );
                        CHECK( weakTextChild->GetHandle() == nullptr );
                        CHECK_FALSE( ::IsWindow(editorHwnd) );
                        CHECK_FALSE( ::IsWindow(textChildHwnd) );
                        CHECK( ::IsWindow(popupHwnd) );
                        CHECK( ::GetParent(popupHwnd) != parkingHostHwnd );
                        CHECK( wxPGMSWGetEditorParkingHostCountForTesting() ==
                               parkingHostsBefore + 1 );
                    }

                    // A nested idle/event loop inside the destructive callback
                    // must not see the batch: its epoch has not exited yet.
                    wxYield();
                    survivedNestedYield =
                        weakEditor &&
                        weakTextChild &&
                        weakPopup &&
                        wxPGMSWGetDeferredEditorBatchCountForTesting() ==
                            batchesBefore + 1 &&
                        (parkingPath == ParkingPath::Direct ||
                         ::IsWindow(parkingHostHwnd));
                }
            });

        wxSizeEvent resizeEvent(combo->GetSize(), combo->GetId());
        resizeEvent.SetEventObject(combo);
        combo->ProcessWindowEvent(resizeEvent);

        CHECK( destroyed );
        CHECK_FALSE( weakGrid );
        CHECK( survivedNestedYield );
        REQUIRE( weakEditor );
        REQUIRE( weakPopup );
        CHECK_FALSE( ::IsWindow(gridHwnd) );
        if ( parkingPath == ParkingPath::DissociateFallback )
        {
            CHECK( weakEditor->GetHandle() == nullptr );
            CHECK_FALSE( ::IsWindow(editorHwnd) );
        }
        else
        {
            CHECK( weakEditor->GetHandle() ==
                   reinterpret_cast<WXWidget>(editorHwnd) );
            CHECK( ::IsWindow(editorHwnd) );
        }

        // The outer forwarding epoch has now unwound and published exactly one
        // envelope. Deleting the editor may itself enter ProcessIdle(), but the
        // envelope was removed from wxPendingDelete before Drain() began.
        wxTheApp->ProcessIdle();
        CHECK_FALSE( weakEditor );
        CHECK_FALSE( weakTextChild );
        CHECK( destroyHandlerRan );
        CHECK( hostWasAliveDuringEditorDestroy );
        CHECK_FALSE( ::IsWindow(editorHwnd) );
        CHECK_FALSE( ::IsWindow(textChildHwnd) );
        if ( parkingHostHwnd )
            CHECK_FALSE( ::IsWindow(parkingHostHwnd) );
        CHECK( wxPGMSWGetEditorParkingHostCountForTesting() ==
               parkingHostsBefore );
        CHECK( wxPGMSWGetDeferredEditorBatchCountForTesting() ==
               batchesBefore );

        // wxComboCtrl destroys its external transient asynchronously. It was
        // hidden and never reparented by parking, and now follows its own
        // ordinary pending-delete path.
        if ( weakPopup )
            wxTheApp->ProcessIdle();
        REQUIRE( WaitFor("PropertyGrid editor popup destruction",
                         [&]() { return !weakPopup; }) );
        CHECK_FALSE( ::IsWindow(popupHwnd) );
        CHECK( FindPropertyGridEditorParkingHost() == nullptr );
        wxTheApp->GetTopWindow()->SetFocus();
        wxYield();

        // Both failure seams are one-shot. A second editor, without rearming,
        // must take the direct HWND_MESSAGE path and leave no host behind.
        if ( parkingPath != ParkingPath::Direct )
        {
            ReentrantPropertyGrid* const secondGrid =
                new ReentrantPropertyGrid(wxTheApp->GetTopWindow(),
                                          wxID_ANY,
                                          wxDefaultPosition,
                                          wxSize(300, 180));
            wxPGProperty* const secondProperty =
                secondGrid->Append(
                    new wxStringProperty("second", wxPG_LABEL, "value"));
            REQUIRE( secondGrid->SelectProperty(secondProperty, true) );
            wxWindow* const secondEditor = secondGrid->GetEditorControl();
            REQUIRE( secondEditor );
            const wxWeakRef<wxWindow> weakSecondEditor(secondEditor);
            const HWND secondEditorHwnd =
                reinterpret_cast<HWND>(secondEditor->GetHandle());
            bool secondDestroyed = false;
            secondEditor->Bind(
                wxEVT_SIZE,
                [secondGrid, &secondDestroyed](wxSizeEvent& event)
                {
                    event.Skip();
                    if ( !secondDestroyed )
                    {
                        secondDestroyed = true;
                        delete secondGrid;
                    }
                });

            wxSizeEvent secondResize(secondEditor->GetSize(),
                                     secondEditor->GetId());
            secondResize.SetEventObject(secondEditor);
            secondEditor->ProcessWindowEvent(secondResize);
            CHECK( secondDestroyed );
            REQUIRE( weakSecondEditor );
            CHECK( IsMessageOnlyWindow(secondEditorHwnd) );
            CHECK( wxPGMSWGetEditorParkingHostCountForTesting() ==
                   parkingHostsBefore );
            wxTheApp->ProcessIdle();
            CHECK_FALSE( weakSecondEditor );
            CHECK_FALSE( ::IsWindow(secondEditorHwnd) );
            CHECK( wxPGMSWGetDeferredEditorBatchCountForTesting() ==
                   batchesBefore );
            wxTheApp->GetTopWindow()->SetFocus();
            wxYield();
        }
#else
        WARN("MSW-only deferred native editor parking test");
#endif
    }
#endif // !wxHAS_DPI_INDEPENDENT_PIXELS

    SECTION("RTL_hit_test_uses_backend_coordinate_mapping")
    {
        wxPropertyGrid* const pg = pgManager->GetGrid();
        wxPGProperty* const property = pgManager->GetProperty("Label");
        REQUIRE( property );
        const wxRect propertyRect = pg->GetPropertyRect(property, property);
        REQUIRE( propertyRect.height > 0 );

        const int splitter = pg->GetSplitterPosition();
        const int logicalX = splitter - pg->FromDIP(8);
        const int virtualWidth = pg->GetVirtualSize().x;
        const int y = propertyRect.y + propertyRect.height / 2;

        pg->SetLayoutDirection(wxLayout_RightToLeft);
        REQUIRE( WaitFor("PropertyGrid RTL projection",
                         [&]()
                         {
                             return pg->GetLayoutDirection() ==
                                    wxLayout_RightToLeft;
                         },
                         1000) );

        const int backendX =
            pg->AdjustForLayoutDirection(logicalX, 0, virtualWidth);
        CHECK( pg->HitTest(wxPoint(backendX, y)).GetColumn() == 0 );

        pg->SetLayoutDirection(wxLayout_LeftToRight);
        REQUIRE( WaitFor("PropertyGrid LTR restoration",
                         [&]()
                         {
                             return pg->GetLayoutDirection() ==
                                    wxLayout_LeftToRight;
                         },
                         1000) );
        CHECK( pg->HitTest(wxPoint(logicalX, y)).GetColumn() == 0 );
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
                wxPGFlags oldFlags = wxPGFlags::Null;
                if ( p->HasFlag(wxPGFlags::Collapsed) )
                {
                    oldFlags |= wxPGFlags::Collapsed;
                }
                if ( p->HasFlag(wxPGFlags::Disabled) )
                {
                    oldFlags |= wxPGFlags::Disabled;
                }
                if ( p->HasFlag(wxPGFlags::Hidden) )
                {
                    oldFlags |= wxPGFlags::Hidden;
                }
                if ( p->HasFlag(wxPGFlags::NoEditor) )
                {
                    oldFlags |= wxPGFlags::NoEditor;
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
                    !p->HasFlag(wxPGFlags::Collapsed) )
                {
                    FAIL_CHECK(wxString::Format("Error setting flag from string 'COLLAPSED' for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("COLLAPSED") == wxNOT_FOUND &&
                    p->HasFlag(wxPGFlags::Collapsed) )
                {
                    FAIL_CHECK(wxString::Format("Error resetting flag from string 'COLLAPSED'for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("DISABLED") != wxNOT_FOUND &&
                    !p->HasFlag(wxPGFlags::Disabled) )
                {
                    FAIL_CHECK(wxString::Format("Error setting flag from string 'DISABLED' for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("DISABLED") == wxNOT_FOUND &&
                    p->HasFlag(wxPGFlags::Disabled) )
                {
                    FAIL_CHECK(wxString::Format("Error resetting flag from string 'DISABLED' for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("HIDDEN") != wxNOT_FOUND &&
                    !p->HasFlag(wxPGFlags::Hidden) )
                {
                    FAIL_CHECK(wxString::Format("Error setting flag from string 'HIDDEN' for property '%s'",
                        p->GetName()).c_str());
                }
                if ( flags.Find("HIDDEN") == wxNOT_FOUND &&
                    p->HasFlag(wxPGFlags::Hidden) )
                {
                    FAIL_CHECK(wxString::Format("Error resetting flag from string 'HIDDEN' for property '%s'",
                        p->GetName()).c_str());
                }

                // Get individual flags
                bool ok;

                flags = p->GetFlagsAsString(wxPGFlags::Collapsed);
                if ( p->HasFlag(wxPGFlags::Collapsed) )
                {
                    ok = (flags == "COLLAPSED");
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPGFlags::Collapsed flag for property '%s'",
                        p->GetName()).c_str());
                }

                flags = p->GetFlagsAsString(wxPGFlags::Disabled);
                if ( p->HasFlag(wxPGFlags::Disabled) )
                {
                    ok = (flags == "DISABLED");
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPGFlags::Disabled flag for property '%s'",
                        p->GetName()).c_str());
                }

                flags = p->GetFlagsAsString(wxPGFlags::Hidden);
                if ( p->HasFlag(wxPGFlags::Hidden) )
                {
                    ok = (flags == "HIDDEN");
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPGFlags::Hidden flag for property '%s'",
                        p->GetName()).c_str());
                }

                flags = p->GetFlagsAsString(wxPGFlags::NoEditor);
                if ( p->HasFlag(wxPGFlags::NoEditor) )
                {
                    ok = (flags == "NOEDITOR");
                }
                else
                {
                    ok = flags.empty();
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPGFlags::NoEditor flag for property '%s'",
                        p->GetName()).c_str());
                }

                // Get all flags
                flags = p->GetFlagsAsString(wxPGFlags::StringStoredFlags);
                if ( p->HasFlag(wxPGFlags::Collapsed) )
                {
                    ok = (flags.Find("COLLAPSED") != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find("COLLAPSED") == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPGFlags::Collapsed flag for property '%s'",
                        p->GetName()).c_str());
                }

                if ( p->HasFlag(wxPGFlags::Disabled) )
                {
                    ok = (flags.Find("DISABLED") != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find("DISABLED") == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPGFlags::Disabled flag for property '%s'",
                        p->GetName()).c_str());
                }

                if ( p->HasFlag(wxPGFlags::Hidden) )
                {
                    ok = (flags.Find("HIDDEN") != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find("HIDDEN") == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPGFlags::Hidden flag for property '%s'",
                        p->GetName()).c_str());
                }

                if ( p->HasFlag(wxPGFlags::NoEditor) )
                {
                    ok = (flags.Find("NOEDITOR") != wxNOT_FOUND);
                }
                else
                {
                    ok = (flags.Find("NOEDITOR") == wxNOT_FOUND);
                }
                if ( !ok )
                {
                    FAIL_CHECK(wxString::Format("Invalid string for wxPGFlags::NoEditor flag for property '%s'",
                        p->GetName()).c_str());
                }

                // Restore original flags
                p->ChangeFlag(wxPGFlags::Collapsed, !!(oldFlags & wxPGFlags::Collapsed));
                p->ChangeFlag(wxPGFlags::Disabled, !!(oldFlags & wxPGFlags::Disabled));
                p->ChangeFlag(wxPGFlags::Hidden, !!(oldFlags & wxPGFlags::Hidden));
                p->ChangeFlag(wxPGFlags::NoEditor, !!(oldFlags & wxPGFlags::NoEditor));
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

TEST_CASE("PropertyGrid::EditorForwarderOwnerLifetime",
          "[propgrid][propgrid-editor-lifetime]")
{
    class TestGrid final : public wxPropertyGrid
    {
    public:
        using wxPropertyGrid::wxPropertyGrid;
        using wxPropertyGrid::SetupChildEventHandling;
    };

    class ForwardedChild final : public wxWindow
    {
    public:
        using wxWindow::wxWindow;

        ~ForwardedChild() override
        {
            // Exercise the real child-destruction event while the parent's
            // wxPropertyGrid subobject has already been destroyed. The child
            // owns the handlers it pushes, just as an editor control does.
            SendDestroyEvent();
            while ( GetEventHandler() != this )
                delete PopEventHandler(false);
        }
    };

    class ButtonProperty final : public wxStringProperty
    {
    public:
        explicit ButtonProperty(int& buttonEvents)
            : wxStringProperty("Editable", wxPG_LABEL, "before"),
              m_buttonEvents(buttonEvents)
        {
        }

        bool OnEvent(wxPropertyGrid* grid,
                     wxWindow* WXUNUSED(control),
                     wxEvent& event) override
        {
            if ( grid->IsMainButtonEvent(event) )
                ++m_buttonEvents;
            return false;
        }

    private:
        int& m_buttonEvents;
    };

    int buttonEvents = 0;
    int childEvents = 0;
    int childDestroyEvents = 0;
    bool ownerDestroyEvent = false;
    bool childDestroyedAfterGridSubobject = false;
    std::unique_ptr<TestGrid> grid(
        new TestGrid(wxTheApp->GetTopWindow(), wxID_ANY,
                     wxDefaultPosition, wxSize(360, 220)));
    wxPGProperty* const property = grid->Append(new ButtonProperty(buttonEvents));
    grid->SetPropertyEditor(property, wxPGEditor_TextCtrlAndButton);
    REQUIRE( grid->SelectProperty(property) );
    wxTextCtrl* const text = grid->GetEditorTextCtrl();
    wxWindow* const button = grid->GetEditorControlSecondary();
    REQUIRE( text );
    REQUIRE( button );

    bool textDestroyed = false;
    bool buttonDestroyed = false;
    text->Bind(wxEVT_DESTROY,
               [text, &textDestroyed](wxWindowDestroyEvent& event)
               {
                   if ( event.GetEventObject() == text )
                   {
                       textDestroyed = true;
                       CHECK( text->GetEventHandler() == text );
                   }
                   event.Skip();
               });
    button->Bind(wxEVT_DESTROY,
                 [button, &buttonDestroyed](wxWindowDestroyEvent& event)
                 {
                     if ( event.GetEventObject() == button )
                     {
                         buttonDestroyed = true;
                         CHECK( button->GetEventHandler() == button );
                     }
                     event.Skip();
                 });

    // Keep both live forwarding contracts: Enter commits the text value and
    // the secondary button reaches the property exactly once.
    text->ChangeValue("after");
    grid->EditorsValueWasModified();
    wxCommandEvent enter(wxEVT_TEXT_ENTER, text->GetId());
    enter.SetEventObject(text);
    CHECK( text->GetEventHandler()->ProcessEvent(enter) );
    CHECK( property->GetValue().GetString() == "after" );
    wxCommandEvent click(wxEVT_BUTTON, button->GetId());
    click.SetEventObject(button);
    CHECK( button->GetEventHandler()->ProcessEvent(click) );
    CHECK( buttonEvents == 1 );

    // A custom child need not be one of the two currently tracked editor
    // roots. It remains a child until the base window destroys its children.
    ForwardedChild* const child =
        new ForwardedChild(grid.get(), wxID_ANY);
    grid->SetupChildEventHandling(child);
    const wxWeakRef<wxWindow> weakChild(child);
    child->Bind(wxEVT_BUTTON,
                [&childEvents](wxCommandEvent&)
                {
                    ++childEvents;
                });
    child->Bind(wxEVT_DESTROY,
                [&](wxWindowDestroyEvent& event)
                {
                    if ( event.GetEventObject() == child )
                    {
                        ++childDestroyEvents;
                        childDestroyedAfterGridSubobject =
                            !wxDynamicCast(child->GetParent(), wxPropertyGrid);
                    }
                    event.Skip();
                });
    const auto sendChildEvent = [child]()
    {
        wxCommandEvent event(wxEVT_BUTTON, child->GetId());
        event.SetEventObject(child);
        return child->GetEventHandler()->ProcessEvent(event);
    };
    CHECK( sendChildEvent() );
    CHECK( childEvents == 1 );
    TestGrid* const owner = grid.get();
    grid->Bind(wxEVT_DESTROY,
               [&](wxWindowDestroyEvent& event)
               {
                   if ( event.GetEventObject() == owner )
                   {
                       ownerDestroyEvent = true;
                       CHECK( sendChildEvent() );
                       CHECK( childEvents == 2 );
                   }
                   event.Skip();
               });

    grid.reset();
    CHECK( ownerDestroyEvent );
    CHECK( childEvents == 2 );
    CHECK( childDestroyEvents == 1 );
    CHECK( childDestroyedAfterGridSubobject );
    CHECK_FALSE( weakChild );
    CHECK( buttonEvents == 1 );
    CHECK( textDestroyed );
    CHECK( buttonDestroyed );

    // Cover the real combo editor whose base-window destructor requires all
    // pushed handlers to be detached, without asking the fixture to pop them.
    std::unique_ptr<TestGrid> choiceGrid(
        new TestGrid(wxTheApp->GetTopWindow(), wxID_ANY,
                     wxDefaultPosition, wxSize(360, 220)));
    wxArrayString choices;
    choices.Add("first");
    choices.Add("second");
    wxPGProperty* const choiceProperty =
        choiceGrid->Append(new wxEnumProperty("Choice", wxPG_LABEL, choices));
    REQUIRE( choiceGrid->SelectProperty(choiceProperty) );
    wxWindow* const choiceEditor = choiceGrid->GetEditorControl();
    REQUIRE( choiceEditor );
    REQUIRE( choiceEditor->GetEventHandler() != choiceEditor );
    bool choiceDestroyed = false;
    choiceEditor->Bind(wxEVT_DESTROY,
                      [choiceEditor, &choiceDestroyed](wxWindowDestroyEvent& event)
                      {
                          if ( event.GetEventObject() == choiceEditor )
                          {
                              choiceDestroyed = true;
                              CHECK( choiceEditor->GetEventHandler() == choiceEditor );
                          }
                          event.Skip();
                      });
    choiceGrid.reset();
    CHECK( choiceDestroyed );

    // The generic hint implementation (used by wxQt) owns a pushed handler
    // which its text-entry destructor must pop itself. Grid cleanup must
    // remove its forwarder without unlinking the control-owned hint handler.
    std::unique_ptr<TestGrid> hintGrid(
        new TestGrid(wxTheApp->GetTopWindow(), wxID_ANY,
                     wxDefaultPosition, wxSize(360, 220)));
    wxPGProperty* const hintProperty =
        hintGrid->Append(new wxStringProperty("Hinted", wxPG_LABEL, ""));
    hintProperty->SetAttribute(wxPG_ATTR_HINT, "Enter a value");
    REQUIRE( hintGrid->SelectProperty(hintProperty) );
    wxTextCtrl* const hintEditor = hintGrid->GetEditorTextCtrl();
    REQUIRE( hintEditor );
    CHECK( hintEditor->GetHint() == "Enter a value" );
    const wxWeakRef<wxWindow> weakHintEditor(hintEditor);
    hintGrid.reset();
    CHECK_FALSE( weakHintEditor );
}

#endif // wxUSE_PROPGRID
