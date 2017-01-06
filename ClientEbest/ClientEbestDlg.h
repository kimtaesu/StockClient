
// ClientEbestDlg.h : 헤더 파일
//

#pragma once
#include "CRabbitMQConsumer.h"
#include "EBestResponseProxy.h"
#include "EbstFactory.h"
#include <map>
// CClientEbestDlg 대화 상자
class CClientEbestDlg : public CDialogEx
{
	// 생성입니다.
public:
	CClientEbestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLIENTEBEST_DIALOG };

	static void CALLBACK MsgQueueErrorCatch(int type, CString errorMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();

private:
	CRabbitMQConsumer p_rabbitmq;
	EBestResponseProxy* pEBestResponseProxy;
	std::map<CStockAbsractFactory::API_FACTORIES, CStockAbsractFactory*> mAPIMap;
	void InitAPI();
	void DestroyAPI();
protected:
	afx_msg LRESULT OnXMReceiveRealData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXMReceiveData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXMTimeoutData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXMLogin(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
