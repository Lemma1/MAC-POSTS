
DocumentedList Sphinx Extension
===============================

.. attention::
        This extention has been moved to github as if version 0.4. 
        See http://github.com/chintal/sphinxcontrib-documentedlist

This file provides a Sphinx extension to convert a Python list into
a table in the generated documentation. The intended application of
this extension is to document the items of essentially list-like
objects of immutable data (possibly enums, though python 3.4 enums
are not supported yet).

In the source code, each list item, instead of being just it's native
data type, should be replaced by a tuple of two elements. In the
simpest application, the second element of the tuple should be a string
which provides a description for the item.

The following options are also included to enable slightly more complex
use cases :

:header: The number of columns displayed is 2 by default, with titles
    "Item" and "Description". This option allows you to add a custom
    header and change the number of columns in the table::

        .. documentedlist::
            :listobject: some.list.object
            :header: "First Name" "Last Name" Email


:spantolast: If this flag is present, the last column of any row with
    an insufficient number of columns will spread to span all remaining
    columns. This would typically be used to insert headings within
    the table::

        .. documentedlist::
            :listobject: some.list.object
            :spantolast:


:descend: This flag allows you to construct a relatively complex table.
    with subsections. Any row containing a list as one of it's cells is
    expanded into a sub-table (specifically, a docutils tgroup). The
    list is popped from the original row, and each element of the list
    is turned into a row::

        .. documentedlist::
            :listobject: some.list.object
            :descend:


In the first use of this extension, the aim was to document a list of
supported device classes (each of which is a string). This was
originally specified in the python code as a list, and the same list
with descriptions was manually maintained (or, in reality, left
unmaintained) in the corresponding documentation. The list is now
replaced with a tuple containing both the recognized strings and the
description for each, and Sphinx is able to use this extension to
'autogenerate' the table in the documentation.

Based heavily on slides from Doug Hellman's PyCon talk,
http://www.slideshare.net/doughellmann/better-documentation-through-automation-creating-docutils-sphinx-extensions

Installation & Usage
--------------------

This extension is part of the sphinxcontrib namespace and can be
installed from pypi :

    .. code-block:: bash

        pip install sphinxcontrib-documentedlist

In the .rst file where the table should appear, insert the Sphinx
directive provided by this module :

    .. code-block:: rest

        .. documentedlist::
            :listobject: full.importable.path.of.thelist

This extension will import the list as :

    .. code-block:: python

        from full.importable.path.of import thelist

For a basic usage example, see:

:Python: https://github.com/chintal/tendril/blob/public/tendril/conventions/electronics.py#L28
:Generated: https://github.com/chintal/tendril/blob/public/doc/userdoc/conventions/gedasymbols.rst#symbol-attributes

For a more complex example of the extension's usage, including the
:header:, :spantolast:, and :descend: options, see:

:Python: https://github.com/chintal/tendril/blob/public/tendril/utils/config.py#L625
:ReST: https://github.com/chintal/tendril/blob/public/tendril/utils/config.py#L31
:Generated: http://tendril.chintal.in/doc/apidoc/tendril.utils.config/#configuration-options


License
-------

This Sphinx Extension is made available under the BSD 2-clause License. See
sphinxcontrib's LICENSE file for the full text.
