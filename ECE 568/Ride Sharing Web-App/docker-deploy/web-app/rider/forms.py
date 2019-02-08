from django import forms
from django.contrib.auth.models import User
from .models import Ride
from django.core.validators import MaxValueValidator, MinValueValidator
from django.utils import timezone
from django.core.exceptions import ValidationError


class OwnerUpdateForm(forms.Form):
    address = forms.CharField()
    arrival = forms.DateTimeField()
    num_pass = forms.IntegerField()
    CanShare = forms.BooleanField()

    def cleaned_arrival(self):
        arrival = self.cleaned_data['arrival']
        if arrival < timezone.now():
            raise ValidationError('Please put future time!')
        return arrival

    def cleaned_num_pass(self):
        num_pass = self.cleaned_data['num_pass']
        if num_pass < 1 or num_pass > 10:
            raise ValidationError('Please put number between 1 to 10!')
        return num_pass


