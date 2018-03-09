# from aptk.__version__ import release
import sys, os

from setuptools import setup, find_packages

long_desc = '''
This package contains the FindAnything Sphinx extension.

This enables Sublime Text 2 findanything-like searching for 
section names, page titles and index entries.  This enables you
to find any entries increadible fast.  Since I have this feature
at my documentations, I do not use full-text-search anymore.
'''

#if sys.argv[1] == 'test':
#    sys.path.insert(0, os.path.dirname(__file__))
#    import aptk
#    r = aptk._test(aptk)
#    failed, attempted = r
#    if not failed:
#        sys.stdout.write("%s tests ok\n"%attempted)

#else:    

requires = ['Sphinx>=0.6']

setup(
    name='sphinxcontrib-findanything',
    version='0.2',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-findanything',
    license='BSD',
    author='Kay-Uwe (Kiwi) Lorenz',
    author_email='kiwi@franka.dyndns.org',
    description='Sphinx "FindAnything" extension',
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
