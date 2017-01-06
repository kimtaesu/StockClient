#pragma once
#include <stdint.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

#include "CRabbitMQBuilder.h"

typedef void(CALLBACK* PF_CONSUMER_LISTEN)(LPVOID);

class CRabbitMQDirector
{
public:
	CRabbitMQDirector();
	~CRabbitMQDirector();
	amqp_connection_state_t Connect(CMQBuilder* builder);
	void Bind(amqp_connection_state_t conn);
	void Consumer(amqp_connection_state_t conn);
	void CRabbitMQDirector::Publish(amqp_connection_state_t conn, CString message);
	void Disconnect(amqp_connection_state_t conn);
private:
	CMQBuilder* pbuilder;
	amqp_connection_state_t conn;
	amqp_bytes_t queuename;
};

