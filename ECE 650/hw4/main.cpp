#include <fstream>
#include <iostream>
#include <iterator>
#include <pqxx/pqxx>
#include <string>

#include "exerciser.h"

using namespace std;
using namespace pqxx;

string apostrophe_case(const string &str) {
  size_t found = str.find("'");
  if (found != string::npos) {
    string s = str.substr(0, found);
    s += "'";
    s += str.substr(found);
    return s;
  }
  return str;
}

void CreateTable_Player(connection *C) {
  string sql = "DROP TABLE IF EXISTS PLAYER CASCADE;"
               "CREATE TABLE PLAYER("
               "PLAYER_ID           INT    NOT NULL,"
               "TEAM_ID             INT    NOT NULL,"
               "UNIFORM_NUM         INT    NOT NULL,"
               "FIRST_NAME          VARCHAR(50)    NOT NULL,"
               "LAST_NAME           VARCHAR(50)    NOT NULL,"
               "MPG                 INT    NOT NULL,"
               "PPG                 INT    NOT NULL,"
               "RPG                 INT    NOT NULL,"
               "APG                 INT    NOT NULL,"
               "SPG                 FLOAT    NOT NULL,"
               "BPG                 FLOAT    NOT NULL,"
               "PRIMARY KEY (PLAYER_ID),"
               "FOREIGN KEY (TEAM_ID) REFERENCES TEAM(TEAM_ID));";
  work W(*C);
  W.exec(sql);
  W.commit();
  // cout << "PLAYER Table created successfully" << endl;
}

void CreateTable_Team(connection *C) {
  string sql = "DROP TABLE IF EXISTS TEAM CASCADE;"
               "CREATE TABLE TEAM("
               "TEAM_ID             INT    NOT NULL,"
               "NAME                TEXT   NOT NULL,"
               "STATE_ID            INT    NOT NULL,"
               "COLOR_ID            INT    NOT NULL,"
               "WINS                INT    NOT NULL,"
               "LOSSES              INT    NOT NULL,"
               "PRIMARY KEY (TEAM_ID),"
               "FOREIGN KEY (STATE_ID) REFERENCES STATE(STATE_ID),"
               "FOREIGN KEY (COLOR_ID) REFERENCES COLOR(COLOR_ID));";
  work W(*C);
  W.exec(sql);
  W.commit();
  // cout << "TEAM Table created successfully" << endl;
}

void CreateTable_State(connection *C) {
  string sql = "DROP TABLE IF EXISTS STATE CASCADE;"
               "CREATE TABLE STATE("
               "STATE_ID            INT    NOT NULL,"
               "NAME                TEXT   NOT NULL,"
               "PRIMARY KEY (STATE_ID));";
  work W(*C);
  W.exec(sql);
  W.commit();
  // cout << "STATE Table created successfully" << endl;
}

void CreateTable_Color(connection *C) {
  string sql = "DROP TABLE IF EXISTS COLOR CASCADE;"
               "CREATE TABLE COLOR("
               "COLOR_ID            INT    NOT NULL,"
               "NAME                TEXT   NOT NULL,"
               "PRIMARY KEY (COLOR_ID));";
  work W(*C);
  W.exec(sql);
  W.commit();
  // cout << "COLOR Table created successfully" << endl;
}

void InsertInfo_Player(connection *C) {
  ifstream players("player.txt");
  string player_id;
  string team_id;
  string uniform_num;
  string first_name;
  string last_name;
  string mpg;
  string ppg;
  string rpg;
  string apg;
  string spg;
  string bpg;
  string sql;

  if (players.is_open()) {
    while (players >> player_id >> team_id >> uniform_num >> first_name >>
           last_name >> mpg >> ppg >> rpg >> apg >> spg >> bpg) {
      string fname = apostrophe_case(first_name);
      string lname = apostrophe_case(last_name);
      sql += "INSERT INTO PLAYER (PLAYER_ID, TEAM_ID, UNIFORM_NUM, FIRST_NAME, "
             "LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) "
             "VALUES (" +
             player_id + ", " + team_id + ", " + uniform_num + ", '" + fname +
             "', '" + lname + "', " + mpg + ", " + ppg + ", " + rpg + ", " +
             apg + ", " + spg + ", " + bpg + " );";
    }
    players.close();

    work W(*C);
    W.exec(sql);
    W.commit();
    // cout << "Player Records created successfully" << endl;
  }

  else {
    cout << "Unable to open file 'player.txt'";
  }
}

void InsertInfo_Team(connection *C) {
  ifstream teams("team.txt");
  string team_id;
  string name;
  string state_id;
  string color_id;
  string wins;
  string losses;
  string sql;

  if (teams.is_open()) {
    while (teams >> team_id >> name >> state_id >> color_id >> wins >> losses) {
      sql += "INSERT INTO TEAM (TEAM_ID, NAME, "
             "STATE_ID, COLOR_ID, WINS, LOSSES) "
             "VALUES (" +
             team_id + ", '" + name + "', " + state_id + ", " + color_id +
             ", " + wins + ", " + losses + " );";
    }
    teams.close();

    work W(*C);
    W.exec(sql);
    W.commit();
    // cout << "Team Records created successfully" << endl;
  }

  else {
    cout << "Unable to open file 'player.txt'";
  }
}

void InsertInfo_State(connection *C) {
  ifstream states("state.txt");
  string state_id;
  string name;
  string sql;

  if (states.is_open()) {
    while (states >> state_id >> name) {
      sql += "INSERT INTO STATE (STATE_ID, NAME) "
             "VALUES (" +
             state_id + ", '" + name + "' );";
    }
    states.close();

    work W(*C);
    W.exec(sql);
    W.commit();
    // cout << "State Records created successfully" << endl;
  }

  else {
    cout << "Unable to open file 'state.txt'";
  }
}

void InsertInfo_Color(connection *C) {
  ifstream colors("color.txt");
  string color_id;
  string name;
  string sql;

  if (colors.is_open()) {
    while (colors >> color_id >> name) {
      sql += "INSERT INTO COLOR (COLOR_ID, NAME) "
             "VALUES (" +
             color_id + ", '" + name + "' );";
    }
    colors.close();

    work W(*C);
    W.exec(sql);
    W.commit();
    // cout << "Color Records created successfully" << endl;
  }

  else {
    cout << "Unable to open file 'color.txt'";
  }
}

int main(int argc, char *argv[]) {

  // Allocate & initialize a Postgres connection object
  connection *C;

  try {
    // Establish a connection to the database
    // Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      // cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }

    // cout << endl;
    CreateTable_State(C);
    CreateTable_Color(C);
    CreateTable_Team(C);
    CreateTable_Player(C);
    // cout << endl;

    InsertInfo_State(C);
    InsertInfo_Color(C);
    InsertInfo_Team(C);
    InsertInfo_Player(C);
    // cout << endl;

  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
    return 1;
  }

  exercise(C);

  // Close database connection
  C->disconnect();

  // cout << "closed database connection ..." << endl;

  return 0;
}
