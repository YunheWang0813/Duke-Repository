from django import forms
from django.contrib.auth.models import User
from .models import Order, Item


class OrderCreationForm(forms.ModelForm):

    class Meta:
        model = Order
        fields = ['item_id', 'x', 'y', 'bought_quantity']


class ItemFillForm(forms.ModelForm):

    class Meta:
        model = Item
        fields = ['name', 'quantity']
