#include "potato.h"
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int max_fd(int self, int l, int r) {
  int res = self;
  res = (res > l) ? res : l;
  res = (res > r) ? res : r;
  return res;
}

int main(int argc, char *argv[]) {
  // 3 arguments judge
  if (argc != 3) {
    cerr << "Syntax: ./player <hostname> <port_num>" << endl;
    exit(EXIT_FAILURE);
  }

  int status;
  int server_fd;
  struct hostent *server_info;
  struct hostent *client_info;
  struct sockaddr_in server_addr_info;
  struct sockaddr_in client_addr_info;
  char client_name[512];
  const char *port = argv[2];
  int port_num = atoi(argv[2]);
  struct potato ptt;

  if (port_num < 1024 || port_num > 65535) {
    cerr << "Error: port number should be between 1024 - 65535" << endl;
    exit(EXIT_FAILURE);
  }

  gethostname(client_name, sizeof(client_name));
  client_info = gethostbyname(client_name);

  server_info = gethostbyname(argv[1]);

  server_addr_info.sin_family = AF_INET;
  server_addr_info.sin_port = htons(port_num);
  memcpy(&server_addr_info.sin_addr, server_info->h_addr_list[0],
         server_info->h_length);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << client_name << "," << port << ")" << endl;
    return -1;
  }

  // cout << "Connecting to " << argv[1] << " on port " << port << "..." <<
  // endl;

  status = connect(server_fd, (struct sockaddr *)&server_addr_info,
                   sizeof(server_addr_info));
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << argv[1] << "," << port << ")" << endl;
    return -1;
  }

  // player to player connection

  int client_ser_fd;
  client_ser_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_ser_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << client_name << "," << port << ")" << endl;
    return -1;
  }

  srand((unsigned int)time(NULL));
  // int random = (rand() % 48127) + 1024;
  // cout << random << endl;
  int c_port = 10000;
  while (true) {
    client_addr_info.sin_family = AF_INET;
    client_addr_info.sin_port = htons(c_port);
    client_addr_info.sin_addr.s_addr = INADDR_ANY;
    memcpy(&client_addr_info.sin_addr, client_info->h_addr_list[0],
           client_info->h_length);

    int yes = 1;
    status =
        setsockopt(client_ser_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(client_ser_fd, (struct sockaddr *)&client_addr_info,
                  sizeof(client_addr_info));
    if (status != -1) {
      break;
    }
    if (c_port == 65535) {
      c_port = 0;
    } else {
      c_port++;
    }
  }

  status = listen(client_ser_fd, 2);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl;
    exit(EXIT_FAILURE);
  }

  int num_players, player_id, num_hops;
  // send player_port to ringmaster
  status = send(server_fd, (char *)&c_port, sizeof(c_port), 0);
  // recv 3 stuff from ringmaster
  status = recv(server_fd, &num_players, sizeof(num_players), 0);
  status = recv(server_fd, &player_id, sizeof(player_id), 0);
  status = recv(server_fd, &num_hops, sizeof(num_hops), 0);

  cout << "Connected as player " << player_id << " out of " << num_players
       << " total players" << endl;

  int left_id, left_port, right_id, right_port;
  char left_hostname[512], right_hostname[512];
  // recieve additional 6 stuff from ringmaster
  status = recv(server_fd, &left_id, sizeof(left_id), 0);
  status = recv(server_fd, &left_port, sizeof(left_port), 0);
  status = recv(server_fd, &left_hostname, sizeof(left_hostname), 0);
  status = recv(server_fd, &right_id, sizeof(right_id), 0);
  status = recv(server_fd, &right_port, sizeof(right_port), 0);
  status = recv(server_fd, &right_hostname, sizeof(right_hostname), 0);

  /*
  // TEST: recv
  cout << "this_port: " << c_port << endl;
  cout << "left_id: " << left_id << endl;
  cout << "left_port: " << left_port << endl;
  cout << "left_hostname: " << left_hostname << endl;
  cout << "right_id: " << right_id << endl;
  cout << "right_port: " << right_port << endl;
  cout << "right_hostname: " << right_hostname << endl;
  */

  // connection with left player
  int left_client_fd;
  struct hostent *left_client_info;
  struct sockaddr_in left_client_addr_info;

  left_client_info = gethostbyname(left_hostname);

  left_client_addr_info.sin_family = AF_INET;
  left_client_addr_info.sin_port = htons(left_port);
  memcpy(&left_client_addr_info.sin_addr, left_client_info->h_addr_list[0],
         left_client_info->h_length);

  left_client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (left_client_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << left_hostname << "," << left_port << ")" << endl;
    return -1;
  }

  status = connect(left_client_fd, (struct sockaddr *)&left_client_addr_info,
                   sizeof(left_client_addr_info));
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << left_hostname << "," << left_port << ")" << endl;
    return -1;
  }

  struct sockaddr_in left_player_in;
  socklen_t left_player_len = sizeof(left_player_in);
  int left_player_fd = accept(client_ser_fd, (struct sockaddr *)&left_player_in,
                              &left_player_len);
  if (left_player_fd < 0) {
    cerr << "Error: Cannot accept connection" << endl;
    exit(EXIT_FAILURE);
  }

  // connection with right player
  int right_client_fd;
  struct hostent *right_client_info;
  struct sockaddr_in right_client_addr_info;

  right_client_info = gethostbyname(right_hostname);

  right_client_addr_info.sin_family = AF_INET;
  right_client_addr_info.sin_port = htons(right_port);
  memcpy(&right_client_addr_info.sin_addr, right_client_info->h_addr_list[0],
         right_client_info->h_length);

  right_client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (right_client_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << right_hostname << "," << right_port << ")" << endl;
    return -1;
  }

  status = connect(right_client_fd, (struct sockaddr *)&right_client_addr_info,
                   sizeof(right_client_addr_info));
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << right_hostname << "," << right_port << ")" << endl;
    return -1;
  }

  struct sockaddr_in right_player_in;
  socklen_t right_player_len = sizeof(left_player_in);
  int right_player_fd = accept(
      client_ser_fd, (struct sockaddr *)&right_player_in, &right_player_len);
  if (right_player_fd < 0) {
    cerr << "Error: Cannot accept connection" << endl;
    exit(EXIT_FAILURE);
  }

  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(server_fd, &fds);
  FD_SET(left_player_fd, &fds);
  FD_SET(right_player_fd, &fds);
  int maxFD = max_fd(server_fd, left_player_fd, right_player_fd);

  while (1) {
    fd_set fds_ex = fds;
    status = select(maxFD + 1, &fds_ex, NULL, NULL, NULL);
    if (status == -1) {
      cerr << "Error: select" << endl;
      exit(EXIT_FAILURE);
    }

    int temp_fd;
    // ringmaster
    if (FD_ISSET(server_fd, &fds_ex)) {
      temp_fd = server_fd;
    }
    // left
    else if (FD_ISSET(left_player_fd, &fds_ex)) {
      temp_fd = left_player_fd;
    }
    // right
    else if (FD_ISSET(right_player_fd, &fds_ex)) {
      temp_fd = right_player_fd;
    }
    // weird situation
    else {
      cerr << "Error: something wrong!!!" << endl;
      exit(EXIT_FAILURE);
    }

    //    recv(temp_fd, &ptt.potato_idx, sizeof(ptt.potato_idx), 0);
    recv(temp_fd, &ptt.left_count, sizeof(ptt.left_count), 0); // hop,
    recv(temp_fd, &ptt.track, sizeof(ptt.track), 0);           // potato
    // cout << "left_count: " << ptt.left_count << endl;
    //   ptt.track[num_hops - ptt.left_count] = player_id;

    FD_ZERO(&fds_ex);
    FD_SET(server_fd, &fds_ex);
    FD_SET(left_player_fd, &fds_ex);
    FD_SET(right_player_fd, &fds_ex);

    ptt.track[num_hops - ptt.left_count] = player_id;
    ptt.left_count--;

    if (ptt.left_count < 0) {
      break;
    }

    if (ptt.left_count == 0) {
      // strcat(ptt.track, prev_track);
      //   ptt.track[ptt.potato_idx] = player_id;
      // ptt.track[num_hops - ptt.left_count] = player_id;
      // cout << "LAST player_id: " << player_id << endl;
      //  status = send(server_fd, &ptt.potato_idx, sizeof(ptt.potato_idx), 0);
      status = send(server_fd, &ptt.left_count, sizeof(ptt.left_count), 0);
      status = send(server_fd, &ptt.track, sizeof(ptt.track), 0);
      cout << "I'm it" << endl;
      continue;
    }

    // ptt.potato_idx++;
    // ptt.left_count--;

    // srand((unsigned int)time(NULL));
    int next_player = (rand()) % 2;
    //    cout << "NEXT PLAYER: " << next_player << endl;
    // left
    if (next_player == 0) {
      // ptt.track[ptt.potato_idx] = left_id;
      // ptt.track[num_hops - ptt.left_count] = left_id;
      cout << "Sending potato to " << left_id << endl;
      // status = send(left_client_fd, &ptt.potato_idx, sizeof(ptt.potato_idx),
      // 0);
      status = send(left_client_fd, &ptt.left_count, sizeof(ptt.left_count), 0);
      status = send(left_client_fd, &ptt.track, sizeof(ptt.track), 0);
    }
    // right
    else {
      // ptt.track[ptt.potato_idx] = right_id;
      // ptt.track[num_hops - ptt.left_count] = right_id;
      cout << "Sending potato to " << right_id << endl;
      // status =
      //  send(right_client_fd, &ptt.potato_idx, sizeof(ptt.potato_idx), 0);
      status =
          send(right_client_fd, &ptt.left_count, sizeof(ptt.left_count), 0);
      status = send(right_client_fd, &ptt.track, sizeof(ptt.track), 0);
    }
    //    cout << ptt.left_count << endl;
  }

  close(server_fd);
  // close(client_ser_fd);
  // close(left_player_fd);
  // close(right_player_fd);

  return 0;
}
