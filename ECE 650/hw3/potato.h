struct player {
  int total_player_num;
  int socket;
  int this_id;
  int this_port;
  char this_hostname[512];
  int left_id;
  int left_port;
  char left_hostname[512];
  int right_id;
  int right_port;
  char right_hostname[512];
};

struct potato {
  //  int total_count;
  // int potato_idx;
  int left_count;
  // char track[2048];
  int track[512];
};
