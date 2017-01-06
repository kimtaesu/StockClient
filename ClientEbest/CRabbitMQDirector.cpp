#include "stdafx.h"
#include "CRabbitMQDirector.h"

CRabbitMQDirector::CRabbitMQDirector() :  pbuilder(NULL)
{
}

CRabbitMQDirector::~CRabbitMQDirector()
{
}

amqp_connection_state_t CRabbitMQDirector::Connect(CMQBuilder* builder)
{
	pbuilder = builder;
	assert(pbuilder->hostName.Compare(""));
	assert(pbuilder->vHost.Compare(""));
	assert(pbuilder->exchange.Compare(""));
	assert(pbuilder->port != 0);
	assert(pbuilder->pass.Compare(""));
	assert(pbuilder->id.Compare(""));

	amqp_socket_t *socket = NULL;
	conn = amqp_new_connection();
	socket = amqp_tcp_socket_new(conn);
	assert(socket);

	int status = amqp_socket_open(socket, builder->hostName, builder->port);
	assert(status == 0);

	amqp_rpc_reply_t result = amqp_login(conn, builder->vHost, 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, builder->id, builder->pass);
	assert(result.reply_type != AMQP_RESPONSE_NONE);

	amqp_channel_open(conn, 1);
#if 0
	//amqp_bytes_t queuename = amqp_cstring_bytes(builder->queueName);
	amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, 1, amqp_empty_bytes, 0, 0, 0, 1,
		amqp_empty_table);

	amqp_bytes_t queuename = amqp_bytes_malloc_dup(r->queue);
	assert(queuename.bytes != NULL);

	amqp_queue_bind(conn, 1, queuename, amqp_cstring_bytes(builder->exchange), amqp_cstring_bytes(builder->bindingKey),
		amqp_empty_table);

	amqp_basic_consume(conn, 1, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
#endif
	return conn;
}

void CRabbitMQDirector::Bind(amqp_connection_state_t conn)
{
	assert(conn);
	amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, 1, amqp_empty_bytes, 0, 0, 0, 1,
		amqp_empty_table);
	
	queuename = amqp_bytes_malloc_dup(r->queue);
	assert(queuename.bytes != NULL);
	
	amqp_queue_bind(conn, 1, queuename, amqp_cstring_bytes(pbuilder->exchange), amqp_cstring_bytes(pbuilder->bindingKey),
		amqp_empty_table);
}

void CRabbitMQDirector::Consumer(amqp_connection_state_t conn)
{
	assert(conn);
	assert(queuename.bytes != NULL);
	amqp_basic_consume_ok_t * ok = amqp_basic_consume(conn, 1, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
	assert(ok);
}

void CRabbitMQDirector::Publish(amqp_connection_state_t conn, CString message)
{
	int res = amqp_basic_publish(conn,
		1,	
		amqp_cstring_bytes(pbuilder->exchange),	
		amqp_cstring_bytes(pbuilder->bindingKey), 
		0, 
		0,	
		NULL, 
		amqp_cstring_bytes(message.GetBuffer(0)));

	assert(res != AMQP_RESPONSE_NONE);
}

void CRabbitMQDirector::Disconnect(amqp_connection_state_t conn)
{
	amqp_rpc_reply_t result = amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
	assert(result.reply_type != AMQP_RESPONSE_NONE);
	result = amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
	assert(result.reply_type != AMQP_RESPONSE_NONE);
	int res = amqp_destroy_connection(conn);
	assert(res <= 0);
}
