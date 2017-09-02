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
    services = models.ManyToManyField(Service)

    def __str__(self):
        return "{}".format(self.name)

    class Meta:
        unique_together = ('queue', 'address')
