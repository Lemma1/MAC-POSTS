# -*- coding: utf-8 -*-
import os
from setuptools import setup, find_packages

with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'README.rst')) as stream:
  long_desc = stream.read()

requires = ['Sphinx>=0.6', 'xlrd', 'future', 'docutils']

VERSION = '0.2.2'

setup(
  name = 'sphinxcontrib-exceltable',
  version = VERSION,
  url = 'http://packages.python.org/sphinxcontrib-exceltable',
  download_url = 'http://pypi.python.org/pypi/sphinxcontrib-exceltable',
  license = 'BSD',
  author = 'Juha Mustonen',
  author_email = 'juha.p.mustonen@gmail.com',
  description = 'Support for including Excel spreadsheets into Sphinx documents',
  long_description = long_desc,
  zip_safe = False,
  classifiers = [
    'Development Status :: 4 - Beta',
    'Environment :: Console',
    'Environment :: Web Environment',
    'Intended Audience :: Developers',
    'License :: OSI Approved :: BSD License',
    'Operating System :: OS Independent',
    'Programming Language :: Python',
    'Programming Language :: Python :: 2',
    'Programming Language :: Python :: 3',
    'Topic :: Documentation',
    'Topic :: Utilities',
  ],
  platforms = 'any',
  packages = find_packages(),
  include_package_data = True,
  install_requires = requires,
  namespace_packages = ['sphinxcontrib'],
  test_suite = 'nose.collector'
)
