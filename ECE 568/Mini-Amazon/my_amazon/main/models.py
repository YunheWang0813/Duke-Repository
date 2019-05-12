from django.db import models
from django.utils import timezone
from django.contrib.auth.models import User
from django.core.validators import MaxValueValidator, MinValueValidator
from django.urls import reverse


class Item(models.Model):
    name = models.CharField('Name', max_length=255, null=True, blank=False)
    quantity = models.IntegerField('Quantity', validators=[MinValueValidator(0)],
                                   default=1, null=True, blank=False)

    def __str__(self):
        return self.name

    class Meta:
        ordering = ['name']


class Warehouse(models.Model):
    x = models.IntegerField('Address X', validators=[MinValueValidator(0), MaxValueValidator(360)],
                            null=True, blank=False)
    y = models.IntegerField('Address Y', validators=[MinValueValidator(0), MaxValueValidator(360)],
                            null=True, blank=False)


class Order(models.Model):
    user_id = models.ForeignKey(User, on_delete=models.CASCADE, related_name='order_user', null=True, blank=True)
    item_id = models.ForeignKey(Item, on_delete=models.SET_NULL, related_name='order_item', null=True, blank=True)
    warehouse_id = models.ForeignKey(Warehouse, on_delete=models.CASCADE, related_name='order_warehouse',
                                  null=True, blank=True, default=1)
    truck_id = models.IntegerField('Truck ID', null=True, blank=True)
    x = models.IntegerField('Address X', validators=[MinValueValidator(0), MaxValueValidator(360)],
                            null=True, blank=False)
    y = models.IntegerField('Address Y', validators=[MinValueValidator(0), MaxValueValidator(360)],
                            null=True, blank=False)
    time_created = models.DateTimeField('Time created', default=timezone.now, null=True, blank=True)
    bought_quantity = models.PositiveIntegerField('Buy Quantity', default=1, null=True, blank=False)
    status = models.CharField('Status', max_length=255, choices=[("pending", "pending"), ("to pack", "to pack"),
                                                                 ("ready", "ready"), ("load", "load"),
                                                                 ("loaded", "loaded"), ("delivering", "delivering"),
                                                                 ("delivered", "delivered")], default='pending')
    s_status = models.CharField('Super Status', max_length=255, choices=[("not packed", "not packed"), ("packing", "packing"),
                                                                 ("not loaded", "not loaded"), ("loading", "loading")], default='not packed')

class Fill(models.Model):
    user_id = models.ForeignKey(User, on_delete=models.CASCADE, related_name='fill_user', null=True, blank=True)
    item_id = models.ForeignKey(Item, on_delete=models.SET_NULL, related_name='fill_item', null=True, blank=True)
    #warehouse = models.ForeignKey(Warehouse, on_delete=models.CASCADE, related_name='fill_warehouse', null=True, blank=True)
    time_created = models.DateTimeField('Time created', default=timezone.now, null=True, blank=True)
    fill_quantity = models.PositiveIntegerField('Fill Quantity', default=1, null=True, blank=False)
    status = models.CharField('Status', max_length=255, choices=[("unfilled", "unfilled"), ("filled", "filled")], default='unfilled')
