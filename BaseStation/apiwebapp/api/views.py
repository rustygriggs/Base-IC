from django.http import HttpResponse, JsonResponse, HttpResponseBadRequest
from django.views.generic import View
from .serializers import PeripheralSerializer, RecipeSerializer
from .models import Peripheral, Recipe
from .validator import Validator
from .protocol import Protocol
import json, sys



# Create your views here.
def index(request):
    return HttpResponse("Hello world! You are at the API v1 index.")


class PeripheralView(View):
    def get(self, request):
        """
        This will return a list of all available peripherals
        """

        serializer = PeripheralSerializer(Peripheral.objects.all(), many=True)

        message = {'success': True, 'peripherals': serializer.data}
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

        # Every time we receive a process request it will contains two pieces of data.
        # First will be the data, second will be the address. The queue will come from the URL.

        queue = kwargs['queue']
        address = parsed_data['address']
        data = parsed_data['data']

        # Data will contain an array that will be in the format according to our overleaf doc.
        message = Protocol(queue, address, data).process()

        status_code = 200
        if not message['success']:
            status_code = 400  # 400 Bad Request

        return JsonResponse(message, safe=False, status=status_code)


class RecipeView(View):
    def get(self, request, *args, **kwargs):
        """
        This will return a list of all the existing recipes.
        """

        if 'id' in kwargs:
            serializer = RecipeSerializer(Recipe.objects.get(pk=kwargs['id']))
        else:
            serializer = RecipeSerializer(Recipe.objects.all(), many=True)

        message = {'success': True, 'recipes': serializer.data}
        return JsonResponse(message, safe=False)

    def post(self, request, *args, **kwargs):
        """
        This will create a new recipe.
        """

        parsed_data = json.loads(request.body.decode())
        if 'id' in kwargs:
            # If id is present then we are updating a recipe.
            return self._update_recipe(kwargs['id'], parsed_data)
        else:
            # Otherwise, we are creating a new recipe.
            return self._update_recipe(parsed_data)

    def _update_recipe(self, id, data):
        allowed_updates = ['input_value', 'output_value', 'delay']

        recipe = Recipe.objects.get(pk=id)

        for column, value in data.items():
            if column not in allowed_updates:
                return HttpResponseBadRequest('Tried to update a field that is not allowed to be updated.')

            setattr(recipe, column, value)

        try:
            recipe.save()
            serializer = RecipeSerializer(recipe)
            return JsonResponse({'success': True, 'recipe': serializer.data})
        except ValueError as err:
            return HttpResponseBadRequest(err)

    def _create_recipe(self, data):
        return HttpResponseBadRequest("Not implemented")

        v = Validator(data, ['input_peripheral_service_id', 'input_value', 'output_peripheral_service_id', 'output_value'])
        if v.has_errors():
            return HttpResponseBadRequest(v.get_message())

        # DO STUFF WITH THE DATA HERE
        message = {'success': False}

        status_code = 200
        if not message['success']:
            status_code = 400  # 400 Bad Request

        return JsonResponse(message, safe=False, status=status_code)
