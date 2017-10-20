# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

long_desc = '''
This package contains the imagesvg Sphinx extension.

The extension defines a directive, "imagesvg", for embedding an svg image
in a specified HTML tag.
'''

requires = ['Sphinx>=0.6', 'setuptools']

setup(
    name='sphinxcontrib-imagesvg',
    version='0.1',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-imagesvg',
    license='BSD',
    author='Theo J.A. de Vries',
    author_email='t.j.a.devries@gmail.com',
    description='Sphinx "imagesvg" extension',
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
        'Framework :: Sphinx :: Extension',
        'Topic :: Documentation',
        'Topic :: Utilities',
    ],
    platforms='any',
    packages=find_packages(),
    include_package_data=True,
    install_requires=requires,
    namespace_packages=['sphinxcontrib'],
)
