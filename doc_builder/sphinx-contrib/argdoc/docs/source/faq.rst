Frequently-asked questions
==========================

Is there, or will there be, support for :mod:`optparse`?
    Because :mod:`optparse` was deprecated in Python 2.7, we have no plans
    to support it at present. If you'd still like to use :obj:`sphinxcontrib.argdoc`,
    see the Python documentation on
    `upgrading optparse code <http://docs.python.org/2.7/library/argparse.html#upgrading-optparse-codeimport warnings>`_.

My :term:`main-like function` is not named `main`. Can I use :obj:`sphinxcontrib.argdoc` without changing the function name?
    Absolutely. Just set `argdoc_main_func` to the name of your function
    (as a string) in ``conf.py``. See :ref:`other-main-name` for instructions.

My :term:`executable scripts` use alternate `prefix characters`_.
    Can :data:`sphinxcontrib.argdoc` handle these? Yes. See :ref:`alt-prefix-chars` for details.

Does :data:`sphinxcontrib.argdoc` support subcommands?
    Yes. You don't need to do anything special. :data:`sphinxcontrib.argdoc`
    will find these all by itself.

Does :data:`sphinxcontrib.argdoc` support nested subcommands?
    Yes. See the :doc:`examples`.
