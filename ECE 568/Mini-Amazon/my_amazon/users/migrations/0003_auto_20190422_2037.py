# Generated by Django 2.2 on 2019-04-22 20:37

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('users', '0002_auto_20190422_1545'),
    ]

    operations = [
        migrations.AlterField(
            model_name='profile',
            name='ups_id',
            field=models.IntegerField(blank=True, null=True, verbose_name='UPS ID'),
        ),
    ]
