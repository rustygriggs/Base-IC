from django.http import HttpResponse, JsonResponse, HttpResponseBadRequest
from django.views.generic import View
from .serializers import PeripheralSerializer
from .models import Peripheral, Service
import json


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
        errors = []

        # Make sure that address, name, and queue are in the request data.
        if 'address' not in parsed_data or not parsed_data['address']:
            errors.append("'address' must be provided and have a value.")

        if 'name' not in parsed_data or not parsed_data['name']:
            errors.append("'name' must be provided and have a value.")

        if 'queue' not in parsed_data or not parsed_data['queue']:
            errors.append("'queue' must be provided and have a value.")

        if 'services' in parsed_data and not len(parsed_data['services']):
            errors.append("If 'services' is present it cannot be empty.")

        # If there are any errors then we cannot continue.
        if len(errors):
            message = {'success': False, 'errors': errors}
            return HttpResponseBadRequest(json.dumps(message))

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

        errors = []

        if 'queue' not in kwargs or not kwargs['queue']:
            errors.append("'queue' must be provided and have a value.")

        if 'address' not in kwargs or not kwargs['address']:
            errors.append("'address' must be provided and have a value.")

        if len(errors):
            message = {'success': False, 'errors': errors}
            return HttpResponseBadRequest(json.dumps(message))

        print(kwargs)

        return HttpResponse("This is a get request")

    def post(self, request):
        """
        This will assign services to a peripheral. It will take a list of services from the post
        request and assign them one by one to the peripheral. It will also have to delete services
        that were not present in the post request.
        """

        return HttpResponse("This is a post request")
