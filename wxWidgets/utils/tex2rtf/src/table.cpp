/////////////////////////////////////////////////////////////////////////////
// Name:        table.cpp
// Purpose:     Utilities for manipulating tables
// Author:      Julian Smart
// Modified by: Wlodzimiez ABX Skiba 2003/2004 Unicode support
//              Ron Lee
// Created:     01/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include "wx/hash.h"

#ifdef new
#undef new
#endif

#if wxUSE_IOSTREAMH
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <ctype.h>
#include "tex2any.h"
#include "table.h"

ColumnData TableData[40];
bool inTabular = false;

bool startRows = false;
bool tableVerticalLineLeft = false;
bool tableVerticalLineRight = false;
int noColumns = 0;   // Current number of columns in table
int ruleTop = 0;
int ruleBottom = 0;
int currentRowNumber = 0;

/*
 * Parse table argument
 *
 */

bool ParseTableArgument(wxChar *value)
{
  noColumns = 0;
  int i = 0;
  int len = wxStrlen(value);
  bool isBorder = false;
  while (i < len)
  {
    int ch = value[i];
    if (ch == '|')
    {
      i ++;
      isBorder = true;
    }
    else if (ch == 'l')
    {
      TableData[noColumns].leftBorder = isBorder;
      TableData[noColumns].rightBorder = false;
      TableData[noColumns].justification = 'l';
      TableData[noColumns].width = 2000; // Estimate
      TableData[noColumns].absWidth = false;
//      TableData[noColumns].spacing = ??
      noColumns ++;
      i ++;
      isBorder = false;
    }
    else if (ch == 'c')
    {
      TableData[noColumns].leftBorder = isBorder;
      TableData[noColumns].rightBorder = false;
      TableData[noColumns].justification = 'c';
      TableData[noColumns].width = defaultTableColumnWidth; // Estimate
      TableData[noColumns].absWidth = false;
//      TableData[noColumns].spacing = ??
      noColumns ++;
      i ++;
      isBorder = false;
    }
    else if (ch == 'r')
    {
      TableData[noColumns].leftBorder = isBorder;
      TableData[noColumns].rightBorder = false;
      TableData[noColumns].justification = 'r';
      TableData[noColumns].width = 2000; // Estimate
      TableData[noColumns].absWidth = false;
//      TableData[noColumns].spacing = ??
      noColumns ++;
      i ++;
      isBorder = false;
    }
    else if (ch == 'p')
    {
      i ++;
      int j = 0;
      wxChar numberBuf[50];
      ch = value[i];
      if (ch == '{')
      {
        i++;
        ch = value[i];
      }

      while ((i < len) && (isdigit(ch) || ch == '.'))
      {
        numberBuf[j] = (wxChar)ch;
        j ++;
        i ++;
        ch = value[i];
      }
      // Assume we have 2 characters for units
      numberBuf[j] = value[i];
      j ++; i++;
      numberBuf[j] = value[i];
      j ++; i++;
      numberBuf[j] = 0;
      if (value[i] == '}') i++;

      TableData[noColumns].leftBorder = isBorder;
      TableData[noColumns].rightBorder = false;
      TableData[noColumns].justification = 'l';
      TableData[noColumns].width = 20*ParseUnitArgument(numberBuf);
      TableData[noColumns].absWidth = true;
//      TableData[noColumns].spacing = ??
      noColumns ++;
      isBorder = false;
    }
    else
    {
      wxChar *buf = new wxChar[wxStrlen(value) + 80];
      wxSnprintf(buf, wxStrlen(value) + 80, _T("Tabular first argument \"%s\" too complex!"), value);
      OnError(buf);
      delete[] buf;
      return false;
    }
  }
  if (isBorder)
    TableData[noColumns-1].rightBorder = true;
  return true;
}
