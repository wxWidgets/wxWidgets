/////////////////////////////////////////////////////////////////////////////
// Name:        tipprovider
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page tipprovider_overview wxTipProvider

 This is the class used together with #wxShowTip function.
 It must implement #GetTip function and return the
 current tip from it (different tip each time it is called).
 You will never use this class yourself, but you need it to show startup tips
 with wxShowTip. Also, if you want to get the tips text from elsewhere than a
 simple text file, you will want to derive a new class from wxTipProvider and
 use it instead of the one returned by #wxCreateFileTipProvider.
 @b Derived from
 None.
 @b Include files
 wx/tipdlg.h
 @b Library
 #wxAdv
 @b See also
 @ref tips_overview, ::wxShowTip
 @b Members
 wxTipProvider::wxTipProvider
 wxTipProvider::GetTip
 wxTipProvider::PreprocessTip
 wxCurrentTipProvider::GetCurrentTip


 @section wxtipproviderctor wxTipProvider::wxTipProvider

 @b  @b wxTipProvider(@b size_t@e currentTip)
 Constructor.
 @e currentTip
 The starting tip index.


 @section wxtipprovidergettip wxTipProvider::GetTip

 @b #wxString  @b GetTip()
 Return the text of the current tip and pass to the next one. This function is
 pure virtual, it should be implemented in the derived classes.

 @section wxtipproviderpreprocesstip wxTipProvider::PreprocessTip

 @b virtual #wxString  @b PreProcessTip(@b const #wxString&@e tip)
 Returns a modified tip. This function will be called immediately after read,
 and before being check whether it is a comment, an empty string or a string
 to translate. You can optionally override this in your custom user-derived class
 to optionally to modify the tip as soon as it is read. You can return any
 modification to the string. If you return wxEmptyString, then this tip is
 skipped, and the next one is read.

 @section wxtipprovidergetcurrenttip wxCurrentTipProvider::GetCurrentTip

 @b size_t @b GetCurrentTip() @b const
 Returns the index of the current tip (i.e. the one which would be returned by
 GetTip).
 The program usually remembers the value returned by this function after calling
 #wxShowTip. Note that it is not the same as the value which
 was passed to wxShowTip + 1 because the user might have pressed the "Next"
 button in the tip dialog.

 */


