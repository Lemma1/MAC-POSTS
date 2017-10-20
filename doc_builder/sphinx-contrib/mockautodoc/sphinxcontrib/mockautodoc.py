# -*- coding: utf-8 -*-
import os
import re
import sys
import imp
import copy
from docutils.parsers.rst import directives
from sphinx.ext.autodoc import ModuleDocumenter#, ClassDocumenter

# TODO rewrite imports extract 

import_re = re.compile(r'^(from|import) ([\.\w]+)', re.M)
mock_types_re = re.compile('class[\s\w]+\(([\.\, \w]+)\)', re.M)


def find_dotted_module(name, path=None):
    res = None
    for x in name.split('.'):
        res = imp.find_module(x, path)
        path = [res[1]]
    if os.path.isdir(res[1]): # package
        # TODO pyc ?
        return res[0], os.path.join(res[1], '__init__.py'), res[2]
    return res


class Mock(object):
    TYPES = []

    def __getattribute__(self, name):
        if name in Mock.TYPES:
            return type(self)
        return Mock()

    def __call__(self, *args, **kwargs):
        if args and callable(args[0]): # called as decorator
            return args[0]
        return Mock()\




class MockedModuleDocumenter(ModuleDocumenter):
    option_spec = copy.deepcopy(ModuleDocumenter.option_spec)
    option_spec.update({
        'mockimport': directives.unchanged,
        'mocktype': directives.unchanged,
    })
 
    def _to_mock_imports(self, module):
        names = module.strip().split('.')
        for k in range(1, len(names) + 1):
            self.imports.add('.'.join(names[0:k])) 

    def _mock_imports(self):
        self.env.app.info('by <.. automodule:: %s> directive mocked: %s %s' \
            % (self.modname, self.imports, self.mock_types))
        Mock.TYPES = self.mock_types
        for name in self.imports:
            if name:
                sys.modules[name] = Mock()

    def import_object(self):

        def _mock_list(name):
            res = list(self.env.config.mockautodoc.get(name, [])) 
            if name in self.options and self.options[name]:
                res += self.options[name].split(',')
            return res

        if 'mockimport' in self.options or 'mocktype' in self.options:

            self.imports = set(map(lambda i: i.strip(), _mock_list('mockimport')))
            self.mock_types = set(map(lambda i: i.strip().split('.')[-1], _mock_list('mocktype')))

            # find module source file
            src_path = find_dotted_module(self.modname)[1]

            # extract all imports
            with open(src_path, 'r') as src:
                content = src.read()
                for i in import_re.findall(content):
                    for t in self.imports:
                        mod = i[1]
                        if t in mod:
                            self._to_mock_imports(mod)
                            break

                for bases in mock_types_re.findall(content):
                    for base in bases.split(','):
                        mod = base.strip().split('.')[-1]
                        self.mock_types.add(mod)

            # mock
            self._mock_imports()

        return super(MockedModuleDocumenter, self).import_object()

    def generate(self, more_content=None, real_modname=None,
                 check_module=False, all_members=False):
        try:
            return super(MockedModuleDocumenter, self).generate(more_content,
                real_modname, check_module, all_members)
        finally:
            # TODO it will be unmocked?
            # unmock
            for name in getattr(self, 'imports', []):
                if name in sys.modules:
                    del sys.modules[name]


def setup(app):
    app.add_autodocumenter(MockedModuleDocumenter)
    app.add_config_value('mockautodoc', {}, False)
