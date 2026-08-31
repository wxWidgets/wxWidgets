///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/dialogtest.cpp
// Purpose:     wxWindow unit test
// Author:      Vaclav Slavik
// Created:     2012-08-30
// Copyright:   (c) 2012 Vaclav Slavik
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"


#include "wx/testing.h"

#include "wx/msgdlg.h"
#include "wx/filedlg.h"
#include "wx/weakref.h"

#ifdef __WXQT__
    #include <QtCore/QTimer>
    #include <QtWidgets/QDialog>
#endif

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/tlwhostmsw.h"
#endif

// This test suite tests helpers from wx/testing.h intended for testing of code
// that calls modal dialogs. It does not test the implementation of wxWidgets'
// dialogs.

TEST_CASE("Modal::MessageDialog", "[modal]")
{
    int rc;

#if wxUSE_FILEDLG
    #define FILE_DIALOG_TEST ,\
        wxExpectModal<wxFileDialog>(wxGetCwd() + "/test.txt").Optional()
#else
    #define FILE_DIALOG_TEST
#endif

    wxTEST_DIALOG
    (
        rc = wxMessageBox("Should I fail?", "Question", wxYES|wxNO),
        wxExpectModal<wxMessageDialog>(wxNO)
        FILE_DIALOG_TEST
    );

    CHECK( rc == wxNO );
}

#if wxUSE_FILEDLG
TEST_CASE("Modal::FileDialog", "[modal]")
{
#if defined(__WXQT__) && defined(__WINDOWS__)
    WARN("Skipping test known to fail under wxQt for Windows");
    return;
#else
    wxFileDialog dlg(nullptr);
    int rc;

    wxTEST_DIALOG
    (
        rc = dlg.ShowModal(),
        wxExpectModal<wxFileDialog>(wxGetCwd() + "/test.txt")
    );

    CHECK( rc == wxID_OK );

    CHECK( dlg.GetFilename() == "test.txt" );

#ifdef __WXGTK3__
    // The native file dialog in GTK+ 3 launches an async operation which tries
    // to dereference the already deleted dialog object if we don't let it to
    // complete before leaving this function.
    wxYield();
#endif
#endif
}
#endif

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
    virtual int OnInvoked(MyDialog *dlg) const override
    {
        // Simulate the user entering the expected number:
        dlg->m_value = m_valueToSet;
        return wxID_OK;
    }

    int m_valueToSet;
};

TEST_CASE("Modal::CustomDialog", "[modal]")
{
    MyDialog dlg(nullptr);

    wxTEST_DIALOG
    (
        dlg.ShowModal(),
        wxExpectModal<MyDialog>(42)
    );

    CHECK( dlg.m_value == 42 );
}


class MyModalDialog : public wxDialog
{
public:
    MyModalDialog() : wxDialog (nullptr, wxID_ANY, "Modal Dialog")
    {
        m_wasModal = false;
        Bind( wxEVT_INIT_DIALOG, &MyModalDialog::OnInit, this );
    }

    void OnInit(wxInitDialogEvent& WXUNUSED(event))
    {
        m_wasModal = IsModal();
        CallAfter( &MyModalDialog::EndModal, wxID_OK );
    }

    bool WasModal() const
    {
        return m_wasModal;
    }

private:
    bool m_wasModal;
};

TEST_CASE("Modal::InitDialog", "[modal]")
{
    MyModalDialog dlg;
    dlg.ShowModal();
    CHECK( dlg.WasModal() );
}

#ifdef __WXQT__

TEST_CASE("Modal::QtEndModalDuringInit", "[modal][qt]")
{
    wxDialog dialog(nullptr, wxID_ANY, "End modal during initialization");
    int initCalls = 0;
    int expectedCode = wxID_YES;
    int watchdogCalls = 0;

    // This is a fallback for the broken implementation, not the close path:
    // an INIT handler must be able to end the modal session synchronously,
    // without requiring an event-loop callback or any physical input.
    QTimer watchdog;
    watchdog.setSingleShot(true);
    QObject::connect(&watchdog, &QTimer::timeout,
                     [&dialog, &watchdogCalls]()
                     {
                         ++watchdogCalls;
                         dialog.EndModal(wxID_ABORT);
                     });
    dialog.Bind(wxEVT_INIT_DIALOG,
                [&](wxInitDialogEvent&)
                {
                    ++initCalls;
                    CHECK(dialog.IsModal());
                    dialog.EndModal(expectedCode);
                });

    for ( const int code : { wxID_YES, wxID_NO } )
    {
        expectedCode = code;
        watchdog.start(1000);
        const int result = dialog.ShowModal();
        watchdog.stop();

        CHECK(result == code);
        CHECK(watchdogCalls == 0);
        CHECK_FALSE(dialog.IsShown());
        CHECK_FALSE(dialog.GetDialogHandle()->isVisible());
        CHECK_FALSE(dialog.IsModal());
    }
    CHECK(initCalls == 2);
}

TEST_CASE("Modal::QtDeferredModalReuse", "[modal][qt]")
{
    wxDialog dialog(nullptr, wxID_ANY, "Reuse modal dialog");
    int invocation = 0;
    int initCalls = 0;
    int closeCalls = 0;
    int watchdogCalls = 0;
    const int expectedCodes[] = { wxID_YES, wxID_NO, wxID_CANCEL, wxID_OK };

    QTimer watchdog;
    watchdog.setSingleShot(true);
    QObject::connect(&watchdog, &QTimer::timeout,
                     [&dialog, &watchdogCalls]()
                     {
                         ++watchdogCalls;
                         dialog.EndModal(wxID_ABORT);
                     });
    dialog.Bind(wxEVT_INIT_DIALOG,
                [&](wxInitDialogEvent&)
                {
                    ++initCalls;
                    CHECK(dialog.IsModal());
                    dialog.CallAfter([&]()
                    {
                        ++closeCalls;
                        if ( invocation < 2 )
                            dialog.EndModal(expectedCodes[invocation]);
                        else if ( invocation == 2 )
                            dialog.GetDialogHandle()->reject();
                        else
                            dialog.GetDialogHandle()->accept();
                    });
                });

    for ( ; invocation != 4; ++invocation )
    {
        watchdog.start(1000);
        const int result = dialog.ShowModal();
        watchdog.stop();

        CHECK(result == expectedCodes[invocation]);
        CHECK(initCalls == invocation + 1);
        CHECK(closeCalls == invocation + 1);
        CHECK(watchdogCalls == 0);
        CHECK_FALSE(dialog.IsShown());
        CHECK_FALSE(dialog.GetDialogHandle()->isVisible());
        CHECK_FALSE(dialog.IsModal());
    }
}

#endif // __WXQT__

namespace
{

class SelfDestroyingDialog final : public wxDialog
{
public:
    enum class Action
    {
        Validate,
        Transfer
    };

    SelfDestroyingDialog(Action action,
                         int* validateCalls,
                         int* transferCalls)
        : wxDialog(nullptr, wxID_ANY, "Self-destroying dialog"),
          m_action(action),
          m_validateCalls(validateCalls),
          m_transferCalls(transferCalls)
    {
    }

    void AcceptForTest() { AcceptAndClose(); }

    void ApplyForTest()
    {
        wxCommandEvent event(wxEVT_BUTTON, wxID_APPLY);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }

    bool Validate() override
    {
        ++*m_validateCalls;
        if ( m_action == Action::Validate )
            Destroy();
        return true;
    }

    bool TransferDataFromWindow() override
    {
        ++*m_transferCalls;
        if ( m_action == Action::Transfer )
            Destroy();
        return true;
    }

private:
    const Action m_action;
    int* const m_validateCalls;
    int* const m_transferCalls;
};

} // anonymous namespace

TEST_CASE("Dialog::DestroyDuringValidation", "[dialog][lifetime]")
{
    for ( const SelfDestroyingDialog::Action action :
          { SelfDestroyingDialog::Action::Validate,
            SelfDestroyingDialog::Action::Transfer } )
    {
        for ( const bool apply : { false, true } )
        {
            CAPTURE(action, apply);
            int validateCalls = 0;
            int transferCalls = 0;
            SelfDestroyingDialog* const dialog =
                new SelfDestroyingDialog(action,
                                         &validateCalls,
                                         &transferCalls);
            const wxWeakRef<wxWindow> weakDialog(dialog);

#if defined(__WXWINUI__) && wxUSE_WINUI3
            {
                wxWinUITLWHostWindowEventGuard retainedCallback(dialog);
                if ( apply )
                    dialog->ApplyForTest();
                else
                    dialog->AcceptForTest();

                CHECK(wxWinUITLWHostIsDestroyScheduled(dialog));
                CHECK(weakDialog.get() == dialog);
                CHECK(validateCalls == 1);
                CHECK(transferCalls ==
                      (action == SelfDestroyingDialog::Action::Transfer
                           ? 1
                           : 0));
            }
#else
            if ( apply )
                dialog->ApplyForTest();
            else
                dialog->AcceptForTest();

            CHECK(validateCalls == 1);
            CHECK(transferCalls ==
                  (action == SelfDestroyingDialog::Action::Transfer ? 1 : 0));
#endif

            REQUIRE(WaitFor("self-destroying dialog cleanup",
                            [&weakDialog]()
                            {
                                return weakDialog.get() == nullptr;
                            }));
        }
    }
}
