from django.db import models
from django.contrib.auth.models import User
from django.core.validators import MaxValueValidator, MinValueValidator
from django.db.models.signals import post_save
from django.dispatch import receiver
from django.core.exceptions import ObjectDoesNotExist


class Profile(models.Model):
    user = models.OneToOneField(User, on_delete=models.CASCADE)
    car_type = models.CharField('Vehicle Type', max_length=255, null=True, blank=True)
    license = models.CharField('License Plate #', max_length=255, null=True,  blank=True)
    max_pass = models.PositiveIntegerField('Maximum Passengers #',
                                           validators=[MinValueValidator(1), MaxValueValidator(10)],
                                           null=True, blank=True)

    def __str__(self):
        return f'{self.user.username} Profile'


@receiver(post_save, sender=User)
def create_user_profile(sender, instance, created, **kwargs):
    try:
        instance.profile.save()
    except ObjectDoesNotExist:
        Profile.objects.create(user=instance)
    # if created:
        # Profile.objects.create(user=instance)


@receiver(post_save, sender=User)
def save_user_profile(sender, instance, **kwargs):
    instance.profile.save()
