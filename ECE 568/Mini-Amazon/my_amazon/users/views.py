from django.shortcuts import render, redirect
from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.decorators import login_required
from django.contrib import messages
from .forms import UserRegisterForm, UserUpdateForm, ProfileUpdateForm
from django.contrib.auth.models import User
from .models import Profile
from django.core.mail import send_mail


def register(request):
    if request.method == 'POST':
        form = UserRegisterForm(request.POST)
        if form.is_valid():
            form.save()
            username = form.cleaned_data.get('username')
            email = form.cleaned_data.get('email')
            messages.success(request, f'Account of {username} created successfully! Now you can log in')
            send_mail(
                'Your Amazon account has been created!',
                'Your account has been created. Go shopping at Amazon!',
                'yw314@duke.edu',
                [email],
                fail_silently=False)
            return redirect('login')
    else:
        form = UserRegisterForm()
    return render(request, 'users/register.html', {'form': form})


@login_required
def profile(request):
    if request.method == 'POST':
        u_form = UserUpdateForm(request.POST, instance=request.user)
        p_form = ProfileUpdateForm(request.POST, request.FILES,
                                   instance=request.user.profile)
        if u_form.is_valid() and p_form.is_valid():
            u_form.save()
            p_form.save()
            messages.success(request, f'Your account has been updated!')
            return redirect('profile')

    else:
        u_form = UserUpdateForm(instance=request.user)
        p_form = ProfileUpdateForm(instance=request.user.profile)

    context = {
        'u_form': u_form,
        'p_form': p_form,
    }

    return render(request, 'users/profile.html', context)


@login_required
def beprime(request):
    if request.method == 'POST':
        instance = Profile.objects.filter(user_id_id=request.user).first()
        print(instance.is_prime)
        if instance.is_prime is False:
            instance.is_prime = True
        else:
            instance.is_prime = False
        instance.save()

    context = {
        'profiles': Profile.objects.filter(user_id_id=request.user).first(),
        'title': 'Be Prime',
    }

    return render(request, 'users/beprime.html', context)
