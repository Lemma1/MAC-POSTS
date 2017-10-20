# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

long_desc = '''
This package contains the zopeext Sphinx extension.

Documentation: http://packages.python.org/sphinxcontrib-zopeext

Install with ``pip install sphinxcontrib-zopeext``.

To use this extension, include `'sphinxcontrib.zopeext.autointerface'` in your
`extensions` list in the `conf.py` file for your documentation.

This provides some support for Zope interfaces by providing an `autointerface`
directive that acts like `autoclass` except uses the Zope interface methods for
attribute and method lookup (the interface mechanism hides the attributes and
method so the usual `autoclass` directive fails.)  Interfaces are intended
to be very different beasts than regular python classes, and as a result require
customized access to documentation, signatures etc.

See Also
--------
* http://sphinx.pocoo.org/
* http://sphinx.pocoo.org/ext/autodoc.html
* http://docs.zope.org/zope.interface/README.html
* http://packages.python.org/sphinxcontrib-zopeext/

'''

requires = ['Sphinx>=0.6', 'zope.interface']

import sphinxcontrib.zopeext

setup(
    name='sphinxcontrib-zopeext',
    version=sphinxcontrib.zopeext.__version__,
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-zopeext',
    license='BSD',
    author='Michael McNeil Forbes',
    author_email='michael.forbes+pypim@gmail.com',
    description='Sphinx extension zopeext',
    long_description=long_desc,
    zip_safe=False,
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Console',
        'Environment :: Web Environment',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Framework :: Zope3',
        'Topic :: Documentation',
        'Topic :: Utilities',
    ],
    platforms='any',
    packages=find_packages(),
    include_package_data=True,
    install_requires=requires,
    namespace_packages=['sphinxcontrib'],
)
