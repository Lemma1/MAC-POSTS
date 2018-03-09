mockautodoc
===========

Mock imports for build docs without install packages in environment.

This extension add `:mockimport:` and `:mocktype:` for standard automodule
derictive from autodoc_.

Main goal of extension is to solve task defined on question_.

Install
-------

::

    pip install sphinxcontrib-mockautodoc


Usage
-----

Add mockautodoc to extensions list::

    extenstion = ['sphinx.ext.autodoc', 'sphinxcontrib.mockautodoc', ]

The extension add two options to standard automodule:

- `:mockimport:` list of name spaces to mock separated by ','.
- `:mocktype:` list of objects which must be mock as type object 
    for right way subclass them in sources, separated by ','.

If you have module for documenting::
    
    from twisted.internet import protocol, defer
    
    @defer.inlineCallbacks
    def my_func():
        """my function"""
        pass

    class MyProtocol(protocol.Protocol):
        """my protocol"""
        pass

And twisted_ not installed you can document module like that::
    
    .. automodule:: package
        :mockimport: twisted
        :mocktype: protocol.Protocol


Where on `:mockimport:` will be mocked all twisted.* imports
in automodule building context. And `:mocktype:` means which instances must be 
object types to successful subclass them without loss target documentation.


mockatudoc settings may be defined on conf.py and all automodule directives
with present `:mockimport:` will be mocked. Where data on mockatodoc settings
will be summed with `:mockimport:` and `:mocktype:` options::
    
    mockautodoc = {
        'mockimport': ['twisted',],
        'mocktype': ['protocol.Protocol'],
    }

.. _twisted: http://twistedmatrix.com/
.. _autodoc: http://sphinx-doc.org/ext/autodoc.html
.. _question: http://stackoverflow.com/questions/15587305/any-ways-build-docs-by-sphinx-with-both-python2-x-and-python3-x-ecosystems
