# -*- coding: utf-8 -*-
import os
import tempfile
import shutil
from sphinx.application import Sphinx


_fixturedir = os.path.join(os.path.dirname(__file__), 'fixture')

_tempdir = _srcdir = _outdir = None


def setup():
    global _tempdir, _srcdir, _outdir
    _tempdir = tempfile.mkdtemp()
    # _srcdir = os.path.join(_tempdir, 'src')
    _srcdir = os.path.join(os.path.dirname(__file__), 'fixture')
    _outdir = os.path.join(_tempdir, 'out')
    # os.mkdir(_srcdir)


def teardown():
    shutil.rmtree(_tempdir)


def readfile(fname):
    with open(os.path.join(_outdir, fname), 'r') as f:
        return f.read()


def runsphinx(text, builder, confoverrides):
    f = open(os.path.join(_srcdir, 'index.rst'), 'w')
    try:
        f.write(text)
    finally:
        f.close()
    app = Sphinx(_srcdir, _fixturedir, _outdir, _outdir, builder,
                 confoverrides)
    app.build()


def with_runsphinx(builder, confoverrides=None):
    if confoverrides is None:
        confoverrides = {
        }

    def wrapfunc(func):
        def test():
            src = '\n'.join(l[4:] for l in func.__doc__.splitlines()[2:])
            os.mkdir(_outdir)
            try:
                runsphinx(src, builder, confoverrides)
                func()
            finally:
                # os.unlink(os.path.join(_srcdir, 'index.rst'))
                shutil.rmtree(_outdir)
        test.__name__ = func.__name__
        return test
    return wrapfunc


@with_runsphinx('html')
def test_buildhtml_simple():
    """Generate simple

    .. autoanysrc:: services
        :src: app/*.js
        :analyzer: js
    """
    content = readfile('index.html')
    # print(content)

    # check head
    assert 'Services' in content

    # check link to function
    assert 'someService()' in content

    # check param desc
    assert 'An URI to the location' in content


@with_runsphinx('html', confoverrides={
    'autoanysrc_analyzers': {
        'js-custom': 'conf.JSCustomAnalyzer',
    },
})
def test_custom_analyzer():
    """Generate simple

    .. autoanysrc:: services
        :src: app/*.js
        :analyzer: js-custom
    """
    content = readfile('index.html')

    # check head
    assert 'From custom analyzer' in content
