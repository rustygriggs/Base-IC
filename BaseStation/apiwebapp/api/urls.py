from django.conf.urls import url
from .views import PeripheralView, PeripheralServiceView, ProcessView, RecipeView

from . import views

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^process/(?P<queue>[0-9a-zA-Z]+)$', ProcessView.as_view(), name='process'),
    url(r'^peripherals/$', PeripheralView.as_view(), name='peripheral'),
    url(r'^peripherals/services/(?P<id>[0-9]+)$', PeripheralServiceView.as_view(), name='peripheral_service'),
    url(r'^recipes/$', RecipeView.as_view(), name='recipe'),
    url(r'^recipes/(?P<id>[0-9]+)$', RecipeView.as_view(), name='recipe'),
]
