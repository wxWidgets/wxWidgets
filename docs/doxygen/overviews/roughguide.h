/////////////////////////////////////////////////////////////////////////////
// Name:        roughguide.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_roughguide A Quick Guide to Writing Applications

@tableofcontents

@section overview_roughguide_structure Application Structure

To set a wxWidgets application going, you will need to derive a wxApp class and
override wxApp::OnInit() in which you will typically create your application
main top-level window.

This window can be a wxFrame or a wxDialog and may contain one or more
instances of classes such as wxPanel, wxSplitterWindow or other windows and
controls. These windows can be created from C++ code or loaded from resource
definitions in @ref overview_xrc "XRC format".

A frame can have a wxMenuBar, a wxToolBar, a wxStatusBar, and a wxIcon for when
the frame is iconized.

A wxPanel is used to place controls (classes derived from wxControl) which are
used for user interaction. Examples of controls are wxButton, wxCheckBox,
wxChoice, wxListBox, wxRadioBox, and wxSlider. Such controls need to be
positioned correctly -- and also repositioned when the top-level window is
resized by the user -- and to do this you use wxSizer-derived classes, such as
wxBoxSizer and wxFlexGridSizer, to layout everything correctly.

Instances of wxDialog can also be used for controls and they have the advantage
of not requiring a separate panel inside them.

Instead of creating a dialog box and populating it with items, it is possible
to choose one of the convenient common dialog classes, such as wxMessageDialog
and wxFileDialog.


@section overview_roughguide_draw Drawing on the Screen

You never draw directly onto a window -- you use either one of the older
<em>device context</em> (DC) classes or the newer <em>graphics context</em>
(GC) one, that support features such as alpha transparency or anti-aliasing.
wxDC is the base for wxClientDC, wxPaintDC, wxMemoryDC, wxPostScriptDC,
wxMemoryDC, wxMetafileDC and wxPrinterDC. If your drawing functions have wxDC
as a parameter, you can pass any of these DCs to the function, and thus use the
same code to draw to several different devices. You can draw using the member
functions of wxDC, such as wxDC::DrawLine and wxDC::DrawText. Control colour on
a window (wxColour) with brushes (wxBrush) and pens (wxPen).

With wxGraphicsContext, you create it using one of the methods of
wxGraphicsRenderer and then construct your drawing from wxGraphicsPath objects,
finally using wxGraphicsContext::StrokePath() or wxGraphicsContext::FillPath().


@section overview_roughguide_events Event Handling

GUI programs spend most of their time waiting for the user-initiated events --
and then processing them. To do it, you use wxEvtHandler::Bind() to specify the
handler for an event of the given time. Event handlers receive the object
describing the event, such as wxKeyEvent or wxMouseEvent, and perform whichever
action corresponds to it. See @ref overview_events "events handling" overview
for much more information about this subject.

@see @ref group_class

*/
