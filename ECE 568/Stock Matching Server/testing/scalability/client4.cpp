#include "socket.h"
#include "tinyxml2.h"
#include <arpa/inet.h>
#include <cstring>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;
using namespace tinyxml2;

int main(int argc, char *argv[]) {
  if (argc < 2)
    cout << "invalid input" << endl;
  const char *port = "12345";
  const char *hostname = argv[1];
  Socket client(hostname, port);
  int socket_fd = client.init_client();

  string base =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?><transactions id=\"123\">";
  for (int i = 1; i <= 1000; i++) {
    string tmp = "<query id=\"" + to_string(i) + "\"/>";
    base += tmp;
  }
  base += "</transactions>";

  const char *buffer = base.c_str();

  // XMLDocument xml;
  // xml.Parse(buffer);
  // cout << base << endl;
  // xml.Print();

  //  for (int i = 0; i < 10; i++) {

  send(socket_fd, buffer, 50000, 0);

  // cout << buffer << endl;

  char recv_buf[50000];
  recv(socket_fd, &recv_buf, 50000, 0);

  XMLDocument xml;
  string pSource = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><results>" +
                   string(recv_buf) + "</results>";
  //  cout << pSource << endl;
  xml.Parse(pSource.c_str());
  // cout << "xml.Print():" << endl;
  xml.Print();
  // sleep(0.15);
  // }

  close(socket_fd);

  return 0;
}
