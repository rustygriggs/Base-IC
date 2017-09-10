from django.http import HttpResponse, JsonResponse, HttpResponseBadRequest
from django.views.generic import View
from .serializers import PeripheralSerializer, WorkflowSerializer
from .models import Peripheral, Service, Workflow
from .validator import Validator
import json
from .remote_queue import RemoteQueue


# Create your views here.
def index(request):
    return HttpResponse("Hello world! You are at the API v1 index.")


class PeripheralView(View):
    def get(self, request):
        """
        This will return a list of all available peripherals
        """

        serializer = PeripheralSerializer(Peripheral.objects.all(), many=True)

        message = {'success': True, 'peripheral': serializer.data}
        return JsonResponse(message, safe=False)

    def post(self, request):
        """
        This will create a new peripheral from an address and a name provided in the post request.
        If the address is already being used then the peripheral will just be returned.
        :return:
        """

        parsed_data = json.loads(request.body.decode())

        v = Validator(parsed_data, ['queue', 'address', 'name', 'services'])
        if v.has_errors():
            return HttpResponseBadRequest(v.get_message())

        # Now we have all the data that we need to create or lookup a peripheral.
        try:
            peripheral = Peripheral.objects.get(queue=parsed_data['queue'], address=parsed_data['address'])
            # If the name is different then we can update the name.
            if peripheral.name is not parsed_data['name']:
                peripheral.name = parsed_data['name']
                peripheral.save()

                # TODO: If services is present then we need to add any that don't exist and remove any that
                # aren't present but exist in the DB.
        except Peripheral.DoesNotExist:
            # Since there was no peripheral found we can create one.
            peripheral = Peripheral.objects.create(queue=parsed_data['queue'],
                                                   address=parsed_data['address'],
                                                   name=parsed_data['name'])

        serializer = PeripheralSerializer(peripheral)

        message = {'success': True, 'peripheral': serializer.data}
        return JsonResponse(message, safe=False)


class PeripheralDetailsView(View):
    def get(self, request, *args, **kwargs):
        """
        This will list a single peripheral from a parameter in the url called PK
        """

        v = Validator(kwargs, ['queue', 'address'])
        if v.has_errors():
            return HttpResponseBadRequest(v.get_message())

        return HttpResponse("This is a get request")


class PeripheralActionView(View):
    def post(self, request):
        """
        This will receive an action from a peripheral and add any results from the workflow's
        table to Rabbit
        """

        parsed_data = json.loads(request.body.decode())

        v = Validator(parsed_data, ['queue', 'address', 'service', 'service_number', 'value'])
        if v.has_errors():
            return HttpResponseBadRequest(v.get_message())

        # Find peripheral by queue and address
        peripheral = Peripheral.objects.get(queue=parsed_data['queue'], address=parsed_data['address'])

        # If the peripheral exists then lookup the results from the workflow table using the
        # service ID, service number, and the service value
        workflows = []
        if peripheral:
            workflows = Workflow.objects.filter(from_peripheral=peripheral,
                                                from_service_id=parsed_data['service'],
                                                from_service_number=parsed_data['service_number'],
                                                from_value=parsed_data['value'])

            if workflows:
                # If we found any workflows attached to the received action then publish them to the remote queue.
                RemoteQueue.publish_workflows_to_queue(workflows)

        serializer = WorkflowSerializer(workflows, many=True)

        message = {'success': True, 'workflows': serializer.data}
        return JsonResponse(message, safe=False)
