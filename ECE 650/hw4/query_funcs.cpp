#include "query_funcs.h"

#include <iomanip>

void add_player(connection *C, int team_id, int jersey_num, string first_name,
                string last_name, int mpg, int ppg, int rpg, int apg,
                double spg, double bpg) {}

void add_team(connection *C, string name, int state_id, int color_id, int wins,
              int losses) {}

void add_state(connection *C, string name) {}

void add_color(connection *C, string name) {}

void query1(connection *C, int use_mpg, int min_mpg, int max_mpg, int use_ppg,
            int min_ppg, int max_ppg, int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg, int use_spg, double min_spg,
            double max_spg, int use_bpg, double min_bpg, double max_bpg) {
  string sql = "SELECT * FROM player p WHERE ";

  if (use_mpg) {
    string min_mpg_str = to_string(min_mpg);
    string max_mpg_str = to_string(max_mpg);
    sql += "p.mpg >= " + min_mpg_str + " and p.mpg <= " + max_mpg_str;
  }

  if (use_ppg) {
    if (use_mpg) {
      sql += " and ";
    }
    string min_ppg_str = to_string(min_ppg);
    string max_ppg_str = to_string(max_ppg);
    sql += "p.ppg >= " + min_ppg_str + " and p.ppg <= " + max_ppg_str;
  }

  if (use_rpg) {
    if (use_mpg || use_ppg) {
      sql += " and ";
    }
    string min_rpg_str = to_string(min_rpg);
    string max_rpg_str = to_string(max_rpg);
    sql += "p.rpg >= " + min_rpg_str + " and p.rpg <= " + max_rpg_str;
  }

  if (use_apg) {
    if (use_mpg || use_ppg || use_rpg) {
      sql += " and ";
    }
    string min_apg_str = to_string(min_apg);
    string max_apg_str = to_string(max_apg);
    sql += "p.apg >= " + min_apg_str + " and p.apg <= " + max_apg_str;
  }

  if (use_spg) {
    if (use_mpg || use_ppg || use_rpg || use_apg) {
      sql += " and ";
    }
    string min_spg_str = to_string(min_spg);
    string max_spg_str = to_string(max_spg);
    sql += "p.spg >= " + min_spg_str + " and p.spg <= " + max_spg_str;
  }

  if (use_bpg) {
    if (use_mpg || use_ppg || use_rpg || use_apg || use_spg) {
      sql += " and ";
    }
    string min_bpg_str = to_string(min_bpg);
    string max_bpg_str = to_string(max_bpg);
    sql += "p.bpg >= " + min_bpg_str + " and p.bpg <= " + max_bpg_str;
  }

  if (!(use_mpg || use_ppg || use_rpg || use_apg || use_spg ||
        use_bpg)) { // if no use of any
    sql = "SELECT * FROM PLAYER";
  }

  sql += ";";

  nontransaction N(*C);
  result R(N.exec(sql));

  cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG "
          "SPG BPG"
       << endl;
  cout << fixed;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<int>() << " " << c[1].as<int>() << " " << c[2].as<int>()
         << " " << c[3].as<string>() << " " << c[4].as<string>() << " "
         << c[5].as<int>() << " " << c[6].as<int>() << " " << c[7].as<int>()
         << " " << c[8].as<int>() << " " << setprecision(1) << c[9].as<float>()
         << " " << setprecision(1) << c[10].as<float>() << endl;
  }
}

void query2(connection *C, string team_color) {
  string sql =
      "SELECT t.name FROM team t, color c WHERE t.color_id = c.color_id "
      "and c.name = '" +
      team_color + "';";

  nontransaction N(*C);
  result R(N.exec(sql));

  cout << "NAME" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<string>() << endl;
  }
}

void query3(connection *C, string team_name) {
  string sql = "SELECT p.first_name, p.last_name FROM player p, team t WHERE "
               "t.team_id = p.team_id and t.name = '" +
               team_name + "' ORDER BY p.ppg desc;";

  nontransaction N(*C);
  result R(N.exec(sql));

  cout << "FIRST_NAME LAST_NAME" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<string>() << " " << c[1].as<string>() << endl;
  }
}

void query4(connection *C, string team_state, string team_color) {
  string sql =
      "SELECT p.first_name, p.last_name, p.uniform_num FROM player p, state s, "
      "team t, color c WHERE p.team_id = t.team_id and t.state_id = s.state_id "
      "and t.color_id = c.color_id and s.name = '" +
      team_state + "' and c.name = '" + team_color + "';";

  nontransaction N(*C);
  result R(N.exec(sql));

  cout << "FIRST_NAME LAST_NAME UNIFORM_NUM" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<string>() << " " << c[1].as<string>() << " "
         << c[2].as<int>() << endl;
  }
}

void query5(connection *C, int num_wins) {
  string num_wins_str = to_string(num_wins);
  string sql =
      "SELECT p.first_name, p.last_name, t.name as TEAM_NAME, t.wins FROM "
      "player p, team t WHERE p.team_id = t.team_id and t.wins > " +
      num_wins_str + ";";

  nontransaction N(*C);
  result R(N.exec(sql));

  cout << "FIRST_NAME LAST_NAME TEAM_NAME WINS" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<string>() << " " << c[1].as<string>() << " "
         << c[2].as<string>() << " " << c[3].as<int>() << endl;
  }
}
