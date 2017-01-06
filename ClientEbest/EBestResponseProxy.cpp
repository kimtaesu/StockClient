#include "stdafx.h"
#include "EBestResponseProxy.h"

EBestResponseProxy::EBestResponseProxy(IXingAPI* g_iXingAPI) : pXingApi(g_iXingAPI)
{

}


EBestResponseProxy::~EBestResponseProxy()
{
}

LRESULT EBestResponseProxy::onXMRealReceived(WPARAM wParam, LPARAM lParam)
{
	LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;
	return LRESULT();
}

LRESULT EBestResponseProxy::onXMReceived(WPARAM wParam, LPARAM lParam)
{
	//-------------------------------------------------------------------------------------
	// Data를 받음
	if (wParam == REQUEST_DATA)
	{
		TRACE("REQUEST_DATA");
	}
	//-------------------------------------------------------------------------------------
	// 메시지를 받음
	else if (wParam == MESSAGE_DATA)
	{
		TRACE("MESSAGE_DATA");
		//TODO MESSAGE
		LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;

		CString strMsg((char*)pMsg->lpszMessageData, pMsg->nMsgLength);
		//	GetParent()->SendMessage(WMU_SET_MESSAGE, (WPARAM)pMsg->szMsgCode, (LPARAM)(LPCTSTR)strMsg);
		LOG_TRACE((LPARAM)(LPCTSTR)strMsg);
		pXingApi->ReleaseMessageData(lParam);

	}
	else if (wParam == SYSTEM_ERROR_DATA)
	{
		TRACE("SYSTEM_ERROR_DATA");
		//TODO ERROR
		LPMSG_PACKET pMsg = (LPMSG_PACKET)lParam;

		CString strMsg((char*)pMsg->lpszMessageData, pMsg->nMsgLength);
		//	GetParent()->SendMessage(WMU_SET_MESSAGE, (WPARAM)pMsg->szMsgCode, (LPARAM)(LPCTSTR)strMsg);
		OutputDebugString((LPCTSTR)strMsg);
		pXingApi->ReleaseMessageData(lParam);
	}
	//-------------------------------------------------------------------------------------
	// Release Data를 받음
	else if (wParam == RELEASE_DATA)
	{
		TRACE("RELEASE_DATA");
		pXingApi->ReleaseRequestData((int)lParam);
	}
	return 0L;
}

LRESULT EBestResponseProxy::OnXMTimeout(WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}

