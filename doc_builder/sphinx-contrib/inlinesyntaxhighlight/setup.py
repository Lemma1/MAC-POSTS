# from aptk.__version__ import release
import sys, os

from setuptools import setup, find_packages

long_desc = '''
This package contains the Inline Syntax Highlight Sphinx Extension

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
    name='sphinxcontrib-inlinesyntaxhighlight',
    version='0.2',
    url='http://sphinxcontrib-inlinesyntaxhighlight.readthedocs.org',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-inlinesyntaxhighlight',
    license='BSD',
    author='Kay-Uwe (Kiwi) Lorenz',
    author_email='kiwi@franka.dyndns.org',
    description='Inline Syntax Highlighting for Sphinx',
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
