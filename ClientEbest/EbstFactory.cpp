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
		// ���� Directory�� ã���Ŀ� 

		CString strPath = _PATH_EBEST_LIB;

		// ���� Directory�� �Է��Ͽ� �ʱ�ȭ �Ѵ�.
		if (g_iXingAPI.Init(strPath) == FALSE)
		{
			TRACE("Fail to init EBest Api");
			//			MessageBox(mHwnd, "init ����", "xingAPI DLL�� Load �� �� �����ϴ�.", MB_ICONSTOP);
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

	// �̹� ���ӵǾ� ������ ������ �����Ѵ�.
	if (g_iXingAPI.IsConnected())
	{
		LOG_TRACE("already connected xing api");
		g_iXingAPI.Disconnect();
	}

	//-----------------------------------------------------------------------------
	// ���� IP

	CString strServerIP = _T("hts.etrade.co.kr");
	//-----------------------------------------------------------------------------
	// ���� Port
	int nServerPort = 20001;

	//-----------------------------------------------------------------------------
	// ������ ���
	int nSendPacketSize = -1;		// -1 �� �����ϸ� �⺻���� ����Ѵ�.

									//-----------------------------------------------------------------------------
									// ��������ð�
	int nConnectTimeOut = -1;		// -1 �� �����ϸ� �⺻���� ����Ѵ�.

									//-----------------------------------------------------------------------------
									// ��������
	BOOL bResult = g_iXingAPI.Connect(
		mHWND,			// Connect�� �� ���Ŀ� Disconnect �޽����� ���� ������ Handle
												// Login ������� Login �� ó���ϴ� ������ �̹Ƿ� Disconnect�� ���������쿡�� �޴´�.


		strServerIP,							// �����ּ�

		nServerPort,							// ������Ʈ

		WM_USER,								// XingAPI���� ����ϴ� �޽����� ���۹�ȣ, Windows������ ����ڸ޽����� 0x400(=WM_USER) �̻���
												// ����ؾ� ��. �⺻�����δ� WM_USER�� ����ϸ� ������ ���α׷� ���ο��� �޽��� ID�� ��ġ�� �Ǹ�
												// �� ���� �����Ͽ� �޽��� ID �浹�� ���Ҽ� ����

		nConnectTimeOut,						// ������ �ð��̻�(1/1000 �� ����)���� �ð��� �ɸ��� �� ��� ������з� ������

		nSendPacketSize							// ���������� Packet Size, -1 �̸� �⺻�� ���
												// ���ͳ� ���������� Ư�� ũ�� �̻��� �����͸� �ѹ��� ������ ������ �������� ��찡 �߻�
												// �̷� ��쿡 �ѹ��� ������ Packet Size�� �����Ͽ� �� �̻� �Ǵ� Packet�� �������� ���� ����
	);

	LOG_TRACE("Connect to server %d", bResult);
	RETURN(bResult);
}

BOOL CEbstFactory::Login()
{
	// �α���
	if (Connect())
	{

		BOOL bResult = g_iXingAPI.Login(
			mHWND,							// Login �������� �޽����� ���� ������
			Functions::GetID(),									// ����� ID
			Functions::GetPass(),									// ����� ��й�ȣ
			Functions::GetPublicPass(),								// �������� ��й�ȣ
			0,							// 0 : �Ǽ���, 1 : �������ڼ���
			TRUE							// �α��� �߿� �������� ������ �߻��� �����޽��� ǥ�ÿ���
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