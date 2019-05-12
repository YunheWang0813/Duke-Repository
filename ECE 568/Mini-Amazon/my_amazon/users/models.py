from django.db import models
from django.contrib.auth.models import User
from django.db.models.signals import post_save
from django.dispatch import receiver
from django.core.exceptions import ObjectDoesNotExist


class Profile(models.Model):
    user_id = models.OneToOneField(User, on_delete=models.CASCADE)
    ups_id = models.IntegerField('UPS ID', null=True, blank=True)
    ups_pass = models.CharField('UPS Password', max_length=255, null=True, blank=True)
    is_prime = models.BooleanField('Prime', default=False)
    

    def __str__(self):
        return f'{self.user_id.username} Profile'


@receiver(post_save, sender=User)
def create_user_profile(sender, instance, created, **kwargs):
    try:
        instance.profile.save()
    except ObjectDoesNotExist:
        Profile.objects.create(user_id=instance)
    # if created:
        # Profile.objects.create(user=instance)


@receiver(post_save, sender=User)
def save_user_profile(sender, instance, **kwargs):
    instance.profile.save()
