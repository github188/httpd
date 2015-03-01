#include "../include/analyhttp.h"
static char buf[TRANS_EACH] = { 0 };
static size_t trans_len, total, start, end;
void  DataHandle(string &data)
{
	Rsp_ rsp;
	rsp.req._recv = data.data();
	rsp.req.Decode();
	LOG(LOG_DEBUG, "filename = %s ", rsp.req._uri.c_str())
		ifstream in(rsp.req._uri.c_str(), ios::binary | ios::in);
	if (in.fail())//请求的文件名不存在
	{
		rsp._head._statusCode = HTTP_STATUS_404_NotFound;
		rsp._body = "<html><h1>404 NOT FOUND</h1></html>";
	}
	else
	{
		Rsp_header_handle_Range_ & _range_ = rsp.req._range;
		in.seekg(0, in.end);
		total = in.tellg();

		if (total > TRANS_EACH)
		{
			rsp._head._statusCode = HTTP_STATUS_302_Redirect;
		}
		else
		{
			if (_range_.status[0] == '0')
			{
				rsp._head._statusCode = HTTP_STATUS_200_OK;
				start = 0;
				trans_len = total;
			}
			else
			{
				rsp._head._statusCode = HTTP_STATUS_206_Range;
				start = _range_.start;
				end = _range_.end;
				if (start + 1 > total)
					start = 0;
				if (end == 0 || end + 1 > total)
					end = total - 1;
				trans_len = end - start + 1;
				sprintf(buf, "Content-Range: bytes %lu-%lu/%lu\r\n", start, end, total);
				rsp.ContentRange = buf;
			}
			in.seekg(start, in.beg);
			in.read(buf, trans_len);
			rsp._body.assign(buf, trans_len);
		}
	}

	rsp.Render();
	rsp.Encode();
	data.clear();
	data.resize(24);
	data.append(rsp._result);
	in.close();
}