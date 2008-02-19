/////////////////////////////////////////////////////////////////////////////
// Name:        bookctrl
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page bookctrl_overview wxBookCtrl overview

 Classes: #wxNotebook, #wxListbook, #wxChoicebook,
 #wxTreebook, #wxToolbook
 #Introduction
 @ref bestbookctrl_overview


 @section wxbookctrlintro Introduction

 A book control is a convenient way of displaying multiple pages of information,
 displayed one page at a time. wxWidgets has five variants of this control:


  #wxNotebook: uses a row of tabs
  #wxListbook: controlled by a #wxListCtrl
  #wxChoicebook: controlled by a #wxChoice
  #wxTreebook: controlled by a #wxTreeCtrl
  #wxToolbook: controlled by a #wxToolBar


 See @ref samplenotebook_overview for an example of wxBookCtrl usage.

 @section wxbestbookctrl Best book

 wxBookCtrl is mapped to the class best suited for a given platform.
 Currently it provides #wxChoicebook for smartphones equipped with
 WinCE, and #wxNotebook for all other platforms. The mapping consists of:







 wxBookCtrl




 wxChoicebook or wxNotebook





 wxBookCtrlEvent




 wxChoicebookEvent or wxNotebookEvent





 wxEVT_COMMAND_BOOKCTRL_PAGE_CHANGED




 wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED or wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED





 wxEVT_COMMAND_BOOKCTRL_PAGE_CHANGING




 wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING or wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING





 EVT_BOOKCTRL_PAGE_CHANGED(id, fn)




 EVT_CHOICEBOOK_PAGE_CHANGED(id, fn) or EVT_NOTEBOOK_PAGE_CHANGED(id, fn)





 EVT_BOOKCTRL_PAGE_CHANGING(id, fn)




 EVT_CHOICEBOOK_PAGE_CHANGING(id, fn) or EVT_NOTEBOOK_PAGE_CHANGING(id, fn)





 For orientation of the book controller, use following flags in style:







 @b wxBK_TOP




 controller above pages





 @b wxBK_BOTTOM




 controller below pages





 @b wxBK_LEFT




 controller on the left





 @b wxBK_RIGHT




 controller on the right





 @b wxBK_DEFAULT




 native controller placement

 */


