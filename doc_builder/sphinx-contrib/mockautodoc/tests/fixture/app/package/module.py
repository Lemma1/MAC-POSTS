# -*- coding: utf-8 -*-
"Some module docs"
import gevent
import twisted as tw
from sqlalchemy import Table, Column, Integer, Unicode, ForeignKey


class SomeModuleClass(Table.a.b.c):
    "Some class docs"

    def __init__(self, *args, **kwargs):
        pass


@gevent.patch
def somefunc():
    "Some func docs"
    return True
