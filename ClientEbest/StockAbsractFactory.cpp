#include "stdafx.h"
#include "StockAbsractFactory.h"
#include "EbstFactory.h"

CStockAbsractFactory* CStockAbsractFactory::CreateAPIFactory(HWND hwnd, API_FACTORIES feature)
{
	if (feature == API_FACTORIES::EBEST)
	{
		return new CEbstFactory(hwnd);
	}
	else {
		return nullptr;
	}
}