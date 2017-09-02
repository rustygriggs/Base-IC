from rest_framework import serializers
from .models import Peripheral, Service


class ServiceSerializer(serializers.ModelSerializer):
    class Meta:
        model = Service
        fields = ('id', 'name')


class PeripheralSerializer(serializers.ModelSerializer):
    class Meta:
        model = Peripheral
        fields = ('id', 'address', 'queue', 'name', 'services')
        depth = 1
