/////////////////////////////////////////////////////////////////////////////
// Name:        commondialogs
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!
 
 @page commondialogs_overview Common dialogs overview
 
 Classes: #wxColourDialog, #wxFontDialog,
 #wxPrintDialog, #wxFileDialog,
 #wxDirDialog, #wxTextEntryDialog,
 #wxPasswordEntryDialog,
 #wxMessageDialog, #wxSingleChoiceDialog,
 #wxMultiChoiceDialog
 Common dialog classes and functions encapsulate commonly-needed dialog box requirements.
 They are all 'modal', grabbing the flow of control until the user dismisses the dialog,
 to make them easy to use within an application.
 Some dialogs have both platform-dependent and platform-independent implementations,
 so that if underlying windowing systems do not provide the required functionality,
 the generic classes and functions can stand in. For example, under MS Windows, wxColourDialog
 uses the standard colour selector. There is also an equivalent called wxGenericColourDialog
 for other platforms, and a macro defines wxColourDialog to be the same as wxGenericColourDialog
 on non-MS Windows platforms. However, under MS Windows, the generic dialog can also be
 used, for testing or other purposes.
 @ref colourdialog_overview
 @ref fontdialog_overview
 @ref printdialog_overview
 @ref filedialog_overview
 @ref dirdialog_overview
 @ref textentrydialog_overview
 @ref passwordentrydialog_overview
 @ref messagedialog_overview
 @ref singlechoicedialog_overview
 @ref multichoicedialog_overview
 
 
 @section wxcolourdialogoverview wxColourDialog overview
 
 Classes: #wxColourDialog, #wxColourData
 The wxColourDialog presents a colour selector to the user, and returns
 with colour information.
 @b The MS Windows colour selector
 Under Windows, the native colour selector common dialog is used. This
 presents a dialog box with three main regions: at the top left, a
 palette of 48 commonly-used colours is shown. Under this, there is a
 palette of 16 'custom colours' which can be set by the application if
 desired. Additionally, the user may open up the dialog box to show
 a right-hand panel containing controls to select a precise colour, and add
 it to the custom colour palette.
 @b The generic colour selector
 Under non-MS Windows platforms, the colour selector is a simulation of
 most of the features of the MS Windows selector. Two palettes of 48
 standard and 16 custom colours are presented, with the right-hand area
 containing three sliders for the user to select a colour from red,
 green and blue components. This colour may be added to the custom colour
 palette, and will replace either the currently selected custom colour,
 or the first one in the palette if none is selected. The RGB colour sliders
 are not optional in the generic colour selector. The generic colour
 selector is also available under MS Windows; use the name
 wxGenericColourDialog.
 @b Example
 In the samples/dialogs directory, there is an example of using
 the wxColourDialog class. Here is an excerpt, which
 sets various parameters of a wxColourData object, including
 a grey scale for the custom colours. If the user did not cancel
 the dialog, the application retrieves the selected colour and
 uses it to set the background of a window.
 
 @code
 wxColourData data;
   data.SetChooseFull(@true);
   for (int i = 0; i  16; i++)
   {
     wxColour colour(i*16, i*16, i*16);
     data.SetCustomColour(i, colour);
   }
       
   wxColourDialog dialog(this, );
   if (dialog.ShowModal() == wxID_OK)
   {
     wxColourData retData = dialog.GetColourData();
     wxColour col = retData.GetColour();
     wxBrush brush(col, wxSOLID);
     myWindow-SetBackground(brush);
     myWindow-Clear();
     myWindow-Refresh();
   }
 @endcode
 
 
 
 @section wxfontdialogoverview wxFontDialog overview
 
 Classes: #wxFontDialog, #wxFontData
 The wxFontDialog presents a font selector to the user, and returns
 with font and colour information.
 @b The MS Windows font selector
 Under Windows, the native font selector common dialog is used. This
 presents a dialog box with controls for font name, point size, style, weight,
 underlining, strikeout and text foreground colour. A sample of the
 font is shown on a white area of the dialog box. Note that
 in the translation from full MS Windows fonts to wxWidgets font
 conventions, strikeout is ignored and a font family (such as
 Swiss or Modern) is deduced from the actual font name (such as Arial
 or Courier).
 @b The generic font selector
 Under non-MS Windows platforms, the font selector is simpler.
 Controls for font family, point size, style, weight,
 underlining and text foreground colour are provided, and
 a sample is shown upon a white background. The generic font selector
 is also available under MS Windows; use the name wxGenericFontDialog.
 @b Example
 In the samples/dialogs directory, there is an example of using
 the wxFontDialog class. The application uses the returned font
 and colour for drawing text on a canvas. Here is an excerpt:
 
 @code
 wxFontData data;
   data.SetInitialFont(canvasFont);
   data.SetColour(canvasTextColour);
       
   wxFontDialog dialog(this, );
   if (dialog.ShowModal() == wxID_OK)
   {
     wxFontData retData = dialog.GetFontData();
     canvasFont = retData.GetChosenFont();
     canvasTextColour = retData.GetColour();
     myWindow-Refresh();
   }
 @endcode
 
 
 @section wxprintdialogoverview wxPrintDialog overview
 
 Classes: #wxPrintDialog, #wxPrintData
 This class represents the print and print setup common dialogs.
 You may obtain a #wxPrinterDC device context from
 a successfully dismissed print dialog.
 The samples/printing example shows how to use it: see @ref printing_overview for
 an excerpt from this example.
 
 @section wxfiledialogoverview wxFileDialog overview
 
 Classes: #wxFileDialog
 Pops up a file selector box. In Windows and GTK2.4+, this is the common
 file selector dialog. In X, this is a file selector box with somewhat less
 functionality. The path and filename are distinct elements of a full file pathname.
 If path is "", the current directory will be used. If filename is "",
 no default filename will be supplied. The wildcard determines what files
 are displayed in the file selector, and file extension supplies a type
 extension for the required filename. Flags may be a combination of wxFD_OPEN,
 wxFD_SAVE, wxFD_OVERWRITE_PROMPT, wxFD_HIDE_READONLY, wxFD_FILE_MUST_EXIST,
 wxFD_MULTIPLE, wxFD_CHANGE_DIR or 0.
 Both the X and Windows versions implement a wildcard filter. Typing a
 filename containing wildcards (*, ?) in the filename text item, and
 clicking on Ok, will result in only those files matching the pattern being
 displayed. In the X version, supplying no default name will result in the
 wildcard filter being inserted in the filename text item; the filter is
 ignored if a default name is supplied.
 The wildcard may be a specification for multiple
 types of file with a description for each, such as:
 
 @code
 "BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"
 @endcode
 
 
 @section wxdirdialogoverview wxDirDialog overview
 
 Classes: #wxDirDialog
 This dialog shows a directory selector dialog, allowing the user to select a single
 directory.
 
 @section wxtextentrydialogoverview wxTextEntryDialog overview
 
 Classes: #wxTextEntryDialog
 This is a dialog with a text entry field. The value that the user
 entered is obtained using wxTextEntryDialog::GetValue.
 
 @section wxpasswordentrydialogoverview wxPasswordEntryDialog overview
 
 Classes: #wxPasswordEntryDialog
 This is a dialog with a password entry field. The value that the user
 entered is obtained using wxTextEntryDialog::GetValue.
 
 @section wxmessagedialogoverview wxMessageDialog overview
 
 Classes: #wxMessageDialog
 This dialog shows a message, plus buttons that can be chosen from OK, Cancel, Yes, and No.
 Under Windows, an optional icon can be shown, such as an exclamation mark or question mark.
 The return value of wxMessageDialog::ShowModal indicates
 which button the user pressed.
 
 @section wxsinglechoicedialogoverview wxSingleChoiceDialog overview
 
 Classes: #wxSingleChoiceDialog
 This dialog shows a list of choices, plus OK and (optionally) Cancel. The user can
 select one of them. The selection can be obtained from the dialog as an index,
 a string or client data.
 
 @section wxmultichoicedialogoverview wxMultiChoiceDialog overview
 
 Classes: #wxMultiChoiceDialog
 This dialog shows a list of choices, plus OK and (optionally) Cancel. The user can
 select one or more of them.
 
 */
 
 
