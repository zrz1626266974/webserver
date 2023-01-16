#ifndef __TCP_HEAD__
#define __TCP_HEAD__
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

class tcp{

public:
	tcp();
	tcp(const tcp& t);
	~tcp();
	
	void init();
	
	int new_server_socket();
	int new_socket(string ip, int port);
	int new_server_socket(string ip, int port);
	int new_client_socket(string ip, int port);
	
	int accept_socket();
	void print_client_info();
	int send_client_buffer(string buffer);
	int recv_client_buffer();
	
	/*setter*/
	int get_fd();
	string get_ip();
	int get_port();
	int get_client_fd();
	struct sockaddr_in get_client_addr();
	string get_client_ip();
	int get_client_port();
	string get_recv_buffer();
	
	/*getter*/
	void set_fd(int fd);
	void set_ip(string ip);
	void set_port(int port);
	void set_client_fd(int fd);
	void set_client_addr(struct sockaddr_in client_addr);
	void set_recv_buffer(char *recv_buffer);
	void set_recv_buffer(string recv_buffer);
	
private:
	int fd;
	string ip;
	int port;
	
	int client_fd;
	struct sockaddr_in client_addr;
	
	string recv_buffer;
};

#endif
