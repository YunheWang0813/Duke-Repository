#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include "http.h"
#include <assert.h>
#include <thread>
#include <unordered_map>
using namespace std;
struct addrinfo host_info;
struct addrinfo *host_info_list;

const char *hostname = NULL;
const char *port     = "12345";
int status;
int socket_fd;//服务器端fd
int client_connection_fd;//客户端fd(browser)
int client_socket_fd;//


void set_server(){
    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    } //if

    socket_fd = socket(host_info_list->ai_family,
                       host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    } //if

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot bind socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    } //if

    status = listen(socket_fd, 1000);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    } //if
}

void proxy(int client_connection_fd,int id,string ip_addr){
    http HTTP(client_connection_fd,id,ip_addr);
}
int main(int argc, char *argv[]) {
    //daemon(0,0);
    set_server();//initialize the socket
    vector<thread> threads;
    int id = 0;
    file.open("log.txt", fstream::out | fstream::app);
    while(1){
        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        client_connection_fd = accept(socket_fd, (struct sockaddr *) &socket_addr, &socket_addr_len);
        if (client_connection_fd == -1) {
            cerr << "Error: cannot accept connection on socket" << endl;
            exit(EXIT_FAILURE);
        }
        string ip_addr(inet_ntoa(((struct sockaddr_in *)&socket_addr)->sin_addr));//获取浏览器ip地址

        thread(proxy,client_connection_fd,id,ip_addr).detach();
        id++;
    }
}
