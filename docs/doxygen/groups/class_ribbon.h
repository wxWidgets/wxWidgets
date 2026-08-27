/////////////////////////////////////////////////////////////////////////////
// Name:        class_ribbon.h
// Purpose:     Ribbon classes group docs
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@defgroup group_class_ribbon Ribbon User Interface
@ingroup group_class

The wxRibbon library is a set of classes for writing a ribbon user interface.
At the most generic level, this is a combination of a tab control with a
toolbar. At a more functional level, it is similar to the user interface
present in recent versions of Microsoft Office.

Since wxWidgets 3.3.4, the ribbon can also be driven from the keyboard using
Office-style KeyTips, improving accessibility for users who cannot rely on a
mouse. Pressing @c WXK_F10 (customizable, see
wxRibbonBar::SetKeyTipsTriggerKey()) badges every element which was assigned
one, and typing its letters activates that element. See
wxRibbonBar::ShowKeyTips() and wxRibbonBar::SetPageKeyTip().

*/
