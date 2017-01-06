#include "stdafx.h"
#include "CRabbitMQConsumer.h"
#include <json/value.h>
#include <json/reader.h>
#include "JsonSerializer.h"
#include "resource.h"
#include "MessageOperation.h"
CRabbitMQConsumer::CRabbitMQConsumer() : m_pConsumerThread(NULL), builder(NULL)
{
	builder = new CRabbitMQBuilder();
	pDirector = new CRabbitMQDirector();
}


CRabbitMQConsumer::~CRabbitMQConsumer()
{
	pDirector->Disconnect(conn);
	if (m_pConsumerThread)
	{
		InterlockedExchange(&mAbortConsumer, 1);
		WaitForSingleObject(m_pConsumerThread, INFINITE);
		m_pConsumerThread->Delete();
	}
	SAFE_DELETE(m_pConsumerThread);
	SAFE_DELETE(builder);
	SAFE_DELETE(pDirector);
}

CRabbitMQConsumer* CRabbitMQConsumer::Config(CString exchange, CString queue, CString routingKey)
{
	
	builder->ConnectFactory();
	builder->ConfigExchange(exchange);
	builder->ConfigBindingKey(routingKey);
	builder->ConfigQueueName(queue);
	conn = pDirector->Connect(builder);
	return this;
}

CRabbitMQConsumer* CRabbitMQConsumer::Bind()
{
	pDirector->Bind(conn);
	return this;
}

void CRabbitMQConsumer::Consumer(PF_CONSUMER_LISTEN listen)
{

	pf_consumer_listen = listen;
	mAbortConsumer = 0;
	pDirector->Consumer(conn);
	m_pConsumerThread = AfxBeginThread(CRabbitMQConsumer::ConsumerThread, this,
		THREAD_PRIORITY_NORMAL, 0,
		CREATE_SUSPENDED);

	assert(m_pConsumerThread);
	m_pConsumerThread->ResumeThread();
}

void CRabbitMQConsumer::Publish(CString message)
{
	pDirector->Publish(conn, message);
}

static void asciiToUTF8(CString parm_ascii_string, CString &parm_utf8_string)
{
	parm_utf8_string.Empty();

	// �ƽ�Ű �ڵ带 UTF8������ �ڵ�� ��ȯ�ؾ� �Ѵ�. �ƽ�Ű �ڵ带 UTF8 �ڵ�� ��ȯ�Ҷ���
	// �ƽ�Ű �ڵ带 �����ڵ�� ���� ��ȯ�ϰ� ��ȯ�� �����ڵ带 UTF8 �ڵ�� ��ȯ�ؾ� �Ѵ�.

	// �ƽ�Ű �ڵ�ε� ���ڿ��� �����ڵ�ȭ ������ ���� ���̸� ���Ѵ�.      
	int temp_length = MultiByteToWideChar(CP_ACP, 0, (char *)(const char *)parm_ascii_string, -1, NULL, 0);
	// ��ȯ�� �����ڵ带 ������ ������ �Ҵ��Ѵ�.    
	BSTR unicode_str = SysAllocStringLen(NULL, temp_length + 1);

	// �ƽ�Ű �ڵ�ε� ���ڿ��� ���� �ڵ� ������ ���ڿ��� �����Ѵ�.      
	MultiByteToWideChar(CP_ACP, 0, (char *)(const char *)parm_ascii_string, -1, unicode_str, temp_length);

	// �����ڵ� ������ ���ڿ��� UTF8 �������� ���������� �ʿ��� �޸� ������ ũ�⸦ ��´�.    
	temp_length = WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, NULL, 0, NULL, NULL);

	if (temp_length > 0) {
		CString str;
		// UTF8 �ڵ带 ������ �޸� ������ �Ҵ��Ѵ�.        
		char *p_utf8_string = new char[temp_length];
		memset(p_utf8_string, 0, temp_length);
		// �����ڵ带 UTF8�ڵ�� ��ȯ�Ѵ�.        
		WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, p_utf8_string, temp_length, NULL, NULL);

		// UTF8 �������� ����� ���ڿ��� �� ������ �ڵ尪���� �� URL�� ���Ǵ� �������� ��ȯ�Ѵ�.        
		for (int i = 0; i < temp_length - 1; i++) {
			if (p_utf8_string[i] & 0x80) {
				// ���� �ڵ尡 �ѱ��� ���..
				// UTF8 �ڵ�� ǥ���� �ѱ��� 3����Ʈ�� ǥ�õȴ�. "�ѱ�"  ->  %ED%95%9C%EA%B8%80                
				for (int sub_i = 0; sub_i < 3; sub_i++) {
					str.Format("%%%X", p_utf8_string[i] & 0x00FF);
					parm_utf8_string += str;
					i++;
				}

				i--;
			}
			else {
				// ���� �ڵ尡 ������ ���, ������� �״�� ����Ѵ�.
				parm_utf8_string += p_utf8_string[i];
			}
		}

		delete[] p_utf8_string;


		// �����ڵ� ������ ���ڿ��� �����ϱ� ���� �����ߴ� �޸𸮸� �����Ѵ�.
		SysFreeString(unicode_str);
	}
}

UINT CRabbitMQConsumer::ConsumerThread(LPVOID pParam)
{
#define SUMMARY_EVERY_US 1000000

	CRabbitMQConsumer *const pConsumerParam = static_cast<CRabbitMQConsumer *>(pParam);
	amqp_frame_t frame;

	uint64_t now;
	for (;;) {
		amqp_rpc_reply_t ret;
		amqp_envelope_t envelope;

		amqp_maybe_release_buffers(pConsumerParam->conn);
		ret = amqp_consume_message(pConsumerParam->conn, &envelope, NULL, 0);
		if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
			if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type &&
				AMQP_STATUS_UNEXPECTED_STATE == ret.library_error) {
				if (AMQP_STATUS_OK != amqp_simple_wait_frame(pConsumerParam->conn, &frame)) {
					return 0;
				}

				if (AMQP_FRAME_METHOD == frame.frame_type) {
					switch (frame.payload.method.id) {
					case AMQP_BASIC_ACK_METHOD:
						/* if we've turned publisher confirms on, and we've published a message
						* here is a message being confirmed
						*/

						break;
					case AMQP_BASIC_RETURN_METHOD:
						/* if a published message couldn't be routed and the mandatory flag was set
						* this is what would be returned. The message then needs to be read.
						*/
					{
						amqp_message_t message;
						ret = amqp_read_message(pConsumerParam->conn, frame.channel, &message, 0);

						if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
							return 0;
						}

						amqp_destroy_message(&message);
					}

					break;

					case AMQP_CHANNEL_CLOSE_METHOD:
						/* a channel.close method happens when a channel exception occurs, this
						* can happen by publishing to an exchange that doesn't exist for example
						*
						* In this case you would need to open another channel redeclare any queues
						* that were declared auto-delete, and restart any consumers that were attached
						* to the previous channel
						*/
						return 0;

					case AMQP_CONNECTION_CLOSE_METHOD:
						/* a connection.close method happens when a connection exception occurs,
						* this can happen by trying to use a channel that isn't open for example.
						*
						* In this case the whole connection must be restarted.
						*/
						return 0;

					default:
						fprintf(stderr, "An unexpected method was received %u\n", frame.payload.method.id);
						return 0;
					}
				}
			}

		}
		else {
			#if 0
CString in, out;
			in.SetString((char *)envelope.message.body.bytes, envelope.message.body.len);
			asciiToUTF8(in, out);
			JSONCPP_STRING jsonData = out.GetBuffer(0);
			CMessageOperation operation;
			CJsonSerializer::Deserialize(&operation, jsonData);
#endif

			amqp_destroy_envelope(&envelope);
		}
	}
	LOG_TRACE("thread end");
}

