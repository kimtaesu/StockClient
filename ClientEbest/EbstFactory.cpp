#include "stdafx.h"
#include "EbstFactory.h"
#include "CEbestGateWay.h"
#include "include/Account.h"
#include "T8425_Theme.h"

CEbstFactory::CEbstFactory(HWND hwnd) : mEbestGateWay(NULL), mHWND(hwnd)
{
	mEbestGateWay = new CEbestGateWay();
}

CEbstFactory::~CEbstFactory()
{
	SAFE_DELETE(mEbestGateWay);
}

CAbstractGateWay* CEbstFactory::GetGateWay()
{
	return mEbestGateWay;
}

IXingAPI* CEbstFactory::GetXingAPI()
{
	return &g_iXingAPI;
}

BOOL CEbstFactory::SubscribeReal()
{

	JIFReal* jif = new JIFReal();
	BOOL result = mEbestGateWay->RequestAPI(mHWND, GetXingAPI(), jif);
	if (result)
	{
		LOG_TRACE("result is TRUE. Start to subscribe operations");
	}
	SAFE_DELETE(jif);
	return result;
}

BOOL CEbstFactory::Init()
{
	ENTER();
	if (g_iXingAPI.Init() == FALSE)
	{
		// 메인 Directory를 찾은후에 

		CString strPath = _PATH_EBEST_LIB;

		// 메인 Directory를 입력하여 초기화 한다.
		if (g_iXingAPI.Init(strPath) == FALSE)
		{
			TRACE("Fail to init EBest Api");
			//			MessageBox(mHwnd, "init 실패", "xingAPI DLL을 Load 할 수 없습니다.", MB_ICONSTOP);
			return FALSE;
		}
		else {
			TRACE("Success to init EBest Api");
			return TRUE;
		}
	}
	END();
}

BOOL CEbstFactory::Connect()
{
	ENTER();

	// 이미 접속되어 있으면 접속을 종료한다.
	if (g_iXingAPI.IsConnected())
	{
		LOG_TRACE("already connected xing api");
		g_iXingAPI.Disconnect();
	}

	//-----------------------------------------------------------------------------
	// 서버 IP

	CString strServerIP = _T("hts.etrade.co.kr");
	//-----------------------------------------------------------------------------
	// 서버 Port
	int nServerPort = 20001;

	//-----------------------------------------------------------------------------
	// 공유기 사용
	int nSendPacketSize = -1;		// -1 로 설정하면 기본값을 사용한다.

									//-----------------------------------------------------------------------------
									// 서버연결시간
	int nConnectTimeOut = -1;		// -1 로 설정하면 기본값을 사용한다.

									//-----------------------------------------------------------------------------
									// 서버접속
	BOOL bResult = g_iXingAPI.Connect(
		mHWND,			// Connect가 된 이후에 Disconnect 메시지를 받을 윈도우 Handle
												// Login 윈도우는 Login 만 처리하는 윈도우 이므로 Disconnect는 메인윈도우에서 받는다.


		strServerIP,							// 서버주소

		nServerPort,							// 서버포트

		WM_USER,								// XingAPI에서 사용하는 메시지의 시작번호, Windows에서는 사용자메시지를 0x400(=WM_USER) 이상을
												// 사용해야 함. 기본적으로는 WM_USER를 사용하면 되지만 프로그램 내부에서 메시지 ID가 겹치게 되면
												// 이 값을 조정하여 메시지 ID 충돌을 피할수 있음

		nConnectTimeOut,						// 지정한 시간이상(1/1000 초 단위)으로 시간이 걸리게 될 경우 연결실패로 간주함

		nSendPacketSize							// 보내어지는 Packet Size, -1 이면 기본값 사용
												// 인터넷 공유기등에서는 특정 크기 이상의 데이터를 한번에 보내면 에러가 떨어지는 경우가 발생
												// 이럴 경우에 한번에 보내는 Packet Size를 지정하여 그 이상 되는 Packet은 여러번에 걸쳐 전송
	);

	LOG_TRACE("Connect to server %d", bResult);
	RETURN(bResult);
}

BOOL CEbstFactory::Login()
{
	// 로그인
	if (Connect())
	{

		BOOL bResult = g_iXingAPI.Login(
			mHWND,							// Login 성공여부 메시지를 받을 윈도우
			Functions::GetID(),									// 사용자 ID
			Functions::GetPass(),									// 사용자 비밀번호
			Functions::GetPublicPass(),								// 공인인증 비밀번호
			0,							// 0 : 실서버, 1 : 모의투자서버
			TRUE							// 로그인 중에 공인인증 에러가 발생시 에러메시지 표시여부
		);
		LOG_TRACE("Success to login ");
		RETURN(bResult);
	}
	else {
		LOG_TRACE("Fail to login ");
		RETURN(FALSE);
	}
}

BOOL CEbstFactory::DisConnect()
{

	if (g_iXingAPI.IsConnected())
	{
		g_iXingAPI.Logout(mHWND);
		return g_iXingAPI.Disconnect();
	}
	else {
		return TRUE;
	}

	return TRUE;
}