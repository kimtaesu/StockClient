
// ClientEbestDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ClientEbest.h"
#include "ClientEbestDlg.h"
#include "afxdialogex.h"
#include "T8425_Theme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
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


// CClientEbestDlg 대화 상자



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

// CClientEbestDlg 메시지 처리기
BOOL CClientEbestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitAPI();
		// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

		// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == UID_INIT_TIMER)
	{
		KillTimer(nIDEvent);
		std::map<CStockAbsractFactory::API_FACTORIES, CStockAbsractFactory*>::iterator findObj = mAPIMap.find(CStockAbsractFactory::API_FACTORIES::EBEST);
		CEbstFactory* ebest = dynamic_cast<CEbstFactory*>(findObj->second);
		assert(ebest->Login());
	}
	CDialogEx::OnTimer(nIDEvent);
}
