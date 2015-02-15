/////////////////////////////////////////////////////////////////////////////
// Name:        samples/propgrid/propgrid.cpp
// Purpose:     wxPropertyGrid sample
// Author:      Jaakko Salli
// Modified by:
// Created:     2004-09-25
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//
//
// NOTES
//
// * Examples of custom property classes are in sampleprops.cpp.
//
// * Additional ones can be found below.
//
// * Currently there is no example of a custom property editor. However,
//   SpinCtrl editor sample is well-commented. It can be found in
//   src/propgrid/advprops.cpp.
//
// * To find code that populates the grid with properties, search for
//   string "::Populate".
//
// * To find code that handles property grid changes, search for string
//   "::OnPropertyGridChange".
//

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_PROPGRID
    #error "Please set wxUSE_PROPGRID to 1 and rebuild the library."
#endif

#include <wx/numdlg.h>

// -----------------------------------------------------------------------


// Main propertygrid header.
#include <wx/propgrid/propgrid.h>

// Extra property classes.
#include <wx/propgrid/advprops.h>

// This defines wxPropertyGridManager.
#include <wx/propgrid/manager.h>

#include "propgrid.h"
#include "sampleprops.h"

#if wxUSE_DATEPICKCTRL
    #include <wx/datectrl.h>
#endif

#include <wx/artprov.h>

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include <wx/popupwin.h>

// -----------------------------------------------------------------------
// wxSampleMultiButtonEditor
//   A sample editor class that has multiple buttons.
// -----------------------------------------------------------------------

class wxSampleMultiButtonEditor : public wxPGTextCtrlEditor
{
    wxDECLARE_DYNAMIC_CLASS(wxSampleMultiButtonEditor);
public:
    wxSampleMultiButtonEditor() {}
    virtual ~wxSampleMultiButtonEditor() {}

    virtual wxPGWindowList CreateControls( wxPropertyGrid* propGrid,
                                           wxPGProperty* property,
                                           const wxPoint& pos,
                                           const wxSize& sz ) const wxOVERRIDE;
    virtual bool OnEvent( wxPropertyGrid* propGrid,
                          wxPGProperty* property,
                          wxWindow* ctrl,
                          wxEvent& event ) const wxOVERRIDE;
};

IMPLEMENT_DYNAMIC_CLASS(wxSampleMultiButtonEditor, wxPGTextCtrlEditor)

wxPGWindowList wxSampleMultiButtonEditor::CreateControls( wxPropertyGrid* propGrid,
                                                          wxPGProperty* property,
                                                          const wxPoint& pos,
                                                          const wxSize& sz ) const
{
    // Create and populate buttons-subwindow
    wxPGMultiButton* buttons = new wxPGMultiButton( propGrid, sz );

    // Add two regular buttons
    buttons->Add( "..." );
    buttons->Add( "A" );
    // Add a bitmap button
    buttons->Add( wxArtProvider::GetBitmap(wxART_FOLDER) );

    // Create the 'primary' editor control (textctrl in this case)
    wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls
                             ( propGrid, property, pos,
                               buttons->GetPrimarySize() );

    // Finally, move buttons-subwindow to correct position and make sure
    // returned wxPGWindowList contains our custom button list.
    buttons->Finalize(propGrid, pos);

    wndList.SetSecondary( buttons );
    return wndList;
}

bool wxSampleMultiButtonEditor::OnEvent( wxPropertyGrid* propGrid,
                                         wxPGProperty* property,
                                         wxWindow* ctrl,
                                         wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_BUTTON )
    {
        wxPGMultiButton* buttons = (wxPGMultiButton*) propGrid->GetEditorControlSecondary();

        if ( event.GetId() == buttons->GetButtonId(0) )
        {
            // Do something when the first button is pressed
            wxLogDebug("First button pressed");
            return false;  // Return false since value did not change
        }
        if ( event.GetId() == buttons->GetButtonId(1) )
        {
            // Do something when the second button is pressed
            wxMessageBox("Second button pressed");
            return false;  // Return false since value did not change
        }
        if ( event.GetId() == buttons->GetButtonId(2) )
        {
            // Do something when the third button is pressed
            wxMessageBox("Third button pressed");
            return false;  // Return false since value did not change
        }
    }
    return wxPGTextCtrlEditor::OnEvent(propGrid, property, ctrl, event);
}

// -----------------------------------------------------------------------
// Validator for wxValidator use sample
// -----------------------------------------------------------------------

#if wxUSE_VALIDATORS

// wxValidator for testing

class wxInvalidWordValidator : public wxValidator
{
public:

    wxInvalidWordValidator( const wxString& invalidWord )
        : wxValidator(), m_invalidWord(invalidWord)
    {
    }

    virtual wxObject* Clone() const wxOVERRIDE
    {
        return new wxInvalidWordValidator(m_invalidWord);
    }

    virtual bool Validate(wxWindow* WXUNUSED(parent)) wxOVERRIDE
    {
        wxTextCtrl* tc = wxDynamicCast(GetWindow(), wxTextCtrl);
        wxCHECK_MSG(tc, true, wxT("validator window must be wxTextCtrl"));

        wxString val = tc->GetValue();

        if ( val.find(m_invalidWord) == wxString::npos )
            return true;

        ::wxMessageBox(wxString::Format(wxT("%s is not allowed word"),m_invalidWord.c_str()),
                       wxT("Validation Failure"));

        return false;
    }

private:
    wxString    m_invalidWord;
};

#endif // wxUSE_VALIDATORS

// -----------------------------------------------------------------------
// wxVectorProperty
// -----------------------------------------------------------------------

// See propgridsample.h for wxVector3f class

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wxVector3f)

wxPG_IMPLEMENT_PROPERTY_CLASS(wxVectorProperty,wxPGProperty,TextCtrl)


wxVectorProperty::wxVectorProperty( const wxString& label,
                                              const wxString& name,
                                              const wxVector3f& value )
    : wxPGProperty(label,name)
{
    SetValue( WXVARIANT(value) );
    AddPrivateChild( new wxFloatProperty(wxT("X"),wxPG_LABEL,value.x) );
    AddPrivateChild( new wxFloatProperty(wxT("Y"),wxPG_LABEL,value.y) );
    AddPrivateChild( new wxFloatProperty(wxT("Z"),wxPG_LABEL,value.z) );
}

wxVectorProperty::~wxVectorProperty() { }

void wxVectorProperty::RefreshChildren()
{
    if ( !GetChildCount() ) return;
    const wxVector3f& vector = wxVector3fRefFromVariant(m_value);
    Item(0)->SetValue( vector.x );
    Item(1)->SetValue( vector.y );
    Item(2)->SetValue( vector.z );
}

wxVariant wxVectorProperty::ChildChanged( wxVariant& thisValue,
                                          int childIndex,
                                          wxVariant& childValue ) const
{
    wxVector3f vector;
    vector << thisValue;
    switch ( childIndex )
    {
        case 0: vector.x = childValue.GetDouble(); break;
        case 1: vector.y = childValue.GetDouble(); break;
        case 2: vector.z = childValue.GetDouble(); break;
    }
    wxVariant newVariant;
    newVariant << vector;
    return newVariant;
}


// -----------------------------------------------------------------------
// wxTriangleProperty
// -----------------------------------------------------------------------

// See propgridsample.h for wxTriangle class

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wxTriangle)

wxPG_IMPLEMENT_PROPERTY_CLASS(wxTriangleProperty,wxPGProperty,TextCtrl)


wxTriangleProperty::wxTriangleProperty( const wxString& label,
                                                  const wxString& name,
                                                  const wxTriangle& value)
    : wxPGProperty(label,name)
{
    SetValue( WXVARIANT(value) );
    AddPrivateChild( new wxVectorProperty(wxT("A"),wxPG_LABEL,value.a) );
    AddPrivateChild( new wxVectorProperty(wxT("B"),wxPG_LABEL,value.b) );
    AddPrivateChild( new wxVectorProperty(wxT("C"),wxPG_LABEL,value.c) );
}

wxTriangleProperty::~wxTriangleProperty() { }

void wxTriangleProperty::RefreshChildren()
{
    if ( !GetChildCount() ) return;
    const wxTriangle& triangle = wxTriangleRefFromVariant(m_value);
    Item(0)->SetValue( WXVARIANT(triangle.a) );
    Item(1)->SetValue( WXVARIANT(triangle.b) );
    Item(2)->SetValue( WXVARIANT(triangle.c) );
}

wxVariant wxTriangleProperty::ChildChanged( wxVariant& thisValue,
                                            int childIndex,
                                            wxVariant& childValue ) const
{
    wxTriangle triangle;
    triangle << thisValue;
    const wxVector3f& vector = wxVector3fRefFromVariant(childValue);
    switch ( childIndex )
    {
        case 0: triangle.a = vector; break;
        case 1: triangle.b = vector; break;
        case 2: triangle.c = vector; break;
    }
    wxVariant newVariant;
    newVariant << triangle;
    return newVariant;
}


// -----------------------------------------------------------------------
// wxSingleChoiceDialogAdapter (wxPGEditorDialogAdapter sample)
// -----------------------------------------------------------------------

class wxSingleChoiceDialogAdapter : public wxPGEditorDialogAdapter
{
public:

    wxSingleChoiceDialogAdapter( const wxPGChoices& choices )
        : wxPGEditorDialogAdapter(), m_choices(choices)
    {
    }

    virtual bool DoShowDialog( wxPropertyGrid* WXUNUSED(propGrid),
                               wxPGProperty* WXUNUSED(property) ) wxOVERRIDE
    {
        wxString s = ::wxGetSingleChoice(wxT("Message"),
                                         wxT("Caption"),
                                         m_choices.GetLabels());
        if ( !s.empty() )
        {
            SetValue(s);
            return true;
        }

        return false;
    }

protected:
    const wxPGChoices&  m_choices;
};


class SingleChoiceProperty : public wxStringProperty
{
public:

    SingleChoiceProperty( const wxString& label,
                          const wxString& name = wxPG_LABEL,
                          const wxString& value = wxEmptyString )
        : wxStringProperty(label, name, value)
    {
        // Prepare choices
        m_choices.Add(wxT("Cat"));
        m_choices.Add(wxT("Dog"));
        m_choices.Add(wxT("Gibbon"));
        m_choices.Add(wxT("Otter"));
    }

    // Set editor to have button
    virtual const wxPGEditor* DoGetEditorClass() const wxOVERRIDE
    {
        return wxPGEditor_TextCtrlAndButton;
    }

    // Set what happens on button click
    virtual wxPGEditorDialogAdapter* GetEditorDialog() const wxOVERRIDE
    {
        return new wxSingleChoiceDialogAdapter(m_choices);
    }

protected:
    wxPGChoices m_choices;
};

// -----------------------------------------------------------------------
// Menu IDs
// -----------------------------------------------------------------------

enum
{
    PGID = 1,
    TCID,
    ID_ABOUT,
    ID_QUIT,
    ID_APPENDPROP,
    ID_APPENDCAT,
    ID_INSERTPROP,
    ID_INSERTCAT,
    ID_ENABLE,
    ID_SETREADONLY,
    ID_HIDE,
    ID_DELETE,
    ID_DELETER,
    ID_DELETEALL,
    ID_UNSPECIFY,
    ID_ITERATE1,
    ID_ITERATE2,
    ID_ITERATE3,
    ID_ITERATE4,
    ID_CLEARMODIF,
    ID_FREEZE,
    ID_DUMPLIST,
    ID_COLOURSCHEME1,
    ID_COLOURSCHEME2,
    ID_COLOURSCHEME3,
    ID_CATCOLOURS,
    ID_SETBGCOLOUR,
    ID_SETBGCOLOURRECUR,
    ID_STATICLAYOUT,
    ID_POPULATE1,
    ID_POPULATE2,
    ID_COLLAPSE,
    ID_COLLAPSEALL,
    ID_GETVALUES,
    ID_SETVALUES,
    ID_SETVALUES2,
    ID_RUNTESTFULL,
    ID_RUNTESTPARTIAL,
    ID_FITCOLUMNS,
    ID_CHANGEFLAGSITEMS,
    ID_TESTINSERTCHOICE,
    ID_TESTDELETECHOICE,
    ID_INSERTPAGE,
    ID_REMOVEPAGE,
    ID_SETSPINCTRLEDITOR,
    ID_SETPROPERTYVALUE,
    ID_TESTREPLACE,
    ID_SETCOLUMNS,
    ID_TESTXRC,
    ID_ENABLECOMMONVALUES,
    ID_SELECTSTYLE,
    ID_SAVESTATE,
    ID_RESTORESTATE,
    ID_RUNMINIMAL,
    ID_ENABLELABELEDITING,
    ID_VETOCOLDRAG,
    ID_SHOWHEADER,
    ID_ONEXTENDEDKEYNAV,
    ID_SHOWPOPUP,
    ID_POPUPGRID
};

// -----------------------------------------------------------------------
// Event table
// -----------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(FormMain, wxFrame)
    EVT_IDLE(FormMain::OnIdle)
    EVT_MOVE(FormMain::OnMove)
    EVT_SIZE(FormMain::OnResize)

    // This occurs when a property is selected
    EVT_PG_SELECTED( PGID, FormMain::OnPropertyGridSelect )
    // This occurs when a property value changes
    EVT_PG_CHANGED( PGID, FormMain::OnPropertyGridChange )
    // This occurs just prior a property value is changed
    EVT_PG_CHANGING( PGID, FormMain::OnPropertyGridChanging )
    // This occurs when a mouse moves over another property
    EVT_PG_HIGHLIGHTED( PGID, FormMain::OnPropertyGridHighlight )
    // This occurs when mouse is right-clicked.
    EVT_PG_RIGHT_CLICK( PGID, FormMain::OnPropertyGridItemRightClick )
    // This occurs when mouse is double-clicked.
    EVT_PG_DOUBLE_CLICK( PGID, FormMain::OnPropertyGridItemDoubleClick )
    // This occurs when propgridmanager's page changes.
    EVT_PG_PAGE_CHANGED( PGID, FormMain::OnPropertyGridPageChange )
    // This occurs when user starts editing a property label
    EVT_PG_LABEL_EDIT_BEGIN( PGID,
        FormMain::OnPropertyGridLabelEditBegin )
    // This occurs when user stops editing a property label
    EVT_PG_LABEL_EDIT_ENDING( PGID,
        FormMain::OnPropertyGridLabelEditEnding )
    // This occurs when property's editor button (if any) is clicked.
    EVT_BUTTON( PGID, FormMain::OnPropertyGridButtonClick )

    EVT_PG_ITEM_COLLAPSED( PGID, FormMain::OnPropertyGridItemCollapse )
    EVT_PG_ITEM_EXPANDED( PGID, FormMain::OnPropertyGridItemExpand )

    EVT_PG_COL_BEGIN_DRAG( PGID, FormMain::OnPropertyGridColBeginDrag )
    EVT_PG_COL_DRAGGING( PGID, FormMain::OnPropertyGridColDragging )
    EVT_PG_COL_END_DRAG( PGID, FormMain::OnPropertyGridColEndDrag )

    EVT_TEXT( PGID, FormMain::OnPropertyGridTextUpdate )

    //
    // Rest of the events are not property grid specific
    EVT_KEY_DOWN( FormMain::OnPropertyGridKeyEvent )
    EVT_KEY_UP( FormMain::OnPropertyGridKeyEvent )

    EVT_MENU( ID_APPENDPROP, FormMain::OnAppendPropClick )
    EVT_MENU( ID_APPENDCAT, FormMain::OnAppendCatClick )
    EVT_MENU( ID_INSERTPROP, FormMain::OnInsertPropClick )
    EVT_MENU( ID_INSERTCAT, FormMain::OnInsertCatClick )
    EVT_MENU( ID_DELETE, FormMain::OnDelPropClick )
    EVT_MENU( ID_DELETER, FormMain::OnDelPropRClick )
    EVT_MENU( ID_UNSPECIFY, FormMain::OnMisc )
    EVT_MENU( ID_DELETEALL, FormMain::OnClearClick )
    EVT_MENU( ID_ENABLE, FormMain::OnEnableDisable )
    EVT_MENU( ID_SETREADONLY, FormMain::OnSetReadOnly )
    EVT_MENU( ID_HIDE, FormMain::OnHide )

    EVT_MENU( ID_ITERATE1, FormMain::OnIterate1Click )
    EVT_MENU( ID_ITERATE2, FormMain::OnIterate2Click )
    EVT_MENU( ID_ITERATE3, FormMain::OnIterate3Click )
    EVT_MENU( ID_ITERATE4, FormMain::OnIterate4Click )
    EVT_MENU( ID_ONEXTENDEDKEYNAV, FormMain::OnExtendedKeyNav )
    EVT_MENU( ID_SETBGCOLOUR, FormMain::OnSetBackgroundColour )
    EVT_MENU( ID_SETBGCOLOURRECUR, FormMain::OnSetBackgroundColour )
    EVT_MENU( ID_CLEARMODIF, FormMain::OnClearModifyStatusClick )
    EVT_MENU( ID_FREEZE, FormMain::OnFreezeClick )
    EVT_MENU( ID_ENABLELABELEDITING, FormMain::OnEnableLabelEditing )
    EVT_MENU( ID_SHOWHEADER, FormMain::OnShowHeader )
    EVT_MENU( ID_DUMPLIST, FormMain::OnDumpList )

    EVT_MENU( ID_COLOURSCHEME1, FormMain::OnColourScheme )
    EVT_MENU( ID_COLOURSCHEME2, FormMain::OnColourScheme )
    EVT_MENU( ID_COLOURSCHEME3, FormMain::OnColourScheme )
    EVT_MENU( ID_COLOURSCHEME4, FormMain::OnColourScheme )

    EVT_MENU( ID_ABOUT, FormMain::OnAbout )
    EVT_MENU( ID_QUIT, FormMain::OnCloseClick )

    EVT_MENU( ID_CATCOLOURS, FormMain::OnCatColours )
    EVT_MENU( ID_SETCOLUMNS, FormMain::OnSetColumns )
    EVT_MENU( ID_TESTXRC, FormMain::OnTestXRC )
    EVT_MENU( ID_ENABLECOMMONVALUES, FormMain::OnEnableCommonValues )
    EVT_MENU( ID_SELECTSTYLE, FormMain::OnSelectStyle )

    EVT_MENU( ID_STATICLAYOUT, FormMain::OnMisc )
    EVT_MENU( ID_COLLAPSE, FormMain::OnMisc )
    EVT_MENU( ID_COLLAPSEALL, FormMain::OnMisc )

    EVT_MENU( ID_POPULATE1, FormMain::OnPopulateClick )
    EVT_MENU( ID_POPULATE2, FormMain::OnPopulateClick )

    EVT_MENU( ID_GETVALUES, FormMain::OnMisc )
    EVT_MENU( ID_SETVALUES, FormMain::OnMisc )
    EVT_MENU( ID_SETVALUES2, FormMain::OnMisc )

    EVT_MENU( ID_FITCOLUMNS, FormMain::OnFitColumnsClick )

    EVT_MENU( ID_CHANGEFLAGSITEMS, FormMain::OnChangeFlagsPropItemsClick )

    EVT_MENU( ID_RUNTESTFULL, FormMain::OnMisc )
    EVT_MENU( ID_RUNTESTPARTIAL, FormMain::OnMisc )

    EVT_MENU( ID_TESTINSERTCHOICE, FormMain::OnInsertChoice )
    EVT_MENU( ID_TESTDELETECHOICE, FormMain::OnDeleteChoice )

    EVT_MENU( ID_INSERTPAGE, FormMain::OnInsertPage )
    EVT_MENU( ID_REMOVEPAGE, FormMain::OnRemovePage )

    EVT_MENU( ID_SAVESTATE, FormMain::OnSaveState )
    EVT_MENU( ID_RESTORESTATE, FormMain::OnRestoreState )

    EVT_MENU( ID_SETSPINCTRLEDITOR, FormMain::OnSetSpinCtrlEditorClick )
    EVT_MENU( ID_TESTREPLACE, FormMain::OnTestReplaceClick )
    EVT_MENU( ID_SETPROPERTYVALUE, FormMain::OnSetPropertyValue )

    EVT_MENU( ID_RUNMINIMAL, FormMain::OnRunMinimalClick )

    EVT_CONTEXT_MENU( FormMain::OnContextMenu )
    EVT_BUTTON(ID_SHOWPOPUP, FormMain::OnShowPopup)
wxEND_EVENT_TABLE()

// -----------------------------------------------------------------------

void FormMain::OnMove( wxMoveEvent& event )
{
    if ( !m_pPropGridManager )
    {
        // this check is here so the frame layout can be tested
        // without creating propertygrid
        event.Skip();
        return;
    }

    // Update position properties
    int x, y;
    GetPosition(&x,&y);

    wxPGProperty* id;

    // Must check if properties exist (as they may be deleted).

    // Using m_pPropGridManager, we can scan all pages automatically.
    id = m_pPropGridManager->GetPropertyByName( wxT("X") );
    if ( id )
        m_pPropGridManager->SetPropertyValue( id, x );

    id = m_pPropGridManager->GetPropertyByName( wxT("Y") );
    if ( id )
        m_pPropGridManager->SetPropertyValue( id, y );

    id = m_pPropGridManager->GetPropertyByName( wxT("Position") );
    if ( id )
        m_pPropGridManager->SetPropertyValue( id, WXVARIANT(wxPoint(x,y)) );

    // Should always call event.Skip() in frame's MoveEvent handler
    event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnResize( wxSizeEvent& event )
{
    if ( !m_pPropGridManager )
    {
        // this check is here so the frame layout can be tested
        // without creating propertygrid
        event.Skip();
        return;
    }

    // Update size properties
    int w, h;
    GetSize(&w,&h);

    wxPGProperty* id;
    wxPGProperty* p;

    // Must check if properties exist (as they may be deleted).

    // Using m_pPropGridManager, we can scan all pages automatically.
    p = m_pPropGridManager->GetPropertyByName( wxT("Width") );
    if ( p && !p->IsValueUnspecified() )
        m_pPropGridManager->SetPropertyValue( p, w );

    p = m_pPropGridManager->GetPropertyByName( wxT("Height") );
    if ( p && !p->IsValueUnspecified() )
        m_pPropGridManager->SetPropertyValue( p, h );

    id = m_pPropGridManager->GetPropertyByName ( wxT("Size") );
    if ( id )
        m_pPropGridManager->SetPropertyValue( id, WXVARIANT(wxSize(w,h)) );

    // Should always call event.Skip() in frame's SizeEvent handler
    event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridChanging( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();

    if ( p->GetName() == wxT("Font") )
    {
        int res =
        wxMessageBox(wxString::Format(wxT("'%s' is about to change (to variant of type '%s')\n\nAllow or deny?"),
                                      p->GetName().c_str(),event.GetValue().GetType().c_str()),
                     wxT("Testing wxEVT_PG_CHANGING"), wxYES_NO, m_pPropGridManager);

        if ( res == wxNO )
        {
            wxASSERT(event.CanVeto());

            event.Veto();

            // Since we ask a question, it is better if we omit any validation
            // failure behaviour.
            event.SetValidationFailureBehavior(0);
        }
    }
}

//
// Note how we use three types of value getting in this method:
//   A) event.GetPropertyValueAsXXX
//   B) event.GetPropertValue, and then variant's GetXXX
//   C) grid's GetPropertyValueAsXXX(id)
//
void FormMain::OnPropertyGridChange( wxPropertyGridEvent& event )
{
    wxPGProperty* property = event.GetProperty();

    const wxString& name = property->GetName();

    // Properties store values internally as wxVariants, but it is preferred
    // to use the more modern wxAny at the interface level
    wxAny value = property->GetValue();

    // Don't handle 'unspecified' values
    if ( value.IsNull() )
        return;

    // Some settings are disabled outside Windows platform
    if ( name == wxT("X") )
        SetSize( value.As<int>(), -1, -1, -1, wxSIZE_USE_EXISTING );
    else if ( name == wxT("Y") )
    // wxPGVariantToInt is safe long int value getter
        SetSize ( -1, value.As<int>(), -1, -1, wxSIZE_USE_EXISTING );
    else if ( name == wxT("Width") )
        SetSize ( -1, -1, value.As<int>(), -1, wxSIZE_USE_EXISTING );
    else if ( name == wxT("Height") )
        SetSize ( -1, -1, -1, value.As<int>(), wxSIZE_USE_EXISTING );
    else if ( name == wxT("Label") )
    {
        SetTitle( value.As<wxString>() );
    }
    else if ( name == wxT("Password") )
    {
        static int pwdMode = 0;

        //m_pPropGridManager->SetPropertyAttribute(property, wxPG_STRING_PASSWORD, (long)pwdMode);

        pwdMode++;
        pwdMode &= 1;
    }
    else
    if ( name == wxT("Font") )
    {
        wxFont font = value.As<wxFont>();
        wxASSERT( font.IsOk() );

        m_pPropGridManager->SetFont( font );
    }
    else
    if ( name == wxT("Margin Colour") )
    {
        wxColourPropertyValue cpv = value.As<wxColourPropertyValue>();
        m_pPropGridManager->GetGrid()->SetMarginColour( cpv.m_colour );
    }
    else if ( name == wxT("Cell Colour") )
    {
        wxColourPropertyValue cpv = value.As<wxColourPropertyValue>();
        m_pPropGridManager->GetGrid()->SetCellBackgroundColour( cpv.m_colour );
    }
    else if ( name == wxT("Line Colour") )
    {
        wxColourPropertyValue cpv = value.As<wxColourPropertyValue>();
        m_pPropGridManager->GetGrid()->SetLineColour( cpv.m_colour );
    }
    else if ( name == wxT("Cell Text Colour") )
    {
        wxColourPropertyValue cpv = value.As<wxColourPropertyValue>();
        m_pPropGridManager->GetGrid()->SetCellTextColour( cpv.m_colour );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridSelect( wxPropertyGridEvent& event )
{
    wxPGProperty* property = event.GetProperty();
    if ( property )
    {
        m_itemEnable->Enable( true );
        if ( property->IsEnabled() )
            m_itemEnable->SetItemLabel( wxT("Disable") );
        else
            m_itemEnable->SetItemLabel( wxT("Enable") );
    }
    else
    {
        m_itemEnable->Enable( false );
    }

#if wxUSE_STATUSBAR
    wxPGProperty* prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop )
    {
        wxString text(wxT("Selected: "));
        text += m_pPropGridManager->GetPropertyLabel( prop );
        sb->SetStatusText ( text );
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridPageChange( wxPropertyGridEvent& WXUNUSED(event) )
{
#if wxUSE_STATUSBAR
    wxStatusBar* sb = GetStatusBar();
    wxString text(wxT("Page Changed: "));
    text += m_pPropGridManager->GetPageName(m_pPropGridManager->GetSelectedPage());
    sb->SetStatusText( text );
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridLabelEditBegin( wxPropertyGridEvent& event )
{
    wxLogMessage("wxPG_EVT_LABEL_EDIT_BEGIN(%s)",
                 event.GetProperty()->GetLabel().c_str());
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridLabelEditEnding( wxPropertyGridEvent& event )
{
    wxLogMessage("wxPG_EVT_LABEL_EDIT_ENDING(%s)",
                 event.GetProperty()->GetLabel().c_str());
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridHighlight( wxPropertyGridEvent& WXUNUSED(event) )
{
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemRightClick( wxPropertyGridEvent& event )
{
#if wxUSE_STATUSBAR
    wxPGProperty* prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop )
    {
        wxString text(wxT("Right-clicked: "));
        text += prop->GetLabel();
        text += wxT(", name=");
        text += m_pPropGridManager->GetPropertyName(prop);
        sb->SetStatusText( text );
    }
    else
    {
        sb->SetStatusText( wxEmptyString );
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemDoubleClick( wxPropertyGridEvent& event )
{
#if wxUSE_STATUSBAR
    wxPGProperty* prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop )
    {
        wxString text(wxT("Double-clicked: "));
        text += prop->GetLabel();
        text += wxT(", name=");
        text += m_pPropGridManager->GetPropertyName(prop);
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxEmptyString );
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridButtonClick ( wxCommandEvent& )
{
#if wxUSE_STATUSBAR
    wxPGProperty* prop = m_pPropGridManager->GetSelection();
    wxStatusBar* sb = GetStatusBar();
    if ( prop )
    {
        wxString text(wxT("Button clicked: "));
        text += m_pPropGridManager->GetPropertyLabel(prop);
        text += wxT(", name=");
        text += m_pPropGridManager->GetPropertyName(prop);
        sb->SetStatusText( text );
    }
    else
    {
        ::wxMessageBox(wxT("SHOULD NOT HAPPEN!!!"));
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemCollapse( wxPropertyGridEvent& )
{
    wxLogMessage(wxT("Item was Collapsed"));
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemExpand( wxPropertyGridEvent& )
{
    wxLogMessage(wxT("Item was Expanded"));
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridColBeginDrag( wxPropertyGridEvent& event )
{
    if ( m_itemVetoDragging->IsChecked() )
    {
        wxLogMessage("Splitter %i resize was vetoed", event.GetColumn());
        event.Veto();
    }
    else
    {
        wxLogMessage("Splitter %i resize began", event.GetColumn());
    }
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridColDragging( wxPropertyGridEvent& event )
{
    wxUnusedVar(event);
    // For now, let's not spam the log output
    //wxLogMessage("Splitter %i is being resized", event.GetColumn());
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridColEndDrag( wxPropertyGridEvent& event )
{
    wxLogMessage("Splitter %i resize ended", event.GetColumn());
}

// -----------------------------------------------------------------------

// EVT_TEXT handling
void FormMain::OnPropertyGridTextUpdate( wxCommandEvent& event )
{
    event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridKeyEvent( wxKeyEvent& WXUNUSED(event) )
{
    // Occurs on wxGTK mostly, but not wxMSW.
}

// -----------------------------------------------------------------------

void FormMain::OnLabelTextChange( wxCommandEvent& WXUNUSED(event) )
{
// Uncomment following to allow property label modify in real-time
//    wxPGProperty& p = m_pPropGridManager->GetGrid()->GetSelection();
//    if ( !p.IsOk() ) return;
//    m_pPropGridManager->SetPropertyLabel( p, m_tcPropLabel->DoGetValue() );
}

// -----------------------------------------------------------------------

static const wxChar* _fs_windowstyle_labels[] = {
    wxT("wxSIMPLE_BORDER"),
    wxT("wxDOUBLE_BORDER"),
    wxT("wxSUNKEN_BORDER"),
    wxT("wxRAISED_BORDER"),
    wxT("wxNO_BORDER"),
    wxT("wxTRANSPARENT_WINDOW"),
    wxT("wxTAB_TRAVERSAL"),
    wxT("wxWANTS_CHARS"),
#if wxNO_FULL_REPAINT_ON_RESIZE
    wxT("wxNO_FULL_REPAINT_ON_RESIZE"),
#endif
    wxT("wxVSCROLL"),
    wxT("wxALWAYS_SHOW_SB"),
    wxT("wxCLIP_CHILDREN"),
#if wxFULL_REPAINT_ON_RESIZE
    wxT("wxFULL_REPAINT_ON_RESIZE"),
#endif
    (const wxChar*) NULL // terminator is always needed
};

static const long _fs_windowstyle_values[] = {
    wxSIMPLE_BORDER,
    wxDOUBLE_BORDER,
    wxSUNKEN_BORDER,
    wxRAISED_BORDER,
    wxNO_BORDER,
    wxTRANSPARENT_WINDOW,
    wxTAB_TRAVERSAL,
    wxWANTS_CHARS,
#if wxNO_FULL_REPAINT_ON_RESIZE
    wxNO_FULL_REPAINT_ON_RESIZE,
#endif
    wxVSCROLL,
    wxALWAYS_SHOW_SB,
    wxCLIP_CHILDREN,
#if wxFULL_REPAINT_ON_RESIZE
    wxFULL_REPAINT_ON_RESIZE
#endif
};

static const wxChar* _fs_framestyle_labels[] = {
    wxT("wxCAPTION"),
    wxT("wxMINIMIZE"),
    wxT("wxMAXIMIZE"),
    wxT("wxCLOSE_BOX"),
    wxT("wxSTAY_ON_TOP"),
    wxT("wxSYSTEM_MENU"),
    wxT("wxRESIZE_BORDER"),
    wxT("wxFRAME_TOOL_WINDOW"),
    wxT("wxFRAME_NO_TASKBAR"),
    wxT("wxFRAME_FLOAT_ON_PARENT"),
    wxT("wxFRAME_SHAPED"),
    (const wxChar*) NULL
};

static const long _fs_framestyle_values[] = {
    wxCAPTION,
    wxMINIMIZE,
    wxMAXIMIZE,
    wxCLOSE_BOX,
    wxSTAY_ON_TOP,
    wxSYSTEM_MENU,
    wxRESIZE_BORDER,
    wxFRAME_TOOL_WINDOW,
    wxFRAME_NO_TASKBAR,
    wxFRAME_FLOAT_ON_PARENT,
    wxFRAME_SHAPED
};

// -----------------------------------------------------------------------

void FormMain::OnTestXRC(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("Sorrt, not yet implemented"));
}

void FormMain::OnEnableCommonValues(wxCommandEvent& WXUNUSED(event))
{
    wxPGProperty* prop = m_pPropGridManager->GetSelection();
    if ( prop )
        prop->EnableCommonValue();
    else
        wxMessageBox(wxT("First select a property"));
}

void FormMain::PopulateWithStandardItems ()
{
    wxPropertyGridManager* pgman = m_pPropGridManager;
    wxPropertyGridPage* pg = pgman->GetPage(wxT("Standard Items"));

    // Append is ideal way to add items to wxPropertyGrid.
    pg->Append( new wxPropertyCategory(wxT("Appearance"),wxPG_LABEL) );

    pg->Append( new wxStringProperty(wxT("Label"),wxPG_LABEL,GetTitle()) );
    pg->Append( new wxFontProperty(wxT("Font"),wxPG_LABEL) );
    pg->SetPropertyHelpString ( wxT("Font"), wxT("Editing this will change font used in the property grid.") );

    pg->Append( new wxSystemColourProperty(wxT("Margin Colour"),wxPG_LABEL,
        pg->GetGrid()->GetMarginColour()) );

    pg->Append( new wxSystemColourProperty(wxT("Cell Colour"),wxPG_LABEL,
        pg->GetGrid()->GetCellBackgroundColour()) );
    pg->Append( new wxSystemColourProperty(wxT("Cell Text Colour"),wxPG_LABEL,
        pg->GetGrid()->GetCellTextColour()) );
    pg->Append( new wxSystemColourProperty(wxT("Line Colour"),wxPG_LABEL,
        pg->GetGrid()->GetLineColour()) );
    pg->Append( new wxFlagsProperty(wxT("Window Styles"),wxPG_LABEL,
        m_combinedFlags, GetWindowStyle()) );

    //pg->SetPropertyAttribute(wxT("Window Styles"),wxPG_BOOL_USE_CHECKBOX,true,wxPG_RECURSE);

    pg->Append( new wxCursorProperty(wxT("Cursor"),wxPG_LABEL) );

    pg->Append( new wxPropertyCategory(wxT("Position"),wxT("PositionCategory")) );
    pg->SetPropertyHelpString( wxT("PositionCategory"), wxT("Change in items in this category will cause respective changes in frame.") );

    // Let's demonstrate 'Units' attribute here

    // Note that we use many attribute constants instead of strings here
    // (for instance, wxPG_ATTR_MIN, instead of wxT("min")).
    // Using constant may reduce binary size.

    pg->Append( new wxIntProperty(wxT("Height"),wxPG_LABEL,480) );
    pg->SetPropertyAttribute(wxT("Height"), wxPG_ATTR_MIN, (long)10 );
    pg->SetPropertyAttribute(wxT("Height"), wxPG_ATTR_MAX, (long)2048 );
    pg->SetPropertyAttribute(wxT("Height"), wxPG_ATTR_UNITS, wxT("Pixels") );

    // Set value to unspecified so that Hint attribute will be demonstrated
    pg->SetPropertyValueUnspecified("Height");
    pg->SetPropertyAttribute("Height", wxPG_ATTR_HINT,
                             "Enter new height for window" );

    // Difference between hint and help string is that the hint is shown in
    // an empty value cell, while help string is shown either in the
    // description text box, as a tool tip, or on the status bar.
    pg->SetPropertyHelpString("Height",
        "This property uses attributes \"Units\" and \"Hint\"." );

    pg->Append( new wxIntProperty(wxT("Width"),wxPG_LABEL,640) );
    pg->SetPropertyAttribute(wxT("Width"), wxPG_ATTR_MIN, (long)10 );
    pg->SetPropertyAttribute(wxT("Width"), wxPG_ATTR_MAX, (long)2048 );
    pg->SetPropertyAttribute(wxT("Width"), wxPG_ATTR_UNITS, wxT("Pixels") );

    pg->SetPropertyValueUnspecified(wxT("Width"));
    pg->SetPropertyAttribute("Width", wxPG_ATTR_HINT,
                             "Enter new width for window" );
    pg->SetPropertyHelpString("Width",
        "This property uses attributes \"Units\" and \"Hint\"." );

    pg->Append( new wxIntProperty(wxT("X"),wxPG_LABEL,10) );
    pg->SetPropertyAttribute(wxT("X"), wxPG_ATTR_UNITS, wxT("Pixels") );
    pg->SetPropertyHelpString(wxT("X"), wxT("This property uses \"Units\" attribute.") );

    pg->Append( new wxIntProperty(wxT("Y"),wxPG_LABEL,10) );
    pg->SetPropertyAttribute(wxT("Y"), wxPG_ATTR_UNITS, wxT("Pixels") );
    pg->SetPropertyHelpString(wxT("Y"), wxT("This property uses \"Units\" attribute.") );

    const wxChar* disabledHelpString = wxT("This property is simply disabled. In order to have label disabled as well, ")
                                       wxT("you need to set wxPG_EX_GREY_LABEL_WHEN_DISABLED using SetExtraStyle.");

    pg->Append( new wxPropertyCategory(wxT("Environment"),wxPG_LABEL) );
    pg->Append( new wxStringProperty(wxT("Operating System"),wxPG_LABEL,::wxGetOsDescription()) );

    pg->Append( new wxStringProperty(wxT("User Id"),wxPG_LABEL,::wxGetUserId()) );
    pg->Append( new wxDirProperty(wxT("User Home"),wxPG_LABEL,::wxGetUserHome()) );
    pg->Append( new wxStringProperty(wxT("User Name"),wxPG_LABEL,::wxGetUserName()) );

    // Disable some of them
    pg->DisableProperty( wxT("Operating System") );
    pg->DisableProperty( wxT("User Id") );
    pg->DisableProperty( wxT("User Name") );

    pg->SetPropertyHelpString( wxT("Operating System"), disabledHelpString );
    pg->SetPropertyHelpString( wxT("User Id"), disabledHelpString );
    pg->SetPropertyHelpString( wxT("User Name"), disabledHelpString );

    pg->Append( new wxPropertyCategory(wxT("More Examples"),wxPG_LABEL) );

    pg->Append( new wxFontDataProperty( wxT("FontDataProperty"), wxPG_LABEL) );
    pg->SetPropertyHelpString( wxT("FontDataProperty"),
        wxT("This demonstrates wxFontDataProperty class defined in this sample app. ")
        wxT("It is exactly like wxFontProperty from the library, but also has colour sub-property.")
        );

    pg->Append( new wxDirsProperty(wxT("DirsProperty"),wxPG_LABEL) );
    pg->SetPropertyHelpString( wxT("DirsProperty"),
        wxT("This demonstrates wxDirsProperty class defined in this sample app. ")
        wxT("It is built with WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR macro, ")
        wxT("with custom action (dir dialog popup) defined.")
        );

    wxArrayDouble arrdbl;
    arrdbl.Add(-1.0);
    arrdbl.Add(-0.5);
    arrdbl.Add(0.0);
    arrdbl.Add(0.5);
    arrdbl.Add(1.0);

    pg->Append( new wxArrayDoubleProperty(wxT("ArrayDoubleProperty"),wxPG_LABEL,arrdbl) );
    //pg->SetPropertyAttribute(wxT("ArrayDoubleProperty"),wxPG_FLOAT_PRECISION,(long)2);
    pg->SetPropertyHelpString( wxT("ArrayDoubleProperty"),
        wxT("This demonstrates wxArrayDoubleProperty class defined in this sample app. ")
        wxT("It is an example of a custom list editor property.")
        );

    pg->Append( new wxLongStringProperty(wxT("Information"),wxPG_LABEL,
        wxT("Editing properties will have immediate effect on this window, ")
        wxT("and vice versa (at least in most cases, that is).")
        ) );
    pg->SetPropertyHelpString( wxT("Information"),
                               wxT("This property is read-only.") );

    pg->SetPropertyReadOnly( wxT("Information"), true );

    //
    // Set test information for cells in columns 3 and 4
    // (reserve column 2 for displaying units)
    wxPropertyGridIterator it;
    wxBitmap bmp = wxArtProvider::GetBitmap(wxART_FOLDER);

    for ( it = pg->GetGrid()->GetIterator();
          !it.AtEnd();
          it++ )
    {
        wxPGProperty* p = *it;
        if ( p->IsCategory() )
            continue;

        pg->SetPropertyCell( p, 3, wxT("Cell 3"), bmp );
        pg->SetPropertyCell( p, 4, wxT("Cell 4"), wxNullBitmap, *wxWHITE, *wxBLACK );
    }
}

// -----------------------------------------------------------------------

void FormMain::PopulateWithExamples ()
{
    wxPropertyGridManager* pgman = m_pPropGridManager;
    wxPropertyGridPage* pg = pgman->GetPage(wxT("Examples"));
    wxPGProperty* pid;
    wxPGProperty* prop;

    //pg->Append( new wxPropertyCategory(wxT("Examples (low priority)"),wxT("Examples")) );
    //pg->SetPropertyHelpString ( wxT("Examples"), wxT("This category has example of (almost) every built-in property class.") );

#if wxUSE_SPINBTN
    pg->Append( new wxIntProperty ( wxT("SpinCtrl"), wxPG_LABEL, 0 ) );

    pg->SetPropertyEditor( wxT("SpinCtrl"), wxPGEditor_SpinCtrl );
    pg->SetPropertyAttribute( wxT("SpinCtrl"), wxPG_ATTR_MIN, (long)-10 );  // Use constants instead of string
    pg->SetPropertyAttribute( wxT("SpinCtrl"), wxPG_ATTR_MAX, (long)16384 );   // for reduced binary size.
    pg->SetPropertyAttribute( wxT("SpinCtrl"), wxPG_ATTR_SPINCTRL_STEP, (long)2 );
    pg->SetPropertyAttribute( wxT("SpinCtrl"), wxPG_ATTR_SPINCTRL_MOTION, true );
    //pg->SetPropertyAttribute( wxT("SpinCtrl"), wxPG_ATTR_SPINCTRL_WRAP, true );

    pg->SetPropertyHelpString( wxT("SpinCtrl"),
        wxT("This is regular wxIntProperty, which editor has been ")
        wxT("changed to wxPGEditor_SpinCtrl. Note however that ")
        wxT("static wxPropertyGrid::RegisterAdditionalEditors() ")
        wxT("needs to be called prior to using it."));

#endif

    // Add bool property
    pg->Append( new wxBoolProperty( wxT("BoolProperty"), wxPG_LABEL, false ) );

    // Add bool property with check box
    pg->Append( new wxBoolProperty( wxT("BoolProperty with CheckBox"), wxPG_LABEL, false ) );
    pg->SetPropertyAttribute( wxT("BoolProperty with CheckBox"),
                              wxPG_BOOL_USE_CHECKBOX,
                              true );

    pg->SetPropertyHelpString( wxT("BoolProperty with CheckBox"),
        wxT("Property attribute wxPG_BOOL_USE_CHECKBOX has been set to true.") );

    prop = pg->Append( new wxFloatProperty("FloatProperty",
                                           wxPG_LABEL,
                                           1234500.23) );
    prop->SetAttribute(wxPG_ATTR_MIN, -100.12);

    // A string property that can be edited in a separate editor dialog.
    pg->Append( new wxLongStringProperty( wxT("LongStringProperty"), wxT("LongStringProp"),
        wxT("This is much longer string than the first one. Edit it by clicking the button.") ) );

    // A property that edits a wxArrayString.
    wxArrayString example_array;
    example_array.Add( wxT("String 1"));
    example_array.Add( wxT("String 2"));
    example_array.Add( wxT("String 3"));
    pg->Append( new wxArrayStringProperty( wxT("ArrayStringProperty"), wxPG_LABEL,
                                           example_array) );

    // Test adding same category multiple times ( should not actually create a new one )
    //pg->Append( new wxPropertyCategory(wxT("Examples (low priority)"),wxT("Examples")) );

    // A file selector property. Note that argument between name
    // and initial value is wildcard (format same as in wxFileDialog).
    prop = new wxFileProperty( wxT("FileProperty"), wxT("TextFile") );
    pg->Append( prop );

    prop->SetAttribute(wxPG_FILE_WILDCARD,wxT("Text Files (*.txt)|*.txt"));
    prop->SetAttribute(wxPG_FILE_DIALOG_TITLE,wxT("Custom File Dialog Title"));
    prop->SetAttribute(wxPG_FILE_SHOW_FULL_PATH,false);

#ifdef __WXMSW__
    prop->SetAttribute(wxPG_FILE_SHOW_RELATIVE_PATH,wxT("C:\\Windows"));
    pg->SetPropertyValue(prop,wxT("C:\\Windows\\System32\\msvcrt71.dll"));
#endif

#if wxUSE_IMAGE
    // An image file property. Arguments are just like for FileProperty, but
    // wildcard is missing (it is autogenerated from supported image formats).
    // If you really need to override it, create property separately, and call
    // its SetWildcard method.
    pg->Append( new wxImageFileProperty( wxT("ImageFile"), wxPG_LABEL ) );
#endif

    pid = pg->Append( new wxColourProperty(wxT("ColourProperty"),wxPG_LABEL,*wxRED) );
    pg->SetPropertyEditor( wxT("ColourProperty"), wxPGEditor_ComboBox );
    pg->GetProperty(wxT("ColourProperty"))->SetAutoUnspecified(true);
    pg->SetPropertyHelpString( wxT("ColourProperty"),
        wxT("wxPropertyGrid::SetPropertyEditor method has been used to change ")
        wxT("editor of this property to wxPGEditor_ComboBox)"));

    pid = pg->Append( new wxColourProperty("ColourPropertyWithAlpha",
                                           wxPG_LABEL,
                                           wxColour(15, 200, 95, 128)) );
    pg->SetPropertyAttribute("ColourPropertyWithAlpha", wxPG_COLOUR_HAS_ALPHA, true);
    pg->SetPropertyHelpString("ColourPropertyWithAlpha",
        "Attribute \"HasAlpha\" is set to true for this property.");

    //
    // This demonstrates using alternative editor for colour property
    // to trigger colour dialog directly from button.
    pg->Append( new wxColourProperty(wxT("ColourProperty2"),wxPG_LABEL,*wxGREEN) );

    //
    // wxEnumProperty does not store strings or even list of strings
    // ( so that's why they are static in function ).
    static const wxChar* enum_prop_labels[] = { wxT("One Item"),
        wxT("Another Item"), wxT("One More"), wxT("This Is Last"), NULL };

    // this value array would be optional if values matched string indexes
    static long enum_prop_values[] = { 40, 80, 120, 160 };

    // note that the initial value (the last argument) is the actual value,
    // not index or anything like that. Thus, our value selects "Another Item".
    //
    // 0 before value is number of items. If it is 0, like in our example,
    // number of items is calculated, and this requires that the string pointer
    // array is terminated with NULL.
    pg->Append( new wxEnumProperty(wxT("EnumProperty"),wxPG_LABEL,
        enum_prop_labels, enum_prop_values, 80 ) );

    wxPGChoices soc;

    // use basic table from our previous example
    // can also set/add wxArrayStrings and wxArrayInts directly.
    soc.Set( enum_prop_labels, enum_prop_values );

    // add extra items
    soc.Add( wxT("Look, it continues"), 200 );
    soc.Add( wxT("Even More"), 240 );
    soc.Add( wxT("And More"), 280 );
    soc.Add( "", 300 );
    soc.Add( wxT("True End of the List"), 320 );

    // Test custom colours ([] operator of wxPGChoices returns
    // references to wxPGChoiceEntry).
    soc[1].SetFgCol(*wxRED);
    soc[1].SetBgCol(*wxLIGHT_GREY);
    soc[2].SetFgCol(*wxGREEN);
    soc[2].SetBgCol(*wxLIGHT_GREY);
    soc[3].SetFgCol(*wxBLUE);
    soc[3].SetBgCol(*wxLIGHT_GREY);
    soc[4].SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER));

    pg->Append( new wxEnumProperty(wxT("EnumProperty 2"),
                                   wxPG_LABEL,
                                   soc,
                                  240) );
    pg->GetProperty(wxT("EnumProperty 2"))->AddChoice(wxT("Testing Extra"), 360);

    // Here we only display the original 'soc' choices
    pg->Append( new wxEnumProperty(wxT("EnumProperty 3"),wxPG_LABEL,
        soc, 240 ) );

    // Test Hint attribute in EnumProperty
    pg->GetProperty("EnumProperty 3")->SetAttribute(wxPG_ATTR_HINT, "Dummy Hint");

    pg->SetPropertyHelpString("EnumProperty 3",
        "This property uses \"Hint\" attribute.");

    // 'soc' plus one exclusive extra choice "4th only"
    pg->Append( new wxEnumProperty(wxT("EnumProperty 4"),wxPG_LABEL,
        soc, 240 ) );
    pg->GetProperty(wxT("EnumProperty 4"))->AddChoice(wxT("4th only"), 360);

    pg->SetPropertyHelpString(wxT("EnumProperty 4"),
        wxT("Should have one extra item when compared to EnumProperty 3"));

    // Password property example.
    pg->Append( new wxStringProperty(wxT("Password"),wxPG_LABEL, wxT("password")) );
    pg->SetPropertyAttribute( wxT("Password"), wxPG_STRING_PASSWORD, true );
    pg->SetPropertyHelpString( wxT("Password"),
        wxT("Has attribute wxPG_STRING_PASSWORD set to true") );

    // String editor with dir selector button. Uses wxEmptyString as name, which
    // is allowed (naturally, in this case property cannot be accessed by name).
    pg->Append( new wxDirProperty( wxT("DirProperty"), wxPG_LABEL, ::wxGetUserHome()) );
    pg->SetPropertyAttribute( wxT("DirProperty"),
                              wxPG_DIR_DIALOG_MESSAGE,
                              wxT("This is a custom dir dialog message") );

    // Add string property - first arg is label, second name, and third initial value
    pg->Append( new wxStringProperty ( wxT("StringProperty"), wxPG_LABEL ) );
    pg->SetPropertyMaxLength( wxT("StringProperty"), 6 );
    pg->SetPropertyHelpString( wxT("StringProperty"),
        wxT("Max length of this text has been limited to 6, using wxPropertyGrid::SetPropertyMaxLength.") );

    // Set value after limiting so that it will be applied
    pg->SetPropertyValue( wxT("StringProperty"), wxT("some text") );

    //
    // Demonstrate "AutoComplete" attribute
    pg->Append( new wxStringProperty( "StringProperty AutoComplete",
                                      wxPG_LABEL ) );

    wxArrayString autoCompleteStrings;
    autoCompleteStrings.Add("One choice");
    autoCompleteStrings.Add("Another choice");
    autoCompleteStrings.Add("Another choice, yeah");
    autoCompleteStrings.Add("Yet another choice");
    autoCompleteStrings.Add("Yet another choice, bear with me");
    pg->SetPropertyAttribute( "StringProperty AutoComplete",
                              wxPG_ATTR_AUTOCOMPLETE,
                              autoCompleteStrings );

    pg->SetPropertyHelpString( "StringProperty AutoComplete",
        "AutoComplete attribute has been set for this property "
        "(try writing something beginning with 'a', 'o' or 'y').");

    // Add string property with arbitrarily wide bitmap in front of it. We
    // intentionally lower-than-typical row height here so that the ugly
    // scaling code wont't be run.
    pg->Append( new wxStringProperty( wxT("StringPropertyWithBitmap"),
                wxPG_LABEL,
                wxT("Test Text")) );
    wxBitmap myTestBitmap(60, 15, 32);
    wxMemoryDC mdc;
    mdc.SelectObject(myTestBitmap);
    mdc.Clear();
    mdc.SetPen(*wxBLACK);
    mdc.DrawLine(0, 0, 60, 15);
    mdc.SelectObject(wxNullBitmap);
    pg->SetPropertyImage( wxT("StringPropertyWithBitmap"), myTestBitmap );


    // this value array would be optional if values matched string indexes
    //long flags_prop_values[] = { wxICONIZE, wxCAPTION, wxMINIMIZE_BOX, wxMAXIMIZE_BOX };

    //pg->Append( wxFlagsProperty(wxT("Example of FlagsProperty"),wxT("FlagsProp"),
    //    flags_prop_labels, flags_prop_values, 0, GetWindowStyle() ) );


    // Multi choice dialog.
    wxArrayString tchoices;
    tchoices.Add(wxT("Cabbage"));
    tchoices.Add(wxT("Carrot"));
    tchoices.Add(wxT("Onion"));
    tchoices.Add(wxT("Potato"));
    tchoices.Add(wxT("Strawberry"));

    wxArrayString tchoicesValues;
    tchoicesValues.Add(wxT("Carrot"));
    tchoicesValues.Add(wxT("Potato"));

    pg->Append( new wxEnumProperty(wxT("EnumProperty X"),wxPG_LABEL, tchoices ) );

    pg->Append( new wxMultiChoiceProperty( wxT("MultiChoiceProperty"), wxPG_LABEL,
                                           tchoices, tchoicesValues ) );
    pg->SetPropertyAttribute( wxT("MultiChoiceProperty"), wxPG_ATTR_MULTICHOICE_USERSTRINGMODE, true );

    pg->Append( new wxSizeProperty( wxT("SizeProperty"), wxT("Size"), GetSize() ) );
    pg->Append( new wxPointProperty( wxT("PointProperty"), wxT("Position"), GetPosition() ) );

    // UInt samples
    pg->Append( new wxUIntProperty( wxT("UIntProperty"), wxPG_LABEL, wxULongLong(wxULL(0xFEEEFEEEFEEE))));
    pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_PREFIX, wxPG_PREFIX_NONE );
    pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_BASE, wxPG_BASE_HEX );
    //pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_PREFIX, wxPG_PREFIX_NONE );
    //pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_BASE, wxPG_BASE_OCT );

    //
    // wxEditEnumProperty
    wxPGChoices eech;
    eech.Add(wxT("Choice 1"));
    eech.Add(wxT("Choice 2"));
    eech.Add(wxT("Choice 3"));
    pg->Append( new wxEditEnumProperty("EditEnumProperty",
                                       wxPG_LABEL,
                                       eech,
                                       "Choice not in the list") );

    // Test Hint attribute in EditEnumProperty
    pg->GetProperty("EditEnumProperty")->SetAttribute(wxPG_ATTR_HINT, "Dummy Hint");

    //wxString v_;
    //wxTextValidator validator1(wxFILTER_NUMERIC,&v_);
    //pg->SetPropertyValidator( wxT("EditEnumProperty"), validator1 );

#if wxUSE_DATETIME
    //
    // wxDateTimeProperty
    pg->Append( new wxDateProperty(wxT("DateProperty"), wxPG_LABEL, wxDateTime::Now() ) );

#if wxUSE_DATEPICKCTRL
    pg->SetPropertyAttribute( wxT("DateProperty"), wxPG_DATE_PICKER_STYLE,
                              (long)(wxDP_DROPDOWN |
                                     wxDP_SHOWCENTURY |
                                     wxDP_ALLOWNONE) );

    pg->SetPropertyHelpString( wxT("DateProperty"),
        wxT("Attribute wxPG_DATE_PICKER_STYLE has been set to (long)")
        wxT("(wxDP_DROPDOWN | wxDP_SHOWCENTURY | wxDP_ALLOWNONE).") );
#endif

#endif

    //
    // Add Triangle properties as both wxTriangleProperty and
    // a generic parent property (using wxStringProperty).
    //
    wxPGProperty* topId = pg->Append( new wxStringProperty(wxT("3D Object"), wxPG_LABEL, wxT("<composed>")) );

    pid = pg->AppendIn( topId, new wxStringProperty(wxT("Triangle 1"), wxT("Triangle 1"), wxT("<composed>")) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("A"), wxPG_LABEL ) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("B"), wxPG_LABEL ) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("C"), wxPG_LABEL ) );

    pg->AppendIn( topId, new wxTriangleProperty( wxT("Triangle 2"), wxT("Triangle 2") ) );

    pg->SetPropertyHelpString( wxT("3D Object"),
        wxT("3D Object is wxStringProperty with value \"<composed>\". Two of its children are similar wxStringProperties with ")
        wxT("three wxVectorProperty children, and other two are custom wxTriangleProperties.") );

    pid = pg->AppendIn( topId, new wxStringProperty(wxT("Triangle 3"), wxT("Triangle 3"), wxT("<composed>")) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("A"), wxPG_LABEL ) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("B"), wxPG_LABEL ) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("C"), wxPG_LABEL ) );

    pg->AppendIn( topId, new wxTriangleProperty( wxT("Triangle 4"), wxT("Triangle 4") ) );

    //
    // This snippet is a doc sample test
    //
    wxPGProperty* carProp = pg->Append(new wxStringProperty(wxT("Car"),
                                         wxPG_LABEL,
                                         wxT("<composed>")));

    pg->AppendIn(carProp, new wxStringProperty(wxT("Model"),
                                                wxPG_LABEL,
                                                wxT("Lamborghini Diablo SV")));

    pg->AppendIn(carProp, new wxIntProperty(wxT("Engine Size (cc)"),
                                            wxPG_LABEL,
                                            5707) );

    wxPGProperty* speedsProp = pg->AppendIn(carProp,
                                            new wxStringProperty(wxT("Speeds"),
                                              wxPG_LABEL,
                                              wxT("<composed>")));

    pg->AppendIn( speedsProp, new wxIntProperty(wxT("Max. Speed (mph)"),
                                                wxPG_LABEL,290) );
    pg->AppendIn( speedsProp, new wxFloatProperty(wxT("0-100 mph (sec)"),
                                                  wxPG_LABEL,3.9) );
    pg->AppendIn( speedsProp, new wxFloatProperty(wxT("1/4 mile (sec)"),
                                                  wxPG_LABEL,8.6) );

    // This is how child property can be referred to by name
    pg->SetPropertyValue( wxT("Car.Speeds.Max. Speed (mph)"), 300 );

    pg->AppendIn(carProp, new wxIntProperty(wxT("Price ($)"),
                                            wxPG_LABEL,
                                            300000) );

    pg->AppendIn(carProp, new wxBoolProperty(wxT("Convertible"),
                                             wxPG_LABEL,
                                             false) );

    // Displayed value of "Car" property is now very close to this:
    // "Lamborghini Diablo SV; 5707 [300; 3.9; 8.6] 300000"

    //
    // Test wxSampleMultiButtonEditor
    pg->Append( new wxLongStringProperty(wxT("MultipleButtons"), wxPG_LABEL) );
    pg->SetPropertyEditor(wxT("MultipleButtons"), m_pSampleMultiButtonEditor );

    // Test SingleChoiceProperty
    pg->Append( new SingleChoiceProperty(wxT("SingleChoiceProperty")) );


    //
    // Test adding variable height bitmaps in wxPGChoices
    wxPGChoices bc;

    bc.Add(wxT("Wee"), wxBitmap(16, 16));
    bc.Add(wxT("Not so wee"), wxBitmap(32, 32));
    bc.Add(wxT("Friggin' huge"), wxBitmap(64, 64));

    pg->Append( new wxEnumProperty(wxT("Variable Height Bitmaps"),
                                   wxPG_LABEL,
                                   bc,
                                   0) );

    //
    // Test how non-editable composite strings appear
    pid = new wxStringProperty(wxT("wxWidgets Traits"), wxPG_LABEL, wxT("<composed>"));
    pg->SetPropertyReadOnly(pid);

    //
    // For testing purposes, combine two methods of adding children
    //

    pid->AppendChild( new wxStringProperty(wxT("Latest Release"),
                                           wxPG_LABEL,
                                           wxT("2.8.10")));
    pid->AppendChild( new wxBoolProperty(wxT("Win API"),
                                         wxPG_LABEL,
                                         true) );

    pg->Append( pid );

    pg->AppendIn(pid, new wxBoolProperty(wxT("QT"), wxPG_LABEL, false) );
    pg->AppendIn(pid, new wxBoolProperty(wxT("Cocoa"), wxPG_LABEL, true) );
    pg->AppendIn(pid, new wxBoolProperty(wxT("BeOS"), wxPG_LABEL, false) );
    pg->AppendIn(pid, new wxStringProperty(wxT("SVN Trunk Version"), wxPG_LABEL, wxT("2.9.0")) );
    pg->AppendIn(pid, new wxBoolProperty(wxT("GTK+"), wxPG_LABEL, true) );
    pg->AppendIn(pid, new wxBoolProperty(wxT("Sky OS"), wxPG_LABEL, false) );
    pg->AppendIn(pid, new wxBoolProperty(wxT("QT"), wxPG_LABEL, false) );

    AddTestProperties(pg);
}

// -----------------------------------------------------------------------

void FormMain::PopulateWithLibraryConfig ()
{
    wxPropertyGridManager* pgman = m_pPropGridManager;
    wxPropertyGridPage* pg = pgman->GetPage(wxT("wxWidgets Library Config"));

    // Set custom column proportions (here in the sample app we need
    // to check if the grid has wxPG_SPLITTER_AUTO_CENTER style. You usually
    // need not to do it in your application).
    if ( pgman->HasFlag(wxPG_SPLITTER_AUTO_CENTER) )
    {
        pg->SetColumnProportion(0, 3);
        pg->SetColumnProportion(1, 1);
    }

    wxPGProperty* cat;

    wxBitmap bmp = wxArtProvider::GetBitmap(wxART_REPORT_VIEW);

    wxPGProperty* pid;

    wxFont italicFont = pgman->GetGrid()->GetCaptionFont();
    italicFont.SetStyle(wxFONTSTYLE_ITALIC);

    wxString italicFontHelp = "Font of this property's wxPGCell has "
                              "been modified. Obtain property's cell "
                              "with wxPGProperty::"
                              "GetOrCreateCell(column).";

#define ADD_WX_LIB_CONF_GROUP(A) \
    cat = pg->AppendIn( pid, new wxPropertyCategory(A) ); \
    pg->SetPropertyCell( cat, 0, wxPG_LABEL, bmp ); \
    cat->GetCell(0).SetFont(italicFont); \
    cat->SetHelpString(italicFontHelp);

#define ADD_WX_LIB_CONF(A) pg->Append( new wxBoolProperty(wxT(#A),wxPG_LABEL,(bool)((A>0)?true:false)));
#define ADD_WX_LIB_CONF_NODEF(A) pg->Append( new wxBoolProperty(wxT(#A),wxPG_LABEL,(bool)false) ); \
                            pg->DisableProperty(wxT(#A));

    pid = pg->Append( new wxPropertyCategory( wxT("wxWidgets Library Configuration") ) );
    pg->SetPropertyCell( pid, 0, wxPG_LABEL, bmp );

    // Both of following lines would set a label for the second column
    pg->SetPropertyCell( pid, 1, "Is Enabled" );
    pid->SetValue("Is Enabled");

    ADD_WX_LIB_CONF_GROUP(wxT("Global Settings"))
    ADD_WX_LIB_CONF( wxUSE_GUI )

    ADD_WX_LIB_CONF_GROUP(wxT("Compatibility Settings"))
#if defined(WXWIN_COMPATIBILITY_2_8)
    ADD_WX_LIB_CONF( WXWIN_COMPATIBILITY_2_8 )
#endif
#if defined(WXWIN_COMPATIBILITY_3_0)
    ADD_WX_LIB_CONF( WXWIN_COMPATIBILITY_3_0 )
#endif
#ifdef wxFONT_SIZE_COMPATIBILITY
    ADD_WX_LIB_CONF( wxFONT_SIZE_COMPATIBILITY )
#else
    ADD_WX_LIB_CONF_NODEF ( wxFONT_SIZE_COMPATIBILITY )
#endif
#ifdef wxDIALOG_UNIT_COMPATIBILITY
    ADD_WX_LIB_CONF( wxDIALOG_UNIT_COMPATIBILITY )
#else
    ADD_WX_LIB_CONF_NODEF ( wxDIALOG_UNIT_COMPATIBILITY )
#endif

    ADD_WX_LIB_CONF_GROUP(wxT("Debugging Settings"))
    ADD_WX_LIB_CONF( wxUSE_DEBUG_CONTEXT )
    ADD_WX_LIB_CONF( wxUSE_MEMORY_TRACING )
    ADD_WX_LIB_CONF( wxUSE_GLOBAL_MEMORY_OPERATORS )
    ADD_WX_LIB_CONF( wxUSE_DEBUG_NEW_ALWAYS )
    ADD_WX_LIB_CONF( wxUSE_ON_FATAL_EXCEPTION )

    ADD_WX_LIB_CONF_GROUP(wxT("Unicode Support"))
    ADD_WX_LIB_CONF( wxUSE_UNICODE )

    ADD_WX_LIB_CONF_GROUP(wxT("Global Features"))
    ADD_WX_LIB_CONF( wxUSE_EXCEPTIONS )
    ADD_WX_LIB_CONF( wxUSE_EXTENDED_RTTI )
    ADD_WX_LIB_CONF( wxUSE_STL )
    ADD_WX_LIB_CONF( wxUSE_LOG )
    ADD_WX_LIB_CONF( wxUSE_LOGWINDOW )
    ADD_WX_LIB_CONF( wxUSE_LOGGUI )
    ADD_WX_LIB_CONF( wxUSE_LOG_DIALOG )
    ADD_WX_LIB_CONF( wxUSE_CMDLINE_PARSER )
    ADD_WX_LIB_CONF( wxUSE_THREADS )
    ADD_WX_LIB_CONF( wxUSE_STREAMS )
    ADD_WX_LIB_CONF( wxUSE_STD_IOSTREAM )

    ADD_WX_LIB_CONF_GROUP(wxT("Non-GUI Features"))
    ADD_WX_LIB_CONF( wxUSE_LONGLONG )
    ADD_WX_LIB_CONF( wxUSE_FILE )
    ADD_WX_LIB_CONF( wxUSE_FFILE )
    ADD_WX_LIB_CONF( wxUSE_FSVOLUME )
    ADD_WX_LIB_CONF( wxUSE_TEXTBUFFER )
    ADD_WX_LIB_CONF( wxUSE_TEXTFILE )
    ADD_WX_LIB_CONF( wxUSE_INTL )
    ADD_WX_LIB_CONF( wxUSE_DATETIME )
    ADD_WX_LIB_CONF( wxUSE_TIMER )
    ADD_WX_LIB_CONF( wxUSE_STOPWATCH )
    ADD_WX_LIB_CONF( wxUSE_CONFIG )
#ifdef wxUSE_CONFIG_NATIVE
    ADD_WX_LIB_CONF( wxUSE_CONFIG_NATIVE )
#else
    ADD_WX_LIB_CONF_NODEF ( wxUSE_CONFIG_NATIVE )
#endif
    ADD_WX_LIB_CONF( wxUSE_DIALUP_MANAGER )
    ADD_WX_LIB_CONF( wxUSE_DYNLIB_CLASS )
    ADD_WX_LIB_CONF( wxUSE_DYNAMIC_LOADER )
    ADD_WX_LIB_CONF( wxUSE_SOCKETS )
    ADD_WX_LIB_CONF( wxUSE_FILESYSTEM )
    ADD_WX_LIB_CONF( wxUSE_FS_ZIP )
    ADD_WX_LIB_CONF( wxUSE_FS_INET )
    ADD_WX_LIB_CONF( wxUSE_ZIPSTREAM )
    ADD_WX_LIB_CONF( wxUSE_ZLIB )
    ADD_WX_LIB_CONF( wxUSE_APPLE_IEEE )
    ADD_WX_LIB_CONF( wxUSE_JOYSTICK )
    ADD_WX_LIB_CONF( wxUSE_FONTMAP )
    ADD_WX_LIB_CONF( wxUSE_MIMETYPE )
    ADD_WX_LIB_CONF( wxUSE_PROTOCOL )
    ADD_WX_LIB_CONF( wxUSE_PROTOCOL_FILE )
    ADD_WX_LIB_CONF( wxUSE_PROTOCOL_FTP )
    ADD_WX_LIB_CONF( wxUSE_PROTOCOL_HTTP )
    ADD_WX_LIB_CONF( wxUSE_URL )
#ifdef wxUSE_URL_NATIVE
    ADD_WX_LIB_CONF( wxUSE_URL_NATIVE )
#else
    ADD_WX_LIB_CONF_NODEF ( wxUSE_URL_NATIVE )
#endif
    ADD_WX_LIB_CONF( wxUSE_REGEX )
    ADD_WX_LIB_CONF( wxUSE_SYSTEM_OPTIONS )
    ADD_WX_LIB_CONF( wxUSE_SOUND )
#ifdef wxUSE_XRC
    ADD_WX_LIB_CONF( wxUSE_XRC )
#else
    ADD_WX_LIB_CONF_NODEF ( wxUSE_XRC )
#endif
    ADD_WX_LIB_CONF( wxUSE_XML )

    // Set them to use check box.
    pg->SetPropertyAttribute(pid,wxPG_BOOL_USE_CHECKBOX,true,wxPG_RECURSE);
}


//
// Handle events of the third page here.
class wxMyPropertyGridPage : public wxPropertyGridPage
{
public:

    // Return false here to indicate unhandled events should be
    // propagated to manager's parent, as normal.
    virtual bool IsHandlingAllEvents() const wxOVERRIDE { return false; }

protected:

    virtual wxPGProperty* DoInsert( wxPGProperty* parent,
                                    int index,
                                    wxPGProperty* property ) wxOVERRIDE
    {
        return wxPropertyGridPage::DoInsert(parent,index,property);
    }

    void OnPropertySelect( wxPropertyGridEvent& event );
    void OnPropertyChanging( wxPropertyGridEvent& event );
    void OnPropertyChange( wxPropertyGridEvent& event );
    void OnPageChange( wxPropertyGridEvent& event );

private:
    wxDECLARE_EVENT_TABLE();
};


wxBEGIN_EVENT_TABLE(wxMyPropertyGridPage, wxPropertyGridPage)
    EVT_PG_SELECTED( wxID_ANY, wxMyPropertyGridPage::OnPropertySelect )
    EVT_PG_CHANGING( wxID_ANY, wxMyPropertyGridPage::OnPropertyChanging )
    EVT_PG_CHANGED( wxID_ANY, wxMyPropertyGridPage::OnPropertyChange )
    EVT_PG_PAGE_CHANGED( wxID_ANY, wxMyPropertyGridPage::OnPageChange )
wxEND_EVENT_TABLE()


void wxMyPropertyGridPage::OnPropertySelect( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    wxLogDebug(wxT("wxMyPropertyGridPage::OnPropertySelect('%s' is %s"),
               p->GetName().c_str(),
               IsPropertySelected(p)? wxT("selected"): wxT("unselected"));
}

void wxMyPropertyGridPage::OnPropertyChange( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    wxLogVerbose(wxT("wxMyPropertyGridPage::OnPropertyChange('%s', to value '%s')"),
               p->GetName().c_str(),
               p->GetDisplayedString().c_str());
}

void wxMyPropertyGridPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    wxLogVerbose(wxT("wxMyPropertyGridPage::OnPropertyChanging('%s', to value '%s')"),
               p->GetName().c_str(),
               event.GetValue().GetString().c_str());
}

void wxMyPropertyGridPage::OnPageChange( wxPropertyGridEvent& WXUNUSED(event) )
{
    wxLogDebug(wxT("wxMyPropertyGridPage::OnPageChange()"));
}


class wxPGKeyHandler : public wxEvtHandler
{
public:

    void OnKeyEvent( wxKeyEvent& event )
    {
        wxMessageBox(wxString::Format(wxT("%i"),event.GetKeyCode()));
        event.Skip();
    }
private:
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxPGKeyHandler,wxEvtHandler)
    EVT_KEY_DOWN( wxPGKeyHandler::OnKeyEvent )
wxEND_EVENT_TABLE()


// -----------------------------------------------------------------------

void FormMain::InitPanel()
{
    if ( m_panel )
        m_panel->Destroy();

    wxWindow* panel = new wxPanel(this, wxID_ANY,
                                  wxPoint(0, 0), wxSize(400, 400),
                                  wxTAB_TRAVERSAL);
    m_panel = panel;

    // Column
    wxBoxSizer* topSizer = new wxBoxSizer ( wxVERTICAL );

    m_topSizer = topSizer;
}

void FormMain::FinalizePanel( bool wasCreated )
{
    // Button for tab traversal testing
    m_topSizer->Add( new wxButton(m_panel, wxID_ANY,
                     wxS("Should be able to move here with Tab")),
                     0, wxEXPAND );
    m_topSizer->Add( new wxButton(m_panel, ID_SHOWPOPUP,
                     wxS("Show Popup")),
                     0, wxEXPAND );

    m_panel->SetSizer( m_topSizer );
    m_topSizer->SetSizeHints( m_panel );

    wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
    panelSizer->Add( m_panel, 1, wxEXPAND|wxFIXED_MINSIZE );

    SetSizer( panelSizer );
    panelSizer->SetSizeHints( this );

    if ( wasCreated )
        FinalizeFramePosition();
}

void FormMain::PopulateGrid()
{
    wxPropertyGridManager* pgman = m_pPropGridManager;
    pgman->AddPage(wxT("Standard Items"));

    PopulateWithStandardItems();

    pgman->AddPage(wxT("wxWidgets Library Config"));

    PopulateWithLibraryConfig();

    wxPropertyGridPage* myPage = new wxMyPropertyGridPage();
    myPage->Append( new wxIntProperty ( wxT("IntProperty"), wxPG_LABEL, 12345678 ) );

    // Use wxMyPropertyGridPage (see above) to test the
    // custom wxPropertyGridPage feature.
    pgman->AddPage(wxT("Examples"),wxNullBitmap,myPage);

    PopulateWithExamples();
}

void FormMain::CreateGrid( int style, int extraStyle )
{
    //
    // This function (re)creates the property grid in our sample
    //

    if ( style == -1 )
        style = // default style
                wxPG_BOLD_MODIFIED |
                wxPG_SPLITTER_AUTO_CENTER |
                wxPG_AUTO_SORT |
                //wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
                //wxPG_TOOLTIPS |
                //wxPG_HIDE_CATEGORIES |
                //wxPG_LIMITED_EDITING |
                wxPG_TOOLBAR |
                wxPG_DESCRIPTION;

    if ( extraStyle == -1 )
        // default extra style
        extraStyle = wxPG_EX_MODE_BUTTONS |
                     wxPG_EX_MULTIPLE_SELECTION;
                //| wxPG_EX_AUTO_UNSPECIFIED_VALUES
                //| wxPG_EX_GREY_LABEL_WHEN_DISABLED
                //| wxPG_EX_NATIVE_DOUBLE_BUFFERING
                //| wxPG_EX_HELP_AS_TOOLTIPS

    bool wasCreated = m_panel ? false : true;

    InitPanel();

    //
    // This shows how to combine two static choice descriptors
    m_combinedFlags.Add( _fs_windowstyle_labels, _fs_windowstyle_values );
    m_combinedFlags.Add( _fs_framestyle_labels, _fs_framestyle_values );

    wxPropertyGridManager* pgman = m_pPropGridManager =
        new wxPropertyGridManager(m_panel,
                                  // Don't change this into wxID_ANY in the sample, or the
                                  // event handling will obviously be broken.
                                  PGID, /*wxID_ANY*/
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  style );

    m_propGrid = pgman->GetGrid();

    pgman->SetExtraStyle(extraStyle);

    // This is the default validation failure behaviour
    m_pPropGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL |
                                                      wxPG_VFB_SHOW_MESSAGEBOX );

    m_pPropGridManager->GetGrid()->SetVerticalSpacing( 2 );

    //
    // Set somewhat different unspecified value appearance
    wxPGCell cell;
    cell.SetText("Unspecified");
    cell.SetFgCol(*wxLIGHT_GREY);
    m_propGrid->SetUnspecifiedValueAppearance(cell);

    PopulateGrid();

    // Change some attributes in all properties
    //pgman->SetPropertyAttributeAll(wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING,true);
    //pgman->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX,true);

    //m_pPropGridManager->SetSplitterLeft(true);
    //m_pPropGridManager->SetSplitterPosition(137);

    /*
    // This would setup event handling without event table entries
    Connect(m_pPropGridManager->GetId(), wxEVT_PG_SELECTED,
            wxPropertyGridEventHandler(FormMain::OnPropertyGridSelect) );
    Connect(m_pPropGridManager->GetId(), wxEVT_PG_CHANGED,
            wxPropertyGridEventHandler(FormMain::OnPropertyGridChange) );
    */

    m_topSizer->Add( m_pPropGridManager, 1, wxEXPAND );

    FinalizePanel(wasCreated);
}

// -----------------------------------------------------------------------

FormMain::FormMain(const wxString& title, const wxPoint& pos, const wxSize& size) :
           wxFrame((wxFrame *)NULL, -1, title, pos, size,
               (wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCAPTION|
                wxTAB_TRAVERSAL|wxCLOSE_BOX|wxNO_FULL_REPAINT_ON_RESIZE) )
{
    SetIcon(wxICON(sample));

    m_propGrid = NULL;
    m_panel = NULL;

#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = ID_ABOUT;
#endif

#if wxUSE_IMAGE
    // This is here to really test the wxImageFileProperty.
    wxInitAllImageHandlers();
#endif

    // Register all editors (SpinCtrl etc.)
    m_pPropGridManager->RegisterAdditionalEditors();

    // Register our sample custom editors
    m_pSampleMultiButtonEditor =
        wxPropertyGrid::RegisterEditorClass(new wxSampleMultiButtonEditor());

    CreateGrid( // style
                wxPG_BOLD_MODIFIED |
                wxPG_SPLITTER_AUTO_CENTER |
                wxPG_AUTO_SORT |
                //wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
                //wxPG_TOOLTIPS |
                //wxPG_HIDE_CATEGORIES |
                //wxPG_LIMITED_EDITING |
                wxPG_TOOLBAR |
                wxPG_DESCRIPTION,
                // extra style
                wxPG_EX_MODE_BUTTONS |
                wxPG_EX_MULTIPLE_SELECTION
                //| wxPG_EX_AUTO_UNSPECIFIED_VALUES
                //| wxPG_EX_GREY_LABEL_WHEN_DISABLED
                //| wxPG_EX_NATIVE_DOUBLE_BUFFERING
                //| wxPG_EX_HELP_AS_TOOLTIPS
              );

    //
    // Create menubar
    wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    wxMenu *menuTry = new wxMenu;
    wxMenu *menuTools1 = new wxMenu;
    wxMenu *menuTools2 = new wxMenu;
    wxMenu *menuHelp = new wxMenu;

    menuHelp->Append(ID_ABOUT, wxT("&About"), wxT("Show about dialog") );

    menuTools1->Append(ID_APPENDPROP, wxT("Append New Property") );
    menuTools1->Append(ID_APPENDCAT, wxT("Append New Category\tCtrl-S") );
    menuTools1->AppendSeparator();
    menuTools1->Append(ID_INSERTPROP, wxT("Insert New Property\tCtrl-Q") );
    menuTools1->Append(ID_INSERTCAT, wxT("Insert New Category\tCtrl-W") );
    menuTools1->AppendSeparator();
    menuTools1->Append(ID_DELETE, wxT("Delete Selected") );
    menuTools1->Append(ID_DELETER, wxT("Delete Random") );
    menuTools1->Append(ID_DELETEALL, wxT("Delete All") );
    menuTools1->AppendSeparator();
    menuTools1->Append(ID_SETBGCOLOUR, wxT("Set Bg Colour") );
    menuTools1->Append(ID_SETBGCOLOURRECUR, wxT("Set Bg Colour (Recursively)") );
    menuTools1->Append(ID_UNSPECIFY, "Set Value to Unspecified");
    menuTools1->AppendSeparator();
    m_itemEnable = menuTools1->Append(ID_ENABLE, wxT("Enable"),
        wxT("Toggles item's enabled state.") );
    m_itemEnable->Enable( false );
    menuTools1->Append(ID_HIDE, "Hide", "Hides a property" );
    menuTools1->Append(ID_SETREADONLY, "Set as Read-Only",
                       "Set property as read-only" );

    menuTools2->Append(ID_ITERATE1, wxT("Iterate Over Properties") );
    menuTools2->Append(ID_ITERATE2, wxT("Iterate Over Visible Items") );
    menuTools2->Append(ID_ITERATE3, wxT("Reverse Iterate Over Properties") );
    menuTools2->Append(ID_ITERATE4, wxT("Iterate Over Categories") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_ONEXTENDEDKEYNAV, "Extend Keyboard Navigation",
                       "This will set Enter to navigate to next property, "
                       "and allows arrow keys to navigate even when in "
                       "editor control.");
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_SETPROPERTYVALUE, wxT("Set Property Value") );
    menuTools2->Append(ID_CLEARMODIF, wxT("Clear Modified Status"), wxT("Clears wxPG_MODIFIED flag from all properties.") );
    menuTools2->AppendSeparator();
    m_itemFreeze = menuTools2->AppendCheckItem(ID_FREEZE, wxT("Freeze"),
        wxT("Disables painting, auto-sorting, etc.") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_DUMPLIST, wxT("Display Values as wxVariant List"), wxT("Tests GetAllValues method and wxVariant conversion.") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_GETVALUES, wxT("Get Property Values"), wxT("Stores all property values.") );
    menuTools2->Append(ID_SETVALUES, wxT("Set Property Values"), wxT("Reverts property values to those last stored.") );
    menuTools2->Append(ID_SETVALUES2, wxT("Set Property Values 2"), wxT("Adds property values that should not initially be as items (so new items are created).") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_SAVESTATE, wxT("Save Editable State") );
    menuTools2->Append(ID_RESTORESTATE, wxT("Restore Editable State") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_ENABLECOMMONVALUES, wxT("Enable Common Value"),
        wxT("Enable values that are common to all properties, for selected property."));
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_COLLAPSE, wxT("Collapse Selected") );
    menuTools2->Append(ID_COLLAPSEALL, wxT("Collapse All") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_INSERTPAGE, wxT("Add Page") );
    menuTools2->Append(ID_REMOVEPAGE, wxT("Remove Page") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_FITCOLUMNS, wxT("Fit Columns") );
    m_itemVetoDragging =
        menuTools2->AppendCheckItem(ID_VETOCOLDRAG,
                                    "Veto Column Dragging");
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_CHANGEFLAGSITEMS, wxT("Change Children of FlagsProp") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_TESTINSERTCHOICE, wxT("Test InsertPropertyChoice") );
    menuTools2->Append(ID_TESTDELETECHOICE, wxT("Test DeletePropertyChoice") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_SETSPINCTRLEDITOR, wxT("Use SpinCtrl Editor") );
    menuTools2->Append(ID_TESTREPLACE, wxT("Test ReplaceProperty") );

    menuTry->Append(ID_SELECTSTYLE, wxT("Set Window Style"),
        wxT("Select window style flags used by the grid."));
    menuTry->Append(ID_ENABLELABELEDITING, "Enable label editing",
        "This calls wxPropertyGrid::MakeColumnEditable(0)");
    menuTry->AppendCheckItem(ID_SHOWHEADER,
        "Enable header",
        "This calls wxPropertyGridManager::ShowHeader()");
    menuTry->AppendSeparator();
    menuTry->AppendRadioItem( ID_COLOURSCHEME1, wxT("Standard Colour Scheme") );
    menuTry->AppendRadioItem( ID_COLOURSCHEME2, wxT("White Colour Scheme") );
    menuTry->AppendRadioItem( ID_COLOURSCHEME3, wxT(".NET Colour Scheme") );
    menuTry->AppendRadioItem( ID_COLOURSCHEME4, wxT("Cream Colour Scheme") );
    menuTry->AppendSeparator();
    m_itemCatColours = menuTry->AppendCheckItem(ID_CATCOLOURS, wxT("Category Specific Colours"),
        wxT("Switches between category-specific cell colours and default scheme (actually done using SetPropertyTextColour and SetPropertyBackgroundColour).") );
    menuTry->AppendSeparator();
    menuTry->AppendCheckItem(ID_STATICLAYOUT, wxT("Static Layout"),
        wxT("Switches between user-modifiable and static layouts.") );
    menuTry->Append(ID_SETCOLUMNS, wxT("Set Number of Columns") );
    menuTry->AppendSeparator();
    menuTry->Append(ID_TESTXRC, wxT("Display XRC sample") );
    menuTry->AppendSeparator();
    menuTry->Append(ID_RUNTESTFULL, wxT("Run Tests (full)") );
    menuTry->Append(ID_RUNTESTPARTIAL, wxT("Run Tests (fast)") );

    menuFile->Append(ID_RUNMINIMAL, wxT("Run Minimal Sample") );
    menuFile->AppendSeparator();
    menuFile->Append(ID_QUIT, wxT("E&xit\tAlt-X"), wxT("Quit this program") );

    // Now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, wxT("&File") );
    menuBar->Append(menuTry, wxT("&Try These!") );
    menuBar->Append(menuTools1, wxT("&Basic") );
    menuBar->Append(menuTools2, wxT("&Advanced") );
    menuBar->Append(menuHelp, wxT("&Help") );

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar
    CreateStatusBar(1);
    SetStatusText(wxEmptyString);
#endif // wxUSE_STATUSBAR

    FinalizeFramePosition();

#if wxUSE_LOGWINDOW
    // Create log window
    m_logWindow = new wxLogWindow(this, "Log Messages", false);
    m_logWindow->GetFrame()->Move(GetPosition().x + GetSize().x + 10,
                                  GetPosition().y);
    m_logWindow->Show();
#endif
}

void FormMain::FinalizeFramePosition()
{
    wxSize frameSize((wxSystemSettings::GetMetric(wxSYS_SCREEN_X)/10)*4,
                     (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)/10)*8);

    if ( frameSize.x > 500 )
        frameSize.x = 500;

    SetSize(frameSize);

    Centre();
}

//
// Normally, wxPropertyGrid does not check whether item with identical
// label already exists. However, since in this sample we use labels for
// identifying properties, we have to be sure not to generate identical
// labels.
//
void GenerateUniquePropertyLabel( wxPropertyGridManager* pg, wxString& baselabel )
{
    int count = -1;
    wxString newlabel;

    if ( pg->GetPropertyByLabel( baselabel ) )
    {
        for (;;)
        {
            count++;
            newlabel.Printf(wxT("%s%i"),baselabel.c_str(),count);
            if ( !pg->GetPropertyByLabel( newlabel ) ) break;
        }
    }

    if ( count >= 0 )
    {
        baselabel = newlabel;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnInsertPropClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString propLabel;

    if ( !m_pPropGridManager->GetGrid()->GetRoot()->GetChildCount() )
    {
        wxMessageBox(wxT("No items to relate - first add some with Append."));
        return;
    }

    wxPGProperty* id = m_pPropGridManager->GetGrid()->GetSelection();
    if ( !id )
    {
        wxMessageBox(wxT("First select a property - new one will be inserted right before that."));
        return;
    }
    if ( propLabel.Len() < 1 ) propLabel = wxT("Property");

    GenerateUniquePropertyLabel( m_pPropGridManager, propLabel );

    m_pPropGridManager->Insert( m_pPropGridManager->GetPropertyParent(id),
                                id->GetIndexInParent(),
                                new wxStringProperty(propLabel) );

}

// -----------------------------------------------------------------------

void FormMain::OnAppendPropClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString propLabel;

    if ( propLabel.Len() < 1 ) propLabel = wxT("Property");

    GenerateUniquePropertyLabel( m_pPropGridManager, propLabel );

    m_pPropGridManager->Append( new wxStringProperty(propLabel) );

    m_pPropGridManager->Refresh();
}

// -----------------------------------------------------------------------

void FormMain::OnClearClick( wxCommandEvent& WXUNUSED(event) )
{
    m_pPropGridManager->GetGrid()->Clear();
}

// -----------------------------------------------------------------------

void FormMain::OnAppendCatClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString propLabel;

    if ( propLabel.Len() < 1 ) propLabel = wxT("Category");

    GenerateUniquePropertyLabel( m_pPropGridManager, propLabel );

    m_pPropGridManager->Append( new wxPropertyCategory (propLabel) );

    m_pPropGridManager->Refresh();

}

// -----------------------------------------------------------------------

void FormMain::OnInsertCatClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString propLabel;

    if ( !m_pPropGridManager->GetGrid()->GetRoot()->GetChildCount() )
    {
        wxMessageBox(wxT("No items to relate - first add some with Append."));
        return;
    }

    wxPGProperty* id = m_pPropGridManager->GetGrid()->GetSelection();
    if ( !id )
    {
        wxMessageBox(wxT("First select a property - new one will be inserted right before that."));
        return;
    }

    if ( propLabel.Len() < 1 ) propLabel = wxT("Category");

    GenerateUniquePropertyLabel( m_pPropGridManager, propLabel );

    m_pPropGridManager->Insert( m_pPropGridManager->GetPropertyParent(id),
                                id->GetIndexInParent(),
                                new wxPropertyCategory (propLabel) );
}

// -----------------------------------------------------------------------

void FormMain::OnDelPropClick( wxCommandEvent& WXUNUSED(event) )
{
    wxPGProperty* id = m_pPropGridManager->GetGrid()->GetSelection();
    if ( !id )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }

    m_pPropGridManager->DeleteProperty( id );
}

// -----------------------------------------------------------------------

void FormMain::OnDelPropRClick( wxCommandEvent& WXUNUSED(event) )
{
    // Delete random property
    wxPGProperty* p = m_pPropGridManager->GetGrid()->GetRoot();

    for (;;)
    {
        if ( !p->IsCategory() )
        {
            m_pPropGridManager->DeleteProperty( p );
            break;
        }

        if ( !p->GetChildCount() )
            break;

        int n = rand() % ((int)p->GetChildCount());

        p = p->Item(n);
    }
}

// -----------------------------------------------------------------------

void FormMain::OnContextMenu( wxContextMenuEvent& event )
{
    wxLogDebug(wxT("FormMain::OnContextMenu(%i,%i)"),
        event.GetPosition().x,event.GetPosition().y);

    wxUnusedVar(event);

    //event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnCloseClick( wxCommandEvent& WXUNUSED(event) )
{
/*#ifdef __WXDEBUG__
    m_pPropGridManager->GetGrid()->DumpAllocatedChoiceSets();
    wxLogDebug(wxT("\\-> Don't worry, this is perfectly normal in this sample."));
#endif*/

    Close(false);
}

// -----------------------------------------------------------------------

int IterateMessage( wxPGProperty* prop )
{
    wxString s;

    s.Printf( wxT("\"%s\" class = %s, valuetype = %s"), prop->GetLabel().c_str(),
        prop->GetClassInfo()->GetClassName(), prop->GetValueType().c_str() );

    return wxMessageBox( s, wxT("Iterating... (press CANCEL to end)"), wxOK|wxCANCEL );
}

// -----------------------------------------------------------------------

void FormMain::OnIterate1Click( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGridIterator it;

    for ( it = m_pPropGridManager->GetCurrentPage()->
            GetIterator();
          !it.AtEnd();
          it++ )
    {
        wxPGProperty* p = *it;
        int res = IterateMessage( p );
        if ( res == wxCANCEL ) break;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnIterate2Click( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGridIterator it;

    for ( it = m_pPropGridManager->GetCurrentPage()->
            GetIterator( wxPG_ITERATE_VISIBLE );
          !it.AtEnd();
          it++ )
    {
        wxPGProperty* p = *it;

        int res = IterateMessage( p );
        if ( res == wxCANCEL ) break;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnIterate3Click( wxCommandEvent& WXUNUSED(event) )
{
    // iterate over items in reverse order
    wxPropertyGridIterator it;

    for ( it = m_pPropGridManager->GetCurrentPage()->
                GetIterator( wxPG_ITERATE_DEFAULT, wxBOTTOM );
          !it.AtEnd();
          it-- )
    {
        wxPGProperty* p = *it;

        int res = IterateMessage( p );
        if ( res == wxCANCEL ) break;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnIterate4Click( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGridIterator it;

    for ( it = m_pPropGridManager->GetCurrentPage()->
            GetIterator( wxPG_ITERATE_CATEGORIES );
          !it.AtEnd();
          it++ )
    {
        wxPGProperty* p = *it;

        int res = IterateMessage( p );
        if ( res == wxCANCEL ) break;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnExtendedKeyNav( wxCommandEvent& WXUNUSED(event) )
{
    // Use AddActionTrigger() and DedicateKey() to set up Enter,
    // Up, and Down keys for navigating between properties.
    wxPropertyGrid* propGrid = m_pPropGridManager->GetGrid();

    propGrid->AddActionTrigger(wxPG_ACTION_NEXT_PROPERTY,
                               WXK_RETURN);
    propGrid->DedicateKey(WXK_RETURN);

    // Up and Down keys are already associated with navigation,
    // but we must also prevent them from being eaten by
    // editor controls.
    propGrid->DedicateKey(WXK_UP);
    propGrid->DedicateKey(WXK_DOWN);
}

// -----------------------------------------------------------------------

void FormMain::OnFitColumnsClick( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGridPage* page = m_pPropGridManager->GetCurrentPage();

    // Remove auto-centering
    m_pPropGridManager->SetWindowStyle( m_pPropGridManager->GetWindowStyle() & ~wxPG_SPLITTER_AUTO_CENTER);

    // Grow manager size just prior fit - otherwise
    // column information may be lost.
    wxSize oldGridSize = m_pPropGridManager->GetGrid()->GetClientSize();
    wxSize oldFullSize = GetSize();
    SetSize(1000, oldFullSize.y);

    wxSize newSz = page->FitColumns();

    int dx = oldFullSize.x - oldGridSize.x;
    int dy = oldFullSize.y - oldGridSize.y;

    newSz.x += dx;
    newSz.y += dy;

    SetSize(newSz);
}

// -----------------------------------------------------------------------

void FormMain::OnChangeFlagsPropItemsClick( wxCommandEvent& WXUNUSED(event) )
{
    wxPGProperty* p = m_pPropGridManager->GetPropertyByName(wxT("Window Styles"));

    wxPGChoices newChoices;

    newChoices.Add(wxT("Fast"),0x1);
    newChoices.Add(wxT("Powerful"),0x2);
    newChoices.Add(wxT("Safe"),0x4);
    newChoices.Add(wxT("Sleek"),0x8);

    p->SetChoices(newChoices);
}

// -----------------------------------------------------------------------

void FormMain::OnEnableDisable( wxCommandEvent& )
{
    wxPGProperty* id = m_pPropGridManager->GetGrid()->GetSelection();
    if ( !id )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }

    if ( m_pPropGridManager->IsPropertyEnabled( id ) )
    {
        m_pPropGridManager->DisableProperty ( id );
        m_itemEnable->SetItemLabel( wxT("Enable") );
    }
    else
    {
        m_pPropGridManager->EnableProperty ( id );
        m_itemEnable->SetItemLabel( wxT("Disable") );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnSetReadOnly( wxCommandEvent& WXUNUSED(event) )
{
    wxPGProperty* p = m_pPropGridManager->GetGrid()->GetSelection();
    if ( !p )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }
    m_pPropGridManager->SetPropertyReadOnly(p);
}

// -----------------------------------------------------------------------

void FormMain::OnHide( wxCommandEvent& WXUNUSED(event) )
{
    wxPGProperty* id = m_pPropGridManager->GetGrid()->GetSelection();
    if ( !id )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }

    m_pPropGridManager->HideProperty( id, true );
}

// -----------------------------------------------------------------------

#include "wx/colordlg.h"

void
FormMain::OnSetBackgroundColour( wxCommandEvent& event )
{
    wxPropertyGrid* pg = m_pPropGridManager->GetGrid();
    wxPGProperty* prop = pg->GetSelection();
    if ( !prop )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }

    wxColour col = ::wxGetColourFromUser(this, *wxWHITE, "Choose colour");

    if ( col.IsOk() )
    {
        bool recursively = (event.GetId()==ID_SETBGCOLOURRECUR) ? true : false;
        pg->SetPropertyBackgroundColour(prop, col, recursively);
    }
}

// -----------------------------------------------------------------------

void FormMain::OnInsertPage( wxCommandEvent& WXUNUSED(event) )
{
    m_pPropGridManager->AddPage(wxT("New Page"));
}

// -----------------------------------------------------------------------

void FormMain::OnRemovePage( wxCommandEvent& WXUNUSED(event) )
{
    m_pPropGridManager->RemovePage(m_pPropGridManager->GetSelectedPage());
}

// -----------------------------------------------------------------------

void FormMain::OnSaveState( wxCommandEvent& WXUNUSED(event) )
{
    m_savedState = m_pPropGridManager->SaveEditableState();
    wxLogDebug(wxT("Saved editable state string: \"%s\""), m_savedState.c_str());
}

// -----------------------------------------------------------------------

void FormMain::OnRestoreState( wxCommandEvent& WXUNUSED(event) )
{
    m_pPropGridManager->RestoreEditableState(m_savedState);
}

// -----------------------------------------------------------------------

void FormMain::OnSetSpinCtrlEditorClick( wxCommandEvent& WXUNUSED(event) )
{
#if wxUSE_SPINBTN
    wxPGProperty* pgId = m_pPropGridManager->GetSelection();
    if ( pgId )
        m_pPropGridManager->SetPropertyEditor( pgId, wxPGEditor_SpinCtrl );
    else
        wxMessageBox(wxT("First select a property"));
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnTestReplaceClick( wxCommandEvent& WXUNUSED(event) )
{
    wxPGProperty* pgId = m_pPropGridManager->GetSelection();
    if ( pgId )
    {
        wxPGChoices choices;
        choices.Add(wxT("Flag 0"),0x0001);
        choices.Add(wxT("Flag 1"),0x0002);
        choices.Add(wxT("Flag 2"),0x0004);
        choices.Add(wxT("Flag 3"),0x0008);
        wxPGProperty* newId = m_pPropGridManager->ReplaceProperty( pgId,
            new wxFlagsProperty(wxT("ReplaceFlagsProperty"), wxPG_LABEL, choices, 0x0003) );
        m_pPropGridManager->SetPropertyAttribute( newId,
                                              wxPG_BOOL_USE_CHECKBOX,
                                              true,
                                              wxPG_RECURSE );
    }
    else
        wxMessageBox(wxT("First select a property"));
}

// -----------------------------------------------------------------------

void FormMain::OnClearModifyStatusClick( wxCommandEvent& WXUNUSED(event) )
{
    m_pPropGridManager->ClearModifiedStatus();
}

// -----------------------------------------------------------------------

// Freeze check-box checked?
void FormMain::OnFreezeClick( wxCommandEvent& event )
{
    if ( !m_pPropGridManager ) return;

    if ( event.IsChecked() )
    {
        if ( !m_pPropGridManager->IsFrozen() )
        {
            m_pPropGridManager->Freeze();
        }
    }
    else
    {
        if ( m_pPropGridManager->IsFrozen() )
        {
            m_pPropGridManager->Thaw();
            m_pPropGridManager->Refresh();
        }
    }
}

// -----------------------------------------------------------------------

void FormMain::OnEnableLabelEditing( wxCommandEvent& WXUNUSED(event) )
{
    m_propGrid->MakeColumnEditable(0);
}

// -----------------------------------------------------------------------

void FormMain::OnShowHeader( wxCommandEvent& event )
{
    m_pPropGridManager->ShowHeader(event.IsChecked());
    m_pPropGridManager->SetColumnTitle(2, _("Units"));
}

// -----------------------------------------------------------------------

void FormMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( wxT("wxPropertyGrid Sample")
#if wxUSE_UNICODE
  #if defined(wxUSE_UNICODE_UTF8) && wxUSE_UNICODE_UTF8
                wxT(" <utf-8>")
  #else
                wxT(" <unicode>")
  #endif
#else
                wxT(" <ansi>")
#endif
#ifdef __WXDEBUG__
                wxT(" <debug>")
#else
                wxT(" <release>")
#endif
                wxT("\n\n")
                wxT("Programmed by %s\n\n")
                wxT("Using %s\n\n"),
            wxT("Jaakko Salli"), wxVERSION_STRING
            );

    wxMessageBox(msg, wxT("About"), wxOK | wxICON_INFORMATION, this);
}

// -----------------------------------------------------------------------

void FormMain::OnColourScheme( wxCommandEvent& event )
{
    int id = event.GetId();
    if ( id == ID_COLOURSCHEME1 )
    {
        m_pPropGridManager->GetGrid()->ResetColours();
    }
    else if ( id == ID_COLOURSCHEME2 )
    {
        // white
        wxColour my_grey_1(212,208,200);
        wxColour my_grey_3(113,111,100);
        m_pPropGridManager->Freeze();
        m_pPropGridManager->GetGrid()->SetMarginColour( *wxWHITE );
        m_pPropGridManager->GetGrid()->SetCaptionBackgroundColour( *wxWHITE );
        m_pPropGridManager->GetGrid()->SetCellBackgroundColour( *wxWHITE );
        m_pPropGridManager->GetGrid()->SetCellTextColour( my_grey_3 );
        m_pPropGridManager->GetGrid()->SetLineColour( my_grey_1 ); //wxColour(160,160,160)
        m_pPropGridManager->Thaw();
    }
    else if ( id == ID_COLOURSCHEME3 )
    {
        // .NET
        wxColour my_grey_1(212,208,200);
        wxColour my_grey_2(236,233,216);
        m_pPropGridManager->Freeze();
        m_pPropGridManager->GetGrid()->SetMarginColour( my_grey_1 );
        m_pPropGridManager->GetGrid()->SetCaptionBackgroundColour( my_grey_1 );
        m_pPropGridManager->GetGrid()->SetLineColour( my_grey_1 );
        m_pPropGridManager->Thaw();
    }
    else if ( id == ID_COLOURSCHEME4 )
    {
        // cream

        wxColour my_grey_1(212,208,200);
        wxColour my_grey_2(241,239,226);
        wxColour my_grey_3(113,111,100);
        m_pPropGridManager->Freeze();
        m_pPropGridManager->GetGrid()->SetMarginColour( *wxWHITE );
        m_pPropGridManager->GetGrid()->SetCaptionBackgroundColour( *wxWHITE );
        m_pPropGridManager->GetGrid()->SetCellBackgroundColour( my_grey_2 );
        m_pPropGridManager->GetGrid()->SetCellBackgroundColour( my_grey_2 );
        m_pPropGridManager->GetGrid()->SetCellTextColour( my_grey_3 );
        m_pPropGridManager->GetGrid()->SetLineColour( my_grey_1 );
        m_pPropGridManager->Thaw();
    }
}

// -----------------------------------------------------------------------

void FormMain::OnCatColours( wxCommandEvent& event )
{
    wxPropertyGrid* pg = m_pPropGridManager->GetGrid();
    m_pPropGridManager->Freeze();

    if ( event.IsChecked() )
    {
        // Set custom colours.
        pg->SetPropertyTextColour( wxT("Appearance"), wxColour(255,0,0), false );
        pg->SetPropertyBackgroundColour( wxT("Appearance"), wxColour(255,255,183) );
        pg->SetPropertyTextColour( wxT("Appearance"), wxColour(255,0,183) );
        pg->SetPropertyTextColour( wxT("PositionCategory"), wxColour(0,255,0), false );
        pg->SetPropertyBackgroundColour( wxT("PositionCategory"), wxColour(255,226,190) );
        pg->SetPropertyTextColour( wxT("PositionCategory"), wxColour(255,0,190) );
        pg->SetPropertyTextColour( wxT("Environment"), wxColour(0,0,255), false );
        pg->SetPropertyBackgroundColour( wxT("Environment"), wxColour(208,240,175) );
        pg->SetPropertyTextColour( wxT("Environment"), wxColour(255,255,255) );
        pg->SetPropertyBackgroundColour( wxT("More Examples"), wxColour(172,237,255) );
        pg->SetPropertyTextColour( wxT("More Examples"), wxColour(172,0,255) );
    }
    else
    {
        // Revert to original.
        pg->SetPropertyColoursToDefault( wxT("Appearance") );
        pg->SetPropertyColoursToDefault( wxT("PositionCategory") );
        pg->SetPropertyColoursToDefault( wxT("Environment") );
        pg->SetPropertyColoursToDefault( wxT("More Examples") );
    }
    m_pPropGridManager->Thaw();
    m_pPropGridManager->Refresh();
}

// -----------------------------------------------------------------------

#define ADD_FLAG(FLAG) \
        chs.Add(wxT(#FLAG)); \
        vls.Add(FLAG); \
        if ( (flags & FLAG) == FLAG ) sel.Add(ind); \
        ind++;

void FormMain::OnSelectStyle( wxCommandEvent& WXUNUSED(event) )
{
    int style = 0;
    int extraStyle = 0;

    {
        wxArrayString chs;
        wxArrayInt vls;
        wxArrayInt sel;
        unsigned int ind = 0;
        int flags = m_pPropGridManager->GetWindowStyle();
        ADD_FLAG(wxPG_HIDE_CATEGORIES)
        ADD_FLAG(wxPG_AUTO_SORT)
        ADD_FLAG(wxPG_BOLD_MODIFIED)
        ADD_FLAG(wxPG_SPLITTER_AUTO_CENTER)
        ADD_FLAG(wxPG_TOOLTIPS)
        ADD_FLAG(wxPG_STATIC_SPLITTER)
        ADD_FLAG(wxPG_HIDE_MARGIN)
        ADD_FLAG(wxPG_LIMITED_EDITING)
        ADD_FLAG(wxPG_TOOLBAR)
        ADD_FLAG(wxPG_DESCRIPTION)
        ADD_FLAG(wxPG_NO_INTERNAL_BORDER)
        wxMultiChoiceDialog dlg( this, wxT("Select window styles to use"),
                                 wxT("wxPropertyGrid Window Style"), chs );
        dlg.SetSelections(sel);
        if ( dlg.ShowModal() == wxID_CANCEL )
            return;

        flags = 0;
        sel = dlg.GetSelections();
        for ( ind = 0; ind < sel.size(); ind++ )
            flags |= vls[sel[ind]];

        style = flags;
    }

    {
        wxArrayString chs;
        wxArrayInt vls;
        wxArrayInt sel;
        unsigned int ind = 0;
        int flags = m_pPropGridManager->GetExtraStyle();
        ADD_FLAG(wxPG_EX_INIT_NOCAT)
        ADD_FLAG(wxPG_EX_NO_FLAT_TOOLBAR)
        ADD_FLAG(wxPG_EX_MODE_BUTTONS)
        ADD_FLAG(wxPG_EX_HELP_AS_TOOLTIPS)
        ADD_FLAG(wxPG_EX_NATIVE_DOUBLE_BUFFERING)
        ADD_FLAG(wxPG_EX_AUTO_UNSPECIFIED_VALUES)
        ADD_FLAG(wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES)
        ADD_FLAG(wxPG_EX_HIDE_PAGE_BUTTONS)
        ADD_FLAG(wxPG_EX_MULTIPLE_SELECTION)
        ADD_FLAG(wxPG_EX_ENABLE_TLP_TRACKING)
        ADD_FLAG(wxPG_EX_NO_TOOLBAR_DIVIDER)
        ADD_FLAG(wxPG_EX_TOOLBAR_SEPARATOR)
        wxMultiChoiceDialog dlg( this, wxT("Select extra window styles to use"),
                                 wxT("wxPropertyGrid Extra Style"), chs );
        dlg.SetSelections(sel);
        if ( dlg.ShowModal() == wxID_CANCEL )
            return;

        flags = 0;
        sel = dlg.GetSelections();
        for ( ind = 0; ind < sel.size(); ind++ )
            flags |= vls[sel[ind]];

        extraStyle = flags;
    }

    CreateGrid( style, extraStyle );

    FinalizeFramePosition();
}

// -----------------------------------------------------------------------

void FormMain::OnSetColumns( wxCommandEvent& WXUNUSED(event) )
{
    long colCount = ::wxGetNumberFromUser(wxT("Enter number of columns (2-20)."),wxT("Columns:"),
                                          wxT("Change Columns"),m_pPropGridManager->GetColumnCount(),
                                          2,20);

    if ( colCount >= 2 )
    {
        m_pPropGridManager->SetColumnCount(colCount);
    }
}

// -----------------------------------------------------------------------

void FormMain::OnSetPropertyValue( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridManager->GetGrid();
    wxPGProperty* selected = pg->GetSelection();

    if ( selected )
    {
        wxString value = ::wxGetTextFromUser( wxT("Enter new value:") );
        pg->SetPropertyValue( selected, value );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnInsertChoice( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridManager->GetGrid();

    wxPGProperty* selected = pg->GetSelection();
    const wxPGChoices& choices = selected->GetChoices();

    // Insert new choice to the center of list

    if ( choices.IsOk() )
    {
        int pos = choices.GetCount() / 2;
        selected->InsertChoice(wxT("New Choice"), pos);
    }
    else
    {
        ::wxMessageBox(wxT("First select a property with some choices."));
    }
}

// -----------------------------------------------------------------------

void FormMain::OnDeleteChoice( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridManager->GetGrid();

    wxPGProperty* selected = pg->GetSelection();
    const wxPGChoices& choices = selected->GetChoices();

    // Deletes choice from the center of list

    if ( choices.IsOk() )
    {
        int pos = choices.GetCount() / 2;
        selected->DeleteChoice(pos);
    }
    else
    {
        ::wxMessageBox(wxT("First select a property with some choices."));
    }
}

// -----------------------------------------------------------------------

#include <wx/colordlg.h>

void FormMain::OnMisc ( wxCommandEvent& event )
{
    int id = event.GetId();
    if ( id == ID_STATICLAYOUT )
    {
        long wsf = m_pPropGridManager->GetWindowStyleFlag();
        if ( event.IsChecked() ) m_pPropGridManager->SetWindowStyleFlag( wsf|wxPG_STATIC_LAYOUT );
        else m_pPropGridManager->SetWindowStyleFlag( wsf&~(wxPG_STATIC_LAYOUT) );
    }
    else if ( id == ID_COLLAPSEALL )
    {
        wxPGVIterator it;
        wxPropertyGrid* pg = m_pPropGridManager->GetGrid();

        for ( it = pg->GetVIterator( wxPG_ITERATE_ALL ); !it.AtEnd(); it.Next() )
            it.GetProperty()->SetExpanded( false );

        pg->RefreshGrid();
    }
    else if ( id == ID_GETVALUES )
    {
        m_storedValues = m_pPropGridManager->GetGrid()->GetPropertyValues(wxT("Test"),
                                                                      m_pPropGridManager->GetGrid()->GetRoot(),
                                                                      wxPG_KEEP_STRUCTURE|wxPG_INC_ATTRIBUTES);
    }
    else if ( id == ID_SETVALUES )
    {
        if ( m_storedValues.GetType() == wxT("list") )
        {
            m_pPropGridManager->GetGrid()->SetPropertyValues(m_storedValues);
        }
        else
            wxMessageBox(wxT("First use Get Property Values."));
    }
    else if ( id == ID_SETVALUES2 )
    {
        wxVariant list;
        list.NullList();
        list.Append( wxVariant((long)1234,wxT("VariantLong")) );
        list.Append( wxVariant(true,wxT("VariantBool")) );
        list.Append( wxVariant(wxT("Test Text"),wxT("VariantString")) );
        m_pPropGridManager->GetGrid()->SetPropertyValues(list);
    }
    else if ( id == ID_COLLAPSE )
    {
        // Collapses selected.
        wxPGProperty* id = m_pPropGridManager->GetSelection();
        if ( id )
        {
            m_pPropGridManager->Collapse(id);
        }
    }
    else if ( id == ID_RUNTESTFULL )
    {
        // Runs a regression test.
        RunTests(true);
    }
    else if ( id == ID_RUNTESTPARTIAL )
    {
        // Runs a regression test.
        RunTests(false);
    }
    else if ( id == ID_UNSPECIFY )
    {
        wxPGProperty* prop = m_pPropGridManager->GetSelection();
        if ( prop )
        {
            m_pPropGridManager->SetPropertyValueUnspecified(prop);
            prop->RefreshEditor();
        }
    }
}

// -----------------------------------------------------------------------

void FormMain::OnPopulateClick( wxCommandEvent& event )
{
    int id = event.GetId();
    m_propGrid->Clear();
    m_propGrid->Freeze();
    if ( id == ID_POPULATE1 )
    {
        PopulateWithStandardItems();
    }
    else if ( id == ID_POPULATE2 )
    {
        PopulateWithLibraryConfig();
    }
    m_propGrid->Thaw();
}

// -----------------------------------------------------------------------

void DisplayMinimalFrame(wxWindow* parent);  // in minimal.cpp

void FormMain::OnRunMinimalClick( wxCommandEvent& WXUNUSED(event) )
{
    DisplayMinimalFrame(this);
}

// -----------------------------------------------------------------------

FormMain::~FormMain()
{
}

// -----------------------------------------------------------------------

IMPLEMENT_APP(cxApplication)

bool cxApplication::OnInit()
{
    //wxLocale Locale;
    //Locale.Init(wxLANGUAGE_FINNISH);

    FormMain* frame = Form1 = new FormMain( wxT("wxPropertyGrid Sample"), wxPoint(0,0), wxSize(300,500) );
    frame->Show(true);

    //
    // Parse command-line
    wxApp& app = wxGetApp();
    if ( app.argc > 1 )
    {
        wxString s = app.argv[1];
        if ( s == wxT("--run-tests") )
        {
            //
            // Run tests
            bool testResult = frame->RunTests(true);

            if ( testResult )
                return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------

void FormMain::OnIdle( wxIdleEvent& event )
{
    /*
    // This code is useful for debugging focus problems
    static wxWindow* last_focus = (wxWindow*) NULL;

    wxWindow* cur_focus = ::wxWindow::FindFocus();

    if ( cur_focus != last_focus )
    {
        const wxChar* class_name = wxT("<none>");
        if ( cur_focus )
            class_name = cur_focus->GetClassInfo()->GetClassName();
        last_focus = cur_focus;
        wxLogDebug( wxT("FOCUSED: %s %X"),
            class_name,
            (unsigned int)cur_focus);
    }
    */

    event.Skip();
}

// -----------------------------------------------------------------------


wxPGProperty* GetRealRoot(wxPropertyGrid *grid)
{
    wxPGProperty *property = grid->GetRoot();
    return property ? grid->GetFirstChild(property) : NULL;
}

void GetColumnWidths(wxClientDC &dc, wxPropertyGrid *grid, wxPGProperty *root, int width[3])
{
    wxPropertyGridPageState *state = grid->GetState();

    width[0] =
    width[1] =
    width[2] = 0;
    int minWidths[3] = { state->GetColumnMinWidth(0),
                         state->GetColumnMinWidth(1),
                         state->GetColumnMinWidth(2) };
    unsigned ii;
    for (ii = 0; ii < root->GetChildCount(); ++ii)
    {
        wxPGProperty* p = root->Item(ii);

        width[0] = wxMax(width[0], state->GetColumnFullWidth(dc, p, 0));
        width[1] = wxMax(width[1], state->GetColumnFullWidth(dc, p, 1));
        width[2] = wxMax(width[2], state->GetColumnFullWidth(dc, p, 2));
    }
    for (ii = 0; ii < root->GetChildCount(); ++ii)
    {
        wxPGProperty* p = root->Item(ii);
        if (p->IsExpanded())
        {
            int w[3];
            GetColumnWidths(dc, grid, p, w);
            width[0] = wxMax(width[0], w[0]);
            width[1] = wxMax(width[1], w[1]);
            width[2] = wxMax(width[2], w[2]);
        }
    }

    width[0] = wxMax(width[0], minWidths[0]);
    width[1] = wxMax(width[1], minWidths[1]);
    width[2] = wxMax(width[2], minWidths[2]);
}

void GetColumnWidths(wxPropertyGrid *grid, wxPGProperty *root, int width[3])
{
    wxClientDC dc(grid);
    dc.SetFont(grid->GetFont());
    GetColumnWidths(dc, grid, root, width);
}

void SetMinSize(wxPropertyGrid *grid)
{
    wxPGProperty *p = GetRealRoot(grid);
    wxPGProperty *first = grid->wxPropertyGridInterface::GetFirst(wxPG_ITERATE_ALL);
    wxPGProperty *last = grid->GetLastItem(wxPG_ITERATE_DEFAULT);
    wxRect rect = grid->GetPropertyRect(first, last);
    int height = rect.height + 2 * grid->GetVerticalSpacing();

    // add some height when the root item is collapsed,
    // this is needed to prevent the vertical scroll from showing
    if (!grid->IsPropertyExpanded(p))
        height += 2 * grid->GetVerticalSpacing();

    int width[3];
    GetColumnWidths(grid, grid->GetRoot(), width);
    rect.width = width[0]+width[1]+width[2];

    int minWidth = (wxSystemSettings::GetMetric(wxSYS_SCREEN_X, grid->GetParent())*3)/2;
    int minHeight = (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y, grid->GetParent())*3)/2;

    wxSize size(wxMin(minWidth, rect.width + grid->GetMarginWidth()), wxMin(minHeight, height));
    grid->SetMinSize(size);

    int proportions[3];
    proportions[0] = wxRound((double)width[0]/size.x*100.0);
    proportions[1] = wxRound((double)width[1]/size.x*100.0);
    proportions[2]= wxMax(100 - proportions[0] - proportions[1], 0);
    grid->SetColumnProportion(0, proportions[0]);
    grid->SetColumnProportion(1, proportions[1]);
    grid->SetColumnProportion(2, proportions[2]);
    grid->ResetColumnSizes(true);
}

struct PropertyGridPopup : wxPopupWindow
{
    wxScrolledWindow *m_panel;
    wxPropertyGrid *m_grid;
    wxBoxSizer *m_sizer;

    PropertyGridPopup(wxWindow *parent) : wxPopupWindow(parent, wxBORDER_NONE|wxWANTS_CHARS)
    {
        m_panel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
        m_grid = new wxPropertyGrid(m_panel, ID_POPUPGRID, wxDefaultPosition, wxSize(400,400), wxPG_SPLITTER_AUTO_CENTER);
        m_grid->SetColumnCount(3);

        wxPGProperty *prop=m_grid->Append(new wxStringProperty("test_name", wxPG_LABEL, "test_value"));
        m_grid->SetPropertyAttribute(prop, wxPG_ATTR_UNITS, "type");
        wxPGProperty *prop1 = m_grid->AppendIn(prop, new wxStringProperty("sub_name1", wxPG_LABEL, "sub_value1"));

        m_grid->AppendIn(prop1, new wxSystemColourProperty(wxT("Cell Colour"),wxPG_LABEL, m_grid->GetGrid()->GetCellBackgroundColour()));
        wxPGProperty *prop2 = m_grid->AppendIn(prop, new wxStringProperty("sub_name2", wxPG_LABEL, "sub_value2"));
        m_grid->AppendIn(prop2, new wxStringProperty("sub_name21", wxPG_LABEL, "sub_value21"));

        wxArrayDouble arrdbl;
        arrdbl.Add(-1.0); arrdbl.Add(-0.5); arrdbl.Add(0.0); arrdbl.Add(0.5); arrdbl.Add(1.0);
        m_grid->AppendIn(prop, new wxArrayDoubleProperty(wxT("ArrayDoubleProperty"),wxPG_LABEL,arrdbl) );
        m_grid->AppendIn(prop, new wxFontProperty(wxT("Font"),wxPG_LABEL));
        m_grid->AppendIn(prop2, new wxStringProperty("sub_name22", wxPG_LABEL, "sub_value22"));
        m_grid->AppendIn(prop2, new wxStringProperty("sub_name23", wxPG_LABEL, "sub_value23"));
        prop2->SetExpanded(false);

        ::SetMinSize(m_grid);

        m_sizer = new wxBoxSizer( wxVERTICAL );
        m_sizer->Add(m_grid, 0, wxALL | wxEXPAND, 0);
        m_panel->SetAutoLayout(true);
        m_panel->SetSizer(m_sizer);
        m_sizer->Fit(m_panel);
        m_sizer->Fit(this);
    }

    void OnCollapse(wxPropertyGridEvent& WXUNUSED(event))
    {
        wxLogMessage("OnCollapse");
        Fit();
    }

    void OnExpand(wxPropertyGridEvent& WXUNUSED(event))
    {
        wxLogMessage("OnExpand");
        Fit();
    }

    void Fit() wxOVERRIDE
    {
        ::SetMinSize(m_grid);
        m_sizer->Fit(m_panel);
        wxPoint pos = GetScreenPosition();
        wxSize size = m_panel->GetScreenRect().GetSize();
        SetSize(pos.x, pos.y, size.x, size.y);
    }

    wxDECLARE_ABSTRACT_CLASS(PropertyGridPopup);
    wxDECLARE_EVENT_TABLE();
};

IMPLEMENT_CLASS(PropertyGridPopup, wxPopupWindow)
wxBEGIN_EVENT_TABLE(PropertyGridPopup, wxPopupWindow)
    EVT_PG_ITEM_COLLAPSED(ID_POPUPGRID, PropertyGridPopup::OnCollapse)
    EVT_PG_ITEM_EXPANDED(ID_POPUPGRID, PropertyGridPopup::OnExpand)
wxEND_EVENT_TABLE()


void FormMain::OnShowPopup(wxCommandEvent& WXUNUSED(event))
{
    static PropertyGridPopup *popup = NULL;
    if ( popup )
    {
        delete popup;
        popup = NULL;
        return;
    }
    popup = new PropertyGridPopup(this);
    wxPoint pt = wxGetMousePosition();
    popup->Position(pt, wxSize(0, 0));
    popup->Show();
}
