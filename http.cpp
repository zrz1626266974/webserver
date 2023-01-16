#include "http.h"
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdio>

#define FMT_HEADER 	"%s %s %s\r"
#define FMT_KV 		"%[^:]: %[^\r]\r"
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
	
	kv.clear();
	
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
			this->kv[key] = value;
		} 
		else {
			break;
		}
	}
	
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

string http::get_version() {
	return this->version;
}

map<string, string> http::get_kv() {
	return this->kv;
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

void http::set_version(string version) {
	this->version = version;
}

void http::set_kv(map<string, string> kv) {
	this->kv = kv;
}

void http::set_req_data(string req_body) {
	this->req_body = req_body;
}

void http::set_req_content(string req_content) {
	this->req_content = req_content;
}




