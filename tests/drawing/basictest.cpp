///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/basictest.cpp
// Purpose:     Basic tests for wxGraphicsContext
// Author:      Armel Asselin
// Created:     2014-02-28
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#include "drawing.h"
#include "wx/brush.h"

#if wxUSE_GRAPHICS_CONTEXT && !defined(__WXX11__)

void DoBasicDrawings(wxGraphicsContext *gc)
{
    wxGraphicsBrush gbBackground =
        gc->CreateBrush(wxBrush(wxColour(255, 255, 255)));

    gc->SetBrush(gbBackground);
    gc->DrawRectangle(0, 0, 800, 600);
}

#endif // wxUSE_GRAPHICS_CONTEXT && !defined(__WXX11__)
