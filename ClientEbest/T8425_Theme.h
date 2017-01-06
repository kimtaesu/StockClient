#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////////
// 테마전체조회 ( ATTR,BLOCK,HEADTYPE=A )
#pragma pack( push, 1 )

#define NAME_t8425     "t8425"

// 기본입력                       
typedef struct _t8425InBlock
{
	char    dummy[1];    char    _dummy;    // [string,    1] Dummy                           StartPos 0, Length 1
} t8425InBlock, *LPt8425InBlock;
#define NAME_t8425InBlock     "t8425InBlock"

// 출력                           , occurs
typedef struct _t8425OutBlock
{
	char    tmname[36];    char    _tmname;    // [string,   36] 테마명                          StartPos 0, Length 36
	char    tmcode[4];    char    _tmcode;    // [string,    4] 테마코드                        StartPos 37, Length 4
} t8425OutBlock, *LPt8425OutBlock;
#define NAME_t8425OutBlock     "t8425OutBlock"

#pragma pack( pop )
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "AbstractBase.h"
class T8425_Theme : public CAbstractBase
{
public:
	TCHAR* szTrNo;
	t8425InBlock inBlock;
	LPt8425OutBlock outBlock;
	BOOL bNext;
	TCHAR* bNextKey;
	virtual void Request() {};
	T8425_Theme() {
		szTrNo = NAME_t8425;
		inBlock.dummy[0] = 1;
	}
};