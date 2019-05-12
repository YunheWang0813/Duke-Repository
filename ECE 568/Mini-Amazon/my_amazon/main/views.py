from django.shortcuts import render, redirect, get_object_or_404
from django.http import HttpResponse
from .models import Item, Order, Fill
from .forms import OrderCreationForm, ItemFillForm
from django.contrib.auth.decorators import login_required
from django.contrib import messages
from django.core.mail import send_mail


def home(request):
    context = {
        'items': Item.objects.all().order_by('name'),
    }
    return render(request, 'main/home.html', context)


@login_required()
def order(request):
    if request.method == 'POST':
        form = OrderCreationForm(request.POST)

        if form.is_valid():
            curr_item = form.cleaned_data['item_id']
            curr_quant = form.cleaned_data['bought_quantity']
            ori = Item.objects.filter(name=curr_item).first()
            if ori.quantity < curr_quant:
                messages.error(request, f'No enough storage, go fill it!')
                return redirect('order-create')

            # ori.quantity -= curr_quant
            ori.save()
            model_form = form.save(commit=False)
            model_form.user_id = request.user
            model_form.save()
            messages.success(request, f'New order created!')
            send_mail(
                'Your order has been created!',
                'Your order has been created. Go to Amazon site to check shipping status of the package, or go to UPS site to track the package!',
                'yw314@duke.edu',
                [request.user.email],
                fail_silently=False)
            return redirect('main-status')

    else:
        form = OrderCreationForm()

    context = {
        'form': form,
        'title': Order,
        'items': Item.objects.all().order_by('name'),
    }

    return render(request, 'main/order_create.html', context)


@login_required()
def fill(request):
    if request.method == 'POST':
        form = ItemFillForm(request.POST)

        if form.is_valid():
            curr_name = form.cleaned_data['name']
            curr_quant = form.cleaned_data['quantity']
            ori = Item.objects.filter(name=curr_name).first()
            if ori:
                #ori.quantity += curr_quant
                #ori.save()
                messages.success(request, f'The item fill request is sent!')
                fill_obj = Fill(user_id=request.user, item_id=ori, fill_quantity=curr_quant)
                fill_obj.save()
                return redirect('main-home')
            else:
                form.save()
                messages.success(request, f'New item is added and will be filled!')
                fill_obj = Fill(user_id=request.user, item_id=Item.objects.filter(name=curr_name).first(), fill_quantity=curr_quant)
                fill_obj.save()

                instance = Item.objects.filter(name=curr_name).first()
                instance.quantity = 0
                instance.save()

                return redirect('main-home')

    else:
        form = ItemFillForm()

    context = {
        'form': form,
        'title': 'Fill',
        'items': Item.objects.all().order_by('name'),
    }

    return render(request, 'main/fill.html', context)


@login_required()
def status(request):
    context = {
        'items': Item.objects.all().order_by('name'),
        'title': 'Status(Order)',
        'orders': Order.objects.all().order_by('-time_created'),
    }
    return render(request, 'main/status.html', context)


@login_required()
def status2(request):
    context = {
        'items': Item.objects.all().order_by('name'),
        'title': 'Status(Fill)',
        'fills': Fill.objects.all().order_by('-time_created'),
    }
    return render(request, 'main/status2.html', context)