#include "http.h"
#include "decode.h"
#include "tcp.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <codecvt>
#include <locale>
#include <unistd.h>

#include <sys/epoll.h>
#include "nlohmann/json.hpp"
#define MAX_CLIENTS	100

#define STATIC_DIR	"static/"

using namespace std;
using json = nlohmann::json;


string read_file(string file_name)
{
	//int ret = -1;
	ifstream in(file_name, ios::in);
	if (!in.is_open()) 
	{
		return nullptr;
	}
	
	in.seekg(0, ios::end);
	int file_length = in.tellg();
	in.seekg(0);
	
	
	cout << ">>>>>>>>> " << file_length << " <<<<<<<<<" << endl;
	string content = "";
	while (!in.eof()) 
	{
		char buffer[128] = {0};
		in.read(buffer, 128);
		//getline(in, buffer);
		//cout << buffer << endl;
		content.append(buffer, in.gcount());
	}
	in.close();
	return content;
}

int main(int argc, char **argv) {
	
	cout << "======start=========" << endl;
	#if 0

	cout << "===================================" << endl;

	HttpHead head;
	map<string, string>& head_kv = head.get_kv();
	head_kv["test"] = "right";
	cout << head.to_req_str() << endl;
	cout << "===================================" << endl;
	return 0;
	#endif
	tcp t;
	t.set_port(8888);
		
	int maxfd = -1;
	int sev_fd = -1;
	sev_fd = t.new_server_socket();
	//t.new_server_socket();
	cout << "server fd: " << t.get_fd()
		 << endl;
	
	struct epoll_event events[MAX_CLIENTS];
	struct epoll_event ev;
	ev.data.fd = sev_fd;
	ev.events = EPOLLIN;
	int epfd = epoll_create(1);
	epoll_ctl(epfd, EPOLL_CTL_ADD, sev_fd, &ev);
	
	cout << "===================================" << endl;
	http h;

	Decode decoder;
	int cnt = 1;
	while (cnt) 
	{
	
		
		
		int fd = -1;
		
		int ret = 0;
		#if 1
		ret = epoll_wait(epfd, events, MAX_CLIENTS, -1);
		if (ret < 0)
		{
			cout << "[epoll wait error]" << endl;
			break;
		}
		
		if (ret == 0)
			continue;
		
		int i = 0;
		for (i = 0; i < ret; ++i)
		{
			int fd = events[i].data.fd;
			if ((fd == sev_fd) && (events[i].events & EPOLLIN))
			{
				int cli_fd = t.accept_socket();
				if (cli_fd < 0)
					continue;
				t.print_client_info();
				struct epoll_event event;
				event.data.fd = cli_fd;
				event.events = EPOLLIN;
				
				epoll_ctl(epfd, EPOLL_CTL_ADD, cli_fd, &event);
			}
			else if (events[i].events & EPOLLIN)
			{
				t.set_client_fd(events[i].data.fd);
				ret = t.recv_client_buffer();
				if (ret == 0)
				{
					epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					close(events[i].data.fd);
					
				}
				else 
				{
					cout << ">>>>>>>>>>> http parse start <<<<<<<<<<<<<" << endl;
					Http hp;
					HttpHead& head = hp.get_req_head();
		
					string recv_buffer = t.get_recv_buffer();
					//head.parse_req(recv_buffer);
					hp.parse_req(recv_buffer);
					cout << "===================================" << endl;
					cout << head.to_req_str() << endl;
					cout << "===================================" << endl;
					cout << hp.get_body() << endl;
					cout << "===================================" << endl;
					map<string, string> kv2 = head.get_kv();
					for (map<string, string>::iterator it = kv2.begin(); it != kv2.end(); ++ it) 
					{
						//cout << it->first << ": " << it->second << endl;
					}
					cout << ">>>>>>>>>>> http parse end   <<<<<<<<<<<<<" << endl;
					//#endif
					cout << " head method [" << head.get_method() << "]" << endl;
					if (head.get_method() == "GET") 
					{
						if (head.get_url() == "/favicon.ico") 
						{
							HttpHead& head = hp.get_res_head();
							head.set_proto("HTTP/1.1");
							head.set_status("OK");
							head.set_code("200");
				
							map<string, string>& kv2 = head.get_kv();
				
							kv2.clear();
				
							/***********************************************/
							// favicon
							string favicon_path = STATIC_DIR"favicon.ico";
							string favicon = read_file(favicon_path);
							/***********************************************/
				
							kv2["Cache-Control"] = "max-age=0";
							kv2["Content-Length"] = to_string(favicon.length());
							kv2["Content-Type"] = "image/x-icon";
				
							//kv2["connection"] = "keep-alive";
							//h2.set_body(favicon);
							hp.set_body(favicon);
							ret = t.send_client_buffer(hp.get_res_content());
						} else {
							string file_name = STATIC_DIR"index.html";
							string content = read_file(file_name);
							string body = content;
							#if 0
							cout << h.get_message() << endl;
							ret = t.send_client_buffer(h.get_message());
							#else
							HttpHead& head = hp.get_res_head();
							head.set_proto("HTTP/1.1");
							head.set_status("OK");
							head.set_code("200");
							hp.set_body(body);
							map<string, string>& kv2 = head.get_kv();
				
							kv2.clear();
				
							kv2["Content-Length"] = to_string(body.length());
							kv2["Content-Type"] = "text/html";
							//kv2["connection"] = "keep-alive";
							string msg = hp.get_res_content();
							cout << "-------------------------------------------" << endl;
							//cout << msg << endl;
							cout << "-------------------------------------------" << endl;
							//ret = t.send_client_buffer(msg);
				
							ret = 0;
							for (int i=0;; ++i) {
								int size = t.send_client_buffer(msg.substr(i*1024, 1024));
								ret += size;
								if (size < 1024) {
									break;
								}
							}
				
							#endif
				
						}
					}
					else if (head.get_method() == "POST") 
					{	string s = hp.get_body();
						auto j = json::parse(s.c_str());
						//cout << "===[" << j.at("userid") << endl;
						string body = j.dump();
						#if 0
						hp.parse_post_body();
						map<string, string> kv = hp.get_post_kv();
						map<string, string> kv_req = hp.get_req_head().get_url_kv();
						cout << "-------------------------------------------" << endl;
							//cout << msg << endl;
						for (map<string, string>::iterator it = kv_req.begin(); it != kv_req.end(); ++ it) 
						{
							cout << it->first << ": " << it->second << endl;
						}
			
						cout << "-------------------------------------------" << endl;
						//map<string, string> kv = h.get_req_data_kv();
						string body = "{";
						//body += "\"data\": {";
						for (map<string, string>::iterator it = kv.begin(); it != kv.end(); ++ it) 
						{
							body += "\"" + it->first + "\": \"" + decoder.decode_urlencode(it->second) + "\"";
							if ( distance(it, kv.end()) != 1) 
							{
								body += ",";
							}
						}
						//body +="}";
						body += "}";
						#endif
						cout << body << endl;
			
						HttpHead& head = hp.get_res_head();
						head.set_proto("HTTP/1.1");
			
						head.set_status("OK");
						head.set_code("200");
						hp.set_body(body);
						map<string, string>& kv2 = head.get_kv();
			
						kv2.clear();
			
						kv2["Content-Length"] = to_string(body.length());
						kv2["Content-Type"] = "application/json";
						kv2["Access-Control-Allow-Origin"] = "*";
						kv2["Access-Control-Request-Method"] = "POST, GET, OPTIONS, DELETE, PUT";
						kv2["Access-Control-Allow-Headers"] = "Origin, X-Requested-With, Content-Type, Accept";
						//kv2["connection"] = "keep-alive";
						string msg = hp.get_res_content();
						ret = t.send_client_buffer(msg);
					}
					else if (head.get_method() == "OPTIONS")
					{
						string body = "";
						HttpHead& head = hp.get_res_head();
						head.set_proto("HTTP/1.1");
			
						head.set_status("OK");
						head.set_code("200");
						hp.set_body(body);
						map<string, string>& kv2 = head.get_kv();
			
						kv2.clear();
			
						//kv2["Content-Length"] = to_string(body.length());
						//kv2["Content-Type"] = "application/json";
						kv2["Access-Control-Allow-Origin"] = "*";
						kv2["Access-Control-Request-Method"] = "POST, GET, OPTIONS, DELETE, PUT";
						kv2["Access-Control-Allow-Headers"] = "Origin, X-Requested-With, Content-Type, Accept";
						//kv2["connection"] = "keep-alive";
						string msg = hp.get_res_content();
						ret = t.send_client_buffer(msg);
					}
		
		
					if (ret < 0)
						break;
					cout << "send size: " << ret << endl;
					epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					close(events[i].data.fd);
				}
			}
		}
		
		#endif
		#if 0
		cout << "[... wait for client connection ...]" << endl;
		fd = t.accept_socket();
		if (fd < 0)
			continue;
		t.print_client_info();
		//t.recv_client_buffer();
		cout << "[... wait for client buffer ...]" << endl;
		ret = t.recv_client_buffer();
		if (ret <= 0)
			break;
		cout << "recv size: " << ret << endl;
		//cout << "recv content: \n" << t.get_recv_buffer() << endl;
		#if 1
		cout << ">>>>>>>>>>> http parse start <<<<<<<<<<<<<" << endl;
		Http hp;
		HttpHead& head = hp.get_req_head();
		
		string recv_buffer = t.get_recv_buffer();
		//head.parse_req(recv_buffer);
		hp.parse_req(recv_buffer);
		cout << "===================================" << endl;
		cout << head.to_req_str() << endl;
		cout << "===================================" << endl;
		cout << hp.get_body() << endl;
		cout << "===================================" << endl;
		map<string, string> kv2 = head.get_kv();
		for (map<string, string>::iterator it = kv2.begin(); it != kv2.end(); ++ it) 
		{
			//cout << it->first << ": " << it->second << endl;
		}
		cout << ">>>>>>>>>>> http parse end   <<<<<<<<<<<<<" << endl;
		#endif
		cout << " head method [" << head.get_method() << "]" << endl;
		if (head.get_method() == "GET") 
		{
			if (head.get_url() == "/favicon.ico") 
			{
				HttpHead& head = hp.get_res_head();
				head.set_proto("HTTP/1.1");
				head.set_status("OK");
				head.set_code("200");
				
				map<string, string>& kv2 = head.get_kv();
				
				kv2.clear();
				
				/***********************************************/
				// favicon
				string favicon_path = STATIC_DIR"favicon.ico";
				string favicon = read_file(favicon_path);
				/***********************************************/
				
				kv2["Cache-Control"] = "max-age=0";
				kv2["Content-Length"] = to_string(favicon.length());
				kv2["Content-Type"] = "image/x-icon";
				
				//kv2["connection"] = "keep-alive";
				//h2.set_body(favicon);
				hp.set_body(favicon);
				ret = t.send_client_buffer(hp.get_res_content());
			} else {
				string file_name = STATIC_DIR"index.html";
				string content = read_file(file_name);
				string body = content;
				#if 0
				cout << h.get_message() << endl;
				ret = t.send_client_buffer(h.get_message());
				#else
				HttpHead& head = hp.get_res_head();
				head.set_proto("HTTP/1.1");
				head.set_status("OK");
				head.set_code("200");
				hp.set_body(body);
				map<string, string>& kv2 = head.get_kv();
				
				kv2.clear();
				
				kv2["Content-Length"] = to_string(body.length());
				kv2["Content-Type"] = "text/html";
				//kv2["connection"] = "keep-alive";
				string msg = hp.get_res_content();
				cout << "-------------------------------------------" << endl;
				//cout << msg << endl;
				cout << "-------------------------------------------" << endl;
				//ret = t.send_client_buffer(msg);
				
				ret = 0;
				for (int i=0;; ++i) {
					int size = t.send_client_buffer(msg.substr(i*1024, 1024));
					ret += size;
					if (size < 1024) {
						break;
					}
				}
				
				#endif
				
			}
		}
		else if (head.get_method() == "POST") 
		{	string s = hp.get_body();
			auto j = json::parse(s.toStdString().c_str());
			cout << "===[" << j.at("a") << endl;
			string body = j.dump();
			#if 0
			hp.parse_post_body();
			map<string, string> kv = hp.get_post_kv();
			map<string, string> kv_req = hp.get_req_head().get_url_kv();
			cout << "-------------------------------------------" << endl;
				//cout << msg << endl;
			for (map<string, string>::iterator it = kv_req.begin(); it != kv_req.end(); ++ it) 
			{
				cout << it->first << ": " << it->second << endl;
			}
			
			cout << "-------------------------------------------" << endl;
			//map<string, string> kv = h.get_req_data_kv();
			string body = "{";
			//body += "\"data\": {";
			for (map<string, string>::iterator it = kv.begin(); it != kv.end(); ++ it) 
			{
				body += "\"" + it->first + "\": \"" + decoder.decode_urlencode(it->second) + "\"";
				if ( distance(it, kv.end()) != 1) 
				{
					body += ",";
				}
			}
			//body +="}";
			body += "}";
			#endif
			cout << "==========post body=============" << endl;
			cout << body << endl;
			
			HttpHead& head = hp.get_res_head();
			head.set_proto("HTTP/1.1");
			
			head.set_status("OK");
			head.set_code("200");
			hp.set_body(body);
			map<string, string>& kv2 = head.get_kv();
			
			kv2.clear();
			
			kv2["Content-Length"] = to_string(body.length());
			kv2["Content-Type"] = "application/json";
			kv2["Access-Control-Allow-Origin"] = "*";
			kv2["Access-Control-Request-Method"] = "POST, GET, OPTIONS, DELETE, PUT";
			kv2["Access-Control-Allow-Headers"] = "Origin, X-Requested-With, Content-Type, Accept";
			//kv2["connection"] = "keep-alive";
			string msg = hp.get_res_content();
			ret = t.send_client_buffer(msg);
		}
		else if (head.get_method() == "OPTIONS")
		{
			string body = "";
			HttpHead& head = hp.get_res_head();
			head.set_proto("HTTP/1.1");
			
			head.set_status("OK");
			head.set_code("200");
			hp.set_body(body);
			map<string, string>& kv2 = head.get_kv();
			
			kv2.clear();
			
			//kv2["Content-Length"] = to_string(body.length());
			//kv2["Content-Type"] = "application/json";
			kv2["Access-Control-Allow-Origin"] = "*";
			kv2["Access-Control-Request-Method"] = "POST, GET, OPTIONS, DELETE, PUT";
			kv2["Access-Control-Allow-Headers"] = "Origin, X-Requested-With, Content-Type, Accept";
			//kv2["connection"] = "keep-alive";
			string msg = hp.get_res_content();
			ret = t.send_client_buffer(msg);
		}
		
		
		if (ret < 0)
			break;
		cout << "send size: " << ret << endl;
		close(fd);
		#endif
		//t.recv_client_buffer();
	}

	
	cout << "===================================" << endl;
	//while (1);
	return 0;
}
