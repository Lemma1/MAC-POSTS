# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

with open('VERSION', 'r') as version_file:
    version = version_file.read().rstrip()

with open('README.rst', 'r') as readme_file:
    long_desc = readme_file.read()

requires = ['Sphinx >= 1.0']

setup(
    name='sphinxcontrib-cheader',
    version=version,
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-cheader',
    license='Public Domain',
    author='Arto Bendiken',
    author_email='arto@bendiken.net',
    description='Sphinx c:header directive',
    long_description=long_desc,
    zip_safe=False,
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Environment :: Console',
        'Environment :: Web Environment',
        'Intended Audience :: Developers',
        'License :: Public Domain',
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
