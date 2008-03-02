/////////////////////////////////////////////////////////////////////////////
// Name:        bookctrl.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page overview_bookctrl wxBookCtrl overview

 Classes: wxNotebook, wxListbook, wxChoicebook, wxTreebook, wxToolbook

 @li @ref overview_bookctrl_intro
 @li @ref overview_bookctrl_bestbookctrl


 <hr>


 @section overview_bookctrl_intro Introduction

 A book control is a convenient way of displaying multiple pages of information,
 displayed one page at a time. wxWidgets has five variants of this control:

  @li wxNotebook: uses a row of tabs
  @li wxListbook: controlled by a wxListCtrl
  @li wxChoicebook: controlled by a wxChoice
  @li wxTreebook: controlled by a wxTreeCtrl
  @li wxToolbook: controlled by a wxToolBar

 See @ref page_utils_samples_notebook for an example of wxBookCtrl usage.


 @section overview_bookctrl_bestbookctrl Best book

 wxBookCtrl is mapped to the class best suited for a given platform.
 Currently it provides wxChoicebook for smartphones equipped with
 WinCE, and wxNotebook for all other platforms. The mapping consists of:

 @beginTable
 @row2col{wxBookCtrl, wxChoicebook or wxNotebook}
 @row2col{wxBookCtrlEvent, wxChoicebookEvent or wxNotebookEvent}
 @row2col{wxEVT_COMMAND_BOOKCTRL_PAGE_CHANGED,
          wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED or wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED}
 @row2col{wxEVT_COMMAND_BOOKCTRL_PAGE_CHANGING,
          wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING or wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING}
 @row2col{EVT_BOOKCTRL_PAGE_CHANGED(id\, fn),
          EVT_CHOICEBOOK_PAGE_CHANGED(id, fn) or EVT_NOTEBOOK_PAGE_CHANGED(id, fn)}
 @row2col{EVT_BOOKCTRL_PAGE_CHANGING(id\, fn),
          EVT_CHOICEBOOK_PAGE_CHANGING(id, fn) or EVT_NOTEBOOK_PAGE_CHANGING(id, fn)}
 @endTable

 For orientation of the book controller, use following flags in style:

 @li @b wxBK_TOP: controller above pages
 @li @b wxBK_BOTTOM: controller below pages
 @li @b wxBK_LEFT: controller on the left
 @li @b wxBK_RIGHT: controller on the right
 @li @b wxBK_DEFAULT: native controller placement

*/

