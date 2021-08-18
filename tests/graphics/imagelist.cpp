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

        idx = il.Add(bmpRGB, *wxRED);
        CHECK(il.GetImageCount() == 3);
        wxBitmap bmp3 = il.GetBitmap(idx);
        CHECK(bmp3.HasAlpha() == false);
        CHECK(bmp3.GetMask() != NULL);
        CHECK(bmp3.GetWidth() == 32);
        CHECK(bmp3.GetHeight() == 32);
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

        idx = il.Add(bmpRGBA, *wxRED);
        CHECK(il.GetImageCount() == 3);
        wxBitmap bmp3 = il.GetBitmap(idx);
        CHECK(bmp3.HasAlpha() == false);
        CHECK(bmp3.GetMask() != NULL);
        CHECK(bmp3.GetWidth() == 32);
        CHECK(bmp3.GetHeight() == 32);
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

    SECTION("Add images with incompatible sizes")
    {
        il.RemoveAll();
        wxSize sz = il.GetSize();

        wxBitmap bmpSmallerW(sz.GetWidth() / 2, sz.GetHeight(), 24);
        {
            wxMemoryDC mdc(bmpSmallerW);
            mdc.SetBackground(*wxBLUE_BRUSH);
            mdc.Clear();
        }
        REQUIRE(bmpSmallerW.IsOk());

        wxBitmap bmpSmallerH(sz.GetWidth(), sz.GetHeight() / 2, 24);
        {
            wxMemoryDC mdc(bmpSmallerH);
            mdc.SetBackground(*wxBLUE_BRUSH);
            mdc.Clear();
        }
        REQUIRE(bmpSmallerH.IsOk());

        wxBitmap bmpSmallerWH(sz.GetWidth() / 2, sz.GetHeight() / 2, 24);
        {
            wxMemoryDC mdc(bmpSmallerWH);
            mdc.SetBackground(*wxBLUE_BRUSH);
            mdc.Clear();
        }
        REQUIRE(bmpSmallerWH.IsOk());

        wxBitmap bmpBiggerW(sz.GetWidth() * 3 / 2, sz.GetHeight(), 24);
        {
            wxMemoryDC mdc(bmpBiggerW);
            mdc.SetBackground(*wxBLUE_BRUSH);
            mdc.Clear();
        }
        REQUIRE(bmpBiggerW.IsOk());

        wxBitmap bmpBiggerW2x(sz.GetWidth() * 2, sz.GetHeight(), 24);
        {
            wxMemoryDC mdc(bmpBiggerW2x);
            mdc.SetBackground(*wxBLUE_BRUSH);
            mdc.Clear();
        }
        REQUIRE(bmpBiggerW2x.IsOk());

        wxBitmap bmpBiggerH(sz.GetWidth(), sz.GetHeight() * 3 / 2, 24);
        {
            wxMemoryDC mdc(bmpBiggerH);
            mdc.SetBackground(*wxBLUE_BRUSH);
            mdc.Clear();
        }
        REQUIRE(bmpBiggerH.IsOk());

        wxBitmap bmpBiggerH2x(sz.GetWidth(), sz.GetHeight() * 2, 24);
        {
            wxMemoryDC mdc(bmpBiggerH2x);
            mdc.SetBackground(*wxBLUE_BRUSH);
            mdc.Clear();
        }
        REQUIRE(bmpBiggerH2x.IsOk());

        wxBitmap bmpBiggerWH(sz.GetWidth() * 3 / 2, sz.GetHeight() * 3 / 2, 24);
        {
            wxMemoryDC mdc(bmpBiggerWH);
            mdc.SetBackground(*wxBLUE_BRUSH);
            mdc.Clear();
        }
        REQUIRE(bmpBiggerWH.IsOk());

        wxBitmap bmpBiggerWH2x(sz.GetWidth() * 2, sz.GetHeight() * 2, 24);
        {
            wxMemoryDC mdc(bmpBiggerWH2x);
            mdc.SetBackground(*wxBLUE_BRUSH);
            mdc.Clear();
        }
        REQUIRE(bmpBiggerWH2x.IsOk());

        // Adding
        int cnt = il.GetImageCount();
        int idx = il.Add(bmpSmallerW);
        CHECK(idx == -1);
        CHECK(il.GetImageCount() == cnt);

        cnt = il.GetImageCount();
        idx = il.Add(bmpSmallerH);
        CHECK(idx >= 0);
        CHECK(il.GetImageCount() == cnt + 1);
        wxBitmap bmp = il.GetBitmap(idx);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        idx = il.Add(bmpSmallerWH);
        CHECK(idx == -1);
        CHECK(il.GetImageCount() == cnt);

        cnt = il.GetImageCount();
        idx = il.Add(bmpBiggerW);
        CHECK(idx >= 0);
        CHECK(il.GetImageCount() == cnt + 1);
        bmp = il.GetBitmap(idx);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        idx = il.Add(bmpBiggerW2x);
        CHECK(idx >= 0);
        CHECK(il.GetImageCount() == cnt + 2);
        bmp = il.GetBitmap(idx);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        idx = il.Add(bmpBiggerH);
        CHECK(idx >= 0);
        CHECK(il.GetImageCount() == cnt + 1);
        bmp = il.GetBitmap(idx);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        idx = il.Add(bmpBiggerH2x);
        CHECK(idx >= 0);
        CHECK(il.GetImageCount() == cnt + 1);
        bmp = il.GetBitmap(idx);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        idx = il.Add(bmpBiggerWH);
        CHECK(idx >= 0);
        CHECK(il.GetImageCount() == cnt + 1);
        bmp = il.GetBitmap(idx);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        idx = il.Add(bmpBiggerWH2x);
        CHECK(idx >= 0);
        CHECK(il.GetImageCount() == cnt + 2);
        bmp = il.GetBitmap(idx);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        // Replacing
        il.RemoveAll();

        cnt = il.GetImageCount();
        bool ok = il.Replace(0, bmpRGBA);
        CHECK(ok == false);
        CHECK(il.GetImageCount() == cnt);

        // List with 1 image
        idx = il.Add(bmpRGB);
        CHECK(idx >= 0);

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpRGBA);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpSmallerW);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpSmallerH);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpSmallerWH);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpBiggerW);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpBiggerW2x);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpBiggerH);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpBiggerH2x);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpBiggerWH);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());

        cnt = il.GetImageCount();
        ok = il.Replace(0, bmpBiggerWH2x);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == cnt);
        bmp = il.GetBitmap(0);
        CHECK(bmp.GetWidth() == sz.GetWidth());
        CHECK(bmp.GetHeight() == sz.GetHeight());
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

        idx = il.Add(bmpRGB, *wxRED);
        CHECK(il.GetImageCount() == 3);
        wxBitmap bmp3 = il.GetBitmap(idx);
        CHECK(bmp3.HasAlpha() == true);
        CHECK(bmp3.GetMask() == NULL);
        CHECK(bmp3.GetWidth() == 32);
        CHECK(bmp3.GetHeight() == 32);
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

        idx = il.Add(bmpRGBA, *wxRED);
        CHECK(il.GetImageCount() == 3);
        wxBitmap bmp3 = il.GetBitmap(idx);
        CHECK(bmp3.HasAlpha() == true);
        CHECK(bmp3.GetMask() == NULL);
        CHECK(bmp3.GetWidth() == 32);
        CHECK(bmp3.GetHeight() == 32);
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

TEST_CASE("ImageList:NegativeTests", "[imagelist][negative]")
{
    wxBitmap bmp(32, 32, 24);
    {
        wxMemoryDC mdc(bmp);
        mdc.SetBackground(*wxBLUE_BRUSH);
        mdc.Clear();
        mdc.SetBrush(*wxRED_BRUSH);
        mdc.DrawRectangle(4, 4, 24, 24);
    }
    REQUIRE(bmp.IsOk());

    SECTION("Invalid size (negative)")
    {
        wxImageList il;
        bool ok = il.Create(-1, -1);
        CHECK_FALSE(ok);
#ifdef __WXDEBUG__
        REQUIRE_THROWS(il.GetImageCount());
#else
        CHECK(il.GetImageCount() == 0);
#endif

        wxSize sz = il.GetSize();
        CHECK(sz.x == 0);
        CHECK(sz.y == 0);

        int w = -1;
        int h = -1;
#ifdef __WXDEBUG__
        REQUIRE_THROWS(il.GetSize(0, w, h));
#else
        ok = il.GetSize(0, w, h);
        CHECK_FALSE(ok);
        CHECK(w == 0);
        CHECK(h == 0);
#endif

        int idx = il.Add(bmp);
        CHECK(idx == -1);
#ifdef __WXDEBUG__
        REQUIRE_THROWS(il.GetImageCount());
#else
        CHECK(il.GetImageCount() == 0);
#endif
    }

    SECTION("Invalid size (zero)")
    {
        wxImageList il;
        bool ok = il.Create(0, 0);
        CHECK_FALSE(ok);
#ifdef __WXDEBUG__
        REQUIRE_THROWS(il.GetImageCount());
#else
        CHECK(il.GetImageCount() == 0);
#endif

        wxSize sz = il.GetSize();
        CHECK(sz.x == 0);
        CHECK(sz.y == 0);

        int w = -1;
        int h = -1;
#ifdef __WXDEBUG__
        REQUIRE_THROWS(ok = il.GetSize(0, w, h));
#else
        ok = il.GetSize(0, w, h);
        CHECK_FALSE(ok);
        CHECK(w == 0);
        CHECK(h == 0);
#endif

        int idx = il.Add(bmp);
        CHECK(idx == -1);
#ifdef __WXDEBUG__
        REQUIRE_THROWS(il.GetImageCount());
#else
        CHECK(il.GetImageCount() == 0);
#endif

        ok = il.Replace(0, bmp);
        CHECK_FALSE(ok);
#ifdef __WXDEBUG__
        REQUIRE_THROWS(il.GetImageCount());
#else
        CHECK(il.GetImageCount() == 0);
#endif
    }

    SECTION("Invalid Get/Replace/Remove indices")
    {
        wxImageList il(32, 32, false);
        CHECK(il.GetImageCount() == 0);

        wxSize sz = il.GetSize();
        CHECK(sz.x == 32);
        CHECK(sz.y == 32);

        int w = -1;
        int h = -1;
        bool ok = il.GetSize(0, w, h);
        CHECK(ok == true);
        CHECK(w == 32);
        CHECK(h == 32);

        int idx = il.Add(bmp);
        CHECK(idx == 0);
        CHECK(il.GetImageCount() == 1);

        wxBitmap bmp2 = il.GetBitmap(-1);
        CHECK_FALSE(bmp2.IsOk());
        CHECK(il.GetImageCount() == 1);

        wxBitmap bmp3 = il.GetBitmap(5);
        CHECK_FALSE(bmp3.IsOk());
        CHECK(il.GetImageCount() == 1);

        wxIcon icon2 = il.GetIcon(-1);
        CHECK_FALSE(icon2.IsOk());
        CHECK(il.GetImageCount() == 1);

        wxBitmap icon3 = il.GetIcon(5);
        CHECK_FALSE(icon3.IsOk());
        CHECK(il.GetImageCount() == 1);

        ok = il.Replace(-1, bmp);
        CHECK_FALSE(ok);
        CHECK(il.GetImageCount() == 1);

        ok = il.Replace(5, bmp);
        CHECK_FALSE(ok);
        CHECK(il.GetImageCount() == 1);

        ok = il.Remove(-1);
        CHECK_FALSE(ok);
        CHECK(il.GetImageCount() == 1);

        ok = il.Remove(5);
        CHECK_FALSE(ok);
        CHECK(il.GetImageCount() == 1);
    }
}

TEST_CASE("ImageList:HiDPI", "[imagelist][hidpi]")
{
#if defined(__WXMSW__)  || ( defined(__WXGTK20__) && !defined(__WXGTK3__) )

    WARN("Skipping HiDPI image tests known not to work in wxMSW and wxGTK2.");
#else
    wxImage img(16, 8);
    img.SetRGB(wxRect(0, 0, 16, 8), 255, 128, 64);
    REQUIRE(img.IsOk());

    wxBitmap bmp1x(img, -1, 1.0);
    REQUIRE(bmp1x.IsOk());
    CHECK(bmp1x.GetWidth() == 16);
    CHECK(bmp1x.GetHeight() == 8);
    CHECK(bmp1x.GetScaledWidth() == 16);
    CHECK(bmp1x.GetScaledHeight() == 8);
    CHECK_FALSE(bmp1x.HasAlpha());
    CHECK(bmp1x.GetMask() == NULL);

    wxBitmap bmp2x(img, -1, 2.0);
    REQUIRE(bmp2x.IsOk());
    CHECK(bmp2x.GetWidth() == 16);
    CHECK(bmp2x.GetHeight() == 8);
    CHECK(bmp2x.GetScaledWidth() == 8);
    CHECK(bmp2x.GetScaledHeight() == 4);
    CHECK_FALSE(bmp2x.HasAlpha());
    CHECK(bmp2x.GetMask() == NULL);

    SECTION("Add images 2x to the list 2x")
    {
        // Logical image size
        wxImageList il(8, 4, false);

        int idx = il.Add(bmp2x);
        CHECK(idx == 0);
        CHECK(il.GetImageCount() == 1);

        idx = il.Add(bmp1x);
        CHECK(idx == -1);
        CHECK(il.GetImageCount() == 1);

        idx = il.Add(bmp2x);
        CHECK(idx == 1);
        CHECK(il.GetImageCount() == 2);

        idx = il.Add(bmp1x);
        CHECK(idx == -1);
        CHECK(il.GetImageCount() == 2);

        wxBitmap bmp = il.GetBitmap(1);
        CHECK(bmp.IsOk() == true);
        CHECK(bmp.GetScaleFactor() == 2.0);
        CHECK(bmp.GetScaledWidth() == 8);
        CHECK(bmp.GetScaledHeight() == 4);
        CHECK(bmp.GetWidth() == 16);
        CHECK(bmp.GetHeight() == 8);
        CHECK_FALSE(bmp.HasAlpha());
        CHECK(bmp.GetMask() == NULL);
    }

    SECTION("Add images 2x to the list 1x")
    {
        // Logical image size
        wxImageList il(16, 8, false);

        int idx = il.Add(bmp1x);
        CHECK(idx == 0);
        CHECK(il.GetImageCount() == 1);

        idx = il.Add(bmp2x);
        CHECK(idx == -1);
        CHECK(il.GetImageCount() == 1);

        idx = il.Add(bmp1x);
        CHECK(idx == 1);
        CHECK(il.GetImageCount() == 2);

        idx = il.Add(bmp2x);
        CHECK(idx == -1);
        CHECK(il.GetImageCount() == 2);

        wxBitmap bmp = il.GetBitmap(1);
        CHECK(bmp.IsOk() == true);
        CHECK(bmp.GetScaleFactor() == 1.0);
        CHECK(bmp.GetScaledWidth() == 16);
        CHECK(bmp.GetScaledHeight() == 8);
        CHECK(bmp.GetWidth() == 16);
        CHECK(bmp.GetHeight() == 8);
        CHECK_FALSE(bmp.HasAlpha());
        CHECK(bmp.GetMask() == NULL);
    }

    SECTION("Replaces images in the list 2x")
    {
        // Logical image size
        wxImageList il(8, 4, false);

        int idx = il.Add(bmp2x);
        CHECK(idx == 0);
        CHECK(il.GetImageCount() == 1);

        idx = il.Add(bmp2x);
        CHECK(idx == 1);
        CHECK(il.GetImageCount() == 2);

        bool ok = il.Replace(1, bmp1x);
        CHECK_FALSE(ok);
        CHECK(il.GetImageCount() == 2);

        ok = il.Replace(0, bmp2x);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == 2);

        wxBitmap bmp = il.GetBitmap(0);
        CHECK(bmp.IsOk() == true);
        CHECK(bmp.GetScaleFactor() == 2.0);
        CHECK(bmp.GetScaledWidth() == 8);
        CHECK(bmp.GetScaledHeight() == 4);
        CHECK(bmp.GetWidth() == 16);
        CHECK(bmp.GetHeight() == 8);
        CHECK_FALSE(bmp.HasAlpha());
        CHECK(bmp.GetMask() == NULL);
    }

    SECTION("Replaces images in the list 1x")
    {
        // Logical image size
        wxImageList il(16, 8, false);

        int idx = il.Add(bmp1x);
        CHECK(idx == 0);
        CHECK(il.GetImageCount() == 1);

        idx = il.Add(bmp1x);
        CHECK(idx == 1);
        CHECK(il.GetImageCount() == 2);

        bool ok = il.Replace(1, bmp2x);
        CHECK_FALSE(ok);
        CHECK(il.GetImageCount() == 2);

        ok = il.Replace(0, bmp1x);
        CHECK(ok == true);
        CHECK(il.GetImageCount() == 2);

        wxBitmap bmp = il.GetBitmap(0);
        CHECK(bmp.GetScaleFactor() == 1.0);
        CHECK(bmp.GetScaledWidth() == 16);
        CHECK(bmp.GetScaledHeight() == 8);
        CHECK(bmp.GetWidth() == 16);
        CHECK(bmp.GetHeight() == 8);
        CHECK_FALSE(bmp.HasAlpha());
        CHECK(bmp.GetMask() == NULL);
    }

    SECTION("Changes list 1x to 2x")
    {
        wxImage img2(32, 16);
        img2.SetRGB(wxRect(0, 0, 32, 16), 255, 128, 64);
        REQUIRE(img2.IsOk());

        wxBitmap bmp2x2(img2, -1, 2.0);
        REQUIRE(bmp2x2.IsOk());
        CHECK(bmp2x2.GetWidth() == 32);
        CHECK(bmp2x2.GetHeight() == 16);
        CHECK(bmp2x2.GetScaledWidth() == 16);
        CHECK(bmp2x2.GetScaledHeight() == 8);
        CHECK(bmp2x2.HasAlpha() == false);
        CHECK(bmp2x2.GetMask() == NULL);

        // Logical image size
        wxImageList il(16, 8, false);

        // Now it should be the list with 1x images
        int idx = il.Add(bmp1x);
        CHECK(idx == 0);
        CHECK(il.GetImageCount() == 1);

        idx = il.Add(bmp1x);
        CHECK(idx == 1);
        CHECK(il.GetImageCount() == 2);

        idx = il.Add(bmp2x2);
        CHECK(idx == -1);
        CHECK(il.GetImageCount() == 2);

        wxBitmap bmp = il.GetBitmap(0);
        CHECK(bmp.GetScaleFactor() == 1.0);
        CHECK(bmp.GetScaledWidth() == 16);
        CHECK(bmp.GetScaledHeight() == 8);
        CHECK(bmp.GetWidth() == 16);
        CHECK(bmp.GetHeight() == 8);
        CHECK_FALSE(bmp.HasAlpha());
        CHECK(bmp.GetMask() == NULL);

        il.RemoveAll();

        // Now it should be the list with 2x images (the same logical size 16x8)
        idx = il.Add(bmp2x2);
        CHECK(idx == 0);
        CHECK(il.GetImageCount() == 1);

        idx = il.Add(bmp2x2);
        CHECK(idx == 1);
        CHECK(il.GetImageCount() == 2);

        idx = il.Add(bmp1x);
        CHECK(idx == -1);
        CHECK(il.GetImageCount() == 2);

        bmp = il.GetBitmap(0);
        CHECK(bmp.GetScaleFactor() == 2.0);
        CHECK(bmp.GetScaledWidth() == 16);
        CHECK(bmp.GetScaledHeight() == 8);
        CHECK(bmp.GetWidth() == 32);
        CHECK(bmp.GetHeight() == 16);
        CHECK_FALSE(bmp.HasAlpha());
        CHECK(bmp.GetMask() == NULL);
    }
#endif // !__WXMSW__
}
