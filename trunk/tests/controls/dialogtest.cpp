///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/dialogtest.cpp
// Purpose:     wxWindow unit test
// Author:      Vaclav Slavik
// Created:     2012-08-30
// Copyright:   (c) 2012 Vaclav Slavik
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/testing.h"

#ifdef HAVE_VARIADIC_MACROS

#include "wx/msgdlg.h"
#include "wx/filedlg.h"

// This test suite tests helpers from wx/testing.h intended for testing of code
// that calls modal dialogs. It does not test the implementation of wxWidgets'
// dialogs.
class ModalDialogsTestCase : public CppUnit::TestCase
{
public:
    ModalDialogsTestCase() { }

private:
    CPPUNIT_TEST_SUITE( ModalDialogsTestCase );
        CPPUNIT_TEST( MessageDialog );
        CPPUNIT_TEST( FileDialog );
        CPPUNIT_TEST( CustomDialog );
    CPPUNIT_TEST_SUITE_END();

    void MessageDialog();
    void FileDialog();
    void CustomDialog();

    DECLARE_NO_COPY_CLASS(ModalDialogsTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ModalDialogsTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ModalDialogsTestCase, "ModalDialogsTestCase" );

void ModalDialogsTestCase::MessageDialog()
{
    int rc;

    wxTEST_DIALOG
    (
        rc = wxMessageBox("Should I fail?", "Question", wxYES|wxNO),
        wxExpectModal<wxMessageDialog>(wxNO),
        wxExpectModal<wxFileDialog>(wxGetCwd() + "/test.txt").Optional()
    );

    CPPUNIT_ASSERT_EQUAL(wxNO, rc);
}

void ModalDialogsTestCase::FileDialog()
{
    wxFileDialog dlg(NULL);
    int rc;

    wxTEST_DIALOG
    (
        rc = dlg.ShowModal(),
        wxExpectModal<wxFileDialog>(wxGetCwd() + "/test.txt")
    );

    CPPUNIT_ASSERT_EQUAL((int)wxID_OK, rc);

    CPPUNIT_ASSERT_EQUAL("test.txt", dlg.GetFilename());
}


class MyDialog : public wxDialog
{
public:
    MyDialog(wxWindow *parent) : wxDialog(parent, wxID_ANY, "Entry"), m_value(-1)
    {
        // Dummy. Imagine it's a real dialog that shows some number-entry
        // controls.
    }

    int m_value;
};


template<>
class wxExpectModal<MyDialog> : public wxExpectModalBase<MyDialog>
{
public:
    wxExpectModal(int valueToSet) : m_valueToSet(valueToSet) {}

protected:
    virtual int OnInvoked(MyDialog *dlg) const
    {
        // Simulate the user entering the expected number:
        dlg->m_value = m_valueToSet;
        return wxID_OK;
    }

    int m_valueToSet;
};

void ModalDialogsTestCase::CustomDialog()
{
    MyDialog dlg(NULL);

    wxTEST_DIALOG
    (
        dlg.ShowModal(),
        wxExpectModal<MyDialog>(42)
    );

    CPPUNIT_ASSERT_EQUAL( 42, dlg.m_value );
}

#endif // HAVE_VARIADIC_MACROS
