 # -*- coding: utf-8 -*-
"Some package/module docs"
import not_existed


class PackageSomeModuleClass(not_existed.Base.BaseModuleClass):
    "Some package/class docs"

    def __init__(self, *args, **kwargs):
        pass


@not_existed.decorate
def package_somefunc():
    "Some package/func docs"
    return True
