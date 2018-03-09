# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

long_desc = '''
This package contains the `Google Chart`_ Sphinx extension.

.. _Google Chart: http://code.google.com/intl/ja/apis/chart/

This extension enable you to insert charts and graphs in your Sphinx document.
Following code is sample::

   .. piechart::

      dog: 100
      cat: 80
      rabbit: 40


See more examples and output images in http://packages.python.org/sphinxcontrib-googlechart/ .

This module needs internet connection.

'''

requires = ['Sphinx>=0.6', 'funcparserlib', 'setuptools']

setup(
    name='sphinxcontrib-googlechart',
    version='0.2.1',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-googlechart',
    license='BSD',
    author='Takeshi KOMIYA',
    author_email='i.tkomiya@gmail.com',
    description='Sphinx "googlechart" extension',
    long_description=long_desc,
    zip_safe=False,
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Environment :: Console',
        'Environment :: Web Environment',
        'Framework :: Sphinx :: Extension',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.2',
        'Programming Language :: Python :: 3.3',
        'Topic :: Documentation',
        'Topic :: Documentation :: Sphinx',
        'Topic :: Utilities',
    ],
    platforms='any',
    packages=find_packages(),
    include_package_data=True,
    install_requires=requires,
    namespace_packages=['sphinxcontrib'],
)
