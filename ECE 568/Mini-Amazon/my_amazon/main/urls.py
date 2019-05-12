from django.urls import path
from . import views

urlpatterns = [
    path('', views.home, name='main-home'),
    path('order/new/', views.order, name='order-create'),
    path('fill/', views.fill, name='main-fill'),
    path('status_order/', views.status, name='main-status'),
    path('status_fill/', views.status2, name='main-status2'),
]