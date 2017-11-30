from rest_framework import serializers
from .models import Peripheral, Service, Recipe, PeripheralService


class ServiceSerializer(serializers.ModelSerializer):
    class Meta:
        model = Service
        fields = ('id', 'name')


class PeripheralServiceSerializer(serializers.ModelSerializer):
    class Meta:
        model = PeripheralService
        fields = ('service_number', 'direction', 'service', 'service_name')
        depth = 1


class PeripheralSerializer(serializers.ModelSerializer):
    input_services = serializers.SerializerMethodField()
    output_services = serializers.SerializerMethodField()

    def get_input_services(self, obj):
        peripheral_services = PeripheralService.objects.filter(peripheral=obj, direction=PeripheralService.INPUT)
        serializer = PeripheralServiceSerializer(peripheral_services, many=True)
        return serializer.data

    def get_output_services(self, obj):
        peripheral_services = PeripheralService.objects.filter(peripheral=obj, direction=PeripheralService.OUTPUT)
        serializer = PeripheralServiceSerializer(peripheral_services, many=True)
        return serializer.data

    class Meta:
        model = Peripheral
        fields = ('id', 'address', 'queue', 'name', 'input_services', 'output_services')
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
