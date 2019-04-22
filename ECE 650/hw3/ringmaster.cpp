#include "potato.h"
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 4) {
    cerr << "Syntax: ./ringmaster <port_num> <num_players> <num_hops>" << endl;
    exit(EXIT_FAILURE);
  }

  int status;
  int server_fd;
  struct hostent *server_info;
  struct sockaddr_in server_addr_info;
  char server_name[512];
  int port_num = atoi(argv[1]);
  int num_players = atoi(argv[2]);
  int num_hops = atoi(argv[3]);

  // arguments validation check
  if (port_num < 1024 || port_num > 65535) {
    cerr << "Error: port number should be between 1024 - 65535" << endl;
    exit(EXIT_FAILURE);
  }
  if (num_players < 2) {
    cerr << "Error: players number should be more than 1" << endl;
    exit(EXIT_FAILURE);
  }
  if (num_hops < 0 || num_hops > 512) {
    cerr << "Error: Hops number should be between 0 - 512" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Potato Ringmaster" << endl;
  cout << "Players = " << num_players << endl;
  cout << "Hops = " << num_hops << endl;

  gethostname(server_name, sizeof(server_name));
  server_info = gethostbyname(server_name);

  server_addr_info.sin_family = AF_INET;
  server_addr_info.sin_port = htons(port_num);
  server_addr_info.sin_addr.s_addr = INADDR_ANY;
  memcpy(&server_addr_info.sin_addr, server_info->h_addr_list[0],
         server_info->h_length);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << server_name << "," << port_num << ")" << endl;
    return -1;
  }

  int yes = 1;
  status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(server_fd, (struct sockaddr *)&server_addr_info,
                sizeof(server_addr_info));
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << server_name << "," << port_num << ")" << endl;
    return -1;
  }

  status = listen(server_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl;
    cerr << "  (" << server_name << "," << port_num << ")" << endl;
    return -1;
  }

  // Collect all info of players
  struct player *players[num_players]; // store all players in an array
  struct sockaddr_in player_in;
  socklen_t player_len = sizeof(player_in);
  fd_set fds;
  FD_ZERO(&fds);
  int nfds = -1;
  for (int i = 0; i < num_players; i++) {
    int new_player_fd =
        accept(server_fd, (struct sockaddr *)&player_in, &player_len);
    if (new_player_fd < 0) {
      cerr << "Error: Cannot accept connection" << endl;
      exit(EXIT_FAILURE);
    }
    cout << "Player " << i << " is ready to play" << endl;

    struct hostent *tmp_host;
    tmp_host = gethostbyaddr((char *)&player_in.sin_addr,
                             sizeof(struct in_addr), AF_INET);
    if (tmp_host == NULL) {
      cerr << "Error: Tmp_host error" << endl;
      exit(EXIT_FAILURE);
    }

    int player_port;
    status = recv(new_player_fd, &player_port, sizeof(player_port), 0);
    if (status < 0) {
      cerr << "Error: send player_port" << endl;
      exit(EXIT_FAILURE);
    }
    status = send(new_player_fd, &num_players, sizeof(num_players), 0);
    if (status < 0) {
      cerr << "Error: send num_players" << endl;
      exit(EXIT_FAILURE);
    }
    status = send(new_player_fd, &i, sizeof(i), 0);
    if (status < 0) {
      cerr << "Error: send ID" << endl;
      exit(EXIT_FAILURE);
    }
    status = send(new_player_fd, &num_hops, sizeof(num_hops), 0);
    if (status < 0) {
      cerr << "Error: send num_hops" << endl;
      exit(EXIT_FAILURE);
    }

    struct player *tmp_player;
    tmp_player = (struct player *)malloc(sizeof(struct player));
    tmp_player->total_player_num = num_players;
    tmp_player->socket = new_player_fd;
    tmp_player->this_id = i;
    tmp_player->this_port = player_port;
    strcpy(tmp_player->this_hostname, tmp_host->h_name); // ERROR MARK
    players[i] = tmp_player;

    // cout << num_players << endl;
    // cout << player_port << endl;
    // cout << tmp_host->h_name << endl;
    // cout << players[i]->this_hostname << endl;
    // cout << endl;

    FD_SET(new_player_fd, &fds);
    nfds = nfds > new_player_fd ? nfds : new_player_fd;
  }

  // update neighbor info of each player process
  for (int i = 0; i < num_players; i++) {
    // first
    if (i == 0) {
      players[i]->left_id = players[num_players - 1]->this_id;
      players[i]->left_port = players[num_players - 1]->this_port;
      strcpy(players[i]->left_hostname,
             players[num_players - 1]->this_hostname);
      players[i]->right_id = players[i + 1]->this_id;
      players[i]->right_port = players[i + 1]->this_port;
      strcpy(players[i]->right_hostname, players[i + 1]->this_hostname);
    }
    // last
    else if (i == num_players - 1) {
      players[i]->left_id = players[i - 1]->this_id;
      players[i]->left_port = players[i - 1]->this_port;
      strcpy(players[i]->left_hostname, players[i - 1]->this_hostname);
      players[i]->right_id = players[0]->this_id;
      players[i]->right_port = players[0]->this_port;
      strcpy(players[i]->right_hostname, players[0]->this_hostname);
    }
    // normal
    else {
      players[i]->left_id = players[i - 1]->this_id;
      players[i]->left_port = players[i - 1]->this_port;
      strcpy(players[i]->left_hostname, players[i - 1]->this_hostname);
      players[i]->right_id = players[i + 1]->this_id;
      players[i]->right_port = players[i + 1]->this_port;
      strcpy(players[i]->right_hostname, players[i + 1]->this_hostname);
    }
  }

  /*
  // TEST: contents of players
  for (int i = 0; i < num_players; i++) {
    cout << players[i]->total_player_num << endl;
    cout << players[i]->socket << endl;
    cout << players[i]->this_id << endl;
    cout << players[i]->this_port << endl;
    cout << players[i]->this_hostname << endl;
    cout << players[i]->left_id << endl;
    cout << players[i]->left_port << endl;
    cout << players[i]->left_hostname << endl;
    cout << players[i]->right_id << endl;
    cout << players[i]->right_port << endl;
    cout << players[i]->right_hostname << endl;
    cout << endl;
  }
  */

  // send info to players
  for (int i = 0; i < num_players; i++) {
    status = send(players[i]->socket, &players[i]->left_id,
                  sizeof(players[i]->left_id), 0);
    status = send(players[i]->socket, &players[i]->left_port,
                  sizeof(players[i]->left_port), 0);
    status = send(players[i]->socket, &players[i]->left_hostname,
                  sizeof(players[i]->left_hostname), 0);
    status = send(players[i]->socket, &players[i]->right_id,
                  sizeof(players[i]->right_id), 0);
    status = send(players[i]->socket, &players[i]->right_port,
                  sizeof(players[i]->right_port), 0);
    status = send(players[i]->socket, &players[i]->right_hostname,
                  sizeof(players[i]->right_hostname), 0);
  }

  // Ready to start the game
  // Corner case: hops = 0
  if (num_hops == 0) {
    for (int i = 0; i < num_players; ++i) {
      close(players[i]->socket);
    }

    close(server_fd);

    return 0;
  }

  srand((unsigned int)time(NULL));
  int first_player = rand() % num_players;
  cout << "Ready to start the game, sending potato to player " << first_player
       << endl;

  struct potato ptt;
  //  num_hops--;
  // ptt.potato_idx = 0;

  ptt.left_count = num_hops;
  ptt.track[0] = first_player;
  // status = send(players[first_player]->socket, &ptt.potato_idx,
  //            sizeof(ptt.potato_idx), 0);
  status = send(players[first_player]->socket, &ptt.left_count,
                sizeof(ptt.left_count), 0);
  status =
      send(players[first_player]->socket, &ptt.track, sizeof(ptt.track), 0);

  status = select(nfds + 1, &fds, NULL, NULL, NULL);
  if (status == -1) {
    cerr << "Error: select" << endl;
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < num_players; i++) {
    // send to master means potato left_count = 0
    if (FD_ISSET(players[i]->socket, &fds)) {
      //      recv(players[i]->socket, &ptt.potato_idx, sizeof(ptt.potato_idx),
      //      0);
      recv(players[i]->socket, &ptt.left_count, sizeof(ptt.left_count), 0);
      recv(players[i]->socket, &ptt.track, sizeof(ptt.track), 0);
      struct potato end_potato;
      end_potato.left_count = -666;
      for (int i = 0; i < num_players; i++) {
        send(players[i]->socket, &end_potato.left_count,
             sizeof(end_potato.left_count), 0);
      }
      break;
    }
  }

  cout << "Trace of potato: " << endl;
  for (int i = 0; i < num_hops; i++) {
    if (i != num_hops - 1) {
      cout << ptt.track[i] << ",";
    } else {
      cout << ptt.track[i] << endl;
    }
  }

  /*
  if (num_hops != 0) {
    cout << "Trace of potato: " << endl;
    cout << ptt.track << endl;
  }
  */

  for (int i = 0; i < num_players; ++i) {
    close(players[i]->socket);
  }

  close(server_fd);

  return 0;
}
