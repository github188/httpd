#include "../include/analyopr.h"
static char buf[256] = { 0 };
int Req_::Decode()
{
	string recv = "";
	string _method_ = "";
	string _head_ = "";
	string value = "";
	recv.assign(_recv);
	//得到是GET还是..
	size_t _head_pos = recv.find(" HTTP/");//确认HTTP协议
	_head_ = recv.substr(0, _head_pos);//获取协议头
	size_t _method_pos = _head_.find_first_of(' ');
	_method_ = _head_.substr(0, _method_pos);//获取METHOD
	if (_method_ == "GET")
	{
		_method = METHOD_GET;
	}
	else if (_method_ == "POST")
	{
		_method = METHOD_POST;
	}
	else
	{
		_method = METHOD_UNSUPPORT;
	}

	//取得文件名
	_uri = recv.substr(_method_pos + 1, _head_pos - _method_pos - 1);
	if (_uri == "/" || _uri == "")
		_uri = INDEXPAGE;
	/*若有range,获取range范围*/
	getValueByKey(recv, "Range", value);
	if (value == "")
	{
		_range.status[0] = '0';
	}
	else
	{
		_range.status[0] = '1';
		size_t e_pos = value.find('=');
		value = value.substr(e_pos + 1);
		size_t r_pos = value.find('-');
		_range.start = atoi(value.substr(0, r_pos).c_str());
		_range.end = atoi(value.substr(r_pos + 1).c_str());
		LOG(LOG_TRACE, "_range start %u _range end %u", _range.start, _range.end)
	}
	return 0;
}
int Rsp_::Encode()
{
	size_t result_size;
	size_t body_size = _body.length();
	//响应头及相应码
	sprintf(buf, "HTTP/1.1 %d %s\r\n", _head._statusCode, _head._statusMsg.c_str());
	_result += buf;
	/*分支处理段开始*/
	_result += ContentRange;
	_result += getType();//类型
	sprintf(buf, "Content-Length: %lu\r\n", body_size);
	_result += buf;
	_result += "Accept-Ranges: bytes\r\n";
	_result += "Connection: keep-alive\r\n";
	/*分支处理段结束*/
	_result += "\r\n";
	result_size = _result.length();
	_result.resize(body_size + result_size);
	memcpy((char *)&_result[result_size], _body.c_str(), body_size);
	return 0;
}
int Rsp_::Render()
{
	switch (_head._statusCode)
	{

	case HTTP_STATUS_200_OK:
		_head._statusMsg = "OK";
		break;
	case HTTP_STATUS_206_Range:
		_head._statusMsg = "Partial Content";
		break;

	case HTTP_STATUS_302_Redirect:
		_head._statusMsg = "Moved Temporarily";
		break;

	case HTTP_STATUS_404_NotFound:
		_head._statusMsg = "Not Found";
		break;

	case HTTP_STATUS_500_InternalServerError:
		_head._statusMsg = "Internal Server Error";
		break;

	case HTTP_STATUS_501_NotImplemented:
		_head._statusMsg = "Not Implemented";
		break;

	case HTTP_STATUS_0_UNKNOWN:
	default:
		_head._statusCode = HTTP_STATUS_500_InternalServerError;
		_head._statusMsg = "Internal Server Error";
		break;
	}

	return 0;
}
string Rsp_::getType()
{
	string giveback = "";
	string suffix = req._uri.substr(req._uri.find_last_of('.') + 1);
	if (content_type_conf.length() == 0)
	{
		http_content_type_conf();//获取传输种类配置表
	}
	__SIZE_TYPE__ pos_start = content_type_conf.find("'" + suffix + "'") + suffix.length() + 7;
	__SIZE_TYPE__ pos_end = content_type_conf.find_first_of(',', pos_start) - 1;
	if (pos_start != string::npos)
	{
		giveback += "Content-Type: ";
		giveback += content_type_conf.substr(pos_start, pos_end - pos_start).c_str();
		giveback += ";charset:utf-8\r\n";

	}
	else
	{
		giveback += "Content-Type: application/x-download\r\n";
		giveback += "Content-Disposition: attachment; filename = '" + req._uri + "'\r\n";
	}
	return giveback;
}
void Rsp_::http_content_type_conf()
{
	ifstream in(CONTENT_TYPE_CONF, ios::binary | ios::in);
	if (!in.is_open())//请求的文件名不存在
	{
		LOG(LOG_ERROR, "content_type_conf no exit.");
		//发送警报给主进程
	}
	else
	{
		content_type_conf.assign(istreambuf_iterator<char>(in), istreambuf_iterator<char>());
	}
	in.close();
}