# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

long_desc = '''
This package contains the swf Sphinx extension.

It adds a directive `swf` for improved flash file handling.

You can pass any parameter mentioned on `flash attribute listing`_.

http://helpx.adobe.com/flash/kb/flash-object-embed-tag-attributes.html

I stumbled across the issue, that screencasts often are "zoomed in", such
that you do not have access to controls. So this directive has a
"zoom-to-fit" option, which zooms out the video using javascript.
'''

requires = ['Sphinx>=0.6']

setup(
    name='sphinxcontrib-swf',
    version='0.3',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-swf',
    license='BSD',
    author='Kay-Uwe (Kiwi) Lorenz',
    author_email='kiwi@franka.dyndns.org',
    description='Sphinx "swf" extension',
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
