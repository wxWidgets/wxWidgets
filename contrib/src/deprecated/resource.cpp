/////////////////////////////////////////////////////////////////////////////
// Name:        contrib/src/deprecated/resource.cpp
// Purpose:     Resource system
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/deprecated/setup.h"

#if wxUSE_WX_RESOURCES

#ifdef __VISUALC__
#pragma warning(disable:4706)   // assignment within conditional expression
#endif // VC++

#ifndef WX_PRECOMP
    #include "wx/panel.h"
    #include "wx/list.h"
    #include "wx/hash.h"
    #include "wx/gdicmn.h"
    #include "wx/utils.h"
    #include "wx/types.h"
    #include "wx/menu.h"
    #include "wx/stattext.h"
    #include "wx/button.h"
    #include "wx/bmpbuttn.h"
    #include "wx/radiobox.h"
    #include "wx/listbox.h"
    #include "wx/choice.h"
    #include "wx/checkbox.h"
    #include "wx/settings.h"
    #include "wx/slider.h"
    #include "wx/icon.h"
    #include "wx/statbox.h"
    #include "wx/statbmp.h"
    #include "wx/gauge.h"
    #include "wx/textctrl.h"
    #include "wx/msgdlg.h"
    #include "wx/intl.h"
#endif

#include "wx/treebase.h"
#include "wx/listctrl.h"

#if wxUSE_RADIOBTN
#include "wx/radiobut.h"
#endif

#if wxUSE_SCROLLBAR
#include "wx/scrolbar.h"
#endif

#if wxUSE_COMBOBOX
#include "wx/combobox.h"
#endif

#include "wx/splitter.h"
#include "wx/toolbar.h"

#include "wx/validate.h"
#include "wx/log.h"
#include "wx/module.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "wx/string.h"
#include "wx/settings.h"
#include "wx/stream.h"

#include "wx/deprecated/resource.h"
#include "wx/deprecated/wxexpr.h"

#if !WXWIN_COMPATIBILITY_2_4
static inline wxChar* copystring(const wxChar* s)
    { return wxStrcpy(new wxChar[wxStrlen(s) + 1], s); }
#endif

// Forward (private) declarations
bool wxResourceInterpretResources(wxResourceTable& table, wxExprDatabase& db);
wxItemResource *wxResourceInterpretDialog(wxResourceTable& table, wxExpr *expr, bool isPanel = false);
wxItemResource *wxResourceInterpretControl(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretMenu(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretMenuBar(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretString(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretBitmap(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretIcon(wxResourceTable& table, wxExpr *expr);
// Interpret list expression
wxFont wxResourceInterpretFontSpec(wxExpr *expr);

bool wxResourceReadOneResource(FILE *fd, wxExprDatabase& db, bool *eof, wxResourceTable *table = (wxResourceTable *) NULL);
bool wxResourceReadOneResource(wxInputStream *fd, wxExprDatabase& db, bool *eof, wxResourceTable *table) ;
bool wxResourceParseIncludeFile(const wxString& f, wxResourceTable *table = (wxResourceTable *) NULL);

wxResourceTable *wxDefaultResourceTable = (wxResourceTable *) NULL;

char *wxResourceBuffer = (char *) NULL;
long wxResourceBufferSize = 0;
long wxResourceBufferCount = 0;
int wxResourceStringPtr = 0;

void wxInitializeResourceSystem()
{
    if (!wxDefaultResourceTable)
        wxDefaultResourceTable = new wxResourceTable;
}

void wxCleanUpResourceSystem()
{
    delete wxDefaultResourceTable;
    if (wxResourceBuffer)
        delete[] wxResourceBuffer;
}

// Module to ensure the resource system data gets initialized
// and cleaned up.

class wxResourceModule: public wxModule
{
public:
    wxResourceModule() : wxModule() {}
    virtual bool OnInit() { wxInitializeResourceSystem(); return true; }
    virtual void OnExit() { wxCleanUpResourceSystem();  }

    DECLARE_DYNAMIC_CLASS(wxResourceModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxResourceModule, wxModule)


IMPLEMENT_DYNAMIC_CLASS(wxItemResource, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxResourceTable, wxHashTable)

wxItemResource::wxItemResource()
{
    m_itemType = wxEmptyString;
    m_title = wxEmptyString;
    m_name = wxEmptyString;
    m_windowStyle = 0;
    m_x = m_y = m_width = m_height = 0;
    m_value1 = m_value2 = m_value3 = m_value5 = 0;
    m_value4 = wxEmptyString;
    m_windowId = 0;
    m_exStyle = 0;
}

wxItemResource::~wxItemResource()
{
    wxNode *node = m_children.GetFirst();
    while (node)
    {
        wxItemResource *item = (wxItemResource *)node->GetData();
        delete item;
        delete node;
        node = m_children.GetFirst();
    }
}

/*
* Resource table
*/

wxResourceTable::wxResourceTable():wxHashTable(wxKEY_STRING), identifiers(wxKEY_STRING)
{
}

wxResourceTable::~wxResourceTable()
{
    ClearTable();
}

wxItemResource *wxResourceTable::FindResource(const wxString& name) const
{
    wxItemResource *item = (wxItemResource *)Get(WXSTRINGCAST name);
    return item;
}

void wxResourceTable::AddResource(wxItemResource *item)
{
    wxString name = item->GetName();
    if (name.empty())
        name = item->GetTitle();
    if (name.empty())
        name = wxT("no name");

    // Delete existing resource, if any.
    Delete(name);

    Put(name, item);
}

bool wxResourceTable::DeleteResource(const wxString& name)
{
    wxItemResource *item = (wxItemResource *)Delete(WXSTRINGCAST name);
    if (item)
    {
        // See if any resource has this as its child; if so, delete from
        // parent's child list.
        BeginFind();
        wxHashTable::Node *node = Next();
        while (node != NULL)
        {
            wxItemResource *parent = (wxItemResource *)node->GetData();
            if (parent->GetChildren().Member(item))
            {
                parent->GetChildren().DeleteObject(item);
                break;
            }
            node = Next();
        }

        delete item;
        return true;
    }
    else
        return false;
}

bool wxResourceTable::ParseResourceFile( wxInputStream *is )
{
    wxExprDatabase db;
    int len = is->GetSize() ;

    bool eof = false;
    while ( is->TellI() + 10 < len) // it's a hack because the streams dont support EOF
    {
        wxResourceReadOneResource(is, db, &eof, this) ;
    }
    return wxResourceInterpretResources(*this, db);
}

bool wxResourceTable::ParseResourceFile(const wxString& filename)
{
    wxExprDatabase db;

    FILE *fd = wxFopen(filename, wxT("r"));
    if (!fd)
        return false;
    bool eof = false;
    while (wxResourceReadOneResource(fd, db, &eof, this) && !eof)
    {
        // Loop
    }
    fclose(fd);
    return wxResourceInterpretResources(*this, db);
}

bool wxResourceTable::ParseResourceData(const wxString& data)
{
    wxExprDatabase db;
    if (!db.ReadFromString(data))
    {
        wxLogWarning(_("Ill-formed resource file syntax."));
        return false;
    }

    return wxResourceInterpretResources(*this, db);
}

bool wxResourceTable::RegisterResourceBitmapData(const wxString& name, char bits[], int width, int height)
{
    // Register pre-loaded bitmap data
    wxItemResource *item = new wxItemResource;
    //  item->SetType(wxRESOURCE_TYPE_XBM_DATA);
    item->SetType(wxT("wxXBMData"));
    item->SetName(name);
    item->SetValue1((long)bits);
    item->SetValue2((long)width);
    item->SetValue3((long)height);
    AddResource(item);
    return true;
}

bool wxResourceTable::RegisterResourceBitmapData(const wxString& name, char **data)
{
    // Register pre-loaded bitmap data
    wxItemResource *item = new wxItemResource;
    //  item->SetType(wxRESOURCE_TYPE_XPM_DATA);
    item->SetType(wxT("wxXPMData"));
    item->SetName(name);
    item->SetValue1((long)data);
    AddResource(item);
    return true;
}

bool wxResourceTable::SaveResource(const wxString& WXUNUSED(filename))
{
    return false;
}

void wxResourceTable::ClearTable()
{
    BeginFind();
    wxHashTable::Node *node = Next();
    while (node)
    {
        wxHashTable::Node *next = Next();
        wxItemResource *item = (wxItemResource *)node->GetData();
        delete item;
        delete node;
        node = next;
    }
}

wxControl *wxResourceTable::CreateItem(wxWindow *parent, const wxItemResource* childResource, const wxItemResource* parentResource) const
{
    int id = childResource->GetId();
    if ( id == 0 )
        id = wxID_ANY;

    bool dlgUnits = ((parentResource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) != 0);

    wxControl *control = (wxControl *) NULL;
    wxString itemType(childResource->GetType());

    wxPoint pos;
    wxSize size;
    if (dlgUnits)
    {
        pos = parent->ConvertDialogToPixels(wxPoint(childResource->GetX(), childResource->GetY()));
        size = parent->ConvertDialogToPixels(wxSize(childResource->GetWidth(), childResource->GetHeight()));
    }
    else
    {
        pos = wxPoint(childResource->GetX(), childResource->GetY());
        size = wxSize(childResource->GetWidth(), childResource->GetHeight());
    }

    if (itemType == wxString(wxT("wxButton")) || itemType == wxString(wxT("wxBitmapButton")))
    {
        if (!childResource->GetValue4().empty())
        {
            // Bitmap button
            wxBitmap bitmap = childResource->GetBitmap();
            if (!bitmap.Ok())
            {
                bitmap = wxResourceCreateBitmap(childResource->GetValue4(), (wxResourceTable *)this);
                ((wxItemResource*) childResource)->SetBitmap(bitmap);
            }
            if (!bitmap.Ok())
#if defined(__WXPM__)
                //
                // OS/2 uses integer id's to access resources, not file name strings
                //
                bitmap.LoadFile(wxCROSS_BITMAP, wxBITMAP_TYPE_BMP_RESOURCE);
#else
                bitmap.LoadFile(wxT("cross_bmp"), wxBITMAP_TYPE_BMP_RESOURCE);
#endif
            control = new wxBitmapButton(parent, id, bitmap, pos, size,
                childResource->GetStyle() | wxBU_AUTODRAW, wxDefaultValidator, childResource->GetName());
        }
        else
            // Normal, text button
            control = new wxButton(parent, id, childResource->GetTitle(), pos, size,
            childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
    }
    else if (itemType == wxString(wxT("wxMessage")) || itemType == wxString(wxT("wxStaticText")) ||
        itemType == wxString(wxT("wxStaticBitmap")))
    {
        if (!childResource->GetValue4().empty() || itemType == wxString(wxT("wxStaticBitmap")) )
        {
            // Bitmap message
            wxBitmap bitmap = childResource->GetBitmap();
            if (!bitmap.Ok())
            {
                bitmap = wxResourceCreateBitmap(childResource->GetValue4(), (wxResourceTable *)this);
                ((wxItemResource*) childResource)->SetBitmap(bitmap);
            }
#if wxUSE_BITMAP_MESSAGE
#ifdef __WXMSW__
            // Use a default bitmap
            if (!bitmap.Ok())
                bitmap.LoadFile(wxT("cross_bmp"), wxBITMAP_TYPE_BMP_RESOURCE);
#endif

            if (bitmap.Ok())
                control = new wxStaticBitmap(parent, id, bitmap, pos, size,
                childResource->GetStyle(), childResource->GetName());
#endif
        }
        else
        {
            control = new wxStaticText(parent, id, childResource->GetTitle(), pos, size,
                childResource->GetStyle(), childResource->GetName());
        }
    }
    else if (itemType == wxString(wxT("wxText")) || itemType == wxString(wxT("wxTextCtrl")) || itemType == wxString(wxT("wxMultiText")))
    {
        control = new wxTextCtrl(parent, id, childResource->GetValue4(), pos, size,
            childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
    }
    else if (itemType == wxString(wxT("wxCheckBox")))
    {
        control = new wxCheckBox(parent, id, childResource->GetTitle(), pos, size,
            childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        ((wxCheckBox *)control)->SetValue((childResource->GetValue1() != 0));
    }
#if wxUSE_GAUGE
    else if (itemType == wxString(wxT("wxGauge")))
    {
        control = new wxGauge(parent, id, (int)childResource->GetValue2(), pos, size,
            childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        ((wxGauge *)control)->SetValue((int)childResource->GetValue1());
    }
#endif
#if wxUSE_RADIOBTN
    else if (itemType == wxString(wxT("wxRadioButton")))
    {
        control = new wxRadioButton(parent, id, childResource->GetTitle(), // (int)childResource->GetValue1(),
            pos, size,
            childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
    }
#endif
#if wxUSE_SCROLLBAR
    else if (itemType == wxString(wxT("wxScrollBar")))
    {
        control = new wxScrollBar(parent, id, pos, size,
            childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
            /*
            ((wxScrollBar *)control)->SetValue((int)childResource->GetValue1());
            ((wxScrollBar *)control)->SetPageSize((int)childResource->GetValue2());
            ((wxScrollBar *)control)->SetObjectLength((int)childResource->GetValue3());
            ((wxScrollBar *)control)->SetViewLength((int)(long)childResource->GetValue5());
        */
        ((wxScrollBar *)control)->SetScrollbar((int)childResource->GetValue1(),(int)childResource->GetValue2(),
            (int)childResource->GetValue3(),(int)(long)childResource->GetValue5(),false);

    }
#endif
    else if (itemType == wxString(wxT("wxSlider")))
    {
        control = new wxSlider(parent, id, (int)childResource->GetValue1(),
            (int)childResource->GetValue2(), (int)childResource->GetValue3(), pos, size,
            childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
    }
    else if (itemType == wxString(wxT("wxGroupBox")) || itemType == wxString(wxT("wxStaticBox")))
    {
        control = new wxStaticBox(parent, id, childResource->GetTitle(), pos, size,
            childResource->GetStyle(), childResource->GetName());
    }
    else if (itemType == wxString(wxT("wxListBox")))
    {
        wxStringList& stringList = childResource->GetStringValues();
        wxString *strings = (wxString *) NULL;
        int noStrings = 0;
        if (stringList.GetCount() > 0)
        {
            noStrings = stringList.GetCount();
            strings = new wxString[noStrings];
            wxStringListNode *node = stringList.GetFirst();
            int i = 0;
            while (node)
            {
                strings[i] = (wxChar *)node->GetData();
                i ++;
                node = node->GetNext();
            }
        }
        control = new wxListBox(parent, id, pos, size,
            noStrings, strings, childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        if (strings)
            delete[] strings;
    }
    else if (itemType == wxString(wxT("wxChoice")))
    {
        wxStringList& stringList = childResource->GetStringValues();
        wxString *strings = (wxString *) NULL;
        int noStrings = 0;
        if (stringList.GetCount() > 0)
        {
            noStrings = stringList.GetCount();
            strings = new wxString[noStrings];
            wxStringListNode *node = stringList.GetFirst();
            int i = 0;
            while (node)
            {
                strings[i] = (wxChar *)node->GetData();
                i ++;
                node = node->GetNext();
            }
        }
        control = new wxChoice(parent, id, pos, size,
            noStrings, strings, childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        if (strings)
            delete[] strings;
    }
#if wxUSE_COMBOBOX
    else if (itemType == wxString(wxT("wxComboBox")))
    {
        wxStringList& stringList = childResource->GetStringValues();
        wxString *strings = (wxString *) NULL;
        int noStrings = 0;
        if (stringList.GetCount() > 0)
        {
            noStrings = stringList.GetCount();
            strings = new wxString[noStrings];
            wxStringListNode *node = stringList.GetFirst();
            int i = 0;
            while (node)
            {
                strings[i] = (wxChar *)node->GetData();
                i ++;
                node = node->GetNext();
            }
        }
        control = new wxComboBox(parent, id, childResource->GetValue4(), pos, size,
            noStrings, strings, childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        if (strings)
            delete[] strings;
    }
#endif
    else if (itemType == wxString(wxT("wxRadioBox")))
    {
        wxStringList& stringList = childResource->GetStringValues();
        wxString *strings = (wxString *) NULL;
        int noStrings = 0;
        if (stringList.GetCount() > 0)
        {
            noStrings = stringList.GetCount();
            strings = new wxString[noStrings];
            wxStringListNode *node = stringList.GetFirst();
            int i = 0;
            while (node)
            {
                strings[i] = (wxChar *)node->GetData();
                i ++;
                node = node->GetNext();
            }
        }
        control = new wxRadioBox(parent, (wxWindowID) id, wxString(childResource->GetTitle()), pos, size,
            noStrings, strings, (int)childResource->GetValue1(), childResource->GetStyle(), wxDefaultValidator,
            childResource->GetName());

        if (strings)
            delete[] strings;
    }

    if ((parentResource->GetResourceStyle() & wxRESOURCE_USE_DEFAULTS) != 0)
    {
        // Don't set font; will be inherited from parent.
    }
    else
    {
        if (control && childResource->GetFont().Ok())
        {
            control->SetFont(childResource->GetFont());

#ifdef __WXMSW__
            // Force the layout algorithm since the size changes the layout
            if (control->IsKindOf(CLASSINFO(wxRadioBox)))
            {
                control->SetSize(wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, wxSIZE_AUTO_WIDTH|wxSIZE_AUTO_HEIGHT);
            }
#endif
        }
    }
    return control;
}

/*
* Interpret database as a series of resources
*/

bool wxResourceInterpretResources(wxResourceTable& table, wxExprDatabase& db)
{
    wxNode *node = db.GetFirst();
    while (node)
    {
        wxExpr *clause = (wxExpr *)node->GetData();
        wxString functor(clause->Functor());

        wxItemResource *item = (wxItemResource *) NULL;
        if (functor == wxT("dialog"))
            item = wxResourceInterpretDialog(table, clause);
        else if (functor == wxT("panel"))
            item = wxResourceInterpretDialog(table, clause, true);
        else if (functor == wxT("menubar"))
            item = wxResourceInterpretMenuBar(table, clause);
        else if (functor == wxT("menu"))
            item = wxResourceInterpretMenu(table, clause);
        else if (functor == wxT("string"))
            item = wxResourceInterpretString(table, clause);
        else if (functor == wxT("bitmap"))
            item = wxResourceInterpretBitmap(table, clause);
        else if (functor == wxT("icon"))
            item = wxResourceInterpretIcon(table, clause);

        if (item)
        {
            // Remove any existing resource of same name
            if (!item->GetName().empty())
                table.DeleteResource(item->GetName());
            table.AddResource(item);
        }
        node = node->GetNext();
    }
    return true;
}

static const wxChar *g_ValidControlClasses[] =
{
    wxT("wxButton"),
        wxT("wxBitmapButton"),
        wxT("wxMessage"),
        wxT("wxStaticText"),
        wxT("wxStaticBitmap"),
        wxT("wxText"),
        wxT("wxTextCtrl"),
        wxT("wxMultiText"),
        wxT("wxListBox"),
        wxT("wxRadioBox"),
        wxT("wxRadioButton"),
        wxT("wxCheckBox"),
        wxT("wxBitmapCheckBox"),
        wxT("wxGroupBox"),
        wxT("wxStaticBox"),
        wxT("wxSlider"),
        wxT("wxGauge"),
        wxT("wxScrollBar"),
        wxT("wxChoice"),
        wxT("wxComboBox")
};

static bool wxIsValidControlClass(const wxString& c)
{
    for ( size_t i = 0; i < WXSIZEOF(g_ValidControlClasses); i++ )
    {
        if ( c == g_ValidControlClasses[i] )
            return true;
    }
    return false;
}

wxItemResource *wxResourceInterpretDialog(wxResourceTable& table, wxExpr *expr, bool isPanel)
{
    wxItemResource *dialogItem = new wxItemResource;
    if (isPanel)
        dialogItem->SetType(wxT("wxPanel"));
    else
        dialogItem->SetType(wxT("wxDialog"));
    wxString style = wxEmptyString;
    wxString title = wxEmptyString;
    wxString name = wxEmptyString;
    wxString backColourHex = wxEmptyString;
    wxString labelColourHex = wxEmptyString;
    wxString buttonColourHex = wxEmptyString;

    long windowStyle = wxDEFAULT_DIALOG_STYLE;
    if (isPanel)
        windowStyle = 0;

    int x = 0; int y = 0; int width = wxDefaultCoord; int height = wxDefaultCoord;
    int isModal = 0;
    wxExpr *labelFontExpr = (wxExpr *) NULL;
    wxExpr *buttonFontExpr = (wxExpr *) NULL;
    wxExpr *fontExpr = (wxExpr *) NULL;
    expr->GetAttributeValue(wxT("style"), style);
    expr->GetAttributeValue(wxT("name"), name);
    expr->GetAttributeValue(wxT("title"), title);
    expr->GetAttributeValue(wxT("x"), x);
    expr->GetAttributeValue(wxT("y"), y);
    expr->GetAttributeValue(wxT("width"), width);
    expr->GetAttributeValue(wxT("height"), height);
    expr->GetAttributeValue(wxT("modal"), isModal);
    expr->GetAttributeValue(wxT("label_font"), &labelFontExpr);
    expr->GetAttributeValue(wxT("button_font"), &buttonFontExpr);
    expr->GetAttributeValue(wxT("font"), &fontExpr);
    expr->GetAttributeValue(wxT("background_colour"), backColourHex);
    expr->GetAttributeValue(wxT("label_colour"), labelColourHex);
    expr->GetAttributeValue(wxT("button_colour"), buttonColourHex);

    int useDialogUnits = 0;
    expr->GetAttributeValue(wxT("use_dialog_units"), useDialogUnits);
    if (useDialogUnits != 0)
        dialogItem->SetResourceStyle(dialogItem->GetResourceStyle() | wxRESOURCE_DIALOG_UNITS);

    int useDefaults = 0;
    expr->GetAttributeValue(wxT("use_system_defaults"), useDefaults);
    if (useDefaults != 0)
        dialogItem->SetResourceStyle(dialogItem->GetResourceStyle() | wxRESOURCE_USE_DEFAULTS);

    int id = 0;
    expr->GetAttributeValue(wxT("id"), id);
    dialogItem->SetId(id);

    if (!style.empty())
    {
        windowStyle = wxParseWindowStyle(style);
    }
    dialogItem->SetStyle(windowStyle);
    dialogItem->SetValue1(isModal);

#if WXWIN_COMPATIBILITY_2_6

#ifdef __VMS
#pragma message disable CODCAUUNR
#endif
   if (windowStyle & wxDIALOG_MODAL) // Uses style in wxWin 2
        dialogItem->SetValue1(true);
#ifdef __VMS
#pragma message enable CODCAUUNR
#endif

#endif // WXWIN_COMPATIBILITY_2_6

    dialogItem->SetName(name);
    dialogItem->SetTitle(title);
    dialogItem->SetSize(x, y, width, height);

    // Check for wxWin 1.68-style specifications
    if (style.Find(wxT("VERTICAL_LABEL")) != wxNOT_FOUND)
        dialogItem->SetResourceStyle(dialogItem->GetResourceStyle() | wxRESOURCE_VERTICAL_LABEL);
    else if (style.Find(wxT("HORIZONTAL_LABEL")) != wxNOT_FOUND)
        dialogItem->SetResourceStyle(dialogItem->GetResourceStyle() | wxRESOURCE_HORIZONTAL_LABEL);

    if (!backColourHex.empty())
    {
        int r = 0;
        int g = 0;
        int b = 0;
        r = wxHexToDec(backColourHex.Mid(0, 2));
        g = wxHexToDec(backColourHex.Mid(2, 2));
        b = wxHexToDec(backColourHex.Mid(4, 2));
        dialogItem->SetBackgroundColour(wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
    }
    if (!labelColourHex.empty())
    {
        int r = 0;
        int g = 0;
        int b = 0;
        r = wxHexToDec(labelColourHex.Mid(0, 2));
        g = wxHexToDec(labelColourHex.Mid(2, 2));
        b = wxHexToDec(labelColourHex.Mid(4, 2));
        dialogItem->SetLabelColour(wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
    }
    if (!buttonColourHex.empty())
    {
        int r = 0;
        int g = 0;
        int b = 0;
        r = wxHexToDec(buttonColourHex.Mid(0, 2));
        g = wxHexToDec(buttonColourHex.Mid(2, 2));
        b = wxHexToDec(buttonColourHex.Mid(4, 2));
        dialogItem->SetButtonColour(wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
    }

    if (fontExpr)
        dialogItem->SetFont(wxResourceInterpretFontSpec(fontExpr));
    else if (buttonFontExpr)
        dialogItem->SetFont(wxResourceInterpretFontSpec(buttonFontExpr));
    else if (labelFontExpr)
        dialogItem->SetFont(wxResourceInterpretFontSpec(labelFontExpr));

    // Now parse all controls
    wxExpr *controlExpr = expr->GetFirst();
    while (controlExpr)
    {
        if (controlExpr->Number() == 3)
        {
            wxString controlKeyword(controlExpr->Nth(1)->StringValue());
            if (!controlKeyword.empty() && controlKeyword == wxT("control"))
            {
                // The value part: always a list.
                wxExpr *listExpr = controlExpr->Nth(2);
                if (listExpr->Type() == PrologList)
                {
                    wxItemResource *controlItem = wxResourceInterpretControl(table, listExpr);
                    if (controlItem)
                    {
                        dialogItem->GetChildren().Append(controlItem);
                    }
                }
            }
        }
        controlExpr = controlExpr->GetNext();
    }
    return dialogItem;
}

wxItemResource *wxResourceInterpretControl(wxResourceTable& table, wxExpr *expr)
{
    wxItemResource *controlItem = new wxItemResource;

    // First, find the standard features of a control definition:
    // [optional integer/string id], control name, title, style, name, x, y, width, height

    wxString controlType;
    wxString style;
    wxString title;
    wxString name;
    int id = 0;
    long windowStyle = 0;
    int x = 0; int y = 0; int width = wxDefaultCoord; int height = wxDefaultCoord;
    int count = 0;

    wxExpr *expr1 = expr->Nth(0);

    if ( expr1->Type() == PrologString || expr1->Type() == PrologWord )
    {
        if ( wxIsValidControlClass(expr1->StringValue()) )
        {
            count = 1;
            controlType = expr1->StringValue();
        }
        else
        {
            wxString str(expr1->StringValue());
            id = wxResourceGetIdentifier(str, &table);
            if (id == 0)
            {
                wxLogWarning(_("Could not resolve control class or id '%s'. Use (non-zero) integer instead\n or provide #define (see manual for caveats)"),
                    (const wxChar*) expr1->StringValue());
                delete controlItem;
                return (wxItemResource *) NULL;
            }
            else
            {
                // Success - we have an id, so the 2nd element must be the control class.
                controlType = expr->Nth(1)->StringValue();
                count = 2;
            }
        }
    }
    else if (expr1->Type() == PrologInteger)
    {
        id = (int)expr1->IntegerValue();
        // Success - we have an id, so the 2nd element must be the control class.
        controlType = expr->Nth(1)->StringValue();
        count = 2;
    }

    expr1 = expr->Nth(count);
    count ++;
    if ( expr1 )
        title = expr1->StringValue();

    expr1 = expr->Nth(count);
    count ++;
    if (expr1)
    {
        style = expr1->StringValue();
        windowStyle = wxParseWindowStyle(style);
    }

    expr1 = expr->Nth(count);
    count ++;
    if (expr1)
        name = expr1->StringValue();

    expr1 = expr->Nth(count);
    count ++;
    if (expr1)
        x = (int)expr1->IntegerValue();

    expr1 = expr->Nth(count);
    count ++;
    if (expr1)
        y = (int)expr1->IntegerValue();

    expr1 = expr->Nth(count);
    count ++;
    if (expr1)
        width = (int)expr1->IntegerValue();

    expr1 = expr->Nth(count);
    count ++;
    if (expr1)
        height = (int)expr1->IntegerValue();

    controlItem->SetStyle(windowStyle);
    controlItem->SetName(name);
    controlItem->SetTitle(title);
    controlItem->SetSize(x, y, width, height);
    controlItem->SetType(controlType);
    controlItem->SetId(id);

    // Check for wxWin 1.68-style specifications
    if (style.Find(wxT("VERTICAL_LABEL")) != wxNOT_FOUND)
        controlItem->SetResourceStyle(controlItem->GetResourceStyle() | wxRESOURCE_VERTICAL_LABEL);
    else if (style.Find(wxT("HORIZONTAL_LABEL")) != wxNOT_FOUND)
        controlItem->SetResourceStyle(controlItem->GetResourceStyle() | wxRESOURCE_HORIZONTAL_LABEL);

    if (controlType == wxT("wxButton"))
    {
        // Check for bitmap resource name (in case loading old-style resource file)
        if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
        {
            wxString str(expr->Nth(count)->StringValue());
            count ++;

            if (!str.empty())
            {
                controlItem->SetValue4(str);
                controlItem->SetType(wxT("wxBitmapButton"));
            }
        }
        if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
    }
    else if (controlType == wxT("wxBitmapButton"))
    {
        // Check for bitmap resource name
        if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
        {
            wxString str(expr->Nth(count)->StringValue());
            controlItem->SetValue4(str);
            count ++;
            if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
                controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
        }
    }
    else if (controlType == wxT("wxCheckBox"))
    {
        // Check for default value
        if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
        {
            controlItem->SetValue1(expr->Nth(count)->IntegerValue());
            count ++;
            if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
                controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
        }
    }
#if wxUSE_RADIOBTN
    else if (controlType == wxT("wxRadioButton"))
    {
        // Check for default value
        if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
        {
            controlItem->SetValue1(expr->Nth(count)->IntegerValue());
            count ++;
            if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
                controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
        }
    }
#endif
    else if (controlType == wxT("wxText") || controlType == wxT("wxTextCtrl") || controlType == wxT("wxMultiText"))
    {
        // Check for default value
        if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
        {
            wxString str(expr->Nth(count)->StringValue());
            controlItem->SetValue4(str);
            count ++;

            if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            {
                // controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(count)));
                // Skip past the obsolete label font spec if there are two consecutive specs
                if (expr->Nth(count+1) && expr->Nth(count+1)->Type() == PrologList)
                    count ++;
                controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
            }
        }
    }
    else if (controlType == wxT("wxMessage") || controlType == wxT("wxStaticText"))
    {
        // Check for bitmap resource name (in case it's an old-style .wxr file)
        if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
        {
            wxString str(expr->Nth(count)->StringValue());
            controlItem->SetValue4(str);
            count ++;
            controlItem->SetType(wxT("wxStaticText"));
        }
        if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
    }
    else if (controlType == wxT("wxStaticBitmap"))
    {
        // Check for bitmap resource name
        if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
        {
            wxString str(expr->Nth(count)->StringValue());
            controlItem->SetValue4(str);
            count ++;
        }
        if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
    }
    else if (controlType == wxT("wxGroupBox") || controlType == wxT("wxStaticBox"))
    {
        if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
    }
    else if (controlType == wxT("wxGauge"))
    {
        // Check for default value
        if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
        {
            controlItem->SetValue1(expr->Nth(count)->IntegerValue());
            count ++;

            // Check for range
            if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
            {
                controlItem->SetValue2(expr->Nth(count)->IntegerValue());
                count ++;

                if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
                {
                    // Skip past the obsolete label font spec if there are two consecutive specs
                    if (expr->Nth(count+1) && expr->Nth(count+1)->Type() == PrologList)
                        count ++;
                    controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
                }
            }
        }
    }
    else if (controlType == wxT("wxSlider"))
    {
        // Check for default value
        if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
        {
            controlItem->SetValue1(expr->Nth(count)->IntegerValue());
            count ++;

            // Check for min
            if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
            {
                controlItem->SetValue2(expr->Nth(count)->IntegerValue());
                count ++;

                // Check for max
                if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
                {
                    controlItem->SetValue3(expr->Nth(count)->IntegerValue());
                    count ++;

                    if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
                    {
                        // controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(count)));
                        // do nothing
                        count ++;

                        if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
                            controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
                    }
                }
            }
        }
    }
    else if (controlType == wxT("wxScrollBar"))
    {
        // DEFAULT VALUE
        if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
        {
            controlItem->SetValue1(expr->Nth(count)->IntegerValue());
            count ++;

            // PAGE LENGTH
            if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
            {
                controlItem->SetValue2(expr->Nth(count)->IntegerValue());
                count ++;

                // OBJECT LENGTH
                if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
                {
                    controlItem->SetValue3(expr->Nth(count)->IntegerValue());
                    count ++;

                    // VIEW LENGTH
                    if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
                        controlItem->SetValue5(expr->Nth(count)->IntegerValue());
                }
            }
        }
    }
    else if (controlType == wxT("wxListBox"))
    {
        wxExpr *valueList = (wxExpr *) NULL;

        if (((valueList = expr->Nth(count)) != 0) && (valueList->Type() == PrologList))
        {
            wxStringList stringList;
            wxExpr *stringExpr = valueList->GetFirst();
            while (stringExpr)
            {
                stringList.Add(stringExpr->StringValue());
                stringExpr = stringExpr->GetNext();
            }
            controlItem->SetStringValues(stringList);
            count ++;
            // This is now obsolete: it's in the window style.
            // Check for wxSINGLE/wxMULTIPLE
            wxExpr *mult = (wxExpr *) NULL;
            /*
            controlItem->SetValue1(wxLB_SINGLE);
            */
            if (((mult = expr->Nth(count)) != 0) && ((mult->Type() == PrologString)||(mult->Type() == PrologWord)))
            {
            /*
            wxString m(mult->StringValue());
            if (m == "wxLB_MULTIPLE")
            controlItem->SetValue1(wxLB_MULTIPLE);
            else if (m == "wxLB_EXTENDED")
            controlItem->SetValue1(wxLB_EXTENDED);
                */
                // Ignore the value
                count ++;
            }
            if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            {
                // Skip past the obsolete label font spec if there are two consecutive specs
                if (expr->Nth(count+1) && expr->Nth(count+1)->Type() == PrologList)
                    count ++;
                controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
            }
        }
    }
    else if (controlType == wxT("wxChoice"))
    {
        wxExpr *valueList = (wxExpr *) NULL;
        // Check for default value list
        if (((valueList = expr->Nth(count)) != 0) && (valueList->Type() == PrologList))
        {
            wxStringList stringList;
            wxExpr *stringExpr = valueList->GetFirst();
            while (stringExpr)
            {
                stringList.Add(stringExpr->StringValue());
                stringExpr = stringExpr->GetNext();
            }
            controlItem->SetStringValues(stringList);

            count ++;

            if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            {
                // Skip past the obsolete label font spec if there are two consecutive specs
                if (expr->Nth(count+1) && expr->Nth(count+1)->Type() == PrologList)
                    count ++;
                controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
            }
        }
    }
#if wxUSE_COMBOBOX
    else if (controlType == wxT("wxComboBox"))
    {
        wxExpr *textValue = expr->Nth(count);
        if (textValue && (textValue->Type() == PrologString || textValue->Type() == PrologWord))
        {
            wxString str(textValue->StringValue());
            controlItem->SetValue4(str);

            count ++;

            wxExpr *valueList = (wxExpr *) NULL;
            // Check for default value list
            if (((valueList = expr->Nth(count)) != 0) && (valueList->Type() == PrologList))
            {
                wxStringList stringList;
                wxExpr *stringExpr = valueList->GetFirst();
                while (stringExpr)
                {
                    stringList.Add(stringExpr->StringValue());
                    stringExpr = stringExpr->GetNext();
                }
                controlItem->SetStringValues(stringList);

                count ++;

                if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
                {
                    // Skip past the obsolete label font spec if there are two consecutive specs
                    if (expr->Nth(count+1) && expr->Nth(count+1)->Type() == PrologList)
                        count ++;
                    controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
                }
            }
        }
    }
#endif
#if 1
    else if (controlType == wxT("wxRadioBox"))
    {
        wxExpr *valueList = (wxExpr *) NULL;
        // Check for default value list
        if (((valueList = expr->Nth(count)) != 0) && (valueList->Type() == PrologList))
        {
            wxStringList stringList;
            wxExpr *stringExpr = valueList->GetFirst();
            while (stringExpr)
            {
                stringList.Add(stringExpr->StringValue());
                stringExpr = stringExpr->GetNext();
            }
            controlItem->SetStringValues(stringList);
            count ++;

            // majorDim (number of rows or cols)
            if (expr->Nth(count) && (expr->Nth(count)->Type() == PrologInteger))
            {
                controlItem->SetValue1(expr->Nth(count)->IntegerValue());
                count ++;
            }
            else
                controlItem->SetValue1(0);

            if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            {
                // Skip past the obsolete label font spec if there are two consecutive specs
                if (expr->Nth(count+1) && expr->Nth(count+1)->Type() == PrologList)
                    count ++;
                controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
            }
        }
    }
#endif
    else
    {
        delete controlItem;
        return (wxItemResource *) NULL;
    }
    return controlItem;
}

// Forward declaration
wxItemResource *wxResourceInterpretMenu1(wxResourceTable& table, wxExpr *expr);

/*
* Interpet a menu item
*/

wxItemResource *wxResourceInterpretMenuItem(wxResourceTable& table, wxExpr *expr)
{
    wxItemResource *item = new wxItemResource;

    wxExpr *labelExpr = expr->Nth(0);
    wxExpr *idExpr = expr->Nth(1);
    wxExpr *helpExpr = expr->Nth(2);
    wxExpr *checkableExpr = expr->Nth(3);

    // Further keywords/attributes to follow sometime...
    if (expr->Number() == 0)
    {
        //    item->SetType(wxRESOURCE_TYPE_SEPARATOR);
        item->SetType(wxT("wxMenuSeparator"));
        return item;
    }
    else
    {
        //    item->SetType(wxTYPE_MENU); // Well, menu item, but doesn't matter.
        item->SetType(wxT("wxMenu")); // Well, menu item, but doesn't matter.
        if (labelExpr)
        {
            wxString str(labelExpr->StringValue());
            item->SetTitle(str);
        }
        if (idExpr)
        {
            int id = 0;
            // If a string or word, must look up in identifier table.
            if ((idExpr->Type() == PrologString) || (idExpr->Type() == PrologWord))
            {
                wxString str(idExpr->StringValue());
                id = wxResourceGetIdentifier(str, &table);
                if (id == 0)
                {
                    wxLogWarning(_("Could not resolve menu id '%s'. Use (non-zero) integer instead\nor provide #define (see manual for caveats)"),
                        (const wxChar*) idExpr->StringValue());
                }
            }
            else if (idExpr->Type() == PrologInteger)
                id = (int)idExpr->IntegerValue();
            item->SetValue1(id);
        }
        if (helpExpr)
        {
            wxString str(helpExpr->StringValue());
            item->SetValue4(str);
        }
        if (checkableExpr)
            item->SetValue2(checkableExpr->IntegerValue());

        // Find the first expression that's a list, for submenu
        wxExpr *subMenuExpr = expr->GetFirst();
        while (subMenuExpr && (subMenuExpr->Type() != PrologList))
            subMenuExpr = subMenuExpr->GetNext();

        while (subMenuExpr)
        {
            wxItemResource *child = wxResourceInterpretMenuItem(table, subMenuExpr);
            item->GetChildren().Append(child);
            subMenuExpr = subMenuExpr->GetNext();
        }
    }
    return item;
}

/*
* Interpret a nested list as a menu
*/
/*
wxItemResource *wxResourceInterpretMenu1(wxResourceTable& table, wxExpr *expr)
{
wxItemResource *menu = new wxItemResource;
//  menu->SetType(wxTYPE_MENU);
menu->SetType("wxMenu");
wxExpr *element = expr->GetFirst();
while (element)
{
wxItemResource *item = wxResourceInterpretMenuItem(table, element);
if (item)
menu->GetChildren().Append(item);
element = element->GetNext();
}
return menu;
}
*/

wxItemResource *wxResourceInterpretMenu(wxResourceTable& table, wxExpr *expr)
{
    wxExpr *listExpr = (wxExpr *) NULL;
    expr->GetAttributeValue(wxT("menu"), &listExpr);
    if (!listExpr)
        return (wxItemResource *) NULL;

    wxItemResource *menuResource = wxResourceInterpretMenuItem(table, listExpr);

    if (!menuResource)
        return (wxItemResource *) NULL;

    wxString name;
    if (expr->GetAttributeValue(wxT("name"), name))
    {
        menuResource->SetName(name);
    }

    return menuResource;
}

wxItemResource *wxResourceInterpretMenuBar(wxResourceTable& table, wxExpr *expr)
{
    wxExpr *listExpr = (wxExpr *) NULL;
    expr->GetAttributeValue(wxT("menu"), &listExpr);
    if (!listExpr)
        return (wxItemResource *) NULL;

    wxItemResource *resource = new wxItemResource;
    resource->SetType(wxT("wxMenu"));
    //  resource->SetType(wxTYPE_MENU);

    wxExpr *element = listExpr->GetFirst();
    while (element)
    {
        wxItemResource *menuResource = wxResourceInterpretMenuItem(table, listExpr);
        resource->GetChildren().Append(menuResource);
        element = element->GetNext();
    }

    wxString name;
    if (expr->GetAttributeValue(wxT("name"), name))
    {
        resource->SetName(name);
    }

    return resource;
}

wxItemResource *wxResourceInterpretString(wxResourceTable& WXUNUSED(table), wxExpr *WXUNUSED(expr))
{
    return (wxItemResource *) NULL;
}

wxItemResource *wxResourceInterpretBitmap(wxResourceTable& WXUNUSED(table), wxExpr *expr)
{
    wxItemResource *bitmapItem = new wxItemResource;
    //  bitmapItem->SetType(wxTYPE_BITMAP);
    bitmapItem->SetType(wxT("wxBitmap"));
    wxString name;
    if (expr->GetAttributeValue(wxT("name"), name))
    {
        bitmapItem->SetName(name);
    }
    // Now parse all bitmap specifications
    wxExpr *bitmapExpr = expr->GetFirst();
    while (bitmapExpr)
    {
        if (bitmapExpr->Number() == 3)
        {
            wxString bitmapKeyword(bitmapExpr->Nth(1)->StringValue());
            if (bitmapKeyword == wxT("bitmap") || bitmapKeyword == wxT("icon"))
            {
                // The value part: always a list.
                wxExpr *listExpr = bitmapExpr->Nth(2);
                if (listExpr->Type() == PrologList)
                {
                    wxItemResource *bitmapSpec = new wxItemResource;
                    //          bitmapSpec->SetType(wxTYPE_BITMAP);
                    bitmapSpec->SetType(wxT("wxBitmap"));

                    // List is of form: [filename, bitmaptype, platform, colours, xresolution, yresolution]
                    // where everything after 'filename' is optional.
                    wxExpr *nameExpr = listExpr->Nth(0);
                    wxExpr *typeExpr = listExpr->Nth(1);
                    wxExpr *platformExpr = listExpr->Nth(2);
                    wxExpr *coloursExpr = listExpr->Nth(3);
                    wxExpr *xresExpr = listExpr->Nth(4);
                    wxExpr *yresExpr = listExpr->Nth(5);
                    if (nameExpr && !nameExpr->StringValue().empty())
                    {
                        bitmapSpec->SetName(nameExpr->StringValue());
                    }
                    if (typeExpr && !typeExpr->StringValue().empty())
                    {
                        bitmapSpec->SetValue1(wxParseWindowStyle(typeExpr->StringValue()));
                    }
                    else
                        bitmapSpec->SetValue1(0);

                    if (platformExpr && !platformExpr->StringValue().empty())
                    {
                        wxString plat(platformExpr->StringValue());
                        if (plat == wxT("windows") || plat == wxT("WINDOWS"))
                            bitmapSpec->SetValue2(RESOURCE_PLATFORM_WINDOWS);
                        else if (plat == wxT("x") || plat == wxT("X"))
                            bitmapSpec->SetValue2(RESOURCE_PLATFORM_X);
                        else if (plat == wxT("mac") || plat == wxT("MAC"))
                            bitmapSpec->SetValue2(RESOURCE_PLATFORM_MAC);
                        else
                            bitmapSpec->SetValue2(RESOURCE_PLATFORM_ANY);
                    }
                    else
                        bitmapSpec->SetValue2(RESOURCE_PLATFORM_ANY);

                    if (coloursExpr)
                        bitmapSpec->SetValue3(coloursExpr->IntegerValue());
                    int xres = 0;
                    int yres = 0;
                    if (xresExpr)
                        xres = (int)xresExpr->IntegerValue();
                    if (yresExpr)
                        yres = (int)yresExpr->IntegerValue();
                    bitmapSpec->SetSize(0, 0, xres, yres);

                    bitmapItem->GetChildren().Append(bitmapSpec);
                }
            }
        }
        bitmapExpr = bitmapExpr->GetNext();
    }

    return bitmapItem;
}

wxItemResource *wxResourceInterpretIcon(wxResourceTable& table, wxExpr *expr)
{
    wxItemResource *item = wxResourceInterpretBitmap(table, expr);
    if (item)
    {
        //    item->SetType(wxTYPE_ICON);
        item->SetType(wxT("wxIcon"));
        return item;
    }
    else
        return (wxItemResource *) NULL;
}

// Interpret list expression as a font
wxFont wxResourceInterpretFontSpec(wxExpr *expr)
{
    if (expr->Type() != PrologList)
        return wxNullFont;

    int point = 10;
    int family = wxSWISS;
    int style = wxNORMAL;
    int weight = wxNORMAL;
    int underline = 0;
    wxString faceName;

    wxExpr *pointExpr = expr->Nth(0);
    wxExpr *familyExpr = expr->Nth(1);
    wxExpr *styleExpr = expr->Nth(2);
    wxExpr *weightExpr = expr->Nth(3);
    wxExpr *underlineExpr = expr->Nth(4);
    wxExpr *faceNameExpr = expr->Nth(5);
    if (pointExpr)
        point = (int)pointExpr->IntegerValue();

    wxString str;
    if (familyExpr)
    {
        str = familyExpr->StringValue();
        family = (int)wxParseWindowStyle(str);
    }
    if (styleExpr)
    {
        str = styleExpr->StringValue();
        style = (int)wxParseWindowStyle(str);
    }
    if (weightExpr)
    {
        str = weightExpr->StringValue();
        weight = (int)wxParseWindowStyle(str);
    }
    if (underlineExpr)
        underline = (int)underlineExpr->IntegerValue();
    if (faceNameExpr)
        faceName = faceNameExpr->StringValue();

    return *wxTheFontList->FindOrCreateFont(point, family, style, weight,
                                            (underline != 0), faceName);
}

/*
* (Re)allocate buffer for reading in from resource file
*/

bool wxReallocateResourceBuffer()
{
    if (!wxResourceBuffer)
    {
        wxResourceBufferSize = 1000;
        wxResourceBuffer = new char[wxResourceBufferSize];
        return true;
    }
    if (wxResourceBuffer)
    {
        long newSize = wxResourceBufferSize + 1000;
        char *tmp = new char[(int)newSize];
        strncpy(tmp, wxResourceBuffer, (int)wxResourceBufferCount);
        delete[] wxResourceBuffer;
        wxResourceBuffer = tmp;
        wxResourceBufferSize = newSize;
    }
    return true;
}

static bool wxEatWhiteSpace(FILE *fd)
{
    int ch;

    while ((ch = getc(fd)) != EOF)
    {
        switch (ch)
        {
        case ' ':
        case 0x0a:
        case 0x0d:
        case 0x09:
            break;
        case '/':
            {
                int prev_ch = ch;
                ch = getc(fd);
                if (ch == EOF)
                {
                    ungetc(prev_ch, fd);
                    return true;
                }

                if (ch == '*')
                {
                    // Eat C comment
                    prev_ch = 0;
                    while ((ch = getc(fd)) != EOF)
                    {
                        if (ch == '/' && prev_ch == '*')
                            break;
                        prev_ch = ch;
                    }
                }
                else if (ch == '/')
                {
                    // Eat C++ comment
                    static char buffer[255];
                    fgets(buffer, 255, fd);
                }
                else
                {
                    ungetc(prev_ch, fd);
                    ungetc(ch, fd);
                    return true;
                }
            }
            break;
        default:
            ungetc(ch, fd);
            return true;

        }
    }
    return false;
}
static bool wxEatWhiteSpace(wxInputStream *is)
{
    char ch = is->GetC() ;
    if ((ch != ' ') && (ch != '/') && (ch != ' ') && (ch != 10) && (ch != 13) && (ch != 9))
    {
        is->Ungetch(ch);
        return true;
    }

    // Eat whitespace
    while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)
        ch = is->GetC();
    // Check for comment
    if (ch == '/')
    {
        ch = is->GetC();
        if (ch == '*')
        {
            bool finished = false;
            while (!finished)
            {
                ch = is->GetC();
                if (is->LastRead() == 0)
                    return false;
                if (ch == '*')
                {
                    int newCh = is->GetC();
                    if (newCh == '/')
                        finished = true;
                    else
                    {
                        is->Ungetch(ch);
                    }
                }
            }
        }
        else // False alarm
            return false;
    }
    else
        is->Ungetch(ch);
    return wxEatWhiteSpace(is);
}

bool wxGetResourceToken(FILE *fd)
{
    if (!wxResourceBuffer)
        wxReallocateResourceBuffer();
    wxResourceBuffer[0] = 0;
    wxEatWhiteSpace(fd);

    int ch = getc(fd);
    if (ch == '"')
    {
        // Get string
        wxResourceBufferCount = 0;
        ch = getc(fd);
        while (ch != '"')
        {
            int actualCh = ch;
            if (ch == EOF)
            {
                wxResourceBuffer[wxResourceBufferCount] = 0;
                return false;
            }
            // Escaped characters
            else if (ch == '\\')
            {
                int newCh = getc(fd);
                if (newCh == '"')
                    actualCh = '"';
                else if (newCh == 10)
                    actualCh = 10;
                else
                {
                    ungetc(newCh, fd);
                }
            }

            if (wxResourceBufferCount >= wxResourceBufferSize-1)
                wxReallocateResourceBuffer();
            wxResourceBuffer[wxResourceBufferCount] = (char)actualCh;
            wxResourceBufferCount ++;
            ch = getc(fd);
        }
        wxResourceBuffer[wxResourceBufferCount] = 0;
    }
    else
    {
        wxResourceBufferCount = 0;
        // Any other token
        while (ch != ' ' && ch != EOF && ch != ' ' && ch != 13 && ch != 9 && ch != 10)
        {
            if (wxResourceBufferCount >= wxResourceBufferSize-1)
                wxReallocateResourceBuffer();
            wxResourceBuffer[wxResourceBufferCount] = (char)ch;
            wxResourceBufferCount ++;

            ch = getc(fd);
        }
        wxResourceBuffer[wxResourceBufferCount] = 0;
        if (ch == EOF)
            return false;
    }
    return true;
}

bool wxGetResourceToken(wxInputStream *is)
{
    if (!wxResourceBuffer)
        wxReallocateResourceBuffer();
    wxResourceBuffer[0] = 0;
    wxEatWhiteSpace(is);

    int ch = is->GetC() ;
    if (ch == '"')
    {
        // Get string
        wxResourceBufferCount = 0;
        ch = is->GetC();
        while (ch != '"')
        {
            int actualCh = ch;
            if (ch == EOF)
            {
                wxResourceBuffer[wxResourceBufferCount] = 0;
                return false;
            }
            // Escaped characters
            else if (ch == '\\')
            {
                char newCh = is->GetC();
                if (newCh == '"')
                    actualCh = '"';
                else if (newCh == 10)
                    actualCh = 10;
                else if (newCh == 13) // mac
                    actualCh = 10;
                else
                {
                    is->Ungetch(newCh);
                }
            }

            if (wxResourceBufferCount >= wxResourceBufferSize-1)
                wxReallocateResourceBuffer();
            wxResourceBuffer[wxResourceBufferCount] = (char)actualCh;
            wxResourceBufferCount ++;
            ch = is->GetC();
        }
        wxResourceBuffer[wxResourceBufferCount] = 0;
    }
    else
    {
        wxResourceBufferCount = 0;
        // Any other token
        while (ch != ' ' && ch != EOF && ch != ' ' && ch != 13 && ch != 9 && ch != 10)
        {
            if (wxResourceBufferCount >= wxResourceBufferSize-1)
                wxReallocateResourceBuffer();
            wxResourceBuffer[wxResourceBufferCount] = (char)ch;
            wxResourceBufferCount ++;

            ch = is->GetC();
        }
        wxResourceBuffer[wxResourceBufferCount] = 0;
        if (ch == EOF)
            return false;
    }
    return true;
}

/*
* Files are in form:
static char *name = "....";
with possible comments.
*/

bool wxResourceReadOneResource(FILE *fd, wxExprDatabase& db, bool *eof, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    // static or #define
    if (!wxGetResourceToken(fd))
    {
        *eof = true;
        return false;
    }

    if (strcmp(wxResourceBuffer, "#define") == 0)
    {
        wxGetResourceToken(fd);
        wxChar *name = copystring(wxConvCurrent->cMB2WX(wxResourceBuffer));
        wxGetResourceToken(fd);
        wxChar *value = copystring(wxConvCurrent->cMB2WX(wxResourceBuffer));
        if (wxIsdigit(value[0]))
        {
            int val = (int)wxAtol(value);
            wxResourceAddIdentifier(name, val, table);
        }
        else
        {
            wxLogWarning(_("#define %s must be an integer."), name);
            delete[] name;
            delete[] value;
            return false;
        }
        delete[] name;
        delete[] value;

        return true;
    }
    else if (strcmp(wxResourceBuffer, "#include") == 0)
    {
        wxGetResourceToken(fd);
        wxChar *name = copystring(wxConvCurrent->cMB2WX(wxResourceBuffer));
        wxChar *actualName = name;
        if (name[0] == wxT('"'))
            actualName = name + 1;
        int len = wxStrlen(name);
        if ((len > 0) && (name[len-1] == wxT('"')))
            name[len-1] = 0;
        if (!wxResourceParseIncludeFile(actualName, table))
        {
            wxLogWarning(_("Could not find resource include file %s."), actualName);
        }
        delete[] name;
        return true;
    }
    else if (strcmp(wxResourceBuffer, "static") != 0)
    {
        wxChar buf[300];
        wxStrcpy(buf, _("Found "));
        wxStrncat(buf, wxConvCurrent->cMB2WX(wxResourceBuffer), 30);
        wxStrcat(buf, _(", expected static, #include or #define\nwhile parsing resource."));
        wxLogWarning(buf);
        return false;
    }

    // char
    if (!wxGetResourceToken(fd))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }

    if (strcmp(wxResourceBuffer, "char") != 0)
    {
        wxLogWarning(_("Expected 'char' while parsing resource."));
        return false;
    }

    // *name
    if (!wxGetResourceToken(fd))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }

    if (wxResourceBuffer[0] != '*')
    {
        wxLogWarning(_("Expected '*' while parsing resource."));
        return false;
    }
    wxChar nameBuf[100];
    wxMB2WX(nameBuf, wxResourceBuffer+1, 99);
    nameBuf[99] = 0;

    // =
    if (!wxGetResourceToken(fd))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }

    if (strcmp(wxResourceBuffer, "=") != 0)
    {
        wxLogWarning(_("Expected '=' while parsing resource."));
        return false;
    }

    // String
    if (!wxGetResourceToken(fd))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }
    else
    {
        if (!db.ReadPrologFromString(wxResourceBuffer))
        {
            wxLogWarning(_("%s: ill-formed resource file syntax."), nameBuf);
            return false;
        }
    }
    // Semicolon
    if (!wxGetResourceToken(fd))
    {
        *eof = true;
    }
    return true;
}

bool wxResourceReadOneResource(wxInputStream *fd, wxExprDatabase& db, bool *eof, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    // static or #define
    if (!wxGetResourceToken(fd))
    {
        *eof = true;
        return false;
    }

    if (strcmp(wxResourceBuffer, "#define") == 0)
    {
        wxGetResourceToken(fd);
        wxChar *name = copystring(wxConvLibc.cMB2WX(wxResourceBuffer));
        wxGetResourceToken(fd);
        wxChar *value = copystring(wxConvLibc.cMB2WX(wxResourceBuffer));
        if (wxIsalpha(value[0]))
        {
            int val = (int)wxAtol(value);
            wxResourceAddIdentifier(name, val, table);
        }
        else
        {
            wxLogWarning(_("#define %s must be an integer."), name);
            delete[] name;
            delete[] value;
            return false;
        }
        delete[] name;
        delete[] value;

        return true;
    }
    else if (strcmp(wxResourceBuffer, "#include") == 0)
    {
        wxGetResourceToken(fd);
        wxChar *name = copystring(wxConvLibc.cMB2WX(wxResourceBuffer));
        wxChar *actualName = name;
        if (name[0] == wxT('"'))
            actualName = name + 1;
        int len = wxStrlen(name);
        if ((len > 0) && (name[len-1] == wxT('"')))
            name[len-1] = 0;
        if (!wxResourceParseIncludeFile(actualName, table))
        {
            wxLogWarning(_("Could not find resource include file %s."), actualName);
        }
        delete[] name;
        return true;
    }
    else if (strcmp(wxResourceBuffer, "static") != 0)
    {
        wxChar buf[300];
        wxStrcpy(buf, _("Found "));
        wxStrncat(buf, wxConvLibc.cMB2WX(wxResourceBuffer), 30);
        wxStrcat(buf, _(", expected static, #include or #define\nwhile parsing resource."));
        wxLogWarning(buf);
        return false;
    }

    // char
    if (!wxGetResourceToken(fd))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }

    if (strcmp(wxResourceBuffer, "char") != 0)
    {
        wxLogWarning(_("Expected 'char' while parsing resource."));
        return false;
    }

    // *name
    if (!wxGetResourceToken(fd))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }

    if (wxResourceBuffer[0] != '*')
    {
        wxLogWarning(_("Expected '*' while parsing resource."));
        return false;
    }
    char nameBuf[100];
    strncpy(nameBuf, wxResourceBuffer+1, 99);

    // =
    if (!wxGetResourceToken(fd))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }

    if (strcmp(wxResourceBuffer, "=") != 0)
    {
        wxLogWarning(_("Expected '=' while parsing resource."));
        return false;
    }

    // String
    if (!wxGetResourceToken(fd))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }
    else
    {
        if (!db.ReadPrologFromString(wxResourceBuffer))
        {
            wxLogWarning(_("%s: ill-formed resource file syntax."), nameBuf);
            return false;
        }
    }
    // Semicolon
    if (!wxGetResourceToken(fd))
    {
        *eof = true;
    }
    return true;
}

/*
* Parses string window style into integer window style
*/

/*
* Style flag parsing, e.g.
* "wxSYSTEM_MENU | wxBORDER" -> integer
*/

wxChar* wxResourceParseWord(wxChar*s, int *i)
{
    if (!s)
        return (wxChar*) NULL;

    static wxChar buf[150];
    int len = wxStrlen(s);
    int j = 0;
    int ii = *i;
    while ((ii < len) && (wxIsalpha(s[ii]) || (s[ii] == wxT('_'))))
    {
        buf[j] = s[ii];
        j ++;
        ii ++;
    }
    buf[j] = 0;

    // Eat whitespace and conjunction characters
    while ((ii < len) &&
        ((s[ii] == wxT(' ')) || (s[ii] == wxT('|')) || (s[ii] == wxT(','))))
    {
        ii ++;
    }
    *i = ii;
    if (j == 0)
        return (wxChar*) NULL;
    else
        return buf;
}

struct wxResourceBitListStruct
{
    const wxChar *word;
    long bits;
};

static wxResourceBitListStruct wxResourceBitListTable[] =
{
    /* wxListBox */
    { wxT("wxSINGLE"), wxLB_SINGLE },
    { wxT("wxMULTIPLE"), wxLB_MULTIPLE },
    { wxT("wxEXTENDED"), wxLB_EXTENDED },
    { wxT("wxLB_SINGLE"), wxLB_SINGLE },
    { wxT("wxLB_MULTIPLE"), wxLB_MULTIPLE },
    { wxT("wxLB_EXTENDED"), wxLB_EXTENDED },
    { wxT("wxLB_NEEDED_SB"), wxLB_NEEDED_SB },
    { wxT("wxLB_ALWAYS_SB"), wxLB_ALWAYS_SB },
    { wxT("wxLB_SORT"), wxLB_SORT },
    { wxT("wxLB_OWNERDRAW"), wxLB_OWNERDRAW },
    { wxT("wxLB_HSCROLL"), wxLB_HSCROLL },

    /* wxComboxBox */
    { wxT("wxCB_SIMPLE"), wxCB_SIMPLE },
    { wxT("wxCB_DROPDOWN"), wxCB_DROPDOWN },
    { wxT("wxCB_READONLY"), wxCB_READONLY },
    { wxT("wxCB_SORT"), wxCB_SORT },

    /* wxGauge */
#if WXWIN_COMPATIBILITY_2_6
    { wxT("wxGA_PROGRESSBAR"), wxGA_PROGRESSBAR },
#endif // WXWIN_COMPATIBILITY_2_6
    { wxT("wxGA_HORIZONTAL"), wxGA_HORIZONTAL },
    { wxT("wxGA_VERTICAL"), wxGA_VERTICAL },

    /* wxTextCtrl */
#if WXWIN_COMPATIBILITY_2_6
    { wxT("wxPASSWORD"), wxTE_PASSWORD},
    { wxT("wxPROCESS_ENTER"), wxTE_PROCESS_ENTER},
#endif
    { wxT("wxTE_PASSWORD"), wxTE_PASSWORD},
    { wxT("wxTE_READONLY"), wxTE_READONLY},
    { wxT("wxTE_PROCESS_ENTER"), wxTE_PROCESS_ENTER},
    { wxT("wxTE_MULTILINE"), wxTE_MULTILINE},
    { wxT("wxTE_NO_VSCROLL"), wxTE_NO_VSCROLL},

    /* wxRadioBox/wxRadioButton */
    { wxT("wxRB_GROUP"), wxRB_GROUP },
    { wxT("wxRA_SPECIFY_COLS"), wxRA_SPECIFY_COLS },
    { wxT("wxRA_SPECIFY_ROWS"), wxRA_SPECIFY_ROWS },
    { wxT("wxRA_HORIZONTAL"), wxRA_HORIZONTAL },
    { wxT("wxRA_VERTICAL"), wxRA_VERTICAL },

    /* wxSlider */
    { wxT("wxSL_HORIZONTAL"), wxSL_HORIZONTAL },
    { wxT("wxSL_VERTICAL"), wxSL_VERTICAL },
    { wxT("wxSL_AUTOTICKS"), wxSL_AUTOTICKS },
    { wxT("wxSL_LABELS"), wxSL_LABELS },
    { wxT("wxSL_LEFT"), wxSL_LEFT },
    { wxT("wxSL_TOP"), wxSL_TOP },
    { wxT("wxSL_RIGHT"), wxSL_RIGHT },
    { wxT("wxSL_BOTTOM"), wxSL_BOTTOM },
    { wxT("wxSL_BOTH"), wxSL_BOTH },
    { wxT("wxSL_SELRANGE"), wxSL_SELRANGE },

    /* wxScrollBar */
    { wxT("wxSB_HORIZONTAL"), wxSB_HORIZONTAL },
    { wxT("wxSB_VERTICAL"), wxSB_VERTICAL },

    /* wxButton */
    { wxT("wxBU_AUTODRAW"), wxBU_AUTODRAW },
    { wxT("wxBU_NOAUTODRAW"), wxBU_NOAUTODRAW },

    /* wxTreeCtrl */
    { wxT("wxTR_HAS_BUTTONS"), wxTR_HAS_BUTTONS },
    { wxT("wxTR_EDIT_LABELS"), wxTR_EDIT_LABELS },
    { wxT("wxTR_LINES_AT_ROOT"), wxTR_LINES_AT_ROOT },

    /* wxListCtrl */
    { wxT("wxLC_ICON"), wxLC_ICON },
    { wxT("wxLC_SMALL_ICON"), wxLC_SMALL_ICON },
    { wxT("wxLC_LIST"), wxLC_LIST },
    { wxT("wxLC_REPORT"), wxLC_REPORT },
    { wxT("wxLC_ALIGN_TOP"), wxLC_ALIGN_TOP },
    { wxT("wxLC_ALIGN_LEFT"), wxLC_ALIGN_LEFT },
    { wxT("wxLC_AUTOARRANGE"), wxLC_AUTOARRANGE },
    { wxT("wxLC_USER_TEXT"), wxLC_USER_TEXT },
    { wxT("wxLC_EDIT_LABELS"), wxLC_EDIT_LABELS },
    { wxT("wxLC_NO_HEADER"), wxLC_NO_HEADER },
    { wxT("wxLC_NO_SORT_HEADER"), wxLC_NO_SORT_HEADER },
    { wxT("wxLC_SINGLE_SEL"), wxLC_SINGLE_SEL },
    { wxT("wxLC_SORT_ASCENDING"), wxLC_SORT_ASCENDING },
    { wxT("wxLC_SORT_DESCENDING"), wxLC_SORT_DESCENDING },

    /* wxSpinButton */
    { wxT("wxSP_VERTICAL"), wxSP_VERTICAL},
    { wxT("wxSP_HORIZONTAL"), wxSP_HORIZONTAL},
    { wxT("wxSP_ARROW_KEYS"), wxSP_ARROW_KEYS},
    { wxT("wxSP_WRAP"), wxSP_WRAP},

    /* wxSplitterWnd */
    { wxT("wxSP_NOBORDER"), wxSP_NOBORDER},
    { wxT("wxSP_3D"), wxSP_3D},
    { wxT("wxSP_BORDER"), wxSP_BORDER},

    /* wxTabCtrl */
    { wxT("wxTC_MULTILINE"), wxTC_MULTILINE},
    { wxT("wxTC_RIGHTJUSTIFY"), wxTC_RIGHTJUSTIFY},
    { wxT("wxTC_FIXEDWIDTH"), wxTC_FIXEDWIDTH},
    { wxT("wxTC_OWNERDRAW"), wxTC_OWNERDRAW},

    /* wxStatusBar95 */
    { wxT("wxST_SIZEGRIP"), wxST_SIZEGRIP},

    /* wxControl */
    { wxT("wxFIXED_LENGTH"), wxFIXED_LENGTH},
    { wxT("wxALIGN_LEFT"), wxALIGN_LEFT},
    { wxT("wxALIGN_CENTER"), wxALIGN_CENTER},
    { wxT("wxALIGN_CENTRE"), wxALIGN_CENTRE},
    { wxT("wxALIGN_RIGHT"), wxALIGN_RIGHT},
    { wxT("wxCOLOURED"), wxCOLOURED},

    /* wxToolBar */
    { wxT("wxTB_3DBUTTONS"), wxTB_3DBUTTONS},
    { wxT("wxTB_HORIZONTAL"), wxTB_HORIZONTAL},
    { wxT("wxTB_VERTICAL"), wxTB_VERTICAL},
    { wxT("wxTB_FLAT"), wxTB_FLAT},

#if WXWIN_COMPATIBILITY_2_6
    /* wxDialog */
    { wxT("wxDIALOG_MODAL"), wxDIALOG_MODAL },
#endif // WXWIN_COMPATIBILITY_2_6

    /* Generic */
    { wxT("wxVSCROLL"), wxVSCROLL },
    { wxT("wxHSCROLL"), wxHSCROLL },
    { wxT("wxCAPTION"), wxCAPTION },
    { wxT("wxSTAY_ON_TOP"), wxSTAY_ON_TOP},
    { wxT("wxICONIZE"), wxICONIZE},
    { wxT("wxMINIMIZE"), wxICONIZE},
    { wxT("wxMAXIMIZE"), wxMAXIMIZE},
    { wxT("wxSDI"), 0},
    { wxT("wxMDI_PARENT"), 0},
    { wxT("wxMDI_CHILD"), 0},
    { wxT("wxTHICK_FRAME"), wxRESIZE_BORDER},
    { wxT("wxRESIZE_BORDER"), wxRESIZE_BORDER},
    { wxT("wxSYSTEM_MENU"), wxSYSTEM_MENU},
    { wxT("wxMINIMIZE_BOX"), wxMINIMIZE_BOX},
    { wxT("wxMAXIMIZE_BOX"), wxMAXIMIZE_BOX},
#if WXWIN_COMPATIBILITY_2_6
    { wxT("wxRESIZE_BOX"), wxRESIZE_BOX},
#endif // WXWIN_COMPATIBILITY_2_6
    { wxT("wxDEFAULT_FRAME_STYLE"), wxDEFAULT_FRAME_STYLE},
    { wxT("wxDEFAULT_FRAME"), wxDEFAULT_FRAME_STYLE},
    { wxT("wxDEFAULT_DIALOG_STYLE"), wxDEFAULT_DIALOG_STYLE},
    { wxT("wxBORDER"), wxBORDER},
    { wxT("wxRETAINED"), wxRETAINED},
    { wxT("wxNATIVE_IMPL"), 0},
    { wxT("wxEXTENDED_IMPL"), 0},
    { wxT("wxBACKINGSTORE"), wxBACKINGSTORE},
    //  { wxT("wxFLAT"), wxFLAT},
    //  { wxT("wxMOTIF_RESIZE"), wxMOTIF_RESIZE},
    { wxT("wxFIXED_LENGTH"), 0},
    { wxT("wxDOUBLE_BORDER"), wxDOUBLE_BORDER},
    { wxT("wxSUNKEN_BORDER"), wxSUNKEN_BORDER},
    { wxT("wxRAISED_BORDER"), wxRAISED_BORDER},
    { wxT("wxSIMPLE_BORDER"), wxSIMPLE_BORDER},
    { wxT("wxSTATIC_BORDER"), wxSTATIC_BORDER},
    { wxT("wxTRANSPARENT_WINDOW"), wxTRANSPARENT_WINDOW},
    { wxT("wxNO_BORDER"), wxNO_BORDER},
    { wxT("wxCLIP_CHILDREN"), wxCLIP_CHILDREN},
    { wxT("wxCLIP_SIBLINGS"), wxCLIP_SIBLINGS},
    { wxT("wxTAB_TRAVERSAL"), 0}, // Compatibility only

    { wxT("wxTINY_CAPTION_HORIZ"), wxTINY_CAPTION_HORIZ},
    { wxT("wxTINY_CAPTION_VERT"), wxTINY_CAPTION_VERT},

    // Text font families
    { wxT("wxDEFAULT"), wxDEFAULT},
    { wxT("wxDECORATIVE"), wxDECORATIVE},
    { wxT("wxROMAN"), wxROMAN},
    { wxT("wxSCRIPT"), wxSCRIPT},
    { wxT("wxSWISS"), wxSWISS},
    { wxT("wxMODERN"), wxMODERN},
    { wxT("wxTELETYPE"), wxTELETYPE},
    { wxT("wxVARIABLE"), wxVARIABLE},
    { wxT("wxFIXED"), wxFIXED},
    { wxT("wxNORMAL"), wxNORMAL},
    { wxT("wxLIGHT"), wxLIGHT},
    { wxT("wxBOLD"), wxBOLD},
    { wxT("wxITALIC"), wxITALIC},
    { wxT("wxSLANT"), wxSLANT},
    { wxT("wxSOLID"), wxSOLID},
    { wxT("wxDOT"), wxDOT},
    { wxT("wxLONG_DASH"), wxLONG_DASH},
    { wxT("wxSHORT_DASH"), wxSHORT_DASH},
    { wxT("wxDOT_DASH"), wxDOT_DASH},
    { wxT("wxUSER_DASH"), wxUSER_DASH},
    { wxT("wxTRANSPARENT"), wxTRANSPARENT},
    { wxT("wxSTIPPLE"), wxSTIPPLE},
    { wxT("wxBDIAGONAL_HATCH"), wxBDIAGONAL_HATCH},
    { wxT("wxCROSSDIAG_HATCH"), wxCROSSDIAG_HATCH},
    { wxT("wxFDIAGONAL_HATCH"), wxFDIAGONAL_HATCH},
    { wxT("wxCROSS_HATCH"), wxCROSS_HATCH},
    { wxT("wxHORIZONTAL_HATCH"), wxHORIZONTAL_HATCH},
    { wxT("wxVERTICAL_HATCH"), wxVERTICAL_HATCH},
    { wxT("wxJOIN_BEVEL"), wxJOIN_BEVEL},
    { wxT("wxJOIN_MITER"), wxJOIN_MITER},
    { wxT("wxJOIN_ROUND"), wxJOIN_ROUND},
    { wxT("wxCAP_ROUND"), wxCAP_ROUND},
    { wxT("wxCAP_PROJECTING"), wxCAP_PROJECTING},
    { wxT("wxCAP_BUTT"), wxCAP_BUTT},

    // Logical ops
    { wxT("wxCLEAR"), wxCLEAR},
    { wxT("wxXOR"), wxXOR},
    { wxT("wxINVERT"), wxINVERT},
    { wxT("wxOR_REVERSE"), wxOR_REVERSE},
    { wxT("wxAND_REVERSE"), wxAND_REVERSE},
    { wxT("wxCOPY"), wxCOPY},
    { wxT("wxAND"), wxAND},
    { wxT("wxAND_INVERT"), wxAND_INVERT},
    { wxT("wxNO_OP"), wxNO_OP},
    { wxT("wxNOR"), wxNOR},
    { wxT("wxEQUIV"), wxEQUIV},
    { wxT("wxSRC_INVERT"), wxSRC_INVERT},
    { wxT("wxOR_INVERT"), wxOR_INVERT},
    { wxT("wxNAND"), wxNAND},
    { wxT("wxOR"), wxOR},
    { wxT("wxSET"), wxSET},

    { wxT("wxFLOOD_SURFACE"), wxFLOOD_SURFACE},
    { wxT("wxFLOOD_BORDER"), wxFLOOD_BORDER},
    { wxT("wxODDEVEN_RULE"), wxODDEVEN_RULE},
    { wxT("wxWINDING_RULE"), wxWINDING_RULE},
    { wxT("wxHORIZONTAL"), wxHORIZONTAL},
    { wxT("wxVERTICAL"), wxVERTICAL},
    { wxT("wxBOTH"), wxBOTH},
    { wxT("wxCENTER_FRAME"), wxCENTER_FRAME},
    { wxT("wxOK"), wxOK},
    { wxT("wxYES_NO"), wxYES_NO},
    { wxT("wxCANCEL"), wxCANCEL},
    { wxT("wxYES"), wxYES},
    { wxT("wxNO"), wxNO},
    { wxT("wxICON_EXCLAMATION"), wxICON_EXCLAMATION},
    { wxT("wxICON_HAND"), wxICON_HAND},
    { wxT("wxICON_QUESTION"), wxICON_QUESTION},
    { wxT("wxICON_INFORMATION"), wxICON_INFORMATION},
    { wxT("wxICON_STOP"), wxICON_STOP},
    { wxT("wxICON_ASTERISK"), wxICON_ASTERISK},
    { wxT("wxICON_MASK"), wxICON_MASK},
    { wxT("wxCENTRE"), wxCENTRE},
    { wxT("wxCENTER"), wxCENTRE},
#if WXWIN_COMPATIBILITY_2_6
    { wxT("wxUSER_COLOURS"), wxUSER_COLOURS},
#endif // WXWIN_COMPATIBILITY_2_6
    { wxT("wxVERTICAL_LABEL"), 0},
    { wxT("wxHORIZONTAL_LABEL"), 0},

    // Bitmap types (not strictly styles)
    { wxT("wxBITMAP_TYPE_XPM"), wxBITMAP_TYPE_XPM},
    { wxT("wxBITMAP_TYPE_XBM"), wxBITMAP_TYPE_XBM},
    { wxT("wxBITMAP_TYPE_BMP"), wxBITMAP_TYPE_BMP},
    { wxT("wxBITMAP_TYPE_RESOURCE"), wxBITMAP_TYPE_BMP_RESOURCE},
    { wxT("wxBITMAP_TYPE_BMP_RESOURCE"), wxBITMAP_TYPE_BMP_RESOURCE},
    { wxT("wxBITMAP_TYPE_GIF"), wxBITMAP_TYPE_GIF},
    { wxT("wxBITMAP_TYPE_TIF"), wxBITMAP_TYPE_TIF},
    { wxT("wxBITMAP_TYPE_ICO"), wxBITMAP_TYPE_ICO},
    { wxT("wxBITMAP_TYPE_ICO_RESOURCE"), wxBITMAP_TYPE_ICO_RESOURCE},
    { wxT("wxBITMAP_TYPE_CUR"), wxBITMAP_TYPE_CUR},
    { wxT("wxBITMAP_TYPE_CUR_RESOURCE"), wxBITMAP_TYPE_CUR_RESOURCE},
    { wxT("wxBITMAP_TYPE_XBM_DATA"), wxBITMAP_TYPE_XBM_DATA},
    { wxT("wxBITMAP_TYPE_XPM_DATA"), wxBITMAP_TYPE_XPM_DATA},
    { wxT("wxBITMAP_TYPE_ANY"), wxBITMAP_TYPE_ANY}
};

static int wxResourceBitListCount = (sizeof(wxResourceBitListTable)/sizeof(wxResourceBitListStruct));

long wxParseWindowStyle(const wxString& bitListString)
{
    int i = 0;
    wxChar *word;
    long bitList = 0;
    word = wxResourceParseWord(WXSTRINGCAST bitListString, &i);
    while (word != NULL)
    {
        bool found = false;
        int j;
        for (j = 0; j < wxResourceBitListCount; j++)
            if (wxStrcmp(wxResourceBitListTable[j].word, word) == 0)
            {
                bitList |= wxResourceBitListTable[j].bits;
                found = true;
                break;
            }
            if (!found)
            {
                wxLogWarning(_("Unrecognized style %s while parsing resource."), word);
                return 0;
            }
            word = wxResourceParseWord(WXSTRINGCAST bitListString, &i);
    }
    return bitList;
}

/*
* Load a bitmap from a wxWidgets resource, choosing an optimum
* depth and appropriate type.
*/

wxBitmap wxResourceCreateBitmap(const wxString& resource, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    wxItemResource *item = table->FindResource(resource);
    if (item)
    {
        if ((item->GetType().empty()) || (item->GetType() != wxT("wxBitmap")))
        {
            wxLogWarning(_("%s not a bitmap resource specification."), (const wxChar*) resource);
            return wxNullBitmap;
        }
        int thisDepth = wxDisplayDepth();
        long thisNoColours = (long)pow(2.0, (double)thisDepth);

        wxItemResource *optResource = (wxItemResource *) NULL;

        // Try to find optimum bitmap for this platform/colour depth
        wxNode *node = item->GetChildren().GetFirst();
        while (node)
        {
            wxItemResource *child = (wxItemResource *)node->GetData();
            int platform = (int)child->GetValue2();
            int noColours = (int)child->GetValue3();
            /*
            char *name = child->GetName();
            int bitmapType = (int)child->GetValue1();
            int xRes = child->GetWidth();
            int yRes = child->GetHeight();
            */

            switch (platform)
            {
            case RESOURCE_PLATFORM_ANY:
                {
                    if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
                        optResource = child;
                    else
                    {
                        // Maximise the number of colours.
                        // If noColours is zero (unspecified), then assume this
                        // is the right one.
                        if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
                            optResource = child;
                    }
                    break;
                }
#ifdef __WXMSW__
            case RESOURCE_PLATFORM_WINDOWS:
                {
                    if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
                        optResource = child;
                    else
                    {
                        // Maximise the number of colours
                        if ((noColours > 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
                            optResource = child;
                    }
                    break;
                }
#endif
#ifdef __WXGTK__
            case RESOURCE_PLATFORM_X:
                {
                    if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
                        optResource = child;
                    else
                    {
                        // Maximise the number of colours
                        if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
                            optResource = child;
                    }
                    break;
                }
#endif
#ifdef wx_max
            case RESOURCE_PLATFORM_MAC:
                {
                    if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
                        optResource = child;
                    else
                    {
                        // Maximise the number of colours
                        if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
                            optResource = child;
                    }
                    break;
                }
#endif
            default:
                break;
            }
            node = node->GetNext();
        }
        // If no matching resource, fail.
        if (!optResource)
            return wxNullBitmap;

        wxString name = optResource->GetName();
        int bitmapType = (int)optResource->GetValue1();
        switch (bitmapType)
        {
        case wxBITMAP_TYPE_XBM_DATA:
            {
#ifdef __WXGTK__
                wxItemResource *item = table->FindResource(name);
                if (!item)
                {
                    wxLogWarning(_("Failed to find XBM resource %s.\n"
                        "Forgot to use wxResourceLoadBitmapData?"), (const wxChar*) name);
                    return wxNullBitmap;
                }
                return wxBitmap(item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3()) ;
#else
                wxLogWarning(_("No XBM facility available!"));
                break;
#endif
            }
        case wxBITMAP_TYPE_XPM_DATA:
            {
                wxItemResource *item = table->FindResource(name);
                if (!item)
                {
                    wxLogWarning(_("Failed to find XPM resource %s.\nForgot to use wxResourceLoadBitmapData?"), (const wxChar*) name);
                    return wxNullBitmap;
                }
                return wxBitmap((char **)item->GetValue1());
            }
        default:
            {
#if defined(__WXPM__)
                return wxNullBitmap;
#else
                return wxBitmap(name, (wxBitmapType)bitmapType);
#endif
            }
        }
#ifndef __WXGTK__
        return wxNullBitmap;
#endif
  }
  else
  {
      wxLogWarning(_("Bitmap resource specification %s not found."), (const wxChar*) resource);
      return wxNullBitmap;
  }
}

/*
* Load an icon from a wxWidgets resource, choosing an optimum
* depth and appropriate type.
*/

wxIcon wxResourceCreateIcon(const wxString& resource, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    wxItemResource *item = table->FindResource(resource);
    if (item)
    {
        if ((item->GetType().empty()) || wxStrcmp(item->GetType(), wxT("wxIcon")) != 0)
        {
            wxLogWarning(_("%s not an icon resource specification."), (const wxChar*) resource);
            return wxNullIcon;
        }
        int thisDepth = wxDisplayDepth();
        long thisNoColours = (long)pow(2.0, (double)thisDepth);

        wxItemResource *optResource = (wxItemResource *) NULL;

        // Try to find optimum icon for this platform/colour depth
        wxNode *node = item->GetChildren().GetFirst();
        while (node)
        {
            wxItemResource *child = (wxItemResource *)node->GetData();
            int platform = (int)child->GetValue2();
            int noColours = (int)child->GetValue3();
            /*
            char *name = child->GetName();
            int bitmapType = (int)child->GetValue1();
            int xRes = child->GetWidth();
            int yRes = child->GetHeight();
            */

            switch (platform)
            {
            case RESOURCE_PLATFORM_ANY:
                {
                    if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
                        optResource = child;
                    else
                    {
                        // Maximise the number of colours.
                        // If noColours is zero (unspecified), then assume this
                        // is the right one.
                        if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
                            optResource = child;
                    }
                    break;
                }
#ifdef __WXMSW__
            case RESOURCE_PLATFORM_WINDOWS:
                {
                    if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
                        optResource = child;
                    else
                    {
                        // Maximise the number of colours
                        if ((noColours > 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
                            optResource = child;
                    }
                    break;
                }
#endif
#ifdef __WXGTK__
            case RESOURCE_PLATFORM_X:
                {
                    if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
                        optResource = child;
                    else
                    {
                        // Maximise the number of colours
                        if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
                            optResource = child;
                    }
                    break;
                }
#endif
#ifdef wx_max
            case RESOURCE_PLATFORM_MAC:
                {
                    if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
                        optResource = child;
                    else
                    {
                        // Maximise the number of colours
                        if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
                            optResource = child;
                    }
                    break;
                }
#endif
            default:
                break;
            }
            node = node->GetNext();
        }
        // If no matching resource, fail.
        if (!optResource)
            return wxNullIcon;

        wxString name = optResource->GetName();
        int bitmapType = (int)optResource->GetValue1();
        switch (bitmapType)
        {
        case wxBITMAP_TYPE_XBM_DATA:
            {
#ifdef __WXGTK__
                wxItemResource *item = table->FindResource(name);
                if (!item)
                {
                    wxLogWarning(_("Failed to find XBM resource %s.\n"
                        "Forgot to use wxResourceLoadIconData?"), (const wxChar*) name);
                    return wxNullIcon;
                }
                return wxIcon((const char **)item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3());
#else
                wxLogWarning(_("No XBM facility available!"));
                break;
#endif
            }
        case wxBITMAP_TYPE_XPM_DATA:
            {
                // *** XPM ICON NOT YET IMPLEMENTED IN wxWidgets ***
                /*
                wxItemResource *item = table->FindResource(name);
                if (!item)
                {
                char buf[400];
                sprintf(buf, _("Failed to find XPM resource %s.\nForgot to use wxResourceLoadIconData?"), name);
                wxLogWarning(buf);
                return NULL;
                }
                return wxIcon((char **)item->GetValue1());
                */
                wxLogWarning(_("No XPM icon facility available!"));
                break;
            }
        default:
            {
#if defined( __WXGTK__ ) || defined( __WXMOTIF__ )
                wxLogWarning(_("Icon resource specification %s not found."), (const wxChar*) resource);
                break;
#else
                return wxIcon(name, (wxBitmapType) bitmapType);
#endif
            }
        }
        return wxNullIcon;
  }
  else
  {
      wxLogWarning(_("Icon resource specification %s not found."), (const wxChar*) resource);
      return wxNullIcon;
  }
}

#if wxUSE_MENUS

wxMenu *wxResourceCreateMenu(wxItemResource *item)
{
    wxMenu *menu = new wxMenu;
    wxNode *node = item->GetChildren().GetFirst();
    while (node)
    {
        wxItemResource *child = (wxItemResource *)node->GetData();
        if ((!child->GetType().empty()) && (child->GetType() == wxT("wxMenuSeparator")))
            menu->AppendSeparator();
        else if (child->GetChildren().GetCount() > 0)
        {
            wxMenu *subMenu = wxResourceCreateMenu(child);
            if (subMenu)
                menu->Append((int)child->GetValue1(), child->GetTitle(), subMenu, child->GetValue4());
        }
        else
        {
            menu->Append((int)child->GetValue1(), child->GetTitle(), child->GetValue4(), (child->GetValue2() != 0));
        }
        node = node->GetNext();
    }
    return menu;
}

wxMenuBar *wxResourceCreateMenuBar(const wxString& resource, wxResourceTable *table, wxMenuBar *menuBar)
{
    if (!table)
        table = wxDefaultResourceTable;

    wxItemResource *menuResource = table->FindResource(resource);
    if (menuResource && (!menuResource->GetType().empty()) && (menuResource->GetType() == wxT("wxMenu")))
    {
        if (!menuBar)
            menuBar = new wxMenuBar;
        wxNode *node = menuResource->GetChildren().GetFirst();
        while (node)
        {
            wxItemResource *child = (wxItemResource *)node->GetData();
            wxMenu *menu = wxResourceCreateMenu(child);
            if (menu)
                menuBar->Append(menu, child->GetTitle());
            node = node->GetNext();
        }
        return menuBar;
    }
    return (wxMenuBar *) NULL;
}

wxMenu *wxResourceCreateMenu(const wxString& resource, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    wxItemResource *menuResource = table->FindResource(resource);
    if (menuResource && (!menuResource->GetType().empty()) && (menuResource->GetType() == wxT("wxMenu")))
        //  if (menuResource && (menuResource->GetType() == wxTYPE_MENU))
        return wxResourceCreateMenu(menuResource);
    return (wxMenu *) NULL;
}

#endif // wxUSE_MENUS

// Global equivalents (so don't have to refer to default table explicitly)
bool wxResourceParseData(const wxString& resource, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    return table->ParseResourceData(resource);
}

bool wxResourceParseData(const char* resource, wxResourceTable *table)
{
    wxString str(resource, wxConvLibc);
    if (!table)
        table = wxDefaultResourceTable;

    return table->ParseResourceData(str);
}

bool wxResourceParseFile(const wxString& filename, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    return table->ParseResourceFile(filename);
}

// Register XBM/XPM data
bool wxResourceRegisterBitmapData(const wxString& name, char bits[], int width, int height, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    return table->RegisterResourceBitmapData(name, bits, width, height);
}

bool wxResourceRegisterBitmapData(const wxString& name, char **data, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    return table->RegisterResourceBitmapData(name, data);
}

void wxResourceClear(wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    table->ClearTable();
}

/*
* Identifiers
*/

bool wxResourceAddIdentifier(const wxString& name, int value, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    table->identifiers.Put(name, (wxObject *)(long)value);
    return true;
}

int wxResourceGetIdentifier(const wxString& name, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    return (int)(long)table->identifiers.Get(name);
}

/*
* Parse #include file for #defines (only)
*/

bool wxResourceParseIncludeFile(const wxString& f, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    FILE *fd = wxFopen(f, wxT("r"));
    if (!fd)
    {
        return false;
    }
    while (wxGetResourceToken(fd))
    {
        if (strcmp(wxResourceBuffer, "#define") == 0)
        {
            wxGetResourceToken(fd);
            wxChar *name = copystring(wxConvCurrent->cMB2WX(wxResourceBuffer));
            wxGetResourceToken(fd);
            wxChar *value = copystring(wxConvCurrent->cMB2WX(wxResourceBuffer));
            if (wxIsdigit(value[0]))
            {
                int val = (int)wxAtol(value);
                wxResourceAddIdentifier(name, val, table);
            }
            delete[] name;
            delete[] value;
        }
    }
    fclose(fd);
    return true;
}

/*
* Reading strings as if they were .wxr files
*/

static int getc_string(char *s)
{
    int ch = s[wxResourceStringPtr];
    if (ch == 0)
        return EOF;
    else
    {
        wxResourceStringPtr ++;
        return ch;
    }
}

static int ungetc_string()
{
    wxResourceStringPtr --;
    return 0;
}

bool wxEatWhiteSpaceString(char *s)
{
    int ch;

    while ((ch = getc_string(s)) != EOF)
    {
        switch (ch)
        {
        case ' ':
        case 0x0a:
        case 0x0d:
        case 0x09:
            break;
        case '/':
            {
                ch = getc_string(s);
                if (ch == EOF)
                {
                    ungetc_string();
                    return true;
                }

                if (ch == '*')
                {
                    // Eat C comment
                    int prev_ch = 0;
                    while ((ch = getc_string(s)) != EOF)
                    {
                        if (ch == '/' && prev_ch == '*')
                            break;
                        prev_ch = ch;
                    }
                }
                else
                {
                    ungetc_string();
                    ungetc_string();
                    return true;
                }
            }
            break;
        default:
            ungetc_string();
            return true;

        }
    }
    return false;
}

bool wxGetResourceTokenString(char *s)
{
    if (!wxResourceBuffer)
        wxReallocateResourceBuffer();
    wxResourceBuffer[0] = 0;
    wxEatWhiteSpaceString(s);

    int ch = getc_string(s);
    if (ch == '"')
    {
        // Get string
        wxResourceBufferCount = 0;
        ch = getc_string(s);
        while (ch != '"')
        {
            int actualCh = ch;
            if (ch == EOF)
            {
                wxResourceBuffer[wxResourceBufferCount] = 0;
                return false;
            }
            // Escaped characters
            else if (ch == '\\')
            {
                int newCh = getc_string(s);
                if (newCh == '"')
                    actualCh = '"';
                else if (newCh == 10)
                    actualCh = 10;
                else
                {
                    ungetc_string();
                }
            }

            if (wxResourceBufferCount >= wxResourceBufferSize-1)
                wxReallocateResourceBuffer();
            wxResourceBuffer[wxResourceBufferCount] = (char)actualCh;
            wxResourceBufferCount ++;
            ch = getc_string(s);
        }
        wxResourceBuffer[wxResourceBufferCount] = 0;
    }
    else
    {
        wxResourceBufferCount = 0;
        // Any other token
        while (ch != ' ' && ch != EOF && ch != ' ' && ch != 13 && ch != 9 && ch != 10)
        {
            if (wxResourceBufferCount >= wxResourceBufferSize-1)
                wxReallocateResourceBuffer();
            wxResourceBuffer[wxResourceBufferCount] = (char)ch;
            wxResourceBufferCount ++;

            ch = getc_string(s);
        }
        wxResourceBuffer[wxResourceBufferCount] = 0;
        if (ch == EOF)
            return false;
    }
    return true;
}

/*
* Files are in form:
static char *name = "....";
with possible comments.
*/

bool wxResourceReadOneResourceString(char *s, wxExprDatabase& db, bool *eof, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    // static or #define
    if (!wxGetResourceTokenString(s))
    {
        *eof = true;
        return false;
    }

    if (strcmp(wxResourceBuffer, "#define") == 0)
    {
        wxGetResourceTokenString(s);
        wxChar *name = copystring(wxConvCurrent->cMB2WX(wxResourceBuffer));
        wxGetResourceTokenString(s);
        wxChar *value = copystring(wxConvCurrent->cMB2WX(wxResourceBuffer));
        if (wxIsdigit(value[0]))
        {
            int val = (int)wxAtol(value);
            wxResourceAddIdentifier(name, val, table);
        }
        else
        {
            wxLogWarning(_("#define %s must be an integer."), name);
            delete[] name;
            delete[] value;
            return false;
        }
        delete[] name;
        delete[] value;

        return true;
    }
    /*
    else if (strcmp(wxResourceBuffer, "#include") == 0)
    {
    wxGetResourceTokenString(s);
    char *name = copystring(wxResourceBuffer);
    char *actualName = name;
    if (name[0] == '"')
    actualName = name + 1;
    int len = strlen(name);
    if ((len > 0) && (name[len-1] == '"'))
    name[len-1] = 0;
    if (!wxResourceParseIncludeFile(actualName, table))
    {
    char buf[400];
    sprintf(buf, _("Could not find resource include file %s."), actualName);
    wxLogWarning(buf);
    }
    delete[] name;
    return true;
    }
    */
    else if (strcmp(wxResourceBuffer, "static") != 0)
    {
        wxChar buf[300];
        wxStrcpy(buf, _("Found "));
        wxStrncat(buf, wxConvCurrent->cMB2WX(wxResourceBuffer), 30);
        wxStrcat(buf, _(", expected static, #include or #define\nwhile parsing resource."));
        wxLogWarning(buf);
        return false;
    }

    // char
    if (!wxGetResourceTokenString(s))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }

    if (strcmp(wxResourceBuffer, "char") != 0)
    {
        wxLogWarning(_("Expected 'char' while parsing resource."));
        return false;
    }

    // *name
    if (!wxGetResourceTokenString(s))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }

    if (wxResourceBuffer[0] != '*')
    {
        wxLogWarning(_("Expected '*' while parsing resource."));
        return false;
    }
    wxChar nameBuf[100];
    wxMB2WX(nameBuf, wxResourceBuffer+1, 99);
    nameBuf[99] = 0;

    // =
    if (!wxGetResourceTokenString(s))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }

    if (strcmp(wxResourceBuffer, "=") != 0)
    {
        wxLogWarning(_("Expected '=' while parsing resource."));
        return false;
    }

    // String
    if (!wxGetResourceTokenString(s))
    {
        wxLogWarning(_("Unexpected end of file while parsing resource."));
        *eof = true;
        return false;
    }
    else
    {
        if (!db.ReadPrologFromString(wxResourceBuffer))
        {
            wxLogWarning(_("%s: ill-formed resource file syntax."), nameBuf);
            return false;
        }
    }
    // Semicolon
    if (!wxGetResourceTokenString(s))
    {
        *eof = true;
    }
    return true;
}

bool wxResourceParseString(const wxString& s, wxResourceTable *WXUNUSED(table))
{
#if wxUSE_UNICODE
    return wxResourceParseString( (char*)s.mb_str().data() );
#else
    return wxResourceParseString( (char*)s.c_str() );
#endif
}

bool wxResourceParseString(char *s, wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    if (!s)
        return false;

    // Turn backslashes into spaces
    if (s)
    {
        int len = strlen(s);
        int i;
        for (i = 0; i < len; i++)
            if (s[i] == 92 && s[i+1] == 13)
            {
                s[i] = ' ';
                s[i+1] = ' ';
            }
    }

    wxExprDatabase db;
    wxResourceStringPtr = 0;

    bool eof = false;
    while (wxResourceReadOneResourceString(s, db, &eof, table) && !eof)
    {
        // Loop
    }
    return wxResourceInterpretResources(*table, db);
}

/*
* resource loading facility
*/

bool wxLoadFromResource(wxWindow* thisWindow, wxWindow *parent, const wxString& resourceName, const wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;

    wxItemResource *resource = table->FindResource((const wxChar *)resourceName);
    //  if (!resource || (resource->GetType() != wxTYPE_DIALOG_BOX))
    if (!resource || (resource->GetType().empty()) ||
        ! ((resource->GetType() == wxT("wxDialog")) || (resource->GetType() == wxT("wxPanel"))))
        return false;

    wxString title(resource->GetTitle());
    long theWindowStyle = resource->GetStyle();
    bool isModal = (resource->GetValue1() != 0) ;
    int x = resource->GetX();
    int y = resource->GetY();
    int width = resource->GetWidth();
    int height = resource->GetHeight();
    wxString name = resource->GetName();

    // this is used for loading wxWizard pages from WXR
    if ( parent != thisWindow )
    {
        if (thisWindow->IsKindOf(CLASSINFO(wxDialog)))
        {
            wxDialog *dialogBox = (wxDialog *)thisWindow;
            long modalStyle = isModal ?
#if WXWIN_COMPATIBILITY_2_6
                                        wxDIALOG_MODAL
#else
                                        0
#endif // WXWIN_COMPATIBILITY_2_6
                                      : 0;
            if (!dialogBox->Create(parent, wxID_ANY, title, wxPoint(x, y), wxSize(width, height), theWindowStyle|modalStyle, name))
                return false;

            // Only reset the client size if we know we're not going to do it again below.
            if ((resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) == 0)
                dialogBox->SetClientSize(width, height);
        }
        else if (thisWindow->IsKindOf(CLASSINFO(wxPanel)))
        {
            wxPanel* panel = (wxPanel *)thisWindow;
            if (!panel->Create(parent, wxID_ANY, wxPoint(x, y), wxSize(width, height), theWindowStyle | wxTAB_TRAVERSAL, name))
                return false;
        }
        else
        {
            if (!((wxWindow *)thisWindow)->Create(parent, wxID_ANY, wxPoint(x, y), wxSize(width, height), theWindowStyle, name))
                return false;
        }
    }

    if ((resource->GetResourceStyle() & wxRESOURCE_USE_DEFAULTS) != 0)
    {
        // No need to do this since it's done in wxPanel or wxDialog constructor.
        // SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    }
    else
    {
        if (resource->GetFont().Ok())
            thisWindow->SetFont(resource->GetFont());
        if (resource->GetBackgroundColour().Ok())
            thisWindow->SetBackgroundColour(resource->GetBackgroundColour());
    }

    // Should have some kind of font at this point
    if (!thisWindow->GetFont().Ok())
        thisWindow->SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    if (!thisWindow->GetBackgroundColour().Ok())
        thisWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    // Only when we've created the window and set the font can we set the correct size,
    // if based on dialog units.
    if ((resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) != 0)
    {
        wxSize sz = thisWindow->ConvertDialogToPixels(wxSize(width, height));
        thisWindow->SetClientSize(sz.x, sz.y);

        wxPoint pt = thisWindow->ConvertDialogToPixels(wxPoint(x, y));
        thisWindow->Move(pt.x, pt.y);
    }

    // Now create children
    wxNode *node = resource->GetChildren().GetFirst();
    while (node)
    {
        wxItemResource *childResource = (wxItemResource *)node->GetData();

        (void) wxCreateItem(thisWindow, childResource, resource, table);

        node = node->GetNext();
    }
    return true;
}

wxControl *wxCreateItem(wxWindow* thisWindow, const wxItemResource *resource, const wxItemResource* parentResource, const wxResourceTable *table)
{
    if (!table)
        table = wxDefaultResourceTable;
    return table->CreateItem(thisWindow, resource, parentResource);
}

#ifdef __VISUALC__
#pragma warning(default:4706)   // assignment within conditional expression
#endif // VC++

#endif // wxUSE_WX_RESOURCES
