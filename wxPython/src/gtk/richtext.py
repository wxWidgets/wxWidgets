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
TEXT_ALIGNMENT_DEFAULT = _richtext.TEXT_ALIGNMENT_DEFAULT
TEXT_ALIGNMENT_LEFT = _richtext.TEXT_ALIGNMENT_LEFT
TEXT_ALIGNMENT_CENTRE = _richtext.TEXT_ALIGNMENT_CENTRE
TEXT_ALIGNMENT_CENTER = _richtext.TEXT_ALIGNMENT_CENTER
TEXT_ALIGNMENT_RIGHT = _richtext.TEXT_ALIGNMENT_RIGHT
TEXT_ALIGNMENT_JUSTIFIED = _richtext.TEXT_ALIGNMENT_JUSTIFIED
#---------------------------------------------------------------------------

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
RICHTEXT_LAYOUT_SPECIFIED_RECT = _richtext.RICHTEXT_LAYOUT_SPECIFIED_RECT
RICHTEXT_DRAW_IGNORE_CACHE = _richtext.RICHTEXT_DRAW_IGNORE_CACHE
RICHTEXT_HITTEST_NONE = _richtext.RICHTEXT_HITTEST_NONE
RICHTEXT_HITTEST_BEFORE = _richtext.RICHTEXT_HITTEST_BEFORE
RICHTEXT_HITTEST_AFTER = _richtext.RICHTEXT_HITTEST_AFTER
RICHTEXT_HITTEST_ON = _richtext.RICHTEXT_HITTEST_ON
RICHTEXT_HITTEST_OUTSIDE = _richtext.RICHTEXT_HITTEST_OUTSIDE
RICHTEXT_FORMATTED = _richtext.RICHTEXT_FORMATTED
RICHTEXT_UNFORMATTED = _richtext.RICHTEXT_UNFORMATTED
RICHTEXT_SETSTYLE_NONE = _richtext.RICHTEXT_SETSTYLE_NONE
RICHTEXT_SETSTYLE_WITH_UNDO = _richtext.RICHTEXT_SETSTYLE_WITH_UNDO
RICHTEXT_SETSTYLE_OPTIMIZE = _richtext.RICHTEXT_SETSTYLE_OPTIMIZE
RICHTEXT_SETSTYLE_PARAGRAPHS_ONLY = _richtext.RICHTEXT_SETSTYLE_PARAGRAPHS_ONLY
RICHTEXT_SETSTYLE_CHARACTERS_ONLY = _richtext.RICHTEXT_SETSTYLE_CHARACTERS_ONLY
RICHTEXT_SETSTYLE_RENUMBER = _richtext.RICHTEXT_SETSTYLE_RENUMBER
RICHTEXT_SETSTYLE_SPECIFY_LEVEL = _richtext.RICHTEXT_SETSTYLE_SPECIFY_LEVEL
RICHTEXT_SETSTYLE_RESET = _richtext.RICHTEXT_SETSTYLE_RESET
RICHTEXT_SETSTYLE_REMOVE = _richtext.RICHTEXT_SETSTYLE_REMOVE
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
TEXT_ATTR_BULLET_TEXT = _richtext.TEXT_ATTR_BULLET_TEXT
TEXT_ATTR_BULLET_NAME = _richtext.TEXT_ATTR_BULLET_NAME
TEXT_ATTR_URL = _richtext.TEXT_ATTR_URL
TEXT_ATTR_PAGE_BREAK = _richtext.TEXT_ATTR_PAGE_BREAK
TEXT_ATTR_EFFECTS = _richtext.TEXT_ATTR_EFFECTS
TEXT_ATTR_OUTLINE_LEVEL = _richtext.TEXT_ATTR_OUTLINE_LEVEL
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
TEXT_ATTR_BULLET_STYLE_STANDARD = _richtext.TEXT_ATTR_BULLET_STYLE_STANDARD
TEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS = _richtext.TEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS
TEXT_ATTR_BULLET_STYLE_OUTLINE = _richtext.TEXT_ATTR_BULLET_STYLE_OUTLINE
TEXT_ATTR_BULLET_STYLE_ALIGN_LEFT = _richtext.TEXT_ATTR_BULLET_STYLE_ALIGN_LEFT
TEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT = _richtext.TEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT
TEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE = _richtext.TEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE
TEXT_ATTR_EFFECT_NONE = _richtext.TEXT_ATTR_EFFECT_NONE
TEXT_ATTR_EFFECT_CAPITALS = _richtext.TEXT_ATTR_EFFECT_CAPITALS
TEXT_ATTR_EFFECT_SMALL_CAPITALS = _richtext.TEXT_ATTR_EFFECT_SMALL_CAPITALS
TEXT_ATTR_EFFECT_STRIKETHROUGH = _richtext.TEXT_ATTR_EFFECT_STRIKETHROUGH
TEXT_ATTR_EFFECT_DOUBLE_STRIKETHROUGH = _richtext.TEXT_ATTR_EFFECT_DOUBLE_STRIKETHROUGH
TEXT_ATTR_EFFECT_SHADOW = _richtext.TEXT_ATTR_EFFECT_SHADOW
TEXT_ATTR_EFFECT_EMBOSS = _richtext.TEXT_ATTR_EFFECT_EMBOSS
TEXT_ATTR_EFFECT_OUTLINE = _richtext.TEXT_ATTR_EFFECT_OUTLINE
TEXT_ATTR_EFFECT_ENGRAVE = _richtext.TEXT_ATTR_EFFECT_ENGRAVE
TEXT_ATTR_EFFECT_SUPERSCRIPT = _richtext.TEXT_ATTR_EFFECT_SUPERSCRIPT
TEXT_ATTR_EFFECT_SUBSCRIPT = _richtext.TEXT_ATTR_EFFECT_SUBSCRIPT
TEXT_ATTR_LINE_SPACING_NORMAL = _richtext.TEXT_ATTR_LINE_SPACING_NORMAL
TEXT_ATTR_LINE_SPACING_HALF = _richtext.TEXT_ATTR_LINE_SPACING_HALF
TEXT_ATTR_LINE_SPACING_TWICE = _richtext.TEXT_ATTR_LINE_SPACING_TWICE
TEXT_ATTR_CHARACTER = _richtext.TEXT_ATTR_CHARACTER
TEXT_ATTR_PARAGRAPH = _richtext.TEXT_ATTR_PARAGRAPH
TEXT_ATTR_ALL = _richtext.TEXT_ATTR_ALL
#---------------------------------------------------------------------------

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

    def ToInternal(*args, **kwargs):
        """
        ToInternal(self) -> RichTextRange

        Convert to internal form: (n, n) is the range of a single character.
        """
        return _richtext.RichTextRange_ToInternal(*args, **kwargs)

    def FromInternal(*args, **kwargs):
        """
        FromInternal(self) -> RichTextRange

        Convert from internal to public API form: (n, n+1) is the range of a
        single character.
        """
        return _richtext.RichTextRange_FromInternal(*args, **kwargs)

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

#---------------------------------------------------------------------------

class RichTextAttr(object):
    """
    The RichTextAttr class stores information about the various attributes
    for a block of text, including font, colour, indents, alignments, and
    etc.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Colour colText=wxNullColour, Colour colBack=wxNullColour, 
            int alignment=TEXT_ALIGNMENT_DEFAULT) -> RichTextAttr

        The RichTextAttr class stores information about the various attributes
        for a block of text, including font, colour, indents, alignments, and
        etc.
        """
        _richtext.RichTextAttr_swiginit(self,_richtext.new_RichTextAttr(*args, **kwargs))
    __swig_destroy__ = _richtext.delete_RichTextAttr
    __del__ = lambda self : None;
    def Init(*args, **kwargs):
        """
        Init(self)

        Initialise this object.
        """
        return _richtext.RichTextAttr_Init(*args, **kwargs)

    def Copy(*args, **kwargs):
        """
        Copy(self, RichTextAttr attr)

        Copy from attr to self.
        """
        return _richtext.RichTextAttr_Copy(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, RichTextAttr attr) -> bool"""
        return _richtext.RichTextAttr___eq__(*args, **kwargs)

    def CreateFont(*args, **kwargs):
        """
        CreateFont(self) -> Font

        Create font from the font attributes in this attr object.
        """
        return _richtext.RichTextAttr_CreateFont(*args, **kwargs)

    def GetFontAttributes(*args, **kwargs):
        """
        GetFontAttributes(self, Font font) -> bool

        Set our font attributes from the font.
        """
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

    def SetListStyleName(*args, **kwargs):
        """SetListStyleName(self, String name)"""
        return _richtext.RichTextAttr_SetListStyleName(*args, **kwargs)

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

    def SetBulletName(*args, **kwargs):
        """SetBulletName(self, String name)"""
        return _richtext.RichTextAttr_SetBulletName(*args, **kwargs)

    def SetURL(*args, **kwargs):
        """SetURL(self, String url)"""
        return _richtext.RichTextAttr_SetURL(*args, **kwargs)

    def SetPageBreak(*args, **kwargs):
        """SetPageBreak(self, bool pageBreak=True)"""
        return _richtext.RichTextAttr_SetPageBreak(*args, **kwargs)

    def SetTextEffects(*args, **kwargs):
        """SetTextEffects(self, int effects)"""
        return _richtext.RichTextAttr_SetTextEffects(*args, **kwargs)

    def SetTextEffectFlags(*args, **kwargs):
        """SetTextEffectFlags(self, int effects)"""
        return _richtext.RichTextAttr_SetTextEffectFlags(*args, **kwargs)

    def SetOutlineLevel(*args, **kwargs):
        """SetOutlineLevel(self, int level)"""
        return _richtext.RichTextAttr_SetOutlineLevel(*args, **kwargs)

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

    def GetListStyleName(*args, **kwargs):
        """GetListStyleName(self) -> String"""
        return _richtext.RichTextAttr_GetListStyleName(*args, **kwargs)

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

    def GetBulletName(*args, **kwargs):
        """GetBulletName(self) -> String"""
        return _richtext.RichTextAttr_GetBulletName(*args, **kwargs)

    def GetURL(*args, **kwargs):
        """GetURL(self) -> String"""
        return _richtext.RichTextAttr_GetURL(*args, **kwargs)

    def GetTextEffects(*args, **kwargs):
        """GetTextEffects(self) -> int"""
        return _richtext.RichTextAttr_GetTextEffects(*args, **kwargs)

    def GetTextEffectFlags(*args, **kwargs):
        """GetTextEffectFlags(self) -> int"""
        return _richtext.RichTextAttr_GetTextEffectFlags(*args, **kwargs)

    def GetOutlineLevel(*args, **kwargs):
        """GetOutlineLevel(self) -> int"""
        return _richtext.RichTextAttr_GetOutlineLevel(*args, **kwargs)

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

    def HasFontWeight(*args, **kwargs):
        """HasFontWeight(self) -> bool"""
        return _richtext.RichTextAttr_HasFontWeight(*args, **kwargs)

    def HasFontSize(*args, **kwargs):
        """HasFontSize(self) -> bool"""
        return _richtext.RichTextAttr_HasFontSize(*args, **kwargs)

    def HasFontItalic(*args, **kwargs):
        """HasFontItalic(self) -> bool"""
        return _richtext.RichTextAttr_HasFontItalic(*args, **kwargs)

    def HasFontUnderlined(*args, **kwargs):
        """HasFontUnderlined(self) -> bool"""
        return _richtext.RichTextAttr_HasFontUnderlined(*args, **kwargs)

    def HasFontFaceName(*args, **kwargs):
        """HasFontFaceName(self) -> bool"""
        return _richtext.RichTextAttr_HasFontFaceName(*args, **kwargs)

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

    def HasListStyleName(*args, **kwargs):
        """HasListStyleName(self) -> bool"""
        return _richtext.RichTextAttr_HasListStyleName(*args, **kwargs)

    def HasBulletStyle(*args, **kwargs):
        """HasBulletStyle(self) -> bool"""
        return _richtext.RichTextAttr_HasBulletStyle(*args, **kwargs)

    def HasBulletNumber(*args, **kwargs):
        """HasBulletNumber(self) -> bool"""
        return _richtext.RichTextAttr_HasBulletNumber(*args, **kwargs)

    def HasBulletText(*args, **kwargs):
        """HasBulletText(self) -> bool"""
        return _richtext.RichTextAttr_HasBulletText(*args, **kwargs)

    def HasBulletName(*args, **kwargs):
        """HasBulletName(self) -> bool"""
        return _richtext.RichTextAttr_HasBulletName(*args, **kwargs)

    def HasURL(*args, **kwargs):
        """HasURL(self) -> bool"""
        return _richtext.RichTextAttr_HasURL(*args, **kwargs)

    def HasPageBreak(*args, **kwargs):
        """HasPageBreak(self) -> bool"""
        return _richtext.RichTextAttr_HasPageBreak(*args, **kwargs)

    def HasTextEffects(*args, **kwargs):
        """HasTextEffects(self) -> bool"""
        return _richtext.RichTextAttr_HasTextEffects(*args, **kwargs)

    def HasTextEffect(*args, **kwargs):
        """HasTextEffect(self, int effect) -> bool"""
        return _richtext.RichTextAttr_HasTextEffect(*args, **kwargs)

    def HasOutlineLevel(*args, **kwargs):
        """HasOutlineLevel(self) -> bool"""
        return _richtext.RichTextAttr_HasOutlineLevel(*args, **kwargs)

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
        """
        IsDefault(self) -> bool

        Returns false if we have any attributes set, true otherwise
        """
        return _richtext.RichTextAttr_IsDefault(*args, **kwargs)

    def Apply(*args, **kwargs):
        """
        Apply(self, RichTextAttr style, RichTextAttr compareWith=None) -> bool

        Merges the given attributes. Does not affect self. If compareWith is
        not None, then it will be used to mask out those attributes that are
        the same in style and compareWith, for situations where we don't want
        to explicitly set inherited attributes.

        """
        return _richtext.RichTextAttr_Apply(*args, **kwargs)

    def Combine(*args, **kwargs):
        """
        Combine(self, RichTextAttr style, RichTextAttr compareWith=None) -> RichTextAttr

        Merges the given attributes and returns the result. Does not affect
        self. If compareWith is not None, then it will be used to mask out
        those attributes that are the same in style and compareWith, for
        situations where we don't want to explicitly set inherited attributes.

        """
        return _richtext.RichTextAttr_Combine(*args, **kwargs)

    Alignment = property(GetAlignment,SetAlignment) 
    BackgroundColour = property(GetBackgroundColour,SetBackgroundColour) 
    BulletFont = property(GetBulletFont,SetBulletFont) 
    BulletNumber = property(GetBulletNumber,SetBulletNumber) 
    BulletStyle = property(GetBulletStyle,SetBulletStyle) 
    BulletText = property(GetBulletText,SetBulletText) 
    CharacterStyleName = property(GetCharacterStyleName,SetCharacterStyleName) 
    Flags = property(GetFlags,SetFlags) 
    Font = property(GetFont,SetFont) 
    FontAttributes = property(GetFontAttributes) 
    FontFaceName = property(GetFontFaceName,SetFontFaceName) 
    FontSize = property(GetFontSize,SetFontSize) 
    FontStyle = property(GetFontStyle,SetFontStyle) 
    FontUnderlined = property(GetFontUnderlined,SetFontUnderlined) 
    FontWeight = property(GetFontWeight,SetFontWeight) 
    LeftIndent = property(GetLeftIndent,SetLeftIndent) 
    LeftSubIndent = property(GetLeftSubIndent) 
    LineSpacing = property(GetLineSpacing,SetLineSpacing) 
    ParagraphSpacingAfter = property(GetParagraphSpacingAfter,SetParagraphSpacingAfter) 
    ParagraphSpacingBefore = property(GetParagraphSpacingBefore,SetParagraphSpacingBefore) 
    ParagraphStyleName = property(GetParagraphStyleName,SetParagraphStyleName) 
    RightIndent = property(GetRightIndent,SetRightIndent) 
    Tabs = property(GetTabs,SetTabs) 
    TextColour = property(GetTextColour,SetTextColour) 
    ListStyleName = property(GetListStyleName,SetListStyleName) 
    BulletName = property(GetBulletName,SetBulletName) 
    URL = property(GetURL,SetURL) 
    TextEffects = property(GetTextEffects,SetTextEffects) 
    TextEffectFlags = property(GetTextEffectFlags,SetTextEffectFlags) 
    OutlineLevel = property(GetOutlineLevel,SetOutlineLevel) 
_richtext.RichTextAttr_swigregister(RichTextAttr)
cvar = _richtext.cvar
RICHTEXT_ALL = cvar.RICHTEXT_ALL
RICHTEXT_NONE = cvar.RICHTEXT_NONE

class RichTextObject(_core.Object):
    """
    This is the base class for all drawable objects in a `RichTextCtrl`.

    The data displayed in a `RichTextCtrl` is handled by `RichTextBuffer`,
    and a `RichTextCtrl` always has one such buffer.

    The content is represented by a hierarchy of objects, all derived from
    `RichTextObject`. An object might be an image, a fragment of text, a
    paragraph, or a whole buffer. Objects store a an attribute object
    containing style information; a paragraph object can contain both
    paragraph and character information, but content objects such as text
    can only store character information. The final style displayed in the
    control or in a printout is a combination of base style, paragraph
    style and content (character) style.

    The top of the hierarchy is the buffer, a kind of
    `RichTextParagraphLayoutBox`. containing further `RichTextParagraph`
    objects, each of which can include text, images and potentially other
    types of objects.

    Each object maintains a range (start and end position) measured from
    the start of the main parent object.

    When Layout is called on an object, it is given a size which the
    object must limit itself to, or one or more flexible directions
    (vertical or horizontal). So, for example, a centred paragraph is
    given the page width to play with (minus any margins), but can extend
    indefinitely in the vertical direction. The implementation of Layout
    caches the calculated size and position.

    When the buffer is modified, a range is invalidated (marked as
    requiring layout), so that only the minimum amount of layout is
    performed.

    A paragraph of pure text with the same style contains just one further
    object, a `RichTextPlainText` object. When styling is applied to part
    of this object, the object is decomposed into separate objects, one
    object for each different character style. So each object within a
    paragraph always has just one attribute object to denote its character
    style. Of course, this can lead to fragmentation after a lot of edit
    operations, potentially leading to several objects with the same style
    where just one would do. So a Defragment function is called when
    updating the control's display, to ensure that the minimum number of
    objects is used.

    To implement your own RichTextObjects in Python you must derive a
    class from `PyRichTextObject`, which has been instrumented to forward
    the virtual C++ method calls to the Python methods in the derived
    class. (This class hasn't been implemented yet!)
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _richtext.delete_RichTextObject
    __del__ = lambda self : None;
    def Draw(*args, **kwargs):
        """
        Draw(self, DC dc, RichTextRange range, RichTextRange selectionRange, 
            Rect rect, int descent, int style) -> bool
        """
        return _richtext.RichTextObject_Draw(*args, **kwargs)

    def Layout(*args, **kwargs):
        """Layout(self, DC dc, Rect rect, int style) -> bool"""
        return _richtext.RichTextObject_Layout(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """HitTest(self, DC dc, Point pt, long OUTPUT) -> int"""
        return _richtext.RichTextObject_HitTest(*args, **kwargs)

    def FindPosition(*args, **kwargs):
        """FindPosition(self, DC dc, long index, Point OUTPUT, int OUTPUT, bool forceLineStart) -> bool"""
        return _richtext.RichTextObject_FindPosition(*args, **kwargs)

    def GetBestSize(*args, **kwargs):
        """GetBestSize(self) -> Size"""
        return _richtext.RichTextObject_GetBestSize(*args, **kwargs)

    def GetRangeSize(*args, **kwargs):
        """
        GetRangeSize(self, RichTextRange range, Size OUTPUT, int OUTPUT, DC dc, 
            int flags, Point position=wxPoint(0,0)) -> bool
        """
        return _richtext.RichTextObject_GetRangeSize(*args, **kwargs)

    def DoSplit(*args, **kwargs):
        """DoSplit(self, long pos) -> RichTextObject"""
        return _richtext.RichTextObject_DoSplit(*args, **kwargs)

    def CalculateRange(*args, **kwargs):
        """CalculateRange(self, long start, long OUTPUT)"""
        return _richtext.RichTextObject_CalculateRange(*args, **kwargs)

    def DeleteRange(*args, **kwargs):
        """DeleteRange(self, RichTextRange range) -> bool"""
        return _richtext.RichTextObject_DeleteRange(*args, **kwargs)

    def IsEmpty(*args, **kwargs):
        """IsEmpty(self) -> bool"""
        return _richtext.RichTextObject_IsEmpty(*args, **kwargs)

    def GetTextForRange(*args, **kwargs):
        """GetTextForRange(self, RichTextRange range) -> String"""
        return _richtext.RichTextObject_GetTextForRange(*args, **kwargs)

    def CanMerge(*args, **kwargs):
        """CanMerge(self, RichTextObject object) -> bool"""
        return _richtext.RichTextObject_CanMerge(*args, **kwargs)

    def Merge(*args, **kwargs):
        """Merge(self, RichTextObject object) -> bool"""
        return _richtext.RichTextObject_Merge(*args, **kwargs)

    def Dump(*args, **kwargs):
        """Dump(self) -> String"""
        return _richtext.RichTextObject_Dump(*args, **kwargs)

    def GetCachedSize(*args, **kwargs):
        """GetCachedSize(self) -> Size"""
        return _richtext.RichTextObject_GetCachedSize(*args, **kwargs)

    def SetCachedSize(*args, **kwargs):
        """SetCachedSize(self, Size sz)"""
        return _richtext.RichTextObject_SetCachedSize(*args, **kwargs)

    CachedSize = property(GetCachedSize,SetCachedSize) 
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _richtext.RichTextObject_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, Point pos)"""
        return _richtext.RichTextObject_SetPosition(*args, **kwargs)

    Position = property(GetPosition,SetPosition) 
    def GetRect(*args, **kwargs):
        """GetRect(self) -> Rect"""
        return _richtext.RichTextObject_GetRect(*args, **kwargs)

    Rect = property(GetRect) 
    def SetRange(*args, **kwargs):
        """SetRange(self, RichTextRange range)"""
        return _richtext.RichTextObject_SetRange(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange(self) -> RichTextRange"""
        return _richtext.RichTextObject_GetRange(*args, **kwargs)

    Range = property(GetRange,SetRange) 
    def GetDirty(*args, **kwargs):
        """GetDirty(self) -> bool"""
        return _richtext.RichTextObject_GetDirty(*args, **kwargs)

    def SetDirty(*args, **kwargs):
        """SetDirty(self, bool dirty)"""
        return _richtext.RichTextObject_SetDirty(*args, **kwargs)

    Dirty = property(GetDirty,SetDirty) 
    def IsComposite(*args, **kwargs):
        """IsComposite(self) -> bool"""
        return _richtext.RichTextObject_IsComposite(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent(self) -> RichTextObject"""
        return _richtext.RichTextObject_GetParent(*args, **kwargs)

    def SetParent(*args, **kwargs):
        """SetParent(self, RichTextObject parent)"""
        return _richtext.RichTextObject_SetParent(*args, **kwargs)

    Parent = property(GetParent,SetParent) 
    def SetSameMargins(*args, **kwargs):
        """SetSameMargins(self, int margin)"""
        return _richtext.RichTextObject_SetSameMargins(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """SetMargins(self, int leftMargin, int rightMargin, int topMargin, int bottomMargin)"""
        return _richtext.RichTextObject_SetMargins(*args, **kwargs)

    def GetLeftMargin(*args, **kwargs):
        """GetLeftMargin(self) -> int"""
        return _richtext.RichTextObject_GetLeftMargin(*args, **kwargs)

    def GetRightMargin(*args, **kwargs):
        """GetRightMargin(self) -> int"""
        return _richtext.RichTextObject_GetRightMargin(*args, **kwargs)

    def GetTopMargin(*args, **kwargs):
        """GetTopMargin(self) -> int"""
        return _richtext.RichTextObject_GetTopMargin(*args, **kwargs)

    def GetBottomMargin(*args, **kwargs):
        """GetBottomMargin(self) -> int"""
        return _richtext.RichTextObject_GetBottomMargin(*args, **kwargs)

    def SetAttributes(*args, **kwargs):
        """SetAttributes(self, RichTextAttr attr)"""
        return _richtext.RichTextObject_SetAttributes(*args, **kwargs)

    def GetAttributes(*args, **kwargs):
        """GetAttributes(self) -> RichTextAttr"""
        return _richtext.RichTextObject_GetAttributes(*args, **kwargs)

    Attributes = property(GetAttributes,SetAttributes) 
    def SetDescent(*args, **kwargs):
        """SetDescent(self, int descent)"""
        return _richtext.RichTextObject_SetDescent(*args, **kwargs)

    def GetDescent(*args, **kwargs):
        """GetDescent(self) -> int"""
        return _richtext.RichTextObject_GetDescent(*args, **kwargs)

    Descent = property(GetDescent,SetDescent) 
    def GetBuffer(*args, **kwargs):
        """GetBuffer(self) -> RichTextBuffer"""
        return _richtext.RichTextObject_GetBuffer(*args, **kwargs)

    def Clone(*args, **kwargs):
        """Clone(self) -> RichTextObject"""
        return _richtext.RichTextObject_Clone(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self, RichTextObject obj)"""
        return _richtext.RichTextObject_Copy(*args, **kwargs)

    def Reference(*args, **kwargs):
        """Reference(self)"""
        return _richtext.RichTextObject_Reference(*args, **kwargs)

    def Dereference(*args, **kwargs):
        """Dereference(self)"""
        return _richtext.RichTextObject_Dereference(*args, **kwargs)

    def ConvertTenthsMMToPixelsDC(*args, **kwargs):
        """ConvertTenthsMMToPixelsDC(self, DC dc, int units) -> int"""
        return _richtext.RichTextObject_ConvertTenthsMMToPixelsDC(*args, **kwargs)

    def ConvertTenthsMMToPixels(*args, **kwargs):
        """ConvertTenthsMMToPixels(int ppi, int units) -> int"""
        return _richtext.RichTextObject_ConvertTenthsMMToPixels(*args, **kwargs)

    ConvertTenthsMMToPixels = staticmethod(ConvertTenthsMMToPixels)
_richtext.RichTextObject_swigregister(RichTextObject)

def RichTextObject_ConvertTenthsMMToPixels(*args, **kwargs):
  """RichTextObject_ConvertTenthsMMToPixels(int ppi, int units) -> int"""
  return _richtext.RichTextObject_ConvertTenthsMMToPixels(*args, **kwargs)

class RichTextObjectList_iterator(object):
    """This class serves as an iterator for a wxRichTextObjectList object."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _richtext.delete_RichTextObjectList_iterator
    __del__ = lambda self : None;
    def next(*args, **kwargs):
        """next(self) -> RichTextObject"""
        return _richtext.RichTextObjectList_iterator_next(*args, **kwargs)

_richtext.RichTextObjectList_iterator_swigregister(RichTextObjectList_iterator)

class RichTextObjectList(object):
    """
    This class wraps a wxList-based class and gives it a Python
    sequence-like interface.  Sequence operations supported are length,
    index access and iteration.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _richtext.delete_RichTextObjectList
    __del__ = lambda self : None;
    def __len__(*args, **kwargs):
        """__len__(self) -> size_t"""
        return _richtext.RichTextObjectList___len__(*args, **kwargs)

    def __getitem__(*args, **kwargs):
        """__getitem__(self, size_t index) -> RichTextObject"""
        return _richtext.RichTextObjectList___getitem__(*args, **kwargs)

    def __contains__(*args, **kwargs):
        """__contains__(self, RichTextObject obj) -> bool"""
        return _richtext.RichTextObjectList___contains__(*args, **kwargs)

    def __iter__(*args, **kwargs):
        """__iter__(self) -> RichTextObjectList_iterator"""
        return _richtext.RichTextObjectList___iter__(*args, **kwargs)

    def __repr__(self):
        return "wxRichTextObjectList: " + repr(list(self))

_richtext.RichTextObjectList_swigregister(RichTextObjectList)

class RichTextCompositeObject(RichTextObject):
    """Objects of this class can contain other rich text objects."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _richtext.delete_RichTextCompositeObject
    __del__ = lambda self : None;
    def GetChildren(*args, **kwargs):
        """GetChildren(self) -> RichTextObjectList"""
        return _richtext.RichTextCompositeObject_GetChildren(*args, **kwargs)

    def GetChildCount(*args, **kwargs):
        """GetChildCount(self) -> size_t"""
        return _richtext.RichTextCompositeObject_GetChildCount(*args, **kwargs)

    def GetChild(*args, **kwargs):
        """GetChild(self, size_t n) -> RichTextObject"""
        return _richtext.RichTextCompositeObject_GetChild(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self, RichTextCompositeObject obj)"""
        return _richtext.RichTextCompositeObject_Copy(*args, **kwargs)

    def AppendChild(*args, **kwargs):
        """AppendChild(self, RichTextObject child) -> size_t"""
        return _richtext.RichTextCompositeObject_AppendChild(*args, **kwargs)

    def InsertChild(*args, **kwargs):
        """InsertChild(self, RichTextObject child, RichTextObject inFrontOf) -> bool"""
        return _richtext.RichTextCompositeObject_InsertChild(*args, **kwargs)

    def RemoveChild(self, child, deleteChild=False):
        val = _richtext.RichTextCompositeObject_RemoveChild(self, child, deleteChild)
        self.this.own(not deleteChild)
        return val


    def DeleteChildren(*args, **kwargs):
        """DeleteChildren(self) -> bool"""
        return _richtext.RichTextCompositeObject_DeleteChildren(*args, **kwargs)

    def Defragment(*args, **kwargs):
        """Defragment(self) -> bool"""
        return _richtext.RichTextCompositeObject_Defragment(*args, **kwargs)

_richtext.RichTextCompositeObject_swigregister(RichTextCompositeObject)

class RichTextBox(RichTextCompositeObject):
    """This defines a 2D space to lay out objects."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, RichTextObject parent=None) -> RichTextBox

        This defines a 2D space to lay out objects.
        """
        _richtext.RichTextBox_swiginit(self,_richtext.new_RichTextBox(*args, **kwargs))
    def Copy(*args, **kwargs):
        """Copy(self, RichTextBox obj)"""
        return _richtext.RichTextBox_Copy(*args, **kwargs)

_richtext.RichTextBox_swigregister(RichTextBox)

class RichTextParagraphLayoutBox(RichTextBox):
    """Proxy of C++ RichTextParagraphLayoutBox class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, RichTextObject parent=None) -> RichTextParagraphLayoutBox"""
        _richtext.RichTextParagraphLayoutBox_swiginit(self,_richtext.new_RichTextParagraphLayoutBox(*args, **kwargs))
    def SetRichTextCtrl(*args, **kwargs):
        """SetRichTextCtrl(self, RichTextCtrl ctrl)"""
        return _richtext.RichTextParagraphLayoutBox_SetRichTextCtrl(*args, **kwargs)

    def GetRichTextCtrl(*args, **kwargs):
        """GetRichTextCtrl(self) -> RichTextCtrl"""
        return _richtext.RichTextParagraphLayoutBox_GetRichTextCtrl(*args, **kwargs)

    def SetPartialParagraph(*args, **kwargs):
        """SetPartialParagraph(self, bool partialPara)"""
        return _richtext.RichTextParagraphLayoutBox_SetPartialParagraph(*args, **kwargs)

    def GetPartialParagraph(*args, **kwargs):
        """GetPartialParagraph(self) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_GetPartialParagraph(*args, **kwargs)

    def GetStyleSheet(*args, **kwargs):
        """GetStyleSheet(self) -> wxRichTextStyleSheet"""
        return _richtext.RichTextParagraphLayoutBox_GetStyleSheet(*args, **kwargs)

    def Init(*args, **kwargs):
        """Init(self)"""
        return _richtext.RichTextParagraphLayoutBox_Init(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear(self)"""
        return _richtext.RichTextParagraphLayoutBox_Clear(*args, **kwargs)

    def Reset(*args, **kwargs):
        """Reset(self)"""
        return _richtext.RichTextParagraphLayoutBox_Reset(*args, **kwargs)

    def AddParagraph(*args, **kwargs):
        """AddParagraph(self, String text, wxTextAttrEx paraStyle=None) -> RichTextRange"""
        return _richtext.RichTextParagraphLayoutBox_AddParagraph(*args, **kwargs)

    def AddImage(*args, **kwargs):
        """AddImage(self, Image image, wxTextAttrEx paraStyle=None) -> RichTextRange"""
        return _richtext.RichTextParagraphLayoutBox_AddImage(*args, **kwargs)

    def AddParagraphs(*args, **kwargs):
        """AddParagraphs(self, String text, wxTextAttrEx paraStyle=None) -> RichTextRange"""
        return _richtext.RichTextParagraphLayoutBox_AddParagraphs(*args, **kwargs)

    def GetLineAtPosition(*args, **kwargs):
        """GetLineAtPosition(self, long pos, bool caretPosition=False) -> RichTextLine"""
        return _richtext.RichTextParagraphLayoutBox_GetLineAtPosition(*args, **kwargs)

    def GetLineAtYPosition(*args, **kwargs):
        """GetLineAtYPosition(self, int y) -> RichTextLine"""
        return _richtext.RichTextParagraphLayoutBox_GetLineAtYPosition(*args, **kwargs)

    def GetParagraphAtPosition(*args, **kwargs):
        """GetParagraphAtPosition(self, long pos, bool caretPosition=False) -> RichTextParagraph"""
        return _richtext.RichTextParagraphLayoutBox_GetParagraphAtPosition(*args, **kwargs)

    def GetLineSizeAtPosition(*args, **kwargs):
        """GetLineSizeAtPosition(self, long pos, bool caretPosition=False) -> Size"""
        return _richtext.RichTextParagraphLayoutBox_GetLineSizeAtPosition(*args, **kwargs)

    def GetVisibleLineNumber(*args, **kwargs):
        """GetVisibleLineNumber(self, long pos, bool caretPosition=False, bool startOfLine=False) -> long"""
        return _richtext.RichTextParagraphLayoutBox_GetVisibleLineNumber(*args, **kwargs)

    def GetLineForVisibleLineNumber(*args, **kwargs):
        """GetLineForVisibleLineNumber(self, long lineNumber) -> RichTextLine"""
        return _richtext.RichTextParagraphLayoutBox_GetLineForVisibleLineNumber(*args, **kwargs)

    def GetLeafObjectAtPosition(*args, **kwargs):
        """GetLeafObjectAtPosition(self, long position) -> RichTextObject"""
        return _richtext.RichTextParagraphLayoutBox_GetLeafObjectAtPosition(*args, **kwargs)

    def GetParagraphAtLine(*args, **kwargs):
        """GetParagraphAtLine(self, long paragraphNumber) -> RichTextParagraph"""
        return _richtext.RichTextParagraphLayoutBox_GetParagraphAtLine(*args, **kwargs)

    def GetParagraphForLine(*args, **kwargs):
        """GetParagraphForLine(self, RichTextLine line) -> RichTextParagraph"""
        return _richtext.RichTextParagraphLayoutBox_GetParagraphForLine(*args, **kwargs)

    def GetParagraphLength(*args, **kwargs):
        """GetParagraphLength(self, long paragraphNumber) -> int"""
        return _richtext.RichTextParagraphLayoutBox_GetParagraphLength(*args, **kwargs)

    def GetParagraphCount(*args, **kwargs):
        """GetParagraphCount(self) -> int"""
        return _richtext.RichTextParagraphLayoutBox_GetParagraphCount(*args, **kwargs)

    def GetLineCount(*args, **kwargs):
        """GetLineCount(self) -> int"""
        return _richtext.RichTextParagraphLayoutBox_GetLineCount(*args, **kwargs)

    def GetParagraphText(*args, **kwargs):
        """GetParagraphText(self, long paragraphNumber) -> String"""
        return _richtext.RichTextParagraphLayoutBox_GetParagraphText(*args, **kwargs)

    def XYToPosition(*args, **kwargs):
        """XYToPosition(self, long x, long y) -> long"""
        return _richtext.RichTextParagraphLayoutBox_XYToPosition(*args, **kwargs)

    def PositionToXY(*args, **kwargs):
        """PositionToXY(self, long pos, long x, long y) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_PositionToXY(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(self, RichTextRange range, RichTextAttr style, int flags=RICHTEXT_SETSTYLE_WITH_UNDO) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_SetStyle(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self, long position, RichTextAttr style) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_GetStyle(*args, **kwargs)

    def GetUncombinedStyle(*args, **kwargs):
        """GetUncombinedStyle(self, long position, RichTextAttr style) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_GetUncombinedStyle(*args, **kwargs)

    def GetStyleForRange(*args, **kwargs):
        """GetStyleForRange(self, RichTextRange range, wxTextAttrEx style) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_GetStyleForRange(*args, **kwargs)

    def CollectStyle(*args, **kwargs):
        """
        CollectStyle(self, wxTextAttrEx currentStyle, wxTextAttrEx style, long multipleStyleAttributes, 
            int multipleTextEffectAttributes) -> bool
        """
        return _richtext.RichTextParagraphLayoutBox_CollectStyle(*args, **kwargs)

    def SetListStyle(*args, **kwargs):
        """
        SetListStyle(self, RichTextRange range, String defName, int flags=RICHTEXT_SETSTYLE_WITH_UNDO, 
            int startFrom=1, int specifiedLevel=-1) -> bool
        """
        return _richtext.RichTextParagraphLayoutBox_SetListStyle(*args, **kwargs)

    def ClearListStyle(*args, **kwargs):
        """ClearListStyle(self, RichTextRange range, int flags=RICHTEXT_SETSTYLE_WITH_UNDO) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_ClearListStyle(*args, **kwargs)

    def NumberList(*args, **kwargs):
        """
        NumberList(self, RichTextRange range, String defName, int flags=RICHTEXT_SETSTYLE_WITH_UNDO, 
            int startFrom=1, int specifiedLevel=-1) -> bool
        """
        return _richtext.RichTextParagraphLayoutBox_NumberList(*args, **kwargs)

    def PromoteList(*args, **kwargs):
        """
        PromoteList(self, int promoteBy, RichTextRange range, String defName, 
            int flags=RICHTEXT_SETSTYLE_WITH_UNDO, int specifiedLevel=-1) -> bool
        """
        return _richtext.RichTextParagraphLayoutBox_PromoteList(*args, **kwargs)

    def DoNumberList(*args, **kwargs):
        """
        DoNumberList(self, RichTextRange range, RichTextRange promotionRange, 
            int promoteBy, wxRichTextListStyleDefinition def, 
            int flags=RICHTEXT_SETSTYLE_WITH_UNDO, int startFrom=1, 
            int specifiedLevel=-1) -> bool
        """
        return _richtext.RichTextParagraphLayoutBox_DoNumberList(*args, **kwargs)

    def FindNextParagraphNumber(*args, **kwargs):
        """FindNextParagraphNumber(self, RichTextParagraph previousParagraph, RichTextAttr attr) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_FindNextParagraphNumber(*args, **kwargs)

    def HasCharacterAttributes(*args, **kwargs):
        """HasCharacterAttributes(self, RichTextRange range, RichTextAttr style) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_HasCharacterAttributes(*args, **kwargs)

    def HasParagraphAttributes(*args, **kwargs):
        """HasParagraphAttributes(self, RichTextRange range, RichTextAttr style) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_HasParagraphAttributes(*args, **kwargs)

    def InsertFragment(*args, **kwargs):
        """InsertFragment(self, long position, RichTextParagraphLayoutBox fragment) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_InsertFragment(*args, **kwargs)

    def CopyFragment(*args, **kwargs):
        """CopyFragment(self, RichTextRange range, RichTextParagraphLayoutBox fragment) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_CopyFragment(*args, **kwargs)

    def ApplyStyleSheet(*args, **kwargs):
        """ApplyStyleSheet(self, wxRichTextStyleSheet styleSheet) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_ApplyStyleSheet(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self, RichTextParagraphLayoutBox obj)"""
        return _richtext.RichTextParagraphLayoutBox_Copy(*args, **kwargs)

    def UpdateRanges(*args, **kwargs):
        """UpdateRanges(self)"""
        return _richtext.RichTextParagraphLayoutBox_UpdateRanges(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(self) -> String"""
        return _richtext.RichTextParagraphLayoutBox_GetText(*args, **kwargs)

    def SetDefaultStyle(*args, **kwargs):
        """SetDefaultStyle(self, wxTextAttrEx style) -> bool"""
        return _richtext.RichTextParagraphLayoutBox_SetDefaultStyle(*args, **kwargs)

    def GetDefaultStyle(*args, **kwargs):
        """GetDefaultStyle(self) -> wxTextAttrEx"""
        return _richtext.RichTextParagraphLayoutBox_GetDefaultStyle(*args, **kwargs)

    def SetBasicStyle(*args, **kwargs):
        """SetBasicStyle(self, RichTextAttr style)"""
        return _richtext.RichTextParagraphLayoutBox_SetBasicStyle(*args, **kwargs)

    def GetBasicStyle(*args, **kwargs):
        """GetBasicStyle(self) -> wxTextAttrEx"""
        return _richtext.RichTextParagraphLayoutBox_GetBasicStyle(*args, **kwargs)

    def Invalidate(*args, **kwargs):
        """Invalidate(self, RichTextRange invalidRange=wxRICHTEXT_ALL)"""
        return _richtext.RichTextParagraphLayoutBox_Invalidate(*args, **kwargs)

    def GetInvalidRange(*args, **kwargs):
        """GetInvalidRange(self, bool wholeParagraphs=False) -> RichTextRange"""
        return _richtext.RichTextParagraphLayoutBox_GetInvalidRange(*args, **kwargs)

_richtext.RichTextParagraphLayoutBox_swigregister(RichTextParagraphLayoutBox)

class RichTextLine(object):
    """
    This object represents a line in a paragraph, and stores offsets from
    the start of the paragraph representing the start and end positions of
    the line.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, RichTextParagraph parent) -> RichTextLine

        This object represents a line in a paragraph, and stores offsets from
        the start of the paragraph representing the start and end positions of
        the line.
        """
        _richtext.RichTextLine_swiginit(self,_richtext.new_RichTextLine(*args, **kwargs))
    __swig_destroy__ = _richtext.delete_RichTextLine
    __del__ = lambda self : None;
    def SetRange(*args, **kwargs):
        """SetRange(self, RichTextRange range)"""
        return _richtext.RichTextLine_SetRange(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent(self) -> RichTextParagraph"""
        return _richtext.RichTextLine_GetParent(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange(self) -> RichTextRange"""
        return _richtext.RichTextLine_GetRange(*args, **kwargs)

    def GetAbsoluteRange(*args, **kwargs):
        """GetAbsoluteRange(self) -> RichTextRange"""
        return _richtext.RichTextLine_GetAbsoluteRange(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> Size"""
        return _richtext.RichTextLine_GetSize(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, Size sz)"""
        return _richtext.RichTextLine_SetSize(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _richtext.RichTextLine_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, Point pos)"""
        return _richtext.RichTextLine_SetPosition(*args, **kwargs)

    def GetAbsolutePosition(*args, **kwargs):
        """GetAbsolutePosition(self) -> Point"""
        return _richtext.RichTextLine_GetAbsolutePosition(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """GetRect(self) -> Rect"""
        return _richtext.RichTextLine_GetRect(*args, **kwargs)

    def SetDescent(*args, **kwargs):
        """SetDescent(self, int descent)"""
        return _richtext.RichTextLine_SetDescent(*args, **kwargs)

    def GetDescent(*args, **kwargs):
        """GetDescent(self) -> int"""
        return _richtext.RichTextLine_GetDescent(*args, **kwargs)

    def Init(*args, **kwargs):
        """Init(self, RichTextParagraph parent)"""
        return _richtext.RichTextLine_Init(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self, RichTextLine obj)"""
        return _richtext.RichTextLine_Copy(*args, **kwargs)

    def Clone(*args, **kwargs):
        """Clone(self) -> RichTextLine"""
        return _richtext.RichTextLine_Clone(*args, **kwargs)

_richtext.RichTextLine_swigregister(RichTextLine)

class RichTextParagraph(RichTextBox):
    """
    This object represents a single paragraph (or in a straight text
    editor, a line).
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String text=EmptyString, RichTextObject parent=None, 
            RichTextAttr paraStyle=None, RichTextAttr charStyle=None) -> RichTextParagraph

        This object represents a single paragraph (or in a straight text
        editor, a line).
        """
        _richtext.RichTextParagraph_swiginit(self,_richtext.new_RichTextParagraph(*args, **kwargs))
    __swig_destroy__ = _richtext.delete_RichTextParagraph
    __del__ = lambda self : None;
    def GetLines(*args, **kwargs):
        """GetLines(self) -> wxRichTextLineList"""
        return _richtext.RichTextParagraph_GetLines(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self, RichTextParagraph obj)"""
        return _richtext.RichTextParagraph_Copy(*args, **kwargs)

    def ClearLines(*args, **kwargs):
        """ClearLines(self)"""
        return _richtext.RichTextParagraph_ClearLines(*args, **kwargs)

    def ApplyParagraphStyle(*args, **kwargs):
        """ApplyParagraphStyle(self, wxTextAttrEx attr, Rect rect)"""
        return _richtext.RichTextParagraph_ApplyParagraphStyle(*args, **kwargs)

    def InsertText(*args, **kwargs):
        """InsertText(self, long pos, String text) -> bool"""
        return _richtext.RichTextParagraph_InsertText(*args, **kwargs)

    def SplitAt(*args, **kwargs):
        """SplitAt(self, long pos, RichTextObject previousObject=None) -> RichTextObject"""
        return _richtext.RichTextParagraph_SplitAt(*args, **kwargs)

    def MoveToList(*args, **kwargs):
        """MoveToList(self, RichTextObject obj, wxList list)"""
        return _richtext.RichTextParagraph_MoveToList(*args, **kwargs)

    def MoveFromList(*args, **kwargs):
        """MoveFromList(self, wxList list)"""
        return _richtext.RichTextParagraph_MoveFromList(*args, **kwargs)

    def GetContiguousPlainText(*args, **kwargs):
        """GetContiguousPlainText(self, String text, RichTextRange range, bool fromStart=True) -> bool"""
        return _richtext.RichTextParagraph_GetContiguousPlainText(*args, **kwargs)

    def FindWrapPosition(*args, **kwargs):
        """FindWrapPosition(self, RichTextRange range, DC dc, int availableSpace, long wrapPosition) -> bool"""
        return _richtext.RichTextParagraph_FindWrapPosition(*args, **kwargs)

    def FindObjectAtPosition(*args, **kwargs):
        """FindObjectAtPosition(self, long position) -> RichTextObject"""
        return _richtext.RichTextParagraph_FindObjectAtPosition(*args, **kwargs)

    def GetBulletText(*args, **kwargs):
        """GetBulletText(self) -> String"""
        return _richtext.RichTextParagraph_GetBulletText(*args, **kwargs)

    def AllocateLine(*args, **kwargs):
        """AllocateLine(self, int pos) -> RichTextLine"""
        return _richtext.RichTextParagraph_AllocateLine(*args, **kwargs)

    def ClearUnusedLines(*args, **kwargs):
        """ClearUnusedLines(self, int lineCount) -> bool"""
        return _richtext.RichTextParagraph_ClearUnusedLines(*args, **kwargs)

    def GetCombinedAttributes(*args, **kwargs):
        """GetCombinedAttributes(self) -> wxTextAttrEx"""
        return _richtext.RichTextParagraph_GetCombinedAttributes(*args, **kwargs)

    def GetFirstLineBreakPosition(*args, **kwargs):
        """GetFirstLineBreakPosition(self, long pos) -> long"""
        return _richtext.RichTextParagraph_GetFirstLineBreakPosition(*args, **kwargs)

    def InitDefaultTabs(*args, **kwargs):
        """InitDefaultTabs()"""
        return _richtext.RichTextParagraph_InitDefaultTabs(*args, **kwargs)

    InitDefaultTabs = staticmethod(InitDefaultTabs)
    def ClearDefaultTabs(*args, **kwargs):
        """ClearDefaultTabs()"""
        return _richtext.RichTextParagraph_ClearDefaultTabs(*args, **kwargs)

    ClearDefaultTabs = staticmethod(ClearDefaultTabs)
    def GetDefaultTabs(*args, **kwargs):
        """GetDefaultTabs() -> wxArrayInt"""
        return _richtext.RichTextParagraph_GetDefaultTabs(*args, **kwargs)

    GetDefaultTabs = staticmethod(GetDefaultTabs)
_richtext.RichTextParagraph_swigregister(RichTextParagraph)

def RichTextParagraph_InitDefaultTabs(*args):
  """RichTextParagraph_InitDefaultTabs()"""
  return _richtext.RichTextParagraph_InitDefaultTabs(*args)

def RichTextParagraph_ClearDefaultTabs(*args):
  """RichTextParagraph_ClearDefaultTabs()"""
  return _richtext.RichTextParagraph_ClearDefaultTabs(*args)

def RichTextParagraph_GetDefaultTabs(*args):
  """RichTextParagraph_GetDefaultTabs() -> wxArrayInt"""
  return _richtext.RichTextParagraph_GetDefaultTabs(*args)

class RichTextPlainText(RichTextObject):
    """This object represents a single piece of text."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String text=wxEmptyString, RichTextObject parent=None, 
            wxTextAttrEx style=None) -> RichTextPlainText

        This object represents a single piece of text.
        """
        _richtext.RichTextPlainText_swiginit(self,_richtext.new_RichTextPlainText(*args, **kwargs))
    def GetFirstLineBreakPosition(*args, **kwargs):
        """GetFirstLineBreakPosition(self, long pos) -> long"""
        return _richtext.RichTextPlainText_GetFirstLineBreakPosition(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(self) -> String"""
        return _richtext.RichTextPlainText_GetText(*args, **kwargs)

    def SetText(*args, **kwargs):
        """SetText(self, String text)"""
        return _richtext.RichTextPlainText_SetText(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self, RichTextPlainText obj)"""
        return _richtext.RichTextPlainText_Copy(*args, **kwargs)

_richtext.RichTextPlainText_swigregister(RichTextPlainText)

class RichTextImage(RichTextObject):
    """This object represents an image."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, RichTextObject parent=None) -> RichTextImage

        This object represents an image.
        """
        _richtext.RichTextImage_swiginit(self,_richtext.new_RichTextImage(*args, **kwargs))
    def GetImage(*args, **kwargs):
        """GetImage(self) -> Image"""
        return _richtext.RichTextImage_GetImage(*args, **kwargs)

    def SetImage(*args, **kwargs):
        """SetImage(self, Image image)"""
        return _richtext.RichTextImage_SetImage(*args, **kwargs)

    def GetImageBlock(*args, **kwargs):
        """GetImageBlock(self) -> wxRichTextImageBlock"""
        return _richtext.RichTextImage_GetImageBlock(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self, RichTextImage obj)"""
        return _richtext.RichTextImage_Copy(*args, **kwargs)

    def LoadFromBlock(*args, **kwargs):
        """LoadFromBlock(self) -> bool"""
        return _richtext.RichTextImage_LoadFromBlock(*args, **kwargs)

    def MakeBlock(*args, **kwargs):
        """MakeBlock(self) -> bool"""
        return _richtext.RichTextImage_MakeBlock(*args, **kwargs)

_richtext.RichTextImage_swigregister(RichTextImage)

class RichTextFileHandlerList_iterator(object):
    """This class serves as an iterator for a wxRichTextFileHandlerList object."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _richtext.delete_RichTextFileHandlerList_iterator
    __del__ = lambda self : None;
    def next(*args, **kwargs):
        """next(self) -> RichTextFileHandler"""
        return _richtext.RichTextFileHandlerList_iterator_next(*args, **kwargs)

_richtext.RichTextFileHandlerList_iterator_swigregister(RichTextFileHandlerList_iterator)

class RichTextFileHandlerList(object):
    """
    This class wraps a wxList-based class and gives it a Python
    sequence-like interface.  Sequence operations supported are length,
    index access and iteration.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _richtext.delete_RichTextFileHandlerList
    __del__ = lambda self : None;
    def __len__(*args, **kwargs):
        """__len__(self) -> size_t"""
        return _richtext.RichTextFileHandlerList___len__(*args, **kwargs)

    def __getitem__(*args, **kwargs):
        """__getitem__(self, size_t index) -> RichTextFileHandler"""
        return _richtext.RichTextFileHandlerList___getitem__(*args, **kwargs)

    def __contains__(*args, **kwargs):
        """__contains__(self, RichTextFileHandler obj) -> bool"""
        return _richtext.RichTextFileHandlerList___contains__(*args, **kwargs)

    def __iter__(*args, **kwargs):
        """__iter__(self) -> RichTextFileHandlerList_iterator"""
        return _richtext.RichTextFileHandlerList___iter__(*args, **kwargs)

    def __repr__(self):
        return "wxRichTextFileHandlerList: " + repr(list(self))

_richtext.RichTextFileHandlerList_swigregister(RichTextFileHandlerList)

class RichTextBuffer(RichTextParagraphLayoutBox):
    """This is a kind of box, used to represent the whole buffer."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> RichTextBuffer

        This is a kind of box, used to represent the whole buffer.
        """
        _richtext.RichTextBuffer_swiginit(self,_richtext.new_RichTextBuffer(*args, **kwargs))
    __swig_destroy__ = _richtext.delete_RichTextBuffer
    __del__ = lambda self : None;
    def GetCommandProcessor(*args, **kwargs):
        """GetCommandProcessor(self) -> wxCommandProcessor"""
        return _richtext.RichTextBuffer_GetCommandProcessor(*args, **kwargs)

    def SetStyleSheet(*args, **kwargs):
        """SetStyleSheet(self, wxRichTextStyleSheet styleSheet)"""
        return _richtext.RichTextBuffer_SetStyleSheet(*args, **kwargs)

    def SetStyleSheetAndNotify(*args, **kwargs):
        """SetStyleSheetAndNotify(self, wxRichTextStyleSheet sheet) -> bool"""
        return _richtext.RichTextBuffer_SetStyleSheetAndNotify(*args, **kwargs)

    def PushStyleSheet(*args, **kwargs):
        """PushStyleSheet(self, wxRichTextStyleSheet styleSheet) -> bool"""
        return _richtext.RichTextBuffer_PushStyleSheet(*args, **kwargs)

    def PopStyleSheet(*args, **kwargs):
        """PopStyleSheet(self) -> wxRichTextStyleSheet"""
        return _richtext.RichTextBuffer_PopStyleSheet(*args, **kwargs)

    def Init(*args, **kwargs):
        """Init(self)"""
        return _richtext.RichTextBuffer_Init(*args, **kwargs)

    def ResetAndClearCommands(*args, **kwargs):
        """ResetAndClearCommands(self)"""
        return _richtext.RichTextBuffer_ResetAndClearCommands(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(self, String filename, int type=RICHTEXT_TYPE_ANY) -> bool"""
        return _richtext.RichTextBuffer_LoadFile(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """SaveFile(self, String filename, int type=RICHTEXT_TYPE_ANY) -> bool"""
        return _richtext.RichTextBuffer_SaveFile(*args, **kwargs)

    def LoadStream(*args, **kwargs):
        """LoadStream(self, InputStream stream, int type=RICHTEXT_TYPE_ANY) -> bool"""
        return _richtext.RichTextBuffer_LoadStream(*args, **kwargs)

    def SaveStream(*args, **kwargs):
        """SaveStream(self, wxOutputStream stream, int type=RICHTEXT_TYPE_ANY) -> bool"""
        return _richtext.RichTextBuffer_SaveStream(*args, **kwargs)

    def SetHandlerFlags(*args, **kwargs):
        """SetHandlerFlags(self, int flags)"""
        return _richtext.RichTextBuffer_SetHandlerFlags(*args, **kwargs)

    def GetHandlerFlags(*args, **kwargs):
        """GetHandlerFlags(self) -> int"""
        return _richtext.RichTextBuffer_GetHandlerFlags(*args, **kwargs)

    def BeginBatchUndo(*args, **kwargs):
        """BeginBatchUndo(self, String cmdName) -> bool"""
        return _richtext.RichTextBuffer_BeginBatchUndo(*args, **kwargs)

    def EndBatchUndo(*args, **kwargs):
        """EndBatchUndo(self) -> bool"""
        return _richtext.RichTextBuffer_EndBatchUndo(*args, **kwargs)

    def BatchingUndo(*args, **kwargs):
        """BatchingUndo(self) -> bool"""
        return _richtext.RichTextBuffer_BatchingUndo(*args, **kwargs)

    def SubmitAction(*args, **kwargs):
        """SubmitAction(self, RichTextAction action) -> bool"""
        return _richtext.RichTextBuffer_SubmitAction(*args, **kwargs)

    def GetBatchedCommand(*args, **kwargs):
        """GetBatchedCommand(self) -> RichTextCommand"""
        return _richtext.RichTextBuffer_GetBatchedCommand(*args, **kwargs)

    def BeginSuppressUndo(*args, **kwargs):
        """BeginSuppressUndo(self) -> bool"""
        return _richtext.RichTextBuffer_BeginSuppressUndo(*args, **kwargs)

    def EndSuppressUndo(*args, **kwargs):
        """EndSuppressUndo(self) -> bool"""
        return _richtext.RichTextBuffer_EndSuppressUndo(*args, **kwargs)

    def SuppressingUndo(*args, **kwargs):
        """SuppressingUndo(self) -> bool"""
        return _richtext.RichTextBuffer_SuppressingUndo(*args, **kwargs)

    def CopyToClipboard(*args, **kwargs):
        """CopyToClipboard(self, RichTextRange range) -> bool"""
        return _richtext.RichTextBuffer_CopyToClipboard(*args, **kwargs)

    def PasteFromClipboard(*args, **kwargs):
        """PasteFromClipboard(self, long position) -> bool"""
        return _richtext.RichTextBuffer_PasteFromClipboard(*args, **kwargs)

    def CanPasteFromClipboard(*args, **kwargs):
        """CanPasteFromClipboard(self) -> bool"""
        return _richtext.RichTextBuffer_CanPasteFromClipboard(*args, **kwargs)

    def BeginStyle(*args, **kwargs):
        """BeginStyle(self, wxTextAttrEx style) -> bool"""
        return _richtext.RichTextBuffer_BeginStyle(*args, **kwargs)

    def EndStyle(*args, **kwargs):
        """EndStyle(self) -> bool"""
        return _richtext.RichTextBuffer_EndStyle(*args, **kwargs)

    def EndAllStyles(*args, **kwargs):
        """EndAllStyles(self) -> bool"""
        return _richtext.RichTextBuffer_EndAllStyles(*args, **kwargs)

    def ClearStyleStack(*args, **kwargs):
        """ClearStyleStack(self)"""
        return _richtext.RichTextBuffer_ClearStyleStack(*args, **kwargs)

    def GetStyleStackSize(*args, **kwargs):
        """GetStyleStackSize(self) -> size_t"""
        return _richtext.RichTextBuffer_GetStyleStackSize(*args, **kwargs)

    def BeginBold(*args, **kwargs):
        """BeginBold(self) -> bool"""
        return _richtext.RichTextBuffer_BeginBold(*args, **kwargs)

    def EndBold(*args, **kwargs):
        """EndBold(self) -> bool"""
        return _richtext.RichTextBuffer_EndBold(*args, **kwargs)

    def BeginItalic(*args, **kwargs):
        """BeginItalic(self) -> bool"""
        return _richtext.RichTextBuffer_BeginItalic(*args, **kwargs)

    def EndItalic(*args, **kwargs):
        """EndItalic(self) -> bool"""
        return _richtext.RichTextBuffer_EndItalic(*args, **kwargs)

    def BeginUnderline(*args, **kwargs):
        """BeginUnderline(self) -> bool"""
        return _richtext.RichTextBuffer_BeginUnderline(*args, **kwargs)

    def EndUnderline(*args, **kwargs):
        """EndUnderline(self) -> bool"""
        return _richtext.RichTextBuffer_EndUnderline(*args, **kwargs)

    def BeginFontSize(*args, **kwargs):
        """BeginFontSize(self, int pointSize) -> bool"""
        return _richtext.RichTextBuffer_BeginFontSize(*args, **kwargs)

    def EndFontSize(*args, **kwargs):
        """EndFontSize(self) -> bool"""
        return _richtext.RichTextBuffer_EndFontSize(*args, **kwargs)

    def BeginFont(*args, **kwargs):
        """BeginFont(self, Font font) -> bool"""
        return _richtext.RichTextBuffer_BeginFont(*args, **kwargs)

    def EndFont(*args, **kwargs):
        """EndFont(self) -> bool"""
        return _richtext.RichTextBuffer_EndFont(*args, **kwargs)

    def BeginTextColour(*args, **kwargs):
        """BeginTextColour(self, Colour colour) -> bool"""
        return _richtext.RichTextBuffer_BeginTextColour(*args, **kwargs)

    def EndTextColour(*args, **kwargs):
        """EndTextColour(self) -> bool"""
        return _richtext.RichTextBuffer_EndTextColour(*args, **kwargs)

    def BeginAlignment(*args, **kwargs):
        """BeginAlignment(self, int alignment) -> bool"""
        return _richtext.RichTextBuffer_BeginAlignment(*args, **kwargs)

    def EndAlignment(*args, **kwargs):
        """EndAlignment(self) -> bool"""
        return _richtext.RichTextBuffer_EndAlignment(*args, **kwargs)

    def BeginLeftIndent(*args, **kwargs):
        """BeginLeftIndent(self, int leftIndent, int leftSubIndent=0) -> bool"""
        return _richtext.RichTextBuffer_BeginLeftIndent(*args, **kwargs)

    def EndLeftIndent(*args, **kwargs):
        """EndLeftIndent(self) -> bool"""
        return _richtext.RichTextBuffer_EndLeftIndent(*args, **kwargs)

    def BeginRightIndent(*args, **kwargs):
        """BeginRightIndent(self, int rightIndent) -> bool"""
        return _richtext.RichTextBuffer_BeginRightIndent(*args, **kwargs)

    def EndRightIndent(*args, **kwargs):
        """EndRightIndent(self) -> bool"""
        return _richtext.RichTextBuffer_EndRightIndent(*args, **kwargs)

    def BeginParagraphSpacing(*args, **kwargs):
        """BeginParagraphSpacing(self, int before, int after) -> bool"""
        return _richtext.RichTextBuffer_BeginParagraphSpacing(*args, **kwargs)

    def EndParagraphSpacing(*args, **kwargs):
        """EndParagraphSpacing(self) -> bool"""
        return _richtext.RichTextBuffer_EndParagraphSpacing(*args, **kwargs)

    def BeginLineSpacing(*args, **kwargs):
        """BeginLineSpacing(self, int lineSpacing) -> bool"""
        return _richtext.RichTextBuffer_BeginLineSpacing(*args, **kwargs)

    def EndLineSpacing(*args, **kwargs):
        """EndLineSpacing(self) -> bool"""
        return _richtext.RichTextBuffer_EndLineSpacing(*args, **kwargs)

    def BeginNumberedBullet(*args, **kwargs):
        """
        BeginNumberedBullet(self, int bulletNumber, int leftIndent, int leftSubIndent, 
            int bulletStyle=wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD) -> bool
        """
        return _richtext.RichTextBuffer_BeginNumberedBullet(*args, **kwargs)

    def EndNumberedBullet(*args, **kwargs):
        """EndNumberedBullet(self) -> bool"""
        return _richtext.RichTextBuffer_EndNumberedBullet(*args, **kwargs)

    def BeginSymbolBullet(*args, **kwargs):
        """BeginSymbolBullet(self, String symbol, int leftIndent, int leftSubIndent, int bulletStyle=TEXT_ATTR_BULLET_STYLE_SYMBOL) -> bool"""
        return _richtext.RichTextBuffer_BeginSymbolBullet(*args, **kwargs)

    def EndSymbolBullet(*args, **kwargs):
        """EndSymbolBullet(self) -> bool"""
        return _richtext.RichTextBuffer_EndSymbolBullet(*args, **kwargs)

    def BeginStandardBullet(*args, **kwargs):
        """
        BeginStandardBullet(self, String bulletName, int leftIndent, int leftSubIndent, 
            int bulletStyle=TEXT_ATTR_BULLET_STYLE_STANDARD) -> bool
        """
        return _richtext.RichTextBuffer_BeginStandardBullet(*args, **kwargs)

    def EndStandardBullet(*args, **kwargs):
        """EndStandardBullet(self) -> bool"""
        return _richtext.RichTextBuffer_EndStandardBullet(*args, **kwargs)

    def BeginCharacterStyle(*args, **kwargs):
        """BeginCharacterStyle(self, String characterStyle) -> bool"""
        return _richtext.RichTextBuffer_BeginCharacterStyle(*args, **kwargs)

    def EndCharacterStyle(*args, **kwargs):
        """EndCharacterStyle(self) -> bool"""
        return _richtext.RichTextBuffer_EndCharacterStyle(*args, **kwargs)

    def BeginParagraphStyle(*args, **kwargs):
        """BeginParagraphStyle(self, String paragraphStyle) -> bool"""
        return _richtext.RichTextBuffer_BeginParagraphStyle(*args, **kwargs)

    def EndParagraphStyle(*args, **kwargs):
        """EndParagraphStyle(self) -> bool"""
        return _richtext.RichTextBuffer_EndParagraphStyle(*args, **kwargs)

    def BeginListStyle(*args, **kwargs):
        """BeginListStyle(self, String listStyle, int level=1, int number=1) -> bool"""
        return _richtext.RichTextBuffer_BeginListStyle(*args, **kwargs)

    def EndListStyle(*args, **kwargs):
        """EndListStyle(self) -> bool"""
        return _richtext.RichTextBuffer_EndListStyle(*args, **kwargs)

    def BeginURL(*args, **kwargs):
        """BeginURL(self, String url, String characterStyle=wxEmptyString) -> bool"""
        return _richtext.RichTextBuffer_BeginURL(*args, **kwargs)

    def EndURL(*args, **kwargs):
        """EndURL(self) -> bool"""
        return _richtext.RichTextBuffer_EndURL(*args, **kwargs)

    def AddEventHandler(*args, **kwargs):
        """AddEventHandler(self, EvtHandler handler) -> bool"""
        return _richtext.RichTextBuffer_AddEventHandler(*args, **kwargs)

    def RemoveEventHandler(*args, **kwargs):
        """RemoveEventHandler(self, EvtHandler handler, bool deleteHandler=False) -> bool"""
        return _richtext.RichTextBuffer_RemoveEventHandler(*args, **kwargs)

    def ClearEventHandlers(*args, **kwargs):
        """ClearEventHandlers(self)"""
        return _richtext.RichTextBuffer_ClearEventHandlers(*args, **kwargs)

    def SendEvent(*args, **kwargs):
        """SendEvent(self, Event event, bool sendToAll=True) -> bool"""
        return _richtext.RichTextBuffer_SendEvent(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self, RichTextBuffer obj)"""
        return _richtext.RichTextBuffer_Copy(*args, **kwargs)

    def InsertParagraphsWithUndo(*args, **kwargs):
        """
        InsertParagraphsWithUndo(self, long pos, RichTextParagraphLayoutBox paragraphs, RichTextCtrl ctrl, 
            int flags=0) -> bool
        """
        return _richtext.RichTextBuffer_InsertParagraphsWithUndo(*args, **kwargs)

    def InsertTextWithUndo(*args, **kwargs):
        """InsertTextWithUndo(self, long pos, String text, RichTextCtrl ctrl, int flags=0) -> bool"""
        return _richtext.RichTextBuffer_InsertTextWithUndo(*args, **kwargs)

    def InsertNewlineWithUndo(*args, **kwargs):
        """InsertNewlineWithUndo(self, long pos, RichTextCtrl ctrl, int flags=0) -> bool"""
        return _richtext.RichTextBuffer_InsertNewlineWithUndo(*args, **kwargs)

    def InsertImageWithUndo(*args, **kwargs):
        """
        InsertImageWithUndo(self, long pos, wxRichTextImageBlock imageBlock, RichTextCtrl ctrl, 
            int flags=0) -> bool
        """
        return _richtext.RichTextBuffer_InsertImageWithUndo(*args, **kwargs)

    def DeleteRangeWithUndo(*args, **kwargs):
        """DeleteRangeWithUndo(self, RichTextRange range, RichTextCtrl ctrl) -> bool"""
        return _richtext.RichTextBuffer_DeleteRangeWithUndo(*args, **kwargs)

    def Modify(*args, **kwargs):
        """Modify(self, bool modify=True)"""
        return _richtext.RichTextBuffer_Modify(*args, **kwargs)

    def IsModified(*args, **kwargs):
        """IsModified(self) -> bool"""
        return _richtext.RichTextBuffer_IsModified(*args, **kwargs)

    def GetStyleForNewParagraph(*args, **kwargs):
        """GetStyleForNewParagraph(self, long pos, bool caretPosition=False, bool lookUpNewParaStyle=False) -> RichTextAttr"""
        return _richtext.RichTextBuffer_GetStyleForNewParagraph(*args, **kwargs)

    def GetHandlers(*args, **kwargs):
        """GetHandlers() -> wxRichTextFileHandlerList_t"""
        return _richtext.RichTextBuffer_GetHandlers(*args, **kwargs)

    GetHandlers = staticmethod(GetHandlers)
    def AddHandler(*args, **kwargs):
        """AddHandler(RichTextFileHandler handler)"""
        return _richtext.RichTextBuffer_AddHandler(*args, **kwargs)

    AddHandler = staticmethod(AddHandler)
    def InsertHandler(*args, **kwargs):
        """InsertHandler(RichTextFileHandler handler)"""
        return _richtext.RichTextBuffer_InsertHandler(*args, **kwargs)

    InsertHandler = staticmethod(InsertHandler)
    def RemoveHandler(*args, **kwargs):
        """RemoveHandler(String name) -> bool"""
        return _richtext.RichTextBuffer_RemoveHandler(*args, **kwargs)

    RemoveHandler = staticmethod(RemoveHandler)
    def FindHandlerByName(*args, **kwargs):
        """FindHandlerByName(String name) -> RichTextFileHandler"""
        return _richtext.RichTextBuffer_FindHandlerByName(*args, **kwargs)

    FindHandlerByName = staticmethod(FindHandlerByName)
    def FindHandlerByExtension(*args, **kwargs):
        """FindHandlerByExtension(String extension, int imageType) -> RichTextFileHandler"""
        return _richtext.RichTextBuffer_FindHandlerByExtension(*args, **kwargs)

    FindHandlerByExtension = staticmethod(FindHandlerByExtension)
    def FindHandlerByFilename(*args, **kwargs):
        """FindHandlerByFilename(String filename, int imageType) -> RichTextFileHandler"""
        return _richtext.RichTextBuffer_FindHandlerByFilename(*args, **kwargs)

    FindHandlerByFilename = staticmethod(FindHandlerByFilename)
    def FindHandlerByType(*args, **kwargs):
        """FindHandlerByType(int imageType) -> RichTextFileHandler"""
        return _richtext.RichTextBuffer_FindHandlerByType(*args, **kwargs)

    FindHandlerByType = staticmethod(FindHandlerByType)
    def GetExtWildcard(*args, **kwargs):
        """
        GetExtWildcard(self, bool combine=False, bool save=False) --> (wildcards, types)

        Gets a wildcard string for the file dialog based on all the currently
        loaded richtext file handlers, and a list that can be used to map
        those filter types to the file handler type.
        """
        return _richtext.RichTextBuffer_GetExtWildcard(*args, **kwargs)

    GetExtWildcard = staticmethod(GetExtWildcard)
    def CleanUpHandlers(*args, **kwargs):
        """CleanUpHandlers()"""
        return _richtext.RichTextBuffer_CleanUpHandlers(*args, **kwargs)

    CleanUpHandlers = staticmethod(CleanUpHandlers)
    def InitStandardHandlers(*args, **kwargs):
        """InitStandardHandlers()"""
        return _richtext.RichTextBuffer_InitStandardHandlers(*args, **kwargs)

    InitStandardHandlers = staticmethod(InitStandardHandlers)
    def GetRenderer(*args, **kwargs):
        """GetRenderer() -> RichTextRenderer"""
        return _richtext.RichTextBuffer_GetRenderer(*args, **kwargs)

    GetRenderer = staticmethod(GetRenderer)
    def SetRenderer(*args, **kwargs):
        """SetRenderer(RichTextRenderer renderer)"""
        return _richtext.RichTextBuffer_SetRenderer(*args, **kwargs)

    SetRenderer = staticmethod(SetRenderer)
    def GetBulletRightMargin(*args, **kwargs):
        """GetBulletRightMargin() -> int"""
        return _richtext.RichTextBuffer_GetBulletRightMargin(*args, **kwargs)

    GetBulletRightMargin = staticmethod(GetBulletRightMargin)
    def SetBulletRightMargin(*args, **kwargs):
        """SetBulletRightMargin(int margin)"""
        return _richtext.RichTextBuffer_SetBulletRightMargin(*args, **kwargs)

    SetBulletRightMargin = staticmethod(SetBulletRightMargin)
    def GetBulletProportion(*args, **kwargs):
        """GetBulletProportion() -> float"""
        return _richtext.RichTextBuffer_GetBulletProportion(*args, **kwargs)

    GetBulletProportion = staticmethod(GetBulletProportion)
    def SetBulletProportion(*args, **kwargs):
        """SetBulletProportion(float prop)"""
        return _richtext.RichTextBuffer_SetBulletProportion(*args, **kwargs)

    SetBulletProportion = staticmethod(SetBulletProportion)
    def GetScale(*args, **kwargs):
        """GetScale(self) -> double"""
        return _richtext.RichTextBuffer_GetScale(*args, **kwargs)

    def SetScale(*args, **kwargs):
        """SetScale(self, double scale)"""
        return _richtext.RichTextBuffer_SetScale(*args, **kwargs)

_richtext.RichTextBuffer_swigregister(RichTextBuffer)

def RichTextBuffer_GetHandlers(*args):
  """RichTextBuffer_GetHandlers() -> wxRichTextFileHandlerList_t"""
  return _richtext.RichTextBuffer_GetHandlers(*args)

def RichTextBuffer_AddHandler(*args, **kwargs):
  """RichTextBuffer_AddHandler(RichTextFileHandler handler)"""
  return _richtext.RichTextBuffer_AddHandler(*args, **kwargs)

def RichTextBuffer_InsertHandler(*args, **kwargs):
  """RichTextBuffer_InsertHandler(RichTextFileHandler handler)"""
  return _richtext.RichTextBuffer_InsertHandler(*args, **kwargs)

def RichTextBuffer_RemoveHandler(*args, **kwargs):
  """RichTextBuffer_RemoveHandler(String name) -> bool"""
  return _richtext.RichTextBuffer_RemoveHandler(*args, **kwargs)

def RichTextBuffer_FindHandlerByName(*args, **kwargs):
  """RichTextBuffer_FindHandlerByName(String name) -> RichTextFileHandler"""
  return _richtext.RichTextBuffer_FindHandlerByName(*args, **kwargs)

def RichTextBuffer_FindHandlerByExtension(*args, **kwargs):
  """RichTextBuffer_FindHandlerByExtension(String extension, int imageType) -> RichTextFileHandler"""
  return _richtext.RichTextBuffer_FindHandlerByExtension(*args, **kwargs)

def RichTextBuffer_FindHandlerByFilename(*args, **kwargs):
  """RichTextBuffer_FindHandlerByFilename(String filename, int imageType) -> RichTextFileHandler"""
  return _richtext.RichTextBuffer_FindHandlerByFilename(*args, **kwargs)

def RichTextBuffer_FindHandlerByType(*args, **kwargs):
  """RichTextBuffer_FindHandlerByType(int imageType) -> RichTextFileHandler"""
  return _richtext.RichTextBuffer_FindHandlerByType(*args, **kwargs)

def RichTextBuffer_GetExtWildcard(*args, **kwargs):
  """
    GetExtWildcard(self, bool combine=False, bool save=False) --> (wildcards, types)

    Gets a wildcard string for the file dialog based on all the currently
    loaded richtext file handlers, and a list that can be used to map
    those filter types to the file handler type.
    """
  return _richtext.RichTextBuffer_GetExtWildcard(*args, **kwargs)

def RichTextBuffer_CleanUpHandlers(*args):
  """RichTextBuffer_CleanUpHandlers()"""
  return _richtext.RichTextBuffer_CleanUpHandlers(*args)

def RichTextBuffer_InitStandardHandlers(*args):
  """RichTextBuffer_InitStandardHandlers()"""
  return _richtext.RichTextBuffer_InitStandardHandlers(*args)

def RichTextBuffer_GetRenderer(*args):
  """RichTextBuffer_GetRenderer() -> RichTextRenderer"""
  return _richtext.RichTextBuffer_GetRenderer(*args)

def RichTextBuffer_SetRenderer(*args, **kwargs):
  """RichTextBuffer_SetRenderer(RichTextRenderer renderer)"""
  return _richtext.RichTextBuffer_SetRenderer(*args, **kwargs)

def RichTextBuffer_GetBulletRightMargin(*args):
  """RichTextBuffer_GetBulletRightMargin() -> int"""
  return _richtext.RichTextBuffer_GetBulletRightMargin(*args)

def RichTextBuffer_SetBulletRightMargin(*args, **kwargs):
  """RichTextBuffer_SetBulletRightMargin(int margin)"""
  return _richtext.RichTextBuffer_SetBulletRightMargin(*args, **kwargs)

def RichTextBuffer_GetBulletProportion(*args):
  """RichTextBuffer_GetBulletProportion() -> float"""
  return _richtext.RichTextBuffer_GetBulletProportion(*args)

def RichTextBuffer_SetBulletProportion(*args, **kwargs):
  """RichTextBuffer_SetBulletProportion(float prop)"""
  return _richtext.RichTextBuffer_SetBulletProportion(*args, **kwargs)

#---------------------------------------------------------------------------

RICHTEXT_HANDLER_INCLUDE_STYLESHEET = _richtext.RICHTEXT_HANDLER_INCLUDE_STYLESHEET
RICHTEXT_HANDLER_SAVE_IMAGES_TO_MEMORY = _richtext.RICHTEXT_HANDLER_SAVE_IMAGES_TO_MEMORY
RICHTEXT_HANDLER_SAVE_IMAGES_TO_FILES = _richtext.RICHTEXT_HANDLER_SAVE_IMAGES_TO_FILES
RICHTEXT_HANDLER_SAVE_IMAGES_TO_BASE64 = _richtext.RICHTEXT_HANDLER_SAVE_IMAGES_TO_BASE64
RICHTEXT_HANDLER_NO_HEADER_FOOTER = _richtext.RICHTEXT_HANDLER_NO_HEADER_FOOTER
class RichTextFileHandler(_core.Object):
    """Base class for file handlers"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _richtext.delete_RichTextFileHandler
    __del__ = lambda self : None;
    def LoadStream(*args, **kwargs):
        """LoadStream(self, RichTextBuffer buffer, InputStream stream) -> bool"""
        return _richtext.RichTextFileHandler_LoadStream(*args, **kwargs)

    def SaveStream(*args, **kwargs):
        """SaveStream(self, RichTextBuffer buffer, wxOutputStream stream) -> bool"""
        return _richtext.RichTextFileHandler_SaveStream(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(self, RichTextBuffer buffer, String filename) -> bool"""
        return _richtext.RichTextFileHandler_LoadFile(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """SaveFile(self, RichTextBuffer buffer, String filename) -> bool"""
        return _richtext.RichTextFileHandler_SaveFile(*args, **kwargs)

    def CanHandle(*args, **kwargs):
        """CanHandle(self, String filename) -> bool"""
        return _richtext.RichTextFileHandler_CanHandle(*args, **kwargs)

    def CanSave(*args, **kwargs):
        """CanSave(self) -> bool"""
        return _richtext.RichTextFileHandler_CanSave(*args, **kwargs)

    def CanLoad(*args, **kwargs):
        """CanLoad(self) -> bool"""
        return _richtext.RichTextFileHandler_CanLoad(*args, **kwargs)

    def IsVisible(*args, **kwargs):
        """IsVisible(self) -> bool"""
        return _richtext.RichTextFileHandler_IsVisible(*args, **kwargs)

    def SetVisible(*args, **kwargs):
        """SetVisible(self, bool visible)"""
        return _richtext.RichTextFileHandler_SetVisible(*args, **kwargs)

    def SetName(*args, **kwargs):
        """SetName(self, String name)"""
        return _richtext.RichTextFileHandler_SetName(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _richtext.RichTextFileHandler_GetName(*args, **kwargs)

    Name = property(GetName,SetName) 
    def SetExtension(*args, **kwargs):
        """SetExtension(self, String ext)"""
        return _richtext.RichTextFileHandler_SetExtension(*args, **kwargs)

    def GetExtension(*args, **kwargs):
        """GetExtension(self) -> String"""
        return _richtext.RichTextFileHandler_GetExtension(*args, **kwargs)

    Extension = property(GetExtension,SetExtension) 
    def SetType(*args, **kwargs):
        """SetType(self, int type)"""
        return _richtext.RichTextFileHandler_SetType(*args, **kwargs)

    def GetType(*args, **kwargs):
        """GetType(self) -> int"""
        return _richtext.RichTextFileHandler_GetType(*args, **kwargs)

    Type = property(GetType,SetType) 
    def SetFlags(*args, **kwargs):
        """SetFlags(self, int flags)"""
        return _richtext.RichTextFileHandler_SetFlags(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags(self) -> int"""
        return _richtext.RichTextFileHandler_GetFlags(*args, **kwargs)

    Flags = property(GetFlags,SetFlags) 
    def SetEncoding(*args, **kwargs):
        """SetEncoding(self, String encoding)"""
        return _richtext.RichTextFileHandler_SetEncoding(*args, **kwargs)

    def GetEncoding(*args, **kwargs):
        """GetEncoding(self) -> String"""
        return _richtext.RichTextFileHandler_GetEncoding(*args, **kwargs)

    Encoding = property(GetEncoding,SetEncoding) 
_richtext.RichTextFileHandler_swigregister(RichTextFileHandler)

class RichTextPlainTextHandler(RichTextFileHandler):
    """Proxy of C++ RichTextPlainTextHandler class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String name=TextName, String ext=TextExt, int type=RICHTEXT_TYPE_TEXT) -> RichTextPlainTextHandler"""
        _richtext.RichTextPlainTextHandler_swiginit(self,_richtext.new_RichTextPlainTextHandler(*args, **kwargs))
_richtext.RichTextPlainTextHandler_swigregister(RichTextPlainTextHandler)
TextName = cvar.TextName
TextExt = cvar.TextExt

#---------------------------------------------------------------------------

class RichTextRenderer(_core.Object):
    """Proxy of C++ RichTextRenderer class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _richtext.delete_RichTextRenderer
    __del__ = lambda self : None;
    def DrawStandardBullet(*args, **kwargs):
        """
        DrawStandardBullet(self, RichTextParagraph paragraph, DC dc, wxTextAttrEx attr, 
            Rect rect) -> bool
        """
        return _richtext.RichTextRenderer_DrawStandardBullet(*args, **kwargs)

    def DrawTextBullet(*args, **kwargs):
        """
        DrawTextBullet(self, RichTextParagraph paragraph, DC dc, wxTextAttrEx attr, 
            Rect rect, String text) -> bool
        """
        return _richtext.RichTextRenderer_DrawTextBullet(*args, **kwargs)

    def DrawBitmapBullet(*args, **kwargs):
        """
        DrawBitmapBullet(self, RichTextParagraph paragraph, DC dc, wxTextAttrEx attr, 
            Rect rect) -> bool
        """
        return _richtext.RichTextRenderer_DrawBitmapBullet(*args, **kwargs)

    def EnumerateStandardBulletNames(*args, **kwargs):
        """EnumerateStandardBulletNames(self, wxArrayString bulletNames) -> bool"""
        return _richtext.RichTextRenderer_EnumerateStandardBulletNames(*args, **kwargs)

_richtext.RichTextRenderer_swigregister(RichTextRenderer)

class RichTextStdRenderer(RichTextRenderer):
    """Proxy of C++ RichTextStdRenderer class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> RichTextStdRenderer"""
        _richtext.RichTextStdRenderer_swiginit(self,_richtext.new_RichTextStdRenderer(*args, **kwargs))
_richtext.RichTextStdRenderer_swigregister(RichTextStdRenderer)

#---------------------------------------------------------------------------

RE_READONLY = _richtext.RE_READONLY
RE_MULTILINE = _richtext.RE_MULTILINE
RICHTEXT_SHIFT_DOWN = _richtext.RICHTEXT_SHIFT_DOWN
RICHTEXT_CTRL_DOWN = _richtext.RICHTEXT_CTRL_DOWN
RICHTEXT_ALT_DOWN = _richtext.RICHTEXT_ALT_DOWN
RICHTEXT_SELECTED = _richtext.RICHTEXT_SELECTED
RICHTEXT_TAGGED = _richtext.RICHTEXT_TAGGED
RICHTEXT_FOCUSSED = _richtext.RICHTEXT_FOCUSSED
RICHTEXT_IS_FOCUS = _richtext.RICHTEXT_IS_FOCUS
class RichTextCtrl(_windows.ScrolledWindow):
    """Proxy of C++ RichTextCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=RE_MULTILINE, Validator validator=DefaultValidator, 
            String name=RichTextCtrlNameStr) -> RichTextCtrl
        """
        _richtext.RichTextCtrl_swiginit(self,_richtext.new_RichTextCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=RE_MULTILINE, Validator validator=DefaultValidator, 
            String name=RichTextCtrlNameStr) -> bool
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

    def GetStyle(*args, **kwargs):
        """
        GetStyle(self, long position, RichTextAttr style) -> bool

        Retrieve the style used at the given position.  Copies the style
        values at ``position`` into the ``style`` parameter and returns ``True``
        if successful.  Returns ``False`` otherwise.
        """
        return _richtext.RichTextCtrl_GetStyle(*args, **kwargs)

    def GetStyleForRange(*args, **kwargs):
        """
        GetStyleForRange(self, RichTextRange range, RichTextAttr style) -> bool

        Get the common set of styles for the range
        """
        return _richtext.RichTextCtrl_GetStyleForRange(*args, **kwargs)

    def SetStyleEx(*args, **kwargs):
        """
        SetStyleEx(self, RichTextRange range, RichTextAttr style, int flags=RICHTEXT_SETSTYLE_WITH_UNDO) -> bool

        Extended style setting operation with flags including:
        RICHTEXT_SETSTYLE_WITH_UNDO, RICHTEXT_SETSTYLE_OPTIMIZE,
        RICHTEXT_SETSTYLE_PARAGRAPHS_ONLY, RICHTEXT_SETSTYLE_CHARACTERS_ONLY
        """
        return _richtext.RichTextCtrl_SetStyleEx(*args, **kwargs)

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

    def SetListStyle(*args, **kwargs):
        """
        SetListStyle(self, RichTextRange range, String defName, int flags=RICHTEXT_SETSTYLE_WITH_UNDO, 
            int startFrom=1, int specifiedLevel=-1) -> bool
        """
        return _richtext.RichTextCtrl_SetListStyle(*args, **kwargs)

    def ClearListStyle(*args, **kwargs):
        """ClearListStyle(self, RichTextRange range, int flags=RICHTEXT_SETSTYLE_WITH_UNDO) -> bool"""
        return _richtext.RichTextCtrl_ClearListStyle(*args, **kwargs)

    def NumberList(*args, **kwargs):
        """
        NumberList(self, RichTextRange range, String defName, int flags=RICHTEXT_SETSTYLE_WITH_UNDO, 
            int startFrom=1, int specifiedLevel=-1) -> bool
        """
        return _richtext.RichTextCtrl_NumberList(*args, **kwargs)

    def PromoteList(*args, **kwargs):
        """
        PromoteList(self, int promoteBy, RichTextRange range, String defName, 
            int flags=RICHTEXT_SETSTYLE_WITH_UNDO, int specifiedLevel=-1) -> bool
        """
        return _richtext.RichTextCtrl_PromoteList(*args, **kwargs)

    def Delete(*args, **kwargs):
        """Delete(self, RichTextRange range) -> bool"""
        return _richtext.RichTextCtrl_Delete(*args, **kwargs)

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

    def LineBreak(*args, **kwargs):
        """LineBreak(self) -> bool"""
        return _richtext.RichTextCtrl_LineBreak(*args, **kwargs)

    def SetBasicStyle(*args, **kwargs):
        """SetBasicStyle(self, RichTextAttr style)"""
        return _richtext.RichTextCtrl_SetBasicStyle(*args, **kwargs)

    def GetBasicStyle(*args, **kwargs):
        """GetBasicStyle(self) -> RichTextAttr"""
        return _richtext.RichTextCtrl_GetBasicStyle(*args, **kwargs)

    def BeginStyle(*args, **kwargs):
        """BeginStyle(self, RichTextAttr style) -> bool"""
        return _richtext.RichTextCtrl_BeginStyle(*args, **kwargs)

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

    def BeginStandardBullet(*args, **kwargs):
        """
        BeginStandardBullet(self, String bulletName, int leftIndent, int leftSubIndent, 
            int bulletStyle=TEXT_ATTR_BULLET_STYLE_STANDARD) -> bool
        """
        return _richtext.RichTextCtrl_BeginStandardBullet(*args, **kwargs)

    def EndStandardBullet(*args, **kwargs):
        """EndStandardBullet(self) -> bool"""
        return _richtext.RichTextCtrl_EndStandardBullet(*args, **kwargs)

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
        """GetBuffer(self) -> RichTextBuffer"""
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

    def ApplyStyle(*args, **kwargs):
        """ApplyStyle(self, wxRichTextStyleDefinition def) -> bool"""
        return _richtext.RichTextCtrl_ApplyStyle(*args, **kwargs)

    def SetStyleSheet(*args, **kwargs):
        """SetStyleSheet(self, wxRichTextStyleSheet styleSheet)"""
        return _richtext.RichTextCtrl_SetStyleSheet(*args, **kwargs)

    def GetStyleSheet(*args, **kwargs):
        """GetStyleSheet(self) -> wxRichTextStyleSheet"""
        return _richtext.RichTextCtrl_GetStyleSheet(*args, **kwargs)

    def PushStyleSheet(*args, **kwargs):
        """PushStyleSheet(self, wxRichTextStyleSheet styleSheet) -> bool"""
        return _richtext.RichTextCtrl_PushStyleSheet(*args, **kwargs)

    def PopStyleSheet(*args, **kwargs):
        """PopStyleSheet(self) -> wxRichTextStyleSheet"""
        return _richtext.RichTextCtrl_PopStyleSheet(*args, **kwargs)

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
    Selection = property(GetSelection,SetSelectionRange,doc="See `GetSelection` and `SetSelection`") 
    SelectionRange = property(GetSelectionRange,SetSelectionRange,doc="See `GetSelectionRange` and `SetSelectionRange`") 
    StringSelection = property(GetStringSelection,doc="See `GetStringSelection`") 
    StyleSheet = property(GetStyleSheet,SetStyleSheet,doc="See `GetStyleSheet` and `SetStyleSheet`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_richtext.RichTextCtrl_swigregister(RichTextCtrl)
RichTextCtrlNameStr = cvar.RichTextCtrlNameStr

def PreRichTextCtrl(*args, **kwargs):
    """PreRichTextCtrl() -> RichTextCtrl"""
    val = _richtext.new_PreRichTextCtrl(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

wxEVT_COMMAND_RICHTEXT_LEFT_CLICK = _richtext.wxEVT_COMMAND_RICHTEXT_LEFT_CLICK
wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK = _richtext.wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK
wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK = _richtext.wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK
wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK = _richtext.wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK
wxEVT_COMMAND_RICHTEXT_RETURN = _richtext.wxEVT_COMMAND_RICHTEXT_RETURN
wxEVT_COMMAND_RICHTEXT_CHARACTER = _richtext.wxEVT_COMMAND_RICHTEXT_CHARACTER
wxEVT_COMMAND_RICHTEXT_DELETE = _richtext.wxEVT_COMMAND_RICHTEXT_DELETE
wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING = _richtext.wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING
wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED = _richtext.wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED
wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING = _richtext.wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING
wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED = _richtext.wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED
wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED = _richtext.wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED
wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED = _richtext.wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED
wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED = _richtext.wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED
wxEVT_COMMAND_RICHTEXT_SELECTION_CHANGED = _richtext.wxEVT_COMMAND_RICHTEXT_SELECTION_CHANGED
EVT_RICHTEXT_LEFT_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_LEFT_CLICK, 1)
EVT_RICHTEXT_RIGHT_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_RIGHT_CLICK, 1)
EVT_RICHTEXT_MIDDLE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_MIDDLE_CLICK, 1)
EVT_RICHTEXT_LEFT_DCLICK = wx.PyEventBinder(wxEVT_COMMAND_RICHTEXT_LEFT_DCLICK, 1)
EVT_RICHTEXT_RETURN = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_RETURN, 1)
EVT_RICHTEXT_CHARACTER = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_CHARACTER, 1)
EVT_RICHTEXT_DELETE = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_DELETE, 1)

EVT_RICHTEXT_STYLESHEET_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING, 1)
EVT_RICHTEXT_STYLESHEET_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGED, 1)
EVT_RICHTEXT_STYLESHEET_REPLACING = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING, 1)
EVT_RICHTEXT_STYLESHEET_REPLACED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED, 1)

EVT_RICHTEXT_CONTENT_INSERTED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED, 1)
EVT_RICHTEXT_CONTENT_DELETED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED, 1)
EVT_RICHTEXT_STYLE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED, 1)
EVT_RICHTEXT_SELECTION_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_RICHTEXT_SELECTION_CHANGED, 1)    

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

    def GetOldStyleSheet(*args, **kwargs):
        """GetOldStyleSheet(self) -> wxRichTextStyleSheet"""
        return _richtext.RichTextEvent_GetOldStyleSheet(*args, **kwargs)

    def SetOldStyleSheet(*args, **kwargs):
        """SetOldStyleSheet(self, wxRichTextStyleSheet sheet)"""
        return _richtext.RichTextEvent_SetOldStyleSheet(*args, **kwargs)

    def GetNewStyleSheet(*args, **kwargs):
        """GetNewStyleSheet(self) -> wxRichTextStyleSheet"""
        return _richtext.RichTextEvent_GetNewStyleSheet(*args, **kwargs)

    def SetNewStyleSheet(*args, **kwargs):
        """SetNewStyleSheet(self, wxRichTextStyleSheet sheet)"""
        return _richtext.RichTextEvent_SetNewStyleSheet(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange(self) -> RichTextRange"""
        return _richtext.RichTextEvent_GetRange(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(self, RichTextRange range)"""
        return _richtext.RichTextEvent_SetRange(*args, **kwargs)

    def GetCharacter(*args, **kwargs):
        """GetCharacter(self) -> wxChar"""
        return _richtext.RichTextEvent_GetCharacter(*args, **kwargs)

    def SetCharacter(*args, **kwargs):
        """SetCharacter(self, wxChar ch)"""
        return _richtext.RichTextEvent_SetCharacter(*args, **kwargs)

    Flags = property(GetFlags,SetFlags) 
    Index = property(GetPosition,SetPosition) 
    OldStyleSheet = property(GetOldStyleSheet,SetOldStyleSheet) 
    NewStyleSheet = property(GetNewStyleSheet,SetNewStyleSheet) 
    Range = property(GetRange,SetRange) 
    Character = property(GetCharacter,SetCharacter) 
_richtext.RichTextEvent_swigregister(RichTextEvent)

#---------------------------------------------------------------------------

class RichTextHTMLHandler(RichTextFileHandler):
    """Proxy of C++ RichTextHTMLHandler class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String name=HtmlName, String ext=HtmlExt, int type=RICHTEXT_TYPE_HTML) -> RichTextHTMLHandler"""
        _richtext.RichTextHTMLHandler_swiginit(self,_richtext.new_RichTextHTMLHandler(*args, **kwargs))
    def SetTemporaryImageLocations(*args, **kwargs):
        """
        SetTemporaryImageLocations(self, wxArrayString locations)

        Set the list of image locations generated by the last operation
        """
        return _richtext.RichTextHTMLHandler_SetTemporaryImageLocations(*args, **kwargs)

    def GetTemporaryImageLocations(*args, **kwargs):
        """
        GetTemporaryImageLocations(self) -> wxArrayString

        Get the list of image locations generated by the last operation
        """
        return _richtext.RichTextHTMLHandler_GetTemporaryImageLocations(*args, **kwargs)

    TemporaryImageLocations = property(GetTemporaryImageLocations,SetTemporaryImageLocations) 
    def ClearTemporaryImageLocations(*args, **kwargs):
        """
        ClearTemporaryImageLocations(self)

        Clear the image locations generated by the last operation
        """
        return _richtext.RichTextHTMLHandler_ClearTemporaryImageLocations(*args, **kwargs)

    def DeleteTemporaryImages(*args, **kwargs):
        """
        DeleteTemporaryImages(self) -> bool

        Delete the in-memory or temporary files generated by the last operation
        """
        return _richtext.RichTextHTMLHandler_DeleteTemporaryImages(*args, **kwargs)

    def SetFileCounter(*args, **kwargs):
        """
        SetFileCounter(int counter)

        Reset the file counter, in case, for example, the same names are required each
        time
        """
        return _richtext.RichTextHTMLHandler_SetFileCounter(*args, **kwargs)

    SetFileCounter = staticmethod(SetFileCounter)
    def SetTempDir(*args, **kwargs):
        """
        SetTempDir(self, String tempDir)

        Set the directory for storing temporary files. If empty, the system temporary
        directory will be used.
        """
        return _richtext.RichTextHTMLHandler_SetTempDir(*args, **kwargs)

    def GetTempDir(*args, **kwargs):
        """
        GetTempDir(self) -> String

        Get the directory for storing temporary files. If empty, the system temporary
        directory will be used.
        """
        return _richtext.RichTextHTMLHandler_GetTempDir(*args, **kwargs)

    TempDir = property(GetTempDir,SetTempDir) 
    def SetFontSizeMapping(*args, **kwargs):
        """
        SetFontSizeMapping(self, wxArrayInt fontSizeMapping)

        Set mapping from point size to HTML font size. There should be 7 elements, one
        for each HTML font size, each element specifying the maximum point size for
        that HTML font size. E.g. 8, 10, 13, 17, 22, 29, 100

        """
        return _richtext.RichTextHTMLHandler_SetFontSizeMapping(*args, **kwargs)

    def GetFontSizeMapping(*args, **kwargs):
        """
        GetFontSizeMapping(self) -> wxArrayInt

        Get mapping deom point size to HTML font size.
        """
        return _richtext.RichTextHTMLHandler_GetFontSizeMapping(*args, **kwargs)

    FontSizeMapping = property(GetFontSizeMapping,SetFontSizeMapping) 
_richtext.RichTextHTMLHandler_swigregister(RichTextHTMLHandler)
HtmlName = cvar.HtmlName
HtmlExt = cvar.HtmlExt

def RichTextHTMLHandler_SetFileCounter(*args, **kwargs):
  """
    RichTextHTMLHandler_SetFileCounter(int counter)

    Reset the file counter, in case, for example, the same names are required each
    time
    """
  return _richtext.RichTextHTMLHandler_SetFileCounter(*args, **kwargs)

#---------------------------------------------------------------------------

class RichTextXMLHandler(RichTextFileHandler):
    """Proxy of C++ RichTextXMLHandler class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String name=XmlName, String ext=XmlExt, int type=RICHTEXT_TYPE_XML) -> RichTextXMLHandler"""
        _richtext.RichTextXMLHandler_swiginit(self,_richtext.new_RichTextXMLHandler(*args, **kwargs))
_richtext.RichTextXMLHandler_swigregister(RichTextXMLHandler)
XmlName = cvar.XmlName
XmlExt = cvar.XmlExt



