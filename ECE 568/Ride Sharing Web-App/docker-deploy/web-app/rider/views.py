from django.shortcuts import render, redirect, get_object_or_404
from django.views.generic import ListView, DetailView, CreateView, UpdateView, DeleteView
from django.http import HttpResponse, HttpResponseRedirect
from django.contrib import messages
from django.contrib.auth.decorators import login_required
from django.contrib.auth.mixins import LoginRequiredMixin, UserPassesTestMixin
from django.db.models import Q, F
from django.core.mail import send_mail
from django.conf import settings
from .models import Ride
from .forms import OwnerUpdateForm
from .filters import SharerSearchFilter


def home(request):
    '''
    context = {
        'rides': Ride.objects.all()
    }
    '''
    return render(request, 'rider/home.html')


class OwnerListView(ListView):
    model = Ride
    template_name = 'rider/owner.html'
    context_object_name = 'rides'
    ordering = ['-arrival']


class OwnerDetailView(DetailView):
    model = Ride
    template_name = 'rider/owner_status.html'


class OwnerCreateView(LoginRequiredMixin, CreateView):
    model = Ride
    fields = ['address', 'arrival', 'num_pass', 'CanShare']
    template_name = 'rider/owner_create.html'

    def form_valid(self, form):
        form.instance.owner = self.request.user
        return super().form_valid(form)


class OwnerUpdateView(LoginRequiredMixin, UserPassesTestMixin, UpdateView):
    model = Ride
    fields = ['address', 'arrival', 'num_pass', 'CanShare']
    template_name = 'rider/owner_create.html'

    def form_valid(self, form):
        form.instance.owner = self.request.user
        return super().form_valid(form)

    def test_func(self):
        ride = self.get_object()
        if self.request.user == ride.owner:
            return True
        return False


class OwnerDeleteView(LoginRequiredMixin, UserPassesTestMixin, DeleteView):
    model = Ride
    template_name = 'rider/owner_delete.html'
    success_url = '/owner/'

    def test_func(self):
        ride = self.get_object()
        if self.request.user == ride.owner:
            return True
        return False


def about(request):
    return render(request, 'rider/about.html', {'title': 'About'})


@login_required()
def owner(request):
    '''
    ride = Ride()
    if request.method == 'POST':
        owner_form = OwnerUpdateForm(request.POST)
        if owner_form.is_valid():
            ride.owner = request.user
            ride.address = owner_form.cleaned_data['address']
            ride.arrival = owner_form.cleaned_arrival()
            ride.num_pass = owner_form.cleaned_num_pass()
            ride.CanShare = owner_form.cleaned_data['CanShare']
            ride.status = 'open'
            ride.save()
            messages.success(request, f'Your request has been updated!')
            return redirect('rider-owner')

    else:
        owner_form = OwnerUpdateForm()

    context = {
        'title': 'Owner',
        'owner_form': owner_form,
    }

    return render(request, 'rider/owner.html', context)
    '''
    context = {
        'rides': Ride.objects.all()
    }
    return render(request, 'rider/owner.html', context)


def driver(request):
    return render(request, 'rider/driver.html', {'title': 'Driver'})


def driver_check(request):
    return render(request, 'rider/driver_check.html', {'title': 'Driver Check'})


class driver_filter(ListView):
    model = Ride
    template_name = 'rider/driver_filter.html'
    context_object_name = 'rides'
    ordering = ['-arrival']


class DriverDetailView(DetailView):
    model = Ride
    template_name = 'rider/driver_status.html'


class DriverConfirmView(DetailView):
    '''
    model = Ride
    fields = ['status']
    template_name = 'rider/driver_confirm.html'

    def form_valid(self, form):
        messages.success(self.request, "The request is confirmed!")
        return super().form_valid(form)

    def form_invalid(self, form):
        message.warning(self.request, "Request confirmation failed...")
        return super().form_invalid(form)
    '''
    def driver_confirm_view(request, pk):
        ride = get_object_or_404(Ride, pk=pk)

        if request.method == 'POST':
            ride.driver = request.user
            ride.status = 'confirmed'
            ride.save()

            subject = 'Your request has been confirmed!'
            message = 'Hi! I am happy to announce you that your ride request has been confirmed!\n' \
                      'Go to Rider App now to see the detail.'
            from_email = settings.EMAIL_HOST_USER
            to_list = [ride.owner.email]
            send_mail(subject, message, from_email, to_list)
            for s in ride.sharer.all():
                to_list_s = [s.email]
                send_mail(subject, message, from_email, to_list_s)

            messages.success(request, f'The request has been confirmed!')
            return redirect('rider-home')

        else:
            return render(request, 'rider/driver_confirm.html')


class DriverCompleteView(DetailView):
    def driver_complete_view(request, pk):
        ride = get_object_or_404(Ride, pk=pk)

        if request.method == 'POST':
            ride.status = 'completed'
            ride.save()
            messages.success(request, f'The request has been completed!')
            return redirect('rider-home')

        else:
            return render(request, 'rider/driver_complete.html')


'''
class DriverCompleteView(UpdateView):
    model = Ride
    fields = ['status']
    template_name = 'rider/driver_complete.html'
'''


class DriverOrderlistView(ListView):
    model = Ride
    template_name = 'rider/driver_orderlist.html'
    context_object_name = 'rides'
    ordering = ['-arrival']


def sharer(request):
    return render(request, 'rider/sharer.html', {'title': 'Sharer'})


class SharerSearchView(ListView):
    model = Ride
    template_name = 'rider/sharer_search.html'

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context['filter'] = SharerSearchFilter(self.request.GET, queryset=self.get_queryset())
        return context


class SharerJoinView(DetailView):
    model = Ride
    template_name = 'rider/sharer_join.html'


class SharerJoinNumView(DetailView):
    def sharer_join_num_view(request, pk):
        ride = get_object_or_404(Ride, pk=pk)

        if request.method == 'POST':
            ride.sharer.add(request.user)
            ride.num_pass += 1
            ride.save()
            messages.success(request, f'You have been added as a sharer!')
            return redirect('rider-home')

        else:
            return render(request, 'rider/sharer_joinNum.html')


class SharerListView(ListView):
    model = Ride
    template_name = 'rider/sharer_list.html'
    context_object_name = 'rides'
    ordering = ['-arrival']


class SharerDetailView(DetailView):
    model = Ride
    template_name = 'rider/sharer_status.html'


class SharerWithdrawView(DetailView):
    def sharer_withdraw_view(request, pk):
        ride = get_object_or_404(Ride, pk=pk)

        if request.method == 'POST':
            ride.num_pass -= 1
            ride.sharer.remove(request.user)
            ride.save()
            messages.success(request, f'You have withdrawn from the ride.')
            return redirect('rider-home')

        else:
            return render(request, 'rider/sharer_withdraw.html')
