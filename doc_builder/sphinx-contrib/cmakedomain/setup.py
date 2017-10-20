# from aptk.__version__ import release
import sys, os

from setuptools import setup, find_packages

long_desc = '''
This package contains a CMake domain.
'''

sys.path.insert(0, 'sphinxcontrib')
import domaintools

requires = ['Sphinx>=1.0', 'sphinxcontrib-domaintools>=0.1']

setup(
    name='sphinxcontrib-cmakedomain',
    version=domaintools.__version__,
    url='http://bitbucket.org/klorenz/sphinxcontrib-cmakedomain',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-cmakedomain',
    license='BSD',
    author='Kay-Uwe (Kiwi) Lorenz',
    author_email='kiwi@franka.dyndns.org',
    description='Sphinx extension for CMake domain.',
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
