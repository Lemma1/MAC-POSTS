# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

long_desc = '''
This package contains the py_directive Sphinx extension.

py_directive adds a directive named ``py`` which supports executing python code
and returning math.  
'''

requires = ['Sphinx>=0.6']

setup(
    name='sphinxcontrib-py_directive',
    version='dev',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-py_directive',
    license='BSD',
    author='Bill Purcell',
    author_email='william@whpiv.net',
    description='Sphinx "py_directive" extension',
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
