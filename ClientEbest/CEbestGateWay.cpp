#include "stdafx.h"
#include "CEbestGateWay.h"
#include "CJIFInfo.h"
#include <typeinfo>
#include "T8425_Theme.h"
BOOL CEbestGateWay::RequestAPI(HWND hwnd, IXingAPI* context, CAbstractBase* tr)
{
	if (dynamic_cast<const JIFReal*>(tr) != NULL)
	{
		return AdviseRealData(hwnd, context, tr);
	}
	else if (dynamic_cast<const T8425_Theme*>(tr) != NULL)
	{
		RequestTR(hwnd, context, tr);
	}

	return FALSE;
}

BOOL CEbestGateWay::AdviseRealData(HWND hwnd, IXingAPI* context, CAbstractBase* tr)
{

	if (JIFReal* jif = dynamic_cast<JIFReal*>(tr))
	{
		BOOL Result = context->AdviseRealData(
			hwnd,				// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
			NAME_JIF,					// TR 번호
			"1",							// SC4는 빈 데이터를 넘겨야 한다.
			1							// SC4의 데이터 길이는 0이다.
		);
		TRACE("JIFReal request result : %d", Result);
		return Result;
	}
	return TRUE;
}

BOOL CEbestGateWay::RequestTR(HWND hwnd, IXingAPI* context, CAbstractBase* tr)
{
	int nRqID = FALSE;
	if (dynamic_cast<const T8425_Theme*>(tr) != NULL)
	{
		T8425_Theme* t8425 = dynamic_cast<T8425_Theme*>(tr);
		FillMemory(&t8425->inBlock, sizeof(t8425->inBlock), ' ');
		nRqID = context->Request(
			hwnd,				// 데이터를 받을 윈도우, XM_RECEIVE_DATA 으로 온다.
			t8425->szTrNo,						// TR 번호
			&t8425->inBlock,				// InBlock 데이터
			sizeof(t8425->inBlock),		// InBlock 데이터 크기
			t8425->bNext,						// 다음조회 여부 : 다음조회일 경우에 세팅한다.
			t8425->bNextKey,					// 다음조회 Key : Header Type이 B 일 경우엔 이전 조회때 받은 Next Key를 넣어준다.
			30							// Timeout(초) : 해당 시간(초)동안 데이터가 오지 않으면 Timeout에 발생한다. XM_TIMEOUT_DATA 메시지가 발생한다.
		);
	}

	if (nRqID < 0)
	{
		LOG_TRACE("Fail RequestTR");
	}
	return nRqID < 0 ? FALSE : TRUE;
}
