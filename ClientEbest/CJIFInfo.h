#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////////
// ������ ( KEY=1, GROUP=1 )
#pragma pack( push, 1 )

#define NAME_JIF     "JIF"

// �Է�                           
typedef struct _JIF_InBlock
{
	char    jangubun[1];    // [string,    1] �屸��                          StartPos 0, Length 1
} JIF_InBlock, *LPJIF_InBlock;
#define NAME_JIF_InBlock     "InBlock"

// ���                           
typedef struct _JIF_OutBlock
{
	char    jangubun[1];    // [string,    1] �屸��                          StartPos 0, Length 1
	char    jstatus[2];    // [string,    2] �����                          StartPos 1, Length 2
} JIF_OutBlock, *LPJIF_OutBlock;
#define NAME_JIF_OutBlock     "OutBlock"

#pragma pack( pop )

#include "AbstractBase.h"
class JIFReal : public CAbstractBase
{
public:
	JIF_InBlock inBlock;
	LPJIF_OutBlock outBlock;
	JIFReal() {
		inBlock.jangubun[0] = 1;
	}
	virtual void Request() {};
};
///////////////////////////////////////////////////////////////////////////////////////////////////
