#pragma once
#include "CRabbitMQDirector.h"


class CRabbitMQConsumer
{
public:
	CRabbitMQConsumer();
	~CRabbitMQConsumer();
	CRabbitMQConsumer* Config(CString exchange, CString queue, CString routingKey);
	CRabbitMQConsumer* Bind();
	void Consumer(PF_CONSUMER_LISTEN listen);
	void Publish(CString message);
private:
	CRabbitMQDirector* pDirector;
	amqp_connection_state_t conn;
	CWinThread* m_pConsumerThread;
	static UINT ConsumerThread(LPVOID pv);
	LONG mAbortConsumer;	
	PF_CONSUMER_LISTEN pf_consumer_listen;
	CMQBuilder* builder;
};

