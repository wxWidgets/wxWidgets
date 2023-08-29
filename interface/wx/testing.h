/////////////////////////////////////////////////////////////////////////////
// Name:        wx/testing.h
// Purpose:     Documentation for GUI testing helpers.
// Author:      Vadim Zeitlin
// Created:     2023-01-13 (edit-inherited from wx/testing.h)
// Copyright:   (c) 2012 Vaclav Slavik
//              (c) 2023 Vadim Zeitlin
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/**
    This template is specialized for some of the standard dialog classes and can
    also be specialized outside of the library for the custom dialogs.

    All specializations must derive from wxExpectModalBase<T>.

    @since 2.9.5
 */
template<class T> class wxExpectModal;


/**
    Base class for the expectation of a dialog of the given type T.

    Test code can derive ad hoc classes from this class directly and implement
    its OnInvoked() to perform the necessary actions or derive wxExpectModal<T>
    and implement it once if the implementation of OnInvoked() is always the
    same, i.e. depends just on the type T.

    T must be a class derived from wxDialog and E is the derived class type,
    i.e. this is an example of using CRTP. The default value of E is fine in
    case you're using this class as a base for your wxExpectModal<>
    specialization anyhow but also if you don't use either Optional() or
    Describe() methods, as the derived class type is only needed for them.

    The library provides specialization of this class for several standard
    dialogs, including wxMessageDialog and wxFileDialog.
 */
template<class T, class E = wxExpectModal<T> >
class wxExpectModalBase
{
public:
    typedef T DialogType;
    typedef E ExpectationType;

    /**
        Returns a copy of the expectation where the expected dialog is marked
        as optional.

        Optional dialogs aren't required to appear, it's not an error if they
        don't.
     */
    ExpectationType Optional() const;

    /**
        Sets a description shown in the error message if the expectation fails.

        Using this method with unique descriptions for the different dialogs is
        recommended to make it easier to find out which one of the expected
        dialogs exactly was not shown.
     */
    ExpectationType Describe(const wxString& description) const;

protected:
    /// Returns description of the expected dialog (by default, its class).
    virtual wxString GetDefaultDescription() const;

    /**
        This method is called when ShowModal() was invoked on a dialog of type T.

        @return Return value is used as ShowModal()'s return value.
     */
    virtual int OnInvoked(DialogType *dlg) const = 0;
};


/**
    Expectation of any kind of modal dialog.

    This can be used when the exact type of the dialog doesn't matter and it
    just needs to be dismissed (e.g. by passing `wxID_OK` to the constructor to
    pretend that its "OK" button was pressed).

    @since 2.9.5
*/
class wxExpectAny : public wxExpectDismissableModal<wxDialog>
{
public:
    /**
        Creates an object expecting any kind of dialog.

        The given @a id is returned from wxDialog::ShowModal() call.
     */
    explicit wxExpectAny(int id);
};

/**
    Implementation of wxModalDialogHook for use in testing.

    This class is used by default by wxTEST_DIALOG() macro and is not intended
    for direct use.

    It can, however, be derived from if you wish to customize the default
    behaviour, see wxTEST_DIALOG_HOOK_CLASS.

    @since 2.9.5
*/
class wxTestingModalHook : public wxModalDialogHook
{
public:
    /**
        Constructor used by wxTEST_DIALOG().

        This object is created with the location of the macro containing it by
        wxTEST_DIALOG macro, otherwise it falls back to the location of this
        line itself, which is not very useful, so normally you should provide
        your own values.
     */
    wxTestingModalHook(const char* file = nullptr,
                       int line = 0,
                       const char* func = nullptr);

    /**
        Called to verify that all expectations were met.

        This cannot be done in the destructor, because ReportFailure() may
        throw (either because it's overridden or because wx's assertions
        handling is, globally). And throwing from the destructor would
        introduce all sort of problems, including messing up the order of
        errors in some cases.
     */
    void CheckUnmetExpectations();

    /**
        Add an expectation checked by CheckUnmetExpectations().
     */
    void AddExpectation(const wxModalExpectation& e);

protected:
    /**
        Called to describe a dialog shown unexpectedly.

        This method may be overridden to provide a better description of
        (unexpected) dialogs, e.g. add knowledge of custom dialogs used by the
        program here.

        The default implementation shows the type of the dialog.
     */
    virtual wxString DescribeUnexpectedDialog(wxDialog* dlg) const;

    /**
        Report the given failure.

        This method may be overridden to change the way test failures are
        handled. By default they result in an assertion failure which, of
        course, can itself be customized.
     */
    virtual void ReportFailure(const wxString& msg);
};


/**
    Macro determining the class of modal dialog hook to use.

    By default defined as wxTestingModalHook but can be predefined before
    including this file to the name of a custom class inheriting from it to
    customize its behaviour, e.g. provide better error reporting.
 */
#ifndef wxTEST_DIALOG_HOOK_CLASS
    #define wxTEST_DIALOG_HOOK_CLASS wxTestingModalHook
#endif

/**
    Runs given code with all modal dialogs redirected to wxExpectModal<T>
    hooks, instead of being shown to the user.

    This macro is useful for non-interactive tests that need to call a function
    showing a modal dialog, as it allows to check that the expected dialog is
    indeed shown and then continues the execution with the provided result
    instead of blocking.

    The first argument is any valid expression, typically a function call. The
    remaining arguments are wxExpectModal<T> instances defining the dialogs
    that are expected to be shown, in order of appearance.

    Some typical examples:

    @code
    wxTEST_DIALOG
    (
        rc = dlg.ShowModal(),
        wxExpectModal<wxFileDialog>(wxGetCwd() + "/test.txt")
    );
    @endcode

    Sometimes, the code may show more than one dialog:

    @code
    wxTEST_DIALOG
    (
        RunSomeFunction(),
        wxExpectModal<wxMessageDialog>(wxNO),
        wxExpectModal<MyConfirmationDialog>(wxYES),
        wxExpectModal<wxFileDialog>(wxGetCwd() + "/test.txt")
    );
    @endcode

    Notice that wxExpectModal<T> has some convenience methods for further
    tweaking the expectations. For example, it's possible to mark an expected
    dialog as @em optional for situations when a dialog may be shown, but isn't
    required to, by calling the Optional() method:

    @code
    wxTEST_DIALOG
    (
        RunSomeFunction(),
        wxExpectModal<wxMessageDialog>(wxNO),
        wxExpectModal<wxFileDialog>(wxGetCwd() + "/test.txt").Optional()
    );
    @endcode

    @note By default, errors are reported with wxFAIL_MSG(). You may customize this by
          implementing a class derived from wxTestingModalHook, overriding its
          ReportFailure() method and redefining the wxTEST_DIALOG_HOOK_CLASS
          macro to be the name of this class.

    @note Custom dialogs are supported too. All you have to do is to specialize
          wxExpectModal<> for your dialog type and implement its OnInvoked()
          method.

    @since 2.9.5
 */
#define wxTEST_DIALOG(codeToRun, ...)
