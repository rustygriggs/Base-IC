from django.conf.urls import url
from .views import PeripheralView, ProcessView

from . import views

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^process/(?P<queue>[0-9a-zA-Z]+)$', ProcessView.as_view(), name='process'),
    url(r'^peripherals/$', PeripheralView.as_view(), name='create'),
]
