
// ClientEbestDlg.h : ��� ����
//

#pragma once
#include "CRabbitMQConsumer.h"
#include "EBestResponseProxy.h"
#include "EbstFactory.h"
#include <map>
// CClientEbestDlg ��ȭ ����
class CClientEbestDlg : public CDialogEx
{
	// �����Դϴ�.
public:
	CClientEbestDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CLIENTEBEST_DIALOG };

	static void CALLBACK MsgQueueErrorCatch(int type, CString errorMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
