/////////////////////////////////////////////////////////////////////////////
// Name:        bookctrl.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_bookctrl wxBookCtrl Overview

@tableofcontents

A book control is a convenient way of displaying multiple pages of information,
displayed one page at a time. wxWidgets has five variants of this control:

@li wxChoicebook: controlled by a wxChoice
@li wxListbook: controlled by a wxListCtrl
@li wxNotebook: uses a row of tabs
@li wxSimplebook: doesn't allow the user to change the page at all.
@li wxTreebook: controlled by a wxTreeCtrl
@li wxToolbook: controlled by a wxToolBar

See the @ref page_samples_notebook for an example of wxBookCtrl usage.

Notice that wxSimplebook is special in that it only allows the program to
change the selection, thus it's usually used in slightly different
circumstances than the other variants.

@see @ref group_class_bookctrl



@section overview_bookctrl_bestbookctrl Best Book

::wxBookCtrl is mapped to the class best suited for a given platform. Currently
it is the same as wxNotebook if it is compiled in (i.e. @c wxUSE_NOTEBOOK was
not explicitly set to 0), otherwise it is the same as wxChoicebook.

The mapping consists of:

@beginTable
@row2col{ ::wxBookCtrl, wxChoicebook or wxNotebook }
@row2col{ @c wxEVT_BOOKCTRL_PAGE_CHANGED,
          @c wxEVT_CHOICEBOOK_PAGE_CHANGED or
          @c wxEVT_NOTEBOOK_PAGE_CHANGED }
@row2col{ @c wxEVT_BOOKCTRL_PAGE_CHANGING,
          @c wxEVT_CHOICEBOOK_PAGE_CHANGING or
          @c wxEVT_NOTEBOOK_PAGE_CHANGING }
@row2col{ @c EVT_BOOKCTRL_PAGE_CHANGED(id\, fn),
          @c EVT_CHOICEBOOK_PAGE_CHANGED(id, fn) or
          @c EVT_NOTEBOOK_PAGE_CHANGED(id, fn) }
@row2col{ @c EVT_BOOKCTRL_PAGE_CHANGING(id\, fn),
          @c EVT_CHOICEBOOK_PAGE_CHANGING(id, fn) or
          @c EVT_NOTEBOOK_PAGE_CHANGING(id, fn) }
@endTable

For orientation of the book controller, use following flags in style:

@li @b wxBK_TOP: controller above pages
@li @b wxBK_BOTTOM: controller below pages
@li @b wxBK_LEFT: controller on the left
@li @b wxBK_RIGHT: controller on the right
@li @b wxBK_DEFAULT: native controller placement

*/

