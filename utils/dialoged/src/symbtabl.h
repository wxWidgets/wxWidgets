/////////////////////////////////////////////////////////////////////////////
// Name:        symbtabl.h
// Purpose:     wxResourceSymbolTable class for storing/reading #defines
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _SYMBTABL_H_
#define _SYMBTABL_H_

#ifdef __GNUG__
#pragma interface "symbtabl.h"
#endif

class wxResourceSymbolTable: public wxObject
{
public:
    wxResourceSymbolTable();
    ~wxResourceSymbolTable();

// Operations
    bool ReadIncludeFile(const wxString& filename);
    bool WriteIncludeFile(const wxString& filename);
    void Clear();
    bool AddSymbol(const wxString& symbol, int id);
    bool RemoveSymbol(const wxString& symbol);
    bool RemoveSymbol(int id);
    void AddStandardSymbols();
    bool FillComboBox(wxComboBox* comboBox);

// Accessors
    wxString GetSymbolForId(int id);
    int GetIdForSymbol(const wxString& symbol);
    bool SymbolExists(const wxString& symbol) const;
    bool IdExists(int id) ;
    bool IsStandardSymbol(const wxString& symbol) const;
    int FindHighestId() ;

// Implementation

// Member variables
protected:
    wxHashTable m_hashTable;
};

#endif
    // _SYMBTABL_H_

