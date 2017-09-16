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


class ProcessView(View):
    def post(self, request, *args, **kwargs):
        """
        This will process a request that is received through the API worker.
        """

        v = Validator(kwargs, ['queue'])
        if v.has_errors():
            return HttpResponseBadRequest(v.get_message())

        parsed_data = json.loads(request.body.decode())
        v = Validator(parsed_data, ['data', 'address'])
        if v.has_errors():
            return HttpResponseBadRequest(v.get_message())

        # In order to be quick about this there is going to be a lot of code here.
        # This should be moved into it's own class.

        # Everytime we receive a process request it will contains two pieces of data.
        # First will be the data, second will be the address. The queue will come from the URL.

        data = parsed_data['data']
        address = parsed_data['address']
        queue = kwargs['queue']

        # Data will contain an array that will be in the format according to our overleaf doc.

        # First will be the registration command.
        if data[0] == '0':
            # We are registering a new device.
            name = data[1]

            # The rest of the indexes will be the provided services.
            services = set()
            for service in data[2:]:
                # Each service will be an ID that relates to a specific service.
                services.add(int(service))

            try:
                # Check to see if the peripheral already exists. If it does then lets do an update.
                peripheral_set = Peripheral.objects.filter(address=address, queue=queue)
                peripheral = peripheral_set.first()

                # First we will update the name if it has changed
                if peripheral.name != name:
                    peripheral.name = name
                    peripheral.save()

                # Next we will get all the related services and add/remove them as necessary.
                peripheral_service_ids = set(peripheral_set.values_list('services__id', flat=True))

                services_to_add = services - peripheral_service_ids
                for service in services_to_add:
                    try:
                        service = Service.objects.get(pk=service)
                        print("Adding service {}".format(service))
                        peripheral.services.add(service)
                    except Service.DoesNotExist:
                        message = {'success': False, 'message': 'Service with id "{}" does not exist'.format(service)}
                        return JsonResponse(message, safe=False)

                services_to_remove = peripheral_service_ids - services
                for service in services_to_remove:
                    try:
                        service = Service.objects.get(pk=service)
                        print("Removing service {}".format(service))
                        peripheral.services.remove(service)
                    except Service.DoesNotExist:
                        message = {'success': False, 'message': 'Service with id "{}" does not exist'.format(service)}
                        return JsonResponse(message, safe=False)

            except Peripheral.DoesNotExist:
                # We are creating a new peripheral.
                peripheral = Peripheral(queue=queue, address=address, name=name)
                peripheral.save()
                
                for service in data[2:]:
                    try:
                        service = Service.objects.get(pk=service)
                        peripheral.services.add(service)
                    except Service.DoesNotExist:
                        message = {'success': False, 'message': 'Service with id "{}" does not exist'.format(service)}
                        return JsonResponse(message, safe=False)

                message = {'succes': True, 'message': 'Peripheral was created successfully'}
                return JsonResponse(message, safe=False)

        message = {'success': True, 'message': 'Got it!'}
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
