#ifndef __HTTP_HEAD__
#define __HTTP_HEAD__
#include <string>
#include <map>
using namespace std;
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
	string get_version();
	map<string, string> get_kv();
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
	void set_version(string version);
	void set_kv(map<string, string> kv);
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
	map<string, string> kv;
	string req_body;
	string req_content;
};

#endif
