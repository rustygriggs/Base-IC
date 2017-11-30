from django.db import models


class Service(models.Model):
    name = models.CharField(max_length=100)

    def __str__(self):
        return "{}".format(self.name)


class Peripheral(models.Model):
    # Example zigbee address: 0013a200415537c5
    address = models.CharField(max_length=50)
    queue = models.CharField(max_length=20)
    name = models.CharField(max_length=50)
    services = models.ManyToManyField(Service, through='PeripheralService')

    def __str__(self):
        return "{}".format(self.name)

    class Meta:
        unique_together = ('queue', 'address')


class PeripheralService(models.Model):
    INPUT = 'I'
    OUTPUT = 'O'
    DIRECTIONS = (
        (INPUT, 'Input'),
        (OUTPUT, 'Output')
    )

    peripheral = models.ForeignKey(Peripheral, on_delete=models.DO_NOTHING)
    service = models.ForeignKey(Service, on_delete=models.DO_NOTHING)
    service_number = models.IntegerField()
    direction = models.CharField(max_length=1, choices=DIRECTIONS)
    service_name = models.CharField(max_length=50, null=True)

    def __str__(self):
        return "Peripheral: {} Service #{}: {} Direction: {} Service Name {}".format(
            self.peripheral,
            self.service_number,
            self.service,
            self.get_direction_display(),
            self.service_name
        )

    class Meta:
        db_table = 'api_peripheral_service'


class Recipe(models.Model):
    input_peripheral_service = models.ForeignKey(PeripheralService,
                                                 related_name="input_peripheral_service",
                                                 on_delete=models.DO_NOTHING)
    input_value = models.CharField(max_length=500)
    output_peripheral_service = models.ForeignKey(PeripheralService,
                                                  related_name="output_peripheral_service",
                                                  on_delete=models.DO_NOTHING)
    output_value = models.CharField(max_length=500)
    delay = models.IntegerField(default=0)

    def __str__(self):
        return "Input Peripheral \"{}\" Service \"{}\" Service Number \"{}\" Value \"{}\" " \
               "- Output Peripheral \"{}\" Service \"{}\" Service Number \"{}\" Value \"{}\"".format(
            self.input_peripheral_service.peripheral.name,
            self.input_peripheral_service.service,
            self.input_peripheral_service.service_number,
            self.input_value,
            self.output_peripheral_service.peripheral.name,
            self.output_peripheral_service.service,
            self.output_peripheral_service.service_number,
            self.output_value
        )
