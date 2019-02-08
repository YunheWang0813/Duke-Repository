import django_filters
from .models import Ride


class SharerSearchFilter(django_filters.FilterSet):
    arrival_between = django_filters.DateTimeFromToRangeFilter(field_name='arrival', label='Arrival(Between)')

    class Meta:
        model = Ride
        fields = ['address']
