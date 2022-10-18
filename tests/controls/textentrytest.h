///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/textentrytest.h
// Purpose:     Base class implementing wxTextEntry unit tests
// Author:      Vadim Zeitlin
// Created:     2008-09-19 (extracted from textctrltest.cpp)
// Copyright:   (c) 2007, 2008 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_CONTROLS_TEXTENTRYTEST_H_
#define _WX_TESTS_CONTROLS_TEXTENTRYTEST_H_

class WXDLLIMPEXP_FWD_CORE wxTextEntry;

// ----------------------------------------------------------------------------
// abstract base class testing wxTextEntry methods
// ----------------------------------------------------------------------------

class TextEntryTestCase
{
public:
    TextEntryTestCase() { }
    virtual ~TextEntryTestCase() { }

protected:
    // this function must be overridden by the derived classes to return the
    // text entry object we're testing, typically this is done by creating a
    // control implementing wxTextEntry interface in setUp() virtual method and
    // just returning it from here
    virtual wxTextEntry *GetTestEntry() const = 0;

    // and this one must be overridden to return the window which implements
    // wxTextEntry interface -- usually it will return the same pointer as
    // GetTestEntry(), just as a different type
    virtual wxWindow *GetTestWindow() const = 0;

    // this should be inserted in the derived class CPPUNIT_TEST_SUITE
    // definition to run all wxTextEntry tests as part of it
    #define wxTEXT_ENTRY_TESTS() \
        CPPUNIT_TEST( SetValue ); \
        CPPUNIT_TEST( TextChangeEvents ); \
        CPPUNIT_TEST( Selection ); \
        CPPUNIT_TEST( InsertionPoint ); \
        CPPUNIT_TEST( Replace ); \
        WXUISIM_TEST( Editable ); \
        CPPUNIT_TEST( Hint ); \
        CPPUNIT_TEST( CopyPaste ); \
        CPPUNIT_TEST( UndoRedo ); \
        CPPUNIT_TEST( WriteText )

    void SetValue();
    void TextChangeEvents();
    void Selection();
    void InsertionPoint();
    void Replace();
    void Editable();
    void Hint();
    void CopyPaste();
    void UndoRedo();
    void WriteText();

private:
    // Selection() test helper: verify that selection is as described by the
    // function parameters
    void AssertSelection(int from, int to, const char *sel);

    // helper of AssertSelection(): check that the text selected in the control
    // is the given one
    //
    // this is necessary to disable testing this in wxComboBox test as it
    // doesn't provide any way to access the string selection directly, its
    // GetStringSelection() method returns the currently selected string in the
    // wxChoice part of the control, not the selected text
    virtual void CheckStringSelection(const char *sel);

    wxDECLARE_NO_COPY_CLASS(TextEntryTestCase);
};

// Helper used for creating the control of the specific type (currently either
// wxTextCtrl or wxComboBox) with the given flag.
class TextLikeControlCreator
{
public:
    TextLikeControlCreator() {}

    // Create the control of the right type using the given parent and style.
    virtual wxControl* Create(wxWindow* parent, int style) const = 0;

    // Return another creator similar to this one, but creating multiline
    // version of the control. If the returned pointer is non-null, it must be
    // deleted by the caller.
    virtual TextLikeControlCreator* CloneAsMultiLine() const { return nullptr; }

    // Give it a virtual dtor to avoid warnings even though this class is not
    // supposed to be used polymorphically.
    virtual ~TextLikeControlCreator() {}

private:
    wxDECLARE_NO_COPY_CLASS(TextLikeControlCreator);
};

// Use the given control creator to check that various combinations of
// specifying and not specifying wxTE_PROCESS_ENTER and handling or not
// handling the resulting event work as expected.
void TestProcessEnter(const TextLikeControlCreator& controlCreator);

#endif // _WX_TESTS_CONTROLS_TEXTENTRYTEST_H_
