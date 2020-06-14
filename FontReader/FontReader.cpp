
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include "FontAnalyze.h"

using namespace std;

void PrintCharactor(unsigned int iTag)
{
	unsigned char lch = iTag >> 24;
	cout << lch;
	lch = (iTag >> 16) & 0x00FF;
	cout << lch;
	lch = (iTag >> 8) & 0x00FF;
	cout << lch;
	lch = iTag & 0x00FF;
	cout << lch << endl;
}

bool IsEqual(unsigned int iTag, const string& irStr)
{
	if (irStr.length() != 4)
		return false;

	unsigned char a = iTag >> 24;
	unsigned char b = (iTag >> 16) & 0x00FF;
	unsigned char c = (iTag >> 8) & 0x00FF;
	unsigned char d = iTag & 0x00FF;

	return a == irStr[0] && b == irStr[1] && c == irStr[2] && d == irStr[3];
}

void ProcessTable(FontAnalyze& iFontAnalyze, unsigned int iTableTag)
{
	if (IsEqual(iTableTag, "cmap"))
	{
		iFontAnalyze.PrintInforFromCMAP();
	}
	else if (IsEqual(iTableTag, "glyf"))
	{

	}
	else if (IsEqual(iTableTag, "head"))
	{
		iFontAnalyze.PrintInforFromHEAD();
	}
	else if (IsEqual(iTableTag, "hhea"))
	{
		iFontAnalyze.PrintInforFromHHEA();
	}
	else if (IsEqual(iTableTag, "hmtx"))
	{

	}
	else if (IsEqual(iTableTag, "loca"))
	{
		iFontAnalyze.PrintInforFromLOCA();
	}
	else if (IsEqual(iTableTag, "maxp"))
	{
		iFontAnalyze.PrintInforFromMAXP();
	}
	else if (IsEqual(iTableTag, "name"))
	{
		iFontAnalyze.PrintInforFromNAME();
	}
	else if (IsEqual(iTableTag, "post"))
	{
		iFontAnalyze.PrintInforFromPOST();
	}
}

int main()
{
//	system("mode con:cols=100 lines=1000");
	string lFontPath = "C:/Users/Administrator/Desktop/TestFont/times.ttf";

	FontAnalyze lFontAnalyze(lFontPath);
	short lCurData = 0;

	lFontAnalyze.MoveByOffset(4);
	unsigned short lNumTables = 0;
	lFontAnalyze.Read16(lNumTables);
	cout << "The number of total tables: " << lNumTables << endl;
	lFontAnalyze.MoveByOffset(6);

	for (auto i = 0; i < lNumTables; ++i)
	{
		cout << dec << i + 1 << "  ";

		unsigned int lTag = 0;
		lFontAnalyze.Read32(lTag);

		PrintCharactor(lTag);

		ProcessTable(lFontAnalyze, lTag);

		lFontAnalyze.MoveByOffset(12);
	}
	
 	return 0;
}