/////////////////////////////////////////////////////////////////////////////
// Name:        symbols.cpp
// Purpose:     Implements the Studio symbol database
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/wxexpr.h>

#include "studio.h"
#include "doc.h"
#include "shapes.h"
#include "view.h"
#include "symbols.h"

/*
 * csSymbol
 * Represents information about a symbol.
 */

csSymbol::csSymbol(const wxString& name, wxShape* shape)
{
    m_name = name;
    m_shape = shape;
    m_toolId = 0;
}

csSymbol::~csSymbol()
{
    delete m_shape;
}

/*
 * A table of all possible shapes.
 * We can use this to construct a palette, etc.
 */
csSymbolDatabase::csSymbolDatabase()
{
    m_currentId = 800;
}

csSymbolDatabase::~csSymbolDatabase()
{
    ClearSymbols();
}

void csSymbolDatabase::AddSymbol(csSymbol* symbol)
{
    symbol->SetToolId(m_currentId);
    m_symbols.Append(symbol);

    m_currentId ++;
}

void csSymbolDatabase::ClearSymbols()
{
    wxNode* node = m_symbols.First();
    while (node)
    {
        csSymbol* symbol = (csSymbol*) node->Data();
        delete symbol;

        node = node->Next();
    }
    m_symbols.Clear();
}

csSymbol* csSymbolDatabase::FindSymbol(const wxString& name) const
{
    wxNode* node = m_symbols.First();
    while (node)
    {
        csSymbol* symbol = (csSymbol*) node->Data();
        if (symbol->GetName() == name)
            return symbol;

        node = node->Next();
    }
    return NULL;
}

csSymbol* csSymbolDatabase::FindSymbol(int toolId) const
{
    wxNode* node = m_symbols.First();
    while (node)
    {
        csSymbol* symbol = (csSymbol*) node->Data();
        if (symbol->GetToolId() == toolId)
            return symbol;

        node = node->Next();
    }
    return NULL;
}

// Add symbols to database
void csApp::InitSymbols()
{
    m_symbolDatabase = new csSymbolDatabase;

    wxShape* shape = new csCircleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxString("")));

    m_symbolDatabase->AddSymbol(new csSymbol("Circle", shape));

    shape = new csCircleShadowShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxString("")));

    m_symbolDatabase->AddSymbol(new csSymbol("Circle shadow", shape));

    shape = new csThinRectangleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxString("")));

    m_symbolDatabase->AddSymbol(new csSymbol("Thin Rectangle", shape));

    shape = new csWideRectangleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxString("")));

    m_symbolDatabase->AddSymbol(new csSymbol("Wide Rectangle", shape));

    shape = new csSemiCircleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxString("")));

    m_symbolDatabase->AddSymbol(new csSymbol("SemiCircle", shape));

    shape = new csTriangleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxString("")));

    m_symbolDatabase->AddSymbol(new csSymbol("Triangle", shape));

    shape = new csOctagonShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxString("")));

    m_symbolDatabase->AddSymbol(new csSymbol("Octagon", shape));

    shape = new csGroupShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxString("")));

    m_symbolDatabase->AddSymbol(new csSymbol("Group", shape));
}

wxBitmap* csSymbolDatabase::CreateToolBitmap(csSymbol* symbol)
{
    int objectBitmapSize = 32;

    symbol->GetShape()->Recompute();

    wxBitmap *newBitmap = new wxBitmap(objectBitmapSize, objectBitmapSize);

    wxMemoryDC memDC;
        
    double height, width, maxSize;
    symbol->GetShape()->GetBoundingBoxMax(&width, &height);

    if (height > width)
        maxSize = height;
    else
        maxSize = width;

    double borderMargin = 4.0;
    double scaleFactor = (double)(objectBitmapSize / (maxSize + 2*borderMargin));
    double centreX = (double)((objectBitmapSize/scaleFactor)/2.0)-1;
    double centreY = centreX;

    memDC.SelectObject(*newBitmap);
    memDC.SetUserScale(scaleFactor, scaleFactor);

    memDC.SetBackground(wxBrush(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE), wxSOLID));
    memDC.Clear();
    symbol->GetShape()->Show(TRUE);
    symbol->GetShape()->Move(memDC, centreX, centreY);
    memDC.SelectObject(wxNullBitmap);

    return newBitmap;
}

