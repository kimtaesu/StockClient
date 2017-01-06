#pragma once
#include "AbstractTRManager.h"

class CEbestGateWay : public CAbstractGateWay
{
public:
	BOOL RequestAPI(HWND hwnd, IXingAPI* context, CAbstractBase* tr);
private:
	BOOL AdviseRealData(HWND hwnd, IXingAPI* context, CAbstractBase* tr);
	BOOL UnAdviseRealData(HWND hwnd, IXingAPI context, CAbstractBase* tr);
	BOOL RequestTR(HWND hwnd, IXingAPI* context, CAbstractBase* tr);
};