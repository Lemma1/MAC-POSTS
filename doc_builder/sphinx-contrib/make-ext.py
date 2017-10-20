#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Helper script to create a new Sphinx extension in the sphinx-contrib
project.

"""
from __future__ import print_function, with_statement

import argparse
import os
import re
import shutil
import string
import sys


NOT_ALNUM_RE = re.compile(r'[^\w-]')

TEMPLATE_DIRNAME = '_template'
TEMPLATE_FILES   = ('setup.py', 'README.rst')


def make_pkg(pkgname, context):
    """Create a new extension package.

    :param pkgname: Name of the package to create.

    :param context: Mapping with keys that match the placeholders in the
                    templates.

    :return: True if package creation succeeded or a tuple with False and an
             error message in case the creation failed.

    :rtype: Bool or Tuple

    """
    try:
        shutil.copytree(TEMPLATE_DIRNAME, pkgname)
    except (OSError, IOError, shutil.Error) as e:
        return False, e.strerror

    for f in TEMPLATE_FILES:
        try:
            write_template(pkgname, f, context)
        except (OSError, IOError) as e:
            return False, e.strerror
    return True


def write_template(root, filename, context):
    """Write template file in given root directory.

    :param root: Path to root folder where to put resulting file.

    :param filename: Name of the template file relative to ``root``.

    :param context: Mapping with keys that match the placeholders in the
                    templates.

    """
    infile  = os.path.join(TEMPLATE_DIRNAME, filename)
    outfile = os.path.join(root, filename)
    with open(infile, 'r') as in_fp, open(outfile, 'w') as out_fp:
        out_fp.write(string.Template(in_fp.read()).safe_substitute(context))


def get_argparser():
    """Generate argument parser for the script."""
    parser = argparse.ArgumentParser(
        description='Utility script to create a new sphinx-contrib package.'
    )

    parser.add_argument('pkgname', metavar='PKGNAME',nargs='+',
                        help="name of the package to create")
    parser.add_argument('-a', '--author',
                        help="author name for the package")
    parser.add_argument('-e', '--email',
                        help="author's email address")

    return parser


if __name__ == '__main__':
    if not os.path.isdir(TEMPLATE_DIRNAME):
        sys.exit("Run this script from sphinx-contrib root directory.")

    # Parsing arguments.
    opts = get_argparser().parse_args(sys.argv[1:])
    if not opts.author:
        opts.author = raw_input("Author name: ")
    if not opts.email:
        opts.email = raw_input("E-mail: ")

    # Creating each given packages.
    # Exit value is the number of failed package creation.
    failed = 0
    for name in opts.pkgname:
        context = {
            'author': opts.author,
            'author_email': opts.email,
            'name': name,
        }

        name_out = "%s: " % name
        if NOT_ALNUM_RE.search(name):
            sys.stderr.write(
                name_out + "IGNORE, non-alphanumeric package name!\n"
            )
            continue

        try:
            success, msg = make_pkg(name, context)
        except TypeError:
            sys.stdout.write(name_out + "OK\n")
            continue
        sys.stderr.write(name_out + "FAIL, %s!\n"%msg)
        failed += 1

    sys.exit(failed)
