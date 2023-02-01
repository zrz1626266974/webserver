#include "http.h"
#include "decode.h"
#include "tcp.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <codecvt>
#include <locale>
#define STATIC_DIR	"static/"

using namespace std;



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
	t.new_server_socket();
	cout << "server fd: " << t.get_fd()
		 << endl;
	
	cout << "... wait for client connection ..." << endl;
	t.accept_socket();
	t.print_client_info();
	
	
	cout << "===================================" << endl;
	http h;

	string file_name = STATIC_DIR"index.html";
	string content = read_file(file_name);
	

	cout << "===file content===" << endl;
	//cout << content << endl;
	
	string body = content;
	int content_len = body.length();
	//cout << content_len << endl;
	
	map<string, string> kv;
	//kv["connection"] = "close";
	kv["Cache-Control"] = "max-age=0, no-cache, no-store, must-revalidate";
	h.add_msg_head(kv);
	string kv_content_len = "Content-Length: ";
	kv_content_len.append(to_string(content_len));
	h.add_msg_head(kv_content_len);
	h.add_msg_head("Content-Type: text/html");
	h.set_body(body);
	//cout << h.get_message() << endl;
	//http h2 = h;

	


	
	Decode decoder;
	int cnt = 1;
	while (cnt) 
	{
		int ret = 0;
		//t.recv_client_buffer();
		cout << "... wait for client buffer ..." << endl;
		ret = t.recv_client_buffer();
		if (ret < 0)
			break;
		cout << "recv size: " << ret << endl;
		cout << "recv content: \n" << t.get_recv_buffer() << endl;
		#if 1
		cout << ">>>>>>>>>>> http parse start <<<<<<<<<<<<<" << endl;
		Http hp;
		HttpHead& head = hp.get_req_head();
		
		string recv_buffer = t.get_recv_buffer();
		head.parse_req(recv_buffer);
		
		cout << "===================================" << endl;
		cout << head.to_req_str() << endl;
		cout << "===================================" << endl;
		map<string, string> kv2 = head.get_kv();
		for (map<string, string>::iterator it = kv2.begin(); it != kv2.end(); ++ it) 
		{
			cout << it->first << ": " << it->second << endl;
		}
		cout << ">>>>>>>>>>> http parse end   <<<<<<<<<<<<<" << endl;
		#endif
		
		#if 1
		h.parse_req_content(t.get_recv_buffer());
		map<string, string> kv = h.get_req_header_kv();
		for (map<string, string>::iterator it = kv.begin(); it != kv.end(); ++ it) 
		{
			cout << it->first << ": " << it->second << endl;
		}
		h.parse_req_url(h.get_url());
		h.parse_req_body(h.get_req_body());
		#endif
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
				
				//h2.set_body(favicon);
				hp.set_body(favicon);
				ret = t.send_client_buffer(hp.get_res_content());
			} else {
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
				//kv2["connection"] = "close";
				string msg = hp.get_res_content();
				cout << "-------------------------------------------" << endl;
				cout << msg << endl;
				cout << "-------------------------------------------" << endl;
				#if 0
				string msg = h.get_message();
				#endif
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
		{
			map<string, string> kv = h.get_req_data_kv();
			string body = "{";
			for (map<string, string>::iterator it = kv.begin(); it != kv.end(); ++ it) 
			{
				body += "\"" + it->first + "\": \"" + decoder.decode_urlencode(it->second) + "\"";
				if ( distance(it, kv.end()) != 1) 
				{
					body += ",";
				}
			}
			
			body += "}";
			http tmp;
			tmp.set_body(body);
			map<string, string> kv2;
			kv2["Content-Length"] = to_string(body.length());
			//kv2["connection"] = "close";
			kv2["Content-Type"] = "application/json";
			tmp.add_msg_head(kv2);
			//cout << "body length == " << body.length() << endl;
			//cout << tmp.get_message() << endl;
			ret = t.send_client_buffer(tmp.get_message());
			//cout << body << endl;
		}
		
		
		if (ret < 0)
			break;
		cout << "send size: " << ret << endl;
		//t.recv_client_buffer();
	}

	
	cout << "===================================" << endl;
	//while (1);
	return 0;
}
