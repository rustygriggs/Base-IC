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
            'input_peripheral_service',
            'input_value',
            'output_peripheral_service',
            'output_value',
            'delay'
        )
        depth = 1
