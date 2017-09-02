# -*- coding: utf-8 -*-
# Generated by Django 1.11.2 on 2017-08-30 03:00
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Peripheral',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('address', models.CharField(max_length=50)),
                ('queue', models.CharField(max_length=20)),
                ('name', models.CharField(max_length=50)),
            ],
        ),
        migrations.CreateModel(
            name='Service',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(max_length=100)),
            ],
        ),
        migrations.AddField(
            model_name='peripheral',
            name='services',
            field=models.ManyToManyField(to='api.Service'),
        ),
        migrations.AlterUniqueTogether(
            name='peripheral',
            unique_together=set([('queue', 'address')]),
        ),
    ]
