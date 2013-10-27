
wxRichTextCtrl README
=====================

Welcome to wxRichTextCtrl. It includes the following functionality:

* Text entry, paragraph wrapping

* Scrolling, keyboard navigation

* Application of character styles:

  bold, italic, underlined, font face, text colour

* Application of paragraph styles:

  left/right indentation, sub-indentation (first-line indent),
  paragraph spacing (before and after), line spacing,
  left/centre/right alignment, numbered bullets

* Insertion of images

* Copy/paste

* Undo/Redo with optional batching and undo history suppression

* Named paragraph and character styles management and application

* File handlers allow addition of file formats

* Text saving and loading, XML saving and loading, HTML saving (unfinished)

Sorry, this is a Windows-only demo for now but the code should
compile on other platforms.

Design
======

Data is represented by a hierarchy of objects, all derived from
wxRichTextObject.

The top of the hierarchy is the buffer, a kind of wxRichTextParagraphLayoutBox.
These boxes will allow flexible placement of text boxes on a page, but
for now there will be a single box representing the document,
and this box will a wxRichTextParagraphLayoutBox which contains further
wxRichTextParagraph objects, each of which can include text and images.

Each object maintains a range (start and end position) measured
from the start of the main parent box.
A paragraph object knows its range, and a text fragment knows its range
too. So, a character or image in a page has a position relative to the
start of the document, and a character in an embedded text box has
a position relative to that text box. For now, we will not be dealing with
embedded objects but it's something to bear in mind for later.

Before display, a changed buffer must have Layout() called on it,
to do wrapping, alignment etc. Ranges representing wrapped lines are stored
with each paragraph.

Since wxRichTextBuffer is separate from wxRichTextCtrl, the storage
and rendering facilities can be used by other controls.

API
===

It's basically the wxTextCtrl with some additions. There is a new
wxTextAttrEx class deriving from wxTextAttr, to accomodate new
style attributes. This could be merged with wxTextAttr. There
is also a wxRichTextAttr which is similar to wxTextAttrEx but
doesn't store the font as a wxFont: this allows much more
efficient operations, especially when querying styles in a
UI update handler. We would not want to create several new wxFonts
when querying for italics, bold, etc. every few milliseconds.

See "Functionality specific to wxRichTextCtrl" section in richtextctrl.h.

One addition is Set/GetBasicStyle, which is needed in addition to
Set/GetDefaultStyle to get the overall style for the buffer
from which content will inherit (after apply the default style).

wxRichTextRange is a new class representing start and end positions.
It's used in the implementation so that pieces of content
know their range, and also in the API in preference to using
two positions.

What next?
==========

- Decision about where to put it: wxCode, wxWidgets
- Makefiles/bakefiles
- Refining the API
- Documentation
- Bug fixing/improvements


See todo.txt for a list of bugs, improvements and features,
and also TODO throughout the source.

==
Julian Smart, October 18th 2005
