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
    bool AddSymbol(const wxString& symbol, long id);

// Accessors
    wxString GetSymbolForId(long id);
    long GetIdForSymbol(const wxString& symbol);
    bool SymbolExists(const wxString& symbol) const;

// Implementation

// Member variables
protected:
    wxHashTable m_hashTable;
};

#endif
    // _SYMBTABL_H_

