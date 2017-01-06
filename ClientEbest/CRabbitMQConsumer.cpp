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

	// 아스키 코드를 UTF8형식의 코드로 변환해야 한다. 아스키 코드를 UTF8 코드로 변환할때는
	// 아스키 코드를 유니코드로 먼저 변환하고 변환된 유니코드를 UTF8 코드로 변환해야 한다.

	// 아스키 코드로된 문자열을 유니코드화 시켰을 때의 길이를 구한다.      
	int temp_length = MultiByteToWideChar(CP_ACP, 0, (char *)(const char *)parm_ascii_string, -1, NULL, 0);
	// 변환된 유니코드를 저장할 공간을 할당한다.    
	BSTR unicode_str = SysAllocStringLen(NULL, temp_length + 1);

	// 아스키 코드로된 문자열을 유니 코드 형식의 문자열로 변경한다.      
	MultiByteToWideChar(CP_ACP, 0, (char *)(const char *)parm_ascii_string, -1, unicode_str, temp_length);

	// 유니코드 형식의 문자열을 UTF8 형식으로 변경했을때 필요한 메모리 공간의 크기를 얻는다.    
	temp_length = WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, NULL, 0, NULL, NULL);

	if (temp_length > 0) {
		CString str;
		// UTF8 코드를 저장할 메모리 공간을 할당한다.        
		char *p_utf8_string = new char[temp_length];
		memset(p_utf8_string, 0, temp_length);
		// 유니코드를 UTF8코드로 변환한다.        
		WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, p_utf8_string, temp_length, NULL, NULL);

		// UTF8 형식으로 변경된 문자열을 각 문자의 코드값별로 웹 URL에 사용되는 형식으로 변환한다.        
		for (int i = 0; i < temp_length - 1; i++) {
			if (p_utf8_string[i] & 0x80) {
				// 현재 코드가 한글인 경우..
				// UTF8 코드로 표현된 한글은 3바이트로 표시된다. "한글"  ->  %ED%95%9C%EA%B8%80                
				for (int sub_i = 0; sub_i < 3; sub_i++) {
					str.Format("%%%X", p_utf8_string[i] & 0x00FF);
					parm_utf8_string += str;
					i++;
				}

				i--;
			}
			else {
				// 현재 코드가 영문인 경우, 변경없이 그대로 사용한다.
				parm_utf8_string += p_utf8_string[i];
			}
		}

		delete[] p_utf8_string;


		// 유니코드 형식의 문자열을 저장하기 위해 생성했던 메모리를 삭제한다.
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

