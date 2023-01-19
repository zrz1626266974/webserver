#include "http.h"
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdio>

#define FMT_HEADER 	"%s %s %s\r"
#define FMT_KV 		"%[^:]: %[^\r]\r"

/*HttpHead*/

HttpHead::HttpHead(){}
HttpHead::HttpHead(const HttpHead& head){}
HttpHead::~HttpHead(){}

string HttpHead::to_req_str()
{
	string result;
	
	stringstream buffer;
	/* request line */
	buffer << this->method << " " << this->url << " " << this->proto << "\r\n";
	
	/* kv line */
	for (map<string, string>::iterator it=this->kv.begin(); it != this->kv.end(); ++ it)
	{
		buffer << it->first << ": " << it->second << "\r\n";
	}
	
	/* empty line */
	buffer << "\r\n";
	
	result = buffer.str();
	return result;
}

string HttpHead::to_res_str()
{
	string result;
	
	stringstream buffer;
	/* response line */
	buffer << this->proto << " " << this->code << " " << this->status << "\r\n";
	
	/* kv line */
	for (map<string, string>::iterator it=this->kv.begin(); it != this->kv.end(); ++ it)
	{
		buffer << it->first << ": " << it->second << "\r\n";
	}
	
	/* empty line */
	buffer << "\r\n";
	
	result = buffer.str();
	return result;
}

void HttpHead::parse_req(string data)
{
	stringstream buffer(data);
	string line;
	
	getline(buffer, line);
	
	char proto[64] = {0};
	char url[512] = {0};
	char method[16] = {0};
	
	/* http request first line */
	if ( !line.empty() )
	{
		sscanf(line.c_str(), "%s %s %[^\r]\r", proto, url, method);
		this->proto 	= proto;
		this->url 		= url;
		this->method 	= method;
	}
	
	/* http key: value line */
	while ( getline(buffer, line) )
	{
		/* break when parse to http line "\r\n" */
		if ( line.empty() || line == "\r")
			break;

		char k[64] = {0};
		char v[512] = {0};
	
		sscanf(line.c_str(), "%[^:]: %[^\r\n]\r\n", k, v);
		
		this->kv[k] = v;
	}
	
}

void HttpHead::parse_res(string data)
{

}

void HttpHead::clear_kv()
{
	this->kv.clear();
}

/*setter*/
void HttpHead::set_proto(string proto)
{
	this->proto = proto;
}

void HttpHead::set_code(string code)
{
	this->code = code;
}

void HttpHead::set_status(string status)
{
	this->status = status;
}

void HttpHead::set_method(string method)
{
	this->method = method;
}

void HttpHead::set_url(string url)
{
	this->url = url;
}

void HttpHead::set_kv(map<string, string> kv)
{
	this->kv = kv;
}

/*getter*/
string HttpHead::get_proto()
{
	return this->proto;
}

string HttpHead::get_code()
{
	return this->code;
}

string HttpHead::get_status()
{
	return this->status;
}

string HttpHead::get_method()
{
	return this->url;
}

string HttpHead::get_url()
{
	return this->url;
}

map<string, string>& HttpHead::get_kv()
{
	return this->kv;
}



//--------------------------------------------------------------------------------------------------

/*HttpHead*/

Http::Http(){}
Http::Http(const Http& http){}
Http::~Http(){}

/*setter*/
void Http::set_req_head(HttpHead head)
{
	this->req_head = head;
}

void Http::set_res_head(HttpHead head)
{
	this->res_head = head;
}

void Http::set_body(string body)
{
	this->body = body;
}


/*getter*/
HttpHead Http::get_req_head()
{
	return this->req_head;
}

HttpHead Http::get_res_head()
{
	return this->res_head;
}

string Http::get_body()
{
	return this->body;
}

//--------------------------------------------------------------------------------------------------
/*old*/
http::http(){
	this->status_line = "HTTP/1.1 200 OK";
	this->msg_head = "";
	this->body = "";
	this->message = "";
	
}

http::http(const http& h){
	cout << "copy constructor" << endl;
}

http::http(string body){
	this->status_line = "HTTP/1.1 200 OK";
	this->msg_head = "";
	this->body = body;
	this->message = "";
	
}

http::~http(){}

void http::add_msg_head(string kv){
	//cout << "add key value :" << key_value << endl;
	this->msg_head = this->msg_head + kv + "\r\n";
}

void http::add_msg_head(map<string, string> kv) {
	if (!kv.empty()) {
		for (map<string, string>::iterator it = kv.begin(); it != kv.end(); it ++) {
			
			stringstream buffer;
			buffer << it->first
					<< ": "
					<< it->second;
					
			this->add_msg_head(buffer.str());
		}
	}
}

int http::parse_req_content(string req_content) {
	//cout << "===========parse req \n" << req_content << endl;
	if (req_content.empty()) {
		#if 0
		if (this->req_content.empty()) {
			return -1;
		}
		req_content = this->req_content;
		#endif
		return -1;
	}

	string item;
	string header;

	
	char method[10];
	char url[256];
	char version[16];
	
	this->req_header_kv.clear();
	
	stringstream buffer(req_content);
	
	getline(buffer, header);
	if (!header.empty()) {
		sscanf(header.c_str(), FMT_HEADER, method, url, version);
		
		this->method = method;
		this->url = url;
		this->version = version;
		
		cout << "method: " << this->method << "\n" 
			<< "url: " << this->url << "\n"
			<< "version: " << this->version << "\n"
			<< endl;
	}
	
	
	while (getline(buffer, item)) {
		if (item != "\r") {
			char key[64];
			char value[1024];
			sscanf(item.c_str(), FMT_KV, key, value);
			//cout << item << endl;
			cout << key << " : " << value << endl;
			this->req_header_kv[key] = value;
		} 
		else {
			break;
		}
	}
	
	string req_body;
	getline(buffer, req_body);
	if (!req_body.empty()) {
		this->req_body = req_body;
	}
	
	return 0;
}

int http::parse_req_url(string url) {
	if (url.empty()) {
		return -1;
	}
	
	char path[256] = {0};
	char req_data[256] = {0};
	sscanf(url.c_str(), "%[^?]?%s", path, req_data);
	cout << path << " : " << req_data << endl;
	
	stringstream buffer(req_data);
	string kv;
	
	while (getline(buffer, kv, '&')) {
		if (!kv.empty()) {
			
			char key[32], value[256];
			sscanf(kv.c_str(), "%[^=]=%s", key, value);
			this->req_data_kv[key] = value;
			cout << "\"" << kv << "\"" << endl;
		}
	}
	
	
	return 0;
}

int http::parse_req_body(string req_body) {
	if (req_body.empty()) {
		return -1;
	}
	
	stringstream buffer(req_body);
	string kv;
	
	while (getline(buffer, kv, '&')) {
		if (!kv.empty()) {
			
			char key[32], value[256];
			sscanf(kv.c_str(), "%[^=]=%s", key, value);
			this->req_data_kv[key] = value;
			cout << "\"" << kv << "\"" << endl;
		}
	}
	
	#if 0
	for (map<string, string>::iterator it = this->req_data_kv.begin(); it != this->req_data_kv.end(); ++ it) {
		cout << it->first << ": " << it->second << endl;
	}
	#endif
	return 0;
}

/************************/
/*		getter			*/
/************************/
string http::get_status_line(){
	return this->status_line;
}

string http::get_msg_head(){
	return this->msg_head;
}

string http::get_body(){
	return this->body;
}

string http::get_message(){
	stringstream buffer;
	buffer << this->status_line << "\r\n"
			<< this->msg_head
			<< "\r\n"
			<< this->body;
	this->message = buffer.str();
	return this->message;
}

// for client request
string http::get_method() {
	return this->method;
}

string http::get_url() {
	return this->url;
}

string http::get_path() {
	return this->path;
}

map<string, string> http::get_req_data_kv() {
	return this->req_data_kv;
}

string http::get_version() {
	return this->version;
}

map<string, string> http::get_req_header_kv() {
	return this->req_header_kv;
}

string http::get_req_body() {
	return this->req_body;
}

string http::get_req_content() {
	return this->req_content;
}
	
	
/************************/
/*		setter			*/
/************************/
void http::set_status_line(string status_line){
	this->status_line = status_line;
}

void http::set_msg_head(string msg_head){
	this->msg_head = msg_head;
}

void http::set_body(string body){
	this->body = body;
}

void http::set_message(string message){
	
}

// for client request
void http::set_method(string method) {
	this->method = method;
}

void http::set_url(string url) {
	this->url = url;
}

void http::set_path(string path) {
	this->path = path;
}

void http::set_req_data_kv(map<string, string> req_data_kv) {
	this->req_data_kv = req_data_kv;
}

void http::set_version(string version) {
	this->version = version;
}

void http::set_req_header_kv(map<string, string> req_header_kv) {
	this->req_header_kv = req_header_kv;
}

void http::set_req_data(string req_body) {
	this->req_body = req_body;
}

void http::set_req_content(string req_content) {
	this->req_content = req_content;
}




