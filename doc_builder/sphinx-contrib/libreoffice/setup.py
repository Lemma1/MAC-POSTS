# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

with open('README.rst') as file:
    long_description = file.read()

requires = ['Sphinx>=0.6', 'Pillow']

setup(
    name='sphinxcontrib-libreoffice',
    version='0.2',
    url='http://pypi.python.org/pypi/sphinxcontrib-libreoffice',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-libreoffice',
    license='BSD',
    author='Gerard Marull-Paretas',
    author_email='gerardmarull@gmail.com',
    description='Sphinx "libreoffice" extension',
    long_description=long_description,
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
