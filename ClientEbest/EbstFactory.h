#pragma once
#include "StockAbsractFactory.h"
#include "CJIFInfo.h"
#include "CEbestGateWay.h"
class CEbstFactory : public CStockAbsractFactory
{
public:
	CEbstFactory(HWND hwnd);
	~CEbstFactory();
	virtual CAbstractGateWay* GetGateWay();
	virtual BOOL Init();
	virtual BOOL Connect();
	virtual BOOL Login();
	virtual BOOL DisConnect();
	IXingAPI* GetXingAPI();
	BOOL SubscribeReal();
private:
	HWND mHWND;
	IXingAPI g_iXingAPI;
	CEbestGateWay* mEbestGateWay;
};

