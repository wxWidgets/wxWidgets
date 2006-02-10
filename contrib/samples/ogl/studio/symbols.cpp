/////////////////////////////////////////////////////////////////////////////
// Name:        contrib/samples/ogl/studio/symbols.cpp
// Purpose:     Implements the Studio symbol database
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
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
#include "wx/wx.h"
#endif

#include "wx/ogl/ogl.h" // base header of OGL, includes and adjusts wx/deprecated/setup.h

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
    wxObjectList::compatibility_iterator node = m_symbols.GetFirst();
    while (node)
    {
        csSymbol* symbol = (csSymbol*) node->GetData();
        delete symbol;

        node = node->GetNext();
    }
    m_symbols.Clear();
}

csSymbol* csSymbolDatabase::FindSymbol(const wxString& name) const
{
    wxObjectList::compatibility_iterator node = m_symbols.GetFirst();
    while (node)
    {
        csSymbol* symbol = (csSymbol*) node->GetData();
        if (symbol->GetName() == name)
            return symbol;

        node = node->GetNext();
    }
    return NULL;
}

csSymbol* csSymbolDatabase::FindSymbol(int toolId) const
{
    wxObjectList::compatibility_iterator node = m_symbols.GetFirst();
    while (node)
    {
        csSymbol* symbol = (csSymbol*) node->GetData();
        if (symbol->GetToolId() == toolId)
            return symbol;

        node = node->GetNext();
    }
    return NULL;
}

// Add symbols to database
void csApp::InitSymbols()
{
    m_symbolDatabase = new csSymbolDatabase;

    wxShape* shape = new csCircleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxEmptyString));

    m_symbolDatabase->AddSymbol(new csSymbol(_T("Circle"), shape));

    shape = new csCircleShadowShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxEmptyString));

    m_symbolDatabase->AddSymbol(new csSymbol(_T("Circle shadow"), shape));

    shape = new csThinRectangleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxEmptyString));

    m_symbolDatabase->AddSymbol(new csSymbol(_T("Thin Rectangle"), shape));

    shape = new csWideRectangleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxEmptyString));

    m_symbolDatabase->AddSymbol(new csSymbol(_T("Wide Rectangle"), shape));

    shape = new csSemiCircleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxEmptyString));

    m_symbolDatabase->AddSymbol(new csSymbol(_T("SemiCircle"), shape));

    shape = new csTriangleShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxEmptyString));

    m_symbolDatabase->AddSymbol(new csSymbol(_T("Triangle"), shape));

    shape = new csOctagonShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxEmptyString));

    m_symbolDatabase->AddSymbol(new csSymbol(_T("Octagon"), shape));

    shape = new csGroupShape();
    shape->AssignNewIds();
    shape->SetEventHandler(new csEvtHandler(shape, shape, wxEmptyString));

    m_symbolDatabase->AddSymbol(new csSymbol(_T("Group"), shape));
}

wxBitmap* csSymbolDatabase::CreateToolBitmap(csSymbol* symbol, const wxSize& toolSize)
{
    symbol->GetShape()->Recompute();

    wxBitmap *newBitmap = new wxBitmap(toolSize.x, toolSize.y);

    // Test code
#if 0
    wxMemoryDC memDC;
    memDC.SelectObject(*newBitmap);
    memDC.SetPen(* wxBLACK_PEN);
    memDC.SetBrush(* wxWHITE_BRUSH);
    memDC.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), wxSOLID));
    memDC.SetLogicalFunction(wxCOPY);
    memDC.Clear();

    memDC.DrawLine(0, 0, toolSize.x, toolSize.y);
    memDC.DrawLine(0, toolSize.y, toolSize.x, 0);

    memDC.SelectObject(wxNullBitmap);
#endif

#if 1
    wxMemoryDC memDC;

    double height, width, maxSize;
    symbol->GetShape()->GetBoundingBoxMax(&width, &height);

    if (height > width)
        maxSize = height;
    else
        maxSize = width;

    double borderMargin = 4.0;
    double scaleFactor = (double)(toolSize.x / (maxSize + 2*borderMargin));
    double centreX = (double)((toolSize.x/scaleFactor)/2.0)-1;
    double centreY = centreX;

    memDC.SelectObject(*newBitmap);

    memDC.SetUserScale(scaleFactor, scaleFactor);

    memDC.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), wxSOLID));
    memDC.Clear();

    symbol->GetShape()->Show(true);
    symbol->GetShape()->Move(memDC, centreX, centreY);

    memDC.SelectObject(wxNullBitmap);
#endif

    return newBitmap;
}
