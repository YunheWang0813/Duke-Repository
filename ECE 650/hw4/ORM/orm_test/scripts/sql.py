from orm_demo.models import Color, Player, State, Team


def run():
    # query1(C, 1, 35, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG")
    player_q1 = Player.objects.filter(mpg__gte=35, mpg__lte=40)
    for q1 in player_q1:
        print(q1.player_id, q1.team_id, q1.uniform_num, q1.first_name, q1.last_name, q1.mpg, q1.ppg, q1.rpg, q1.apg, q1.spg, q1.bpg)
    print()

    # query1(C, 1, 35, 40, 1, 15, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG")
    player_q1 = Player.objects.filter(mpg__gte=35, mpg__lte=40, ppg__gte=15, ppg__lte=20)
    for q1 in player_q1:
        print(q1.player_id, q1.team_id, q1.uniform_num, q1.first_name, q1.last_name, q1.mpg, q1.ppg, q1.rpg, q1.apg, q1.spg, q1.bpg)
    print()

    # query2(C, "Gold");
    print("NAME")
    team_q2 = Team.objects.filter(color__name="Gold")
    for q2 in team_q2:
        print(q2.name)
    print()

    # query3(C, "Duke");
    print("FIRST_NAME LAST_NAME")
    player_q3 = Player.objects.filter(team__name="Duke").order_by("-ppg")
    for q3 in player_q3:
        print(q3.first_name, q3.last_name)
    print()

    # query4(C, "NY", "Orange");
    print("FIRST_NAME LAST_NAME UNIFORM_NUM")
    team_q4 = Team.objects.filter(state__name="NY", color__name="Orange")
    for i in range(len(team_q4)):
        player_q4 = Player.objects.filter(team__team_id=team_q4[i].team_id)
        for player in player_q4:
            print(player.first_name, player.last_name, player.uniform_num)
    print()

    # query5(C, 12);
    print("FIRST_NAME LAST_NAME NAME WINS")
    team_q5 = Team.objects.filter(wins__gt=12)
    for i in range(len(team_q5)):
        player_q5 = Player.objects.filter(team__team_id=team_q5[i].team_id)
        for player in player_q5:
                print(player.first_name, player.last_name, team_q5[i].name, team_q5[i].wins)
