from rest_framework import serializers
from .models import Peripheral, Service, Workflow


class ServiceSerializer(serializers.ModelSerializer):
    class Meta:
        model = Service
        fields = ('id', 'name')


class PeripheralSerializer(serializers.ModelSerializer):
    class Meta:
        model = Peripheral
        fields = ('id', 'address', 'queue', 'name', 'services')
        depth = 1


class WorkflowSerializer(serializers.ModelSerializer):
    class Meta:
        model = Workflow
        fields = (
            'from_peripheral', 'from_service',
            'from_service_number', 'from_value',
            'to_peripheral', 'to_service',
            'to_service_number', 'to_value'
        )
        depth = 1
