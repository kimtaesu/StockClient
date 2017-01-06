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
			hwnd,				// �����͸� ���� ������, XM_RECEIVE_REAL_DATA ���� �´�.
			NAME_JIF,					// TR ��ȣ
			"1",							// SC4�� �� �����͸� �Ѱܾ� �Ѵ�.
			1							// SC4�� ������ ���̴� 0�̴�.
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
			hwnd,				// �����͸� ���� ������, XM_RECEIVE_DATA ���� �´�.
			t8425->szTrNo,						// TR ��ȣ
			&t8425->inBlock,				// InBlock ������
			sizeof(t8425->inBlock),		// InBlock ������ ũ��
			t8425->bNext,						// ������ȸ ���� : ������ȸ�� ��쿡 �����Ѵ�.
			t8425->bNextKey,					// ������ȸ Key : Header Type�� B �� ��쿣 ���� ��ȸ�� ���� Next Key�� �־��ش�.
			30							// Timeout(��) : �ش� �ð�(��)���� �����Ͱ� ���� ������ Timeout�� �߻��Ѵ�. XM_TIMEOUT_DATA �޽����� �߻��Ѵ�.
		);
	}

	if (nRqID < 0)
	{
		LOG_TRACE("Fail RequestTR");
	}
	return nRqID < 0 ? FALSE : TRUE;
}
