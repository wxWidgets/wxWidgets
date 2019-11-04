Adding wxWidgets class documentation
====================================

This note is aimed at people wishing to add documentation for a
class to either the main wxWidgets manual, or to their own
manual.

wxWidgets uses Doxygen to process header input files with embedded
documentation in the form of C++ comments and output in HTML, and XML
(Doxygen itself can also output Latex, manpages, RTF, PDF etc).
See http://www.doxygen.org for more info about Doxygen.

If you want to add documentation of a new class/function to the
existing manual in docs/doxygen, you need to create a new .h file,
e.g. myclass.h, under the interface folder, which contains the public
interface of the new class/function in C++ syntax.
The documentation can then be added in form of Doxygen comments to
the header file.

You may also want to write a separate topic file,
e.g. `docs/doxygen/overviews/myclass.h`, and add the entry to
`docs/doxygen/mainpages/topics.h`.

If applicable, also add an entry to one of the `docs/doxygen/mainpages/cat_*.h`
files.

You can generate a first raw version of myclass.h simply taking its
"real" header and removing all the private and protected sections and
in general removing everything the user "shouldn't know": i.e. all things
which are implementation details.


Running Doxygen
---------------

First, make sure you have a recent version of Doxygen (currently Doxygen 1.8.8
is used) and Graphviz installed in your system (under Windows Graphviz
location should be in %PATH%).

On Unix:

  1. run `wxWidgets/docs/doxygen/regen.sh [format-to-generate]`

On Windows:

  1. cd wxWidgets/docs/doxygen
  2. run `regen.bat [format-to-generate]`

If you don't specify which format to generate, all output formats will
be enabled. Possible values for `format-to-generate` are: `html`, `chm`, `latex`,
`xml` and `all`.

The output of Doxygen is all placed in the wxWidgets/docs/doxygen/out folder.


Important Dos and Don'ts
------------------------

DO:

- use present tense verbs in 3rd person singular form to describe functions,
  i.e. write "Returns the answer to life, universe and everything", rather than
  the imperative mood used in Git commit message ("Return the answer ...").

- Doxygen supports both commands in the form \command and @command;
  all wxWidgets documentation uses the @command form.
  Follow strictly this rule.

- strive to use dedicated Doxygen commands for e.g. notes, lists,
  sections, etc. The "Special commands" page:
    http://www.doxygen.org/manual/commands.html
  is your friend!
  It's also very important to make a consistent use of the ALIASES
  defined by wxWidgets' Doxyfile. Open that file for more info.

- when you write true, false and NULL with their C++ semantic meaning,
  then use the @true, @false and @NULL commands.

- separate different paragraphs with an empty comment line.
  This is important otherwise Doxygen puts everything in the same
  paragraph making the result less readable.

- leave a blank comment line between a @section, @subsection, @page
  and the next paragraph.

- test your changes, both reading the generated HTML docs and by looking
  at the "doxygen.log" file produced (which will warn you about any
  eventual mistake found in the comments).

- quote all the following characters prefixing them with a "@" char:

         @  $  \  &  <  >  #  %

  unless they appear inside a @code or @verbatim section
  (you can also use HTML-style escaping, e.g. &amp; rather than @ escaping)

- when using a Doxygen alias like @itemdef{}, you need to escape the
  comma characters which appear on the first argument, otherwise Doxygen
  will interpret them as the marker of the end of the first argument and
  the beginning of the second argument's text.

  E.g. if you want to define the item "wxEVT_MACRO(id, func)" you need to write:

            @itemdef{wxEVT_MACRO(id\, func), This is the description of the macro}

  Also note that you need to escape only the commas of the first argument's
  text; second argument can have up to 10 commas unescaped (see the Doxyfile
  for the trick used to implement this).

- for linking use one of:
     - the @ref command to refer to topic overviews;
     - the () suffix to refer to function members of the same class you're
     - documenting or to refer to global functions or macros;
     - the classname:: operator to refer to functions of classes different
     - from the one you're documenting;
     - the `::` prefix to refer to global variables (e.g. ::wxEmptyString).
  Class names are auto-linked by Doxygen without the need of any explicit
  command.

DON'T:

- use jargon, such as 'gonna', or omit the definite article.
  The manual is intended to be a fluent, English document and
  not a collection of rough notes.

- use non-alphanumeric characters in link anchors.

- use Doxygen @b @c @e commands when referring to more than a single word;
  in that case you need to use the `<b>...</b>`, `<tt>...</tt>`, `<em>...</em>`
  HTML-style tags instead

- use HTML style tags for creation of tables or lists.
  Use wx aliases instead like @beginTable, @row2col, @row3col, @endTable and
  @beginDefList, @itemdef, @endDefList, etc.
  See the Doxyfile.inc for more info.


Documentation comment for a class
---------------------------------

Start off with:

	/**
	    @class wxMyClass

	    ...here goes the description...

	    @beginEventTable
	    @event{EVT_SOME_EVENT(id, func)}:
		    Description for EVT_SOME_EVENT.
	    @endEventTable

	    @beginStyleTable
	    @style{wxSOME_STYLE}:
		    Description for wxSOME_STYLE.
	    ...
	    @endStyleTable

	    @beginExtraStyleTable
	    @style{wxSOME_EXTRA_STYLE}:
		    Description for wxSOME_EXTRA_STYLE.
	    ...
	    @endExtraStyleTable

	    @library{wxbase}
	    @category{cat_shortcut}

	    @nativeimpl{wxgtk, wxmsw, ...}
	    @onlyfor{wxgtk, wxmsw, ...}

	    @appearance{button.png}

	    @stdobjects
	    ...here goes the list of predefined instances...

	    @see ...here goes the see-also list...
		 you can make references to topic overviews or other
		 manual pages using the @ref command
	*/

Note that everything *except* the @class, @library and @category
commands are optionals.

Also note that if you use @section and @subsection in the class description
(at the beginning), you should use as the section's anchor name "xxxx_yyyy"
where "xxxx" is the class name without the initial "wx" in lowercase
and "yyyy" is a lowercase word which uniquely identifies that section.
E.g.:

	/**
	    @class wxMyClass

	    This class does not exist really and is only used as an example
	    of best documentation practices.

	    @section myclass_special Special functions of this class

	    This section describes the functions whose usage is reserved for
	    wxWidgets internal mechanisms... etc etc...


	    @section myclass_custom Customizing wxMyClass

	    What if you want to customize this powerful class?
	    First you should do this and that, etc etc...


	    @library{wxbase}
	    @category{misc}

	    @see wxMyOtherClass
	*/



Documentation comment for a function
------------------------------------

Start off with:

	/**
	    ...here goes the description of the function....

	    @param param1
		...here goes the description for the first parameter of this function
	    @param param2
		...here goes the description for the second parameter of this function
		...

	    @return
	    ...here goes the description of what the function returns...

	    @note ...here go any eventual notes about this function...

	    @remarks ...here go any eventual remarks about this function...

	    @see ...here goes the see-also list...
	 */

Note that the @return, @note, @remarks, @see commands are optional.

The @param command has an optional attribute specifying the direction of
the attribute. Possible values are "in" and "out". E.g.

	/**
	 * Copies bytes from a source memory area to a destination memory area,
	 * where both areas may not overlap.
	 * @param[out]     dest   The memory area to copy to.
	 * @param[in]      src    The memory area to copy from.
	 * @param[in]      n      The number of bytes to copy.
	 * @param[in,out]  pmisc  Used both as input and as output.
	 */
	void func(void *dest, const void *src, size_t n, void *pmisc);


Documentation comment for a topic overview
------------------------------------------

Topic overviews are stored inside the docs/doxygen/overviews folder
and are completely placed inside a single comment block in the form of:

	/*!

	 @page overview_tname wxSomeStuff overview

	 This page provides an overview of the wxSomeStuff and related classes.
	 ....

	 @li @ref overview_tname_intro
	 @li @ref overview_tname_details
	 ...

	 <hr>


	 @section overview_tname_intro Introduction

	 ...here goes the introduction to this topic...


	 @section overview_tname_details Details

	 ...here go the details to this topic...

	*/

Note that there is a convention in the anchor link names.
Doxygen in fact requires that for each @page, @section, @subsection, etc tag,
there is a corresponding link anchor.

The following conventions are used in wxWidgets doxygen comments:

 1. all "main" pages of the manual (those which are placed in
   docs/doxygen/mainpages) have link anchors which begin with `page_`
 2. all topic overviews (those which are placed in docs/doxygen/overviews) have
   link anchors which begin with `overview_`
 3. all @section, @subsection, @subsubsection tags should have as link anchor
   name the name of the parent section plus a specific word separated with an
   underscore; e.g.:

	/*!

	 @page overview_tname wxSomeStuff overview

	 @section overview_tname_intro Introduction
	  @subsection overview_tname_intro_firstpart First part
	  @subsection overview_tname_intro_secondpart Second part
	   @subsubsection overview_tname_intro_secondpart_sub Second part subsection
	  @subsection overview_tname_intro_thirdpart Third part

	 @section overview_tname_details Details
	 ...

	*/
