///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/uiaction.h
// Purpose:     wxUIActionSimulatorImpl declaration
// Author:      Vadim Zeitlin
// Created:     2016-05-21
// Copyright:   (c) 2016 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_UIACTION_H_
#define _WX_PRIVATE_UIACTION_H_

// ----------------------------------------------------------------------------
// Platform-specific implementation of wxUIActionSimulator
// ----------------------------------------------------------------------------

class wxUIActionSimulatorImpl
{
public:
    wxUIActionSimulatorImpl() = default;
    virtual ~wxUIActionSimulatorImpl() = default;

    // Low level mouse methods which must be implemented in the derived class.
    virtual bool MouseMove(long x, long y) = 0;
    virtual bool MouseDown(int button = wxMOUSE_BTN_LEFT) = 0;
    virtual bool MouseUp(int button = wxMOUSE_BTN_LEFT) = 0;

    // Higher level mouse methods which have default implementation in the base
    // class but can be overridden if necessary.
    virtual bool MouseClick(int button = wxMOUSE_BTN_LEFT);
    virtual bool MouseDblClick(int button = wxMOUSE_BTN_LEFT);
    virtual bool MouseDragDrop(long x1, long y1, long x2, long y2,
                               int button = wxMOUSE_BTN_LEFT);

    // The low-level port-specific function which really generates the key
    // presses. It should generate exactly one key event with the given
    // parameters.
    virtual bool DoKey(int keycode, int modifiers, bool isDown) = 0;

private:
    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulatorImpl);
};

#endif // _WX_PRIVATE_UIACTION_H_
