#ifndef ANALYOPR
#define ANALYOPR
#include "stdafx.h"

static string content_type_conf;
enum REQ_METHOD
{
	METHOD_GET,
	METHOD_POST,
	METHOD_UNKNOW,
	METHOD_UNSUPPORT
};
enum RSP_CODE
{
	HTTP_STATUS_0_UNKNOWN = 0,

	HTTP_STATUS_200_OK = 200,

	HTTP_STATUS_206_Range = 206,

	HTTP_STATUS_302_Redirect = 302,

	HTTP_STATUS_404_NotFound = 404,

	HTTP_STATUS_500_InternalServerError = 500,
	HTTP_STATUS_501_NotImplemented = 501,
};

class Rsp_Head
{
public:
	RSP_CODE _statusCode;
	string _statusMsg;

	Rsp_Head()
		: _statusCode(HTTP_STATUS_0_UNKNOWN)
	{}
};
struct Rsp_header_handle_Range_
{
public:
	char status[1];
	unsigned start;
	unsigned end;
};
class Req_
{
public:
	int Decode();//get head value
	REQ_METHOD _method;//http����
	string _uri;//�ļ���
	const char* _recv;//�������Ķ���
	Rsp_header_handle_Range_ _range;//range
	Req_()
		: _method(METHOD_UNSUPPORT), _uri("")
	{}
};
class Rsp_
{
public:
	Req_ req;
	int Render();//����_statusCode���_statusMsg
	int Encode();//��ȡ_result ,������ͷ�Ͱ���
	/*httpͷ�Լ������ļ���׺��*/
	Rsp_Head _head;
	/*http����*/
	string _body;
	string _result;/*���Է���*/
	/*����*/
	string getType();
	string ContentRange;
	Rsp_()
		:_body(""), _result(""), ContentRange("")
	{}
private:
	void http_content_type_conf();//��ȡcontent_type���ñ�
};
#endif