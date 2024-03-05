# wxQt Architecture                    {#plat_qt_architecture}

## Internals

wxQT uses the same techniques like other ports to wrap the Qt toolkit classes inside the wxWidget hierarchy (especially similar to wxGTK).

### Current (original) Approach

An internal pointer `m_qtWindow` in wxWindow holds the reference to the QWidget (or derived) counterpart, and is accessible through the virtual method `GetHandle`.
This pointer and other window styles are set up in the `PostCreation` method that must be called by the derived classes (mostly controls) to initialize the widget correctly.
Not doing so will cause painting and deletion issues, as the base class  will not know how to handle the Qt widget.
wxControl even provides a protected method `QtCreateControl` that will do the common initialization (including post creation step, moving, sizing, etc., and calling the base to add the child to the parent).

**Warning**: Take care of not calling any function that can raise an assertion before `PostCreation`, for example wxFAIL_MSG, as it will interrupt the normal initialization, hence the later cleanup will crash.
For example, this issue was caused by WXValidateStyle in wxCheckBox::Create, that was "failing silently" in unit tests, and then raising segmentation faults when the object was later deleted (as Qt checkbox counterpart was never being deleted due the aborted initialization).

Many controls have also other pointers to allow to map different sub-widgets and other features.

### New (tentative) Approach

In the other end, Top Level Windows (frames and dialogs) uses directly the internal window pointer, doing a static cast to return the correct type for GetHandle, avoiding multilevel pointer hierarchies.
This would be the ideal solution, but not all classes could be mapped 1:1 and that could introduce potential issues (i.e. invalid static casts) and more boilerplate due to additional specific accessor methods.

For a longer discussion of pro and cons, see [PR#43 comments](https://github.com/reingart/wxWidgets/pull/43)

Note that some special cases are not real windows like the `wxTabFrame` (AUI), so they don't set the internal pointer and hence drawing methods should not be used at all.

### Scroll Areas

In both approaches, special care should be taken with scrolling areas, as Qt manages this ones slightly different to wxWidgets.
`QtGetScrollBarsContainer` should be reimplemented to return the QScrollArea widget or similar (where the scroll bars are places).

That widget should implement a `viewport()` (Qt idiom to differentiate the draw-able area).
Attempts to paint directly to the scroll area itself will fail.
This is already handled in the QtHandlePaintEvent wxWindowQt method.

### wxWidgets to Qt symmetries

Many wxWidgets classes maps 1:1 to Qt ones, but there are some exceptions are (1:N or N:1):

* wxAnyButton (wxButton, wxBitmapButton, wxToggleButton): QPushButton
* wxFrame: QMainWindow with a QWidget inside as central widget
* wxRadioBox: QGroupBox with a QButtonGroup inside
* wxStaticText & wxStaticBitmap: QLabel
* wxTextCtrl: QLineEdit or QTextEdit (multiline)
* wxWindow (wxPanel): QWidget or QScrollArea

### Private helpers

Qt objects needs to be sub-classed to re-implement events and connect signals (more info in [wx-dev forum](https://groups.google.com/d/msg/wx-dev/UpkJMnT3V2o/hIoJwT3qpw4J)):

* Qt events are just virtual methods that needs to be overridden by the derived classes to handle them
* Qt signals can be connected to QObject members or simple functions (thanks to Qt5 new signal slot syntax)

The approach chosen was to use templates to help inherit QObject's (QWidget), providing a common base to handle events and signal infrastructure:

* `wxQtSignalHandler< wxWindow >`: allows emitting wx events for Qt events & signals. This should be used for all QObjects derivatives that are not widgets, for example QAction (used for shortcut / accelerators).
* `wxQtEventSignalHandler< QWidget, wxWindow >`: derived from `wxQtSignalHandler`, also handles basic events (change, focus, mouse, keyboard, paint, close, etc.). This should be used for all QWidget derivatives (controls, top level windows, etc.)

### Delete later

Both templates also have some safety checks to avoid invalid spurious access to deleted wx objects (using a special pointer to the wx instance stored in the Qt object, that is reset to @NULL when the wx counterpart is marked to deletion).

This is due that in some situations, Qt object could still be referenced in the Qt event queue, so it cannot be removed immediately.

**Important**: Currently wxQT is using Qt's `deleteLater` method to avoid this kind of issues.
Please, don't use delete directly except you're confident it will not cause faults or other issues.

Note that no public wxWidget class should be derived directly from QWidget as they could have different lifespans and other implications to run time type systems (RTTI).
Some QObjects are even owned by Qt (for example: menubar, statusbar) and some parents (ie. `QTabWidget`) cannot be deleted immediately in some circumstances (they would cause segmentation faults due spurious events / signals caused by the children destruction if not correctly handled as explained previously)

For more information about the deletion issues, see [deleteLater](https://github.com/reingart/wxWidgets/wiki/WxQtDeleteLaterNotes ) notes and [wx-dev thread](https://groups.google.com/d/msg/wx-dev/H0Xc9aQzaH4/crjFDPsEA0cJ) discussion.

### Files Structure

wxQT follows the same conventions used in other wxWidgets ports:

* Source code is inside src/qt folder
* Headers are inside include/qt folder

There are also some `__WXQT__` guards to enable special features in common / generic code (i.e. event loop, graphic renders, grid)

Although some Qt headers are included in public wx headers, this dependencies should be avoided as this could change in the future (decoupling completely the public wxQT headers from Qt).

Private headers should be include/qt/private, currently they hold:

* converter.h: conversion functions between Qt and wxWidgets for point, rect, size, string, date, orientation and keycodes
* winevent.h: common templates for window event/signal handling (wxQtSignalHandler and wxQtEventSignalHandler)
* utils.h: debug functions and common event utilities

### Adding files

To add a Qt derived class simply put it in a .h file and add the corresponding .cpp file to the build/bakefiles/files.bkl e.g.:


    <set var="QT_LOWLEVEL_HDR" hints="files">
        wx/qt/menuitem.h
    </set>

    <set var="QT_LOWLEVEL_SRC" hints="files">
        src/qt/menuitem.cpp
    </set>

From within of the bakefiles directory, regenerate the autoconf files with:

    bakefile_gen --formats autoconf

Generate the 'configure' script in your wxQt root directory with:

    autoconf

**IMPORTANT NOTE**: The precompilation step (Qt's moc) is no more needed so the build rule was removed. There is no need to use `Q_OBJECT` nor `Q_SLOTS` macros.

    // include/wx/qt/menuitem.h

    class wxMenuItem : public wxMenuItemBase
    {
       // ...
    };

    class wxQtAction : public QAction
    {
    public:
        wxQtAction( wxMenuItem *menuItem, const QString &text, QObject *parent );

    private:
        void OnActionTriggered( bool checked );

    private:
        wxMenuItem *m_menuItem;
    };

### Coding guidelines

* If you leave out an implementation for whatever reason, then mark it with the wxMISSING_IMPLEMENTATION() macro from wx/qt/utils.h i.e.:

        void wxSomeClass::SomeMethod()
        {
            wxMISSING_IMPLEMENTATION( __FUNCTION__ );
        }


    or if only some implementation is missing like evaluating flags:

        void wxSomeClass::SomeMethod( unsigned methodFlags )
        {
            wxMISSING_IMPLEMENTATION( "methodFlags" );
        }

* To avoid name clashes with a 3rd party library like boost, and due precompilation step was removed, don't use the internal moc keywords `signals` and `slots` nor `SIGNAL` / `SLOT` macros for `connect`. Instead, use the "New Signal Slot Qt syntax":

        class wxQtClass : public QObject
        {
            wxQtClass(): QObject()
            {
                connect(this, &QObject::objectNameChanged, this, &wxQtClass::objectNameChanged);
            }
        private:  // "signal slots":
            void objectNameChanged();
        };

### Naming conventions

* Global helper classes and global functions should be prefixed with `wxQt` i.e.:

        class wxQtButton : public QPushButton
        {
        }

        QRect wxQtConvertRect( const wxRect & );

* Public wxWidgets Qt classes should not use any prefix or suffix, except for special cases, for example:

        // no prefix/suffix in most clases:
        class wxButton : public wxButtonBase;

            // suffix for consistency with other ports:
            class wxWindowQt : public wxWindowBase;
            class wxTopLevelWindowQt : public wxTopLevelWindowBase; // (BTW: avoid using Native)

            // special case (to avoid ambiguity with wxQtSpinBoxBase helper
            class wxSpinCtrlQt : public wxSpinCtrlBase;             // (not a wxQt helper)

            // prefix for consistency with other ports:
        class wxQtDCImpl : public wxDCImpl;                     // with wxMSWDCImpl, wxGTKDCImpl

* Internal methods in publicly visible classes (like wxWindowQt) should be prefixed with `Qt` i.e.:

        class wxWindowQt : public wxWindowBase
        {
        public:
            QWidget *QtGetContainer() const;
        };

* Internal private instance variables (like in wxWindowQt) should be prefixed with `'m_qt` i.e.:

        class wxWindow : public wxWindowBase
        {
        private:
            QWidget *m_qtWindow;
            QScrollArea *m_qtContainer;
            QPicture *m_qtPicture;
            QPainter *m_qtPainter;
        };

* Qt derived names should use mixedCase (in helper methods, for example for `clicked` signal, following the original Qt name), but wx methods should be CamelCase (i.e. `Emit`):

        void wxQtPushButton::clicked( bool WXUNUSED(checked) )
        {
            wxAnyButton *handler = GetHandler();
            if ( handler )
            {
                wxCommandEvent event( wxEVT_BUTTON, handler->GetId() );
                EmitEvent( event );
            }
        }
