/////////////////////////////////////////////////////////////////////////////
// Name:        resource.cpp
// Purpose:     Resource system
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "resource.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

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
#include "wx/radiobox.h"
#include "wx/listbox.h"
#include "wx/choice.h"
#include "wx/checkbox.h"
#include "wx/slider.h"
#include "wx/statbox.h"
#if USE_GAUGE
#include "wx/gauge.h"
#endif
#include "wx/textctrl.h"
#include "wx/msgbxdlg.h"
#endif

#if USE_SCROLLBAR
#include "wx/scrolbar.h"
#endif

#if USE_COMBOBOX
#include "wx/combobox.h"
#endif

#include "wx/validate.h"

#if USE_WX_RESOURCES

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "wx/resource.h"
#include "wx/string.h"
#include "wx/wxexpr.h"

// Forward (private) declarations
bool wxResourceInterpretResources(wxResourceTable& table, PrologDatabase& db);
wxItemResource *wxResourceInterpretDialog(wxResourceTable& table, PrologExpr *expr, bool isPanel = FALSE);
wxItemResource *wxResourceInterpretControl(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretMenu(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretMenuBar(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretString(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretBitmap(wxResourceTable& table, PrologExpr *expr);
wxItemResource *wxResourceInterpretIcon(wxResourceTable& table, PrologExpr *expr);
// Interpret list expression
wxFont *wxResourceInterpretFontSpec(PrologExpr *expr);

bool wxResourceReadOneResource(FILE *fd, PrologDatabase& db, bool *eof, wxResourceTable *table = NULL);
bool wxResourceParseIncludeFile(char *f, wxResourceTable *table = NULL);

wxResourceTable *wxDefaultResourceTable = NULL;

static char *wxResourceBuffer = NULL;
static long wxResourceBufferSize = 0;
static long wxResourceBufferCount = 0;
static int wxResourceStringPtr = 0;

void wxInitializeResourceSystem(void)
{
    wxDefaultResourceTable = new wxResourceTable;
}

void wxCleanUpResourceSystem(void)
{
    delete wxDefaultResourceTable;
}

void wxWarning(char *msg)
{
  wxMessageBox(msg, _("Warning"), wxOK);
}

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxItemResource, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxResourceTable, wxHashTable)
#endif

wxItemResource::wxItemResource(void)
{
  itemType = NULL;
  title = NULL;
  name = NULL;
  windowStyle = 0;
  x = y = width = height = 0;
  value1 = value2 = value3 = value5 = 0;
  value4 = NULL;
  stringValues = NULL;
  bitmap = NULL;
  backgroundColour = labelColour = buttonColour = NULL;
  windowFont = NULL;
  m_windowId = 0;
}

wxItemResource::~wxItemResource(void)
{
  if (itemType) delete[] itemType;
  if (title) delete[] title;
  if (name) delete[] name;
  if (value4) delete[] value4;
  if (stringValues)
    delete stringValues;
  if (bitmap)
    delete bitmap;
  if (backgroundColour)
    delete backgroundColour;
  if (labelColour)
    delete labelColour;
  if (buttonColour)
    delete buttonColour;
  wxNode *node = children.First();
  while (node)
  {
    wxItemResource *item = (wxItemResource *)node->Data();
    delete item;
    delete node;
    node = children.First();
  }
}

void wxItemResource::SetTitle(char *t)
{
  if (t == title)
    return;
    
  if (title) delete[] title;
  if (t)
    title = copystring(t);
  else
    title = NULL;
}

void wxItemResource::SetType(char *t)
{
  if (itemType == t)
    return;
    
  if (itemType) delete[] itemType;
  if (t)
    itemType = copystring(t);
  else
    itemType = NULL;
}

void wxItemResource::SetName(char *n)
{
  if (n == name)
    return;
    
  if (name) delete[] name;
  if (n)
    name = copystring(n);
  else
    name = NULL;
}

void wxItemResource::SetStringValues(wxStringList *svalues)
{
  if (stringValues)
    delete stringValues;
  if (svalues)
    stringValues = svalues;
  else
    stringValues = NULL;
}

void wxItemResource::SetValue4(char *v)
{
  if (value4 == v)
    return;

  if (value4) delete[] value4;
  if (v)
    value4 = copystring(v);
  else
    value4 = NULL;
}

/*
 * Resource table
 */
 
wxResourceTable::wxResourceTable(void):wxHashTable(wxKEY_STRING), identifiers(wxKEY_STRING)
{
}

wxResourceTable::~wxResourceTable(void)
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
  char *name = item->GetName();
  if (!name)
    name = item->GetTitle();
  if (!name)
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
    wxNode *node = NULL;
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

bool wxResourceTable::ParseResourceFile(char *filename)
{
  PrologDatabase db;

  FILE *fd = fopen(filename, "r");
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

bool wxResourceTable::ParseResourceData(char *data)
{
  PrologDatabase db;
  if (!db.ReadPrologFromString(data))
  {
    wxWarning(_("Ill-formed resource file syntax."));
    return FALSE;
  }

  return wxResourceInterpretResources(*this, db);
}

bool wxResourceTable::RegisterResourceBitmapData(char *name, char bits[], int width, int height)
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

bool wxResourceTable::RegisterResourceBitmapData(char *name, char **data)
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

bool wxResourceTable::SaveResource(char *WXUNUSED(filename))
{
  return FALSE;
}

void wxResourceTable::ClearTable(void)
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

wxControl *wxResourceTable::CreateItem(wxWindow *parent, wxItemResource *childResource) const
{
  int id = childResource->GetId();
  if ( id == 0 )
	id = -1;

  wxControl *control = NULL;
  wxString itemType(childResource->GetType());
  if (itemType == wxString("wxButton") || itemType == wxString("wxBitmapButton"))
      {
        if (childResource->GetValue4())
        {
          // Bitmap button
          wxBitmap *bitmap = childResource->GetBitmap();
          if (!bitmap)
          {
            bitmap = wxResourceCreateBitmap(childResource->GetValue4(), (wxResourceTable *)this);
            childResource->SetBitmap(bitmap);
          }
          if (bitmap)
           control = new wxBitmapButton(parent, id, *bitmap,
             wxPoint(childResource->GetX(), childResource->GetY()),
			 wxSize(childResource->GetWidth(), childResource->GetHeight()),
             	childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
        }
        else
          // Normal, text button
          control = new wxButton(parent, id, childResource->GetTitle(),
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
      }
   else if (itemType == wxString("wxMessage") || itemType == wxString("wxStaticText") ||
    		itemType == wxString("wxStaticBitmap"))
      {
        if (childResource->GetValue4())
        {
          // Bitmap message
          wxBitmap *bitmap = childResource->GetBitmap();
          if (!bitmap)
          {
            bitmap = wxResourceCreateBitmap(childResource->GetValue4(), (wxResourceTable *)this);
            childResource->SetBitmap(bitmap);
          }
#if USE_BITMAP_MESSAGE
          if (bitmap)
           control = new wxStaticBitmap(parent, id, *bitmap,
             wxPoint(childResource->GetX(), childResource->GetY()),
			 wxSize(childResource->GetWidth(), childResource->GetHeight()),
             childResource->GetStyle(), childResource->GetName());
#endif
        }
        else
        {
           control = new wxStaticText(parent, id, childResource->GetTitle(),
             wxPoint(childResource->GetX(), childResource->GetY()),
			 wxSize(childResource->GetWidth(), childResource->GetHeight()),
             childResource->GetStyle(), childResource->GetName());
        }
      }
   else if (itemType == wxString("wxText") || itemType == wxString("wxTextCtrl") || itemType == wxString("wxMultiText"))
      {
        control = new wxTextCtrl(parent, id, childResource->GetValue4(),
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
      }
   else if (itemType == wxString("wxCheckBox"))
      {
        control = new wxCheckBox(parent, id, childResource->GetTitle(),
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        ((wxCheckBox *)control)->SetValue((childResource->GetValue1() != 0));
      }
#if USE_GAUGE
   else if (itemType == wxString("wxGauge"))
      {
        control = new wxGauge(parent, id, (int)childResource->GetValue2(),
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        ((wxGauge *)control)->SetValue((int)childResource->GetValue1());
      }
#endif
#if USE_RADIOBUTTON
   else if (itemType == wxString("wxRadioButton"))
      {
        control = new wxRadioButton(parent, id, childResource->GetTitle(), // (int)childResource->GetValue1(),
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
      }
#endif
#if USE_SCROLLBAR
   else if (itemType == wxString("wxScrollBar"))
      {
        control = new wxScrollBar(parent, id,
           wxPoint(childResource->GetX(), childResource->GetY()),
           wxSize(childResource->GetWidth(), childResource->GetHeight()),
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
        ((wxScrollBar *)control)->SetValue((int)childResource->GetValue1());
        ((wxScrollBar *)control)->SetPageSize((int)childResource->GetValue2());
        ((wxScrollBar *)control)->SetObjectLength((int)childResource->GetValue3());
        ((wxScrollBar *)control)->SetViewLength((int)(long)childResource->GetValue5());
      }
#endif
   else if (itemType == wxString("wxSlider"))
      {
        control = new wxSlider(parent, id, (int)childResource->GetValue1(),
           (int)childResource->GetValue2(), (int)childResource->GetValue3(),
           wxPoint(childResource->GetX(), childResource->GetY()),
           wxSize(childResource->GetWidth(), childResource->GetHeight()),
           childResource->GetStyle(), wxDefaultValidator, childResource->GetName());
      }
   else if (itemType == wxString("wxGroupBox") || itemType == wxString("wxStaticBox"))
      {
        control = new wxStaticBox(parent, id, childResource->GetTitle(),
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           childResource->GetStyle(), childResource->GetName());
      }
   else if (itemType == wxString("wxListBox"))
      {
        wxStringList *stringList = childResource->GetStringValues();
        wxString *strings = NULL;
        int noStrings = 0;
        if (stringList && (stringList->Number() > 0))
        {
          noStrings = stringList->Number();
          strings = new wxString[noStrings];
          wxNode *node = stringList->First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxListBox(parent, id,
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           noStrings, strings, childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        if (strings)
          delete[] strings;
      }
   else if (itemType == wxString("wxChoice"))
      {
        wxStringList *stringList = childResource->GetStringValues();
        wxString *strings = NULL;
        int noStrings = 0;
        if (stringList && (stringList->Number() > 0))
        {
          noStrings = stringList->Number();
          strings = new wxString[noStrings];
          wxNode *node = stringList->First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxChoice(parent, id,
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           noStrings, strings, childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        if (strings)
          delete[] strings;
      }
#if USE_COMBOBOX
   else if (itemType == wxString("wxComboBox"))
      {
        wxStringList *stringList = childResource->GetStringValues();
        wxString *strings = NULL;
        int noStrings = 0;
        if (stringList && (stringList->Number() > 0))
        {
          noStrings = stringList->Number();
          strings = new wxString[noStrings];
          wxNode *node = stringList->First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxComboBox(parent, id, childResource->GetValue4(),
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           noStrings, strings, childResource->GetStyle(), wxDefaultValidator, childResource->GetName());

        if (strings)
          delete[] strings;
      }
#endif
   else if (itemType == wxString("wxRadioBox"))
      {
        wxStringList *stringList = childResource->GetStringValues();
        wxString *strings = NULL;
        int noStrings = 0;
        if (stringList && (stringList->Number() > 0))
        {
          noStrings = stringList->Number();
          strings = new wxString[noStrings];
          wxNode *node = stringList->First();
          int i = 0;
          while (node)
          {
            strings[i] = (char *)node->Data();
            i ++;
            node = node->Next();
          }
        }
        control = new wxRadioBox(parent, (wxWindowID) id, wxString(childResource->GetTitle()),
           wxPoint(childResource->GetX(), childResource->GetY()),
		   wxSize(childResource->GetWidth(), childResource->GetHeight()),
           noStrings, strings, (int)childResource->GetValue1(), childResource->GetStyle(), wxDefaultValidator,
		   childResource->GetName());

        if (strings)
          delete[] strings;
      }

  if (control && childResource->GetFont())
    control->SetFont(*childResource->GetFont());
  return control;
}

/*
 * Interpret database as a series of resources
 */

bool wxResourceInterpretResources(wxResourceTable& table, PrologDatabase& db)
{
  wxNode *node = db.First();
  while (node)
  {
    PrologExpr *clause = (PrologExpr *)node->Data();
	wxString functor(clause->Functor());

    wxItemResource *item = NULL;
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
      if (item->GetName())
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

wxItemResource *wxResourceInterpretDialog(wxResourceTable& table, PrologExpr *expr, bool isPanel)
{
  wxItemResource *dialogItem = new wxItemResource;
  if (isPanel)
    dialogItem->SetType("wxPanel");
  else
    dialogItem->SetType("wxDialog");
  char *style = NULL;
  char *title = NULL;
  char *name = NULL;
  char *backColourHex = NULL;
  char *labelColourHex = NULL;
  char *buttonColourHex = NULL;

  long windowStyle = wxDEFAULT_DIALOG_STYLE;
  if (isPanel)
    windowStyle = 0;
    
  int x = 0; int y = 0; int width = -1; int height = -1;
  int isModal = 0;
  PrologExpr *labelFontExpr = NULL;
  PrologExpr *buttonFontExpr = NULL;
  PrologExpr *fontExpr = NULL;
  expr->AssignAttributeValue("style", &style);
  expr->AssignAttributeValue("name", &name);
  expr->AssignAttributeValue("title", &title);
  expr->AssignAttributeValue("x", &x);
  expr->AssignAttributeValue("y", &y);
  expr->AssignAttributeValue("width", &width);
  expr->AssignAttributeValue("height", &height);
  expr->AssignAttributeValue("modal", &isModal);
  expr->AssignAttributeValue("label_font", &labelFontExpr);
  expr->AssignAttributeValue("button_font", &buttonFontExpr);
  expr->AssignAttributeValue("font", &fontExpr);
  expr->AssignAttributeValue("background_colour", &backColourHex);
  expr->AssignAttributeValue("label_colour", &labelColourHex);
  expr->AssignAttributeValue("button_colour", &buttonColourHex);

  long id = 0;
  expr->GetAttributeValue("id", id);
  dialogItem->SetId(id);

  if (style)
  {
    windowStyle = wxParseWindowStyle(style);
  }
  dialogItem->SetStyle(windowStyle);
  dialogItem->SetValue1(isModal);
  if (name)
    dialogItem->SetName(name);
  if (title)
    dialogItem->SetTitle(title);
  dialogItem->SetSize(x, y, width, height);
  
  if (backColourHex)
  {
    int r = 0;
    int g = 0;
    int b = 0;
    r = wxHexToDec(backColourHex);
    g = wxHexToDec(backColourHex+2);
    b = wxHexToDec(backColourHex+4);
    dialogItem->SetBackgroundColour(new wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
    delete[] backColourHex;
  }
  if (labelColourHex)
  {
    int r = 0;
    int g = 0;
    int b = 0;
    r = wxHexToDec(labelColourHex);
    g = wxHexToDec(labelColourHex+2);
    b = wxHexToDec(labelColourHex+4);
    dialogItem->SetLabelColour(new wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
    delete[] labelColourHex;
  }
  if (buttonColourHex)
  {
    int r = 0;
    int g = 0;
    int b = 0;
    r = wxHexToDec(buttonColourHex);
    g = wxHexToDec(buttonColourHex+2);
    b = wxHexToDec(buttonColourHex+4);
    dialogItem->SetButtonColour(new wxColour((unsigned char)r,(unsigned char)g,(unsigned char)b));
    delete[] buttonColourHex;
  }

  if (name)
    delete[] name;
  if (title)
    delete[] title;
  if (style)
    delete[] style;

  if (fontExpr)
    dialogItem->SetFont(wxResourceInterpretFontSpec(fontExpr));
  else if (buttonFontExpr)
    dialogItem->SetFont(wxResourceInterpretFontSpec(buttonFontExpr));
  else if (labelFontExpr)
    dialogItem->SetFont(wxResourceInterpretFontSpec(labelFontExpr));

  // Now parse all controls
  PrologExpr *controlExpr = expr->GetFirst();
  while (controlExpr)
  {
    if (controlExpr->Number() == 3)
    {
      wxString controlKeyword(controlExpr->Nth(1)->StringValue());
      if (controlKeyword != "" && controlKeyword == "control")
      {
        // The value part: always a list.
        PrologExpr *listExpr = controlExpr->Nth(2);
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

wxItemResource *wxResourceInterpretControl(wxResourceTable& table, PrologExpr *expr)
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

  PrologExpr *expr1 = expr->Nth(0);

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
        id = wxResourceGetIdentifier(WXSTRINGCAST str, &table);
        if (id == 0)
        {
          char buf[300];
	  	  sprintf(buf, _("Could not resolve control class or id '%s'. Use (non-zero) integer instead\n or provide #define (see manual for caveats)"),
                 (const char*) expr1->StringValue());
          wxWarning(buf);
		  delete controlItem;
		  return NULL;
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
    windowStyle = wxParseWindowStyle(WXSTRINGCAST style);
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
  controlItem->SetName(WXSTRINGCAST name);
  controlItem->SetTitle(WXSTRINGCAST title);
  controlItem->SetSize(x, y, width, height);
  controlItem->SetType(WXSTRINGCAST controlType);
  controlItem->SetId(id);

  if (controlType == "wxButton")
  {
    // Check for bitmap resource name
    if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
    {
        wxString str(expr->Nth(count)->StringValue());
        controlItem->SetValue4(WXSTRINGCAST str);
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
#if USE_RADIOBUTTON
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
  else if (controlType == "wxText" || controlType == "wxTextCtrl")
  {
    // Check for default value
    if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
	{
      wxString str(expr->Nth(count)->StringValue());
      controlItem->SetValue4(WXSTRINGCAST str);
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
    // Check for bitmap resource name
    if (expr->Nth(count) && ((expr->Nth(count)->Type() == PrologString) || (expr->Nth(count)->Type() == PrologWord)))
    {
      wxString str(expr->Nth(count)->StringValue());
      controlItem->SetValue4(WXSTRINGCAST str);
      count ++;
      if (expr->Nth(count) && expr->Nth(count)->Type() == PrologList)
        controlItem->SetFont(wxResourceInterpretFontSpec(expr->Nth(count)));
    }
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
    PrologExpr *valueList = NULL;

    if ((valueList = expr->Nth(count)) && (valueList->Type() == PrologList))
    {
      wxStringList *stringList = new wxStringList;
      PrologExpr *stringExpr = valueList->GetFirst();
      while (stringExpr)
      {
        stringList->Add(stringExpr->StringValue());
        stringExpr = stringExpr->GetNext();
      }
      controlItem->SetStringValues(stringList);
  	  count ++;

      // Check for wxSINGLE/wxMULTIPLE
      PrologExpr *mult = NULL;
      controlItem->SetValue1(wxLB_SINGLE);
      if ((mult = expr->Nth(count)) && ((mult->Type() == PrologString)||(mult->Type() == PrologWord)))
      {
        wxString m(mult->StringValue());
        if (m == "wxMULTIPLE")
          controlItem->SetValue1(wxLB_MULTIPLE);
        else if (m == "wxEXTENDED")
          controlItem->SetValue1(wxLB_EXTENDED);
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
  else if (controlType == "wxChoice")
  {
    PrologExpr *valueList = NULL;
    // Check for default value list
    if ((valueList = expr->Nth(count)) && (valueList->Type() == PrologList))
    {
      wxStringList *stringList = new wxStringList;
      PrologExpr *stringExpr = valueList->GetFirst();
      while (stringExpr)
      {
        stringList->Add(stringExpr->StringValue());
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
#if USE_COMBOBOX
  else if (controlType == "wxComboBox")
  {
    PrologExpr *textValue = expr->Nth(count);
    if (textValue && (textValue->Type() == PrologString || textValue->Type() == PrologWord))
	{
      wxString str(textValue->StringValue());
      controlItem->SetValue4(WXSTRINGCAST str);

	  count ++;
      
      PrologExpr *valueList = NULL;
      // Check for default value list
      if ((valueList = expr->Nth(count)) && (valueList->Type() == PrologList))
      {
        wxStringList *stringList = new wxStringList;
        PrologExpr *stringExpr = valueList->GetFirst();
        while (stringExpr)
        {
          stringList->Add(stringExpr->StringValue());
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
#if 0
  else if (controlType == "wxRadioBox")
  {
    PrologExpr *valueList = NULL;
    // Check for default value list
    if ((valueList = expr->Nth(count)) && (valueList->Type() == PrologList))
    {
      wxStringList *stringList = new wxStringList;
      PrologExpr *stringExpr = valueList->GetFirst();
      while (stringExpr)
      {
        stringList->Add(stringExpr->StringValue());
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
    return NULL;
  }
  return controlItem;
}

// Forward declaration 
wxItemResource *wxResourceInterpretMenu1(wxResourceTable& table, PrologExpr *expr);

/*
 * Interpet a menu item
 */

wxItemResource *wxResourceInterpretMenuItem(wxResourceTable& table, PrologExpr *expr)
{
  wxItemResource *item = new wxItemResource;
  
  PrologExpr *labelExpr = expr->Nth(0);
  PrologExpr *idExpr = expr->Nth(1);
  PrologExpr *helpExpr = expr->Nth(2);
  PrologExpr *checkableExpr = expr->Nth(3);

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
      item->SetTitle(WXSTRINGCAST str);
    }
    if (idExpr)
    {
      int id = 0;
      // If a string or word, must look up in identifier table.
      if ((idExpr->Type() == PrologString) || (idExpr->Type() == PrologWord))
      {
        wxString str(idExpr->StringValue());
        id = wxResourceGetIdentifier(WXSTRINGCAST str, &table);
        if (id == 0)
        {
          char buf[300];
	  sprintf(buf, _("Could not resolve menu id '%s'. Use (non-zero) integer instead\n or provide #define (see manual for caveats)"),
                 (const char*) idExpr->StringValue());
          wxWarning(buf);
        }
      }
      else if (idExpr->Type() == PrologInteger)
        id = (int)idExpr->IntegerValue();
      item->SetValue1(id);
    }
    if (helpExpr)
    {
      wxString str(helpExpr->StringValue());
      item->SetValue4(WXSTRINGCAST str);
    }
    if (checkableExpr)
      item->SetValue2(checkableExpr->IntegerValue());

    // Find the first expression that's a list, for submenu
    PrologExpr *subMenuExpr = expr->GetFirst();
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
wxItemResource *wxResourceInterpretMenu1(wxResourceTable& table, PrologExpr *expr)
{
  wxItemResource *menu = new wxItemResource;
//  menu->SetType(wxTYPE_MENU);
  menu->SetType("wxMenu");
  PrologExpr *element = expr->GetFirst();
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

wxItemResource *wxResourceInterpretMenu(wxResourceTable& table, PrologExpr *expr)
{
  PrologExpr *listExpr = NULL;
  expr->AssignAttributeValue("menu", &listExpr);
  if (!listExpr)
    return NULL;
  
  wxItemResource *menuResource = wxResourceInterpretMenuItem(table, listExpr);

  if (!menuResource)
    return NULL;
    
  char *name = NULL;
  expr->AssignAttributeValue("name", &name);
  if (name)
  {
    menuResource->SetName(name);
    delete[] name;
  }
  
  return menuResource;
}

wxItemResource *wxResourceInterpretMenuBar(wxResourceTable& table, PrologExpr *expr)
{
  PrologExpr *listExpr = NULL;
  expr->AssignAttributeValue("menu", &listExpr);
  if (!listExpr)
    return NULL;

  wxItemResource *resource = new wxItemResource;
  resource->SetType("wxMenu");
//  resource->SetType(wxTYPE_MENU);
  
  PrologExpr *element = listExpr->GetFirst();
  while (element)
  {
    wxItemResource *menuResource = wxResourceInterpretMenuItem(table, listExpr);
    resource->GetChildren().Append(menuResource);
    element = element->GetNext();
  }

  char *name = NULL;
  expr->AssignAttributeValue("name", &name);
  if (name)
  {
    resource->SetName(name);
    delete[] name;
  }
  
  return resource;
}

wxItemResource *wxResourceInterpretString(wxResourceTable& WXUNUSED(table), PrologExpr *WXUNUSED(expr))
{
  return NULL;
}

wxItemResource *wxResourceInterpretBitmap(wxResourceTable& WXUNUSED(table), PrologExpr *expr)
{
  wxItemResource *bitmapItem = new wxItemResource;
//  bitmapItem->SetType(wxTYPE_BITMAP);
  bitmapItem->SetType("wxBitmap");
  char *name = NULL;
  expr->AssignAttributeValue("name", &name);
  if (name)
  {
    bitmapItem->SetName(name);
    delete[] name;
  }
  // Now parse all bitmap specifications
  PrologExpr *bitmapExpr = expr->GetFirst();
  while (bitmapExpr)
  {
    if (bitmapExpr->Number() == 3)
    {
      wxString bitmapKeyword(bitmapExpr->Nth(1)->StringValue());
      if (bitmapKeyword == "bitmap" || bitmapKeyword == "icon")
      {
        // The value part: always a list.
        PrologExpr *listExpr = bitmapExpr->Nth(2);
        if (listExpr->Type() == PrologList)
        {
          wxItemResource *bitmapSpec = new wxItemResource;
//          bitmapSpec->SetType(wxTYPE_BITMAP);
          bitmapSpec->SetType("wxBitmap");

          // List is of form: [filename, bitmaptype, platform, colours, xresolution, yresolution]
          // where everything after 'filename' is optional.
          PrologExpr *nameExpr = listExpr->Nth(0);
          PrologExpr *typeExpr = listExpr->Nth(1);
          PrologExpr *platformExpr = listExpr->Nth(2);
          PrologExpr *coloursExpr = listExpr->Nth(3);
          PrologExpr *xresExpr = listExpr->Nth(4);
          PrologExpr *yresExpr = listExpr->Nth(5);
          if (nameExpr && nameExpr->StringValue())
          {
            wxString str(nameExpr->StringValue());
            bitmapSpec->SetName(WXSTRINGCAST str);
          }
          if (typeExpr && typeExpr->StringValue())
          {
            wxString str(typeExpr->StringValue());
            bitmapSpec->SetValue1(wxParseWindowStyle(WXSTRINGCAST str));
          }
          else
            bitmapSpec->SetValue1(0);
            
          if (platformExpr && platformExpr->StringValue())
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

wxItemResource *wxResourceInterpretIcon(wxResourceTable& table, PrologExpr *expr)
{
  wxItemResource *item = wxResourceInterpretBitmap(table, expr);
  if (item)
  {
//    item->SetType(wxTYPE_ICON);
    item->SetType("wxIcon");
    return item;
  }
  else
    return NULL;
}

// Interpret list expression as a font
wxFont *wxResourceInterpretFontSpec(PrologExpr *expr)
{
  if (expr->Type() != PrologList)
    return NULL;

  int point = 10;
  int family = wxSWISS;
  int style = wxNORMAL;
  int weight = wxNORMAL;
  int underline = 0;
  wxString faceName("");
  
  PrologExpr *pointExpr = expr->Nth(0);
  PrologExpr *familyExpr = expr->Nth(1);
  PrologExpr *styleExpr = expr->Nth(2);
  PrologExpr *weightExpr = expr->Nth(3);
  PrologExpr *underlineExpr = expr->Nth(4);
  PrologExpr *faceNameExpr = expr->Nth(5);
  if (pointExpr)
    point = (int)pointExpr->IntegerValue();

  wxString str;
  if (familyExpr)
  {
    str = familyExpr->StringValue();
    family = (int)wxParseWindowStyle(WXSTRINGCAST str);
  }
  if (styleExpr)
  {
    str = styleExpr->StringValue();
    style = (int)wxParseWindowStyle(WXSTRINGCAST str);
  }
  if (weightExpr)
  {
    str = weightExpr->StringValue();
    weight = (int)wxParseWindowStyle(WXSTRINGCAST str);
  }
  if (underlineExpr)
    underline = (int)underlineExpr->IntegerValue();
  if (faceNameExpr)
    faceName = faceNameExpr->StringValue();

  char *faceName1 = NULL;
  if (faceName != "")
    faceName1 = WXSTRINGCAST faceName;
  wxFont *font = wxTheFontList->FindOrCreateFont(point, family, style, weight, (underline != 0), faceName1);
  return font;
}

/*
 * (Re)allocate buffer for reading in from resource file
 */

bool wxReallocateResourceBuffer(void)
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
 
bool wxResourceReadOneResource(FILE *fd, PrologDatabase& db, bool *eof, wxResourceTable *table)
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
      char buf[300];
      sprintf(buf, _("#define %s must be an integer."), name);
      wxWarning(buf);
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
      char buf[400];
      sprintf(buf, _("Could not find resource include file %s."), actualName);
      wxWarning(buf);
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
    wxWarning(buf);
    return FALSE;
  }

  // char
  if (!wxGetResourceToken(fd))
  {
    wxWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "char") != 0)
  {
    wxWarning(_("Expected 'char' whilst parsing resource."));
    return FALSE;
  }
    
  // *name
  if (!wxGetResourceToken(fd))
  {
    wxWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (wxResourceBuffer[0] != '*')
  {
    wxWarning(_("Expected '*' whilst parsing resource."));
    return FALSE;
  }
  char nameBuf[100];
  strncpy(nameBuf, wxResourceBuffer+1, 99);
    
  // =
  if (!wxGetResourceToken(fd))
  {
    wxWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "=") != 0)
  {
    wxWarning(_("Expected '=' whilst parsing resource."));
    return FALSE;
  }

  // String
  if (!wxGetResourceToken(fd))
  {
    wxWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }
  else
  {
    if (!db.ReadPrologFromString(wxResourceBuffer))
    {
      char buf[300];
      sprintf(buf, _("%s: ill-formed resource file syntax."), nameBuf);
      wxWarning(buf);
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

char *wxResourceParseWord(char *s, int *i)
{
  if (!s)
    return NULL;

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
    return NULL;
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
  { "wxNEEDED_SB", wxNEEDED_SB },
  { "wxALWAYS_SB", wxALWAYS_SB },
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
  { "wxDEFAULT_FRAME", wxDEFAULT_FRAME},
  { "wxDEFAULT_DIALOG_STYLE", wxDEFAULT_DIALOG_STYLE},
  { "wxBORDER", wxBORDER},
  { "wxRETAINED", wxRETAINED},
  { "wxEDITABLE", wxEDITABLE},
  { "wxREADONLY", wxREADONLY},
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

long wxParseWindowStyle(char *bitListString)
{
  int i = 0;
  char *word;
  long bitList = 0;
  while ((word = wxResourceParseWord(bitListString, &i)))
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
      char buf[200];
      sprintf(buf, _("Unrecognized style %s whilst parsing resource."), word);
      wxWarning(buf);
      return 0;
    }
  }
  return bitList;
}

/*
 * Load a bitmap from a wxWindows resource, choosing an optimum
 * depth and appropriate type.
 */
 
wxBitmap *wxResourceCreateBitmap(char *resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
    
  wxItemResource *item = table->FindResource(resource);
  if (item)
  {
    if (!item->GetType() || strcmp(item->GetType(), "wxBitmap") != 0)
    {
      char buf[300];
      sprintf(buf, _("%s not a bitmap resource specification."), resource);
      wxWarning(buf);
      return NULL;
    }
    int thisDepth = wxDisplayDepth();
    long thisNoColours = (long)pow(2.0, (double)thisDepth);

    wxItemResource *optResource = NULL;
    
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
#ifdef __X__
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
      return NULL;

    char *name = optResource->GetName();
    int bitmapType = (int)optResource->GetValue1();
    wxBitmap *bitmap = NULL;
    switch (bitmapType)
    {
      case wxBITMAP_TYPE_XBM_DATA:
      {
#ifdef __X__
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, _("Failed to find XBM resource %s.\nForgot to use wxResourceLoadBitmapData?"), name);
          wxWarning(buf);
          return NULL;
        }
        bitmap = new wxBitmap((char *)item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3()); 
#else
        wxWarning(_("No XBM facility available!"));
#endif
        break;
      }
      case wxBITMAP_TYPE_XPM_DATA:
      {
#if (defined(__X__) && USE_XPM_IN_X) || (defined(__WXMSW__) && USE_XPM_IN_MSW)
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, _("Failed to find XPM resource %s.\nForgot to use wxResourceLoadBitmapData?"), name);
          wxWarning(buf);
          return NULL;
        }
        bitmap = new wxBitmap(item->GetValue1());
#else
        wxWarning(_("No XPM facility available!"));
#endif
        break;
      }
      default:
      {
        bitmap = new wxBitmap(name, bitmapType);
        break;
      }
    }
    if (!bitmap)
      return NULL;
      
    if (bitmap->Ok())
    {
      return bitmap;
    }
    else
    {
      delete bitmap;
      return NULL;
    }
  }
  else
  {
    char buf[300];
    sprintf(buf, _("Bitmap resource specification %s not found."), resource);
    wxWarning(buf);
    return NULL;
  }
}

/*
 * Load an icon from a wxWindows resource, choosing an optimum
 * depth and appropriate type.
 */
 
wxIcon *wxResourceCreateIcon(char *resource, wxResourceTable *table)
{
  if (!table)
  table = wxDefaultResourceTable;
  
  wxItemResource *item = table->FindResource(resource);
  if (item)
  {
    if (!item->GetType() || strcmp(item->GetType(), "wxIcon") != 0)
    {
      char buf[300];
      sprintf(buf, _("%s not an icon resource specification."), resource);
      wxWarning(buf);
      return NULL;
    }
    int thisDepth = wxDisplayDepth();
    long thisNoColours = (long)pow(2.0, (double)thisDepth);

    wxItemResource *optResource = NULL;
    
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
#ifdef __X__
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
      return NULL;

    char *name = optResource->GetName();
    int bitmapType = (int)optResource->GetValue1();
    wxIcon *icon = NULL;
    switch (bitmapType)
    {
      case wxBITMAP_TYPE_XBM_DATA:
      {
#ifdef __X__
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, _("Failed to find XBM resource %s.\nForgot to use wxResourceLoadIconData?"), name);
          wxWarning(buf);
          return NULL;
        }
        icon = new wxIcon((char *)item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3()); 
#else
        wxWarning(_("No XBM facility available!"));
#endif
        break;
      }
      case wxBITMAP_TYPE_XPM_DATA:
      {
      // *** XPM ICON NOT YET IMPLEMENTED IN WXWINDOWS ***
/*
#if (defined(__X__) && USE_XPM_IN_X) || (defined(__WXMSW__) && USE_XPM_IN_MSW)
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, _("Failed to find XPM resource %s.\nForgot to use wxResourceLoadIconData?"), name);
          wxWarning(buf);
          return NULL;
        }
        icon = new wxIcon((char **)item->GetValue1());
#else
        wxWarning(_("No XPM facility available!"));
#endif
*/
        wxWarning(_("No XPM icon facility available!"));
        break;
      }
      default:
      {
        icon = new wxIcon(name, bitmapType);
        break;
      }
    }
    if (!icon)
      return NULL;
      
    if (icon->Ok())
    {
      return icon;
    }
    else
    {
      delete icon;
      return NULL;
    }
  }
  else
  {
    char buf[300];
    sprintf(buf, _("Icon resource specification %s not found."), resource);
    wxWarning(buf);
    return NULL;
  }
}

wxMenu *wxResourceCreateMenu(wxItemResource *item)
{
  wxMenu *menu = new wxMenu;
  wxNode *node = item->GetChildren().First();
  while (node)
  {
    wxItemResource *child = (wxItemResource *)node->Data();
    if (child->GetType() && strcmp(child->GetType(), "wxMenuSeparator") == 0)
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

wxMenuBar *wxResourceCreateMenuBar(char *resource, wxResourceTable *table, wxMenuBar *menuBar)
{
  if (!table)
    table = wxDefaultResourceTable;
    
  wxItemResource *menuResource = table->FindResource(resource);
  if (menuResource && menuResource->GetType() && strcmp(menuResource->GetType(), "wxMenu") == 0)
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
  return NULL;
}

wxMenu *wxResourceCreateMenu(char *resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
    
  wxItemResource *menuResource = table->FindResource(resource);
  if (menuResource && menuResource->GetType() && strcmp(menuResource->GetType(), "wxMenu") == 0)
//  if (menuResource && (menuResource->GetType() == wxTYPE_MENU))
    return wxResourceCreateMenu(menuResource);
  return NULL;
}

// Global equivalents (so don't have to refer to default table explicitly)
bool wxResourceParseData(char *resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
    
  return table->ParseResourceData(resource);
}

bool wxResourceParseFile(char *filename, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
    
  return table->ParseResourceFile(filename);
}

// Register XBM/XPM data
bool wxResourceRegisterBitmapData(char *name, char bits[], int width, int height, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
    
  return table->RegisterResourceBitmapData(name, bits, width, height);
}

bool wxResourceRegisterBitmapData(char *name, char **data, wxResourceTable *table)
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

bool wxResourceAddIdentifier(char *name, int value, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
    
  table->identifiers.Put(name, (wxObject *)value);
  return TRUE;
}

int wxResourceGetIdentifier(char *name, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
    
  return (int)table->identifiers.Get(name);
}

/*
 * Parse #include file for #defines (only)
 */

bool wxResourceParseIncludeFile(char *f, wxResourceTable *table)
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

static int ungetc_string(void)
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
 
bool wxResourceReadOneResourceString(char *s, PrologDatabase& db, bool *eof, wxResourceTable *table)
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
      char buf[300];
      sprintf(buf, _("#define %s must be an integer."), name);
      wxWarning(buf);
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
      wxWarning(buf);
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
    wxWarning(buf);
    return FALSE;
  }

  // char
  if (!wxGetResourceTokenString(s))
  {
    wxWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "char") != 0)
  {
    wxWarning(_("Expected 'char' whilst parsing resource."));
    return FALSE;
  }
    
  // *name
  if (!wxGetResourceTokenString(s))
  {
    wxWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (wxResourceBuffer[0] != '*')
  {
    wxWarning(_("Expected '*' whilst parsing resource."));
    return FALSE;
  }
  char nameBuf[100];
  strncpy(nameBuf, wxResourceBuffer+1, 99);
    
  // =
  if (!wxGetResourceTokenString(s))
  {
    wxWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "=") != 0)
  {
    wxWarning(_("Expected '=' whilst parsing resource."));
    return FALSE;
  }

  // String
  if (!wxGetResourceTokenString(s))
  {
    wxWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }
  else
  {
    if (!db.ReadPrologFromString(wxResourceBuffer))
    {
      char buf[300];
      sprintf(buf, _("%s: ill-formed resource file syntax."), nameBuf);
      wxWarning(buf);
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

  PrologDatabase db;
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
  if (!resource || !resource->GetType() ||
    ! ((strcmp(resource->GetType(), "wxDialog") == 0) || (strcmp(resource->GetType(), "wxPanel") == 0)))
    return FALSE;

  char *title = resource->GetTitle();
  long theWindowStyle = resource->GetStyle();
  bool isModal = (resource->GetValue1() != 0);
  int x = resource->GetX();
  int y = resource->GetY();
  int width = resource->GetWidth();
  int height = resource->GetHeight();
  char *name = resource->GetName();

  wxFont *theFont = resource->GetFont();

  if (IsKindOf(CLASSINFO(wxDialog)))
  {
    wxDialog *dialogBox = (wxDialog *)this;
	long modalStyle = isModal ? wxDIALOG_MODAL : 0;
    if (!dialogBox->Create(parent, -1, title, wxPoint(x, y), wxSize(width, height), theWindowStyle|modalStyle, name))
      return FALSE;
    dialogBox->SetClientSize(width, height);
  }
  else
  {
    if (!((wxWindow *)this)->Create(parent, -1, wxPoint(x, y), wxSize(width, height), theWindowStyle, name))
      return FALSE;
  }

  if (theFont)
    SetFont(*theFont);

  if (resource->GetBackgroundColour())
    SetBackgroundColour(*resource->GetBackgroundColour());

	// TODO
  if (resource->GetLabelColour())
    SetForegroundColour(*resource->GetLabelColour());
  else if (resource->GetButtonColour())
    SetForegroundColour(*resource->GetButtonColour());
    
  // Now create children
  wxNode *node = resource->GetChildren().First();
  while (node)
  {
    wxItemResource *childResource = (wxItemResource *)node->Data();
    
    (void) CreateItem(childResource, table);

    node = node->Next();
  }
  return TRUE;
}

wxControl *wxWindow::CreateItem(const wxItemResource *resource, const wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
  return table->CreateItem((wxWindow *)this, (wxItemResource *)resource);
}

#endif // USE_WX_RESOURCES
