///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/imagelist.cpp
// Purpose:     image list unit tests
// Author:      Artur Wieczorek
// Created:     2021-01-11
// Copyright:   (c) 2021 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/bitmap.h"
#include "wx/graphics.h"
#include "wx/icon.h"
#include "wx/imaglist.h"

#include "wx/dcmemory.h"

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("ImageList:WithMask", "[imagelist][withmask]")
{
    wxInitAllImageHandlers();

    wxBitmap bmpRGB(32, 32, 24);
    {
        wxMemoryDC mdc(bmpRGB);
        mdc.SetBackground(*wxBLUE_BRUSH);
        mdc.Clear();
        mdc.SetBrush(*wxRED_BRUSH);
        mdc.DrawRectangle(4, 4, 24, 24);
    }
    REQUIRE(bmpRGB.IsOk());

    wxBitmap bmpRGBA;
    bmpRGBA.LoadFile("image/wx.png", wxBITMAP_TYPE_PNG);
    REQUIRE(bmpRGBA.IsOk());

    wxBitmap bmpMask(32, 32, 1);
    {
        wxMemoryDC mdc(bmpMask);
#if wxUSE_GRAPHICS_CONTEXT
        wxGraphicsContext* gc = mdc.GetGraphicsContext();
        if ( gc )
            gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif //wxUSE_GRAPHICS_CONTEXT
        mdc.SetBackground(*wxBLACK_BRUSH);
        mdc.Clear();
        mdc.SetBrush(*wxWHITE_BRUSH);
        mdc.DrawRectangle(0, 0, 16, 32);
    }

    wxBitmap bmpRGBWithMask(bmpRGB);
    bmpRGBWithMask.SetMask(new wxMask(bmpMask));
    REQUIRE(bmpRGBWithMask.IsOk());

    wxBitmap bmpRGBAWithMask(bmpRGBA);
    bmpRGBAWithMask.SetMask(new wxMask(bmpMask));
    REQUIRE(bmpRGBAWithMask.IsOk());

    wxIcon ico;
    ico.LoadFile("image/wx.ico", wxBITMAP_TYPE_ICO);
    REQUIRE(ico.IsOk());

    REQUIRE(bmpRGB.HasAlpha() == false);
    REQUIRE(bmpRGB.GetMask() == NULL);

    REQUIRE(bmpRGBWithMask.HasAlpha() == false);
    REQUIRE(bmpRGBWithMask.GetMask() != NULL);

    REQUIRE(bmpRGBA.HasAlpha() == true);
    REQUIRE(bmpRGBA.GetMask() == NULL);

    REQUIRE(bmpRGBAWithMask.HasAlpha() == true);
    REQUIRE(bmpRGBAWithMask.GetMask() != NULL);

    wxImageList il(32, 32, true);

    SECTION("Add RGB image to list")
    {
        il.RemoveAll();
        int idx = il.Add(bmpRGB);
        CHECK(il.GetImageCount() == 1);
        wxBitmap bmp1 = il.GetBitmap(idx);
        CHECK(bmp1.HasAlpha() == false);
        CHECK(bmp1.GetMask() != NULL);
        CHECK(bmp1.GetWidth() == 32);
        CHECK(bmp1.GetHeight() == 32);

        idx = il.Add(bmpRGBWithMask);
        CHECK(il.GetImageCount() == 2);
        wxBitmap bmp2 = il.GetBitmap(idx);
        CHECK(bmp2.HasAlpha() == false);
        CHECK(bmp2.GetMask() != NULL);
        CHECK(bmp2.GetWidth() == 32);
        CHECK(bmp2.GetHeight() == 32);
    }

    SECTION("Add RGBA image to list")
    {
        il.RemoveAll();
        int idx = il.Add(bmpRGBA);
        CHECK(il.GetImageCount() == 1);
        wxBitmap bmp1 = il.GetBitmap(idx);
        CHECK(bmp1.HasAlpha() == false);
        CHECK(bmp1.GetMask() != NULL);
        CHECK(bmp1.GetWidth() == 32);
        CHECK(bmp1.GetHeight() == 32);

        idx = il.Add(bmpRGBAWithMask);
        CHECK(il.GetImageCount() == 2);
        wxBitmap bmp2 = il.GetBitmap(idx);
        CHECK(bmp2.HasAlpha() == false);
        CHECK(bmp2.GetMask() != NULL);
        CHECK(bmp2.GetWidth() == 32);
        CHECK(bmp2.GetHeight() == 32);
    }

    SECTION("Add icon to list")
    {
        il.RemoveAll();
        int idx = il.Add(ico);
        CHECK(il.GetImageCount() == 1);
        wxIcon icon1 = il.GetIcon(idx);
        CHECK(icon1.GetWidth() == 32);
        CHECK(icon1.GetHeight() == 32);
    }

    SECTION("Replace with RGB image")
    {
        il.RemoveAll();
        int idx1 = il.Add(bmpRGBA);
        CHECK(il.GetImageCount() == 1);
        int idx2 = il.Add(bmpRGBAWithMask);
        CHECK(il.GetImageCount() == 2);

        il.Replace(idx1, bmpRGB);
        il.Replace(idx2, bmpRGBWithMask);

        wxBitmap bmp1 = il.GetBitmap(idx1);
        CHECK(bmp1.HasAlpha() == false);
        CHECK(bmp1.GetMask() != NULL);
        CHECK(bmp1.GetWidth() == 32);
        CHECK(bmp1.GetHeight() == 32);

        wxBitmap bmp2 = il.GetBitmap(idx2);
        CHECK(bmp2.HasAlpha() == false);
        CHECK(bmp2.GetMask() != NULL);
        CHECK(bmp2.GetWidth() == 32);
        CHECK(bmp2.GetHeight() == 32);
    }

    SECTION("Replace with RGBA image")
    {
        il.RemoveAll();
        int idx1 = il.Add(bmpRGB);
        CHECK(il.GetImageCount() == 1);
        int idx2 = il.Add(bmpRGBWithMask);
        CHECK(il.GetImageCount() == 2);

        il.Replace(idx1, bmpRGBA);
        il.Replace(idx2, bmpRGBAWithMask);

        wxBitmap bmp1 = il.GetBitmap(idx1);
        CHECK(bmp1.HasAlpha() == false);
        CHECK(bmp1.GetMask() != NULL);
        CHECK(bmp1.GetWidth() == 32);
        CHECK(bmp1.GetHeight() == 32);

        wxBitmap bmp2 = il.GetBitmap(idx2);
        CHECK(bmp2.HasAlpha() == false);
        CHECK(bmp2.GetMask() != NULL);
        CHECK(bmp2.GetWidth() == 32);
        CHECK(bmp2.GetHeight() == 32);
    }
}

TEST_CASE("ImageList:NoMask", "[imagelist][nomask]")
{
    wxInitAllImageHandlers();

    wxBitmap bmpRGB(32, 32, 24);
    {
        wxMemoryDC mdc(bmpRGB);
        mdc.SetBackground(*wxBLUE_BRUSH);
        mdc.Clear();
        mdc.SetBrush(*wxRED_BRUSH);
        mdc.DrawRectangle(4, 4, 24, 24);
    }
    REQUIRE(bmpRGB.IsOk());

    wxBitmap bmpRGBA;
    bmpRGBA.LoadFile("image/wx.png", wxBITMAP_TYPE_PNG);
    REQUIRE(bmpRGBA.IsOk());

    wxBitmap bmpMask(32, 32, 1);
    {
        wxMemoryDC mdc(bmpMask);
#if wxUSE_GRAPHICS_CONTEXT
        wxGraphicsContext* gc = mdc.GetGraphicsContext();
        if ( gc )
            gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif //wxUSE_GRAPHICS_CONTEXT
        mdc.SetBackground(*wxBLACK_BRUSH);
        mdc.Clear();
        mdc.SetBrush(*wxWHITE_BRUSH);
        mdc.DrawRectangle(0, 0, 16, 32);
    }

    wxBitmap bmpRGBWithMask(bmpRGB);
    bmpRGBWithMask.SetMask(new wxMask(bmpMask));
    REQUIRE(bmpRGBWithMask.IsOk());

    wxBitmap bmpRGBAWithMask(bmpRGBA);
    bmpRGBAWithMask.SetMask(new wxMask(bmpMask));
    REQUIRE(bmpRGBAWithMask.IsOk());

    wxIcon ico;
    ico.LoadFile("image/wx.ico", wxBITMAP_TYPE_ICO);
    REQUIRE(ico.IsOk());

    REQUIRE(bmpRGB.HasAlpha() == false);
    REQUIRE(bmpRGB.GetMask() == NULL);

    REQUIRE(bmpRGBWithMask.HasAlpha() == false);
    REQUIRE(bmpRGBWithMask.GetMask() != NULL);

    REQUIRE(bmpRGBA.HasAlpha() == true);
    REQUIRE(bmpRGBA.GetMask() == NULL);

    REQUIRE(bmpRGBAWithMask.HasAlpha() == true);
    REQUIRE(bmpRGBAWithMask.GetMask() != NULL);

    wxImageList il(32, 32, false);

    SECTION("Add RGB image to list")
    {
        il.RemoveAll();
        int idx = il.Add(bmpRGB);
        CHECK(il.GetImageCount() == 1);
        wxBitmap bmp1 = il.GetBitmap(idx);
        CHECK(bmp1.HasAlpha() == false);
        CHECK(bmp1.GetMask() == NULL);
        CHECK(bmp1.GetWidth() == 32);
        CHECK(bmp1.GetHeight() == 32);

        idx = il.Add(bmpRGBWithMask);
        CHECK(il.GetImageCount() == 2);
        wxBitmap bmp2 = il.GetBitmap(idx);
        CHECK(bmp2.HasAlpha() == true);
        CHECK(bmp2.GetMask() == NULL);
        CHECK(bmp2.GetWidth() == 32);
        CHECK(bmp2.GetHeight() == 32);
    }

    SECTION("Add RGBA image to list")
    {
        il.RemoveAll();
        int idx = il.Add(bmpRGBA);
        CHECK(il.GetImageCount() == 1);
        wxBitmap bmp1 = il.GetBitmap(idx);
        CHECK(bmp1.HasAlpha() == true);
        CHECK(bmp1.GetMask() == NULL);
        CHECK(bmp1.GetWidth() == 32);
        CHECK(bmp1.GetHeight() == 32);

        idx = il.Add(bmpRGBAWithMask);
        CHECK(il.GetImageCount() == 2);
        wxBitmap bmp2 = il.GetBitmap(idx);
        CHECK(bmp2.HasAlpha() == true);
        CHECK(bmp2.GetMask() == NULL);
        CHECK(bmp2.GetWidth() == 32);
        CHECK(bmp2.GetHeight() == 32);
    }

    SECTION("Add icon to list")
    {
        il.RemoveAll();
        int idx = il.Add(ico);
        CHECK(il.GetImageCount() == 1);
        wxIcon icon1 = il.GetIcon(idx);
        CHECK(icon1.GetWidth() == 32);
        CHECK(icon1.GetHeight() == 32);
    }

    SECTION("Replace with RGB image")
    {
        il.RemoveAll();
        int idx1 = il.Add(bmpRGBA);
        CHECK(il.GetImageCount() == 1);
        int idx2 = il.Add(bmpRGBAWithMask);
        CHECK(il.GetImageCount() == 2);

        il.Replace(idx1, bmpRGB);
        il.Replace(idx2, bmpRGBWithMask);

        wxBitmap bmp1 = il.GetBitmap(idx1);
        CHECK(bmp1.HasAlpha() == false);
        CHECK(bmp1.GetMask() == NULL);
        CHECK(bmp1.GetWidth() == 32);
        CHECK(bmp1.GetHeight() == 32);

        wxBitmap bmp2 = il.GetBitmap(idx2);
        CHECK(bmp2.HasAlpha() == true);
        CHECK(bmp2.GetMask() == NULL);
        CHECK(bmp2.GetWidth() == 32);
        CHECK(bmp2.GetHeight() == 32);
    }

    SECTION("Replace with RGBA image")
    {
        il.RemoveAll();
        int idx1 = il.Add(bmpRGB);
        CHECK(il.GetImageCount() == 1);
        int idx2 = il.Add(bmpRGBWithMask);
        CHECK(il.GetImageCount() == 2);

        il.Replace(idx1, bmpRGBA);
        il.Replace(idx2, bmpRGBAWithMask);

        wxBitmap bmp1 = il.GetBitmap(idx1);
        CHECK(bmp1.HasAlpha() == true);
        CHECK(bmp1.GetMask() == NULL);
        CHECK(bmp1.GetWidth() == 32);
        CHECK(bmp1.GetHeight() == 32);

        wxBitmap bmp2 = il.GetBitmap(idx2);
        CHECK(bmp2.HasAlpha() == true);
        CHECK(bmp2.GetMask() == NULL);
        CHECK(bmp2.GetWidth() == 32);
        CHECK(bmp2.GetHeight() == 32);
    }
}
