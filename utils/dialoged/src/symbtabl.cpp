/////////////////////////////////////////////////////////////////////////////
// Name:        symbtabl.cpp
// Purpose:     wxResourceSymbolTable
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "symbtabl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/file.h>

#include <string.h>
#include <stdlib.h>

#include "symbtabl.h"

wxResourceSymbolTable::wxResourceSymbolTable():
  m_hashTable(wxKEY_STRING)
{
}

wxResourceSymbolTable::~wxResourceSymbolTable()
{
    Clear();
}

// Operations

bool wxResourceSymbolTable::ReadIncludeFile(const wxString& filename)
{
    wxFile file;
    if (!file.Open(filename, wxFile::read))
        return FALSE;

    off_t len = file.Length();
    if (len == -1)
        return FALSE;

    wxString str;
    char* p = str.GetWriteBuf(len + 1);

    if (file.Read(p, len) == ofsInvalid)
    {
        str.UngetWriteBuf();
        return FALSE;
    }
    str.UngetWriteBuf();

    // Look for #define occurrences
    size_t pos = str.Find("#define");
    while (pos != -1)
    {
        size_t len = str.Length();

        size_t i = pos + 8;

        // Eat whitespace until symbol
        while ((str[i] == ' ' || str[i] == '\t') && (i < len))
            i ++;

        size_t start = i;

        // Eat symbol
        while (str[i] != ' ' && str[i] != '\t' && (i < len))
            i ++;
        size_t end = i-1;

        wxString symbol(str.Mid(start, (end - start + 1)));

        // Eat whitespace until number
        while ((str[i] == ' ' || str[i] == '\t') && (i < len))
            i ++;

        size_t startNum = i;

        // Eat number
        while (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && (i < len))
            i ++;

        size_t endNum = i-1;

        wxString numStr(str.Mid(startNum, (endNum - startNum + 1)));

        long id = atol(numStr);

        AddSymbol(symbol, id);

        str = str.Right(len - i);
        pos = str.Find("#define");
    }

    return TRUE;
}

bool wxResourceSymbolTable::WriteIncludeFile(const wxString& filename)
{
    wxFile file;
    if (!file.Open(filename, wxFile::write))
        return FALSE;

    m_hashTable.BeginFind();

    wxNode* node = m_hashTable.Next();
    while (node)
    {
        char* str = node->key.string;
        long id = (long) node->Data() ;

        wxString line;
        line.Printf("#define %s %ld\n", str, id);

        file.Write(line, line.Length());

        node = m_hashTable.Next();
    }
    return TRUE;
}

void wxResourceSymbolTable::Clear()
{
    m_hashTable.Clear();
}

bool wxResourceSymbolTable::AddSymbol(const wxString& symbol, long id)
{
    m_hashTable.Put(symbol, (wxObject*) id);
    return TRUE;
}

// Accessors
wxString wxResourceSymbolTable::GetSymbolForId(long id)
{
    m_hashTable.BeginFind();

    wxNode* node = m_hashTable.Next();
    while (node)
    {
        char* str = node->key.string;
        if (str && ( ((long) node->Data()) == id) )
            return wxString(str);

        node = m_hashTable.Next();
    }
    return wxString("");
}

long wxResourceSymbolTable::GetIdForSymbol(const wxString& symbol)
{
    return (long) m_hashTable.Get(symbol);
}

bool wxResourceSymbolTable::SymbolExists(const wxString& symbol) const
{
    return (m_hashTable.Get(symbol) != NULL);
}

