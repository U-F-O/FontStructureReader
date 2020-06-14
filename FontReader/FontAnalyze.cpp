#include "FontAnalyze.h"
#include <fstream>
#include <iostream>
#include <iomanip>



//#define PRINT
#ifdef PRINT
#define  PRINT_WITH_HEX_UNSIGNED(VAR, FORMAT) cout << hex << setw(2) << setfill('0') << static_cast<unsigned FORMAT>(VAR) << " ";
#else
#define  PRINT_WITH_HEX_UNSIGNED(VAR, FORMAT) 
#endif

FontAnalyze::FontAnalyze(const string& irPath)
{
	ifstream lFontFile(irPath, ios::in | ios::binary | ios::ate);
	if (!lFontFile)
		cout << "error" << endl;

	ifstream::pos_type size = lFontFile.tellg();
	cout << "Size of file: " << size << "  byte" <<endl;

	lFontFile.seekg(0, ios::beg);

	__FontData = new char[static_cast<int>(size)];
	lFontFile.read(__FontData, size);
	//__FontData[size] = '\0';

	lFontFile.close();

	__Row = 0;
	__Col = 0;

	__Cur = __FontData;

	__ShortVersionForLOCATable = false;
}

void FontAnalyze::ResetCurPtr()
{
	__Cur = __FontData;
}

void FontAnalyze::Read32(unsigned int& o32, bool ibMoveCur, bool ibBasedOnDataBegin, unsigned int iOffset)
{
	char* lp = NULL;

	if (ibBasedOnDataBegin)
		lp = __FontData;
	else
		lp = __Cur;

	unsigned char a = *(lp + iOffset);
	unsigned char b = *(lp + iOffset + 1);
	unsigned char c = *(lp + iOffset + 2);
	unsigned char d = *(lp + iOffset + 3);
	
	cout.setf(ios::uppercase);
	PRINT_WITH_HEX_UNSIGNED(a, int)
	PRINT_WITH_HEX_UNSIGNED(b, int)
    PRINT_WITH_HEX_UNSIGNED(c, int)
    PRINT_WITH_HEX_UNSIGNED(d, int)

	o32 = (a << 24) | (b << 16) | (c << 8) | d;
	if(ibMoveCur)
	    __Cur += 4;

	hCalculateCurPos();
}

void FontAnalyze::Read16(unsigned short& o16, bool ibMoveCur, bool ibBasedOnDataBegin, unsigned int iOffset)
{
	char* lp = NULL;

	if (ibBasedOnDataBegin)
		lp = __FontData;
	else
		lp = __Cur;

	unsigned char a = *(lp + iOffset);
	unsigned char b = *(lp + iOffset + 1);

	cout.setf(ios::uppercase);
	PRINT_WITH_HEX_UNSIGNED(a, short)
	PRINT_WITH_HEX_UNSIGNED(b, short)

	o16 =  a << 8 | b;
	if (ibMoveCur)
	    __Cur += 2;

	if (__Cur - __FontData == 16 && !ibMoveCur)
		cout << endl;

	hCalculateCurPos();
}

void FontAnalyze::MoveByOffset(unsigned int iOffset)
{
	__Cur += iOffset;
}

void FontAnalyze::hCalculateCurPos()
{
	unsigned __int64 lCurLen = __Cur - __FontData;
	__Row = lCurLen / 16 + 1;
	__Col = lCurLen % 16 + 1;
}

void FontAnalyze::PrintInforFromCMAP()
{
	unsigned int lCMapHeaderOffset = 0;
	unsigned int lLength = 0;
	Read32(lCMapHeaderOffset, false, false, 4);
	Read32(lLength, false, false, 8);

	unsigned short lVersion = 0;
	unsigned short lNumberOfSubtables = 0;
	Read16(lVersion, false, true, lCMapHeaderOffset);
	Read16(lNumberOfSubtables, false, true, lCMapHeaderOffset + 2);

	cout << "The version of cmap is " << lVersion << endl;
	cout << "The number of the subtables is " << lNumberOfSubtables << endl;

	for (auto i = 0; i < lNumberOfSubtables; ++i)
	{
		unsigned short lPlatformID = 0;
		unsigned short lPlatformSpecificID = 0;
		unsigned int lSubtableOffset = 0;

		Read16(lPlatformID, false, true, lCMapHeaderOffset + 4 + i * 8);
		Read16(lPlatformSpecificID, false, true, lCMapHeaderOffset + 4 + i * 8 + 2);
		Read32(lSubtableOffset, false, true, lCMapHeaderOffset + 4 + i * 8 + 4);

		unsigned short lFormatNum = 0;
		Read16(lFormatNum, false, true, lCMapHeaderOffset + lSubtableOffset);
		// Find glyphIdArray according to format number
		// eg: format 0 -> offset 6  format 4 -> offset 24

		//unsigned short lLanguageFormat = 0;
		//Read16(lLanguageFormat, false, true, lCMapHeaderOffset + lSubtableOffset + 4);

		cout << "PlatformID: " << lPlatformID<< ", PlatformSpecificID: " << lPlatformSpecificID << ", Format:" << lFormatNum << endl;
	}
}

void FontAnalyze::PrintInforFromHEAD()
{
	unsigned int lFontHeaderTableOffset = 0;
	unsigned int lLength = 0;
	Read32(lFontHeaderTableOffset, false, false, 4);
	Read32(lLength, false, false, 8);

	unsigned int lVersion = 0;
	Read32(lVersion, false, true, lFontHeaderTableOffset);
	cout << "Version: " << hex << setfill('0') << setw(8)<< static_cast<unsigned int>(lVersion) << endl;

	unsigned int lMagicNumber = 0;
	Read32(lMagicNumber, false, true, lFontHeaderTableOffset + 12);
	// used to confirm whether it is loaded expected.
	cout << "Font Header Table Magic Number: " << hex << setfill('0') << static_cast<unsigned int>(lMagicNumber) << endl;

	unsigned short lFormatVersionForLOCATable = 0;
	Read16(lFormatVersionForLOCATable, false, true, lFontHeaderTableOffset + 50);
	__ShortVersionForLOCATable = lFormatVersionForLOCATable == 0;
	cout << "Format Version for LOCA table: " << static_cast<unsigned int>(lFormatVersionForLOCATable) << endl;
}

void FontAnalyze::PrintInforFromHHEA()
{
	unsigned int lHHEATableOffset = 0;
	unsigned int lLength = 0;
	Read32(lHHEATableOffset, false, false, 4);
	Read32(lLength, false, false, 8);

	unsigned int lVersion = 0;
	Read32(lVersion, false, true, lHHEATableOffset);
	cout << "Version: " << hex << setfill('0') << setw(8) << static_cast<unsigned int>(lVersion) << endl;

	

	//waiting for more information
}

void FontAnalyze::PrintInforFromLOCA()
{
	unsigned int lLOCAHeaderTableOffset = 0;
	unsigned int lLength = 0;
	Read32(lLOCAHeaderTableOffset, false, false, 4);
	Read32(lLength, false, false, 8);

	// Currently print missing character of which the number is zero.
	unsigned int lMissingCharacterOffset = 0;
	if (__ShortVersionForLOCATable)
	{
		unsigned short lTempMissingCharacterOffset = 0;
		Read16(lTempMissingCharacterOffset, false, true, lLOCAHeaderTableOffset);
		lMissingCharacterOffset = static_cast<unsigned int>(lTempMissingCharacterOffset) * 2;
	}
	else
	{
		unsigned int lTempMissingCharacterOffset = 0;
		Read32(lTempMissingCharacterOffset, false, true, lLOCAHeaderTableOffset);
		lMissingCharacterOffset = lTempMissingCharacterOffset;
	}
	cout << "Missing Character the offsets to the locations of the glyphs in the font, relative to the beginning of the glyphData table "<< lMissingCharacterOffset <<endl;
}

void FontAnalyze::PrintInforFromMAXP()
{
	unsigned int lMAXPTableOffset = 0;
	unsigned int lLength = 0;
	Read32(lMAXPTableOffset, false, false, 4);
	Read32(lLength, false, false, 8);

	unsigned int lVersion = 0;
	unsigned short lNumberOfGlyphs = 0;

	Read32(lVersion, false, true, lMAXPTableOffset);
	Read16(lNumberOfGlyphs, false, true, lMAXPTableOffset + 4);

	cout << "Version: " << hex << setfill('0') << setw(8) << static_cast<unsigned int>(lVersion) << endl;
	cout << "The number of Glyphs: " << dec << lNumberOfGlyphs << endl;
}

void FontAnalyze::PrintInforFromPOST()
{
	unsigned int lPOSTTableOffset = 0;
	unsigned int lLength = 0;
	Read32(lPOSTTableOffset, false, false, 4);
	Read32(lLength, false, false, 8);

	unsigned int lVersion = 0;

	Read32(lVersion, false, true, lPOSTTableOffset);

	cout << "Version: " << hex << setfill('0') << setw(8) << static_cast<unsigned int>(lVersion) << endl;
}

void FontAnalyze::PrintInforFromNAME()
{
	unsigned int lNAMETableOffset = 0;
	unsigned int lLength = 0;
	Read32(lNAMETableOffset, false, false, 4);
	Read32(lLength, false, false, 8);

	unsigned short lFormat = 0;
	unsigned short lCount = 0;
	unsigned short lStringOffset = 0;
	Read16(lFormat, false, true, lNAMETableOffset);
	Read16(lCount, false, true, lNAMETableOffset + 2);
	Read16(lStringOffset, false, true, lNAMETableOffset + 4);

	cout << "Format: " << lFormat << endl;
	cout << "The number of nameRecords in this name table: " << lCount << endl;

	for (auto i = 0; i < lCount; ++i)
	{
		unsigned short lPlatformID = 0; 
		unsigned short lPlatformSpecificID = 0;
		unsigned short lLanguageID = 0;
		unsigned short lNameID = 0;
		unsigned short lLength = 0;
		unsigned short lOffset = 0;

		Read16(lPlatformID, false, true, lNAMETableOffset + 6 + i * 12);
		Read16(lPlatformSpecificID, false, true, lNAMETableOffset + 6 + i * 12 + 2);
		Read16(lLanguageID, false, true, lNAMETableOffset + 6 + i * 12 + 4);
		Read16(lNameID, false, true, lNAMETableOffset + 6 + i * 12 + 6);
		Read16(lLength, false, true, lNAMETableOffset + 6 + i * 12 + 8);
		Read16(lOffset, false, true, lNAMETableOffset + 6 + i * 12 + 10);

		cout << "(" << i + 1 << ") " << "Platform ID: " << lPlatformID << " PlatformSpecificID: " << lPlatformSpecificID \
			<< " Language ID: " << lLanguageID << " NameID: " << lNameID << " -> ";

		// By default, we assume it's ASCII which is not always.
		char* lCharacterString = __FontData + lNAMETableOffset + lStringOffset + lOffset;

		if (lPlatformID == 1) // This is for MAC
		{
			cout << endl;
			continue;
		}

		for (auto j = 0; j < lLength; j++, lCharacterString++)
		{
			 cout  << *lCharacterString;
		}
			
		cout << endl;
	}

}

char FontAnalyze::hReadDoubleByte(const char* iPos)
{
	return *(iPos + 1);
}

void FontAnalyze::PrintCurLocation()
{
	cout << "Row: " << __Row << ", Column: " << __Col << endl;
}

FontAnalyze::~FontAnalyze()
{
	if (__FontData)
		delete __FontData;
}