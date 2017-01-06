
// ClientEbestDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ClientEbest.h"
#include "ClientEbestDlg.h"
#include "afxdialogex.h"
#include "T8425_Theme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientEbestDlg ��ȭ ����



CClientEbestDlg::CClientEbestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientEbestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CClientEbestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientEbestDlg, CDialogEx)

	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER + XM_RECEIVE_REAL_DATA, &CClientEbestDlg::OnXMReceiveRealData)
	ON_MESSAGE(WM_USER + XM_RECEIVE_DATA, &CClientEbestDlg::OnXMReceiveData)
	ON_MESSAGE(WM_USER + XM_TIMEOUT_DATA, &CClientEbestDlg::OnXMTimeoutData)
	ON_MESSAGE(WM_USER + XM_LOGIN, &CClientEbestDlg::OnXMLogin)
	ON_WM_TIMER()
END_MESSAGE_MAP()



void CALLBACK CClientEbestDlg::MsgQueueErrorCatch(int type, CString errorMsg)
{
	AfxMessageBox(errorMsg,
		MB_ICONERROR);
}

// CClientEbestDlg �޽��� ó����
BOOL CClientEbestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitAPI();
		// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

		// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}



void CClientEbestDlg::OnDestroy()
{
	__super::OnDestroy();
	DestroyAPI();
	// TODO: Add your message handler code here
}

void CClientEbestDlg::DestroyAPI()
{
	SAFE_DELETE(pEBestResponseProxy);
	std::map<CStockAbsractFactory::API_FACTORIES, CStockAbsractFactory*>::iterator iterator;
	for (iterator = mAPIMap.begin(); iterator != mAPIMap.end(); iterator++) {
		CString type;
		switch (iterator->first)
		{
		case CStockAbsractFactory::API_FACTORIES::EBEST:
			LOG_TRACE("ebest will be disconnect");
			break;
		default:
			break;
		}
		iterator->second->DisConnect();
		TRACE("disconnected");
	}

}

void CClientEbestDlg::InitAPI()
{
	CEbstFactory* ebest = static_cast<CEbstFactory*>(CStockAbsractFactory::CreateAPIFactory(GetSafeHwnd(), CStockAbsractFactory::EBEST));
	mAPIMap.insert(std::pair<CStockAbsractFactory::API_FACTORIES, CStockAbsractFactory*>(CStockAbsractFactory::API_FACTORIES::EBEST, ebest));
	assert(ebest->Init());
	SetTimer(UID_INIT_TIMER, 1000, NULL);
	pEBestResponseProxy = new EBestResponseProxy(ebest->GetXingAPI());
	// rabbitmq connect
	CString exchange, queue, routing;
	exchange.LoadString(IDS_BIND_EXCHANGE);
	queue.LoadString(IDS_BIND_QUEUE);
	routing.LoadString(IDS_BIND_ROUTING_KEY);
	p_rabbitmq.Config(exchange, queue, routing);

}

afx_msg LRESULT CClientEbestDlg::OnXMReceiveRealData(WPARAM wParam, LPARAM lParam)
{
	LOG_TRACE("OnXMReceiveRealData");
	pEBestResponseProxy->onXMRealReceived(wParam, lParam);
	return 0;
}

afx_msg LRESULT CClientEbestDlg::OnXMReceiveData(WPARAM wParam, LPARAM lParam)
{
	LOG_TRACE("OnXMReceiveData");
	pEBestResponseProxy->onXMReceived(wParam, lParam);
	return 0;
}


afx_msg LRESULT CClientEbestDlg::OnXMTimeoutData(WPARAM wParam, LPARAM lParam)
{
	LOG_TRACE("OnXMTimeoutData");
	pEBestResponseProxy->OnXMTimeout(wParam, lParam);
	return 0;
}


afx_msg LRESULT CClientEbestDlg::OnXMLogin(WPARAM wParam, LPARAM lParam)
{
	LOG_TRACE("OnXMLogin");
	LPCSTR pszCode = (LPCSTR)wParam;
	LPCSTR pszMsg = (LPCSTR)lParam;

	CString strMsg;
	strMsg.Format("[%s] %s", pszCode, pszMsg);

	EnableWindow(TRUE);
	assert(atoi(pszCode) == 0);

	std::map<CStockAbsractFactory::API_FACTORIES, CStockAbsractFactory*>::iterator findObj = mAPIMap.find(CStockAbsractFactory::API_FACTORIES::EBEST);
	CEbstFactory* ebest = dynamic_cast<CEbstFactory*>(findObj->second);
	T8425_Theme* t8425 = new T8425_Theme();
	assert(ebest->SubscribeReal());
	return TRUE;
}


void CClientEbestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == UID_INIT_TIMER)
	{
		KillTimer(nIDEvent);
		std::map<CStockAbsractFactory::API_FACTORIES, CStockAbsractFactory*>::iterator findObj = mAPIMap.find(CStockAbsractFactory::API_FACTORIES::EBEST);
		CEbstFactory* ebest = dynamic_cast<CEbstFactory*>(findObj->second);
		assert(ebest->Login());
	}
	CDialogEx::OnTimer(nIDEvent);
}
