/////////////////////////////////////////////////////////////////////////////
// Name:        reswrite.cpp
// Purpose:     Resource writing functionality
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(__WXMSW__) && !defined(__GNUWIN32__)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include <fstream.h>

#include "wx/scrolbar.h"
#include "wx/string.h"

#include "reseditr.h"

char *SafeString(char *s);
char *SafeWord(char *s);

// Save an association between the child resource and the panel item, to allow
// us not to require unique window names.
wxControl *wxResourceTableWithSaving::CreateItem(wxPanel *panel, wxItemResource *childResource)
{
  wxControl *item = wxResourceTable::CreateItem(panel, childResource);
  if (item)
    wxResourceManager::GetCurrentResourceManager()->GetResourceAssociations().Put((long)childResource, item);
  return item;
}

void wxResourceTableWithSaving::OutputFont(ostream& stream, wxFont *font)
{
  stream << "[" << font->GetPointSize() << ", '";
  stream << font->GetFamilyString() << "', '";
  stream << font->GetStyleString() << "', '";
  stream << font->GetWeightString() << "', ";
  stream << (int)font->GetUnderlined();
  if (font->GetFaceName() != "")
    stream << ", '" << font->GetFaceName() << "'";
  stream << "]";
}

/*
 * Resource table with saving (basic one only has loading)
 */
 
bool wxResourceTableWithSaving::Save(const wxString& filename)
{
  ofstream stream(((wxString &) filename).GetData());
  if (stream.bad())
    return FALSE;
    
  BeginFind();
  wxNode *node = NULL;
  while (node = Next())
  {
    wxItemResource *item = (wxItemResource *)node->Data();
    wxString resType(item->GetType());
    
    if (resType == "wxDialogBox" || resType == "wxDialog" || resType == "wxPanel" || resType == "wxBitmap")
    {
      if (!SaveResource(stream, item))
        return FALSE;
    }
  }
  return TRUE;
}

bool wxResourceTableWithSaving::SaveResource(ostream& stream, wxItemResource *item)
{
  char styleBuf[400];
  wxString itemType(item->GetType());
  
  if (itemType == "wxDialogBox" || itemType == "wxDialog" || itemType == "wxPanel")
    {
      if (itemType == "wxDialogBox" || itemType == "wxDialog")
      {
        stream << "static char *" << item->GetName() << " = \"dialog(name = '" << item->GetName() << "',\\\n";
        GenerateDialogStyleString(item->GetStyle(), styleBuf);
      }
      else
      {
        stream << "static char *" << item->GetName() << " = \"panel(name = '" << item->GetName() << "',\\\n";
        GeneratePanelStyleString(item->GetStyle(), styleBuf);
      }
      stream << "  style = '" << styleBuf << "',\\\n";
      stream << "  title = '" << item->GetTitle() << "',\\\n";
      stream << "  x = " << item->GetX() << ", y = " << item->GetY();
      stream << ", width = " << item->GetWidth() << ", height = " << item->GetHeight();
//      stream << "  modal = " << item->GetValue1();
      
      if (1) // item->GetStyle() & wxNO_3D)
      {
        if (item->GetBackgroundColour())
        {
          char buf[7];
          wxDecToHex(item->GetBackgroundColour()->Red(), buf);
          wxDecToHex(item->GetBackgroundColour()->Green(), buf+2);
          wxDecToHex(item->GetBackgroundColour()->Blue(), buf+4);
          buf[6] = 0;

          stream << ",\\\n  " << "background_colour = '" << buf << "'";
        }
#if 0
        if (item->GetLabelColour())
        {
          char buf[7];
          wxDecToHex(item->GetLabelColour()->Red(), buf);
          wxDecToHex(item->GetLabelColour()->Green(), buf+2);
          wxDecToHex(item->GetLabelColour()->Blue(), buf+4);
          buf[6] = 0;

          stream << ",\\\n  " << "label_colour = '" << buf << "'";
        }
        if (item->GetButtonColour())
        {
          char buf[7];
          wxDecToHex(item->GetButtonColour()->Red(), buf);
          wxDecToHex(item->GetButtonColour()->Green(), buf+2);
          wxDecToHex(item->GetButtonColour()->Blue(), buf+4);
          buf[6] = 0;

          stream << ",\\\n  " << "button_colour = '" << buf << "'";
        }
#endif

      }
      
      if (item->GetFont() && item->GetFont()->Ok())
      {
        stream << ",\\\n  font = ";
        OutputFont(stream, item->GetFont());
      }

      if (item->GetChildren().Number() > 0)
        stream << ",\\\n";
      else
        stream << "\\\n";
      wxNode *node = item->GetChildren().First();
      while (node)
      {
        wxItemResource *child = (wxItemResource *)node->Data();
        
        stream << "  control = [";
        
        SaveResource(stream, child);

        stream << "]";

        if (node->Next())
          stream << ",\\\n";
        node = node->Next();
      }
      stream << ").\";\n\n";
    }
  else if (itemType == "wxButton" || itemType == "wxBitmapButton")
    {
      GenerateButtonStyleString(item->GetStyle(), styleBuf);
      stream << itemType << ", " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();
      if (item->GetValue4())
        stream << ", '" << item->GetValue4() << "'";
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxStaticText" || itemType == "wxStaticBitmap")
    {
      GenerateMessageStyleString(item->GetStyle(), styleBuf);
      stream << itemType << ", " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();
      if (item->GetValue4())
        stream << ", '" << item->GetValue4() << "'";
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxCheckBox")
    {
      GenerateCheckBoxStyleString(item->GetStyle(), styleBuf);
      stream << "wxCheckBox, " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();
      stream << ", " << item->GetValue1();
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxStaticBox")
    {
      GenerateGroupBoxStyleString(item->GetStyle(), styleBuf);
      stream << "wxGroupBox, " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxText" || itemType == "wxMultiText" || itemType == "wxTextCtrl")
    {
      GenerateTextStyleString(item->GetStyle(), styleBuf);
      stream << "wxTextCtrl, ";
      stream << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();
      stream << ", " << SafeWord(item->GetValue4());
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxGauge")
    {
      GenerateGaugeStyleString(item->GetStyle(), styleBuf);
      stream << "wxGauge, " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();
      stream << ", " << item->GetValue1() << ", " << item->GetValue2();
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxSlider")
    {
      GenerateSliderStyleString(item->GetStyle(), styleBuf);
      stream << "wxSlider, " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();
      stream << ", " << item->GetValue1() << ", " << item->GetValue2() << ", " << item->GetValue3();
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxScrollBar")
    {
      GenerateScrollBarStyleString(item->GetStyle(), styleBuf);
      stream << "wxScrollBar, " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();
      stream << ", " << item->GetValue1() << ", " << item->GetValue2() << ", " << item->GetValue3() << ", ";
      stream << item->GetValue5();
    }
  else if (itemType == "wxListBox")
    {
      GenerateListBoxStyleString(item->GetStyle(), styleBuf);
      stream << "wxListBox, " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();

      // Default list of values

      stream << ", [";
      if (item->GetStringValues())
      {
        wxNode *node = item->GetStringValues()->First();
        while (node)
        {
          char *s = (char *)node->Data();
          stream << SafeWord(s);
          if (node->Next())
            stream << ", ";
          node = node->Next();
        }
      }
      stream << "], ";
      switch (item->GetValue1())
      {
        case wxLB_MULTIPLE:
        {
          stream << "'wxLB_MULTIPLE'";
          break;
        }
        case wxLB_EXTENDED:
        {
          stream << "'wxLB_EXTENDED'";
          break;
        }
        case wxLB_SINGLE:
        default:
        {
          stream << "'wxLB_SINGLE'";
          break;
        }
      }
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxChoice")
    {
      GenerateChoiceStyleString(item->GetStyle(), styleBuf);
      stream << "wxChoice, " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();

      // Default list of values

      stream << ", [";
      if (item->GetStringValues())
      {
        wxNode *node = item->GetStringValues()->First();
        while (node)
        {
          char *s = (char *)node->Data();
          stream << SafeWord(s);
          if (node->Next())
            stream << ", ";
          node = node->Next();
        }
      }
      stream << "]";
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxRadioBox")
    {
      // Must write out the orientation and number of rows/cols!!
      GenerateRadioBoxStyleString(item->GetStyle(), styleBuf);
      stream << "wxRadioBox, " << SafeWord(item->GetTitle()) << ", '" << styleBuf << "', ";
      stream << SafeWord(item->GetName()) << ", " << item->GetX() << ", " << item->GetY() << ", ";
      stream << item->GetWidth() << ", " << item->GetHeight();

      // Default list of values

      stream << ", [";
      if (item->GetStringValues())
      {
        wxNode *node = item->GetStringValues()->First();
        while (node)
        {
          char *s = (char *)node->Data();
          stream << SafeWord(s);
          if (node->Next())
            stream << ", ";
          node = node->Next();
        }
      }
      stream << "], " << item->GetValue1();
      if (item->GetFont())
      {
        stream << ",\\\n      ";
        OutputFont(stream, item->GetFont());
      }
    }
  else if (itemType == "wxBitmap")
    {
      stream << "static char *" << item->GetName() << " = \"bitmap(name = '" << item->GetName() << "',\\\n";
      
      wxNode *node = item->GetChildren().First();
      while (node)
      {
        wxItemResource *child = (wxItemResource *)node->Data();
        stream << "  bitmap = [";
        
        char buf[400];
        strcpy(buf, child->GetName());
#ifdef __WXMSW__
        wxDos2UnixFilename(buf);
#endif

        stream << "'" << buf << "', ";
        
        int bitmapType = (int)child->GetValue1();
        switch (bitmapType)
        {
          case wxBITMAP_TYPE_XBM_DATA:
          {
            stream << "wxBITMAP_TYPE_XBM_DATA";
            break;
          }
          case wxBITMAP_TYPE_XPM_DATA:
          {
            stream << "wxBITMAP_TYPE_XPM_DATA";
            break;
          }
          case wxBITMAP_TYPE_XBM:
          {
            stream << "wxBITMAP_TYPE_XBM";
            break;
          }
          case wxBITMAP_TYPE_XPM:
          {
            stream << "wxBITMAP_TYPE_XPM";
            break;
          }
          case wxBITMAP_TYPE_BMP:
          {
            stream << "wxBITMAP_TYPE_BMP";
            break;
          }
          case wxBITMAP_TYPE_BMP_RESOURCE:
          {
            stream << "wxBITMAP_TYPE_BMP_RESOURCE";
            break;
          }
          case wxBITMAP_TYPE_GIF:
          {
            stream << "wxBITMAP_TYPE_GIF";
            break;
          }
          case wxBITMAP_TYPE_TIF:
          {
            stream << "wxBITMAP_TYPE_TIF";
            break;
          }
          case wxBITMAP_TYPE_ICO:
          {
            stream << "wxBITMAP_TYPE_ICO";
            break;
          }
          case wxBITMAP_TYPE_ICO_RESOURCE:
          {
            stream << "wxBITMAP_TYPE_ICO_RESOURCE";
            break;
          }
          case wxBITMAP_TYPE_CUR:
          {
            stream << "wxBITMAP_TYPE_CUR";
            break;
          }
          case wxBITMAP_TYPE_CUR_RESOURCE:
          {
            stream << "wxBITMAP_TYPE_CUR_RESOURCE";
            break;
          }
          default:
          case wxBITMAP_TYPE_ANY:
          {
            stream << "wxBITMAP_TYPE_ANY";
            break;
          }
        }
        stream << ", ";
        int platform = child->GetValue2();
        switch (platform)
        {
          case RESOURCE_PLATFORM_WINDOWS:
          {
            stream << "'WINDOWS'";
            break;
          }
          case RESOURCE_PLATFORM_X:
          {
            stream << "'X'";
            break;
          }
          case RESOURCE_PLATFORM_MAC:
          {
            stream << "'MAC'";
            break;
          }
          case RESOURCE_PLATFORM_ANY:
          {
            stream << "'ANY'";
            break;
          }
        }
        int noColours = (int)child->GetValue3();
        if (noColours > 0)
          stream << ", " << noColours;

        stream << "]";

        if (node->Next())
         stream << ",\\\n";
        
        node = node->Next();
      }
      stream << ").\";\n\n";
    }
  return TRUE;
}

void wxResourceTableWithSaving::GenerateWindowStyleString(long windowStyle, char *buf)
{
  GenerateStyle(buf, windowStyle, wxNO_3D, "wxNO_3D");
  GenerateStyle(buf, windowStyle, wxVSCROLL, "wxVSCROLL");
  GenerateStyle(buf, windowStyle, wxHSCROLL, "wxHSCROLL");
  GenerateStyle(buf, windowStyle, wxBORDER, "wxBORDER");
}

void wxResourceTableWithSaving::GenerateDialogStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateWindowStyleString(windowStyle, buf);

/*
  GenerateStyle(buf, windowStyle, wxRETAINED, "wxRETAINED");
*/
  if (!GenerateStyle(buf, windowStyle, wxDEFAULT_DIALOG_STYLE, "wxDEFAULT_DIALOG_STYLE"))
  {
    GenerateStyle(buf, windowStyle, wxCAPTION, "wxCAPTION");
    GenerateStyle(buf, windowStyle, wxTHICK_FRAME, "wxTHICK_FRAME");
    GenerateStyle(buf, windowStyle, wxRESIZE_BORDER, "wxRESIZE_BORDER");
    GenerateStyle(buf, windowStyle, wxSYSTEM_MENU, "wxSYSTEM_MENU");
    GenerateStyle(buf, windowStyle, wxMINIMIZE_BOX, "wxMINIMIZE_BOX");
    GenerateStyle(buf, windowStyle, wxMAXIMIZE_BOX, "wxMAXIMIZE_BOX");
  }
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GeneratePanelStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateWindowStyleString(windowStyle, buf);

/*
  GenerateStyle(buf, windowStyle, wxRETAINED, "wxRETAINED");
*/
  if (strlen(buf) == 0)
    strcat(buf, "0");
}


void wxResourceTableWithSaving::GenerateItemStyleString(long windowStyle, char *buf)
{
  GenerateWindowStyleString(windowStyle, buf);
  
  GenerateStyle(buf, windowStyle, wxHORIZONTAL, "wxHORIZONTAL");
  GenerateStyle(buf, windowStyle, wxVERTICAL, "wxVERTICAL");
}

void wxResourceTableWithSaving::GenerateRadioBoxStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateMessageStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateTextStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  GenerateStyle(buf, windowStyle, wxTE_PROCESS_ENTER, "wxTE_PROCESS_ENTER");
  GenerateStyle(buf, windowStyle, wxTE_READONLY, "wxTE_READONLY");
  GenerateStyle(buf, windowStyle, wxTE_PASSWORD, "wxTE_PASSWORD");
  GenerateStyle(buf, windowStyle, wxTE_MULTILINE, "wxTE_MULTILINE");

  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateButtonStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateCheckBoxStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateListBoxStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  GenerateStyle(buf, windowStyle, wxLB_ALWAYS_SB, "wxLB_ALWAYS_SB");
  GenerateStyle(buf, windowStyle, wxLB_SORT,      "wxLB_SORT");
//  GenerateStyle(buf, windowStyle, wxLB_SINGLE,    "wxLB_SINGLE"); // Done already
  GenerateStyle(buf, windowStyle, wxLB_MULTIPLE,  "wxLB_MULTIPLE");
  GenerateStyle(buf, windowStyle, wxLB_EXTENDED,  "wxLB_EXTENDED");

  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateSliderStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateGroupBoxStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateGaugeStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  GenerateStyle(buf, windowStyle, wxGA_PROGRESSBAR, "wxGA_PROGRESSBAR");

  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateChoiceStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

void wxResourceTableWithSaving::GenerateScrollBarStyleString(long windowStyle, char *buf)
{
  buf[0] = 0;
  GenerateItemStyleString(windowStyle, buf);
  
  if (strlen(buf) == 0)
    strcat(buf, "0");
}

bool wxResourceTableWithSaving::GenerateStyle(char *buf, long windowStyle, long flag, char *strStyle)
{
  if ((windowStyle & flag) == flag)
  {
    if (strlen(buf) > 0)
      strcat(buf, " | ");
    strcat(buf, strStyle);
    return TRUE;
  }
  else
    return FALSE;
}

// Returns quoted string or "NULL"
char *SafeString(char *s)
{
  if (!s)
    return "NULL";
  else
  {
    strcpy(wxBuffer, "\"");
    strcat(wxBuffer, s);
    strcat(wxBuffer, "\"");
    return wxBuffer;
  }
}

// Returns quoted string or ''
char *SafeWord(char *s)
{
  if (!s)
    return "''";
  else
  {
    strcpy(wxBuffer, "'");
    strcat(wxBuffer, s);
    strcat(wxBuffer, "'");
    return wxBuffer;
  }
}

