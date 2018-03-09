# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

long_desc = '''
This package contains the examplecode Sphinx extension.

This extension adds support for a multiple language code block
widget to Sphinx.
'''

requires = ['Sphinx>=1.0']

setup(
    name='sphinxcontrib-examplecode',
    version='0.1.0',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-examplecode',
    license='BSD',
    author='Serge Domkowski',
    author_email='sergedomk@gmail.com',
    description='Sphinx "examplecode" extension',
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
        'Topic :: Documentation',
        'Topic :: Utilities',
    ],
    platforms='any',
    packages=find_packages(),
    include_package_data=True,
    install_requires=requires,
    namespace_packages=['sphinxcontrib'],
)
