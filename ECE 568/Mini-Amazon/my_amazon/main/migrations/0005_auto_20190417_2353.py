# Generated by Django 2.2 on 2019-04-17 23:53

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('main', '0004_auto_20190417_2240'),
    ]

    operations = [
        migrations.AlterField(
            model_name='order',
            name='bought_quantity',
            field=models.PositiveIntegerField(default=1, null=True, verbose_name='Buy Quantity'),
        ),
    ]