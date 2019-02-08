from django.urls import path
from . import views
from .views import (OwnerListView, OwnerDetailView, OwnerCreateView, OwnerUpdateView, OwnerDeleteView,
                    driver_filter, DriverDetailView, DriverConfirmView, DriverCompleteView, DriverOrderlistView,
                    SharerSearchView, SharerJoinView, SharerJoinNumView, SharerListView, SharerDetailView,
                    SharerWithdrawView)

urlpatterns = [
    path('', views.home, name='rider-home'),
    # path('', OwnerListView.as_view(), name='rider-home'),
    path('about/', views.about, name='rider-about'),
    # path('owner/', views.owner, name='rider-owner'),
    path('owner/', OwnerListView.as_view(), name='rider-owner'),
    path('owner_status/<int:pk>/', OwnerDetailView.as_view(), name='rider-owner-status'),
    path('owner_status/<int:pk>/update/', OwnerUpdateView.as_view(), name='rider-owner-update'),
    path('owner_status/<int:pk>/delete/', OwnerDeleteView.as_view(), name='rider-owner-delete'),
    path('owner_create', OwnerCreateView.as_view(), name='rider-owner-create'),
    path('driver/', views.driver, name='rider-driver'),
    path('driver_check/', views.driver_check, name='rider-driver_check'),
    path('driver_filter/', driver_filter.as_view(), name='rider-driver_filter'),
    path('driver_status/<int:pk>/', DriverDetailView.as_view(), name='rider-driver-status'),
    # path('driver_confirm/<int:pk>/confirm/', DriverConfirmView.as_view(), name='rider-driver-confirm'),
    path('driver_confirm/<int:pk>/confirm/', DriverConfirmView.driver_confirm_view, name='rider-driver-confirm'),
    path('driver_complete/<int:pk>/complete/', DriverCompleteView.driver_complete_view, name='rider-driver-complete'),
    path('driver_orderlist/', DriverOrderlistView.as_view(), name='rider-driver-orderlist'),
    # path('driver_complete/<int:pk>/complete/', DriverCompleteView.as_view(), name='rider-driver-complete'),
    path('sharer/', views.sharer, name='rider-sharer'),
    path('sharer_search/', SharerSearchView.as_view(), name='sharer-search'),
    path('sharer_join/<int:pk>/', SharerJoinView.as_view(), name='sharer-join'),
    path('sharer_join/<int:pk>/num/', SharerJoinNumView.sharer_join_num_view, name='sharer-join-num'),
    path('sharer_list/', SharerListView.as_view(), name='sharer-list'),
    path('sharer_status/<int:pk>/', SharerDetailView.as_view(), name='sharer-status'),
    path('sharer_status/<int:pk>/withdraw/', SharerWithdrawView.sharer_withdraw_view, name='sharer-withdraw'),
]
