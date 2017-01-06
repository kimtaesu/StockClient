#pragma once
class EBestResponseProxy
{
public:
	EBestResponseProxy(IXingAPI* g_iXingAPI);
	~EBestResponseProxy();
	LRESULT onXMRealReceived(WPARAM wParam, LPARAM lParam);
	LRESULT onXMReceived(WPARAM wParam, LPARAM lParam);
	LRESULT OnXMTimeout(WPARAM wParam, LPARAM lParam);
private: 
	IXingAPI* pXingApi;
};

