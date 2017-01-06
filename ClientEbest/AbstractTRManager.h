#pragma once
#include "AbstractBase.h"

class CAbstractGateWay
{
public:
	virtual BOOL RequestAPI(HWND hwnd, IXingAPI* context, CAbstractBase* tr) = 0;
};