#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>

#include "PosRegExp.h"

//Up: /[A-Z \x80-\x9f \xf0 ]/x
//Lo: /[a-z \xa0-\xaf \xe0-\xef \xf1 ]/x
//Wd: /[\d _ A-Z a-z \xa0-\xaf \xe0-\xf1 \x80-\x9f]/x
//*   // Dos866
SCharData UCData  = {0x0, 0x0, 0x7fffffe, 0x0, 0xffffffff, 0x0, 0x0, 0x10000},
                     LCData  = {0x0, 0x0, 0x0, 0x7fffffe, 0x0, 0xffff, 0x0, 0x2ffff},
                     WdData  = {0x0, 0x3ff0000, 0x87fffffe, 0x7fffffe, 0xffffffff, 0xffff, 0x0, 0x3ffff},
                     DigData = {0x0, 0x3ff0000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
/*/   // cp1251
SCharData UCData  = {0x0, 0x0, 0x7fffffe, 0x0, 0x0, 0x0, 0xffffffff, 0x0},
          LCData  = {0x0, 0x0, 0x0, 0x7fffffe, 0x0, 0x0, 0x0, 0xffffffff},
          WdData  = {0x0, 0x3ff0000, 0x87fffffe, 0x7fffffe, 0x0, 0x0, 0xffffffff, 0xffffffff},
          DigData = {0x0, 0x3ff0000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
//*/

///////////////////////////////////////////////

int GetNumber(int *str,int s,int e) {
	int r = 1, num = 0;
	if (e < s) return -1;
	for(int i = e-1; i >= s; i--) {
		if (str[i] > '9' || str[i] < '0') return -1;
		num += (str[i] - 0x30)*r;
		r *= 10;
	};
	return num;
	/*
	char tmp[20];
	double Res;
	  if (e == s) return -1;
	  for (int i = s;i < e;i++)
	    tmp[i-s] = (char)Str[i];
	  tmp[e-s] = 0;
	  GetNumber(tmp,&Res);
	  return (int)Res;
	*/
};

bool IsDigit(char Symb) {
	return DigData.GetBit(Symb);
};
bool IsWord(char Symb) {
	return WdData.GetBit(Symb);
};
bool IsUpperCase(char Symb) {
	return UCData.GetBit(Symb);
};
bool IsLowerCase(char Symb) {
	return LCData.GetBit(Symb);
};
char LowCase(char Chr) {
	if (UCData.GetBit(Chr))
		return Chr+0x20;
	return Chr;
};

///////////////////////////////////////////////

SRegInfo::SRegInfo() {
	Next = Parent = 0;
	un.Param = 0;
	Op = ReEmpty;
};
SRegInfo::~SRegInfo() {
	if (Next) delete Next;
	if (un.Param)
		switch(Op) {
		case ReEnum:
		case ReNEnum:
			delete un.ChrClass;
			break;
		default:
			if (Op > ReBlockOps && Op < ReSymbolOps || Op == ReBrackets)
				delete un.Param;
			break;
		};
};

///////////////////////////////////////////////

void SCharData::SetBit(unsigned char Bit) {
	int p = Bit/8;
	CArr[p] |= (1 << Bit%8);
};
void SCharData::ClearBit(unsigned char Bit) {
	int p = Bit/8;
	CArr[p] &= ~(1 << Bit%8);
};
bool SCharData::GetBit(unsigned char Bit) {
	int p = (unsigned char)Bit/8;
	return (CArr[p] & (1 << Bit%8))!=0;
};

/////////////////////////////////////////////////////////////////
//////////////////////  RegExp Class  ///////////////////////////
/////////////////////////////////////////////////////////////////

PosRegExp::PosRegExp() {
	Info = 0;
	Exprn = 0;
	NoMoves = false;
	Error = true;
	FirstChar = 0;
	CurMatch = 0;
};
PosRegExp::~PosRegExp() {
	if (Info) delete Info;
};

bool PosRegExp::SetExpr(const char *Expr) {
	if (!this) return false;
	Error = true;
	CurMatch = 0;
	if (SetExprLow(Expr)) Error = false;
	return !Error;
};
bool PosRegExp::isok() {
	return !Error;
};


bool PosRegExp::SetExprLow(const char *Expr) {
	int Len = strlen(Expr);
	bool  Ok = false;
	int i,j,s = 0,pos,tmp;
	int EnterBr = 0,EnterGr = 0,EnterFg = 0;

	if (Info) delete Info;
	Info = new SRegInfo;
	Exprn = new int[Len];

	NoCase = false;
	Extend = false;
	if (Expr[0] == '/') s++;
	else return false;

	for (i = Len; i > 0 && !Ok;i--)
		if (Expr[i] == '/') {
			Len = i-s;
			Ok = true;
			for (int j = i+1; Expr[j]; j++) {
				if (Expr[j] == 'i') NoCase = true;
				if (Expr[j] == 'x') Extend = true;
			};
		};
	if (!Ok) return false;

	////////////////////////////////
	for (j = 0,pos = 0; j < Len; j++,pos++) {
		if (Extend && Expr[j+s] == ' ') {
			pos--;
			continue;
		};

		Exprn[pos] = (int)(unsigned char)Expr[j+s];

		if (Expr[j+s] == BackSlash) {
			switch (Expr[j+s+1]) {
			case 'd':
				Exprn[pos] = ReDigit;
				break;
			case 'D':
				Exprn[pos] = ReNDigit;
				break;
			case 'w':
				Exprn[pos] = ReWordSymb;
				break;
			case 'W':
				Exprn[pos] = ReNWordSymb;
				break;
			case 's':
				Exprn[pos] = ReWSpace;
				break;
			case 'S':
				Exprn[pos] = ReNWSpace;
				break;
			case 'u':
				Exprn[pos] = ReUCase;
				break;
			case 'l':
				Exprn[pos] = ReNUCase;
				break;
			case 't':
				Exprn[pos] = '\t';
				break;
			case 'n':
				Exprn[pos] = '\n';
				break;
			case 'r':
				Exprn[pos] = '\r';
				break;
			case 'b':
				Exprn[pos] = ReWBound;
				break;
			case 'B':
				Exprn[pos] = ReNWBound;
				break;
			case 'c':
				Exprn[pos] = RePreNW;
				break;
			case 'm':
				Exprn[pos] = ReStart;
				break;
			case 'M':
				Exprn[pos] = ReEnd;
				break;
			case 'x':
				tmp = toupper(Expr[j+s+2])-0x30;
				tmp = (tmp>9?tmp-7:tmp)<<4;
				tmp += (toupper(Expr[j+s+3])-0x30)>9?toupper(Expr[j+s+3])-0x37:(toupper(Expr[j+s+3])-0x30);
				Exprn[pos] = tmp;
				j+=2;
				break;
			case 'y':
				tmp = Expr[j+s+2] - 0x30;
				if (tmp >= 0 && tmp <= 9) {
					if (tmp == 1) {
						tmp = 10 + Expr[j+s+3] - 0x30;
						if (tmp >= 10 && tmp <= 19) j++;
						else tmp = 1;
					};
					Exprn[pos] = ReBkTrace + tmp;
					j++;
					break;
				};
			default:
				tmp = Expr[j+s+1] - 0x30;
				if (tmp >= 0 && tmp <= 9) {
					if (tmp == 1) {
						tmp = 10 + Expr[j+s+2] - 0x30;
						if (tmp >= 10 && tmp <= 19) j++;
						else tmp = 1;
					};
					Exprn[pos] = ReBkBrack + tmp;
					break;
				} else
					Exprn[pos] = Expr[j+s+1];
				break;
			};
			j++;
			continue;
		};
		if (Expr[j+s] == ']') {
			Exprn[pos] = ReEnumE;
			if (EnterFg || !EnterGr) return false;
			EnterGr--;
		};
		if (Expr[j+s] == '-' && EnterGr) Exprn[pos] = ReFrToEnum;

		if (EnterGr) continue;

		if (Expr[j+s] == '[' && Expr[j+s+1] == '^') {
			Exprn[pos] = ReNEnumS;
			if (EnterFg) return false;
			EnterGr++;
			j++;
			continue;
		};
		if (Expr[j+s] == '*' && Expr[j+s+1] == '?') {
			Exprn[pos] = ReNGMul;
			j++;
			continue;
		};
		if (Expr[j+s] == '+' && Expr[j+s+1] == '?') {
			Exprn[pos] = ReNGPlus;
			j++;
			continue;
		};
		if (Expr[j+s] == '?' && Expr[j+s+1] == '?') {
			Exprn[pos] = ReNGQuest;
			j++;
			continue;
		};
		if (Expr[j+s] == '?' && Expr[j+s+1] == '#' &&
		        Expr[j+s+2]>='0' && Expr[j+s+2]<='9') {
			Exprn[pos] = ReBehind+Expr[j+s+2]-0x30;
			j+=2;
			continue;
		};
		if (Expr[j+s] == '?' && Expr[j+s+1] == '~' &&
		        Expr[j+s+2]>='0' && Expr[j+s+2]<='9') {
			Exprn[pos] = ReNBehind+Expr[j+s+2]-0x30;
			j+=2;
			continue;
		};
		if (Expr[j+s] == '?' && Expr[j+s+1] == '=') {
			Exprn[pos] = ReAhead;
			j++;
			continue;
		};
		if (Expr[j+s] == '?' && Expr[j+s+1] == '!') {
			Exprn[pos] = ReNAhead;
			j++;
			continue;
		};

		if (Expr[j+s] == '(') {
			Exprn[pos] = ReLBrack;
			if (EnterFg) return false;
			EnterBr++;
		};
		if (Expr[j+s] == ')') {
			Exprn[pos] = ReRBrack;
			if (!EnterBr || EnterFg) return false;
			EnterBr--;
		};
		if (Expr[j+s] == '[') {
			Exprn[pos] = ReEnumS;
			if (EnterFg) return false;
			EnterGr++;
		};
		if (Expr[j+s] == '{') {
			Exprn[pos] = ReRangeS;
			if (EnterFg) return false;
			EnterFg++;
		};
		if (Expr[j+s] == '}' && Expr[j+s+1] == '?') {
			Exprn[pos] = ReNGRangeE;
			if (!EnterFg) return false;
			EnterFg--;
			j++;
			continue;
		};
		if (Expr[j+s] == '}') {
			Exprn[pos] = ReRangeE;
			if (!EnterFg) return false;
			EnterFg--;
		};

		if (Expr[j+s] == '^') Exprn[pos] = ReSoL;
		if (Expr[j+s] == '$') Exprn[pos] = ReEoL;
		if (Expr[j+s] == '.') Exprn[pos] = ReAnyChr;
		if (Expr[j+s] == '*') Exprn[pos] = ReMul;
		if (Expr[j+s] == '+') Exprn[pos] = RePlus;
		if (Expr[j+s] == '?') Exprn[pos] = ReQuest;
		if (Expr[j+s] == '|') Exprn[pos] = ReOr;
	};
	if (EnterGr || EnterBr || EnterFg) return false;

	Info->Op = ReBrackets;
	Info->un.Param = new SRegInfo;
	Info->s = CurMatch++;

	if (!SetStructs(Info->un.Param,0,pos)) return false;
	Optimize();
	delete Exprn;
	return true;
};

void PosRegExp::Optimize() {
	PRegInfo Next = Info;
	FirstChar = 0;
	while(Next) {
		if (Next->Op == ReBrackets || Next->Op == RePlus  || Next->Op == ReNGPlus) {
			Next = Next->un.Param;
			continue;
		};
		if (Next->Op == ReSymb) {
			if (Next->un.Symb & 0xFF00 &&  Next->un.Symb != ReSoL && Next->un.Symb != ReWBound)
				break;
			FirstChar = Next->un.Symb;
			break;
		};
		break;
	};
};

bool PosRegExp::SetStructs(PRegInfo &re,int start,int end) {
	PRegInfo Next,Prev,Prev2;
	int comma,st,en,ng,i, j,k;
	int EnterBr;
	bool Add;

	if (end - start < 0) return false;
	Next = re;
	for (i = start; i < end; i++) {
		Add = false;
		// Ops
		if (Exprn[i] > ReBlockOps && Exprn[i] < ReSymbolOps) {
			Next->un.Param = 0;
			Next->Op = (EOps)Exprn[i];
			Add = true;
		};
		// {n,m}
		if (Exprn[i] == ReRangeS) {
			st = i;
			en = -1;
			comma = -1;
			ng = 0;
			for (j = i;j < end;j++) {
				if (Exprn[j] == ReNGRangeE) {
					en = j;
					ng = 1;
					break;
				};
				if (Exprn[j] == ReRangeE) {
					en = j;
					break;
				};
				if ((char)Exprn[j] == ',')
					comma = j;
			};
			if (en == -1) return false;
			if (comma == -1) comma = en;
			Next->s = (char)GetNumber(Exprn,st+1,comma);
			if (comma != en)
				Next->e = (char)GetNumber(Exprn,comma+1,en);
			else
				Next->e = Next->s;
			Next->un.Param = 0;
			Next->Op = ng?ReNGRangeNM:ReRangeNM;
			if (en-comma == 1) {
				Next->e = -1;
				Next->Op = ng?ReNGRangeN:ReRangeN;
			};
			i=j;
			Add = true;
		};
		// [] [^]
		if (Exprn[i] == ReEnumS || Exprn[i] == ReNEnumS) {
			Next->Op = (Exprn[i] == ReEnumS)?ReEnum:ReNEnum;
			for (j = i+1;j < end;j++) {
				if (Exprn[j] == ReEnumE)
					break;
			};
			if (j == end) return false;
			Next->un.ChrClass = new SCharData;
			memset(Next->un.ChrClass, 0, 32);
			for (j = i+1;Exprn[j] != ReEnumE;j++) {
				if (Exprn[j+1] == ReFrToEnum) {
					for (i = (Exprn[j]&0xFF); i < (Exprn[j+2]&0xFF);i++)
						Next->un.ChrClass->SetBit(i&0xFF);
					j++;
					continue;
				};
				switch(Exprn[j]) {
				case ReDigit:
					for (k = 0x30;k < 0x40;k++)
						if (IsDigit((char)k))
							Next->un.ChrClass->SetBit(k);
					break;
				case ReNDigit:
					for (k = 0x30;k < 0x40;k++)
						if (!IsDigit((char)k))
							Next->un.ChrClass->SetBit(k);
					Next->un.ChrClass->ClearBit(0x0a);
					Next->un.ChrClass->ClearBit(0x0d);
					break;
				case ReWordSymb:
					for (k = 0;k < 256;k++)
						if (IsWord((char)k))
							Next->un.ChrClass->SetBit(k);
					break;
				case ReNWordSymb:
					for (k = 0;k < 256;k++)
						if (!IsWord((char)k))
							Next->un.ChrClass->SetBit(k);
					Next->un.ChrClass->ClearBit(0x0a);
					Next->un.ChrClass->ClearBit(0x0d);
					break;
				case ReWSpace:
					Next->un.ChrClass->SetBit(0x20);
					Next->un.ChrClass->SetBit(0x09);
					break;
				case ReNWSpace:
					memset(Next->un.ChrClass->IArr, 0xFF, 32);
					Next->un.ChrClass->ClearBit(0x20);
					Next->un.ChrClass->ClearBit(0x09);
					Next->un.ChrClass->ClearBit(0x0a);
					Next->un.ChrClass->ClearBit(0x0d);
					break;
				default:
					if (!(Exprn[j]&0xFF00))
						Next->un.ChrClass->SetBit(Exprn[j]&0xFF);
					break;
				};
			};
			Add = true;
			i=j;
		};
		// ( ... )
		if (Exprn[i] == ReLBrack) {
			EnterBr = 1;
			for (j = i+1;j < end;j++) {
				if (Exprn[j] == ReLBrack) EnterBr++;
				if (Exprn[j] == ReRBrack) EnterBr--;
				if (!EnterBr) break;
			};
			if (EnterBr) return false;
			Next->Op = ReBrackets;
			Next->un.Param = new SRegInfo;
			Next->un.Param->Parent = Next;
			Next->s = CurMatch++;
			if (CurMatch > MatchesNum) CurMatch = MatchesNum;
			if (!SetStructs(Next->un.Param,i+1,j)) return false;
			Add = true;
			i=j;
		};
		if ((Exprn[i]&0xFF00) == ReBkTrace) {
			Next->Op = ReBkTrace;
			Next->un.Symb = Exprn[i]&0xFF;
			Add = true;
		};
		if ((Exprn[i]&0xFF00) == ReBkBrack) {
			Next->Op = ReBkBrack;
			Next->un.Symb = Exprn[i]&0xFF;
			Add = true;
		};
		if ((Exprn[i]&0xFF00) == ReBehind) {
			Next->Op = ReBehind;
			Next->s = Exprn[i]&0xFF;
			Add = true;
		};
		if ((Exprn[i]&0xFF00) == ReNBehind) {
			Next->Op = ReNBehind;
			Next->s = Exprn[i]&0xFF;
			Add = true;
		};
		// Chars
		if (Exprn[i] >= ReAnyChr && Exprn[i] < ReTemp || Exprn[i] < 0x100) {
			Next->Op = ReSymb;
			Next->un.Symb = Exprn[i];
			Add = true;
		};
		// Next
		if (Add && i != end-1) {
			Next->Next = new SRegInfo;
			Next->Next->Parent = Next->Parent;
			Next = Next->Next;
		};
	};
	Next = re;
	Prev = Prev2 = 0;
	while(Next) {
		if (Next->Op > ReBlockOps && Next->Op < ReSymbolOps) {
			if (!Prev) return false;
			if (!Prev2) re = Next;
			else Prev2->Next = Next;
			//if (Prev->Op > ReBlockOps && Prev->Op < ReSymbolOps) return false;
			Prev->Parent = Next;
			Prev->Next = 0;
			Next->un.Param = Prev;
			Prev = Prev2;
		};
		Prev2 = Prev;
		Prev = Next;
		Next = Next->Next;
	};

	return true;
};

/////////////////////////////////////////////////////////////////
/////////////////////////  Parsing  /////////////////////////////
/////////////////////////////////////////////////////////////////

bool PosRegExp::CheckSymb(int Symb,bool Inc) {
	bool Res;
	char ch;
	switch(Symb) {
	case ReAnyChr:
		if (posParse >= posEnd) return false;
		ch = CharAt(posParse,param);
		Res = ch != '\r' && ch != '\n';
		if (Res && Inc) posParse++;
		return Res;
	case ReSoL:
		if (posStart == posParse)
			return true;
		ch = CharAt(posParse-1,param);
		return ch == '\n' || ch == '\r';
	case ReEoL:
		if (posEnd == posParse)
			return true;
		ch = CharAt(posParse,param);
		return ch == '\n' || ch == '\r';
	case ReDigit:
		if (posParse >= posEnd) return false;
		ch = CharAt(posParse,param);
		Res = (ch >= 0x30 && ch <= 0x39);
		if (Res && Inc) posParse++;
		return Res;
	case ReNDigit:
		if (posParse >= posEnd) return false;
		ch = CharAt(posParse,param);
		Res = !(ch >= 0x30 && ch <= 0x39) && ch != '\r' && ch != '\n';
		if (Res && Inc) posParse++;
		return Res;
	case ReWordSymb:
		if (posParse >= posEnd) return false;
		Res = IsWord(CharAt(posParse,param));
		if (Res && Inc) posParse++;
		return Res;
	case ReNWordSymb:
		if (posParse >= posEnd) return false;
		ch = CharAt(posParse,param);
		Res = !IsWord(ch) && ch != '\r' && ch != '\n';
		if (Res && Inc) posParse++;
		return Res;
	case ReWSpace:
		if (posParse >= posEnd) return false;
		ch = CharAt(posParse,param);
		Res = (ch == 0x20 || ch == '\t');
		if (Res && Inc) posParse++;
		return Res;
	case ReNWSpace:
		if (posParse >= posEnd) return false;
		ch = CharAt(posParse,param);
		Res = !(ch == 0x20 || ch == '\t') && ch != '\r' && ch != '\n';
		if (Res && Inc) posParse++;
		return Res;
	case ReUCase:
		if (posParse >= posEnd) return false;
		Res = IsUpperCase(CharAt(posParse,param));
		if (Res && Inc) posParse++;
		return Res;
	case ReNUCase:
		if (posParse >= posEnd) return false;
		Res = IsLowerCase(CharAt(posParse,param));
		if (Res && Inc) posParse++;
		return Res;
	case ReWBound:
		if (posParse >= posEnd) return true;
		ch = CharAt(posParse,param);
		return IsWord(CharAt(posParse,param)) && (posParse == posStart || !IsWord(CharAt(posParse-1,param)));
	case ReNWBound:
		if (posParse >= posEnd) return true;
		return !IsWord(CharAt(posParse,param)) && IsWord(CharAt(posParse-1,param));
	case RePreNW:
		if (posParse >= posEnd) return true;
		return (posParse == posStart || !IsWord(CharAt(posParse-1,param)));
	case ReStart:
		Matches->s[0] = (posParse-posStart);
		return true;
	case ReEnd:
		Matches->e[0] = (posParse-posStart);
		return true;
	default:
		if ((Symb & 0xFF00) || posParse >= posEnd) return false;
		if (NoCase) {
			if (LowCase(CharAt(posParse,param)) != LowCase((char)Symb&0xFF)) return false;
		} else
			if (CharAt(posParse,param) != (char)(Symb&0xFF)) return false;
		if (Inc) posParse++;
		return true;
	};
}

bool PosRegExp::LowParseRe(PRegInfo &Next) {
	PRegInfo OrNext;
	int i,match,sv;
	int posStr;

	switch(Next->Op) {
	case ReSymb:
		if (!CheckSymb(Next->un.Symb,true)) return false;
		break;
	case ReEmpty:
		break;
	case ReBkTrace:
		if (!posBkStr | !BkTrace) return false;
		sv = Next->un.Symb;
		posStr = posParse;
		for (i = BkTrace->s[sv]; i < BkTrace->e[sv]; i++) {
			if (CharAt(posStr,param) != CharAt(posBkStr+i,param) || posEnd == posStr) return false;
			posStr++;
		};
		posParse = posStr;
		break;
	case ReBkBrack:
		sv = Next->un.Symb;
		posStr = posParse;
		if (Matches->s[sv] == -1 || Matches->e[sv] == -1) return false;
		for (i = Matches->s[sv]; i < Matches->e[sv]; i++) {
			if (CharAt(posStr,param) != CharAt(posStart+i,param) || posEnd == posStr) return false;
			posStr++;
		};
		posParse = posStr;
		break;
	case ReBehind:
		sv = Next->s;
		posStr = posParse;
		posParse -= sv;
		if (!LowParse(Next->un.Param)) return false;
		posParse = posStr;
		break;
	case ReNBehind:
		sv = Next->s;
		posStr = posParse;
		posParse -= sv;
		if (LowParse(Next->un.Param)) return false;
		posParse = posStr;
		break;
	case ReAhead:
		posStr = posParse;
		if (!LowParse(Next->un.Param)) return false;
		posParse = posStr;
		break;
	case ReNAhead:
		posStr = posParse;
		if (LowParse(Next->un.Param)) return false;
		posParse = posStr;
		break;
	case ReEnum:
		if (posParse >= posEnd) return false;
		if (!Next->un.ChrClass->GetBit(CharAt(posParse,param))) return false;
		posParse++;
		break;
	case ReNEnum:
		if (posParse >= posEnd) return false;
		if (Next->un.ChrClass->GetBit(CharAt(posParse,param))) return false;
		posParse++;
		break;
	case ReBrackets:
		match = Next->s;
		sv = posParse-posStart;
		posStr = posParse;
		if (LowParse(Next->un.Param)) {
			if (match || (Matches->s[match] == -1))
				Matches->s[match] = sv;
			if (match || (Matches->e[match] == -1))
				Matches->e[match] = posParse-posStart;
			return true;
		};
		posParse = posStr;
		return false;
	case ReMul:
		posStr = posParse;
		while (LowParse(Next->un.Param));
		while(!LowCheckNext(Next) && posStr < posParse) posParse--;
		break;
	case ReNGMul:
		do {
			if (LowCheckNext(Next)) break;
		} while (LowParse(Next->un.Param));
		break;
	case RePlus:
		posStr = posParse;
		match = false;
		while (LowParse(Next->un.Param))
			match = true;
		if (!match) return false;
		while(!LowCheckNext(Next) && posStr < posParse) posParse--;
		break;
	case ReNGPlus:
		if (!LowParse(Next->un.Param)) return false;
		do {
			if (LowCheckNext(Next)) break;
		} while (LowParse(Next->un.Param));
		break;
	case ReQuest:
		LowParse(Next->un.Param);
		break;
	case ReNGQuest:
		if (LowCheckNext(Next)) break;
		if (!LowParse(Next->un.Param)) return false;
		break;
	case ReOr:
		OrNext = Next;
		// posStr = posParse;
		if (LowParse(Next->un.Param)) {
			while (OrNext && OrNext->Op == ReOr)
				OrNext = OrNext->Next;
			/*if (!LowCheckNext(OrNext)){
			  posParse = posStr;
			  OrNext = Next;
		};*/
		};
		Next = OrNext;
		break;
	case ReRangeN:
		posStr = posParse;
		i = 0;
		while (LowParse(Next->un.Param)) i++; // ???
		do {
			if (i < Next->s) {
				posParse = posStr;
				return false;
			};
			i--;
		} while(!LowCheckNext(Next) && posStr < posParse--);
		break;
	case ReNGRangeN:
		posStr = posParse;
		i = 0;
		while (LowParse(Next->un.Param)) {
			i++;
			if (i >= Next->s && LowCheckNext(Next)) // ???
				break;
		};
		if (i < Next->s) {
			posParse = posStr;
			return false;
		};
		break;
	case ReRangeNM:
		posStr = posParse;
		i = 0;
		while (i < Next->s && LowParse(Next->un.Param)) // ???
			i++;
		if (i < Next->s) {
			posParse = posStr;
			return false;
		};
		while (i < Next->e && LowParse(Next->un.Param)) // ???
			i++;

		while(!LowCheckNext(Next)) {
			i--;
			posParse--;
			if (i < Next->s) {
				posParse = posStr;
				return false;
			};
		};
		break;
	case ReNGRangeNM:
		posStr = posParse;
		i = 0;
		while (i < Next->s && LowParse(Next->un.Param)) // ???
			i++;
		if (i < Next->s) {
			posParse = posStr;
			return false;
		};
		while(!LowCheckNext(Next)) {
			i++;
			if (!LowParse(Next->un.Param) || i > Next->e) { // ???
				posParse = posStr;
				return false;
			};
		};
		break;
	};
	return true;
};

bool PosRegExp::LowCheckNext(PRegInfo Re) {
	PRegInfo Next;
	int tmp = posParse;
	Next = Re;
	do {
		if (Next && Next->Op == ReOr)
			while (Next && Next->Op == ReOr)
				Next = Next->Next;
		if (Next->Next && !LowParse(Next->Next)) {
			posParse = tmp;
			Ok = false;
			return false;
		};
		Next = Next->Parent;
	} while(Next);
	posParse = tmp;
	if (Ok != false) Ok = true;
	return true;
};

bool PosRegExp::LowParse(PRegInfo Re) {
	while(Re && posParse <= posEnd) {
		if (!LowParseRe(Re)) return false;
		if (Re) Re = Re->Next;
	};
	return true;
};

bool PosRegExp::QuickCheck() {
	if (!NoMoves || !FirstChar)
		return true;
	switch(FirstChar) {
	case ReSoL:
		if (posParse != posStart) return false;
		return true;
	case ReWBound:
		return IsWord(CharAt(posParse,param)) && (posParse == posStart || !IsWord(CharAt(posParse-1,param)));
	default:
		if (NoCase && LowCase(CharAt(posParse,param)) != LowCase(FirstChar)) return false;
		if (!NoCase && CharAt(posParse,param) != (char)FirstChar) return false;
		return true;
	};
};

bool PosRegExp::ParseRe(int posStr) {
	if (Error) return false;

	posParse = posStr;
	if (!QuickCheck()) return false;

	for (int i = 0; i < MatchesNum; i++)
		Matches->s[i] = Matches->e[i] = -1;
	Matches->CurMatch = CurMatch;

	Ok = -1;
	//try{
	do {
		if (!LowParse(Info)) {
			if (NoMoves) return false;
		} else
			return true;
		posParse = ++posStr;
	} while(posParse != posEnd+1);
	return false;
	//}__except(){
	//  return true;
	//};
}
;

bool PosRegExp::Parse(int posStr,int posSol, int posEol, PMatches Mtch, int Moves) {
	if (!this) return false;

	bool s = NoMoves;
	if (Moves != -1) NoMoves = Moves!=0;
	posStart = posSol;
	posEnd   = posEol;
	Matches = Mtch;
	bool r = ParseRe(posStr);
	NoMoves = s;
	return r;
};

bool PosRegExp::Parse(int posStr, int posStop, PMatches Mtch) {
	if (!this) return false;
	posStart = posStr;
	posEnd = posStop;
	Matches = Mtch;
	return ParseRe(posStr);
};

bool PosRegExp::SetNoMoves(bool Moves) {
	NoMoves = Moves;
	return true;
};

bool PosRegExp::SetBkTrace(int posStr,PMatches Trace) {
	BkTrace = Trace;
	posBkStr = posStr;
	return true;
};

#define EVAL_MATCHES 16
#define EVAL_CHUNKSIZE 256

#define EVAL_LOWERCASE 1
#define EVAL_UPPERCASE 2
#define EVAL_LOWERCASE_NEXT 4
#define EVAL_UPPERCASE_NEXT 8

bool PosRegExp::Evaluate(char *Expr, int posStr, PMatches Mtch, char **Res) {
	int length,
	newlength,
	chunklength,
	value,
	size,
	src,
	end;
	unsigned flag;
	char ch,
	*dest,
	*pool;

	size = EVAL_CHUNKSIZE;
	pool = (char*) malloc (size);
	dest = pool;
	length = 0;
	flag = 0;
	while (*Expr) {
		switch (ch = *Expr++) {
		case '\\':
			switch (ch = *Expr++) {
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				ch -= ('A' - '0');
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				value = ch - '0';
				if (Mtch->s[value] != -1 && value < EVAL_MATCHES) {
					chunklength = Mtch->e[value] - Mtch->s[value];
					if (chunklength) {
						newlength = chunklength + length;
						if (newlength > size) {
							do
								size += EVAL_CHUNKSIZE;
							while (size < newlength);
							pool = (char*) realloc (pool, size);
							dest = pool + length;
						}
						length = newlength;
						src = posStr + Mtch->s[value];
						end = posStr + Mtch->e[value];
						if (flag & EVAL_UPPERCASE) {
							if (flag & EVAL_LOWERCASE_NEXT) {
								*dest++ = tolower (CharAt(src++,param));
								flag &= ~EVAL_LOWERCASE_NEXT;
							}
							while (src < end)
								*dest++ = toupper (CharAt(src++,param));
						} else if (flag & EVAL_LOWERCASE) {
							if (flag & EVAL_UPPERCASE_NEXT) {
								*dest++ = toupper (CharAt(src++,param));
								flag &= ~EVAL_UPPERCASE_NEXT;
							}
							while (src < end)
								*dest++ = tolower (CharAt(src++,param));
						} else {
							if (flag & EVAL_LOWERCASE_NEXT) {
								*dest++ = tolower (CharAt(src++,param));
								flag &= ~EVAL_LOWERCASE_NEXT;
							} else if (flag & EVAL_UPPERCASE_NEXT) {
								*dest++ = toupper (CharAt(src++,param));
								flag &= ~EVAL_UPPERCASE_NEXT;
							}
							while (src < end)
								*dest++ = CharAt(src++,param);
						}
					}
				} else
					goto error;
				continue;
			case '\0':
				goto error;
			case 'r':
				ch = '\r';
				break;
			case 'n':
				ch = '\n';
				break;
			case 'b':
				ch = '\b';
				break;
			case 'a':
				ch = '\a';
				break;
			case 't':
				ch = '\t';
				break;
			case 'U':
				flag |= EVAL_UPPERCASE;
				continue;
			case 'u':
				flag |= EVAL_UPPERCASE_NEXT;
				continue;
			case 'L':
				flag |= EVAL_LOWERCASE;
				continue;
			case 'l':
				flag |= EVAL_LOWERCASE_NEXT;
				continue;
			case 'Q':
			case 'q':
				flag &= ~(EVAL_UPPERCASE | EVAL_LOWERCASE);
				continue;
			case 'x':
				{
					if (!*Expr)
						goto error;
					value = toupper (*Expr) - '0';
					if (value > 9)
						value = value + '0' - 'A' + 10;
					if (value > 15)
						goto error;
					ch = value << 4;
					Expr++;
					if (!*Expr)
						goto error;
					value = toupper (*Expr) - '0';
					if (value > 9)
						value = value + '0' - 'A' + 10;
					if (value > 15)
						goto error;
					Expr++;
					ch |= value;
					break;
				}
			case 'd':
				{
					if (!*Expr)
						goto error;
					value = toupper (*Expr) - '0';
					if (value > 9)
						goto error;
					ch = value * 100;
					Expr++;
					if (!*Expr)
						goto error;
					value = toupper (*Expr) - '0';
					if (value > 9)
						goto error;
					ch += value * 10;
					Expr++;
					if (!*Expr)
						goto error;
					value = toupper (*Expr) - '0';
					if (value > 9)
						goto error;
					ch += value;
					Expr++;
					break;
				}
			case 'o':
				{
					if (!*Expr)
						goto error;
					value = toupper (*Expr) - '0';
					if (value > 9)
						goto error;
					ch = value << 6;
					Expr++;
					if (!*Expr)
						goto error;
					value = toupper (*Expr) - '0';
					if (value > 9)
						goto error;
					ch += value << 3;
					Expr++;
					if (!*Expr)
						goto error;
					value = toupper (*Expr) - '0';
					if (value > 9)
						goto error;
					ch |= value;
					Expr++;
					/* break; */
				}
				/* default:
					break; */
			}
		default:
			if (++length > size) {
				do
					size += EVAL_CHUNKSIZE;
				while (size < length);
				pool = (char*) realloc (pool, size);
				dest = pool + length - 1;
			}
			if (flag & EVAL_LOWERCASE_NEXT) {
				*dest++ = tolower (ch);
				flag &= ~EVAL_LOWERCASE_NEXT;
			} else if (flag & EVAL_UPPERCASE_NEXT) {
				*dest++ = toupper (ch);
				flag &= ~EVAL_UPPERCASE_NEXT;
			} else if (flag & EVAL_UPPERCASE)
				*dest++ = toupper (ch);
			else if (flag & EVAL_LOWERCASE)
				*dest++ = tolower (ch);
			else
				*dest++ = ch;
		}
	}
	if (++length > size) {
		do
			size += EVAL_CHUNKSIZE;
		while (size < length);
		pool = (char*) realloc (pool, size);
		dest = pool + length - 1;
	}
	*dest = '\0';
	*Res = pool;
	return true;
error:
	free (pool);
	return false;
}
