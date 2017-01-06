#pragma once


class CMQBuilder
{
public:
	virtual void ConfigHostName(CString hostName) = 0;
	virtual void ConfigVirtualHost(CString vHost) = 0;
	virtual void ConfigPort(int port) = 0;
	virtual void ConfigId(CString id) = 0;
	virtual void ConfigPass(CString pass) = 0;
	virtual void ConfigExchange(CString exchange) = 0;
	virtual void ConfigBindingKey(CString bindingKey) = 0;
	virtual void ConfigQueueName(CString queueName) = 0;
	virtual void ConnectFactory() = 0;
	CString hostName = NULL;
	int port = 0;
	CString id = NULL;
	CString pass = NULL;
	CString exchange = NULL;
	CString bindingKey = NULL;
	CString vHost = NULL;
	CString queueName = NULL;
};

class CRabbitMQBuilder : public CMQBuilder
{
public:
	virtual void ConfigHostName(CString hostName);
	virtual void ConfigVirtualHost(CString vHost);
	virtual void ConfigPort(int port);
	virtual void ConfigId(CString id);
	virtual void ConfigPass(CString pass);
	virtual void ConfigExchange(CString exchange);
	virtual void ConfigBindingKey(CString bindingKey);
	virtual void ConfigQueueName(CString queueName);
	virtual void ConnectFactory();
private:
};

