# -*- coding: utf-8 -*-
from setuptools import setup, find_packages


LONG_DESC = """A Sphinx extension providing a domain to document CFEngine 3
policies.

CFEngine is a configuration management and automation framework to manage IT
infrastructures.

"""

requires = ['Sphinx>=1.0']

setup(
    name='sphinxcontrib-cf3domain',
    version='0.1',
    license='BSD',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-cf3domain',

    author='Jimmy Thrasibule',
    author_email='jimmy.thrasibule@gmail.com',
    description='Sphinx domain for CFEngine 3 policies',
    long_description=LONG_DESC,
    keywords='sphinx cfengine',

    classifiers=[
        'Development Status :: 3 - Alpha',
        'Environment :: Console',
        'Intended Audience :: Developers',
        'Intended Audience :: Information Technology',
        'Intended Audience :: System Administrators',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Topic :: Documentation',
        'Topic :: Documentation :: Sphinx',
        'Topic :: Text Processing :: Markup',
        'Topic :: Utilities',
    ],

    zip_safe=False,
    platforms='any',
    packages=find_packages(),
    include_package_data=True,
    install_requires=requires,
    namespace_packages=['sphinxcontrib'],
)
