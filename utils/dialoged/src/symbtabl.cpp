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
    if (!wxFileExists(filename))
        return FALSE;

    if (!file.Open(filename, wxFile::read))
        return FALSE;

    off_t len = file.Length();
    if (len == -1)
        return FALSE;

    Clear();
    AddStandardSymbols();

    wxString str;
    char* p = str.GetWriteBuf(len + 1);

    if (file.Read(p, len) == wxFile::fd_invalid)
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

        int id = atol(numStr);

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

    wxString fileOnly(wxFileNameFromPath(filename));
    wxString line;
    line.Printf("/*\n * %s\n * Window identifiers file written by Dialog Editor\n */\n\n",
        (const char*) fileOnly);

    file.Write(line, line.Length());

    m_hashTable.BeginFind();

    wxNode* node = m_hashTable.Next();
    while (node)
    {
        char* str = node->key.string;
        int id = (int) node->Data() ;

        if (!IsStandardSymbol(str))
        {
            wxString line;
            line.Printf("#define %s %ld\n", str, id);

            file.Write(line, line.Length());
        }

        node = m_hashTable.Next();
    }
    return TRUE;
}

void wxResourceSymbolTable::Clear()
{
    m_hashTable.Clear();
}

bool wxResourceSymbolTable::AddSymbol(const wxString& symbol, int id)
{
    m_hashTable.Put(symbol, (wxObject*) id);
    return TRUE;
}

bool wxResourceSymbolTable::RemoveSymbol(const wxString& symbol)
{
    m_hashTable.Delete(symbol);
    return TRUE;
}

bool wxResourceSymbolTable::RemoveSymbol(int id)
{
    wxString symbol(GetSymbolForId(id));
    m_hashTable.Delete(symbol);
    return TRUE;
}

// Accessors
wxString wxResourceSymbolTable::GetSymbolForId(int id)
{
    m_hashTable.BeginFind();

    wxNode* node = m_hashTable.Next();
    while (node)
    {
        char* str = node->key.string;
        if (str && ( ((int) node->Data()) == id) )
            return wxString(str);

        node = m_hashTable.Next();
    }
    return wxString("");
}

int wxResourceSymbolTable::GetIdForSymbol(const wxString& symbol)
{
    return (int) m_hashTable.Get(symbol);
}

bool wxResourceSymbolTable::SymbolExists(const wxString& symbol) const
{
    return (m_hashTable.Get(symbol) != NULL);
}

bool wxResourceSymbolTable::IdExists(int id)
{
    m_hashTable.BeginFind();

    wxNode* node = m_hashTable.Next();
    while (node)
    {
        if ( (((int) node->Data()) == id) )
            return TRUE;

        node = m_hashTable.Next();
    }
    return FALSE;
}

int wxResourceSymbolTable::FindHighestId()
{
    int highest = 0;

    m_hashTable.BeginFind();

    wxNode* node = m_hashTable.Next();
    while (node)
    {
        int id = ((int) node->Data());
        if (id > highest)
            highest = id;

        node = m_hashTable.Next();
    }

    // Make sure we don't clash with future standard wxWindows ids
    if (highest <= wxID_HIGHEST)
        highest = wxID_HIGHEST + 1;
    return highest;
}

/*
 * A table of the standard identifiers
 */

struct wxStandardSymbolStruct
{
    char*       m_name;
    int         m_id;
};

static wxStandardSymbolStruct sg_StandardSymbols[] =
{
    { "wxID_OK",            wxID_OK },
    { "wxID_CANCEL",        wxID_CANCEL },
    { "wxID_APPLY",         wxID_APPLY },
    { "wxID_HELP",          wxID_HELP },
    { "wxID_STATIC",        wxID_STATIC },
    { "wxID_YES",           wxID_YES },
    { "wxID_NO",            wxID_NO },

    { "wxID_OPEN",          wxID_OPEN },
    { "wxID_CLOSE",          wxID_CLOSE },
    { "wxID_NEW",           wxID_NEW },
    { "wxID_SAVE",          wxID_SAVE },
    { "wxID_SAVEAS",        wxID_SAVEAS },
    { "wxID_REVERT",        wxID_REVERT },
    { "wxID_EXIT",          wxID_EXIT },
    { "wxID_UNDO",          wxID_UNDO },
    { "wxID_REDO",          wxID_REDO },
    { "wxID_PRINT",         wxID_PRINT },
    { "wxID_PRINT_SETUP",   wxID_PRINT_SETUP },
    { "wxID_PREVIEW",       wxID_PREVIEW },
    { "wxID_ABOUT",         wxID_ABOUT },
    { "wxID_HELP_CONTENTS", wxID_HELP_CONTENTS },
    { "wxID_HELP_COMMANDS", wxID_HELP_COMMANDS },
    { "wxID_HELP_PROCEDURES", wxID_HELP_PROCEDURES },
    { "wxID_HELP_CONTEXT",  wxID_HELP_CONTEXT },

    { "wxID_CUT",           wxID_CUT },
    { "wxID_COPY",          wxID_COPY },
    { "wxID_PASTE",         wxID_PASTE },
    { "wxID_CLEAR",         wxID_CLEAR },
    { "wxID_FIND",          wxID_FIND },
    { "wxID_DUPLICATE",     wxID_DUPLICATE },

    { "wxID_FILE1",         wxID_FILE1 },
    { "wxID_FILE2",         wxID_FILE2 },
    { "wxID_FILE3",         wxID_FILE3 },
    { "wxID_FILE4",         wxID_FILE4 },
    { "wxID_FILE5",         wxID_FILE5 },
    { "wxID_FILE6",         wxID_FILE6 },
    { "wxID_FILE7",         wxID_FILE7 },
    { "wxID_FILE8",         wxID_FILE8 },
    { "wxID_FILE9",         wxID_FILE9 }

};

static int sg_StandardSymbolSize = (sizeof(sg_StandardSymbols)/sizeof(wxStandardSymbolStruct));

void wxResourceSymbolTable::AddStandardSymbols()
{
    int i;
    for (i = 0; i < sg_StandardSymbolSize; i++)
    {
        AddSymbol(sg_StandardSymbols[i].m_name, sg_StandardSymbols[i].m_id);
    }
}

bool wxResourceSymbolTable::IsStandardSymbol(const wxString& symbol) const
{
    int i;
    for (i = 0; i < sg_StandardSymbolSize; i++)
    {
        if (symbol == sg_StandardSymbols[i].m_name)
            return TRUE;
    }
    return FALSE;
}

bool wxResourceSymbolTable::FillComboBox(wxComboBox* comboBox)
{
    m_hashTable.BeginFind();

    wxNode* node = m_hashTable.Next();
    while (node)
    {
        char* str = node->key.string;

        comboBox->Append(str);
        node = m_hashTable.Next();
    }
    return TRUE;
}

