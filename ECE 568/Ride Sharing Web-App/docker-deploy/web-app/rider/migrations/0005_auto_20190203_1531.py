# Generated by Django 2.1.5 on 2019-02-03 20:31

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('rider', '0004_auto_20190202_0343'),
    ]

    operations = [
        migrations.AlterField(
            model_name='ride',
            name='status',
            field=models.CharField(choices=[('open', 'open'), ('confirmed', 'confirmed'), ('completed', 'completed')], default='open', max_length=255, verbose_name='Status'),
        ),
    ]