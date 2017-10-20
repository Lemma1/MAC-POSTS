# from aptk.__version__ import release
import sys, os

from setuptools import setup, find_packages

long_desc = '''
This package contains a Make Domain for Sphinx.

This adds make:target, make:var and make:expvar directives and roles.
'''

requires = ['Sphinx>=0.6', 'sphinxcontrib-domaintools>=0.1']

setup(
    name='sphinxcontrib-makedomain',
    version='0.1.1',
    url='http://bitbucket.org/klorenz/sphinxcontrib-makedomain',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-makedomain',
    license='BSD',
    author='Kay-Uwe (Kiwi) Lorenz',
    author_email='kiwi@franka.dyndns.org',
    description='Sphinx Make Domain Extension',
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
