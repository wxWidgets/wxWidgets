/////////////////////////////////////////////////////////////////////////////
// Name:        windowdeletion
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page overview_windowdeletion Window deletion overview

 Classes: #wxCloseEvent, #wxWindow

 Window deletion can be a confusing subject, so this overview is provided
 to help make it clear when and how you delete windows, or respond to user requests
 to close windows.

 @section sequence What is the sequence of events in a window deletion?

 When the user clicks on the system close button or system close command,
 in a frame or a dialog, wxWidgets calls wxWindow::Close. This
 in turn generates an EVT_CLOSE event: see #wxCloseEvent.

 It is the duty of the application to define a suitable event handler, and
 decide whether or not to destroy the window.
 If the application is for some reason forcing the application to close
 (wxCloseEvent::CanVeto returns @false), the window should always be destroyed, otherwise there is the option to
 ignore the request, or maybe wait until the user has answered a question
 before deciding whether it is safe to close. The handler for EVT_CLOSE should
 signal to the calling code if it does not destroy the window, by calling
 wxCloseEvent::Veto. Calling this provides useful information
 to the calling code.

 The wxCloseEvent handler should only call wxWindow::Destroy to
 delete the window, and not use the @b delete operator. This is because
 for some window classes, wxWidgets delays actual deletion of the window until all events have been processed,
 since otherwise there is the danger that events will be sent to a non-existent window.

 As reinforced in the next section, calling Close does not guarantee that the window
 will be destroyed. Call wxWindow::Destroy if you want to be
 certain that the window is destroyed.

 @section close How can the application close a window itself?

 Your application can either use wxWindow::Close event just as
 the framework does, or it can call wxWindow::Destroy directly.
 If using Close(), you can pass a @true argument to this function to tell the event handler
 that we definitely want to delete the frame and it cannot be vetoed.

 The advantage of using Close instead of Destroy is that it will call any clean-up code
 defined by the EVT_CLOSE handler; for example it may close a document contained in
 a window after first asking the user whether the work should be saved. Close can be vetoed
 by this process (return @false), whereas Destroy definitely destroys the window.

 @section default What is the default behaviour?

 The default close event handler for wxDialog simulates a Cancel command,
 generating a wxID_CANCEL event. Since the handler for this cancel event might
 itself call @b Close, there is a check for infinite looping. The default handler
 for wxID_CANCEL hides the dialog (if modeless) or calls EndModal(wxID_CANCEL) (if modal).
 In other words, by default, the dialog @e is not destroyed (it might have been created
 on the stack, so the assumption of dynamic creation cannot be made).

 The default close event handler for wxFrame destroys the frame using Destroy().

 @section exit What should I do when the user calls up Exit from a menu?

 You can simply call wxWindow::Close on the frame. This
 will invoke your own close event handler which may destroy the frame.

 You can do checking to see if your application can be safely exited at this point,
 either from within your close event handler, or from within your exit menu command
 handler. For example, you may wish to check that all files have been saved.
 Give the user a chance to save and quit, to not save but quit anyway, or to cancel
 the exit command altogether.

 @section upgrade What should I do to upgrade my 1.xx OnClose to 2.0?

 In wxWidgets 1.xx, the @b OnClose function did not actually delete 'this', but signaled
 to the calling function (either @b Close, or the wxWidgets framework) to delete
 or not delete the window.

 To update your code, you should provide an event table entry in your frame or
 dialog, using the EVT_CLOSE macro. The event handler function might look like this:

 @code
 void MyFrame::OnCloseWindow(wxCloseEvent& event)
   {
     if (MyDataHasBeenModified())
     {
       wxMessageDialog* dialog = new wxMessageDialog(this,
         "Save changed data?", "My app", wxYES_NO|wxCANCEL);

       int ans = dialog-ShowModal();
       dialog-Destroy();

       switch (ans)
       {
         case wxID_YES:      // Save, then destroy, quitting app
           SaveMyData();
           this-Destroy();
           break;
         case wxID_NO:       // Don't save; just destroy, quitting app
           this-Destroy();
           break;
         case wxID_CANCEL:   // Do nothing - so don't quit app.
         default:
           if (!event.CanVeto()) // Test if we can veto this deletion
             this-Destroy();    // If not, destroy the window anyway.
           else
             event.Veto();     // Notify the calling code that we didn't delete the frame.
           break;
       }
     }
   }
 @endcode

 @section exit_app How do I exit the application gracefully?

 A wxWidgets application automatically exits when the last top level window
 (#wxFrame or #wxDialog), is destroyed. Put
 any application-wide cleanup code in wxApp::OnExit (this
 is a virtual function, not an event handler).

 @section deletion Do child windows get deleted automatically?

 Yes, child windows are deleted from within the parent destructor. This includes any children
 that are themselves frames or dialogs, so you may wish to close these child frame or dialog windows
 explicitly from within the parent close handler.

 @section window_kinds What about other kinds of window?

 So far we've been talking about 'managed' windows, i.e. frames and dialogs. Windows
 with parents, such as controls, don't have delayed destruction and don't usually have
 close event handlers, though you can implement them if you wish. For consistency,
 continue to use the wxWindow::Destroy function instead
 of the @b delete operator when deleting these kinds of windows explicitly.

 */


