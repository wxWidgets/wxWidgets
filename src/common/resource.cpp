/////////////////////////////////////////////////////////////////////////////
// Name:        resource.cpp
// Purpose:     Resource system
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:    wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "resource.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_WX_RESOURCES

#ifdef _MSC_VER
    #pragma warning(disable:4706)   // assignment within conditional expression
#endif // VC++

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/setup.h"
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
#include "wx/radiobut.h"
#include "wx/listbox.h"
#include "wx/choice.h"
#include "wx/checkbox.h"
#include "wx/settings.h"
#include "wx/slider.h"
#include "wx/statbox.h"
#include "wx/statbmp.h"
#if wxUSE_GAUGE
#include "wx/gauge.h"
#endif
#include "wx/textctrl.h"
#include "wx/msgdlg.h"
#include "wx/intl.h"
#endif

#if wxUSE_RADIOBUTTON
#include "wx/radiobut.h"
#endif

#if wxUSE_SCROLLBAR
#include "wx/scrolbar.h"
#endif

#if wxUSE_COMBOBOX
#include "wx/combobox.h"
#endif

#include "wx/validate.h"

#include "wx/log.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "wx/resource.h"
#include "wx/string.h"
#include "wx/wxexpr.h"

#include "wx/settings.h"

// Forward (private) declarations
bool wxResourceInterpretResources(wxResourceTable& table, wxExprDatabase& db);
wxItemResource *wxResourceInterpretDialog(wxResourceTable& table, wxExpr *expr, bool isPanel = FALSE);
wxItemResource *wxResourceInterpretControl(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretMenu(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretMenuBar(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretString(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretBitmap(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretIcon(wxResourceTable& table, wxExpr *expr);
// Interpret list expression
wxFont wxResourceInterpretFontSpec(wxExpr *expr);

bool wxResourceReadOneResource(FILE *fd, wxExprDatabase& db, bool *eof, wxResourceTable *table = (wxResourceTable *) NULL);
bool wxResourceParseIncludeFile(const wxString& f, wxResourceTable *table = (wxResourceTable *) NULL);

wxResourceTable *wxDefaultResourceTable = (wxResourceTable *) NULL;

char *wxResourceBuffer = (char *) NULL;
long wxResourceBufferSize = 0;
long wxResourceBufferCount = 0;
int wxResourceStringPtr = 0;

void wxInitializeResourceSystem()
{
    wxDefaultResourceTable = new wxResourceTable;
}

void wxCleanUpResourceSystem()
{
    delete wxDefaultResourceTable;
    if (wxResourceBuffer)
        delete[] wxResourceBuffer;
}

void wxLogWarning(char *msg)
{
  wxMessageBox(msg, _("Warning"), wxOK);
}

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxItemResource, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxResourceTable, wxHashTable)
#endif

wxItemResource::wxItemResource()
{
  m_itemType = "";
  m_title = "";
  m_name = "";
  m_windowStyle = 0;
  m_x = m_y = m_width = m_height = 0;
  m_value1 = m_value2 = m_value3 = m_value5 = 0;
  m_value4 = "";
  m_windowId = 0;
  m_exStyle = 0;
}

wxItemResource::~wxItemResource()
{
  wxNode *node = m_children.First();
  while (node)
  {
    wxItemResource *item = (wxItemResource *)node->Data();
    delete item;
    delete node;
    node = m_children.First();
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
  wxItemResource *item = (wxItemResource *)Get((char *)(const char *)name);
  return item;
}

void wxResourceTable::AddResource(wxItemResource *item)
{
  wxString name = item->GetName();
  if (name == "")
    name = item->GetTitle();
  if (name == "")
    name = "no name";

  // Delete existing resource, if any.
  Delete(name);

  Put(name, item);
}

bool wxResourceTable::DeleteResource(const wxString& name)
{
  wxItemResource *item = (wxItemResource *)Delete((char *)(const char *)name);
  if (item)
  {
    // See if any resource has this as its child; if so, delete from
    // parent's child list.
    BeginFind();
    wxNode *node = (wxNode *) NULL;
    while ((node = Next()))
    {
      wxItemResource *parent = (wxItemResource *)node->Data();
      if (parent->GetChildren().Member(item))
      {
        parent->GetChildren().DeleteObject(item);
        break;
      }
    }

    delete item;
    return TRUE;
  }
  else
    return FALSE;
}

bool wxResourceTable::ParseResourceFile(const wxString& filename)
{
  wxExprDatabase db;

  FILE *fd = fopen((const char*) filename, "r");
  if (!fd)
    return FALSE;
  bool eof = FALSE;
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
    return FALSE;
  }

  return wxResourceInterpretResources(*this, db);
}

bool wxResourceTable::RegisterResourceBitmapData(const wxString& name, char bits[], int width, int height)
{
  // Register pre-loaded bitmap data
  wxItemResource *item = new wxItemResource;
//  item->SetType(wxRESOURCE_TYPE_XBM_DATA);
  item->SetType("wxXBMData");
  item->SetName(name);
  item->SetValue1((long)bits);
  item->SetValue2((long)width);
  item->SetValue3((long)height);
  AddResource(item);
  return TRUE;
}

bool wxResourceTable::RegisterResourceBitmapData(const wxString& name, char **data)
{
  // Register pre-loaded bitmap data
  wxItemResource *item = new wxItemResource;
//  item->SetType(wxRESOURCE_TYPE_XPM_DATA);
  item->SetType("wxXPMData");
  item->SetName(name);
  item->SetValue1((long)data);
  AddResource(item);
  return TRUE;
}

bool wxResourceTable::SaveResource(const wxString& WXUNUSED(filename))
{
  return FALSE;
}

void wxResourceTable::ClearTable()
{
  BeginFind();
  wxNode *node = Next();
  while (node)
  {
    wxNode *next = Next();
    wxItemResource *item = (wxItemResource *)node->Data();
    delete item;
    delete node;
    node = next;
  }
}

wxControl *wxResourceTable::CreateItem(wxWindow *parent, const wxItemResource* childResource, const wxItemResource* parentResource) const
{
  int id = childResource->GetId();
  if ( id == 0 )
   id = -1;

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

  if (itemType == wxString("wxButton") || itemType == wxString("wxBitmapButton"))
      {
        if (childResource->GetValue4() != "")
        {
          // Bitmap button
          wxBitmap bitmap = childResource->GetBitmap();
          if (!bitmap.Ok())
          {
            bitmap = wxResourceCreateBitmap(childResource->GetValue4(), (wxResourceTable *)this);
            ((wxItemResource*) childResource)->SetBitmap(bitmap);
          }
          if (bitmap.Ok())
           control = new wxBitmapButton(parent, id, bitmap, pos, size,
               childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
        }
        else
          // Normal, text button
          control = new wxButton(parent, id, childResource->GetTitle(), pos, size,
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
      }
   else if (itemType == wxString("wxMessage") || itemType == wxString("wxStaticText") ||
         itemType == wxString("wxStaticBitmap"))
      {
        if (childResource->GetValue4() != "")
        {
          // Bitmap message
          wxBitmap bitmap = childResource->GetBitmap();
          if (!bitmap.Ok())
          {
            bitmap = wxResourceCreateBitmap(childResource->GetValue4(), (wxResourceTable *)this);
            ((wxItemResource*) childResource)->SetBitmap(bitmap);
          }
#if wxUSE_BITMAP_MESSAGE
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
   else if (itemType == wxString("wxText") || itemType == wxString("wxTextCtrl") || itemType == wxString("wxMultiText"))
      {
        control = new wxTextCtrl(parent, id, childResource->GetValue4(), pos, size,
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
      }
   else if (itemType == wxString("wxCheckBox"))
      {
        control = new wxCheckBox(parent, id, childResource->GetTitle(), pos, size,
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        ((wxCheckBox *)control)->SetValue((childResource->GetValue1() != 0));
      }
#if wxUSE_GAUGE
   else if (itemType == wxString("wxGauge"))
      {
        control = new wxGauge(parent, id, (int)childResource->GetValue2(), pos, size,
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        ((wxGauge *)control)->SetValue((int)childResource->GetValue1());
      }
#endif
#if wxUSE_RADIOBUTTON
   else if (itemType == wxString("wxRadioButton"))
      {
        control = new wxRadioButton(parent, id, childResource->GetTitle(), // (int)childResource->GetValue1(),
           pos, size,
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
      }
#endif
#if wxUSE_SCROLLBAR
   else if (itemType == wxString("wxScrollBar"))
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
 			(int)childResource->GetValue3(),(int)(long)childResource->GetValue5(),FALSE);

      }
#endif
   else if (itemType == wxString("wxSlider"))
      {
        control = new wxSlider(parent, id, (int)childResource->GetValue1(),
           (int)childResource->GetValue2(), (int)childResource->GetValue3(), pos, size,
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
      }
   else if (itemType == wxString("wxGroupBox") || itemType == wxString("wxStaticBox"))
      {
        control = new wxStaticBox(parent, id, childResource->GetTitle(), pos, size,
           childResource->GetStyle(), childResource->GetName());
      }
   else if (itemType == wxString("wxListBox"))
      {
        wxStringList& stringList = childResource->GetStringValues();
        wxString *strings = (wxString *) NULL;
        int noStrings = 0;
        if (stringList.Number() > 0)
        {
          noStrings = stringList.Number();
          strings = new wxString[noStrings];
          wxNode *node = stringList.First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxListBox(parent, id, pos, size,
           noStrings, strings, childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        if (strings)
          delete[] strings;
      }
   else if (itemType == wxString("wxChoice"))
      {
        wxStringList& stringList = childResource->GetStringValues();
        wxString *strings = (wxString *) NULL;
        int noStrings = 0;
        if (stringList.Number() > 0)
        {
          noStrings = stringList.Number();
          strings = new wxString[noStrings];
          wxNode *node = stringList.First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxChoice(parent, id, pos, size,
           noStrings, strings, childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        if (strings)
          delete[] strings;
      }
#if wxUSE_COMBOBOX
   else if (itemType == wxString("wxComboBox"))
      {
        wxStringList& stringList = childResource->GetStringValues();
        wxString *strings = (wxString *) NULL;
        int noStrings = 0;
        if (stringList.Number() > 0)
        {
          noStrings = stringList.Number();
          strings = new wxString[noStrings];
          wxNode *node = stringList.First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxComboBox(parent, id, childResource->GetValue4(), pos, size,
           noStrings, strings, childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        if (strings)
          delete[] strings;
      }
#endif
   else if (itemType == wxString("wxRadioBox"))
      {
        wxStringList& stringList = childResource->GetStringValues();
        wxString *strings = (wxString *) NULL;
        int noStrings = 0;
        if (stringList.Number() > 0)
        {
          noStrings = stringList.Number();
          strings = new wxString[noStrings];
          wxNode *node = stringList.First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
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
      control->SetFont(childResource->GetFont());
  }
  return control;
}

/*
 * Interpret database as a series of resources
 */

bool wxResourceInterpretResources(wxResourceTable& table, wxExprDatabase& db)
{
  wxNode *node = db.First();
  while (node)
  {
    wxExpr *clause = (wxExpr *)node->Data();
   wxString functor(clause->Functor());

    wxItemResource *item = (wxItemResource *) NULL;
    if (functor == "dialog")
      item = wxResourceInterpretDialog(table, clause);
    else if (functor == "panel")
      item = wxResourceInterpretDialog(table, clause, TRUE);
    else if (functor == "menubar")
      item = wxResourceInterpretMenuBar(table, clause);
    else if (functor == "menu")
      item = wxResourceInterpretMenu(table, clause);
    else if (functor == "string")
      item = wxResourceInterpretString(table, clause);
    else if (functor == "bitmap")
      item = wxResourceInterpretBitmap(table, clause);
    else if (functor == "icon")
      item = wxResourceInterpretIcon(table, clause);

    if (item)
    {
      // Remove any existing resource of same name
      if (item->GetName() != "")
        table.DeleteResource(item->GetName());
      table.AddResource(item);
    }
    node = node->Next();
  }
  return TRUE;
}

static char *g_ValidControlClasses[] = { "wxButton", "wxBitmapButton", "wxMessage",
 "wxStaticText", "wxStaticBitmap", "wxText", "wxTextCtrl", "wxMultiText",
 "wxListBox", "wxRadioBox", "wxRadioButton", "wxCheckBox", "wxBitmapCheckBox",
 "wxGroupBox", "wxStaticBox", "wxSlider", "wxGauge", "wxScrollBar",
 "wxChoice", "wxComboBox" } ;
static int g_ValidControlClassesCount = sizeof(g_ValidControlClasses) / sizeof(char *) ;

static bool wxIsValidControlClass(const wxString& c)
{
   int i;
   for ( i = 0; i < g_ValidControlClassesCount; i++)
   {
      if ( c == g_ValidControlClasses[i] )
         return TRUE;
   }
   return FALSE;
}

wxItemResource *wxResourceInterpretDialog(wxResourceTable& table, wxExpr *expr, bool isPanel)
{
  wxItemResource *dialogItem = new wxItemResource;
  if (isPanel)
    dialogItem->SetType("wxPanel");
  else
    dialogItem->SetType("wxDialog");
  wxString style = "";
  wxString title = "";
  wxString name = "";
  wxString backColourHex = "";
  wxString labelColourHex = "";
  wxString buttonColourHex = "";

  long windowStyle = wxDEFAULT_DIALOG_STYLE;
  if (isPanel)
    windowStyle = 0;

  int x = 0; int y = 0; int width = -1; int height = -1;
  int isModal = 0;
  wxExpr *labelFontExpr = (wxExpr *) NULL;
  wxExpr *buttonFontExpr = (wxExpr *) NULL;
  wxExpr *fontExpr = (wxExpr *) NULL;
  expr->GetAttributeValue("style", style);
  expr->GetAttributeValue("name", name);
  expr->GetAttributeValue("title", title);
  expr->GetAttributeValue("x", x);
  expr->GetAttributeValue("y", y);
  expr->GetAttributeValue("width", width);
  expr->GetAttributeValue("height", height);
  expr->GetAttributeValue("modal", isModal);
  expr->GetAttributeValue("label_font", &labelFontExpr);
  expr->GetAttributeValue("button_font", &buttonFontExpr);
  expr->GetAttributeValue("font", &fontExpr);
  expr->GetAttributeValue("background_colour", backColourHex);
  expr->GetAttributeValue("label_colour", labelColourHex);
  expr->GetAttributeValue("button_colour", buttonColourHex);

  int useDialogUnits = 0;
  expr->GetAttributeValue("use_dialog_units", useDialogUnits);
  if (useDialogUnits != 0)
    dialogItem->SetResourceStyle(dialogItem->GetResourceStyle() | wxRESOURCE_DIALOG_UNITS);

  int useDefaults = 0;
  expr->GetAttributeValue("use_system_defaults", useDefaults);
  if (useDefaults != 0)
    dialogItem->SetResourceStyle(dialogItem->GetResourceStyle() | wxRESOURCE_USE_DEFAULTS);

  long id = 0;
  expr->GetAttributeValue("id", id);
  dialogItem->SetId(id);

  if (style != "")
  {
    windowStyle = wxParseWindowStyle(style);
  }
  dialogItem->SetStyle(windowStyle);
  dialogItem->SetValue1(isModal);
  dialogItem->SetName(name);
  dialogItem->SetTitle(title);
  dialogItem->SetSize(x, y, width, height);

  if (backColourHex != "")
  {
    int r = 0;
    int g = 0;
    int b = 0;
    r = wxHexToDec(backColourHex.Mid(0, 2));
    g = wxHexToDec(backColourHex.Mid(2, 2));
    b = wxHexToDec(backColourHex.Mid(4, 2));
    dialogItem->SetBackgroundColour(wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
  }
  if (labelColourHex != "")
  {
    int r = 0;
    int g = 0;
    int b = 0;
    r = wxHexToDec(labelColourHex.Mid(0, 2));
    g = wxHexToDec(labelColourHex.Mid(2, 2));
    b = wxHexToDec(labelColourHex.Mid(4, 2));
    dialogItem->SetLabelColour(wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
  }
  if (buttonColourHex != "")
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
      if (controlKeyword != "" && controlKeyword == "control")
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
  int x = 0; int y = 0; int width = -1; int height = -1;
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
          wxLogWarning(_("Could not resolve control class or id '%s'. "
                         "Use (non-zero) integer instead\n or provide #define "
                         "(see manual for caveats)"),
                         (const char*) expr1->StringValue());
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

  if (controlType == "wxButton")
  {
    // Check for bitmap resource name (in case loading old-style resource file)
    if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
    {
        wxString str(expr->Nth(count)->StringValue());
        controlItem->SetValue4(str);
        count ++;
        controlItem->SetType("wxBitmapButton");
    }
    if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
      controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
  }
  else if (controlType == "wxBitmapButton")
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
  else if (controlType == "wxCheckBox")
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
#if wxUSE_RADIOBUTTON
  else if (controlType == "wxRadioButton")
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
  else if (controlType == "wxText" || controlType == "wxTextCtrl" || controlType == "wxMultiText")
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
       // Do nothing - no label font any more
      count ++;
        if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
          controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
     }
   }
  }
  else if (controlType == "wxMessage" || controlType == "wxStaticText")
  {
    // Check for bitmap resource name (in case it's an old-style .wxr file)
    if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
    {
      wxString str(expr->Nth(count)->StringValue());
      controlItem->SetValue4(str);
      count ++;
      controlItem->SetType("wxStaticText");
    }
    if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
      controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
  }
  else if (controlType == "wxStaticBitmap")
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
  else if (controlType == "wxGroupBox" || controlType == "wxStaticBox")
  {
    if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
      controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
  }
  else if (controlType == "wxGauge")
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
         // controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(count)));
        // Do nothing
         count ++;

          if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
       }
     }
   }
  }
  else if (controlType == "wxSlider")
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
  else if (controlType == "wxScrollBar")
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
  else if (controlType == "wxListBox")
  {
    wxExpr *valueList = (wxExpr *) NULL;

    if ((valueList = expr->Nth(count)) && (valueList->Type() == PrologList))
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
      if ((mult = expr->Nth(count)) && ((mult->Type() == PrologString)||(mult->Type() == PrologWord)))
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
         // controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(count)));
         count ++;
         if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
           controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
      }
   }
  }
  else if (controlType == "wxChoice")
  {
    wxExpr *valueList = (wxExpr *) NULL;
    // Check for default value list
    if ((valueList = expr->Nth(count)) && (valueList->Type() == PrologList))
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
        // controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(count)));
      count ++;

        if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
          controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
     }
   }
  }
#if wxUSE_COMBOBOX
  else if (controlType == "wxComboBox")
  {
    wxExpr *textValue = expr->Nth(count);
    if (textValue && (textValue->Type() == PrologString || textValue->Type() == PrologWord))
   {
      wxString str(textValue->StringValue());
      controlItem->SetValue4(str);

     count ++;

      wxExpr *valueList = (wxExpr *) NULL;
      // Check for default value list
      if ((valueList = expr->Nth(count)) && (valueList->Type() == PrologList))
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
          // controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(count)));
        count ++;

          if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
            controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
       }
     }
   }
  }
#endif
#if 1
  else if (controlType == "wxRadioBox")
  {
    wxExpr *valueList = (wxExpr *) NULL;
    // Check for default value list
    if ((valueList = expr->Nth(count)) && (valueList->Type() == PrologList))
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
        // controlItem->SetLabelFont(wxResourceInterpretFontSpec(expr->Nth(count)));
      count ++;

        if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
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
    item->SetType("wxMenuSeparator");
    return item;
  }
  else
  {
//    item->SetType(wxTYPE_MENU); // Well, menu item, but doesn't matter.
    item->SetType("wxMenu"); // Well, menu item, but doesn't matter.
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
          wxLogWarning(_("Could not resolve menu id '%s'. "
                         "Use (non-zero) integer instead\n"
                         "or provide #define (see manual for caveats)"),
                         (const char*) idExpr->StringValue());
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
  expr->GetAttributeValue("menu", &listExpr);
  if (!listExpr)
    return (wxItemResource *) NULL;

  wxItemResource *menuResource = wxResourceInterpretMenuItem(table, listExpr);

  if (!menuResource)
    return (wxItemResource *) NULL;

  wxString name;
  if (expr->GetAttributeValue("name", name))
  {
    menuResource->SetName(name);
  }

  return menuResource;
}

wxItemResource *wxResourceInterpretMenuBar(wxResourceTable& table, wxExpr *expr)
{
  wxExpr *listExpr = (wxExpr *) NULL;
  expr->GetAttributeValue("menu", &listExpr);
  if (!listExpr)
    return (wxItemResource *) NULL;

  wxItemResource *resource = new wxItemResource;
  resource->SetType("wxMenu");
//  resource->SetType(wxTYPE_MENU);

  wxExpr *element = listExpr->GetFirst();
  while (element)
  {
    wxItemResource *menuResource = wxResourceInterpretMenuItem(table, listExpr);
    resource->GetChildren().Append(menuResource);
    element = element->GetNext();
  }

  wxString name;
  if (expr->GetAttributeValue("name", name))
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
  bitmapItem->SetType("wxBitmap");
  wxString name;
  if (expr->GetAttributeValue("name", name))
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
      if (bitmapKeyword == "bitmap" || bitmapKeyword == "icon")
      {
        // The value part: always a list.
        wxExpr *listExpr = bitmapExpr->Nth(2);
        if (listExpr->Type() == PrologList)
        {
          wxItemResource *bitmapSpec = new wxItemResource;
//          bitmapSpec->SetType(wxTYPE_BITMAP);
          bitmapSpec->SetType("wxBitmap");

          // List is of form: [filename, bitmaptype, platform, colours, xresolution, yresolution]
          // where everything after 'filename' is optional.
          wxExpr *nameExpr = listExpr->Nth(0);
          wxExpr *typeExpr = listExpr->Nth(1);
          wxExpr *platformExpr = listExpr->Nth(2);
          wxExpr *coloursExpr = listExpr->Nth(3);
          wxExpr *xresExpr = listExpr->Nth(4);
          wxExpr *yresExpr = listExpr->Nth(5);
          if (nameExpr && nameExpr->StringValue() != "")
          {
            bitmapSpec->SetName(nameExpr->StringValue());
          }
          if (typeExpr && typeExpr->StringValue() != "")
          {
            bitmapSpec->SetValue1(wxParseWindowStyle(typeExpr->StringValue()));
          }
          else
            bitmapSpec->SetValue1(0);

          if (platformExpr && platformExpr->StringValue() != "")
          {
            wxString plat(platformExpr->StringValue());
            if (plat == "windows" || plat == "WINDOWS")
              bitmapSpec->SetValue2(RESOURCE_PLATFORM_WINDOWS);
            else if (plat == "x" || plat == "X")
              bitmapSpec->SetValue2(RESOURCE_PLATFORM_X);
            else if (plat == "mac" || plat == "MAC")
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
    item->SetType("wxIcon");
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
  wxString faceName("");

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

  wxFont font(point, family, style, weight, (underline != 0), faceName);
  return font;
}

// Separate file for the remainder of this, for BC++/Win16

#if !(defined(__BORLANDC__) && defined(__WIN16__))
/*
 * (Re)allocate buffer for reading in from resource file
 */

bool wxReallocateResourceBuffer()
{
  if (!wxResourceBuffer)
  {
    wxResourceBufferSize = 1000;
    wxResourceBuffer = new char[wxResourceBufferSize];
    return TRUE;
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
  return TRUE;
}

static bool wxEatWhiteSpace(FILE *fd)
{
  int ch = getc(fd);
  if ((ch != ' ') && (ch != '/') && (ch != ' ') && (ch != 10) && (ch != 13) && (ch != 9))
  {
    ungetc(ch, fd);
    return TRUE;
  }

  // Eat whitespace
  while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)
    ch = getc(fd);
  // Check for comment
  if (ch == '/')
  {
    ch = getc(fd);
    if (ch == '*')
    {
      bool finished = FALSE;
      while (!finished)
      {
        ch = getc(fd);
        if (ch == EOF)
          return FALSE;
        if (ch == '*')
        {
          int newCh = getc(fd);
          if (newCh == '/')
            finished = TRUE;
          else
          {
            ungetc(newCh, fd);
          }
        }
      }
    }
    else // False alarm
      return FALSE;
  }
  else
    ungetc(ch, fd);
  return wxEatWhiteSpace(fd);
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
        return FALSE;
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
      return FALSE;
  }
  return TRUE;
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
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "#define") == 0)
  {
    wxGetResourceToken(fd);
    char *name = copystring(wxResourceBuffer);
    wxGetResourceToken(fd);
    char *value = copystring(wxResourceBuffer);
    if (isalpha(value[0]))
    {
      int val = (int)atol(value);
      wxResourceAddIdentifier(name, val, table);
    }
    else
    {
      wxLogWarning(_("#define %s must be an integer."), name);
      delete[] name;
      delete[] value;
      return FALSE;
    }
    delete[] name;
    delete[] value;

    return TRUE;
  }
  else if (strcmp(wxResourceBuffer, "#include") == 0)
  {
    wxGetResourceToken(fd);
    char *name = copystring(wxResourceBuffer);
    char *actualName = name;
    if (name[0] == '"')
      actualName = name + 1;
    int len = strlen(name);
    if ((len > 0) && (name[len-1] == '"'))
      name[len-1] = 0;
    if (!wxResourceParseIncludeFile(actualName, table))
    {
      wxLogWarning(_("Could not find resource include file %s."), actualName);
    }
    delete[] name;
    return TRUE;
  }
  else if (strcmp(wxResourceBuffer, "static") != 0)
  {
    char buf[300];
    strcpy(buf, _("Found "));
    strncat(buf, wxResourceBuffer, 30);
    strcat(buf, _(", expected static, #include or #define\nwhilst parsing resource."));
    wxLogWarning(buf);
    return FALSE;
  }

  // char
  if (!wxGetResourceToken(fd))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "char") != 0)
  {
    wxLogWarning(_("Expected 'char' whilst parsing resource."));
    return FALSE;
  }

  // *name
  if (!wxGetResourceToken(fd))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (wxResourceBuffer[0] != '*')
  {
    wxLogWarning(_("Expected '*' whilst parsing resource."));
    return FALSE;
  }
  char nameBuf[100];
  strncpy(nameBuf, wxResourceBuffer+1, 99);

  // =
  if (!wxGetResourceToken(fd))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "=") != 0)
  {
    wxLogWarning(_("Expected '=' whilst parsing resource."));
    return FALSE;
  }

  // String
  if (!wxGetResourceToken(fd))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }
  else
  {
    if (!db.ReadPrologFromString(wxResourceBuffer))
    {
      wxLogWarning(_("%s: ill-formed resource file syntax."), nameBuf);
      return FALSE;
    }
  }
  // Semicolon
  if (!wxGetResourceToken(fd))
  {
    *eof = TRUE;
  }
  return TRUE;
}

/*
 * Parses string window style into integer window style
 */

/*
 * Style flag parsing, e.g.
 * "wxSYSTEM_MENU | wxBORDER" -> integer
 */

char* wxResourceParseWord(char*s, int *i)
{
  if (!s)
    return (char*) NULL;

  static char buf[150];
  int len = strlen(s);
  int j = 0;
  int ii = *i;
  while ((ii < len) && (isalpha(s[ii]) || (s[ii] == '_')))
  {
    buf[j] = s[ii];
    j ++;
    ii ++;
  }
  buf[j] = 0;

  // Eat whitespace and conjunction characters
  while ((ii < len) &&
         ((s[ii] == ' ') || (s[ii] == '|') || (s[ii] == ',')))
  {
    ii ++;
  }
  *i = ii;
  if (j == 0)
    return (char*) NULL;
  else
    return buf;
}

struct wxResourceBitListStruct
{
  char *word;
  long bits;
};

static wxResourceBitListStruct wxResourceBitListTable[] =
{
  /* wxListBox */
  { "wxSINGLE", wxLB_SINGLE },
  { "wxMULTIPLE", wxLB_MULTIPLE },
  { "wxEXTENDED", wxLB_EXTENDED },
  { "wxLB_SINGLE", wxLB_SINGLE },
  { "wxLB_MULTIPLE", wxLB_MULTIPLE },
  { "wxLB_EXTENDED", wxLB_EXTENDED },
  { "wxLB_NEEDED_SB", wxLB_NEEDED_SB },
  { "wxLB_ALWAYS_SB", wxLB_ALWAYS_SB },
  { "wxLB_SORT", wxLB_SORT },
  { "wxLB_OWNERDRAW", wxLB_OWNERDRAW },
  { "wxLB_HSCROLL", wxLB_HSCROLL },

  /* wxComboxBox */
  { "wxCB_SIMPLE", wxCB_SIMPLE },
  { "wxCB_DROPDOWN", wxCB_DROPDOWN },
  { "wxCB_READONLY", wxCB_READONLY },
  { "wxCB_SORT", wxCB_SORT },

  /* wxGauge */
  { "wxGA_PROGRESSBAR", wxGA_PROGRESSBAR },
  { "wxGA_HORIZONTAL", wxGA_HORIZONTAL },
  { "wxGA_VERTICAL", wxGA_VERTICAL },

  /* wxTextCtrl */
  { "wxPASSWORD", wxPASSWORD},
  { "wxPROCESS_ENTER", wxPROCESS_ENTER},
  { "wxTE_PASSWORD", wxTE_PASSWORD},
  { "wxTE_READONLY", wxTE_READONLY},
  { "wxTE_PROCESS_ENTER", wxTE_PROCESS_ENTER},
  { "wxTE_MULTILINE", wxTE_MULTILINE},

  /* wxRadioBox/wxRadioButton */
  { "wxRB_GROUP", wxRB_GROUP },
  { "wxRA_SPECIFY_COLS", wxRA_SPECIFY_COLS },
  { "wxRA_SPECIFY_ROWS", wxRA_SPECIFY_ROWS },
  { "wxRA_HORIZONTAL", wxRA_HORIZONTAL },
  { "wxRA_VERTICAL", wxRA_VERTICAL },

  /* wxSlider */
  { "wxSL_HORIZONTAL", wxSL_HORIZONTAL },
  { "wxSL_VERTICAL", wxSL_VERTICAL },
  { "wxSL_AUTOTICKS", wxSL_AUTOTICKS },
  { "wxSL_LABELS", wxSL_LABELS },
  { "wxSL_LEFT", wxSL_LEFT },
  { "wxSL_TOP", wxSL_TOP },
  { "wxSL_RIGHT", wxSL_RIGHT },
  { "wxSL_BOTTOM", wxSL_BOTTOM },
  { "wxSL_BOTH", wxSL_BOTH },
  { "wxSL_SELRANGE", wxSL_SELRANGE },

  /* wxScrollBar */
  { "wxSB_HORIZONTAL", wxSB_HORIZONTAL },
  { "wxSB_VERTICAL", wxSB_VERTICAL },

  /* wxButton */
  { "wxBU_AUTODRAW", wxBU_AUTODRAW },
  { "wxBU_NOAUTODRAW", wxBU_NOAUTODRAW },

  /* wxTreeCtrl */
  { "wxTR_HAS_BUTTONS", wxTR_HAS_BUTTONS },
  { "wxTR_EDIT_LABELS", wxTR_EDIT_LABELS },
  { "wxTR_LINES_AT_ROOT", wxTR_LINES_AT_ROOT },

  /* wxListCtrl */
  { "wxLC_ICON", wxLC_ICON },
  { "wxLC_SMALL_ICON", wxLC_SMALL_ICON },
  { "wxLC_LIST", wxLC_LIST },
  { "wxLC_REPORT", wxLC_REPORT },
  { "wxLC_ALIGN_TOP", wxLC_ALIGN_TOP },
  { "wxLC_ALIGN_LEFT", wxLC_ALIGN_LEFT },
  { "wxLC_AUTOARRANGE", wxLC_AUTOARRANGE },
  { "wxLC_USER_TEXT", wxLC_USER_TEXT },
  { "wxLC_EDIT_LABELS", wxLC_EDIT_LABELS },
  { "wxLC_NO_HEADER", wxLC_NO_HEADER },
  { "wxLC_NO_SORT_HEADER", wxLC_NO_SORT_HEADER },
  { "wxLC_SINGLE_SEL", wxLC_SINGLE_SEL },
  { "wxLC_SORT_ASCENDING", wxLC_SORT_ASCENDING },
  { "wxLC_SORT_DESCENDING", wxLC_SORT_DESCENDING },

  /* wxSpinButton */
  { "wxSP_VERTICAL", wxSP_VERTICAL},
  { "wxSP_HORIZONTAL", wxSP_HORIZONTAL},
  { "wxSP_ARROW_KEYS", wxSP_ARROW_KEYS},
  { "wxSP_WRAP", wxSP_WRAP},

  /* wxSplitterWnd */
  { "wxSP_NOBORDER", wxSP_NOBORDER},
  { "wxSP_3D", wxSP_3D},
  { "wxSP_BORDER", wxSP_BORDER},

  /* wxTabCtrl */
  { "wxTC_MULTILINE", wxTC_MULTILINE},
  { "wxTC_RIGHTJUSTIFY", wxTC_RIGHTJUSTIFY},
  { "wxTC_FIXEDWIDTH", wxTC_FIXEDWIDTH},
  { "wxTC_OWNERDRAW", wxTC_OWNERDRAW},

  /* wxStatusBar95 */
  { "wxST_SIZEGRIP", wxST_SIZEGRIP},

  /* wxControl */
  { "wxFIXED_LENGTH", wxFIXED_LENGTH},
  { "wxALIGN_LEFT", wxALIGN_LEFT},
  { "wxALIGN_CENTER", wxALIGN_CENTER},
  { "wxALIGN_CENTRE", wxALIGN_CENTRE},
  { "wxALIGN_RIGHT", wxALIGN_RIGHT},
  { "wxCOLOURED", wxCOLOURED},

  /* wxToolBar */
  { "wxTB_3DBUTTONS", wxTB_3DBUTTONS},
  { "wxTB_HORIZONTAL", wxTB_HORIZONTAL},
  { "wxTB_VERTICAL", wxTB_VERTICAL},
  { "wxTB_FLAT", wxTB_FLAT},

  /* Generic */
  { "wxVSCROLL", wxVSCROLL },
  { "wxHSCROLL", wxHSCROLL },
  { "wxCAPTION", wxCAPTION },
  { "wxSTAY_ON_TOP", wxSTAY_ON_TOP},
  { "wxICONIZE", wxICONIZE},
  { "wxMINIMIZE", wxICONIZE},
  { "wxMAXIMIZE", wxMAXIMIZE},
  { "wxSDI", 0},
  { "wxMDI_PARENT", 0},
  { "wxMDI_CHILD", 0},
  { "wxTHICK_FRAME", wxTHICK_FRAME},
  { "wxRESIZE_BORDER", wxRESIZE_BORDER},
  { "wxSYSTEM_MENU", wxSYSTEM_MENU},
  { "wxMINIMIZE_BOX", wxMINIMIZE_BOX},
  { "wxMAXIMIZE_BOX", wxMAXIMIZE_BOX},
  { "wxRESIZE_BOX", wxRESIZE_BOX},
  { "wxDEFAULT_FRAME_STYLE", wxDEFAULT_FRAME_STYLE},
  { "wxDEFAULT_FRAME", wxDEFAULT_FRAME_STYLE},
  { "wxDEFAULT_DIALOG_STYLE", wxDEFAULT_DIALOG_STYLE},
  { "wxBORDER", wxBORDER},
  { "wxRETAINED", wxRETAINED},
  { "wxNATIVE_IMPL", 0},
  { "wxEXTENDED_IMPL", 0},
  { "wxBACKINGSTORE", wxBACKINGSTORE},
//  { "wxFLAT", wxFLAT},
//  { "wxMOTIF_RESIZE", wxMOTIF_RESIZE},
  { "wxFIXED_LENGTH", 0},
  { "wxDOUBLE_BORDER", wxDOUBLE_BORDER},
  { "wxSUNKEN_BORDER", wxSUNKEN_BORDER},
  { "wxRAISED_BORDER", wxRAISED_BORDER},
  { "wxSIMPLE_BORDER", wxSIMPLE_BORDER},
  { "wxSTATIC_BORDER", wxSTATIC_BORDER},
  { "wxTRANSPARENT_WINDOW", wxTRANSPARENT_WINDOW},
  { "wxNO_BORDER", wxNO_BORDER},
  { "wxCLIP_CHILDREN", wxCLIP_CHILDREN},

  { "wxTINY_CAPTION_HORIZ", wxTINY_CAPTION_HORIZ},
  { "wxTINY_CAPTION_VERT", wxTINY_CAPTION_VERT},

  // Text font families
  { "wxDEFAULT", wxDEFAULT},
  { "wxDECORATIVE", wxDECORATIVE},
  { "wxROMAN", wxROMAN},
  { "wxSCRIPT", wxSCRIPT},
  { "wxSWISS", wxSWISS},
  { "wxMODERN", wxMODERN},
  { "wxTELETYPE", wxTELETYPE},
  { "wxVARIABLE", wxVARIABLE},
  { "wxFIXED", wxFIXED},
  { "wxNORMAL", wxNORMAL},
  { "wxLIGHT", wxLIGHT},
  { "wxBOLD", wxBOLD},
  { "wxITALIC", wxITALIC},
  { "wxSLANT", wxSLANT},
  { "wxSOLID", wxSOLID},
  { "wxDOT", wxDOT},
  { "wxLONG_DASH", wxLONG_DASH},
  { "wxSHORT_DASH", wxSHORT_DASH},
  { "wxDOT_DASH", wxDOT_DASH},
  { "wxUSER_DASH", wxUSER_DASH},
  { "wxTRANSPARENT", wxTRANSPARENT},
  { "wxSTIPPLE", wxSTIPPLE},
  { "wxBDIAGONAL_HATCH", wxBDIAGONAL_HATCH},
  { "wxCROSSDIAG_HATCH", wxCROSSDIAG_HATCH},
  { "wxFDIAGONAL_HATCH", wxFDIAGONAL_HATCH},
  { "wxCROSS_HATCH", wxCROSS_HATCH},
  { "wxHORIZONTAL_HATCH", wxHORIZONTAL_HATCH},
  { "wxVERTICAL_HATCH", wxVERTICAL_HATCH},
  { "wxJOIN_BEVEL", wxJOIN_BEVEL},
  { "wxJOIN_MITER", wxJOIN_MITER},
  { "wxJOIN_ROUND", wxJOIN_ROUND},
  { "wxCAP_ROUND", wxCAP_ROUND},
  { "wxCAP_PROJECTING", wxCAP_PROJECTING},
  { "wxCAP_BUTT", wxCAP_BUTT},

  // Logical ops
  { "wxCLEAR", wxCLEAR},
  { "wxXOR", wxXOR},
  { "wxINVERT", wxINVERT},
  { "wxOR_REVERSE", wxOR_REVERSE},
  { "wxAND_REVERSE", wxAND_REVERSE},
  { "wxCOPY", wxCOPY},
  { "wxAND", wxAND},
  { "wxAND_INVERT", wxAND_INVERT},
  { "wxNO_OP", wxNO_OP},
  { "wxNOR", wxNOR},
  { "wxEQUIV", wxEQUIV},
  { "wxSRC_INVERT", wxSRC_INVERT},
  { "wxOR_INVERT", wxOR_INVERT},
  { "wxNAND", wxNAND},
  { "wxOR", wxOR},
  { "wxSET", wxSET},

  { "wxFLOOD_SURFACE", wxFLOOD_SURFACE},
  { "wxFLOOD_BORDER", wxFLOOD_BORDER},
  { "wxODDEVEN_RULE", wxODDEVEN_RULE},
  { "wxWINDING_RULE", wxWINDING_RULE},
  { "wxHORIZONTAL", wxHORIZONTAL},
  { "wxVERTICAL", wxVERTICAL},
  { "wxBOTH", wxBOTH},
  { "wxCENTER_FRAME", wxCENTER_FRAME},
  { "wxOK", wxOK},
  { "wxYES_NO", wxYES_NO},
  { "wxCANCEL", wxCANCEL},
  { "wxYES", wxYES},
  { "wxNO", wxNO},
  { "wxICON_EXCLAMATION", wxICON_EXCLAMATION},
  { "wxICON_HAND", wxICON_HAND},
  { "wxICON_QUESTION", wxICON_QUESTION},
  { "wxICON_INFORMATION", wxICON_INFORMATION},
  { "wxICON_STOP", wxICON_STOP},
  { "wxICON_ASTERISK", wxICON_ASTERISK},
  { "wxICON_MASK", wxICON_MASK},
  { "wxCENTRE", wxCENTRE},
  { "wxCENTER", wxCENTRE},
  { "wxUSER_COLOURS", wxUSER_COLOURS},
  { "wxVERTICAL_LABEL", 0},
  { "wxHORIZONTAL_LABEL", 0},

  // Bitmap types (not strictly styles)
  { "wxBITMAP_TYPE_XPM", wxBITMAP_TYPE_XPM},
  { "wxBITMAP_TYPE_XBM", wxBITMAP_TYPE_XBM},
  { "wxBITMAP_TYPE_BMP", wxBITMAP_TYPE_BMP},
  { "wxBITMAP_TYPE_RESOURCE", wxBITMAP_TYPE_BMP_RESOURCE},
  { "wxBITMAP_TYPE_BMP_RESOURCE", wxBITMAP_TYPE_BMP_RESOURCE},
  { "wxBITMAP_TYPE_GIF", wxBITMAP_TYPE_GIF},
  { "wxBITMAP_TYPE_TIF", wxBITMAP_TYPE_TIF},
  { "wxBITMAP_TYPE_ICO", wxBITMAP_TYPE_ICO},
  { "wxBITMAP_TYPE_ICO_RESOURCE", wxBITMAP_TYPE_ICO_RESOURCE},
  { "wxBITMAP_TYPE_CUR", wxBITMAP_TYPE_CUR},
  { "wxBITMAP_TYPE_CUR_RESOURCE", wxBITMAP_TYPE_CUR_RESOURCE},
  { "wxBITMAP_TYPE_XBM_DATA", wxBITMAP_TYPE_XBM_DATA},
  { "wxBITMAP_TYPE_XPM_DATA", wxBITMAP_TYPE_XPM_DATA},
  { "wxBITMAP_TYPE_ANY", wxBITMAP_TYPE_ANY}
};

static int wxResourceBitListCount = (sizeof(wxResourceBitListTable)/sizeof(wxResourceBitListStruct));

long wxParseWindowStyle(const wxString& bitListString)
{
  int i = 0;
  char *word;
  long bitList = 0;
  while ((word = wxResourceParseWord((char*) (const char*) bitListString, &i)))
  {
    bool found = FALSE;
    int j;
    for (j = 0; j < wxResourceBitListCount; j++)
      if (strcmp(wxResourceBitListTable[j].word, word) == 0)
      {
        bitList |= wxResourceBitListTable[j].bits;
        found = TRUE;
        break;
      }
    if (!found)
    {
      wxLogWarning(_("Unrecognized style %s whilst parsing resource."), word);
      return 0;
    }
  }
  return bitList;
}

/*
 * Load a bitmap from a wxWindows resource, choosing an optimum
 * depth and appropriate type.
 */

wxBitmap wxResourceCreateBitmap(const wxString& resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  wxItemResource *item = table->FindResource(resource);
  if (item)
  {
    if ((item->GetType() == "") || (item->GetType() != "wxBitmap"))
    {
      wxLogWarning(_("%s not a bitmap resource specification."), (const char*) resource);
      return wxNullBitmap;
    }
    int thisDepth = wxDisplayDepth();
    long thisNoColours = (long)pow(2.0, (double)thisDepth);

    wxItemResource *optResource = (wxItemResource *) NULL;

    // Try to find optimum bitmap for this platform/colour depth
    wxNode *node = item->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
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
      node = node->Next();
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
                         "Forgot to use wxResourceLoadBitmapData?"), (const char*) name);
          return wxNullBitmap;
        }
        return wxBitmap(item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3()) ;
#else
        wxLogWarning(_("No XBM facility available!"));
#endif
        break;
      }
      case wxBITMAP_TYPE_XPM_DATA:
      {
#if (defined(__WXGTK__)) || (defined(__WXMSW__) && wxUSE_XPM_IN_MSW)
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          wxLogWarning(_("Failed to find XPM resource %s.\n"
                         "Forgot to use wxResourceLoadBitmapData?"), (const char*) name);
          return wxNullBitmap;
        }
        return wxBitmap(item->GetValue1());
#else
        wxLogWarning(_("No XPM facility available!"));
#endif
        break;
      }
      default:
      {
        return wxBitmap(name, bitmapType);
        break;
      }
    }
    return wxNullBitmap;
  }
  else
  {
    wxLogWarning(_("Bitmap resource specification %s not found."), (const char*) resource);
    return wxNullBitmap;
  }
}

/*
 * Load an icon from a wxWindows resource, choosing an optimum
 * depth and appropriate type.
 */

wxIcon wxResourceCreateIcon(const wxString& resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  wxItemResource *item = table->FindResource(resource);
  if (item)
  {
    if ((item->GetType() == "") || strcmp(item->GetType(), "wxIcon") != 0)
    {
      wxLogWarning(_("%s not an icon resource specification."), (const char*) resource);
      return wxNullIcon;
    }
    int thisDepth = wxDisplayDepth();
    long thisNoColours = (long)pow(2.0, (double)thisDepth);

    wxItemResource *optResource = (wxItemResource *) NULL;

    // Try to find optimum icon for this platform/colour depth
    wxNode *node = item->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
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
      node = node->Next();
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
                         "Forgot to use wxResourceLoadIconData?"), (const char*) name);
          return wxNullIcon;
        }
        return wxIcon((const char **)item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3());
#else
        wxLogWarning(_("No XBM facility available!"));
#endif
        break;
      }
      case wxBITMAP_TYPE_XPM_DATA:
      {
      // *** XPM ICON NOT YET IMPLEMENTED IN WXWINDOWS ***
/*
#if (defined(__WXGTK__)) || (defined(__WXMSW__) && wxUSE_XPM_IN_MSW)
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, _("Failed to find XPM resource %s.\nForgot to use wxResourceLoadIconData?"), name);
          wxLogWarning(buf);
          return NULL;
        }
        return wxIcon((char **)item->GetValue1());
#else
        wxLogWarning(_("No XPM facility available!"));
#endif
*/
        wxLogWarning(_("No XPM icon facility available!"));
        break;
      }
      default:
      {
#ifdef __WXGTK__
        wxLogWarning(_("Icon resource specification %s not found."), (const char*) resource);
#else
        return wxIcon(name, bitmapType);
#endif
        break;
      }
    }
    return wxNullIcon;
  }
  else
  {
    wxLogWarning(_("Icon resource specification %s not found."), (const char*) resource);
    return wxNullIcon;
  }
}

wxMenu *wxResourceCreateMenu(wxItemResource *item)
{
  wxMenu *menu = new wxMenu;
  wxNode *node = item->GetChildren().First();
  while (node)
  {
    wxItemResource *child = (wxItemResource *)node->Data();
    if ((child->GetType() != "") && (child->GetType() == "wxMenuSeparator"))
      menu->AppendSeparator();
    else if (child->GetChildren().Number() > 0)
    {
      wxMenu *subMenu = wxResourceCreateMenu(child);
      if (subMenu)
        menu->Append((int)child->GetValue1(), child->GetTitle(), subMenu, child->GetValue4());
    }
    else
    {
      menu->Append((int)child->GetValue1(), child->GetTitle(), child->GetValue4(), (child->GetValue2() != 0));
    }
    node = node->Next();
  }
  return menu;
}

wxMenuBar *wxResourceCreateMenuBar(const wxString& resource, wxResourceTable *table, wxMenuBar *menuBar)
{
  if (!table)
    table = wxDefaultResourceTable;

  wxItemResource *menuResource = table->FindResource(resource);
  if (menuResource && (menuResource->GetType() != "") && (menuResource->GetType() == "wxMenu"))
  {
    if (!menuBar)
      menuBar = new wxMenuBar;
    wxNode *node = menuResource->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
      wxMenu *menu = wxResourceCreateMenu(child);
      if (menu)
        menuBar->Append(menu, child->GetTitle());
      node = node->Next();
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
  if (menuResource && (menuResource->GetType() != "") && (menuResource->GetType() == "wxMenu"))
//  if (menuResource && (menuResource->GetType() == wxTYPE_MENU))
    return wxResourceCreateMenu(menuResource);
  return (wxMenu *) NULL;
}

// Global equivalents (so don't have to refer to default table explicitly)
bool wxResourceParseData(const wxString& resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  return table->ParseResourceData(resource);
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

  table->identifiers.Put(name, (wxObject *)value);
  return TRUE;
}

int wxResourceGetIdentifier(const wxString& name, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  return (int)table->identifiers.Get(name);
}

/*
 * Parse #include file for #defines (only)
 */

bool wxResourceParseIncludeFile(const wxString& f, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  FILE *fd = fopen(f, "r");
  if (!fd)
  {
    return FALSE;
  }
  while (wxGetResourceToken(fd))
  {
    if (strcmp(wxResourceBuffer, "#define") == 0)
    {
      wxGetResourceToken(fd);
      char *name = copystring(wxResourceBuffer);
      wxGetResourceToken(fd);
      char *value = copystring(wxResourceBuffer);
      if (isdigit(value[0]))
      {
        int val = (int)atol(value);
        wxResourceAddIdentifier(name, val, table);
      }
      delete[] name;
      delete[] value;
    }
  }
  fclose(fd);
  return TRUE;
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
  int ch = getc_string(s);
  if (ch == EOF)
    return TRUE;

  if ((ch != ' ') && (ch != '/') && (ch != ' ') && (ch != 10) && (ch != 13) && (ch != 9))
  {
    ungetc_string();
    return TRUE;
  }

  // Eat whitespace
  while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)
    ch = getc_string(s);
  // Check for comment
  if (ch == '/')
  {
    ch = getc_string(s);
    if (ch == '*')
    {
      bool finished = FALSE;
      while (!finished)
      {
        ch = getc_string(s);
        if (ch == EOF)
          return FALSE;
        if (ch == '*')
        {
          int newCh = getc_string(s);
          if (newCh == '/')
            finished = TRUE;
          else
          {
            ungetc_string();
          }
        }
      }
    }
    else // False alarm
      return FALSE;
  }
  else if (ch != EOF)
    ungetc_string();
  return wxEatWhiteSpaceString(s);
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
        return FALSE;
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
      return FALSE;
  }
  return TRUE;
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
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "#define") == 0)
  {
    wxGetResourceTokenString(s);
    char *name = copystring(wxResourceBuffer);
    wxGetResourceTokenString(s);
    char *value = copystring(wxResourceBuffer);
    if (isalpha(value[0]))
    {
      int val = (int)atol(value);
      wxResourceAddIdentifier(name, val, table);
    }
    else
    {
      wxLogWarning(_("#define %s must be an integer."), name);
      delete[] name;
      delete[] value;
      return FALSE;
    }
    delete[] name;
    delete[] value;

    return TRUE;
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
    return TRUE;
  }
*/
  else if (strcmp(wxResourceBuffer, "static") != 0)
  {
    char buf[300];
    strcpy(buf, _("Found "));
    strncat(buf, wxResourceBuffer, 30);
    strcat(buf, _(", expected static, #include or #define\nwhilst parsing resource."));
    wxLogWarning(buf);
    return FALSE;
  }

  // char
  if (!wxGetResourceTokenString(s))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "char") != 0)
  {
    wxLogWarning(_("Expected 'char' whilst parsing resource."));
    return FALSE;
  }

  // *name
  if (!wxGetResourceTokenString(s))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (wxResourceBuffer[0] != '*')
  {
    wxLogWarning(_("Expected '*' whilst parsing resource."));
    return FALSE;
  }
  char nameBuf[100];
  strncpy(nameBuf, wxResourceBuffer+1, 99);

  // =
  if (!wxGetResourceTokenString(s))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "=") != 0)
  {
    wxLogWarning(_("Expected '=' whilst parsing resource."));
    return FALSE;
  }

  // String
  if (!wxGetResourceTokenString(s))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }
  else
  {
    if (!db.ReadPrologFromString(wxResourceBuffer))
    {
      wxLogWarning(_("%s: ill-formed resource file syntax."), nameBuf);
      return FALSE;
    }
  }
  // Semicolon
  if (!wxGetResourceTokenString(s))
  {
    *eof = TRUE;
  }
  return TRUE;
}

bool wxResourceParseString(char *s, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  if (!s)
    return FALSE;

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

  bool eof = FALSE;
  while (wxResourceReadOneResourceString(s, db, &eof, table) && !eof)
  {
    // Loop
  }
  return wxResourceInterpretResources(*table, db);
}

/*
 * resource loading facility
 */

bool wxWindow::LoadFromResource(wxWindow *parent, const wxString& resourceName, const wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  wxItemResource *resource = table->FindResource((const char *)resourceName);
//  if (!resource || (resource->GetType() != wxTYPE_DIALOG_BOX))
  if (!resource || (resource->GetType() == "") ||
    ! ((resource->GetType() == "wxDialog") || (resource->GetType() == "wxPanel")))
    return FALSE;

  wxString title(resource->GetTitle());
  long theWindowStyle = resource->GetStyle();
  bool isModal = (resource->GetValue1() != 0);
  int x = resource->GetX();
  int y = resource->GetY();
  int width = resource->GetWidth();
  int height = resource->GetHeight();
  wxString name = resource->GetName();

  if (IsKindOf(CLASSINFO(wxDialog)))
  {
    wxDialog *dialogBox = (wxDialog *)this;
    long modalStyle = isModal ? wxDIALOG_MODAL : 0;
    if (!dialogBox->Create(parent, -1, title, wxPoint(x, y), wxSize(width, height), theWindowStyle|modalStyle, name))
      return FALSE;

    // Only reset the client size if we know we're not going to do it again below.
    if ((resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) == 0)
      dialogBox->SetClientSize(width, height);
  }
  else if (IsKindOf(CLASSINFO(wxPanel)))
  {
    wxPanel* panel = (wxPanel *)this;
    if (!panel->Create(parent, -1, wxPoint(x, y), wxSize(width, height), theWindowStyle, name))
      return FALSE;
  }
  else
  {
    if (!this->Create(parent, -1, wxPoint(x, y), wxSize(width, height), theWindowStyle, name))
      return FALSE;
  }

  if ((resource->GetResourceStyle() & wxRESOURCE_USE_DEFAULTS) != 0)
  {
    // No need to do this since it's done in wxPanel or wxDialog constructor.
    // SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
  }
  else
  {
    if (resource->GetFont().Ok())
      SetFont(resource->GetFont());
    if (resource->GetBackgroundColour().Ok())
      SetBackgroundColour(resource->GetBackgroundColour());
  }

  // Should have some kind of font at this point
  if (!GetFont().Ok())
      SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
  if (!GetBackgroundColour().Ok())
      SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));

  // Only when we've created the window and set the font can we set the correct size,
  // if based on dialog units.
  if ((resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) != 0)
  {
    wxSize sz = ConvertDialogToPixels(wxSize(width, height));
    SetClientSize(sz.x, sz.y);

    wxPoint pt = ConvertDialogToPixels(wxPoint(x, y));
    Move(pt.x, pt.y);
  }

  // Now create children
  wxNode *node = resource->GetChildren().First();
  while (node)
  {
    wxItemResource *childResource = (wxItemResource *)node->Data();

    (void) CreateItem(childResource, resource, table);

    node = node->Next();
  }
  return TRUE;
}

wxControl *wxWindow::CreateItem(const wxItemResource *resource, const wxItemResource* parentResource, const wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
  return table->CreateItem((wxWindow *)this, resource, parentResource);
}

#ifdef _MSC_VER
    #pragma warning(default:4706)   // assignment within conditional expression
#endif // VC++

#endif
  // BC++/Win16

#endif // wxUSE_WX_RESOURCES
