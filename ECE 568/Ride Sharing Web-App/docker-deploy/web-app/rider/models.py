from django.db import models
from django.utils import timezone
from django.contrib.auth.models import User
from django.urls import reverse
from django.core.validators import MaxValueValidator, MinValueValidator


class Ride(models.Model):
    owner = models.ForeignKey(User, on_delete=models.CASCADE, related_name='ride_owner', null=True, blank=True)
    driver = models.ForeignKey(User, on_delete=models.SET_NULL, related_name='ride_driver', null=True, blank=True)
    sharer = models.ManyToManyField(User, related_name='ride_sharer', null=True, blank=True)
    address = models.CharField('Destination Address', max_length=255, null=True, blank=False)
    arrival = models.DateTimeField('Required Arrival Time', default=timezone.now, null=True, blank=False)
    num_pass = models.PositiveIntegerField('Passengers #', default=1,
                                           validators=[MinValueValidator(1), MaxValueValidator(10)],
                                           null=True, blank=False)
    CanShare = models.BooleanField('Share Ride Allowed?', default=False)
    status = models.CharField('Status', max_length=255, choices=[("open", "open"), ("confirmed", "confirmed"),
                                                                 ("completed", "completed")], default='open')

    def __str__(self):
        # return f'{self.owner.username} Ride Request'
        return f'{self.address} Ride'

    def get_absolute_url(self):
        return reverse('rider-home')
