from rest_framework import serializers
from .models import Peripheral, Service, Recipe


class ServiceSerializer(serializers.ModelSerializer):
    class Meta:
        model = Service
        fields = ('id', 'name')


class PeripheralSerializer(serializers.ModelSerializer):
    class Meta:
        model = Peripheral
        fields = ('id', 'address', 'queue', 'name', 'services')
        depth = 1


class RecipeSerializer(serializers.ModelSerializer):
    class Meta:
        model = Recipe
        fields = (
            'from_peripheral_service',
            'from_value',
            'to_peripheral_service',
            'to_value',
            'delay'
        )
        depth = 1
