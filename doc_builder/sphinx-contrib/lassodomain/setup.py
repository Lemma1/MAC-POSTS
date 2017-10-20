# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

long_desc = '''
This package contains the lassodomain Sphinx extension, which adds
support for the Lasso language to Sphinx.
'''

requires = ['Sphinx>=1.4']

setup(
    name='sphinxcontrib-lassodomain',
    version='0.4',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-lassodomain',
    license='BSD',
    author='Eric Knibbe',
    author_email='eric at lassosoft dotcom',
    description='Lasso domain for Sphinx',
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
        'Topic :: Documentation :: Sphinx',
        'Topic :: Utilities',
    ],
    platforms='any',
    packages=find_packages(),
    include_package_data=True,
    install_requires=requires,
    namespace_packages=['sphinxcontrib'],
)
