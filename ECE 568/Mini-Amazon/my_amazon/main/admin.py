from django.contrib import admin
from .models import Item, Warehouse, Order, Fill
# Register your models here.

admin.site.register(Item)
admin.site.register(Warehouse)
admin.site.register(Order)
admin.site.register(Fill)