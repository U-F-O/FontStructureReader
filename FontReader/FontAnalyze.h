#pragma once
#include <string>
using namespace std;
class FontAnalyze
{
public:

	FontAnalyze(const string&);
	~FontAnalyze();
	void ResetCurPtr();
	void Read32(unsigned int& o32, bool ibMoveCur = true, bool ibBasedOnDataBegin = false, unsigned int iOffset = 0);
	void Read16(unsigned short& o16, bool ibMoveCur = true, bool ibBasedOnDataBegin = false,  unsigned int iOffset = 0);
	void MoveByOffset(unsigned int iOffset);
	void PrintCurLocation();
	void PrintInforFromCMAP();
	void PrintInforFromHEAD();
	void PrintInforFromHHEA();
	void PrintInforFromLOCA();
	void PrintInforFromMAXP();
	void PrintInforFromNAME();
	void PrintInforFromPOST();

private:
	void hCalculateCurPos();
	char hReadDoubleByte(const char* iPos);
	int __Row, __Col;
	char *__FontData;
	char* __Cur;
	bool __ShortVersionForLOCATable;
};

