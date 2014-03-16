///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/pickertest.cpp
// Purpose:     Tests for various wxPickerBase based classes
// Author:      Steven Lamerton
// Modified By: Pana Alexandru
// Created:     2010-08-07
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/clrpicker.h"
#include "wx/filepicker.h"
#include "wx/fontpicker.h"
#include "pickerbasetest.h"

#if wxUSE_COLOURPICKERCTRL

class ColourPickerCtrlTestCase : public PickerBaseTestCase,
                                 public CppUnit::TestCase
{
public:
    ColourPickerCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxPickerBase *GetBase() const { return m_colour; }

    CPPUNIT_TEST_SUITE( ColourPickerCtrlTestCase );
        wxPICKER_BASE_TESTS();
    CPPUNIT_TEST_SUITE_END();

    wxColourPickerCtrl *m_colour;

    DECLARE_NO_COPY_CLASS(ColourPickerCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ColourPickerCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ColourPickerCtrlTestCase,
                                      "ColourPickerCtrlTestCase" );

void ColourPickerCtrlTestCase::setUp()
{
    m_colour = new wxColourPickerCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                                     *wxBLACK, wxDefaultPosition,
                                      wxDefaultSize, wxCLRP_USE_TEXTCTRL);
}

void ColourPickerCtrlTestCase::tearDown()
{
    wxDELETE(m_colour);
}

#endif //wxUSE_COLOURPICKERCTRL

#if wxUSE_DIRPICKERCTRL

class DirPickerCtrlTestCase : public PickerBaseTestCase,
                              public CppUnit::TestCase
{
public:
    DirPickerCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxPickerBase *GetBase() const { return m_dir; }

    CPPUNIT_TEST_SUITE( DirPickerCtrlTestCase );
        wxPICKER_BASE_TESTS();
    CPPUNIT_TEST_SUITE_END();

    wxDirPickerCtrl *m_dir;

    DECLARE_NO_COPY_CLASS(DirPickerCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DirPickerCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DirPickerCtrlTestCase,
                                      "DirPickerCtrlTestCase" );

void DirPickerCtrlTestCase::setUp()
{
    m_dir = new wxDirPickerCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                                wxEmptyString, wxDirSelectorPromptStr,
                                wxDefaultPosition, wxDefaultSize,
                                wxDIRP_USE_TEXTCTRL);
}

void DirPickerCtrlTestCase::tearDown()
{
    wxDELETE(m_dir);
}

#endif //wxUSE_DIRPICKERCTRL

#if wxUSE_FILEPICKERCTRL

class FilePickerCtrlTestCase : public PickerBaseTestCase,
                               public CppUnit::TestCase
{
public:
    FilePickerCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxPickerBase *GetBase() const { return m_file; }

    CPPUNIT_TEST_SUITE( FilePickerCtrlTestCase );
        wxPICKER_BASE_TESTS();
    CPPUNIT_TEST_SUITE_END();

    wxFilePickerCtrl *m_file;

    DECLARE_NO_COPY_CLASS(FilePickerCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FilePickerCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FilePickerCtrlTestCase,
                                      "FilePickerCtrlTestCase" );

void FilePickerCtrlTestCase::setUp()
{
    m_file = new wxFilePickerCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                                  wxEmptyString, wxFileSelectorPromptStr,
                                  wxFileSelectorDefaultWildcardStr,
                                  wxDefaultPosition, wxDefaultSize,
                                  wxFLP_USE_TEXTCTRL);
}

void FilePickerCtrlTestCase::tearDown()
{
    wxDELETE(m_file);
}

#endif //wxUSE_FILEPICKERCTRL

#if wxUSE_FONTPICKERCTRL

class FontPickerCtrlTestCase : public PickerBaseTestCase,
                               public CppUnit::TestCase
{
public:
    FontPickerCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxPickerBase *GetBase() const { return m_font; }

    CPPUNIT_TEST_SUITE( FontPickerCtrlTestCase );
        wxPICKER_BASE_TESTS();
        CPPUNIT_TEST( ColourSelection );
    CPPUNIT_TEST_SUITE_END();

    void ColourSelection();

    wxFontPickerCtrl *m_font;

    DECLARE_NO_COPY_CLASS(FontPickerCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FontPickerCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FontPickerCtrlTestCase,
                                      "FontPickerCtrlTestCase" );

void FontPickerCtrlTestCase::setUp()
{
    m_font = new wxFontPickerCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                                  wxNullFont, wxDefaultPosition, wxDefaultSize,
                                  wxFNTP_USE_TEXTCTRL);
}

void FontPickerCtrlTestCase::tearDown()
{
    wxDELETE(m_font);
}

void FontPickerCtrlTestCase::ColourSelection()
{
    wxUint32 colourRGB = 0xFF4269;
    wxColour selectedColour(colourRGB);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Default font picker color must be black",
        m_font->GetSelectedColour().GetRGB(), wxColour(*wxBLACK).GetRGB());

    m_font->SetSelectedColour(selectedColour);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Font picker did not react to color selection",
        m_font->GetSelectedColour().GetRGB(), colourRGB);
}
#endif //wxUSE_FONTPICKERCTRL
