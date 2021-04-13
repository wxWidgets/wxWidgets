/**
 * Copyright 2006-2020 Mitchell mitchell.att.foicica.com. See License.txt.
 *
 * Lua-powered dynamic language lexer for Scintilla.
 *
 * For documentation on writing lexers, see *../doc/LPegLexer.html*.
 */

#if LPEG_LEXER

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#if CURSES
#include <curses.h>
#endif

#include <set>
#include <sstream>
#include <vector>

#if !_WIN32
#include <dirent.h>
#else
#include <io.h>
#endif

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "PropSetSimple.h"
#include "LexAccessor.h"
#include "LexerModule.h"
#include "DefaultLexer.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
LUALIB_API int luaopen_lpeg(lua_State *L);
}

using namespace Scintilla;

#ifndef NDEBUG
#define RECORD_STACK_TOP(l) int orig_stack_top = lua_gettop(l)
#define ASSERT_STACK_TOP(l) assert(lua_gettop(l) == orig_stack_top)
#else
#define RECORD_STACK_TOP(_) (void)0
#define ASSERT_STACK_TOP(_) (void)0
#endif
#if LUA_VERSION_NUM < 502
#define luaL_traceback(_,__,___,____) (void)0
#define luaL_requiref(l, s, f, _) \
  (lua_pushcfunction(l, f), lua_pushstring(l, s), lua_call(l, 1, 1))
#define lua_rawlen lua_objlen
#define LUA_OK 0
#define lua_rawgetp(l, i, p) \
  (lua_pushlightuserdata(l, p), lua_rawget(l, i), lua_type(l, -1))
#define lua_rawsetp(l, i, p) \
  (lua_pushlightuserdata(l, p), lua_insert(l, -2), lua_rawset(l, i))
#endif
#if LUA_VERSION_NUM < 503
#define lua_getfield(l, i, k) (lua_getfield(l, i, k), lua_type(l, -1))
#define lua_rawget(l, i) (lua_rawget(l, i), lua_type(l, -1))
#endif

/** The LPeg Scintilla lexer. */
class LexerLPeg : public DefaultLexer {
  // Lexer property keys.
  const char * const LexerErrorKey = "lexer.lpeg.error";
  const char * const LexerHomeKey = "lexer.lpeg.home";
  const char * const LexerNameKey = "lexer.lpeg.name";
  const char * const LexerThemeKey = "lexer.lpeg.color.theme";

  /**
   * The lexer's Lua state.
   * It is cleared each time the lexer language changes unless `ownLua` is
   * `true`.
   */
  lua_State *L;
  /**
   * The flag indicating whether or not the Lua State is owned by the lexer.
   */
  bool ownLua = true;
  /**
   * The set of properties for the lexer.
   * The LexerHomeKey and LexerNameKey properties must be defined before running
   * the lexer.
   */
  PropSetSimple props;
  /** The function to send Scintilla messages with. */
  SciFnDirect SS = nullptr;
  /** The Scintilla object the lexer belongs to. */
  sptr_t sci = 0;
  /**
   * The flag indicating whether or not the lexer needs to be re-initialized.
   * Re-initialization is required after the lexer language changes.
   */
  bool reinit = true;
  /**
   * The flag indicating whether or not the lexer language has embedded lexers.
   */
  bool multilang = false;
  /**
   * The list of style numbers considered to be whitespace styles.
   * This is used in multi-language lexers when backtracking to whitespace to
   * determine which lexer grammar to use.
   */
  bool ws[STYLE_MAX + 1];
  /** List of known lexer names. */
  std::set<std::string> lexerNames;
  /** Style name to return for `NameOfStyle()`. */
  std::string styleName;

  /**
   * Searches the given directory for lexers and records their names.
   * @param path Path to a directory containing lexers.
   */
  void ReadLexerNames(const char *path);

  /**
   * Logs the given error message or a Lua error message, prints it, and clears
   * the stack.
   * Error messages are logged to the LexerErrorKey property.
   * @param L The Lua State.
   * @param str The error message to log and print. If `nullptr`, logs and
   *   prints the Lua error message at the top of the stack.
   */
  void LogError(lua_State *L, const char *str = nullptr);

  /**
   * Parses the given style string to set the properties for the given style
   * number.
   * Style strings mimic SciTE's "style.*.stylenumber" properties.
   * (https://scintilla.org/SciTEDoc.html)
   * @param num The style number to set properties for.
   * @param style The style string containing properties to set.
   */
  void SetStyle(int num, const char *style);

  /**
   * Iterates through the lexer's `_TOKENSTYLES`, setting the style properties
   * for all defined styles.
   */
  void SetStyles();

  /**
   * Initializes the lexer once the LexerHomeKey and LexerNameKey properties are
   * set.
   */
  bool Init();

  /**
   * When *lparam* is `0`, returns the size of the buffer needed to store the
   * given string *str* in; otherwise copies *str* into the buffer *lparam* and
   * returns the number of bytes copied.
   * @param lparam `0` to get the number of bytes needed to store *str* or a
   *   pointer to a buffer large enough to copy *str* into.
   * @param str The string to copy.
   * @return number of bytes needed to hold *str*
   */
  void *StringResult(long lparam, const char *str);

public:
  /** Constructor. */
  LexerLPeg();

  /** Destructor. */
  virtual ~LexerLPeg() = default;

  /** Destroys the lexer object. */
  void SCI_METHOD Release() override;

  /**
   * Lexes the Scintilla document.
   * @param startPos The position in the document to start lexing at.
   * @param lengthDoc The number of bytes in the document to lex.
   * @param initStyle The initial style at position *startPos* in the document.
   * @param buffer The document interface.
   */
  void SCI_METHOD Lex(
    Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle,
    IDocument *buffer) override;

  /**
   * Folds the Scintilla document.
   * @param startPos The position in the document to start folding at.
   * @param lengthDoc The number of bytes in the document to fold.
   * @param initStyle The initial style at position *startPos* in the document.
   * @param buffer The document interface.
   */
  void SCI_METHOD Fold(
    Sci_PositionU startPos, Sci_Position lengthDoc, int, IDocument *buffer)
    override;

  /**
   * Sets the *key* lexer property to *value*.
   * If *key* starts with "style.", also set the style for the token.
   * @param key The string property key.
   * @param val The string value.
   */
  Sci_Position SCI_METHOD PropertySet(
    const char *key, const char *value) override;

  /**
   * Allows for direct communication between the application and the lexer.
   * The application uses this to set `SS`, `sci`, `L`, and lexer properties,
   * and to retrieve style names.
   * @param code The communication code.
   * @param arg The argument.
   * @return void *data
   */
  void * SCI_METHOD PrivateCall(int code, void *arg) override;

  /**
   * Returns the style name for the given style number.
   * Note: the returned pointer is not guaranteed to exist after the next call
   * to `NameOfStyle()`, so its contents should be immediately copied.
   * @param style The style number to get the style name for.
   * @return style name or nullptr
   */
  const char * SCI_METHOD NameOfStyle(int style) override;

  /**
   * Returns the lexer property for *key*.
   * @param key The string property key.
   */
  const char * SCI_METHOD PropertyGet(const char *key) override;

  /** Constructs a new instance of the lexer. */
  static ILexer *LexerFactoryLPeg();
};

/** Lua pcall error message handler that adds a traceback. */
static int lua_error_handler(lua_State *L) {
  luaL_traceback(L, L, lua_tostring(L, -1), 1);
  return 1;
}

/** lexer.property[key] metamethod. */
static int lexer_property_index(lua_State *L) {
  const char *property = lua_tostring(L, lua_upvalueindex(1));
  lua_getfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");
  LexerLPeg *lexer = reinterpret_cast<LexerLPeg *>(lua_touserdata(L, -1));
  lua_rawgetp(L, LUA_REGISTRYINDEX, lua_touserdata(L, -1));
  lua_getfield(L, -1, "_BUFFER");
  auto buffer = static_cast<IDocument *>(lua_touserdata(L, -1));
  if (strcmp(property, "fold_level") == 0) {
    luaL_argcheck(L, buffer, 1, "must be lexing or folding");
    lua_pushinteger(L, buffer->GetLevel(luaL_checkinteger(L, 2) - 1));
  } else if (strcmp(property, "indent_amount") == 0) {
    luaL_argcheck(L, buffer, 1, "must be lexing or folding");
    lua_pushinteger(L, buffer->GetLineIndentation(luaL_checkinteger(L, 2) - 1));
  } else if (strcmp(property, "property") == 0) {
    lua_pushstring(L, lexer->PropertyGet(luaL_checkstring(L, 2)));
  } else if (strcmp(property, "property_int") == 0) {
    lua_pushstring(L, lexer->PropertyGet(luaL_checkstring(L, 2)));
    lua_pushinteger(L, lua_tointeger(L, -1));
  } else if (strcmp(property, "style_at") == 0) {
    luaL_argcheck(L, buffer, 1, "must be lexing or folding");
    int style = buffer->StyleAt(luaL_checkinteger(L, 2) - 1);
    lua_getfield(L, 4, "_TOKENSTYLES");
    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
      if (luaL_checkinteger(L, -1) - 1 == style) break;
    lua_pop(L, 1); // style_num, leaving name on top
  } else if (strcmp(property, "line_state") == 0) {
    luaL_argcheck(L, buffer, 1, "must be lexing or folding");
    lua_pushinteger(L, buffer->GetLineState(luaL_checkinteger(L, 2) - 1));
  }
  return 1;
}

/** lexer.property[key] = value metamethod. */
static int lexer_property_newindex(lua_State *L) {
  const char *property = lua_tostring(L, lua_upvalueindex(1));
  luaL_argcheck(
    L, strcmp(property, "fold_level") != 0 &&
    strcmp(property, "indent_amount") != 0 &&
    strcmp(property, "property_int") != 0 &&
    strcmp(property, "style_at") != 0 &&
    strcmp(property, "line_from_position") != 0, 3, "read-only property");
  lua_getfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");
  LexerLPeg *lexer = reinterpret_cast<LexerLPeg *>(lua_touserdata(L, -1));
  lua_rawgetp(L, LUA_REGISTRYINDEX, lua_touserdata(L, -1));
  if (strcmp(property, "property") == 0)
    lexer->PropertySet(luaL_checkstring(L, 2), luaL_checkstring(L, 3));
  else if (strcmp(property, "line_state") == 0) {
    luaL_argcheck(
      L, lua_getfield(L, -1, "_BUFFER"), 1, "must be lexing or folding");
    auto buffer = static_cast<IDocument *>(lua_touserdata(L, -1));
    buffer->SetLineState(luaL_checkinteger(L, 2) - 1, luaL_checkinteger(L, 3));
  }
  return 0;
}

/** The lexer's `line_from_position` Lua function. */
static int line_from_position(lua_State *L) {
  auto buffer = static_cast<IDocument *>(
    lua_touserdata(L, lua_upvalueindex(1)));
  lua_pushinteger(L, buffer->LineFromPosition(luaL_checkinteger(L, 1) - 1) + 1);
  return 1;
}

/** lexer.property metamethod. */
static int lexer_index(lua_State *L) {
  const char *key = lua_tostring(L, 2);
  if (strcmp(key, "fold_level") == 0 || strcmp(key, "indent_amount") == 0 ||
      strcmp(key, "property") == 0 || strcmp(key, "property_int") == 0 ||
      strcmp(key, "style_at") == 0 || strcmp(key, "line_state") == 0) {
    lua_newtable(L);
    lua_createtable(L, 0, 2);
    lua_pushvalue(L, 2), lua_pushcclosure(L, lexer_property_index, 1);
    lua_setfield(L, -2, "__index");
    lua_pushvalue(L, 2), lua_pushcclosure(L, lexer_property_newindex, 1);
    lua_setfield(L, -2, "__newindex");
    lua_setmetatable(L, -2);
  } else if (strcmp(key, "line_from_position") == 0) {
    lua_getfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");
    lua_rawgetp(L, LUA_REGISTRYINDEX, lua_touserdata(L, -1));
    luaL_argcheck(
      L, lua_getfield(L, -1, "_BUFFER"), 2, "must be lexing or folding");
    lua_pushcclosure(L, line_from_position, 1);
  } else if (strncmp(key, "fold", 4) == 0) {
    // Alias lexer.fold* to lexer.property['fold*'].
    if (strcmp(key, "folding") == 0) key = "fold"; // lexer.fold() exists
    lua_getfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");
    LexerLPeg *lexer = reinterpret_cast<LexerLPeg *>(lua_touserdata(L, -1));
    const char *value = lexer->PropertyGet(luaL_gsub(L, key, "_", "."));
    lua_pushboolean(L, strcmp(value, "1") == 0);
  } else lua_rawget(L, 1);
  return 1;
}

/** lexer.property = value metamethod. */
static int lexer_newindex(lua_State *L) {
  const char *key = lua_tostring(L, 2);
  luaL_argcheck(
    L, strcmp(key, "fold_level") != 0 && strcmp(key, "indent_amount") != 0 &&
    strcmp(key, "property") != 0 && strcmp(key, "property_int") != 0 &&
    strcmp(key, "style_at") != 0 && strcmp(key, "line_state") != 0 &&
    strcmp(key, "line_from_position") != 0, 3, "read-only property");
  if (strncmp(key, "fold", 4) == 0 && strcmp(key, "fold_level") != 0) {
    // Alias lexer.fold* to lexer.property['fold*'].
    if (strcmp(key, "folding") == 0) key = "fold"; // lexer.fold() exists
    key = luaL_gsub(L, key, "_", ".");
    lua_getfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");
    LexerLPeg *lexer = reinterpret_cast<LexerLPeg *>(lua_touserdata(L, -1));
    if (lua_toboolean(L, 3))
      lexer->PropertySet(
        key, (!lua_isnumber(L, 3) || lua_tonumber(L, 3) == 1) ? "1" : "0");
    else
      lexer->PropertySet(key, "0");
  } else lua_rawset(L, 1);
  return 0;
}

/**
 * Replaces the string property key the top of the stack with its expanded
 * value.
 * Invokes `lexer.property_expanded[]` to perform the expansion.
 * @param L The Lua State.
 */
static void expand_property(lua_State *L) {
  RECORD_STACK_TOP(L);
  lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED"), lua_getfield(L, -1, "lexer");
  lua_getfield(L, -1, "property_expanded");
  lua_pushvalue(L, -4), lua_gettable(L, -2), lua_replace(L, -5);
  lua_pop(L, 3); // property_expanded, lexer, _LOADED
  ASSERT_STACK_TOP(L);
}

void LexerLPeg::ReadLexerNames(const char *path) {
#if !_WIN32
  DIR *dir = opendir(path);
  if (!dir) return;
  struct dirent *entry;
  while ((entry = readdir(dir))) {
    char *p = strstr(entry->d_name, ".lua");
    if (p) lexerNames.emplace(entry->d_name, p - entry->d_name);
  }
  closedir(dir);
#else
  struct _finddata_t file;
  std::string glob(path);
  glob += "/*";
  intptr_t handle = _findfirst(glob.c_str(), &file);
  if (handle == -1) return;
  do {
    char *p = strstr(file.name, ".lua");
    if (p) lexerNames.emplace(file.name, p - file.name);
  } while (_findnext(handle, &file) != -1);
  _findclose(handle);
#endif
}

void LexerLPeg::LogError(lua_State *L, const char *str) {
  const char *value = str ? str : lua_tostring(L, -1);
  PropertySet(LexerErrorKey, value);
  fprintf(stderr, "Lua Error: %s.\n", value);
  lua_settop(L, 0);
}

void LexerLPeg::SetStyle(int num, const char *style) {
  auto style_copy = static_cast<char *>(malloc(strlen(style) + 1));
  char *key = strcpy(style_copy, style), *next = nullptr, *val = nullptr;
  while (key) {
    if ((next = strchr(key, ','))) *next++ = '\0';
    if ((val = strchr(key, ':'))) *val++ = '\0';
    if (strcmp(key, "font") == 0 && val)
      SS(sci, SCI_STYLESETFONT, num, reinterpret_cast<sptr_t>(val));
    else if (strcmp(key, "size") == 0 && val)
      SS(sci, SCI_STYLESETSIZE, num, static_cast<int>(atoi(val)));
    else if (strcmp(key, "bold") == 0 || strcmp(key, "notbold") == 0 ||
             strcmp(key, "weight") == 0) {
#if !CURSES
      int weight = SC_WEIGHT_NORMAL;
      if (*key == 'b')
        weight = SC_WEIGHT_BOLD;
      else if (*key == 'w' && val)
        weight = atoi(val);
      SS(sci, SCI_STYLESETWEIGHT, num, weight);
#else
      // Scintilla curses requires font attributes to be stored in the "font
      // weight" style attribute.
      // First, clear any existing SC_WEIGHT_NORMAL, SC_WEIGHT_SEMIBOLD, or
      // SC_WEIGHT_BOLD values stored in the lower 16 bits. Then set the
      // appropriate curses attr.
      sptr_t weight =
        SS(sci, SCI_STYLEGETWEIGHT, num, 0) & ~(A_COLOR | A_CHARTEXT);
      int bold =
        *key == 'b' || (*key == 'w' && val && atoi(val) > SC_WEIGHT_NORMAL);
      SS(
        sci, SCI_STYLESETWEIGHT, num,
        bold ? weight | A_BOLD : weight & ~A_BOLD);
#endif
    } else if (strcmp(key, "italics") == 0 || strcmp(key, "notitalics") == 0)
      SS(sci, SCI_STYLESETITALIC, num, *key == 'i');
    else if (strcmp(key, "underlined") == 0 ||
             strcmp(key, "notunderlined") == 0) {
#if !CURSES
      SS(sci, SCI_STYLESETUNDERLINE, num, *key == 'u');
#else
      // Scintilla curses requires font attributes to be stored in the "font
      // weight" style attribute.
      // First, clear any existing SC_WEIGHT_NORMAL, SC_WEIGHT_SEMIBOLD, or
      // SC_WEIGHT_BOLD values stored in the lower 16 bits. Then set the
      // appropriate curses attr.
      sptr_t weight =
        SS(sci, SCI_STYLEGETWEIGHT, num, 0) & ~(A_COLOR | A_CHARTEXT);
      SS(
        sci, SCI_STYLESETWEIGHT, num,
        (*key == 'u') ? weight | A_UNDERLINE : weight & ~A_UNDERLINE);
#endif
    } else if ((strcmp(key, "fore") == 0 || strcmp(key, "back") == 0) &&
               val) {
      int msg = (*key == 'f') ? SCI_STYLESETFORE : SCI_STYLESETBACK;
      int color = static_cast<int>(strtol(val, nullptr, 0));
      if (*val == '#') { // #RRGGBB format; Scintilla format is 0xBBGGRR
        color = static_cast<int>(strtol(val + 1, nullptr, 16));
        color =
          ((color & 0xFF0000) >> 16) | (color & 0xFF00) |
          ((color & 0xFF) << 16); // convert to 0xBBGGRR
      }
      SS(sci, msg, num, color);
    } else if (strcmp(key, "eolfilled") == 0 ||
               strcmp(key, "noteolfilled") == 0)
      SS(sci, SCI_STYLESETEOLFILLED, num, *key == 'e');
    else if (strcmp(key, "characterset") == 0 && val)
      SS(sci, SCI_STYLESETCHARACTERSET, num, static_cast<int>(atoi(val)));
    else if (strcmp(key, "case") == 0 && val) {
      if (*val == 'u')
        SS(sci, SCI_STYLESETCASE, num, SC_CASE_UPPER);
      else if (*val == 'l')
        SS(sci, SCI_STYLESETCASE, num, SC_CASE_LOWER);
    } else if (strcmp(key, "visible") == 0 || strcmp(key, "notvisible") == 0)
      SS(sci, SCI_STYLESETVISIBLE, num, *key == 'v');
    else if (strcmp(key, "changeable") == 0 ||
             strcmp(key, "notchangeable") == 0)
      SS(sci, SCI_STYLESETCHANGEABLE, num, *key == 'c');
    else if (strcmp(key, "hotspot") == 0 || strcmp(key, "nothotspot") == 0)
      SS(sci, SCI_STYLESETHOTSPOT, num, *key == 'h');
    key = next;
  }
  free(style_copy);
}

void LexerLPeg::SetStyles() {
  RECORD_STACK_TOP(L);
  // If the lexer defines additional styles, set their properties first (if
  // the user has not already defined them).
  lua_rawgetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this));
  lua_getfield(L, -1, "_EXTRASTYLES");
  for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
    if (lua_isstring(L, -2) && lua_isstring(L, -1)) {
      lua_pushstring(L, "style."), lua_pushvalue(L, -3), lua_concat(L, 2);
      if (!*props.Get(lua_tostring(L, -1)))
        PropertySet(lua_tostring(L, -1), lua_tostring(L, -2));
      lua_pop(L, 1); // style name
    }
  lua_pop(L, 1); // _EXTRASTYLES

  if (!SS || !sci) {
    lua_pop(L, 1); // lexer object
    // Skip, but do not report an error since `reinit` would remain `false`
    // and subsequent calls to `Lex()` and `Fold()` would repeatedly call this
    // function and error.
    ASSERT_STACK_TOP(L);
    return;
  }
  lua_pushstring(L, "style.default"), expand_property(L);
  SetStyle(STYLE_DEFAULT, lua_tostring(L, -1));
  lua_pop(L, 1); // style
  SS(sci, SCI_STYLECLEARALL, 0, 0); // set default styles
  lua_getfield(L, -1, "_TOKENSTYLES");
  for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
    if (lua_isstring(L, -2) && lua_isnumber(L, -1) &&
        lua_tointeger(L, -1) - 1 != STYLE_DEFAULT) {
      lua_pushstring(L, "style."), lua_pushvalue(L, -3), lua_concat(L, 2);
      expand_property(L);
      SetStyle(lua_tointeger(L, -2) - 1, lua_tostring(L, -1));
      lua_pop(L, 1); // style
    }
  lua_pop(L, 2); // _TOKENSTYLES, lexer object
  ASSERT_STACK_TOP(L);
}

bool LexerLPeg::Init() {
  if (!props.GetExpanded(LexerHomeKey, nullptr) ||
      !*props.Get(LexerNameKey) || !L)
    return false;
  char *_home = reinterpret_cast<char *>(
    malloc(props.GetExpanded(LexerHomeKey, nullptr) + 1));
  props.GetExpanded(LexerHomeKey, _home);
  std::string home(_home);
  free(_home);
  const char *lexer = props.Get(LexerNameKey);
  RECORD_STACK_TOP(L);

  // Designate the currently running LexerLPeg instance.
  // This needs to be done prior to calling any Lua lexer code, particularly
  // when `ownLua` is `false`, as there may be multiple LexerLPeg instances
  // floating around, and the lexer module methods and metamethods need to
  // know which instance to use.
  lua_pushlightuserdata(L, reinterpret_cast<void *>(this));
  lua_setfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");

  // Determine where to look for the lexer module and themes.
  std::vector<std::string> dirs;
  size_t start = 0, end;
  while ((end = home.find(';', start)) != std::string::npos) {
    dirs.emplace_back(home, start, end - start);
    start = end + 1;
  }
  dirs.emplace_back(home, start);

  // If necessary, load the lexer module.
  lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
  if (lua_getfield(L, -1, "lexer") == LUA_TNIL) {
    for (const std::string& dir : dirs) {
      lua_pushstring(L, dir.c_str());
      lua_pushstring(L, "/lexer.lua");
      lua_concat(L, 2);
      int status = luaL_loadfile(L, lua_tostring(L, -1));
      if (status == LUA_ERRFILE) {
        lua_pop(L, 2); // error message, filename
        continue; // try next directory
      }
      lua_remove(L, -2); // filename
      lua_pushcfunction(L, lua_error_handler);
      lua_insert(L, -2);
      if (status == LUA_OK && lua_pcall(L, 0, 1, -2) == LUA_OK) break;
      return (LogError(L), false);
    }
    if (lua_isnil(L, -1))
      return (LogError(L, "'lexer.lua' module not found"), false);
    lua_remove(L, -2); // lua_error_handler
    lua_replace(L, -2); // nil
    lua_pushinteger(L, SC_FOLDLEVELBASE);
    lua_setfield(L, -2, "FOLD_BASE");
    lua_pushinteger(L, SC_FOLDLEVELWHITEFLAG);
    lua_setfield(L, -2, "FOLD_BLANK");
    lua_pushinteger(L, SC_FOLDLEVELHEADERFLAG);
    lua_setfield(L, -2, "FOLD_HEADER");
    lua_createtable(L, 0, 2);
    lua_pushcfunction(L, lexer_index), lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, lexer_newindex), lua_setfield(L, -2, "__newindex");
    lua_setmetatable(L, -2);
    lua_pushvalue(L, -1), lua_setfield(L, -3, "lexer");
  }
  lua_replace(L, -2);
  // Update the userdata needed by lexer metamethods.
  lua_pushvalue(L, -1);
  lua_rawsetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this));

  // Load the language lexer.
  if (lua_getfield(L, -1, "load") != LUA_TFUNCTION)
    return (LogError(L, "'lexer.load' function not found"), false);
  lua_pushcfunction(L, lua_error_handler), lua_insert(L, -2);
  lua_pushstring(L, lexer), lua_pushnil(L), lua_pushboolean(L, 1);
  if (lua_pcall(L, 3, 1, -5) != LUA_OK) return (LogError(L), false);
  lua_remove(L, -2); // lua_error_handler
  lua_remove(L, -2); // lexer module
  lua_rawsetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this));

  // Load the theme and set up styles.
  if (props.GetExpanded(LexerThemeKey, nullptr)) {
    char *theme = reinterpret_cast<char *>(
      malloc(props.GetExpanded(LexerThemeKey, nullptr) + 1));
    props.GetExpanded(LexerThemeKey, theme);
    if (!strstr(theme, "/") && !strstr(theme, "\\")) { // theme name
      for (const std::string& dir : dirs) {
        lua_pushstring(L, dir.c_str());
        lua_pushstring(L, "/themes/");
        lua_pushstring(L, theme);
        lua_pushstring(L, ".lua");
        lua_concat(L, 4);
        if (luaL_loadfile(L, lua_tostring(L, -1)) != LUA_ERRFILE ||
            dir == dirs.back()) {
          lua_pop(L, 1); // function, leaving filename on top
          break;
        }
        lua_pop(L, 2); // error message, filename
      }
    } else lua_pushstring(L, theme); // path to theme
    lua_pushcfunction(L, lua_error_handler);
    lua_insert(L, -2);
    if (luaL_loadfile(L, lua_tostring(L, -1)) == LUA_OK &&
        lua_pcall(L, 0, 0, -3) == LUA_OK)
      lua_pop(L, 2); // theme, lua_error_handler
    else
      LogError(L);
    free(theme);
  }
  SetStyles();

  // If the lexer is a parent, it will have children in its _CHILDREN table.
  // In that case, determine which styles are language whitespace styles
  // ([lang]_whitespace). This is necessary for determining which language
  // to start lexing with.
  lua_rawgetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this));
  if (lua_getfield(L, -1, "_CHILDREN") == LUA_TTABLE) {
    multilang = true;
    for (int i = 0; i <= STYLE_MAX; i++)
      ws[i] = strstr(NameOfStyle(i), "whitespace") ? true : false;
  }
  lua_pop(L, 2); // _CHILDREN, lexer object

  reinit = false;
  PropertySet(LexerErrorKey, "");
  ASSERT_STACK_TOP(L);
  return true;
}

void *LexerLPeg::StringResult(long lparam, const char *str) {
  if (lparam) strcpy(reinterpret_cast<char *>(lparam), str);
  return reinterpret_cast<void *>(strlen(str));
}

LexerLPeg::LexerLPeg() : DefaultLexer("lpeg", SCLEX_LPEG), L(luaL_newstate()) {
  // Initialize the Lua state, load libraries, and set platform variables.
  if (!L) {
    fprintf(stderr, "Lua failed to initialize.\n");
    return;
  }
#if LUA_VERSION_NUM < 502
  luaL_requiref(L, "", luaopen_base, 1), lua_pop(L, 1);
#else
  luaL_requiref(L, "_G", luaopen_base, 1), lua_pop(L, 1);
#endif
  luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1), lua_pop(L, 1);
  luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1), lua_pop(L, 1);
  // TODO: figure out why lua_setglobal() is needed for lpeg.
  luaL_requiref(L, "lpeg", luaopen_lpeg, 1), lua_setglobal(L, "lpeg");
#if _WIN32
  lua_pushboolean(L, 1), lua_setglobal(L, "WIN32");
#endif
#if __APPLE__
  lua_pushboolean(L, 1), lua_setglobal(L, "OSX");
#endif
#if GTK
  lua_pushboolean(L, 1), lua_setglobal(L, "GTK");
#endif
#if CURSES
  lua_pushboolean(L, 1), lua_setglobal(L, "CURSES");
#endif
}

void SCI_METHOD LexerLPeg::Release() {
  if (ownLua && L)
    lua_close(L);
  else if (!ownLua) {
    lua_pushnil(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this));
    lua_pushnil(L), lua_setfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");
  }
  delete this;
}

void SCI_METHOD LexerLPeg::Lex(
  Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle,
  IDocument *buffer)
{
  LexAccessor styler(buffer);
  if ((reinit && !Init()) || !L) {
    // Style everything in the default style.
    styler.StartAt(startPos);
    styler.StartSegment(startPos);
    styler.ColourTo(startPos + lengthDoc - 1, STYLE_DEFAULT);
    styler.Flush();
    return;
  }
  RECORD_STACK_TOP(L);
  lua_pushlightuserdata(L, reinterpret_cast<void *>(this));
  lua_setfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");
  lua_rawgetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this));
  lua_pushlightuserdata(L, reinterpret_cast<void *>(buffer));
  lua_setfield(L, -2, "_BUFFER");

  // Ensure the lexer has a grammar.
  // This could be done in the lexer module's `lex()`, but for large files,
  // passing string arguments from C to Lua is expensive.
  if (!lua_getfield(L, -1, "_GRAMMAR")) {
    lua_pop(L, 2); // _GRAMMAR, lexer object
    // Style everything in the default style.
    styler.StartAt(startPos);
    styler.StartSegment(startPos);
    styler.ColourTo(startPos + lengthDoc - 1, STYLE_DEFAULT);
    styler.Flush();
    return;
  } else lua_pop(L, 1); // _GRAMMAR

  // Start from the beginning of the current style so LPeg matches it.
  // For multilang lexers, start at whitespace since embedded languages have
  // [lang]_whitespace styles. This is so LPeg can start matching child
  // languages instead of parent ones if necessary.
  if (startPos > 0) {
    Sci_PositionU i = startPos;
    while (i > 0 && styler.StyleAt(i - 1) == initStyle) i--;
    if (multilang)
      while (i > 0 && !ws[static_cast<size_t>(styler.StyleAt(i))]) i--;
    lengthDoc += startPos - i, startPos = i;
  }

  if (lua_getfield(L, -1, "lex") != LUA_TFUNCTION)
    return LogError(L, "'lexer.lex' function not found");
  lua_pushcfunction(L, lua_error_handler), lua_insert(L, -2);
  lua_pushvalue(L, -3);
  lua_pushlstring(L, buffer->BufferPointer() + startPos, lengthDoc);
  lua_pushinteger(L, styler.StyleAt(startPos) + 1);
  if (lua_pcall(L, 3, 1, -5) != LUA_OK) return LogError(L);
  if (!lua_istable(L, -1))
    return LogError(L, "Table of tokens expected from 'lexer.lex'");
  // Style the text from the token table returned.
  int len = lua_rawlen(L, -1);
  if (len > 0) {
    int style = STYLE_DEFAULT;
    styler.StartAt(startPos);
    styler.StartSegment(startPos);
    lua_getfield(L, -3, "_TOKENSTYLES");
    // Loop through token-position pairs.
    for (int i = 1; i < len; i += 2) {
      style = STYLE_DEFAULT;
      if (lua_rawgeti(L, -2, i), lua_rawget(L, -2))
        style = lua_tointeger(L, -1) - 1;
      lua_pop(L, 1); // _TOKENSTYLES[token]
      lua_rawgeti(L, -2, i + 1); // pos
      unsigned int position = lua_tointeger(L, -1) - 1;
      lua_pop(L, 1); // pos
      if (style >= 0 && style <= STYLE_MAX)
        styler.ColourTo(startPos + position - 1, style);
      else
        lua_pushfstring(L, "Bad style number: %d", style), LogError(L);
      if (position > startPos + lengthDoc) break;
    }
    lua_pop(L, 1); // _TOKENSTYLES
    styler.ColourTo(startPos + lengthDoc - 1, style);
    styler.Flush();
  }
  lua_pop(L, 3); // token table returned, lua_error_handler, lexer object
  ASSERT_STACK_TOP(L);
}

void SCI_METHOD LexerLPeg::Fold(
  Sci_PositionU startPos, Sci_Position lengthDoc, int, IDocument *buffer)
{
  if ((reinit && !Init()) || !L) return;
  RECORD_STACK_TOP(L);
  lua_pushlightuserdata(L, reinterpret_cast<void *>(this));
  lua_setfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");
  lua_rawgetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this));
  lua_pushlightuserdata(L, reinterpret_cast<void *>(buffer));
  lua_setfield(L, -2, "_BUFFER");
  LexAccessor styler(buffer);

  if (lua_getfield(L, -1, "fold") != LUA_TFUNCTION)
    return LogError(L, "'lexer.fold' function not found");
  lua_pushcfunction(L, lua_error_handler), lua_insert(L, -2);
  lua_pushvalue(L, -3);
  Sci_Position currentLine = styler.GetLine(startPos);
  lua_pushlstring(L, buffer->BufferPointer() + startPos, lengthDoc);
  lua_pushinteger(L, startPos + 1);
  lua_pushinteger(L, currentLine + 1);
  lua_pushinteger(L, styler.LevelAt(currentLine) & SC_FOLDLEVELNUMBERMASK);
  if (lua_pcall(L, 5, 1, -7) != LUA_OK) return LogError(L);
  if (!lua_istable(L, -1))
    return LogError(L, "Table of folds expected from 'lexer.fold'");
  // Fold the text from the fold table returned.
  for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) // line = level
    styler.SetLevel(lua_tointeger(L, -2) - 1, lua_tointeger(L, -1));
  lua_pop(L, 3); // fold table returned, lua_error_handler, lexer object
  ASSERT_STACK_TOP(L);
}

Sci_Position SCI_METHOD LexerLPeg::PropertySet(
  const char *key, const char *value)
{
  props.Set(key, value, strlen(key), strlen(value));
  if (strcmp(key, LexerHomeKey) == 0 && lexerNames.empty())
    ReadLexerNames(value); // not using SCI_LOADLEXERLIBRARY private call
  if (reinit &&
      (strcmp(key, LexerHomeKey) == 0 || strcmp(key, LexerNameKey) == 0))
    Init();
  else if (L && SS && sci && strncmp(key, "style.", 6) == 0) {
    // The container is managing styles manually.
    RECORD_STACK_TOP(L);
    lua_pushlightuserdata(L, reinterpret_cast<void *>(this));
    lua_setfield(L, LUA_REGISTRYINDEX, "sci_lexer_lpeg");
    if (lua_rawgetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this))) {
      lua_getfield(L, -1, "_TOKENSTYLES");
      lua_pushstring(L, key + 6);
      if (lua_rawget(L, -2) == LUA_TNUMBER) {
        lua_pushstring(L, key), expand_property(L);
        int style_num = lua_tointeger(L, -2) - 1;
        SetStyle(style_num, lua_tostring(L, -1));
        if (style_num == STYLE_DEFAULT)
          // Assume a theme change, with the default style being set first.
          // Subsequent style settings will be based on the default.
          SS(sci, SCI_STYLECLEARALL, 0, 0);
        lua_pop(L, 1); // style
      }
      lua_pop(L, 2); // style number, _TOKENSTYLES
    }
    lua_pop(L, 1); // lexer object or nil
    ASSERT_STACK_TOP(L);
  }
  return -1; // no need to re-lex
}

void * SCI_METHOD LexerLPeg::PrivateCall(int code, void *arg) {
  auto lParam = reinterpret_cast<sptr_t>(arg);
  switch(code) {
  case SCI_GETDIRECTFUNCTION:
    SS = reinterpret_cast<SciFnDirect>(lParam);
    return nullptr;
  case SCI_SETDOCPOINTER:
    sci = lParam;
    return nullptr;
  case SCI_CHANGELEXERSTATE:
    if (ownLua) lua_close(L);
    L = reinterpret_cast<lua_State *>(arg), ownLua = false;
    return nullptr;
  case SCI_LOADLEXERLIBRARY: {
    const char *path = reinterpret_cast<const char*>(arg);
    ReadLexerNames(path);
    std::string home(props.Get(LexerHomeKey));
    if (!home.empty()) home.push_back(';');
    home.append(path);
    PropertySet(LexerHomeKey, home.c_str());
    return nullptr;
  } case SCI_PROPERTYNAMES: {
    std::stringstream names;
    for (const std::string& name : lexerNames) names << name << '\n';
    return StringResult(lParam, names.str().c_str());
  } case SCI_SETLEXERLANGUAGE:
    if (strcmp(
          props.Get(LexerNameKey),
          reinterpret_cast<const char *>(arg)) != 0) {
      reinit = true;
      PropertySet(LexerErrorKey, "");
      PropertySet(LexerNameKey, reinterpret_cast<const char *>(arg));
    } else if (L)
      ownLua ? SetStyles() : static_cast<void>(Init());
    return nullptr;
  case SCI_GETLEXERLANGUAGE: {
    if (!L) return StringResult(lParam, "null");
    RECORD_STACK_TOP(L);
    lua_rawgetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this));
    lua_getfield(L, -1, "_NAME");
    std::string val(lua_tostring(L, -1));
    lua_pop(L, 2); // lexer name, lexer object
    ASSERT_STACK_TOP(L);
    if (!SS || !sci || !multilang) return StringResult(lParam, val.c_str());
    val.push_back('/');
    int pos = SS(sci, SCI_GETCURRENTPOS, 0, 0);
    while (pos >= 0 && !ws[SS(sci, SCI_GETSTYLEAT, pos, 0)]) pos--;
    if (pos >= 0) {
      const char *name = NameOfStyle(SS(sci, SCI_GETSTYLEAT, pos, 0)), *p;
      if (name && (p = strstr(name, "_whitespace"))) {
        val.append(name, p - name);
        return StringResult(lParam, val.c_str());
      }
    }
    val.append(val, 0, val.length() - 1); // "lexer/lexer" fallback
    return StringResult(lParam, val.c_str());
  } case SCI_GETNAMEDSTYLES:
    if (!L) return reinterpret_cast<void *>(STYLE_DEFAULT);
    for (int i = 0; i < STYLE_MAX; i++)
      if (strcmp(NameOfStyle(i), reinterpret_cast<const char *>(arg)) == 0)
        return reinterpret_cast<void *>(i);
    return reinterpret_cast<void *>(STYLE_DEFAULT);
  case SCI_GETSTATUS:
    return StringResult(lParam, props.Get(LexerErrorKey));
  }
  return nullptr;
}

const char * SCI_METHOD LexerLPeg::NameOfStyle(int style) {
  if (style < 0 || style > STYLE_MAX || !L) return nullptr;
  RECORD_STACK_TOP(L);
  styleName = "Not Available";
  lua_rawgetp(L, LUA_REGISTRYINDEX, reinterpret_cast<void *>(this));
  lua_getfield(L, -1, "_TOKENSTYLES");
  for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
    if (lua_tointeger(L, -1) - 1 == style) {
      styleName = lua_tostring(L, -2);
      lua_pop(L, 2); // value and key
      break;
    }
  lua_pop(L, 2); // _TOKENSTYLES, lexer object
  ASSERT_STACK_TOP(L);
  return styleName.c_str();
}

const char * SCI_METHOD LexerLPeg::PropertyGet(const char *key) {
  return props.Get(key);
}

ILexer *LexerLPeg::LexerFactoryLPeg() { return new LexerLPeg(); }

LexerModule lmLPeg(SCLEX_LPEG, LexerLPeg::LexerFactoryLPeg, "lpeg");

#else

#include <stdlib.h>
#include <assert.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "LexerModule.h"

using namespace Scintilla;

static void LPegLex(Sci_PositionU, Sci_Position, int, WordList*[], Accessor&) {
  return;
}

LexerModule lmLPeg(SCLEX_LPEG, LPegLex, "lpeg");

#endif // LPEG_LEXER
