/*******************************************************************************
 *
 * Autor:			Joachim Raidl
 * Adresse:			Joachim.Raidl@iname.com
 * Klasse:			XString
 * Files:			XString.cpp, XString.h
 * Beschreibung:	Die Klasse XString stellt eine Erweiterung der Klasse
 *					CString dar. Neben den Standardfunktionen werden zusätzlich
 *					Funktionen für Element-Strings zur Verfügung gestellt.
 *					Elementstrings zeichnen sich dadurch aus, daß sie mehrere
 *					Werte durch einen Separator getrennt enthalten. So kann zum
 *					Beispiel aus dem Elementstring "012|345|678|901" jedes
 *					Element für sich gelesen und überschrieben werden.
 * Operatoren:		operator char*, operator const char*, operator CString,
 *					operator--, operator!
 * Funktionen:		Double(), Int(), Remove(), RemoveAll(), Insert(), Replace(),
 *					ReplaceAll(), Reverse(), Trim(), Lower(), Upper(), Fill(),
 *					ReplaceTabs(), Count(), FindIndex(), ReplaceText(),
 *					NCFind(), NCReplace(), NCReplaceAll(), NCRemove(),
 *					NCRemoveAll(),
 * Element-Fkt.:	SetSeparator(), RestoreSeparator(), Element(), operator(),
 *					Elements(), GetString(), GetDouble(), GetInt(), GetChar(),
 *					Get(), SetElement(), ElementIndex(), ElementSortString(),
 *					ElementSortDouble(), ElementSortChar(), ElementSortInt(),
 *					ElementSort(), SerializeElements(),
 * Friends:			itoa(), utoa(), dtoa(), Repeat(), Concat(),
 *					GetStringTableEntry(), GetDoubleFormatString(),
 *
 ******************************************************************************/
// XString.cpp: implementation of the XString class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "XString.h"
#include <CType.H>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//remove function
XString	XString::Remove(int von, int bis) const
{
	if (bis < 0)					bis = von;

	if ((von < 0) || (bis < von))	return *this;
	else							return Left(von) + Mid(bis);
}
XString	XString::Remove(const XString& s) const
{
	int i = Find(s);

	if (i >= 0)	return Left(i) + Mid(i + s.GetLength());
	else		return *this;
}
XString	XString::Remove(char c) const
{
	int i = Find(c);

	if (i >= 0)	return Left(i) + Mid(i+1);
	else		return *this;
}

XString	XString::RemoveAll(const XString& s) const
{
	XString hilf = *this;

	while (hilf.Find(s) >= 0)	hilf = hilf.Remove(s);
	return hilf;
}
XString	XString::RemoveAll(char c) const
{
	XString hilf = *this;

	while (hilf.Find(c) >= 0)	hilf = hilf.Remove(c);
	return hilf;
}

XString	XString::NCRemove(const XString& s) const
{
	int i = NCFind(s);

	if (i >= 0)	return Left(i) + Mid(i + s.GetLength());
	else		return *this;
}

XString	XString::NCRemove(char c) const
{
	int i = NCFind(c);

	if (i >= 0)	return Left(i) + Mid(i+1);
	else		return *this;
}
//insert function
XString XString::Insert(const XString& s, int at) const
{
	if ((at <= GetLength()) && (at >= 0))	return Left(at) + s + Mid(at);
	else									return *this;
}
//replace function
XString XString::Replace(const XString& s, const XString& search) const
{
	int i = Find(search);
	return Remove(search).Insert(s, i);
}

XString	XString::Replace(const XString& s, int von, int bis) const
{
	return Remove(von, bis).Insert(s, von);
}

XString XString::NCReplace(const XString& s, const XString& search) const
{
	int i = NCFind(search);
	return NCRemove(search).Insert(s, i);
}

int XString::ReplaceAll(const XString& s, const XString& search)
{
	XString aktString = *this;
	this->Empty();

	int len = search.GetLength();
	for (int pos = aktString.Find(search), i = 0; pos >= 0; pos = aktString.Find(search))
	{
		*this		+= aktString.Left(pos) + s;
		aktString	= aktString.Mid(pos + len);
		i++;
	}

	*this += aktString;

	return i;
}
//find 
int XString::NCFind(const XString& s) const
{
	XString copy = this->Upper();

	return copy.Find(s.Upper());
}

int XString::NCFind(char c) const
{
	XString copy = this->Upper();

	return copy.Find((char) toupper(c));
}
//count function
int XString::Count(const XString& str) const
{
	int anz = 0, p, l = str.GetLength();
	XString copy = *this;
	while ((p = copy.Find(str)) >= 0)
	{
		anz++;
		copy = copy.Mid(p + l);
	}

	return anz;
}

int XString::Count(char c) const
{
	int anz = 0;
	for (int i = 0, l = GetLength(); i < l; i++)
		if (m_pchData[i] == c) anz++;

	return anz;
}

int XString::FindIndex(char c, int pos) const
{
	if (!pos)	return 0;

	for (int i=0, l=GetLength(); (i < l) && pos; i++)
		if (m_pchData[i] == c) pos--;

	int erg;
	if (!pos)	erg = i-1;
	else		erg = -1;

	return erg;
}
//format º¯Êý
void AFX_CDECL XString::Format(LPCTSTR lpszFormat, ...)
{
	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	FormatV(lpszFormat, argList);
	va_end(argList);
}
XString XString::ReplaceText(const XString& replace) const
{
	XString erg = *this;
	XString rep;
	for (int i = 0; !!(rep = replace.GetString(i)); i++)
		if (erg.Find("*") >= 0)
			erg = erg.Replace(rep, "*");

	erg.ReplaceAll("*", "§"");

	return erg;
}
/*****************************************************************************
 * Klasse:		XString
 * Funktion:	Fill
 * Parameter:	n			Neue Länge
 * Return:		Ergebnisstring
 * Beipsiel:	XString("12345").Fill(10)	==> "12345     "
 *
 * Füllt den String mit Leerzeichen auf, sodaß er aus n Zeichen besteht.
 ****************************************************************************/
XString XString::Fill(int n) const
{
	XString erg = *this;
	for (int i = GetLength(); i < n; i++) erg += ' ';

	return erg;
}

/*****************************************************************************
 * Klasse:		XString
 * Funktion:	ReplaceTabs
 * Parameter:	abstand			Tablatorabstand
 * Return:		String ohne Tabulatorzeichen
 * Beispiel:	str = "Blank\tEinfügen";
 *				ReplaceTabs(str, 4)		==>	"Blank   einfügen"
 *
 * Ersetzt alle Tabulatoren innerhalb des Strings durch entsprechend viele
 * Leerzeichen und gibt diesen dann zurück. Hierbei werden die Leerzeichen
 * so eingefügt, daß die Position des darauffolgenden Textes mit der
 * vorherigen übereinstimmt. Bei einem Tabulatorabstand von 4 kann ein Text,
 * der einem Tabulatorzeichen folgt immer nur bei der Position 4, 8, 12, ...
 * erscheinen.
 ****************************************************************************/
XString XString::ReplaceTabs(int abstand) const
{
	XString erg = *this;
	int i;
	while ((i = erg.Find('\t')) >= 0)
		erg = erg.Left(i) + Repeat(' ', abstand - i % abstand) + erg.Mid(i + 1);

	return erg;
}

/*****************************************************************************
 * Klasse:		XString
 * Funktion:	operator(), Element, Elements
 * Parameter:	i			Position
 *				von, bis	Bereich
 *				sep			Separator
 * Return:		i-tes Element bzw. (von-bis)-Elemente des Strings
 * Beispiele:	XString str	= "0|1|2|3"
 * 				str(1)			= str.Element(1)	==> "1"
 *				str(5)			= str.GetString(5)	==> ""
 *				str.GetDouble(1)					==> 1.0
 *				str.GetInt(0)						==> 0
 *				str.GetChar(3)						==> '3'
 * 				str(1, 2)		= str.Elements(1,2)	==> "1|2" //add by yuwc---´ÓÆðÊ¼Öµµ½Ö¸¶¨µÄ½áÎ²Öµ
 *				str(2, -1)		= str.Elements(2)	==> "2|3" //´ÓÆðÊ¼Öµµ½×Ö·û´®µÄ×îºó,
 *				XString("0, 1, 2, 3").Element(2,',')	==> "2" //Ö¸¶¨ÁËÌØÊâµÄ·Ö¸î×Ö·û.
 *
 * Der Funktions-Operator '()' gibt ein Element eines Element-Strings zurück.
 * Ein Element-String ist ein String mit mehreren durch ein Separator getrennte
 * Elemente. Je nach Übergabeparameter kann ein einzelnes Element oder eine
 * Teil-Elementstring zurückgegeben werden. Es existieren mehrere Overloads für
 * diese Funktion:
 *
 *		(i)				i-tes Element als String
 *		Element(i)		i-tes Element als String
 *		Element(i, c)	i-tes Element. Als Trenner wird c benutzt
 *		(i, j)			Elementstring mit den Elementen i bis j
 *		Elements(i, j)	Elementstring mit den Elementen i bis j
 *		(i, -1)			Elementstring mit allen Elementen ab dem i-ten Element
 *		Elements(i, -1)	Elementstring mit allen Elementen ab dem i-ten Element
 *
 *		String(i)		i-tes Element als String
 *		Double(i)		i-tes Element als Double
 *		Int(i)			i-tes Element als Int
 *		Char(i)			i-tes Element als Char
 ****************************************************************************/
XString XString::Element(int i, char sep) const
{
	XString erg = Elements(i, i,sep);
	return erg;
}

XString XString::Elements(int von, int bis) const
{
	return Elements(von,bis,'|');
}

XString XString::Elements(int von, int bis, char sep) const
{
	/*EnterCriticalSection(&XString::m_csLock);
	char tempSep = SEP;
	SEP = sep;
	XString erg = Elements(von, bis);
	SEP = tempSep;
	LeaveCriticalSection(&XString::m_csLock);
	return erg;*/
	char SEP=sep;
	if (bis < 0) bis = Count(SEP);	// Rest ab von
	bis += 1 - von;					// bis = bis - von + 1

	if (von <  0) return "";		// Falscher Parameter von
	if (bis <  0) return "";		// Falscher Parameter bis < von 

	int	len = GetLength();			// Länge des aktuellen Strings
	XString copy = *this;			// Copy des aktuellen Strings

	for (int v = 0;	(v < len) && von; v++) if (copy[v] == SEP) von--;
	for (int b = v;	(b < len) && bis; b++) if (copy[b] == SEP) bis--;
	if ((b > 0) && !((b == len) && bis)) b--;

	XString erg;
	if (v >= len)		erg = Repeat(SEP, --bis);
	else if (b == 0)	erg = "";
	else if (b < len)	erg = copy.Mid(v, b - v); 
	else				erg = copy.Mid(v) + Repeat(SEP, --bis);

	return erg;
}

/*****************************************************************************
 * Klasse:		XString
 * Funktion:	SetElement
 * Parameter:	pos			Suchposition
 *				rep			Ersatzstring, -int, -double
 * Return:		String mit ersetzem Wert
 * Beipsiel:	str = "1|2|3"
 *				str.SetElement(1, "99")	==> "1|99|3"
 *				str.SetElement(5, "99")	==> "1|2|3|||99"
 *				str.SetElement(1)		==> "1||3"
 *				str.SetElement(2, 75)	==> "1|2|75"
 *				str.SetElement(1, 1.23)	==> "1|1.23|3"
 *
 * Ersetzt in einem Elemente-String den i-ten Eintrag. Wenn der Eintrag nicht
 * existiert, wird er automatisch generiert. Defaultmäßig wird der Replace-
 * String durch "" repäsentiert, was einem Löschen des i-ten Elements ent-
 * spricht. Alternativ kann auch ein double- oder int-Wert übergeben werden.
 ****************************************************************************/
XString XString::SetElement(int pos, const XString& rep) const
{
	char SEP = '|';
	if (pos < 0)	return *this;

	int n = GetLength();
	int s = Count(SEP);

	if (s < pos) return (*this + Repeat(SEP, pos - s) + rep);

	for (int von = 0;	pos;	 von++)	if (m_pchData[von] == SEP) pos--;
	for (int bis = von; bis < n; bis++)	if (m_pchData[bis] == SEP) break;

	if (bis < von)	return Insert(rep, von);
	else			return Replace(rep, von, bis);
}

/*****************************************************************************
 * Klasse:		XString
 * Funktion:	ElementIndex
 * Parameter:	such		Suchstring
 * Return:		Elementposition des Suchstrings, wenn er gefunden wurde,
 *				-1 sonst
 * Beipsiel:	str = "0|1|2|3"
 *	 			str.ElementIndex("2")	==> 2
 *				str.ElementIndex("5")	==> -1
 *				str.ElementIndex(1.0)	==> 1
 *				str.ElementIndex(3)		==> 3
 *
 * Liefert den Index eines Eintrags in einer Element-Liste zurück. Bei der
 * Übergabe von int- oder double-Werten, werden die Werte überprüft und nicht
 * die Strings. So wird der Eintrag "3.0" auch dann gefunden, wenn nach 3
 * gesucht wird.
 ****************************************************************************/
int XString::ElementIndex(const XString& such) const
{
	char SEP = '|';
	for (int j=0; j<=Count(SEP); j++) if (GetString(j) == such) return j;
	return -1;
}

int XString::ElementIndex(double such) const
{
	char SEP = '|';
	for (int j=0; j<=Count(SEP); j++) if (GetDouble(j) == such) return j;
	return -1;
}

int XString::ElementIndex(int such) const
{
	char SEP = '|';
	for (int j=0; j<=Count(SEP); j++) if (GetInt(j) == such) return j;
	return -1;
}

int XString::ElementIndex(char such) const
{
	char SEP = '|';
	for (int j=0; j<=Count(SEP); j++) if (GetChar(j) == such) return j;
	return -1;
}

int XString::ElementIndex(bool such) const
{
	char SEP = '|';
	for (int j=0; j<=Count(SEP); j++) if (GetBool(j) == such) return j;
	return -1;
}

//format function
//make by wangqd
XString itoa(int i)				{ XString h; h.Format("%d", i); return h; }
XString utoa(UINT u)			{ XString h; h.Format("%u", u); return h; }
XString dtoa(double d)			{ XString h; h.Format("%g", d); return h; }
XString dtoa(double d, int n)	{ XString h; h.Format("%.*f", n, d); return h; }

/*****************************************************************************
 * Klasse:		- (friend XString)
 * Funktion:	Repeat
 * Parameter:	c		Zu wiederholendes Zeichen
 *				str		Zu wiederholende Zeichenkette
 *				n		Anzahl der Wiederholungen
 * Return:		Formatierter String
 * Beipsiel:	Repeat('*', 5)	==> "*****"
 *				Repeat('#', 0)	==> ""
 *				Repeat("01", 5)	==> "0101010101"
 *
 * Erzeugt einen String aus mehreren Wiederholungen eines Zeichens oder
 * eines Strings.
 ****************************************************************************/
XString Repeat(char c, int n)
{
	XString erg(c, n);

	return erg;
}

XString Repeat(const XString& str, int n)
{
	for (XString ret; n > 0; n--)
		ret += str;

	return ret;
}

//ÓÃÓÚ´¦Àí "df -k" µÈÇé¿ö
//ÓÃxNewÌæ»»µôÈ«²¿cOld£¬Èç¹ûÓÐ2¸öcOldÁ¬ÔÚÒ»Æð£¬ÔòÖ»»»³ÉÒ»¸öxNew.
//Ê¹ÓÃÌõ¼þ:¿ªÍ·ºÍ½áÎ²²»¿ÉÓÐcOld¡£
//·µ»ØÌæ»»ºóxNewµÄ¸öÊý
int XString::ReplaceAllConc(const char xNew , const char cOld )
{
	XString myStr=*this;
	this->Empty();

	XString tmpStr;
	tmpStr.Empty();
	
	int kk=0;
	char ch;
	int myStrLen=myStr.GetLength();
	for(int i=0;i<myStrLen;i++)
	{
		ch=myStr.GetAt(i);
		if(ch==cOld)
		{	//Èç¹ûmyStrµÄµ±Ç°Î»ÖÃÊÇcOld,Ôò½«xNew¿½±´¸øtmpStr,
			//Èç¹û´ËÊ±tmpStrµÄ×îºóÒ»¸ö×Ö·ûÒÑ¾­ÊÇxNew£¬Ôò²»×ö´Ë²Ù×÷
			if(tmpStr.GetLength()<2)
			{	//Èç¹û²Å¿ªÊ¼¿¼ÂÇtmpStrµÄµÚ1¸ö×Ö·û,Ôò²»¿ÉÒÔÈ¡µ¹ÊýµÚ2¸ö×Ö·û
				tmpStr+=ch;
			}
			else
			{	//tmpStrµÄ³¤¶ÈÒÑ¾­´óÓÚ1ÁË¡£
				
				//¿ÉÒÔ¿¼ÂÇµ½ÊýµÚ2¸ö×Ö·ûÊÇ²»ÊÇÒÑ¾­ÊÇ xNew ÁË¡£
				if(tmpStr.GetAt(strlen(tmpStr)-1)==xNew)
				{	//Èç¹ûÒÑ¾­ÊÇxNewÁË£¬Ôò²»¼ÓÈëxNew.
				}
				else
				{	//Èç¹û²»ÊÇxNew£¬Ôò¼ÓÈëxNew.
					tmpStr+=xNew;
					kk++;
				}
			}
		}
		else
		{	//Èç¹ûmyStrµÄµ±Ç°Î»ÖÃ²»ÊÇcOld£¬Ôò½«µ±Ç°Î»ÖÃ¿½¸øtmpStr
			tmpStr+=ch;
		}

	}//End of for(;;)

	*this = tmpStr;
	return kk;
}
/*****************************************************************************
 * Klasse:		- (friend XString)
 * Funktion:	GetStringTableEntry
 * Parameter:	id		ID des Strings in der String-Table
 * Return:		Eintrag in der String-Table
 *
 * Mit dieser Funktion wird ein Eintrag aus der String-Table gelesen und
 * zurückgegeben
 ****************************************************************************/
XString GetStringTableEntry(UINT id)
{
	XString ret;
	ret.LoadString(id);
	return ret;
}

XString Concat(XS& a,XS& b)
	{ return (a+"|"+b); }

XString Concat(XS& a,XS& b,XS& c)
	{ return (a+"|"+b+"|"+c); }

XString Concat(XS& a,XS& b,XS& c,XS& d)
	{ return (a+"|"+b+"|"+c+"|"+d); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g,XS& h)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g+"|"+h); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g,XS& h,XS& i)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g+"|"+h+"|"+i); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g,XS& h,XS& i,XS& j)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g+"|"+h+"|"+i+"|"+j); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g,XS& h,XS& i,XS& j,XS& k)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g+"|"+h+"|"+i+"|"+j+"|"+k); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g,XS& h,XS& i,XS& j,XS& k,XS& l)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g+"|"+h+"|"+i+"|"+j+"|"+k+"|"+l); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g,XS& h,XS& i,XS& j,XS& k,XS& l,XS& m)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g+"|"+h+"|"+i+"|"+j+"|"+k+"|"+l+"|"+m); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g,XS& h,XS& i,XS& j,XS& k,XS& l,XS& m,XS& n)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g+"|"+h+"|"+i+"|"+j+"|"+k+"|"+l+"|"+m+"|"+n); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g,XS& h,XS& i,XS& j,XS& k,XS& l,XS& m,XS& n,XS& o)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g+"|"+h+"|"+i+"|"+j+"|"+k+"|"+l+"|"+m+"|"+n+"|"+o); }

XString Concat(XS& a,XS& b,XS& c,XS& d,XS& e,XS& f,XS& g,XS& h,XS& i,XS& j,XS& k,XS& l,XS& m,XS& n,XS& o,XS& p)
	{ return (a+"|"+b+"|"+c+"|"+d+"|"+e+"|"+f+"|"+g+"|"+h+"|"+i+"|"+j+"|"+k+"|"+l+"|"+m+"|"+n+"|"+o+"|"+p); }