# -*- coding: utf-8 -*-

import os
import sys
import tempfile
from mock import Mock
from functools import wraps
from sphinx.application import Sphinx

testdir = os.path.dirname(__file__)


def trim_docstring(docstring):
    """ from PEP-257 <http://www.python.org/dev/peps/pep-0257/> """
    if not docstring:
        return ''

    # Convert tabs to spaces (following the normal Python rules)
    # and split into a list of lines:
    lines = docstring.expandtabs().splitlines()

    # Determine minimum indentation (first line doesn't count):
    indent = sys.maxsize
    for line in lines[1:]:
        stripped = line.lstrip()
        if stripped:
            indent = min(indent, len(line) - len(stripped))

    # Remove indentation (first line is special):
    trimmed = [lines[0].strip()]
    if indent < sys.maxsize:
        for line in lines[1:]:
            trimmed.append(line[indent:].rstrip())

    # Strip off trailing and leading blank lines:
    while trimmed and not trimmed[-1]:
        trimmed.pop()
    while trimmed and not trimmed[0]:
        trimmed.pop(0)

    # Return a single string:
    return '\n'.join(trimmed)


class FakeSphinx(Sphinx):
    """ Sphinx application class for testing """

    def __init__(self, srcdir=None, buildername='html', confoverrides={}):
        self.cleanup_dirs = []
        self.readonly = False

        # source settings
        if srcdir is None:
            srcdir = tempfile.mkdtemp()
            self.cleanup_dirs.append(srcdir)

            open(os.path.join(srcdir, 'conf.py'), 'w').close()
        else:
            self.readonly = True
            if not srcdir.startswith('/'):
                srcdir = os.path.join(testdir, srcdir)

        confdir = srcdir

        # _build/ directory setings
        self.builddir = tempfile.mkdtemp()
        outdir = os.path.join(self.builddir, str(buildername))
        doctreedir = os.path.join(self.builddir, 'doctrees')

        os.mkdir(outdir)
        self.cleanup_dirs.append(self.builddir)

        # misc settings
        status = sys.stdout
        warning = sys.stdout

        Sphinx.__init__(self, srcdir, confdir, outdir, doctreedir,
                        buildername, confoverrides, status, warning)

    def __del__(self):
        self.cleanup()

    def cleanup(self):
        import shutil
        for dir in self.cleanup_dirs:
            shutil.rmtree(dir, True)

    def parse_string(self, string):
        if self.readonly:
            raise RuntimeError()

        with open(os.path.join(self.srcdir, 'index.rst'), 'w') as fd:
            fd.write(trim_docstring(string))

        msg, length, iterator = self.env.update(self.config, self.srcdir,
                                                self.doctreedir, self)
        for _ in iterator:
            pass

        return self.env.get_doctree('index').children


def with_app(*sphinxargs, **sphinxkwargs):
    def testcase(func):
        @wraps(func)
        def decorator(*args, **kwargs):
            app = None
            try:
                app = FakeSphinx(*sphinxargs, **sphinxkwargs)
                func(*(args + (app,)), **kwargs)
            finally:
                if app:
                    app.cleanup()
        return decorator

    return testcase


def with_built_docstring(**sphinxargs):
    def testcase(func):
        @wraps(func)
        @with_app(srcdir=None, **sphinxargs)
        def decorator(*args):
            app = args[-1]
            index_rst = os.path.join(app.srcdir, 'index.rst')
            with open(index_rst, 'wt') as fd:
                fd.write('heading\n')
                fd.write('=======\n')
                fd.write(trim_docstring(func.__doc__))
            app.builder.warn = Mock()
            app.builder.build_all()
            func(*args)

        return decorator
    return testcase


def with_parsed(*sphinxargs, **sphinxkwargs):
    def testcase(func):
        @wraps(func)
        def decorator(*args, **kwargs):
            app = None
            try:
                app = FakeSphinx(*sphinxargs, **sphinxkwargs)
                nodes = app.parse_string(func.__doc__)
                func(*(args + (nodes,)), **kwargs)
            finally:
                app.cleanup()
        return decorator

    return testcase
