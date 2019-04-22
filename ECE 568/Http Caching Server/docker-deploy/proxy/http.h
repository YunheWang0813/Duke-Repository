#ifndef SRC_HTTP_H
#define SRC_HTTP_H

#include "cache.h"
#include <arpa/inet.h>
#include <assert.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using namespace std;
ofstream file;
ofstream new_file;

size_t cache_size = 1000000000;
size_t curr_cache_size = 0;
unordered_map<string, CacheNode *> cache_map;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

class http {
  string request;
  string response;
  string response_header_str;
  string request_header_str;
  vector<char> request_header;
  vector<char> response_header;
  vector<char> whole_request;
  vector<char> whole_response;
  string method;
  string response_code;
  string host_url;
  string protocol;
  string header_firstline;
  string response_protocol;
  string response_line;
  char buffer[65535];
  map<string, string> request_header_map;
  map<string, string> response_header_map;
  int client_connection_fd;
  int socket_fd;
  int thread_id;
  string ip_address;
  bool cache;
  vector<char> req;
  // vector<char> response;
  bool is_valid;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;

public:
  explicit http(int client_fd, int id, string ip_addr) {
    client_connection_fd = client_fd;
    thread_id = id;
    ip_address = ip_addr;
    //*******receive request from browser********//
    try {
      // receive_request_whole();
      receive_request_header();
      // GetHeader();
      request_header_str = &request_header.data()[0];
      request = request_header_str;
      // request = &whole_request.data()[0];
      parse_request();
      handle_request(); //处理请求--发送给服务器并将响应传给浏览器
    } catch (const char *mess) {
      return;
    }
  }

  void receive_request_whole() { // fill the request -header
    ssize_t recv_length;         // = recv(fd, buff, 65535, 0);
    int curr = 0;
    whole_request.resize(1);
    while (1) {
      recv_length =
          recv(client_connection_fd, &whole_request.data()[curr], 65535, 0);
      if (recv_length == 0) {
        break;
      }
      curr++;
      ssize_t new_size = whole_request.size();
      whole_request.resize(new_size + 1);
    }
    if (whole_request.empty() ||
        (!whole_request.empty() && whole_request[0] != 'G' &&
         whole_request[0] != 'P' && whole_request[0] != 'C')) {
      close(client_connection_fd);
      throw "Bad Request";
    }
  }
  void receive_response_body_content_length(int content_length) {
    ssize_t recv_length = 0;
    size_t whole_response_size = whole_response.size();
    whole_response.resize(whole_response_size + 1);
    ssize_t received_len = 0;
    size_t curr = whole_response_size;
    while (1) {
      recv_length = recv(socket_fd, &whole_response.data()[curr], 1, 0);
      received_len++;
      curr++;
      ssize_t curr_new = whole_response.size();
      if (whole_response[curr_new - 1] == '\n' &&
          whole_response[curr_new - 2] == '\r' &&
          whole_response[curr_new - 3] == '\n' &&
          whole_response[curr_new - 4] == '\r') {
        break;
      }
      if (received_len == content_length) {
        break;
      }
      whole_response.resize(curr_new + 1);
    }
    // recv_length =
    // recv(socket_fd,&whole_response.data()[whole_response_size],content_length,0);
    response = &whole_response.data()[0]; // fill the whole response
  }
  void receive_response_body_chunked() {
    ssize_t recv_length = 0;
    size_t whole_response_size = whole_response.size();
    whole_response.resize(whole_response_size + 1);
    size_t curr = whole_response_size;

    send(client_connection_fd, &response_header.data()[0],
         response_header.size(), 0);
    while (1) {
      vector<char> temp_buffer(1);
      // temp_buffer.clear();
      recv_length = recv(socket_fd, &whole_response.data()[curr], 1, 0);
      if (recv_length < 0) {
        close(client_connection_fd);
        close(socket_fd);
        return;
      } else if (recv_length == 0) {
        break;
      }
      ssize_t size_new = whole_response.size();
      if (whole_response.data()[size_new - 1] == '\n' &&
          whole_response.data()[size_new - 2] == '\r' &&
          whole_response.data()[size_new - 3] == '\n' &&
          whole_response.data()[size_new - 4] == '\r' &&
          whole_response.data()[size_new - 5] == '0') {
        break;
      }
      send(client_connection_fd, &whole_response.data()[curr], 1, 0);
      curr++;
      size_t curr_response_size = whole_response.size();
      whole_response.resize(curr_response_size + 1);
    }

    response = &whole_response.data()[0];
  }
  void receive_response_header() {
    ssize_t recv_length; // = recv(fd, buff, 65535, 0);
    int curr = 0;
    response_header.resize(1);
    size_t find_crlf = 0;
    while (1) {
      // cout<<"receiving ..."<<endl;
      recv_length = recv(socket_fd, &response_header.data()[curr], 1, 0);
      if (recv_length < 0) {
        perror("receive error");
        break;
      } else if (recv_length == 0) {
        break;
      }
      size_t size_new = response_header.size();
      if (response_header.data()[size_new - 1] == '\n' &&
          response_header.data()[size_new - 2] == '\r' &&
          response_header.data()[size_new - 3] == '\n' &&
          response_header.data()[size_new - 4] == '\r') {
        break;
      }
      curr++;
      size_t curr_size = response_header.size();
      response_header.resize(curr_size + 1);
    }
    if (request_header.empty() ||
        (!request_header.empty() && request_header[0] != 'G' &&
         request_header[0] != 'P' && request_header[0] != 'C')) {
      close(client_connection_fd);
      throw "Bad Request";
    }
    response_header_str = &response_header.data()[0];
  }
  void receive_request_header() {
    ssize_t recv_length; // = recv(fd, buff, 65535, 0);
    int curr = 0;
    request_header.resize(1);
    size_t find_crlf = 0;
    while (1) {
      recv_length =
          recv(client_connection_fd, &request_header.data()[curr], 1, 0);
      if (recv_length < 0) {
        perror("receive error");
        break;
      } else if (recv_length == 0) {
        break;
      }
      size_t size_new = request_header.size();
      if (request_header.data()[size_new - 1] == '\n' &&
          request_header.data()[size_new - 2] == '\r' &&
          request_header.data()[size_new - 3] == '\n' &&
          request_header.data()[size_new - 4] == '\r') {
        break;
      }
      curr++;
      size_t curr_size = request_header.size();
      request_header.resize(curr_size + 1);
    }
    if (request_header.empty() ||
        (!request_header.empty() && request_header[0] != 'G' &&
         request_header[0] != 'P' && request_header[0] != 'C')) {
      close(client_connection_fd);
      throw "Bad Request";
    }
    request_header_str = &request_header.data()[0];
    // assert(request_header_str.find("\r\n\r\n")!=string::npos&&request_header[request_header.size()-1]=='\n');
    //        cout << "***************request header is_************** " <<
    //        endl;
    //        //可以判断一下response是不是合法的
    //        for (size_t i = 0; i < request_header.size(); i++) {
    //            cout << request_header[i]<<" ";
    //        }
    //        cout<<"request header is "<<request_header_str<<endl;
    //        cout << endl << "*************response header
    //        end*****************" << endl;
  }

  void receive_response_and_send() {
    size_t found = response_header_str.find("\r\n");
    if (found != string::npos) {
      string response_header_first_line = response_header_str.substr(0, found);
      new_file.open("/var/log/erss/proxy.log", ofstream::out);
      new_file << to_string(thread_id) << ": Received \""
               << response_header_first_line << "\" from " << parse_host()
               << "\n";
      new_file.close();
    }

    whole_response = response_header;
    size_t whole_response_size = whole_response.size();

    auto it = response_header_map.find("Content-Length");

    /*****content length********/
    if (it != response_header_map.end()) {
      ssize_t content_length = stoi(it->second);
      receive_response_body_content_length(content_length);
      send(client_connection_fd, &whole_response.data()[0],
           whole_response.size(), 0);

      /*******chunked********/
    } else if (response_header_str.find("chunked") != string::npos) {
      receive_response_body_chunked(); // including send to client//

      /********not content-length and chunked -- only header *********/
    } else {
      send(client_connection_fd, &response_header.data()[0],
           response_header.size(), 0);
    }
    new_file.open("/var/log/erss/proxy.log", ofstream::out | ofstream::trunc);
    new_file << to_string(thread_id) << ": Responding \"" << response_line
             << "\"\n";
    new_file.close();
    close(socket_fd);
    close(client_connection_fd);
  }
  void receive_response_body() {}
  // parse the http request
  void parse_request() {
    // first line of the request
    if (request.find("\r\n") != string::npos) {
      header_firstline = request.substr(0, request.find("\r\n"));
    }
    // method
    size_t first_space = request.find_first_of(" ", 0);
    if (first_space != string::npos) {
      method = request.substr(
          0,
          first_space); // second argument is the length from the start position
      if (method != "GET" && method != "POST" && method != "CONNECT") {
        exit(EXIT_FAILURE);
      }
    }

    // URL
    size_t second_space = request.find(
        " ", first_space + 1); // second argument is the start position
    if (second_space != string::npos) {
      host_url =
          request.substr(first_space + 1, second_space - first_space - 1);
    }
    // protocol HTTP/1.1 or HTTP/1.0
    size_t first_newline = request.find("\n", 0);
    if (first_newline != string::npos) {
      protocol =
          request.substr(second_space + 1, first_newline - second_space - 2);
    }

    // header end with \r\n
    //判断是不是cache
    size_t found_cache = request.find("no-cache", 0);
    if (found_cache != string::npos) {
      cache = false;
    } else {
      cache = true;
    }
    size_t newLine = request.find("\n", first_newline + 1);
    while (newLine != string::npos) {
      string line = request.substr(first_newline, newLine - first_newline - 1);
      size_t colon = line.find(":", 0);
      string head_name = line.substr(0, colon);
      string head_value =
          line.substr(colon + 1); //****including the first white space
      request_header_map.insert(pair<string, string>(head_name, head_value));
      first_newline = newLine + 1;
      newLine = request.find("\n", first_newline + 1);
    }
  }

  void parse_response() {
    size_t first_newline = response_header_str.find("\n", 0);
    if (first_newline != string::npos) {
      response_line = response_header_str.substr(0, first_newline);
    } else {
      throw "bad response";
    }
    size_t find_protocol = response_line.find(" ", 0);
    if (find_protocol != string::npos) {
      response_protocol =
          response_line.substr(0, find_protocol); // should be HTTP1.0/
    } else {
      throw "bad response";
    }
    string rest = response_line.substr(find_protocol + 1);
    size_t find_code = rest.find(" ", 0);
    if (find_code != string::npos) {
      response_code = rest.substr(0, find_code);
    } else {
      throw "bad response";
    }
    // header end with \r\n
    //判断是不是cache
    size_t newLine = response_header_str.find("\n", first_newline + 1);
    while (newLine != string::npos) {
      string line = response_header_str.substr(first_newline,
                                               newLine - first_newline - 1);
      // cout<<"the head is"<<line<<"**"<<endl;
      // header name
      size_t colon = line.find(":", 0);
      string head_name = line.substr(0, colon);
      string head_value =
          line.substr(colon + 1); //****including the first white space
      response_header_map.insert(pair<string, string>(head_name, head_value));
      first_newline = newLine + 1;
      newLine = response_header_str.find("\n", first_newline + 1);
    }
  }

  string parse_port() {
    string host_line;
    if (request.find("Host:") != string::npos) {
      host_line = request.substr(request.find("Host:") + 6);
    } else {
    }
    string port_number = host_line.substr(0, host_line.find("\r\n"));
    int port = 80;
    string temp = "80";
    if (port_number.find(":") != std::string::npos) {
      port_number = port_number.substr(port_number.find(":") + 1);
      temp = port_number.substr(0, port_number.find("\r\n"));
      // std::cout<<"port num "<<temp<<std::endl;
      port = std::stoi(temp);
    }
    return temp;
  }

  string parse_host() {
    string recv_requ = request;
    std::string host;
    size_t found_host = request.find("Host: ");
    if (found_host != string::npos) {
      recv_requ = recv_requ.substr(found_host + 6);
    }
    size_t found_crlf = recv_requ.find("\r\n");
    if (found_host != string::npos) {
      host = recv_requ.substr(0, found_crlf);
    }
    if (host.find(":") != std::string::npos) {
      host = host.substr(0, host.find(":"));
    }
    return host;
  }

  void handle_request() {

    //&********write request into log***********//
    time_t Time = time(NULL);
    new_file.open("", ios::app);
    // new_file<<"open success"<<endl;
    new_file << thread_id << " : " << header_firstline << " from " << ip_address
             << " @ " << asctime(gmtime(&Time));

    string req_log = to_string(thread_id) + " : " + "Requesting \"";
    new_file << req_log << header_firstline << "\" from " << parse_host()
             << "\n";
    new_file.close();

    //&**********connecting with original server **********//
    try {
      socket_fd = set_client(parse_host());
    } catch (const char *bad_request) {
      // cout << bad_request << endl;
      throw "Bad Request";
    }

    //&**********deal with request method ****************//
    if (method == "GET") { // "GET" request
      try {
        // handle_get_request();
        pthread_mutex_lock(&lock);
        get_method_cache();
        pthread_mutex_unlock(&lock);
      } catch (const char *mess) {
        // cout << mess << endl;
        throw "bad response";
      }

    } else if (method == "POST") { //"POST" request
      handle_post_request();

    } else if (method == "CONNECT") {
      handle_connect_request();
    } else {
      // cout<<"invalid request"<<endl;
    }
  }

  void add_to_cache(bool need_revalidation, int max_age) {
    if (curr_cache_size + whole_response.size() >
        cache_size) { // need to evict a block
      if (cache_map.empty()) {
        return;
      }
      curr_cache_size -= (cache_map.begin()->second->response_content).size();
      cache_map.erase(cache_map.begin());

      curr_cache_size += whole_response.size();
      CacheNode *cache_node = new CacheNode();
      cache_node->response_content = &whole_response.data()[0];
      time_t Time = time(NULL);
      asctime(gmtime(&Time));
      cache_node->create_time = Time;
      cache_node->expire_time = max_age; // 10 years later
      auto it = response_header_map.find("ETag");
      if (it != response_header_map.end()) {
        cache_node->Etag = it->second;
        // cout<<"Etag is "<<cache_node->Etag<<endl;
      }
      cache_node->revalid_required = need_revalidation;
      cache_map.insert(pair<string, CacheNode *>(host_url, cache_node));
    }
  }

  void get_method_cache() {
    // check_cache();
    auto it_cache_control = request_header_map.find("Cache-Control");
    string cache_control;
    if (it_cache_control != request_header_map.end()) { // find the
                                                        // cache-control
      cache_control = it_cache_control->second;
    } else {
    }
    if (cache_control == "no-store") { // do not need cache
      //-----------get reqeust--write into log-----------------
      new_file.open("/var/log/erss/proxy.log", ios::app);
      new_file << thread_id << " : not in cache\n";
      new_file.close();
      //----------------------------
      handle_get_request();
      //---------------------------------
      if (response_code == "200") {
        new_file.open("/var/log/erss/proxy.log", ios::app);
        new_file << thread_id << " : not cacheable because no-store\n";
        new_file.close();
      }
      //------------------------------
      return;
    }
    auto it1 = cache_map.find(host_url);
    auto it2 = request_header_map.find("Expires");
    auto it3 = request_header_map.find("Last-Modified");
    if (it1 ==
        cache_map
            .end()) { // cannot find the response in cache--add  to the cache
      //----------------------------
      new_file.open("/var/log/erss/proxy.log", ios::app);
      new_file << thread_id << " : not in cache\n";
      new_file.close();
      //----------------------------
      handle_get_request();
      time_t Time = time(NULL);
      asctime(gmtime(&Time));
      if (cache_control == "no-cache") {
        pthread_mutex_lock(&lock);
        add_to_cache(true, Time + 315360000);
        pthread_mutex_unlock(&lock);
        // ----------------------------
        if (response_code == "200") {
          new_file.open("/var/log/erss/proxy.log", ios::app);
          new_file << thread_id << " : cached, but requires re-validation\n";
          new_file.close();
        }
        // ----------------------------
      } else if (cache_control.find("s-maxage") != string::npos) {
        size_t max_age = cache_control.find("=");
        string max_str = cache_control.substr(max_age + 1);
        int max_age_int = stoi(max_str);
        pthread_mutex_lock(&lock);
        add_to_cache(false, max_age_int);
        pthread_mutex_unlock(&lock);
        // ----------------------------
        if (response_code == "200") {
          new_file.open("/var/log/erss/proxy.log", ios::app);
          new_file << thread_id << " : cached, expires at " << max_age_int
                   << "\n";
          new_file.close();
        }
        // ----------------------------
      } else if (cache_control.find("max-age") != string::npos) {
        size_t max_age = cache_control.find("=");
        string max_str = cache_control.substr(max_age + 1);
        int max_age_int = stoi(max_str);
        //                cout << "max-age is " << max_age_int << endl;
        pthread_mutex_lock(&lock);
        add_to_cache(false, max_age_int);
        pthread_mutex_unlock(&lock);
        // ----------------------------
        if (response_code == "200") {
          new_file.open("/var/log/erss/proxy.log", ios::app);
          new_file << thread_id << " : cached, expires at " << max_age_int
                   << "\n";
          new_file.close();
        }
        // ----------------------------

      } else if (it3 != response_header_map.end()) { // Last-Modified

      } else if (it2 != response_header_map.end()) { // expire time
        add_to_cache(false, Time_Modify(it2->second));
        // ----------------------------
        if (response_code == "200") {
          new_file.open("/var/log/erss/proxy.log", ios::app);
          new_file << thread_id << " : cached, expires at " << it2->second
                   << "\n";
          new_file.close();
        }
        // ----------------------------
      }

      return;
    }
    // found cache
    bool Revalidation_Needed = it1->second->revalid_required;
    time_t curr_time;
    time(&curr_time);
    double diff_time = difftime(curr_time, it1->second->expire_time);
    bool expired = (diff_time > 0) ? false : true;

    if (Revalidation_Needed || expired) {
      string etag = it1->second->Etag;
      string request_append = "If-None-Match: " + etag + "\r\n\r\n";
      string new_request =
          request.substr(0, request.size() - 2) + request_append;
      send(socket_fd, new_request.c_str(), new_request.size(), 0);
      // ".........................................."
      // "      send new_request to server          "
      // ".........................................."
      // ".........................................."
      // "             recv header                  "
      // ".........................................."
      receive_response_header();
      try {
        parse_response();
      } catch (const char *mess) {
        cout << mess << endl;
      }
      if (response_code == "304") {
        string resp = it1->second->response_content;
        send(client_connection_fd, resp.c_str(), resp.size(), 0);
        //!!!not update the response header in the cache

        // ".........................................."
        // "       send back directly to user         "
        // ".........................................."

        size_t n = response.find("\r\n");
        new_file.open("/var/log/erss/proxy.log", ios::app);
        new_file << to_string(thread_id) << ": Responding \""
                 << resp.substr(0, n) << "\""
                 << "\n";
        new_file.close();
      } else if (response_code == "200") {
        receive_response_and_send();
        // ".........................................."
        // "   recv content and send back to user     "
        // ".........................................."
      }
    }
    // no need of revalidation
    else {
      string resp = it1->second->response_content;
      send(client_connection_fd, resp.c_str(), resp.size(), 0);
      // ".........................................."
      // "       send back directly to user         "
      // ".........................................."

      size_t n = response.find("\r\n");
      new_file.open("/var/log/erss/proxy.log", ios::app);
      new_file << to_string(thread_id) << ": Responding \"" << resp.substr(0, n)
               << "\""
               << "\n";
      new_file.close();
    }

    //---------find the request in the map----------- (it1->second is the
    //cacheNode)
    //        if(cache_control=="no-cache"|| it1->second->revalid_required){
    //
    //        }
    // handle_get_request();
  }
  time_t Time_Modify(string &temp_str) {
    time_t res_time;
    // Thu, 27 Feb 2020 00:31:29 GMT
    vector<string> days = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    string day_str = temp_str.substr(0, 3);
    int day_num;
    for (int i = 0; i < 7; i++) {
      if (day_str == days[i]) {
        day_num = i;
        break;
      }
    }

    temp_str = temp_str.substr(5); // 27 Feb 2020 00:31:29 GMT
    string date_str = temp_str.substr(0, 2);
    int date_num = stoi(date_str);

    temp_str = temp_str.substr(3); // Feb 2020 00:31:29 GMT
    vector<string> months = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    string month_str = temp_str.substr(0, 3);
    int month_num;
    for (int i = 0; i < 12; i++) {
      if (month_str == months[i]) {
        month_num = i;
        break;
      }
    }

    temp_str = temp_str.substr(0, 4); // 2020 00:31:29 GMT
    string year_str = temp_str.substr(0, 4);
    int year_num = stoi(year_str);

    temp_str = temp_str.substr(5); // 00:31:29 GMT
    string h_str = temp_str.substr(0, 2);
    int h = stoi(h_str);
    temp_str = temp_str.substr(3); // 31:29 GMT
    string m_str = temp_str.substr(0, 2);
    int m = stoi(m_str);
    temp_str = temp_str.substr(3); // 29 GMT
    string s_str = temp_str.substr(0, 2);
    int s = stoi(s_str);

    temp_str = temp_str.substr(3); // GMT

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    timeinfo->tm_sec = s;
    timeinfo->tm_min = m;
    timeinfo->tm_hour = h;
    timeinfo->tm_mday = date_num;
    timeinfo->tm_mon = month_num;
    timeinfo->tm_year = year_num;
    timeinfo->tm_wday = day_num;
    res_time = mktime(timeinfo);
    return res_time;
  }

  void handle_get_request() {
    //----------------forward the whole request to the original server
    //-----------
    ssize_t send_mess =
        send(socket_fd, &request_header.data()[0], request_header.size(), 0);
    if (send_mess < 0) {
      perror("send error");
    }

    //&******receive response from the server *********
    receive_response_header();
    try {
      parse_response();
    } catch (const char *mess) {
      //            cout << mess << endl;
      return;
    }

    //&*******write the response into the log **********
    size_t found = response_header_str.find("\r\n");
    if (found != string::npos) {
      string response_header_first_line = response_header_str.substr(0, found);
      new_file.open("/var/log/erss/proxy.log", ios::app);
      new_file << to_string(thread_id) << ": Received \""
               << response_header_first_line << "\" from " << parse_host()
               << "\n";
      new_file.close();
    }

    whole_response = response_header;
    size_t whole_response_size = whole_response.size();

    auto it = response_header_map.find("Content-Length");

    /*****content length********/
    if (it != response_header_map.end()) {
      ssize_t content_length = stoi(it->second);
      receive_response_body_content_length(content_length);
      send(client_connection_fd, &whole_response.data()[0],
           whole_response.size(), 0);

      /*******chunked********/
    } else if (response_header_str.find("chunked") != string::npos) {
      receive_response_body_chunked(); // including send to client//

      /********not content-length and chunked -- only header *********/
    } else {
      send(client_connection_fd, &response_header.data()[0],
           response_header.size(), 0);
    }
    new_file.open("/var/log/erss/proxy.log", ios::app);
    new_file << to_string(thread_id) << ": Responding \"" << response_line
             << "\"\n";
    new_file.close();
    close(socket_fd);
    close(client_connection_fd);
  }

  void handle_get_request_copy() {
    //----------------forward the whole request to the original server
    //-----------
    ssize_t send_mess =
        send(socket_fd, &request_header.data()[0], request_header.size(), 0);
    if (send_mess < 0) {
      perror("send error");
    }

    //&******receive response from the server *********
    receive_response_header();
    try {
      parse_response();
    } catch (const char *mess) {
      cout << mess << endl;
    }
    ssize_t recv_length;

    //&*******write the response into the log **********
    size_t found = response_header_str.find("\r\n");
    if (found != string::npos) {
      string response_header_first_line = response_header_str.substr(0, found);
      new_file.open("/var/log/erss/proxy.log", ios::app);
      new_file << to_string(thread_id) << ": Received \""
               << response_header_first_line << "\" from " << parse_host()
               << "\n";
      new_file.close();
    }

    whole_response = response_header;
    size_t whole_response_size = whole_response.size();

    ssize_t content_length;
    auto it = response_header_map.find("Content-Length");

    /*****content length********/
    if (it != response_header_map.end()) {
      cout << "trasfering is content-length " << endl;
      content_length = stoi(it->second);
      receive_response_body_content_length(content_length);
      // size_t new_size = whole_response_size + content_length;
      // whole_response.resize(whole_response_size + content_length);
      //            whole_response.resize(whole_response_size+1);
      //            ssize_t received_len = 0;
      //            size_t curr = whole_response_size;
      //            while (1) {
      //                recv_length = recv(socket_fd,
      //                &whole_response.data()[curr], 1, 0); received_len ++;
      //                curr++;
      //                ssize_t curr_new = whole_response.size();
      //                if(whole_response[curr_new-1]=='\n'&&whole_response[curr_new-2]=='\r'
      //                &&whole_response[curr_new-3]=='\n'&&whole_response[curr_new-4]=='\r'){
      //                    break;
      //                }
      //                if(received_len == content_length){
      //                    break;
      //                }
      //                whole_response.resize(curr_new+1);
      //            }
      //            //recv_length =
      //            recv(socket_fd,&whole_response.data()[whole_response_size],content_length,0);
      //            response = &whole_response.data()[0];
      //
      //            cout << "***********whole response of
      //            content-length*********" << endl;
      //            //ssize_t size_new = whole_response.size();
      //            // assert(whole_response[size_new-1]=='\n');
      //            for (size_t i = 0; i < whole_response.size(); i++) {
      //                cout << whole_response.data()[i];
      //            }
      //            cout << "*************response end****************" << endl;

      //--------------send all the response to client-------------
      send(client_connection_fd, &whole_response.data()[0],
           whole_response.size(), 0);

      /*******chunked********/
    } else if (response_header_str.find("chunked") != string::npos) {
      whole_response.resize(whole_response_size + 1);
      size_t curr = whole_response_size;

      send(client_connection_fd, &response_header.data()[0],
           response_header.size(), 0);
      while (1) {
        vector<char> temp_buffer(1);
        // temp_buffer.clear();
        recv_length = recv(socket_fd, &whole_response.data()[curr], 1, 0);
        if (recv_length < 0) {
          close(client_connection_fd);
          close(socket_fd);
          return;
        } else if (recv_length == 0) {
          break;
        }
        ssize_t size_new = whole_response.size();
        if (whole_response.data()[size_new - 1] == '\n' &&
            whole_response.data()[size_new - 2] == '\r' &&
            whole_response.data()[size_new - 3] == '\n' &&
            whole_response.data()[size_new - 4] == '\r' &&
            whole_response.data()[size_new - 5] == '0') {
          break;
        }
        send(client_connection_fd, &whole_response.data()[curr], 1, 0);
        curr++;
        size_t curr_response_size = whole_response.size();
        whole_response.resize(curr_response_size + 1);
      }

      response = &whole_response.data()[0];
      // cout << "sending chunked data" << endl;
      // send(client_connection_fd, &whole_response.data()[whole_response_size],
      // whole_response.size()-whole_response_size, 0);
      // send(client_connection_fd, &whole_response.data()[0],
      // whole_response.size(), 0);
      /********not content-length and chunked*********/
    } else {
      send(client_connection_fd, &response_header.data()[0],
           response_header.size(), 0);
      while (1) {
        vector<char> temp_buffer(1024);
        temp_buffer.clear();
        recv_length = recv(socket_fd, &temp_buffer.data()[0], 1024, 0);
        if (recv_length < 0) {
          close(client_connection_fd);
          close(socket_fd);
          return;
        } else if (recv_length == 0) {
          break;
        }
        send(client_connection_fd, &temp_buffer.data()[0], recv_length, 0);
        //                string check_end = &temp_buffer.data()[0];
        //                if(!check_end.empty()&&check_end.find("0\r\n\r\n")!=string::npos){
        //                    break;
        //                }
        if (strstr(temp_buffer.data(), "0\r\n\r\n") != NULL) {
          break;
        }
      }
    }
    new_file.open("/var/log/erss/proxy.log", ios::app);
    new_file << to_string(thread_id) << ": Responding \"" << response_line
             << "\"\n";
    new_file.close();
    close(socket_fd);
    close(client_connection_fd);
  }

  int set_client(string host) {
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(host.c_str(), parse_port().c_str(), &host_info,
                             &host_info_list);
    if (status != 0) {
      throw "Bad Request";
    }

    int socket_fd =
        socket(host_info_list->ai_family, host_info_list->ai_socktype,
               host_info_list->ai_protocol);
    if (socket_fd == -1) {
      throw "Socket Error";
    }
    status =
        connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      throw "Connect error";
    }
    return socket_fd;
  }

  void handle_connect_request() {
    string connection = "HTTP/1.1 200 Connection Established\r\n\r\n";
    ssize_t status =
        send(client_connection_fd, connection.c_str(), connection.size(), 0);
    if (status < 0) {
      close(socket_fd);
      throw "sending error";
    }
    char buff[65535];
    memset(buff, 0, 65535);
    int max =
        (socket_fd > client_connection_fd) ? socket_fd : client_connection_fd;
    fd_set read_set;
    while (1) {
      FD_ZERO(&read_set);
      FD_SET(socket_fd, &read_set);
      FD_SET(client_connection_fd, &read_set);
      if (select(max + 1, &read_set, NULL, NULL, NULL) < 0) {
        // perror("select error");
        break;
      }

      memset(buff, 0, 65535);
      int curr;
      // int recv_len;
      if (FD_ISSET(client_connection_fd, &read_set)) { // receive from browser
        // vector<char> response_server(65535);
        whole_request.clear();
        whole_request.resize(65535);
        ssize_t recv_length =
            recv(client_connection_fd, &whole_request.data()[0], 65535, 0);
        if (recv_length < 0) {
          // perror("receiving error");
          break;
        }
        if (recv_length == 0) {
          break;
        }
        send(socket_fd, &whole_request.data()[0], recv_length, 0); // error??
      } else if (FD_ISSET(socket_fd,
                          &read_set)) { // receive from original server
        // vector<char> request_client(65535);
        whole_response.clear();
        whole_response.resize(65535);
        ssize_t recv_length =
            recv(socket_fd, &whole_response.data()[0], 65535, 0);
        if (recv_length < 0) {
          break;
        }
        if (recv_length == 0) {
          break;
        }
        response_header_str =
            &whole_response.data()[0]; // write the connect response to log
        size_t found = response_header_str.find("\r\n");
        // ID: Received "RESPONSE" from SERVER
        if (found != string::npos) {
          string response_header_first_line =
              response_header_str.substr(0, found);
          new_file.open("/var/log/erss/proxy.log", ios::app);
          new_file << to_string(thread_id) << ": Received \""
                   << response_header_first_line << "\" from " << parse_host()
                   << "\n";
          new_file.close();
        }

        send(client_connection_fd, &whole_response.data()[0], recv_length, 0);
      }
    }
    new_file.open("/var/log/erss/proxy.log", ios::app);
    new_file << to_string(thread_id) << ": "
             << "Tunnel closed\n";
    new_file.close();
    close(socket_fd);
    close(client_connection_fd);
    return;
  }

  void handle_post_request() {
    // only have response header
    auto it = request_header_map.find("Content-Length");
    size_t content_len;
    whole_request = request_header;
    size_t request_header_length = request_header.size();
    assert(request_header[request_header_length - 1] == '\n' &&
           request_header[request_header_length - 2] == '\r');
    send(socket_fd, &request_header.data()[0], request_header.size(), 0);
    if (it != request_header_map.end()) {
      content_len = stoi(it->second);

      whole_request.resize(request_header_length + 1);
      size_t curr = request_header_length;
      size_t received_length = 0;
      while (1) {
        size_t recv_length =
            recv(client_connection_fd, &whole_request.data()[curr], 1, 0);
        received_length += recv_length;
        curr += recv_length;
        if (received_length == content_len) {
          break;
        }
        size_t new_size = whole_request.size();
        whole_request.resize(new_size + recv_length);
        // cout<<"receiving request from browser of
        // size"<<received_length<<endl;
      }
      request = &whole_request.data()[0];
      assert(whole_request.size() == request_header.size() + content_len);
      send(socket_fd, &whole_request.data()[request_header_length], content_len,
           0); // send all the request to browser

    } else {
    }
    receive_response_header();
    try {
      parse_response();
    } catch (const char *mess) {
      cout << mess << endl;
    }
    if (response_code != "200") {
      return;
    }
    size_t found = response_header_str.find("\r\n");
    if (found != string::npos) {
      string response_header_first_line = response_header_str.substr(0, found);
      new_file.open("/var/log/erss/proxy.log", ios::app);
      new_file << to_string(thread_id) << ": Received \""
               << response_header_first_line << "\" from " << parse_host()
               << "\n";
      new_file.close();
    }

    whole_response = response_header;
    size_t whole_response_size = whole_response.size();

    auto it1 = response_header_map.find("Content-Length");

    /*****content length********/
    if (it1 != response_header_map.end()) {
      ssize_t content_length = stoi(it->second);
      receive_response_body_content_length(content_length);
      send(client_connection_fd, &whole_response.data()[0],
           whole_response.size(), 0);

      /*******chunked********/
    } else if (response_header_str.find("chunked") != string::npos) {
      receive_response_body_chunked(); // including send to client//

      /********not content-length and chunked -- only header *********/
    } else {
      send(client_connection_fd, &response_header.data()[0],
           response_header.size(), 0);
    }
    new_file.open("/var/log/erss/proxy.log", ios::app);
    new_file << to_string(thread_id) << ": Responding \"" << response_line
             << "\"\n";
    new_file.close();
    close(socket_fd);
    close(client_connection_fd);

    //        cout<<"send the response header back to browser -- post"<<endl;
  }
  void handle_post_request1() {
    // send the request header to the server
    // ssize_t status = send(socket_fd, &request_header.data()[0],
    // request_header.size(), 0);
    ssize_t found_body = request.find("\r\n\r\n");
    string request_body;
    if (found_body != string::npos) {
      request_body = request.substr(found_body + 4);
    } else {
      close(socket_fd);
      close(client_connection_fd);
      return;
    }
    //---------decide how many bytes to send according to the content length
    //-------
    auto it = request_header_map.find("Content-Length");
    size_t content_len;
    if (it != request_header_map.end()) {
      content_len = stoi(it->second);
      cout << "Content Length is " << content_len << endl;
      send(socket_fd, &whole_request.data()[0], found_body + 4, 0);
      send(socket_fd, &whole_request.data()[found_body + 4], content_len, 0);
      whole_response.resize(65535);
      ssize_t recv_length =
          recv(socket_fd, &whole_response.data()[0], 65535, 0);
      if (recv_length <= 0) {
        close(socket_fd);
        close(client_connection_fd);
        return;
      }
      whole_response.resize(recv_length);
      send(client_connection_fd, &whole_response.data()[0], recv_length, 0);
    } else { // cannot find content length
      send(socket_fd, &whole_request.data()[0], whole_request.size(), 0);
      whole_response.resize(65535);
      ssize_t recv_length =
          recv(socket_fd, &whole_response.data()[0], 65535, 0);
      if (recv_length <= 0) {
        close(socket_fd);
        close(client_connection_fd);
        return;
      }
      whole_response.resize(recv_length);
      send(client_connection_fd, &whole_response.data()[0], recv_length, 0);
    }

    //*******receive response from the server *********
    //        ssize_t recv_length;
    ////        response.resize(65535,'\0');
    ////        ssize_t recv_length = recv(socket_fd, &response.data()[0],
    ///response.size(), 0); /        ssize_t index = recv_length;
    //        string response_header(&response[0]);
    //        size_t found = response_header.find("\r\n");
    //        cout << "response is " << response_header << endl;
    //
    //        //********write the response into the log **********
    //        if (found != string::npos) {
    //            string response_header_first_line = response_header.substr(0,
    //            found); new_file.open("history.txt", ios::app); new_file <<
    //            to_string(thread_id) << ": Received " <<
    //            response_header_first_line << " from " << parse_host()
    //                     << "\n";
    //            new_file.close();
    //        }
    //
    //        //********check the response content type *********
    //        int content_length;
    //        map<string,string>::iterator it =
    //        response_header_map.find("Content-Length");
    //        /*****content length********/
    //        if(it!=response_header_map.end()){
    //            content_length = stoi(it->second);
    //            recv_length =
    //            recv(socket_fd,&whole_response.data()[0],content_length,0);
    //            send(client_connection_fd, &whole_response.data()[0],
    //            response.size(), 0);
    //            /*******chunked********/
    //        }else if (response_header.find("chunked")!=string::npos){
    //            string check(buffer);
    //            while (1) {
    //                ssize_t found;
    //                if ((found = check.find("0\r\n")) != std::string::npos) {
    //                    //if (check[found - 1] == '\n') {
    //                    if (check.find("\n0\r\n") != string::npos) {//find the
    //                    position
    //                        break;
    //                    }
    //                }
    //                //int recv_len = 0;
    //                std::vector<char> temp(65535);
    //                recv_length = recv(socket_fd, temp.data(), 65535, 0);
    //                if (recv_length <= 0) {
    //                    break;
    //                }
    //                send(client_connection_fd, temp.data(), recv_length, 0);
    //                std::string track(temp.begin(), temp.end());
    //                check.append(track);
    //            }
    //            /********other way?*********/
    //        }else {
    //            cout << "beside contect-length and chunked -- other way " <<
    //            endl; cout << "response of this POST method is " <<
    //            response_header<<endl;
    //        }
    close(socket_fd);
    close(client_connection_fd);
    // free(host_info_list);
  }

  bool check_cache() {
    auto it = request_header_map.find("Cache-Control");
    string cache_control;
    if (it != request_header_map.end()) { // find the cache-control
      cache_control = it->second;
    } else {
      cout << "cannot find cache-control" << endl;
    }
    if (cache_control == "no-store") { // do not need cache
      return false;
    } else {
      // check whether in cache
      auto it1 = cache_map.find(host_url);
      if (it1 == cache_map.end()) { // cannot find the cache--add  to the cache
      }
    }
  }
  //    void evict_cache(){
  //        if(cache_map.empty()){
  //            cout<<"cannot delete"<<endl;
  //            return;
  //        }
  //        auto it = cache_map.begin();
  //        assert(it!=cache_map.end());
  //        curr_cache_size -= it->second.size();
  //        cache_map.erase(it);
  //    }
};

#endif // SRC_HTTP_H
