#include "stdafx.h"
#include "CRabbitMQBuilder.h"
#include "resource.h"

void CRabbitMQBuilder::ConfigHostName(CString hostName)
{
	this->hostName = hostName;
}

void CRabbitMQBuilder::ConfigVirtualHost(CString vHost)
{
	this->vHost = vHost;
}

void CRabbitMQBuilder::ConfigPort(int port)
{
	this->port = port;
}

void CRabbitMQBuilder::ConfigId(CString id)
{
	this->id = id;
}

void CRabbitMQBuilder::ConfigPass(CString pass)
{
	this->pass = pass;
}

void CRabbitMQBuilder::ConfigExchange(CString exchange)
{
	this->exchange = exchange;
}

void CRabbitMQBuilder::ConfigBindingKey(CString bindingKey)
{
	this->bindingKey = bindingKey;
}

void CRabbitMQBuilder::ConfigQueueName(CString queueName)
{
	this->queueName = queueName;
}

void CRabbitMQBuilder::ConnectFactory()
{
	this->hostName.LoadString(IDS_RABBIT_HOST);
	this->vHost.LoadString(IDS_RABBIT_VIRTUAL_HOST);
	CString port;
	port.LoadString(IDS_RABBIT_PORT);
	this->port = _ttoi(port);
	this->id.LoadString(IDS_RABBIT_ID);
	this->pass.LoadString(IDS_RABBIT_PASS);
}

