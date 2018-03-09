autoanysrc
==========

.. attention::

    Currently in early development stage

Extension for gathering reST documentation from any files.
This is a documenter_ from ext.autodoc.

In current state this extension will only insert reST docs from files to
target documentation project without auto generation definitions
and signatures.

But it simple and clean to make documentation for API and store documentation
strings in the source code.

Install
-------

::

    pip install sphinxcontrib-autoanysrc


Usage
-----

Add autoanysrc to extensions list::

    extensions = ['sphinxcontrib.autoanysrc', ]

Example of usage::

    .. autoanysrc:: blabla
        :src: app/**/*.js
        :analyzer: js

.. note::

    directive argument 'blabla' not used now, but it required by autodoc
    behaviour

Where:

 - `src` option is the pattern to list source files where docs are stored
 - `analyzer` option to determine witch analyzer must be used for
   processing this files

Directive will iterate over `app/**/*.js` files and process
it line by line.


Custom analyzer
---------------

autoanysrc allow define custom analyzers.

Define custom analyzer (conf.py)::

    # make conf.py importtable
    sys.path.insert(0, os.path.abspath('.'))

    from sphinxcontrib.autoanysrc import analyzers

    class CustomAnalyzer(analyzers.BaseAnalyzer):

        def process(self, content):
            """
            Must process content line by line

            :param content: processing file content
            :returns: generator of pairs docs line and line number
            """
            for lineno, srcline in enumerate(content.split('\n')):
                yield 'some parsed doc line from content', lineno


    # put analyzer to the autonaysrc setting
    autoanysrc_analyzers = {
        'my-custom': 'conf.CustomAnalyzer',
    }


And use it::

    .. autoanysrc:: blabla
        :src: ../src/*.js
        :analyzer: my-custom


Default analyzers
-----------------

JSAnalyzer
``````````

Search comments blocks starts by `/*"""` and ends by `*/`
(inspired by `Nuulogic/sphinx-jsapidoc`_).

::

    .. autoanysrc:: directives
        :src: app/services.js
        :analyzer: js

Where services.js::

    /*"""
    Services
    ````````

    The function :func:`someService` does a some function.
    */

    function someService(href, callback, errback) {
    /*"""
    .. function:: someService(href, callback[, errback])

        :param string href: An URI to the location of the resource.
        :param callback: Gets called with the object.
        :throws SomeError: For whatever reason in that case.
        :returns: Something.
    */
        return 'some result';
    };


TODO
----

- encoding option
- allow internal indent in comment block
- generate signatures like ext.autodoc...


.. _documenter: http://sphinx-doc.org/extdev/appapi.html?highlight=documenter#sphinx.application.Sphinx.add_autodocumenter
.. _`Nuulogic/sphinx-jsapidoc`: https://github.com/Nuulogic/sphinx-jsapidoc
