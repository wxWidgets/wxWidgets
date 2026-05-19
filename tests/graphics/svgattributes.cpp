///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/svgattributes.cpp
// Purpose:     wxSVGAttributes unit tests
// Author:      wxWidgets team
// Created:     2026-05-19
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_SVG

#include "wx/dcsvg.h"

TEST_CASE("wxSVGAttributes::Getters", "[svg][attributes]")
{
    wxSVGAttributes attr;
    attr.Role("img")
        .AriaLabel("label")
        .AriaLabelledBy("id1")
        .AriaDescribedBy("id2")
        .AriaHidden(true)
        .AriaDetails("id3")
        .AriaRoleDescription("desc")
        .Id("myid")
        .Class("myclass");

    CHECK( attr.GetRole() == "img" );
    CHECK( attr.GetAriaLabel() == "label" );
    CHECK( attr.GetAriaLabelledBy() == "id1" );
    CHECK( attr.GetAriaDescribedBy() == "id2" );
    CHECK( attr.IsAriaHidden() == true );
    CHECK( attr.GetAriaDetails() == "id3" );
    CHECK( attr.GetAriaRoleDescription() == "desc" );
    CHECK( attr.GetId() == "myid" );
    CHECK( attr.GetClass() == "myclass" );

    attr.AriaHidden(false);
    CHECK( attr.IsAriaHidden() == false );

    attr.Add("custom", "value");
    CHECK( attr.GetAttribute("custom") == "value" );
    CHECK( attr.GetAttribute("nonexistent").empty() );
}

#endif // wxUSE_SVG
