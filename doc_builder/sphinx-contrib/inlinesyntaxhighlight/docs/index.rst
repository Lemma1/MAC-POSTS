Welcome to Inline Syntax Highlight Sphinx Extension
===================================================

.. highlight:: rst

.. role:: python(code)
    :language: py

.. role:: latex(code)
    :language: latex

.. default-domain:: rst

Docutils supports inline syntax highlighting through :code:`code` directive 
and role.  Sphinx does it in a different way.  This results in the need of two
pygments stylesheets for getting having both syntax highlightings (code blocks 
and inline code).  And it is always also a matter of docutils and sphinx
versions, because you need newest docutils installed, for supporting this.

This extension handles ``literal`` nodes like sphinx handles ``literal_block``
nodes and creates sphinx style syntax highlighting for inline code.

You can configure, if literals like ````this literal```` are also highlighted 
using language, set by :code:`highlight` directive.  You can also configure, if 
:code:`code` is highlighted using language defined by :code:`highlight` 
directive.

If you have not yet the docutils version installed, which provides role 
:code:`code`, this extension provides it.

In this documentation both configuration values below are set to true.  Then
you can use this extension as follows::

    .. highlight:: rst

    .. role:: python(code)
        :language: python

    .. role:: latex(code)
        :language: latex

    Now here are latex command :latex:`\\setlength` and python command 
    :python:`import`, created by ``:python:`import```.  Here is a 
    :literal:`literal`, which stays a literal, and 
    :code:`.. highlight:: rst` makes code role look as it looks.

Which is rendered as follows::

.. highlight:: rst

Now here are latex command :latex:`\\setlength` and python command 
:python:`import`, created by ``:python:`import```.  Here is a 
:literal:`literal`, which stays a literal, and 
:code:`.. highlight:: rst` makes code role look as it looks.



Configuration
-------------

.. confval:: inline_highlight_respect_highlight

    This (boolean) setting triggers, if language, which is set by 
    :code:`highlight` directive, shall be used in :code:`code` role, if no 
    language is set by a customization.  Then instead of::

       .. role:: python(code)
           :language: py

       this can :python:`trigger("python", "syntax highlight")`

    Wich is rendered:

       this can :python:`trigger("python", "syntax highlight")`

    You can also write::

       .. highlight:: py

       this can :code:`trigger("python", "syntax highlight")`

.. confval:: inline_highlight_literals

    This (boolean) setting triggers, if ````literals```` shall be highlighted.
    Default is :python:`True`.

    If :confval:`inline_highlight_literals` is on, you can even write for
    example above::

        .. highlight:: py

        this can ``trigger("python", "syntax highlight")``

