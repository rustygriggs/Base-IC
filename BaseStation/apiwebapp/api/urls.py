from django.conf.urls import url
from .views import PeripheralView, PeripheralDetailsView, PeripheralActionView

from . import views

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^peripherals/$', PeripheralView.as_view(), name='create'),
    url(r'^peripherals/actions$', PeripheralActionView.as_view(), name='actions'),
    url(r'^peripherals/(?P<queue>[0-9a-zA-Z]+)/(?P<address>[0-9a-zA-Z]+)/$', PeripheralDetailsView.as_view(),
        name='details'),
]
