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


class Workflow(models.Model):
    from_peripheral = models.ForeignKey(Peripheral, related_name="from_peripheral", on_delete=models.DO_NOTHING)
    from_service = models.ForeignKey(Service, related_name="from_service", on_delete=models.DO_NOTHING)
    from_service_number = models.IntegerField(name="from_service_number")
    from_value = models.CharField(max_length=500)
    to_peripheral = models.ForeignKey(Peripheral, related_name="to_peripheral", on_delete=models.DO_NOTHING)
    to_service = models.ForeignKey(Service, related_name="to_service", on_delete=models.DO_NOTHING)
    to_service_number = models.IntegerField(name="to_service_number")
    to_value = models.CharField(max_length=500)

    def __str__(self):
        return "From Peripheral \"{}\" Service \"{}\" Service Number \"{}\" Value \"{}\" " \
               "- To Peripheral \"{}\" Service \"{}\" Service Number \"{}\" Value \"{}\"".format(
            self.from_peripheral,
            self.from_service,
            self.from_service_number,
            self.from_value,
            self.to_peripheral,
            self.to_service,
            self.to_service_number,
            self.to_value
        )
