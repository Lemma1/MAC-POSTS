# -*- coding: utf-8 -*-
import os
import shutil
import tempfile
from sphinx.application import Sphinx

_fixturedir = os.path.join(os.path.dirname(__file__), 'fixture')
_fakecmd = os.path.join(os.path.dirname(__file__), 'fakecmd.py')

_tempdir = _srcdir = _outdir = None


def setup():
    global _tempdir, _srcdir, _outdir
    _tempdir = tempfile.mkdtemp()
    _srcdir = os.path.join(_tempdir, 'src')
    _outdir = os.path.join(_tempdir, 'out')
    os.mkdir(_srcdir)


def teardown():
    shutil.rmtree(_tempdir)


def readfile(fname):
    with open(os.path.join(_outdir, fname), 'r') as f:
        return f.read()


def runsphinx(text, builder, confoverrides, config=None):
    f = open(os.path.join(_srcdir, 'index.rst'), 'w')
    try:
        f.write(text)
    finally:
        f.close()
    app = Sphinx(_srcdir, _fixturedir, _outdir, _outdir, builder,
                 confoverrides)
    if config:
        app.config.update(config)
    app.build()


def with_runsphinx(builder, confoverrides=None):
    def wrapfunc(func):
        def test():
            src = '\n'.join(l[4:] for l in func.__doc__.splitlines()[2:])
            os.mkdir(_outdir)
            try:
                runsphinx(src, builder, confoverrides)
                func()
            finally:
                os.unlink(os.path.join(_srcdir, 'index.rst'))
                shutil.rmtree(_outdir)
        test.__name__ = func.__name__
        return test
    return wrapfunc


@with_runsphinx('html')
def test_buildhtml_simple():
    """Generate simple HTML

    .. automodule:: package.module
        :members:
        :mockimport: not_existed, gevent, twisted, sqlalchemy
        :mocktype: BaseModuleClass
 
    .. automodule:: package
        :members:
        :mockimport: not_existed
        :mocktype: BaseModuleClass
    """
    content = readfile('index.html')
    assert 'Some func docs' in content
    assert 'somefunc' in content

    assert 'Some class docs' in content
    assert 'SomeModuleClass' in content

    assert 'Some package/func docs' in content
    assert 'package_somefunc' in content

    assert 'Some package/class docs' in content
    assert 'PackageSomeModuleClass' in content 
