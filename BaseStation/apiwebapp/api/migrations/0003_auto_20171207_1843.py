# -*- coding: utf-8 -*-
# Generated by Django 1.11.2 on 2017-12-08 01:43
from __future__ import unicode_literals

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('api', '0002_peripheralservice_service_name'),
    ]

    operations = [
        migrations.AlterField(
            model_name='peripheralservice',
            name='peripheral',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='api.Peripheral'),
        ),
        migrations.AlterField(
            model_name='peripheralservice',
            name='service',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='api.Service'),
        ),
        migrations.AlterField(
            model_name='recipe',
            name='input_peripheral_service',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='input_peripheral_service', to='api.PeripheralService'),
        ),
        migrations.AlterField(
            model_name='recipe',
            name='output_peripheral_service',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='output_peripheral_service', to='api.PeripheralService'),
        ),
    ]