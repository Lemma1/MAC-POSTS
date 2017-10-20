# -*- coding: utf-8 -*-
"""
    makedomain
    ~~~~~~~~~~

    A GNU Make domain.

    This domain provides `make:target`, `make:var` and `make:expvar`
    directives and roles.

    :copyright: 2012 by Kay-Uwe (Kiwi) Lorenz, ModuleWorks GmbH
    :license: BSD, see LICENSE for details.
"""

__version__ = "0.1.1"
# for this module's sphinx doc 
release = __version__
version = release.rsplit('.', 1)[0]

from sphinxcontrib.domaintools import custom_domain
import re

def setup(app):
    app.add_domain(custom_domain('GnuMakeDomain',
        name  = 'make',
        label = "GNU Make", 

        elements = dict(
            target = dict(
                objname      = "Make Target",
                indextemplate = "pair: %s; Make Target",
            ),
            var   = dict(
                objname = "Make Variable",
                indextemplate = "pair: %s; Make Variable"
            ),
            expvar = dict(
                objname = "Make Variable, exported",
                indextemplate = "pair: %s; Make Variable, exported"
            )
        )))

# vim: ts=4 : sw=4 : et
