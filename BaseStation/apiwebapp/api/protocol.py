from .models import PeripheralService, Peripheral, Service, Recipe
from .remote_queue import RemoteQueue
from .serializers import RecipeSerializer
import sys


class ProtocolException(Exception):
    pass


class Protocol:
    def __init__(self, queue, address, data):
        self.queue = queue
        self.address = address
        self.data = data

    def process(self):
        # First will be the registration command.
        if self.data[0] == '0':
            return self._process_registration()
        else:
            return self._process_command()

    def _process_command(self):

        service_id = self.data[0]
        service_number = self.data[1]
        value = self.data[2]

        # Find peripheral by queue and address
        peripheral = Peripheral.objects.get(queue=self.queue, address=self.address)


        # If the peripheral exists then lookup the results from the recipe table using the
        # service ID, service number, and the service value
        recipes = []
        if peripheral:
            try:
                peripheral_service = PeripheralService.objects.filter(peripheral=peripheral,
                                                                      service_number=service_number,
                                                                      service_id=service_id,
                                                                      direction=PeripheralService.INPUT).first()

                # If the service is a range service then we dont use the input_value
                if service_id == '2':
                    recipes = Recipe.objects.filter(input_peripheral_service=peripheral_service)
                    # And we will replace the output_value with the input value we received.
                    for recipe in recipes:
                        recipe.output_value = value
                else:
                    recipes = Recipe.objects.filter(input_peripheral_service=peripheral_service,
                                                    input_value=value)
                if recipes:
                # If we found any recipes attached to the received action then publish them to the remote queue.
                    RemoteQueue.publish_recipes_to_queue(recipes)
            except PeripheralService.DoesNotExist:
                return {'success': False, 'message': 'Peripheral Service was not found'}

        serializer = RecipeSerializer(recipes, many=True)

        sys.stdout.flush()
        return {'success': True, 'recipes': serializer.data}

    def _process_registration(self):
        """
        The peripheral has sent a registration command, so lets register it.
        :returns An array representing the success and message of the registration.
        """
        # We are registering a new device.
        name = self.data[1]

        # First find the start and end index of the input services.
        input_services = []
        input_services_start = 3
        input_services_end = 3 + int(self.data[2])
        for input_service in self.data[input_services_start:input_services_end]:
            # Each service will be an ID that relates to a specific service.
            input_services.append(int(input_service))

        # Then find the start and end index of the output services.
        output_services = []
        output_services_start = 3 + int(self.data[2]) + 1
        output_services_end = output_services_start + int(self.data[input_services_end])
        for output_service in self.data[output_services_start:output_services_end]:
            output_services.append(int(output_service))

        try:
            # Check to see if the peripheral already exists.
            peripheral = Peripheral.objects.get(address=self.address, queue=self.queue)

            # The peripheral already exists so lets make sure the name is correct.
            if peripheral.name != name:
                peripheral.name = name
                peripheral.save()

        except Peripheral.DoesNotExist:
            # The peripheral did not exist so we'll create a new one.
            peripheral = Peripheral(queue=self.queue, address=self.address, name=name)
            peripheral.save()

        try:
            # Then we will and and remove any services that are necessary.
            Protocol._add_remove_services(peripheral, PeripheralService.INPUT, input_services)
            Protocol._add_remove_services(peripheral, PeripheralService.OUTPUT, output_services)
        except ProtocolException as exception:
            return {'success': False, 'message': exception.args}

        # If we made it here then the peripheral was created or updated successfully.
        return {'success': True, 'message': 'Peripheral updated successfully.'}

    @staticmethod
    def _add_remove_services(peripheral, direction, services):
        """
        Since every row in the peripheral services map will have an ID and peripherals are expected to register
        themselves every time they turn on, we must be careful to not delete and remove mapping rows every time.
        If we do then the ID's could skyrocket without any gain.
        :param peripheral: Peripheral
        :param direction: string PeripheralService.INPUT or PeripheralService.OUTPUT
        :param services: list List of services that the peripheral provides in the specific direction. Order matters
        as the service numbers come from the order of this array.
        :raises ProtocolException
        """
        # Next we will get all the related services and add/remove them as necessary.
        existing_services = PeripheralService.objects.filter(peripheral=peripheral,
                                                             direction=direction).all()

        # Add missing services
        for service_number, service_id in enumerate(services):
            # We have to do service_number + 1 because we can't send 0's very easily.
            if not existing_services.filter(service_number=(service_number + 1),
                                            service_id=service_id):
                try:
                    service = Service.objects.get(pk=service_id)

                    PeripheralService.objects.create(
                        peripheral=peripheral,
                        service=service,
                        service_number=(service_number + 1),
                        direction=direction
                    )

                except Service.DoesNotExist:
                    raise ProtocolException("Service with id '{}' does not exist.".format(service_id))

        # Remove extra services
        # TODO there is a better way to do this, but it is escaping me right now.
        for existing_service in existing_services:
            found_service = False
            for service_number, service_id in enumerate(services):
                # We have to do service_number + 1 because we can't send 0's very easily.
                if existing_service.service_number == service_number + 1 and existing_service.service_id == service_id:
                    found_service = True
                    break

            # If there was no match for the requested input_services then we should delete the existing service.
            if not found_service:
                existing_service.delete()
