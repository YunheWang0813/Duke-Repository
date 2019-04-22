# This is an auto-generated Django model module.
# You'll have to do the following manually to clean this up:
#   * Rearrange models' order
#   * Make sure each model has one field with primary_key=True
#   * Make sure each ForeignKey has `on_delete` set to the desired behavior.
#   * Remove `managed = False` lines if you wish to allow Django to create, modify, and delete the table
# Feel free to rename the models, but don't rename db_table values or field names.
from django.db import models


class Color(models.Model):
    color_id = models.IntegerField(primary_key=True)
    name = models.TextField()

    class Meta:
        managed = False
        db_table = 'color'


class Player(models.Model):
    player_id = models.IntegerField(primary_key=True)
    team = models.ForeignKey('Team', on_delete=models.CASCADE, related_name="player_team")
    uniform_num = models.IntegerField()
    first_name = models.CharField(max_length=50)
    last_name = models.CharField(max_length=50)
    mpg = models.IntegerField()
    ppg = models.IntegerField()
    rpg = models.IntegerField()
    apg = models.IntegerField()
    spg = models.FloatField()
    bpg = models.FloatField()

    class Meta:
        managed = False
        db_table = 'player'


class State(models.Model):
    state_id = models.IntegerField(primary_key=True)
    name = models.TextField()

    class Meta:
        managed = False
        db_table = 'state'


class Team(models.Model):
    team_id = models.IntegerField(primary_key=True)
    name = models.TextField()
    state = models.ForeignKey(State, on_delete=models.CASCADE, related_name="team_state")
    color = models.ForeignKey(Color, on_delete=models.CASCADE, related_name="team_color")
    wins = models.IntegerField()
    losses = models.IntegerField()

    class Meta:
        managed = False
        db_table = 'team'
