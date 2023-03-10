#ifndef __HTTP_HEAD__
#define __HTTP_HEAD__
#include <string>
#include <map>
using namespace std;

class HttpHead {
	
public:
	HttpHead();
	HttpHead(const HttpHead& head);
	~HttpHead();
	
	string to_req_str();
	string to_res_str();
	
	void parse_req(string data);
	void parse_res(string data);
	void parse_url();
	//void add_kv(string k, string v);
	
	void clear_kv();
	
	/*setter*/
	void set_proto(string proto);
	void set_code(string code);
	void set_status(string status);
	void set_method(string method);
	void set_url(string url);
	void set_kv(map<string, string> kv);
	void set_url_kv(map<string, string> kv);
	/*getter*/
	string get_proto();
	string get_code();
	string get_status();
	string get_method();
	string get_url();
	map<string, string>& get_kv();
	map<string, string>& get_url_kv();
//==============
//for response
//==============
	string code;
	string status;
	
//==============
//for request
//==============
	string method;
	string url;

	string proto;
	map<string, string> kv;
	
	map<string, string> url_kv;
};

class Http {

public:
	Http();
	Http(const Http& http);
	~Http();
	
	string get_req_content();
	string get_res_content();
	
	void parse_req(string content);
	void parse_res(string content);
	void parse_post_body();
	/*setter*/
	void set_req_head(HttpHead head);
	void set_res_head(HttpHead head);
	void set_body(string body);
	void set_post_kv(map<string, string> kv);
	
	/*getter*/
	HttpHead& get_req_head();
	HttpHead& get_res_head();
	string get_body();
	map<string, string> get_post_kv();
private:
//==============
//for response
//==============

	/*response head*/
	HttpHead res_head;
	
//==============
//for request
//==============

	/*request head*/
	HttpHead req_head;
	
	string body;
	
	map<string, string> post_kv;
};


class http{

public:
	http();
	http(const http& h);
	http(string body);
	
	~http();
	
	void add_msg_head(string kv);
	void add_msg_head(map<string, string> kv);
	
	int parse_req_content(string req_content);
	int parse_req_url(string url);
	int parse_req_body(string req_body);
	
/************************/
/*		getter			*/
/************************/
	string get_status_line();
	string get_msg_head();
	string get_body();
	string get_message();
	
	// for client request
	string get_method();
	string get_url();
	string get_path();
	map<string, string> get_req_data_kv();
	
	string get_version();
	map<string, string> get_req_header_kv();
	string get_req_body();
	string get_req_content();
	
/************************/
/*		setter			*/
/************************/
	void set_status_line(string status_line);
	void set_msg_head(string msg_head);
	void set_body(string body);
	void set_message(string message);
	
	// for client request
	void set_method(string method);
	void set_url(string url);
	void set_path(string path);
	void set_req_data_kv(map<string, string> req_data_kv);
	void set_version(string version);
	void set_req_header_kv(map<string, string> req_header_kv);
	void set_req_data(string req_body);
	void set_req_content(string req_content);
	
private:
	string status_line;
	string msg_head;
	string body;
	string message;
	
	// for client request
	string method;
	string url;
	string path;
	map<string, string> req_data_kv;
	string version;
	map<string, string> req_header_kv;
	string req_body;
	string req_content;
};

#endif
