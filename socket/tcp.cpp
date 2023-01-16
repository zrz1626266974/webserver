#include "tcp.h"

#include <unistd.h>
#include <cstring>
#include <errno.h>

tcp::tcp(){
	init();
}

void tcp::init() {
	this->fd = -1;
	this->ip = "0"; //INADDR_ANY;
	this->port = 80;
	
	this->client_fd = -1;
	memset(&this->client_addr, 0, sizeof(struct sockaddr));
}

tcp::tcp(const tcp& t){
	cout << "copy constructor" << endl;
}

tcp::~tcp(){
	if (this->fd != -1) {
		close(this->fd);
	}
	
	if (this->client_fd != -1) {
		close(this->client_fd);
	}
	
	cout << "destroy constructor" << endl;
}

int tcp::new_server_socket(){
	if (this->fd != -1) {
		close(this->fd);
	}
	
	this->fd = new_server_socket(this->ip, this->port);
	
	return this->fd;
}

int tcp::new_socket(string ip, int port){
	cout << INADDR_ANY << "\n"
		 "ip:   " << ip << "\n"
		 "port: " << port << "\n"
		 << endl;
		 
	do {
		int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (fd < 0) {
			break;
		}
		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
		
		if (bind(fd, (sockaddr*)&addr, sizeof(struct sockaddr)) < 0) {
			break;
		}
		
		return fd;
	} while (0);
	
	return -1;
}

int tcp::new_server_socket(string ip, int port){
	cout << 
		 "ip:   " << ip << "\n"
		 "port: " << port << "\n"
		 << endl;

	do {
		int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (fd < 0) {
			break;
		}
		
		int opt = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		
		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
		
		if (bind(fd, (sockaddr*)&addr, sizeof(struct sockaddr)) < 0) {
			break;
		}
		
		if (listen(fd, 5) < 0) {
			break;
		}
		
		return fd;
	} while (0);
	
	return -1;
}

int tcp::new_client_socket(string ip, int port){
	cout << INADDR_ANY << "\n"
		 "ip:   " << ip << "\n"
		 "port: " << port << "\n"
		 << endl;
		 
	do {
		int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (fd < 0) {
			break;
		}
		
		int opt = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		
		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
		
		//if (bind(fd, (sockaddr*)&addr, sizeof(struct sockaddr)) < 0) {
		//	break;
		//}
		
		if (connect(fd, (sockaddr*)&addr, sizeof(struct sockaddr)) < 0) {
			break;
		}
		
		return fd;
	} while (0);
	
	return -1;
}

int tcp::accept_socket() {
	//struct sockaddr_in addr;
	if (this->client_fd != -1) {
		close(this->client_fd);
	}
	memset(&this->client_addr, 0, sizeof(struct sockaddr));
	socklen_t addr_len = sizeof(socklen_t);
	this->client_fd = accept(this->fd, (struct sockaddr*)&this->client_addr, &addr_len);
	return this->client_fd;
}

void tcp::print_client_info() {
	cout << "client ip: " << this->get_client_ip() << "\n"
		<< "client port: " << this->get_client_port() << "\n"
		<< endl;
}

int tcp::send_client_buffer(string buffer) {
	return send(this->client_fd, buffer.c_str(), buffer.length(), MSG_NOSIGNAL);
}

int tcp::recv_client_buffer() {
	char buffer[1024] = {0};
	int ret = recv(this->client_fd, buffer, sizeof(buffer), 0);
		
	this->set_recv_buffer(buffer);
	return ret;
}

//====================
// getter
//====================
int tcp::get_fd() {
	return this->fd;
}

string tcp::get_ip() {
	return this->ip;
}

int tcp::get_port() {
	return this->port;
}

int tcp::get_client_fd() {
	return this->client_fd;
}

struct sockaddr_in tcp::get_client_addr() {
	return this->client_addr;
}

string tcp::get_client_ip() {
	return inet_ntoa(this->client_addr.sin_addr);
}

int tcp::get_client_port() {
	return ntohs(this->client_addr.sin_port);
}

string tcp::get_recv_buffer() {
	return this->recv_buffer;
}

//====================
// setter
//====================
void tcp::set_fd(int fd) {
	this->fd = fd;
}

void tcp::set_ip(string ip) {
	this->ip = ip;
}

void tcp::set_port(int port) {
	this->port = port;
}

void tcp::set_client_fd(int fd) {
	this->client_fd = fd;
}

void tcp::set_client_addr(struct sockaddr_in client_addr) {
	this->client_addr = client_addr;
}

void tcp::set_recv_buffer(char *recv_buffer) {
	this->recv_buffer = recv_buffer;
}

void tcp::set_recv_buffer(string recv_buffer) {
	this->recv_buffer = recv_buffer;
}


