#pragma once
#include "AbstractTRManager.h"
class CStockAbsractFactory
{
public:
	enum API_FACTORIES
	{
		EBEST = 0,
	};
	virtual CAbstractGateWay* GetGateWay() = 0;

	virtual BOOL Init() = 0;
	virtual BOOL Connect() = 0;
	virtual BOOL Login() = 0;
	virtual BOOL DisConnect() = 0;
	static CStockAbsractFactory* CreateAPIFactory(HWND hwnd, API_FACTORIES feature);
};

