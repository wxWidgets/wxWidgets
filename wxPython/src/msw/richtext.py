# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

import _richtext
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


USE_TEXTATTREX = _richtext.USE_TEXTATTREX
import _windows
import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
RE_READONLY = _richtext.RE_READONLY
RE_MULTILINE = _richtext.RE_MULTILINE
RICHTEXT_SHIFT_DOWN = _richtext.RICHTEXT_SHIFT_DOWN
RICHTEXT_CTRL_DOWN = _richtext.RICHTEXT_CTRL_DOWN
RICHTEXT_ALT_DOWN = _richtext.RICHTEXT_ALT_DOWN
RICHTEXT_SELECTED = _richtext.RICHTEXT_SELECTED
RICHTEXT_TAGGED = _richtext.RICHTEXT_TAGGED
RICHTEXT_FOCUSSED = _richtext.RICHTEXT_FOCUSSED
RICHTEXT_IS_FOCUS = _richtext.RICHTEXT_IS_FOCUS
RICHTEXT_TYPE_ANY = _richtext.RICHTEXT_TYPE_ANY
RICHTEXT_TYPE_TEXT = _richtext.RICHTEXT_TYPE_TEXT
RICHTEXT_TYPE_XML = _richtext.RICHTEXT_TYPE_XML
RICHTEXT_TYPE_HTML = _richtext.RICHTEXT_TYPE_HTML
RICHTEXT_TYPE_RTF = _richtext.RICHTEXT_TYPE_RTF
RICHTEXT_TYPE_PDF = _richtext.RICHTEXT_TYPE_PDF
RICHTEXT_FIXED_WIDTH = _richtext.RICHTEXT_FIXED_WIDTH
RICHTEXT_FIXED_HEIGHT = _richtext.RICHTEXT_FIXED_HEIGHT
RICHTEXT_VARIABLE_WIDTH = _richtext.RICHTEXT_VARIABLE_WIDTH
RICHTEXT_VARIABLE_HEIGHT = _richtext.RICHTEXT_VARIABLE_HEIGHT
RICHTEXT_HITTEST_NONE = _richtext.RICHTEXT_HITTEST_NONE
RICHTEXT_HITTEST_BEFORE = _richtext.RICHTEXT_HITTEST_BEFORE
RICHTEXT_HITTEST_AFTER = _richtext.RICHTEXT_HITTEST_AFTER
RICHTEXT_HITTEST_ON = _richtext.RICHTEXT_HITTEST_ON
RICHTEXT_FORMATTED = _richtext.RICHTEXT_FORMATTED
RICHTEXT_UNFORMATTED = _richtext.RICHTEXT_UNFORMATTED
RICHTEXT_SETSTYLE_NONE = _richtext.RICHTEXT_SETSTYLE_NONE
RICHTEXT_SETSTYLE_WITH_UNDO = _richtext.RICHTEXT_SETSTYLE_WITH_UNDO
RICHTEXT_SETSTYLE_OPTIMIZE = _richtext.RICHTEXT_SETSTYLE_OPTIMIZE
RICHTEXT_SETSTYLE_PARAGRAPHS_ONLY = _richtext.RICHTEXT_SETSTYLE_PARAGRAPHS_ONLY
RICHTEXT_SETSTYLE_CHARACTERS_ONLY = _richtext.RICHTEXT_SETSTYLE_CHARACTERS_ONLY
RICHTEXT_INSERT_NONE = _richtext.RICHTEXT_INSERT_NONE
RICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE = _richtext.RICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE
TEXT_ATTR_TEXT_COLOUR = _richtext.TEXT_ATTR_TEXT_COLOUR
TEXT_ATTR_BACKGROUND_COLOUR = _richtext.TEXT_ATTR_BACKGROUND_COLOUR
TEXT_ATTR_FONT_FACE = _richtext.TEXT_ATTR_FONT_FACE
TEXT_ATTR_FONT_SIZE = _richtext.TEXT_ATTR_FONT_SIZE
TEXT_ATTR_FONT_WEIGHT = _richtext.TEXT_ATTR_FONT_WEIGHT
TEXT_ATTR_FONT_ITALIC = _richtext.TEXT_ATTR_FONT_ITALIC
TEXT_ATTR_FONT_UNDERLINE = _richtext.TEXT_ATTR_FONT_UNDERLINE
TEXT_ATTR_FONT = _richtext.TEXT_ATTR_FONT
TEXT_ATTR_ALIGNMENT = _richtext.TEXT_ATTR_ALIGNMENT
TEXT_ATTR_LEFT_INDENT = _richtext.TEXT_ATTR_LEFT_INDENT
TEXT_ATTR_RIGHT_INDENT = _richtext.TEXT_ATTR_RIGHT_INDENT
TEXT_ATTR_TABS = _richtext.TEXT_ATTR_TABS
TEXT_ATTR_PARA_SPACING_AFTER = _richtext.TEXT_ATTR_PARA_SPACING_AFTER
TEXT_ATTR_PARA_SPACING_BEFORE = _richtext.TEXT_ATTR_PARA_SPACING_BEFORE
TEXT_ATTR_LINE_SPACING = _richtext.TEXT_ATTR_LINE_SPACING
TEXT_ATTR_CHARACTER_STYLE_NAME = _richtext.TEXT_ATTR_CHARACTER_STYLE_NAME
TEXT_ATTR_PARAGRAPH_STYLE_NAME = _richtext.TEXT_ATTR_PARAGRAPH_STYLE_NAME
TEXT_ATTR_BULLET_STYLE = _richtext.TEXT_ATTR_BULLET_STYLE
TEXT_ATTR_BULLET_NUMBER = _richtext.TEXT_ATTR_BULLET_NUMBER
TEXT_ATTR_BULLET_STYLE_NONE = _richtext.TEXT_ATTR_BULLET_STYLE_NONE
TEXT_ATTR_BULLET_STYLE_ARABIC = _richtext.TEXT_ATTR_BULLET_STYLE_ARABIC
TEXT_ATTR_BULLET_STYLE_LETTERS_UPPER = _richtext.TEXT_ATTR_BULLET_STYLE_LETTERS_UPPER
TEXT_ATTR_BULLET_STYLE_LETTERS_LOWER = _richtext.TEXT_ATTR_BULLET_STYLE_LETTERS_LOWER
TEXT_ATTR_BULLET_STYLE_ROMAN_UPPER = _richtext.TEXT_ATTR_BULLET_STYLE_ROMAN_UPPER
TEXT_ATTR_BULLET_STYLE_ROMAN_LOWER = _richtext.TEXT_ATTR_BULLET_STYLE_ROMAN_LOWER
TEXT_ATTR_BULLET_STYLE_SYMBOL = _richtext.TEXT_ATTR_BULLET_STYLE_SYMBOL
TEXT_ATTR_BULLET_STYLE_BITMAP = _richtext.TEXT_ATTR_BULLET_STYLE_BITMAP
TEXT_ATTR_BULLET_STYLE_PARENTHESES = _richtext.TEXT_ATTR_BULLET_STYLE_PARENTHESES
TEXT_ATTR_BULLET_STYLE_PERIOD = _richtext.TEXT_ATTR_BULLET_STYLE_PERIOD
TEXT_ATTR_LINE_SPACING_NORMAL = _richtext.TEXT_ATTR_LINE_SPACING_NORMAL
TEXT_ATTR_LINE_SPACING_HALF = _richtext.TEXT_ATTR_LINE_SPACING_HALF
TEXT_ATTR_LINE_SPACING_TWICE = _richtext.TEXT_ATTR_LINE_SPACING_TWICE
TEXT_ALIGNMENT_DEFAULT = _richtext.TEXT_ALIGNMENT_DEFAULT
TEXT_ALIGNMENT_LEFT = _richtext.TEXT_ALIGNMENT_LEFT
TEXT_ALIGNMENT_CENTRE = _richtext.TEXT_ALIGNMENT_CENTRE
TEXT_ALIGNMENT_CENTER = _richtext.TEXT_ALIGNMENT_CENTER
TEXT_ALIGNMENT_RIGHT = _richtext.TEXT_ALIGNMENT_RIGHT
TEXT_ALIGNMENT_JUSTIFIED = _richtext.TEXT_ALIGNMENT_JUSTIFIED
class RichTextRange(object):
    """
    RichTextRange is a data structure that represents a range of text
    within a `RichTextCtrl`.  It simply contains integer ``start`` and
    ``end`` properties and a few operations useful for dealing with
    ranges.  In most places in wxPython where a RichTextRange is expected a
    2-tuple containing (start, end) can be used instead.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, long start=0, long end=0) -> RichTextRange

        Creates a new range object.
        """
        _richtext.RichTextRange_swiginit(self,_richtext.new_RichTextRange(*args, **kwargs))
    __swig_destroy__ = _richtext.delete_RichTextRange
    __del__ = lambda self : None;
    def __eq__(*args, **kwargs):
        """
        __eq__(self, PyObject other) -> bool

        Test for equality of RichTextRange objects.
        """
        return _richtext.RichTextRange___eq__(*args, **kwargs)

    def __sub__(*args, **kwargs):
        """__sub__(self, RichTextRange range) -> RichTextRange"""
        return _richtext.RichTextRange___sub__(*args, **kwargs)

    def __add__(*args, **kwargs):
        """__add__(self, RichTextRange range) -> RichTextRange"""
        return _richtext.RichTextRange___add__(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(self, long start, long end)"""
        return _richtext.RichTextRange_SetRange(*args, **kwargs)

    def SetStart(*args, **kwargs):
        """SetStart(self, long start)"""
        return _richtext.RichTextRange_SetStart(*args, **kwargs)

    def GetStart(*args, **kwargs):
        """GetStart(self) -> long"""
        return _richtext.RichTextRange_GetStart(*args, **kwargs)

    start = property(GetStart, SetStart) 
    def SetEnd(*args, **kwargs):
        """SetEnd(self, long end)"""
        return _richtext.RichTextRange_SetEnd(*args, **kwargs)

    def GetEnd(*args, **kwargs):
        """GetEnd(self) -> long"""
        return _richtext.RichTextRange_GetEnd(*args, **kwargs)

    end = property(GetEnd, SetEnd) 
    def IsOutside(*args, **kwargs):
        """
        IsOutside(self, RichTextRange range) -> bool

        Returns true if this range is completely outside 'range'
        """
        return _richtext.RichTextRange_IsOutside(*args, **kwargs)

    def IsWithin(*args, **kwargs):
        """
        IsWithin(self, RichTextRange range) -> bool

        Returns true if this range is completely within 'range'
        """
        return _richtext.RichTextRange_IsWithin(*args, **kwargs)

    def Contains(*args, **kwargs):
        """
        Contains(self, long pos) -> bool

        Returns true if the given position is within this range. Allow for the
        possibility of an empty range - assume the position is within this
        empty range.
        """
        return _richtext.RichTextRange_Contains(*args, **kwargs)

    def LimitTo(*args, **kwargs):
        """
        LimitTo(self, RichTextRange range) -> bool

        Limit this range to be within 'range'
        """
        return _richtext.RichTextRange_LimitTo(*args, **kwargs)

    def GetLength(*args, **kwargs):
        """
        GetLength(self) -> long

        Gets the length of the range
        """
        return _richtext.RichTextRange_GetLength(*args, **kwargs)

    def Swap(*args, **kwargs):
        """
        Swap(self)

        Swaps the start and end
        """
        return _richtext.RichTextRange_Swap(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (start,end)

        Returns the start and end properties as a tuple.
        """
        return _richtext.RichTextRange_Get(*args, **kwargs)

    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'RichTextRange'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.start = val
        elif index == 1: self.end = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (RichTextRange, self.Get())

    End = property(GetEnd,SetEnd,doc="See `GetEnd` and `SetEnd`") 
    Length = property(GetLength,doc="See `GetLength`") 
    Start = property(GetStart,SetStart,doc="See `GetStart` and `SetStart`") 
_richtext.RichTextRange_swigregister(RichTextRange)

class RichTextAttr(object):
    """
    The RichTextAttr class stored information about the various attributes
    for a block of text, including font, colour, indents, alignments, and
    etc.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Colour colText=wxNullColour, Colour colBack=wxNullColour, 
            int alignment=TEXT_ALIGNMENT_DEFAULT) -> RichTextAttr

        The RichTextAttr class stored information about the various attributes
        for a block of text, including font, colour, indents, alignments, and
        etc.
        """
        _richtext.RichTextAttr_swiginit(self,_richtext.new_RichTextAttr(*args, **kwargs))
    __swig_destroy__ = _richtext.delete_RichTextAttr
    __del__ = lambda self : None;
    def CreateFont(*args, **kwargs):
        """CreateFont(self) -> Font"""
        return _richtext.RichTextAttr_CreateFont(*args, **kwargs)

    def GetFontAttributes(*args, **kwargs):
        """GetFontAttributes(self, Font font) -> bool"""
        return _richtext.RichTextAttr_GetFontAttributes(*args, **kwargs)

    def GetFont(self):
        return self.CreateFont()
    def SetFont(self, font):
        return self.GetFontAttributes(font)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(self, Colour colText)"""
        return _richtext.RichTextAttr_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, Colour colBack)"""
        return _richtext.RichTextAttr_SetBackgroundColour(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(self, int alignment)"""
        return _richtext.RichTextAttr_SetAlignment(*args, **kwargs)

    def SetTabs(*args, **kwargs):
        """SetTabs(self, wxArrayInt tabs)"""
        return _richtext.RichTextAttr_SetTabs(*args, **kwargs)

    def SetLeftIndent(*args, **kwargs):
        """SetLeftIndent(self, int indent, int subIndent=0)"""
        return _richtext.RichTextAttr_SetLeftIndent(*args, **kwargs)

    def SetRightIndent(*args, **kwargs):
        """SetRightIndent(self, int indent)"""
        return _richtext.RichTextAttr_SetRightIndent(*args, **kwargs)

    def SetFontSize(*args, **kwargs):
        """SetFontSize(self, int pointSize)"""
        return _richtext.RichTextAttr_SetFontSize(*args, **kwargs)

    def SetFontStyle(*args, **kwargs):
        """SetFontStyle(self, int fontStyle)"""
        return _richtext.RichTextAttr_SetFontStyle(*args, **kwargs)

    def SetFontWeight(*args, **kwargs):
        """SetFontWeight(self, int fontWeight)"""
        return _richtext.RichTextAttr_SetFontWeight(*args, **kwargs)

    def SetFontFaceName(*args, **kwargs):
        """SetFontFaceName(self, String faceName)"""
        return _richtext.RichTextAttr_SetFontFaceName(*args, **kwargs)

    def SetFontUnderlined(*args, **kwargs):
        """SetFontUnderlined(self, bool underlined)"""
        return _richtext.RichTextAttr_SetFontUnderlined(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(self, long flags)"""
        return _richtext.RichTextAttr_SetFlags(*args, **kwargs)

    def SetCharacterStyleName(*args, **kwargs):
        """SetCharacterStyleName(self, String name)"""
        return _richtext.RichTextAttr_SetCharacterStyleName(*args, **kwargs)

    def SetParagraphStyleName(*args, **kwargs):
        """SetParagraphStyleName(self, String name)"""
        return _richtext.RichTextAttr_SetParagraphStyleName(*args, **kwargs)

    def SetParagraphSpacingAfter(*args, **kwargs):
        """SetParagraphSpacingAfter(self, int spacing)"""
        return _richtext.RichTextAttr_SetParagraphSpacingAfter(*args, **kwargs)

    def SetParagraphSpacingBefore(*args, **kwargs):
        """SetParagraphSpacingBefore(self, int spacing)"""
        return _richtext.RichTextAttr_SetParagraphSpacingBefore(*args, **kwargs)

    def SetLineSpacing(*args, **kwargs):
        """SetLineSpacing(self, int spacing)"""
        return _richtext.RichTextAttr_SetLineSpacing(*args, **kwargs)

    def SetBulletStyle(*args, **kwargs):
        """SetBulletStyle(self, int style)"""
        return _richtext.RichTextAttr_SetBulletStyle(*args, **kwargs)

    def SetBulletNumber(*args, **kwargs):
        """SetBulletNumber(self, int n)"""
        return _richtext.RichTextAttr_SetBulletNumber(*args, **kwargs)

    def SetBulletText(*args, **kwargs):
        """SetBulletText(self, wxChar symbol)"""
        return _richtext.RichTextAttr_SetBulletText(*args, **kwargs)

    def SetBulletFont(*args, **kwargs):
        """SetBulletFont(self, String bulletFont)"""
        return _richtext.RichTextAttr_SetBulletFont(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(self) -> Colour"""
        return _richtext.RichTextAttr_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> Colour"""
        return _richtext.RichTextAttr_GetBackgroundColour(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """GetAlignment(self) -> int"""
        return _richtext.RichTextAttr_GetAlignment(*args, **kwargs)

    def GetTabs(*args, **kwargs):
        """GetTabs(self) -> wxArrayInt"""
        return _richtext.RichTextAttr_GetTabs(*args, **kwargs)

    def GetLeftIndent(*args, **kwargs):
        """GetLeftIndent(self) -> long"""
        return _richtext.RichTextAttr_GetLeftIndent(*args, **kwargs)

    def GetLeftSubIndent(*args, **kwargs):
        """GetLeftSubIndent(self) -> long"""
        return _richtext.RichTextAttr_GetLeftSubIndent(*args, **kwargs)

    def GetRightIndent(*args, **kwargs):
        """GetRightIndent(self) -> long"""
        return _richtext.RichTextAttr_GetRightIndent(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags(self) -> long"""
        return _richtext.RichTextAttr_GetFlags(*args, **kwargs)

    def GetFontSize(*args, **kwargs):
        """GetFontSize(self) -> int"""
        return _richtext.RichTextAttr_GetFontSize(*args, **kwargs)

    def GetFontStyle(*args, **kwargs):
        """GetFontStyle(self) -> int"""
        return _richtext.RichTextAttr_GetFontStyle(*args, **kwargs)

    def GetFontWeight(*args, **kwargs):
        """GetFontWeight(self) -> int"""
        return _richtext.RichTextAttr_GetFontWeight(*args, **kwargs)

    def GetFontUnderlined(*args, **kwargs):
        """GetFontUnderlined(self) -> bool"""
        return _richtext.RichTextAttr_GetFontUnderlined(*args, **kwargs)

    def GetFontFaceName(*args, **kwargs):
        """GetFontFaceName(self) -> String"""
        return _richtext.RichTextAttr_GetFontFaceName(*args, **kwargs)

    def GetCharacterStyleName(*args, **kwargs):
        """GetCharacterStyleName(self) -> String"""
        return _richtext.RichTextAttr_GetCharacterStyleName(*args, **kwargs)

    def GetParagraphStyleName(*args, **kwargs):
        """GetParagraphStyleName(self) -> String"""
        return _richtext.RichTextAttr_GetParagraphStyleName(*args, **kwargs)

    def GetParagraphSpacingAfter(*args, **kwargs):
        """GetParagraphSpacingAfter(self) -> int"""
        return _richtext.RichTextAttr_GetParagraphSpacingAfter(*args, **kwargs)

    def GetParagraphSpacingBefore(*args, **kwargs):
        """GetParagraphSpacingBefore(self) -> int"""
        return _richtext.RichTextAttr_GetParagraphSpacingBefore(*args, **kwargs)

    def GetLineSpacing(*args, **kwargs):
        """GetLineSpacing(self) -> int"""
        return _richtext.RichTextAttr_GetLineSpacing(*args, **kwargs)

    def GetBulletStyle(*args, **kwargs):
        """GetBulletStyle(self) -> int"""
        return _richtext.RichTextAttr_GetBulletStyle(*args, **kwargs)

    def GetBulletNumber(*args, **kwargs):
        """GetBulletNumber(self) -> int"""
        return _richtext.RichTextAttr_GetBulletNumber(*args, **kwargs)

    def GetBulletText(*args, **kwargs):
        """GetBulletText(self) -> String"""
        return _richtext.RichTextAttr_GetBulletText(*args, **kwargs)

    def GetBulletFont(*args, **kwargs):
        """GetBulletFont(self) -> String"""
        return _richtext.RichTextAttr_GetBulletFont(*args, **kwargs)

    def HasTextColour(*args, **kwargs):
        """HasTextColour(self) -> bool"""
        return _richtext.RichTextAttr_HasTextColour(*args, **kwargs)

    def HasBackgroundColour(*args, **kwargs):
        """HasBackgroundColour(self) -> bool"""
        return _richtext.RichTextAttr_HasBackgroundColour(*args, **kwargs)

    def HasAlignment(*args, **kwargs):
        """HasAlignment(self) -> bool"""
        return _richtext.RichTextAttr_HasAlignment(*args, **kwargs)

    def HasTabs(*args, **kwargs):
        """HasTabs(self) -> bool"""
        return _richtext.RichTextAttr_HasTabs(*args, **kwargs)

    def HasLeftIndent(*args, **kwargs):
        """HasLeftIndent(self) -> bool"""
        return _richtext.RichTextAttr_HasLeftIndent(*args, **kwargs)

    def HasRightIndent(*args, **kwargs):
        """HasRightIndent(self) -> bool"""
        return _richtext.RichTextAttr_HasRightIndent(*args, **kwargs)

    def HasFont(*args, **kwargs):
        """HasFont(self) -> bool"""
        return _richtext.RichTextAttr_HasFont(*args, **kwargs)

    def HasParagraphSpacingAfter(*args, **kwargs):
        """HasParagraphSpacingAfter(self) -> bool"""
        return _richtext.RichTextAttr_HasParagraphSpacingAfter(*args, **kwargs)

    def HasParagraphSpacingBefore(*args, **kwargs):
        """HasParagraphSpacingBefore(self) -> bool"""
        return _richtext.RichTextAttr_HasParagraphSpacingBefore(*args, **kwargs)

    def HasLineSpacing(*args, **kwargs):
        """HasLineSpacing(self) -> bool"""
        return _richtext.RichTextAttr_HasLineSpacing(*args, **kwargs)

    def HasCharacterStyleName(*args, **kwargs):
        """HasCharacterStyleName(self) -> bool"""
        return _richtext.RichTextAttr_HasCharacterStyleName(*args, **kwargs)

    def HasParagraphStyleName(*args, **kwargs):
        """HasParagraphStyleName(self) -> bool"""
        return _richtext.RichTextAttr_HasParagraphStyleName(*args, **kwargs)

    def HasBulletStyle(*args, **kwargs):
        """HasBulletStyle(self) -> bool"""
        return _richtext.RichTextAttr_HasBulletStyle(*args, **kwargs)

    def HasBulletNumber(*args, **kwargs):
        """HasBulletNumber(self) -> bool"""
        return _richtext.RichTextAttr_HasBulletNumber(*args, **kwargs)

    def HasBulletText(*args, **kwargs):
        """HasBulletText(self) -> bool"""
        return _richtext.RichTextAttr_HasBulletText(*args, **kwargs)

    def HasFlag(*args, **kwargs):
        """HasFlag(self, long flag) -> bool"""
        return _richtext.RichTextAttr_HasFlag(*args, **kwargs)

    def IsCharacterStyle(*args, **kwargs):
        """IsCharacterStyle(self) -> bool"""
        return _richtext.RichTextAttr_IsCharacterStyle(*args, **kwargs)

    def IsParagraphStyle(*args, **kwargs):
        """IsParagraphStyle(self) -> bool"""
        return _richtext.RichTextAttr_IsParagraphStyle(*args, **kwargs)

    def IsDefault(*args, **kwargs):
        """IsDefault(self) -> bool"""
        return _richtext.RichTextAttr_IsDefault(*args, **kwargs)

    Alignment = property(GetAlignment,SetAlignment,doc="See `GetAlignment` and `SetAlignment`") 
    BackgroundColour = property(GetBackgroundColour,SetBackgroundColour,doc="See `GetBackgroundColour` and `SetBackgroundColour`") 
    BulletFont = property(GetBulletFont,SetBulletFont,doc="See `GetBulletFont` and `SetBulletFont`") 
    BulletNumber = property(GetBulletNumber,SetBulletNumber,doc="See `GetBulletNumber` and `SetBulletNumber`") 
    BulletStyle = property(GetBulletStyle,SetBulletStyle,doc="See `GetBulletStyle` and `SetBulletStyle`") 
    BulletText = property(GetBulletText,SetBulletText,doc="See `GetBulletText` and `SetBulletText`") 
    CharacterStyleName = property(GetCharacterStyleName,SetCharacterStyleName,doc="See `GetCharacterStyleName` and `SetCharacterStyleName`") 
    Flags = property(GetFlags,SetFlags,doc="See `GetFlags` and `SetFlags`") 
    Font = property(GetFont,SetFont,doc="See `GetFont` and `SetFont`") 
    FontAttributes = property(GetFontAttributes,doc="See `GetFontAttributes`") 
    FontFaceName = property(GetFontFaceName,SetFontFaceName,doc="See `GetFontFaceName` and `SetFontFaceName`") 
    FontSize = property(GetFontSize,SetFontSize,doc="See `GetFontSize` and `SetFontSize`") 
    FontStyle = property(GetFontStyle,SetFontStyle,doc="See `GetFontStyle` and `SetFontStyle`") 
    FontUnderlined = property(GetFontUnderlined,SetFontUnderlined,doc="See `GetFontUnderlined` and `SetFontUnderlined`") 
    FontWeight = property(GetFontWeight,SetFontWeight,doc="See `GetFontWeight` and `SetFontWeight`") 
    LeftIndent = property(GetLeftIndent,SetLeftIndent,doc="See `GetLeftIndent` and `SetLeftIndent`") 
    LeftSubIndent = property(GetLeftSubIndent,doc="See `GetLeftSubIndent`") 
    LineSpacing = property(GetLineSpacing,SetLineSpacing,doc="See `GetLineSpacing` and `SetLineSpacing`") 
    ParagraphSpacingAfter = property(GetParagraphSpacingAfter,SetParagraphSpacingAfter,doc="See `GetParagraphSpacingAfter` and `SetParagraphSpacingAfter`") 
    ParagraphSpacingBefore = property(GetParagraphSpacingBefore,SetParagraphSpacingBefore,doc="See `GetParagraphSpacingBefore` and `SetParagraphSpacingBefore`") 
    ParagraphStyleName = property(GetParagraphStyleName,SetParagraphStyleName,doc="See `GetParagraphStyleName` and `SetParagraphStyleName`") 
    RightIndent = property(GetRightIndent,SetRightIndent,doc="See `GetRightIndent` and `SetRightIndent`") 
    Tabs = property(GetTabs,SetTabs,doc="See `GetTabs` and `SetTabs`") 
    TextColour = property(GetTextColour,SetTextColour,doc="See `GetTextColour` and `SetTextColour`") 
_richtext.RichTextAttr_swigregister(RichTextAttr)
cvar = _richtext.cvar
RICHTEXT_ALL = cvar.RICHTEXT_ALL
RICHTEXT_NONE = cvar.RICHTEXT_NONE

TEXT_ATTR_CHARACTER = _richtext.TEXT_ATTR_CHARACTER
TEXT_ATTR_PARAGRAPH = _richtext.TEXT_ATTR_PARAGRAPH
TEXT_ATTR_ALL = _richtext.TEXT_ATTR_ALL
class RichTextCtrl(_windows.ScrolledWindow):
    """Proxy of C++ RichTextCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=RE_MULTILINE) -> RichTextCtrl
        """
        _richtext.RichTextCtrl_swiginit(self,_richtext.new_RichTextCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=RE_MULTILINE) -> bool
        """
        return _richtext.RichTextCtrl_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _richtext.RichTextCtrl_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, String value)"""
        return _richtext.RichTextCtrl_SetValue(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange(self, long from, long to) -> String"""
        return _richtext.RichTextCtrl_GetRange(*args, **kwargs)

    def GetLineLength(*args, **kwargs):
        """GetLineLength(self, long lineNo) -> int"""
        return _richtext.RichTextCtrl_GetLineLength(*args, **kwargs)

    def GetLineText(*args, **kwargs):
        """GetLineText(self, long lineNo) -> String"""
        return _richtext.RichTextCtrl_GetLineText(*args, **kwargs)

    def GetNumberOfLines(*args, **kwargs):
        """GetNumberOfLines(self) -> int"""
        return _richtext.RichTextCtrl_GetNumberOfLines(*args, **kwargs)

    def IsModified(*args, **kwargs):
        """IsModified(self) -> bool"""
        return _richtext.RichTextCtrl_IsModified(*args, **kwargs)

    def IsEditable(*args, **kwargs):
        """IsEditable(self) -> bool"""
        return _richtext.RichTextCtrl_IsEditable(*args, **kwargs)

    def IsSingleLine(*args, **kwargs):
        """IsSingleLine(self) -> bool"""
        return _richtext.RichTextCtrl_IsSingleLine(*args, **kwargs)

    def IsMultiLine(*args, **kwargs):
        """IsMultiLine(self) -> bool"""
        return _richtext.RichTextCtrl_IsMultiLine(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """
        GetSelection() --> (start, end)

        Returns the start and end positions of the current selection.  If the
        values are the same then there is no selection.
        """
        return _richtext.RichTextCtrl_GetSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """GetStringSelection(self) -> String"""
        return _richtext.RichTextCtrl_GetStringSelection(*args, **kwargs)

    def GetFilename(*args, **kwargs):
        """GetFilename(self) -> String"""
        return _richtext.RichTextCtrl_GetFilename(*args, **kwargs)

    def SetFilename(*args, **kwargs):
        """SetFilename(self, String filename)"""
        return _richtext.RichTextCtrl_SetFilename(*args, **kwargs)

    def SetDelayedLayoutThreshold(*args, **kwargs):
        """
        SetDelayedLayoutThreshold(self, long threshold)

        Set the threshold in character positions for doing layout optimization
        during sizing.
        """
        return _richtext.RichTextCtrl_SetDelayedLayoutThreshold(*args, **kwargs)

    def GetDelayedLayoutThreshold(*args, **kwargs):
        """
        GetDelayedLayoutThreshold(self) -> long

        Get the threshold in character positions for doing layout optimization
        during sizing.
        """
        return _richtext.RichTextCtrl_GetDelayedLayoutThreshold(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear(self)"""
        return _richtext.RichTextCtrl_Clear(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(self, long from, long to, String value)"""
        return _richtext.RichTextCtrl_Replace(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(self, long from, long to)"""
        return _richtext.RichTextCtrl_Remove(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """
        LoadFile(self, String file, int type=RICHTEXT_TYPE_ANY) -> bool

        Load the contents of the document from the given filename.
        """
        return _richtext.RichTextCtrl_LoadFile(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """
        SaveFile(self, String file=EmptyString, int type=RICHTEXT_TYPE_ANY) -> bool

        Save the contents of the document to the given filename, or if the
        empty string is passed then to the filename set with `SetFilename`.
        """
        return _richtext.RichTextCtrl_SaveFile(*args, **kwargs)

    def SetHandlerFlags(*args, **kwargs):
        """
        SetHandlerFlags(self, int flags)

        Set the handler flags, controlling loading and saving.
        """
        return _richtext.RichTextCtrl_SetHandlerFlags(*args, **kwargs)

    def GetHandlerFlags(*args, **kwargs):
        """
        GetHandlerFlags(self) -> int

        Get the handler flags, controlling loading and saving.
        """
        return _richtext.RichTextCtrl_GetHandlerFlags(*args, **kwargs)

    def MarkDirty(*args, **kwargs):
        """
        MarkDirty(self)

        Sets the dirty flag, meaning that the contents of the control have
        changed and need to be saved.
        """
        return _richtext.RichTextCtrl_MarkDirty(*args, **kwargs)

    def DiscardEdits(*args, **kwargs):
        """
        DiscardEdits(self)

        Clears the dirty flag.
        :see: `MarkDirty`
        """
        return _richtext.RichTextCtrl_DiscardEdits(*args, **kwargs)

    def SetMaxLength(*args, **kwargs):
        """
        SetMaxLength(self, unsigned long len)

        Set the max number of characters which may be entered in a single line
        text control.
        """
        return _richtext.RichTextCtrl_SetMaxLength(*args, **kwargs)

    def WriteText(*args, **kwargs):
        """
        WriteText(self, String text)

        Insert text at the current position.
        """
        return _richtext.RichTextCtrl_WriteText(*args, **kwargs)

    def AppendText(*args, **kwargs):
        """
        AppendText(self, String text)

        Append text to the end of the document.
        """
        return _richtext.RichTextCtrl_AppendText(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """
        SetStyle(self, RichTextRange range, RichTextAttr style) -> bool

        Set the style for the text in ``range`` to ``style``
        """
        return _richtext.RichTextCtrl_SetStyle(*args, **kwargs)

    def SetStyleEx(*args, **kwargs):
        """
        SetStyleEx(self, RichTextRange range, RichTextAttr style, int flags=RICHTEXT_SETSTYLE_WITH_UNDO) -> bool

        Extended style setting operation with flags including:
        RICHTEXT_SETSTYLE_WITH_UNDO, RICHTEXT_SETSTYLE_OPTIMIZE,
        RICHTEXT_SETSTYLE_PARAGRAPHS_ONLY, RICHTEXT_SETSTYLE_CHARACTERS_ONLY
        """
        return _richtext.RichTextCtrl_SetStyleEx(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """
        GetStyle(self, long position, RichTextAttr style) -> bool

        Retrieve the style used at the given position.  Copies the style
        values at ``position`` into the ``style`` parameter and returns ``True``
        if successful.  Returns ``False`` otherwise.
        """
        return _richtext.RichTextCtrl_GetStyle(*args, **kwargs)

    def GetUncombinedStyle(*args, **kwargs):
        """
        GetUncombinedStyle(self, long position, RichTextAttr style) -> bool

        Get the content (uncombined) attributes for this position.  Copies the
        style values at ``position`` into the ``style`` parameter and returns
        ``True`` if successful.  Returns ``False`` otherwise.
        """
        return _richtext.RichTextCtrl_GetUncombinedStyle(*args, **kwargs)

    def SetDefaultStyle(*args, **kwargs):
        """
        SetDefaultStyle(self, RichTextAttr style) -> bool

        Set the style used by default for the rich text document.
        """
        return _richtext.RichTextCtrl_SetDefaultStyle(*args, **kwargs)

    def GetDefaultStyle(*args, **kwargs):
        """
        GetDefaultStyle(self) -> RichTextAttr

        Retrieves a copy of the default style object.
        """
        return _richtext.RichTextCtrl_GetDefaultStyle(*args, **kwargs)

    def XYToPosition(*args, **kwargs):
        """
        XYToPosition(self, long x, long y) -> long

        Translate a col,row coordinants into a document position.
        """
        return _richtext.RichTextCtrl_XYToPosition(*args, **kwargs)

    def PositionToXY(*args, **kwargs):
        """
        PositionToXY(self, long pos) --> (x, y)

        Retrieves the col,row for the given position within the document
        """
        return _richtext.RichTextCtrl_PositionToXY(*args, **kwargs)

    def ShowPosition(*args, **kwargs):
        """
        ShowPosition(self, long position)

        Ensure that the given position in the document is visible.
        """
        return _richtext.RichTextCtrl_ShowPosition(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(self, Point pt) --> (result, pos)

        Returns the character position at the given point in pixels.  Note
        that ``pt`` should be given in device coordinates, and not be adjusted
        for the client area origin nor for scrolling.  The return value is a
        tuple of the hit test result and the position.
        """
        return _richtext.RichTextCtrl_HitTest(*args, **kwargs)

    def HitTestXY(*args, **kwargs):
        """
        HitTestRC(self, Point pt) --> (result, col, row)

        Returns the column and row of the given point in pixels.  Note that
        ``pt`` should be given in device coordinates, and not be adjusted for
        the client area origin nor for scrolling.  The return value is a tuple
        of the hit test result and the column and row values.
        """
        return _richtext.RichTextCtrl_HitTestXY(*args, **kwargs)

    def Copy(*args, **kwargs):
        """
        Copy(self)

        Copies the selected text to the clipboard.
        """
        return _richtext.RichTextCtrl_Copy(*args, **kwargs)

    def Cut(*args, **kwargs):
        """
        Cut(self)

        Copies the selected text to the clipboard and removes the selection.
        """
        return _richtext.RichTextCtrl_Cut(*args, **kwargs)

    def Paste(*args, **kwargs):
        """
        Paste(self)

        Pastes text from the clipboard into the document at the current
        insertion point.
        """
        return _richtext.RichTextCtrl_Paste(*args, **kwargs)

    def DeleteSelection(*args, **kwargs):
        """
        DeleteSelection(self)

        Remove the current selection.
        """
        return _richtext.RichTextCtrl_DeleteSelection(*args, **kwargs)

    def CanCopy(*args, **kwargs):
        """
        CanCopy(self) -> bool

        Returns ``True`` if the selection can be copied to the clipboard.
        """
        return _richtext.RichTextCtrl_CanCopy(*args, **kwargs)

    def CanCut(*args, **kwargs):
        """
        CanCut(self) -> bool

        Returns ``True`` if the selection can be cut to the clipboard.
        """
        return _richtext.RichTextCtrl_CanCut(*args, **kwargs)

    def CanPaste(*args, **kwargs):
        """
        CanPaste(self) -> bool

        Returns ``True`` if the current contents of the clipboard can be
        pasted into the document.
        """
        return _richtext.RichTextCtrl_CanPaste(*args, **kwargs)

    def CanDeleteSelection(*args, **kwargs):
        """
        CanDeleteSelection(self) -> bool

        Returns ``True`` if the selection can be removed from the document.
        """
        return _richtext.RichTextCtrl_CanDeleteSelection(*args, **kwargs)

    def Undo(*args, **kwargs):
        """
        Undo(self)

        If the last operation can be undone, undoes the last operation.
        """
        return _richtext.RichTextCtrl_Undo(*args, **kwargs)

    def Redo(*args, **kwargs):
        """
        Redo(self)

        If the last operation can be redone, redoes the last operation.
        """
        return _richtext.RichTextCtrl_Redo(*args, **kwargs)

    def CanUndo(*args, **kwargs):
        """
        CanUndo(self) -> bool

        Returns ``True`` if the last operation can be undone.
        """
        return _richtext.RichTextCtrl_CanUndo(*args, **kwargs)

    def CanRedo(*args, **kwargs):
        """
        CanRedo(self) -> bool

        Returns ``True`` if the last operation can be redone.
        """
        return _richtext.RichTextCtrl_CanRedo(*args, **kwargs)

    def SetInsertionPoint(*args, **kwargs):
        """
        SetInsertionPoint(self, long pos)

        Sets the insertion point at the given position.
        """
        return _richtext.RichTextCtrl_SetInsertionPoint(*args, **kwargs)

    def SetInsertionPointEnd(*args, **kwargs):
        """
        SetInsertionPointEnd(self)

        Moves the insertion point to the end of the document.
        """
        return _richtext.RichTextCtrl_SetInsertionPointEnd(*args, **kwargs)

    def GetInsertionPoint(*args, **kwargs):
        """
        GetInsertionPoint(self) -> long

        Returns the insertion point. This is defined as the zero based index
        of the character position to the right of the insertion point.
        """
        return _richtext.RichTextCtrl_GetInsertionPoint(*args, **kwargs)

    def GetLastPosition(*args, **kwargs):
        """
        GetLastPosition(self) -> long

        Returns the zero based index of the last position in the document.
        """
        return _richtext.RichTextCtrl_GetLastPosition(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """
        SetSelection(self, long from, long to)

        Selects the text starting at the first position up to (but not
        including) the character at the last position. If both parameters are
        equal to -1 then all text in the control is selected.
        """
        return _richtext.RichTextCtrl_SetSelection(*args, **kwargs)

    def SelectAll(*args, **kwargs):
        """
        SelectAll(self)

        Select all text in the document.
        """
        return _richtext.RichTextCtrl_SelectAll(*args, **kwargs)

    def SetEditable(*args, **kwargs):
        """
        SetEditable(self, bool editable)

        Makes the document editable or read-only, overriding the RE_READONLY
        flag.
        """
        return _richtext.RichTextCtrl_SetEditable(*args, **kwargs)

    def HasSelection(*args, **kwargs):
        """HasSelection(self) -> bool"""
        return _richtext.RichTextCtrl_HasSelection(*args, **kwargs)

    def WriteImage(*args, **kwargs):
        """WriteImage(self, Image image, int bitmapType=BITMAP_TYPE_PNG) -> bool"""
        return _richtext.RichTextCtrl_WriteImage(*args, **kwargs)

    def WriteBitmap(*args, **kwargs):
        """WriteBitmap(self, Bitmap bitmap, int bitmapType=BITMAP_TYPE_PNG) -> bool"""
        return _richtext.RichTextCtrl_WriteBitmap(*args, **kwargs)

    def WriteImageFile(*args, **kwargs):
        """WriteImageFile(self, String filename, int bitmapType) -> bool"""
        return _richtext.RichTextCtrl_WriteImageFile(*args, **kwargs)

    def WriteImageBlock(*args, **kwargs):
        """WriteImageBlock(self, wxRichTextImageBlock imageBlock) -> bool"""
        return _richtext.RichTextCtrl_WriteImageBlock(*args, **kwargs)

    def Newline(*args, **kwargs):
        """Newline(self) -> bool"""
        return _richtext.RichTextCtrl_Newline(*args, **kwargs)

    def SetBasicStyle(*args, **kwargs):
        """SetBasicStyle(self, RichTextAttr style)"""
        return _richtext.RichTextCtrl_SetBasicStyle(*args, **kwargs)

    def EndStyle(*args, **kwargs):
        """EndStyle(self) -> bool"""
        return _richtext.RichTextCtrl_EndStyle(*args, **kwargs)

    def EndAllStyles(*args, **kwargs):
        """EndAllStyles(self) -> bool"""
        return _richtext.RichTextCtrl_EndAllStyles(*args, **kwargs)

    def BeginBold(*args, **kwargs):
        """BeginBold(self) -> bool"""
        return _richtext.RichTextCtrl_BeginBold(*args, **kwargs)

    def EndBold(*args, **kwargs):
        """EndBold(self) -> bool"""
        return _richtext.RichTextCtrl_EndBold(*args, **kwargs)

    def BeginItalic(*args, **kwargs):
        """BeginItalic(self) -> bool"""
        return _richtext.RichTextCtrl_BeginItalic(*args, **kwargs)

    def EndItalic(*args, **kwargs):
        """EndItalic(self) -> bool"""
        return _richtext.RichTextCtrl_EndItalic(*args, **kwargs)

    def BeginUnderline(*args, **kwargs):
        """BeginUnderline(self) -> bool"""
        return _richtext.RichTextCtrl_BeginUnderline(*args, **kwargs)

    def EndUnderline(*args, **kwargs):
        """EndUnderline(self) -> bool"""
        return _richtext.RichTextCtrl_EndUnderline(*args, **kwargs)

    def BeginFontSize(*args, **kwargs):
        """BeginFontSize(self, int pointSize) -> bool"""
        return _richtext.RichTextCtrl_BeginFontSize(*args, **kwargs)

    def EndFontSize(*args, **kwargs):
        """EndFontSize(self) -> bool"""
        return _richtext.RichTextCtrl_EndFontSize(*args, **kwargs)

    def BeginFont(*args, **kwargs):
        """BeginFont(self, Font font) -> bool"""
        return _richtext.RichTextCtrl_BeginFont(*args, **kwargs)

    def EndFont(*args, **kwargs):
        """EndFont(self) -> bool"""
        return _richtext.RichTextCtrl_EndFont(*args, **kwargs)

    def BeginTextColour(*args, **kwargs):
        """BeginTextColour(self, Colour colour) -> bool"""
        return _richtext.RichTextCtrl_BeginTextColour(*args, **kwargs)

    def EndTextColour(*args, **kwargs):
        """EndTextColour(self) -> bool"""
        return _richtext.RichTextCtrl_EndTextColour(*args, **kwargs)

    def BeginAlignment(*args, **kwargs):
        """BeginAlignment(self, int alignment) -> bool"""
        return _richtext.RichTextCtrl_BeginAlignment(*args, **kwargs)

    def EndAlignment(*args, **kwargs):
        """EndAlignment(self) -> bool"""
        return _richtext.RichTextCtrl_EndAlignment(*args, **kwargs)

    def BeginLeftIndent(*args, **kwargs):
        """BeginLeftIndent(self, int leftIndent, int leftSubIndent=0) -> bool"""
        return _richtext.RichTextCtrl_BeginLeftIndent(*args, **kwargs)

    def EndLeftIndent(*args, **kwargs):
        """EndLeftIndent(self) -> bool"""
        return _richtext.RichTextCtrl_EndLeftIndent(*args, **kwargs)

    def BeginRightIndent(*args, **kwargs):
        """BeginRightIndent(self, int rightIndent) -> bool"""
        return _richtext.RichTextCtrl_BeginRightIndent(*args, **kwargs)

    def EndRightIndent(*args, **kwargs):
        """EndRightIndent(self) -> bool"""
        return _richtext.RichTextCtrl_EndRightIndent(*args, **kwargs)

    def BeginParagraphSpacing(*args, **kwargs):
        """BeginParagraphSpacing(self, int before, int after) -> bool"""
        return _richtext.RichTextCtrl_BeginParagraphSpacing(*args, **kwargs)

    def EndParagraphSpacing(*args, **kwargs):
        """EndParagraphSpacing(self) -> bool"""
        return _richtext.RichTextCtrl_EndParagraphSpacing(*args, **kwargs)

    def BeginLineSpacing(*args, **kwargs):
        """BeginLineSpacing(self, int lineSpacing) -> bool"""
        return _richtext.RichTextCtrl_BeginLineSpacing(*args, **kwargs)

    def EndLineSpacing(*args, **kwargs):
        """EndLineSpacing(self) -> bool"""
        return _richtext.RichTextCtrl_EndLineSpacing(*args, **kwargs)

    def BeginNumberedBullet(*args, **kwargs):
        """
        BeginNumberedBullet(self, int bulletNumber, int leftIndent, int leftSubIndent, 
            int bulletStyle=wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD) -> bool
        """
        return _richtext.RichTextCtrl_BeginNumberedBullet(*args, **kwargs)

    def EndNumberedBullet(*args, **kwargs):
        """EndNumberedBullet(self) -> bool"""
        return _richtext.RichTextCtrl_EndNumberedBullet(*args, **kwargs)

    def BeginSymbolBullet(*args, **kwargs):
        """BeginSymbolBullet(self, String symbol, int leftIndent, int leftSubIndent, int bulletStyle=TEXT_ATTR_BULLET_STYLE_SYMBOL) -> bool"""
        return _richtext.RichTextCtrl_BeginSymbolBullet(*args, **kwargs)

    def EndSymbolBullet(*args, **kwargs):
        """EndSymbolBullet(self) -> bool"""
        return _richtext.RichTextCtrl_EndSymbolBullet(*args, **kwargs)

    def BeginCharacterStyle(*args, **kwargs):
        """BeginCharacterStyle(self, String characterStyle) -> bool"""
        return _richtext.RichTextCtrl_BeginCharacterStyle(*args, **kwargs)

    def EndCharacterStyle(*args, **kwargs):
        """EndCharacterStyle(self) -> bool"""
        return _richtext.RichTextCtrl_EndCharacterStyle(*args, **kwargs)

    def BeginParagraphStyle(*args, **kwargs):
        """BeginParagraphStyle(self, String paragraphStyle) -> bool"""
        return _richtext.RichTextCtrl_BeginParagraphStyle(*args, **kwargs)

    def EndParagraphStyle(*args, **kwargs):
        """EndParagraphStyle(self) -> bool"""
        return _richtext.RichTextCtrl_EndParagraphStyle(*args, **kwargs)

    def BeginListStyle(*args, **kwargs):
        """
        BeginListStyle(self, String listStyle, int level=1, int number=1) -> bool

        Begin named list style.
        """
        return _richtext.RichTextCtrl_BeginListStyle(*args, **kwargs)

    def EndListStyle(*args, **kwargs):
        """
        EndListStyle(self) -> bool

        End named list style.
        """
        return _richtext.RichTextCtrl_EndListStyle(*args, **kwargs)

    def BeginURL(*args, **kwargs):
        """
        BeginURL(self, String url, String characterStyle=wxEmptyString) -> bool

        Begin URL.
        """
        return _richtext.RichTextCtrl_BeginURL(*args, **kwargs)

    def EndURL(*args, **kwargs):
        """
        EndURL(self) -> bool

        End URL.
        """
        return _richtext.RichTextCtrl_EndURL(*args, **kwargs)

    def SetDefaultStyleToCursorStyle(*args, **kwargs):
        """SetDefaultStyleToCursorStyle(self) -> bool"""
        return _richtext.RichTextCtrl_SetDefaultStyleToCursorStyle(*args, **kwargs)

    def SelectNone(*args, **kwargs):
        """SelectNone(self)"""
        return _richtext.RichTextCtrl_SelectNone(*args, **kwargs)

    def SelectWord(*args, **kwargs):
        """SelectWord(self, long position) -> bool"""
        return _richtext.RichTextCtrl_SelectWord(*args, **kwargs)

    def GetSelectionRange(*args, **kwargs):
        """GetSelectionRange(self) -> RichTextRange"""
        return _richtext.RichTextCtrl_GetSelectionRange(*args, **kwargs)

    def SetSelectionRange(*args, **kwargs):
        """SetSelectionRange(self, RichTextRange range)"""
        return _richtext.RichTextCtrl_SetSelectionRange(*args, **kwargs)

    def GetInternalSelectionRange(*args, **kwargs):
        """GetInternalSelectionRange(self) -> RichTextRange"""
        return _richtext.RichTextCtrl_GetInternalSelectionRange(*args, **kwargs)

    def SetInternalSelectionRange(*args, **kwargs):
        """SetInternalSelectionRange(self, RichTextRange range)"""
        return _richtext.RichTextCtrl_SetInternalSelectionRange(*args, **kwargs)

    def AddParagraph(*args, **kwargs):
        """AddParagraph(self, String text) -> RichTextRange"""
        return _richtext.RichTextCtrl_AddParagraph(*args, **kwargs)

    def AddImage(*args, **kwargs):
        """AddImage(self, Image image) -> RichTextRange"""
        return _richtext.RichTextCtrl_AddImage(*args, **kwargs)

    def LayoutContent(*args, **kwargs):
        """LayoutContent(self, bool onlyVisibleRect=False) -> bool"""
        return _richtext.RichTextCtrl_LayoutContent(*args, **kwargs)

    def MoveCaret(*args, **kwargs):
        """MoveCaret(self, long pos, bool showAtLineStart=False) -> bool"""
        return _richtext.RichTextCtrl_MoveCaret(*args, **kwargs)

    def MoveRight(*args, **kwargs):
        """MoveRight(self, int noPositions=1, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveRight(*args, **kwargs)

    def MoveLeft(*args, **kwargs):
        """MoveLeft(self, int noPositions=1, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveLeft(*args, **kwargs)

    def MoveUp(*args, **kwargs):
        """MoveUp(self, int noLines=1, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveUp(*args, **kwargs)

    def MoveDown(*args, **kwargs):
        """MoveDown(self, int noLines=1, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveDown(*args, **kwargs)

    def MoveToLineEnd(*args, **kwargs):
        """MoveToLineEnd(self, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveToLineEnd(*args, **kwargs)

    def MoveToLineStart(*args, **kwargs):
        """MoveToLineStart(self, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveToLineStart(*args, **kwargs)

    def MoveToParagraphEnd(*args, **kwargs):
        """MoveToParagraphEnd(self, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveToParagraphEnd(*args, **kwargs)

    def MoveToParagraphStart(*args, **kwargs):
        """MoveToParagraphStart(self, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveToParagraphStart(*args, **kwargs)

    def MoveHome(*args, **kwargs):
        """MoveHome(self, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveHome(*args, **kwargs)

    def MoveEnd(*args, **kwargs):
        """MoveEnd(self, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_MoveEnd(*args, **kwargs)

    def PageUp(*args, **kwargs):
        """PageUp(self, int noPages=1, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_PageUp(*args, **kwargs)

    def PageDown(*args, **kwargs):
        """PageDown(self, int noPages=1, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_PageDown(*args, **kwargs)

    def WordLeft(*args, **kwargs):
        """WordLeft(self, int noPages=1, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_WordLeft(*args, **kwargs)

    def WordRight(*args, **kwargs):
        """WordRight(self, int noPages=1, int flags=0) -> bool"""
        return _richtext.RichTextCtrl_WordRight(*args, **kwargs)

    def GetBuffer(*args, **kwargs):
        """GetBuffer(self) -> wxRichTextBuffer"""
        return _richtext.RichTextCtrl_GetBuffer(*args, **kwargs)

    def BeginBatchUndo(*args, **kwargs):
        """BeginBatchUndo(self, String cmdName) -> bool"""
        return _richtext.RichTextCtrl_BeginBatchUndo(*args, **kwargs)

    def EndBatchUndo(*args, **kwargs):
        """EndBatchUndo(self) -> bool"""
        return _richtext.RichTextCtrl_EndBatchUndo(*args, **kwargs)

    def BatchingUndo(*args, **kwargs):
        """BatchingUndo(self) -> bool"""
        return _richtext.RichTextCtrl_BatchingUndo(*args, **kwargs)

    def BeginSuppressUndo(*args, **kwargs):
        """BeginSuppressUndo(self) -> bool"""
        return _richtext.RichTextCtrl_BeginSuppressUndo(*args, **kwargs)

    def EndSuppressUndo(*args, **kwargs):
        """EndSuppressUndo(self) -> bool"""
        return _richtext.RichTextCtrl_EndSuppressUndo(*args, **kwargs)

    def SuppressingUndo(*args, **kwargs):
        """SuppressingUndo(self) -> bool"""
        return _richtext.RichTextCtrl_SuppressingUndo(*args, **kwargs)

    def HasCharacterAttributes(*args, **kwargs):
        """HasCharacterAttributes(self, RichTextRange range, RichTextAttr style) -> bool"""
        return _richtext.RichTextCtrl_HasCharacterAttributes(*args, **kwargs)

    def HasParagraphAttributes(*args, **kwargs):
        """HasParagraphAttributes(self, RichTextRange range, RichTextAttr style) -> bool"""
        return _richtext.RichTextCtrl_HasParagraphAttributes(*args, **kwargs)

    def IsSelectionBold(*args, **kwargs):
        """IsSelectionBold(self) -> bool"""
        return _richtext.RichTextCtrl_IsSelectionBold(*args, **kwargs)

    def IsSelectionItalics(*args, **kwargs):
        """IsSelectionItalics(self) -> bool"""
        return _richtext.RichTextCtrl_IsSelectionItalics(*args, **kwargs)

    def IsSelectionUnderlined(*args, **kwargs):
        """IsSelectionUnderlined(self) -> bool"""
        return _richtext.RichTextCtrl_IsSelectionUnderlined(*args, **kwargs)

    def IsSelectionAligned(*args, **kwargs):
        """IsSelectionAligned(self, int alignment) -> bool"""
        return _richtext.RichTextCtrl_IsSelectionAligned(*args, **kwargs)

    def ApplyBoldToSelection(*args, **kwargs):
        """ApplyBoldToSelection(self) -> bool"""
        return _richtext.RichTextCtrl_ApplyBoldToSelection(*args, **kwargs)

    def ApplyItalicToSelection(*args, **kwargs):
        """ApplyItalicToSelection(self) -> bool"""
        return _richtext.RichTextCtrl_ApplyItalicToSelection(*args, **kwargs)

    def ApplyUnderlineToSelection(*args, **kwargs):
        """ApplyUnderlineToSelection(self) -> bool"""
        return _richtext.RichTextCtrl_ApplyUnderlineToSelection(*args, **kwargs)

    def ApplyAlignmentToSelection(*args, **kwargs):
        """ApplyAlignmentToSelection(self, int alignment) -> bool"""
        return _richtext.RichTextCtrl_ApplyAlignmentToSelection(*args, **kwargs)

    def SetStyleSheet(*args, **kwargs):
        """SetStyleSheet(self, wxRichTextStyleSheet styleSheet)"""
        return _richtext.RichTextCtrl_SetStyleSheet(*args, **kwargs)

    def GetStyleSheet(*args, **kwargs):
        """GetStyleSheet(self) -> wxRichTextStyleSheet"""
        return _richtext.RichTextCtrl_GetStyleSheet(*args, **kwargs)

    def ApplyStyleSheet(*args, **kwargs):
        """ApplyStyleSheet(self, wxRichTextStyleSheet styleSheet=None) -> bool"""
        return _richtext.RichTextCtrl_ApplyStyleSheet(*args, **kwargs)

    Buffer = property(GetBuffer,doc="See `GetBuffer`") 
    DefaultStyle = property(GetDefaultStyle,SetDefaultStyle,doc="See `GetDefaultStyle` and `SetDefaultStyle`") 
    DelayedLayoutThreshold = property(GetDelayedLayoutThreshold,SetDelayedLayoutThreshold,doc="See `GetDelayedLayoutThreshold` and `SetDelayedLayoutThreshold`") 
    Filename = property(GetFilename,SetFilename,doc="See `GetFilename` and `SetFilename`") 
    InsertionPoint = property(GetInsertionPoint,SetInsertionPoint,doc="See `GetInsertionPoint` and `SetInsertionPoint`") 
    InternalSelectionRange = property(GetInternalSelectionRange,SetInternalSelectionRange,doc="See `GetInternalSelectionRange` and `SetInternalSelectionRange`") 
    LastPosition = property(GetLastPosition,doc="See `GetLastPosition`") 
    NumberOfLines = property(GetNumberOfLines,doc="See `GetNumberOfLines`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
    SelectionRange = property(GetSelectionRange,SetSelectionRange,doc="See `GetSelectionRange` and `SetSelectionRange`") 
    StringSelection = property(GetStringSelection,doc="See `GetStringSelection`") 
    StyleSheet = property(GetStyleSheet,SetStyleSheet,doc="See `GetStyleSheet` and `SetStyleSheet`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_richtext.RichTextCtrl_swigregister(RichTextCtrl)

def PreRichTextCtrl(*args, **kwargs):
    """PreRichTextCtrl() -> RichTextCtrl"""
    val = _richtext.new_PreRichTextCtrl(*args, **kwargs)
    return val

wxEVT_COMMAND_RICHTEXT_LEFT_CLICK = _richtext.wxEVT_COMMAND_RICHTEXT_LEFT_CLICK
wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK = _richtext.wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK
wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK = _richtext.wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK
wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK = _richtext.wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK
wxEVT_COMMAND_RICHTEXT_RETURN = _richtext.wxEVT_COMMAND_RICHTEXT_RETURN
wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING = _richtext.wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING
wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED = _richtext.wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED
wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING = _richtext.wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING
wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED = _richtext.wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED
wxEVT_COMMAND_RICHTEXT_CHARACTER = _richtext.wxEVT_COMMAND_RICHTEXT_CHARACTER
wxEVT_COMMAND_RICHTEXT_DELETE = _richtext.wxEVT_COMMAND_RICHTEXT_DELETE
EVT_RICHTEXT_LEFT_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_LEFT_CLICK, 1)
EVT_RICHTEXT_RIGHT_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK, 1)
EVT_RICHTEXT_MIDDLE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK, 1)
EVT_RICHTEXT_LEFT_DCLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK, 1)
EVT_RICHTEXT_RETURN = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_RETURN, 1)
EVT_RICHTEXT_STYLESHEET_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING, 1)
EVT_RICHTEXT_STYLESHEET_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED, 1)
EVT_RICHTEXT_STYLESHEET_REPLACING = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING, 1)
EVT_RICHTEXT_STYLESHEET_REPLACED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED, 1)
EVT_RICHTEXT_CHARACTER = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_CHARACTER, 1)
EVT_RICHTEXT_DELETE = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_DELETE, 1)

class RichTextEvent(_core.NotifyEvent):
    """Proxy of C++ RichTextEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventType commandType=wxEVT_NULL, int winid=0) -> RichTextEvent"""
        _richtext.RichTextEvent_swiginit(self,_richtext.new_RichTextEvent(*args, **kwargs))
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> int"""
        return _richtext.RichTextEvent_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, int n)"""
        return _richtext.RichTextEvent_SetPosition(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags(self) -> int"""
        return _richtext.RichTextEvent_GetFlags(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(self, int flags)"""
        return _richtext.RichTextEvent_SetFlags(*args, **kwargs)

    Flags = property(GetFlags,SetFlags,doc="See `GetFlags` and `SetFlags`") 
    Index = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
_richtext.RichTextEvent_swigregister(RichTextEvent)



