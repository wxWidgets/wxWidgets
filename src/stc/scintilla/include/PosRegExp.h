#ifndef POSREGEXP_H
#define POSREGEXP_H

#define MatchesNum 0x10

enum EOps
{
  ReBlockOps = 0x1000,
  ReMul,              // *
  RePlus,             // +
  ReQuest,            // ?
  ReNGMul,            // *?
  ReNGPlus,           // +?
  ReNGQuest,          // ??
  ReRangeN,           // {n,}
  ReRangeNM,          // {n,m}
  ReNGRangeN,         // {n,}?
  ReNGRangeNM,        // {n,m}?
  ReOr,               // |
  ReBehind  = 0x1100, // ?#n
  ReNBehind = 0x1200, // ?~n
  ReAhead   = 0x1300, // ?=
  ReNAhead  = 0x1400, // ?!

  ReSymbolOps = 0x2000,
  ReEmpty,
  ReSymb,             // a b \W \s ...
  ReEnum,             // []
  ReNEnum,            // [^]
  ReBrackets,         // (...)
  ReBkTrace = 0x2100, // \yN
  ReBkBrack = 0x2200 // \N
};

enum ESymbols
{
  ReAnyChr = 0x4000,  // .
  ReSoL,              // ^
  ReEoL,              // $
  ReDigit,            // \d
  ReNDigit,           // \D
  ReWordSymb,         // \w
  ReNWordSymb,        // \W
  ReWSpace,           // \s
  ReNWSpace,          // \S
  ReUCase,            // \u
  ReNUCase ,          // \l
  ReWBound,           // \b
  ReNWBound,          // \B
  RePreNW,            // \c
  ReStart,            // \m
  ReEnd,              // \M

  ReChr    = 0x0      // Char in Lower Byte
};
enum ETempSymb
{
  ReTemp = 0x7000,
  ReLBrack, ReRBrack,
  ReEnumS, ReEnumE, ReNEnumS,
  ReRangeS, ReRangeE, ReNGRangeE, ReFrToEnum
};

#define BackSlash '\\'

typedef union SCharData
{
  int  IArr[8];
  char CArr[32];
  void SetBit(unsigned char Bit);
  void ClearBit(unsigned char Bit);
  bool GetBit(unsigned char Bit);
} *PCharData;

typedef struct SRegInfo
{
  SRegInfo();
  ~SRegInfo();

  EOps   Op;
  union{
    SRegInfo *Param;
    int Symb;
    PCharData ChrClass;
  }un;
  int s,e;
  SRegInfo *Parent;
  SRegInfo *Next;
} *PRegInfo;

typedef struct SMatches
{
  int s[MatchesNum];
  int e[MatchesNum];
  int CurMatch;
} *PMatches;

typedef class PosRegExp
{
  PRegInfo Info;
  PMatches BkTrace;
  bool NoCase,Extend,NoMoves;
  bool Error;
  int  *Exprn;
  int  posParse;
  int  posEnd,posStart;
  int  posBkStr;
  int  FirstChar;

  bool SetExprLow(const char *Expr);
  bool SetStructs(PRegInfo &Info,int st,int end);
  void Optimize();
  bool CheckSymb(int Symb,bool Inc);
  bool LowParse(PRegInfo Re);
  bool LowParseRe(PRegInfo &Next);
  bool LowCheckNext(PRegInfo Re);
  bool ParseRe(int posStr);
  bool QuickCheck();
public:
  PMatches Matches;
  int Ok, CurMatch;

  void *param;
  char (*CharAt)(int pos, void *param);

  PosRegExp();
  ~PosRegExp();

  bool isok();
  bool SetNoMoves(bool Moves);
  bool SetBkTrace(int posStr,PMatches Trace);
  bool SetExpr(const char *Expr);
  bool Parse(int posStr, int posStop, PMatches Mtch);
  bool Parse(int posStr,int posSol, int posEol, PMatches Mtch, int Moves = -1);
  bool Evaluate(char *Expr, int posStr, PMatches Mtch, char **Res);
} *PPosRegExp;

#endif /* POSREGEXP_H */
