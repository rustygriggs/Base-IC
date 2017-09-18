# -*- coding: utf-8 -*-
# Generated by Django 1.11.2 on 2017-09-17 23:57
from __future__ import unicode_literals

from django.db import migrations, models
import django.db.models.deletion


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
            name='PeripheralService',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('service_number', models.IntegerField()),
                ('direction', models.CharField(choices=[('I', 'Input'), ('O', 'Output')], max_length=1)),
                ('peripheral', models.ForeignKey(on_delete=django.db.models.deletion.DO_NOTHING, to='api.Peripheral')),
            ],
            options={
                'db_table': 'api_peripheral_service',
            },
        ),
        migrations.CreateModel(
            name='Recipe',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('from_peripheral', models.ForeignKey(on_delete=django.db.models.deletion.DO_NOTHING, related_name='from_peripheral', to='api.Peripheral')),
                ('from_peripheral_service', models.ForeignKey(on_delete=django.db.models.deletion.DO_NOTHING, related_name='from_peripheral_service', to='api.PeripheralService')),
                ('from_value', models.CharField(max_length=500)),
                ('to_peripheral', models.ForeignKey(on_delete=django.db.models.deletion.DO_NOTHING, related_name='to_peripheral', to='api.Peripheral')),
                ('to_peripheral_service', models.ForeignKey(on_delete=django.db.models.deletion.DO_NOTHING, related_name='to_peripheral_service', to='api.PeripheralService')),
                ('to_value', models.CharField(max_length=500)),
                ('delay', models.IntegerField(default=0)),
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
            model_name='peripheralservice',
            name='service',
            field=models.ForeignKey(on_delete=django.db.models.deletion.DO_NOTHING, to='api.Service'),
        ),
        migrations.AddField(
            model_name='peripheral',
            name='services',
            field=models.ManyToManyField(through='api.PeripheralService', to='api.Service'),
        ),
        migrations.AlterUniqueTogether(
            name='peripheral',
            unique_together=set([('queue', 'address')]),
        ),
    ]
