/////////////////////////////////////////////////////////////////////////////
// Name:        changes_since28.h
// Purpose:     topic overview
// Author:      Vadim Zeitlin
// Created:     2008-05-08
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_changes_since28 Changes Since wxWidgets 2.8

This topic describes backwards-incompatible changes in wxWidgets 3.0 compared
to the last stable release.

The incompatible changes can be grouped into the following categories:

@li @ref overview_changes_unicode
@li @ref overview_changes_other


<hr>

@section overview_changes_unicode Unicode-related Changes

If you used Unicode build of wxWidgets 2.8 or previous version, please read
@ref overview_unicode for the details about how the API changed in 3.0 as a lot
of the information which was correct before doesn't apply any longer.

For example, the notorious (due to the confusion they created) macros @c wxT()
and @c _T() are not needed at all any longer. Basically, you can remove them
from any code which used them. On the other hand, there is no particular harm
in leaving them neither as the code will still compile and work correctly --
you only need to remove them if you think that your code looks tidier without
them. You also don't need to use @c wxChar any longer but can directly use the
standard @c wchar_t type even if, again, @c wxChar continues to work.

The most serious backwards-incompatible change is related to the change of
return type of wxString::c_str() method: it returns a special proxy object
instead of a simple @c char* or @c wchar_t* now. Because of this, you cannot
pass its result to any standard vararg functions such as @c printf() any more
as described in @ref overview_unicode_compilation_errors. All wxWidgets
functions, such as wxPrintf(), wxLogMessage() &c still work with it, but
passing it to @c printf() will now result in a crash. It is strongly advised to
recompile your code with a compiler warning about passing non-POD objects to
vararg functions, such as g++.

The other class of incompatible changes is due to modifying some virtual
methods to use @c wxString parameters instead of @c const @c wxChar* ones to
make them accept both narrow and wide strings. This is not a problem if you
simply call these functions but you need to change the signature of the derived
class versions if you override them as otherwise they wouldn't be called any
more. Again, the best way to ensure that this problem doesn't arise is to
rebuild your code using a compiler which warns about function signature
mismatch (you can use @c -Woverloaded-virtual g++ option).

Finally, a few structure fields, notable @c wxCmdLineEntryDesc::shortName,
@c longName and @c description fields have been changed to be of type @c const
@c char* instead of @c const @c wxChar* so you will need to remove @c wxT() or
@c _T() if you used it with their initializers.

@section overview_changes_other Miscellaneous Other Changes

Please see @c docs/changes.txt for details of the other changes until they are
described in more details here.

*/

