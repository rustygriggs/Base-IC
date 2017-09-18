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

    def __str__(self):
        return "Peripheral: {} Service #{}: {} Direction: {}".format(
            self.peripheral,
            self.service_number,
            self.service,
            self.get_direction_display()
        )

    class Meta:
        db_table = 'api_peripheral_service'


class Recipe(models.Model):
    from_peripheral_service = models.ForeignKey(PeripheralService,
                                                related_name="from_peripheral_service",
                                                on_delete=models.DO_NOTHING)
    from_value = models.CharField(max_length=500)
    to_peripheral_service = models.ForeignKey(PeripheralService,
                                              related_name="to_peripheral_service",
                                              on_delete=models.DO_NOTHING)
    to_value = models.CharField(max_length=500)
    delay = models.IntegerField(default=0)

    def __str__(self):
        return "From Peripheral \"{}\" Service \"{}\" Service Number \"{}\" Value \"{}\" " \
               "- To Peripheral \"{}\" Service \"{}\" Service Number \"{}\" Value \"{}\"".format(
            self.from_peripheral_service.peripheral.name,
            self.from_peripheral_service.service,
            self.from_peripheral_service.service_number,
            self.from_value,
            self.to_peripheral_service.peripheral.name,
            self.to_peripheral_service.service,
            self.to_peripheral_service.service_number,
            self.to_value
        )
