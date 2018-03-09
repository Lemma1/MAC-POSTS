============
Lasso Domain
============

:author: Eric Knibbe <eric at lassosoft dotcom>


About
=====

This extension adds support for the Lasso language to Sphinx.

The following objects are supported:

*  Unbound method
*  Trait

   *  Require
   *  Provide

*  Type/Thread

   *  Member method
   *  Provide


Installation
============

After installing ``lassodomain.py`` in ``site-packages/sphinxcontrib``, add the
``sphinxcontrib.lassodomain`` extension to the ``extensions`` list in your
Sphinx configuration file (``conf.py``):

.. code-block:: python

   extensions = ['sphinxcontrib.lassodomain']

Also, if your project is primarily Lasso, you'll want to define the primary
domain as well:

.. code-block:: python

   primary_domain = 'ls'


Directives example
==================

The following source will generate the output below::

   .. ls:method:: tag_exists(p0::string)

      Indicates whether a tag currently exists.

      :param string find: the tag name to search for

   .. ls:member:: array->exchange(left::integer, right::integer)

      Exchanges the two elements within the array.

   .. ls:type:: string

      Text in Lasso is stored and manipulated using the
      :ls:type:`String` data type or the ``string_...`` methods.

      :see:    http://lassoguide.com/operations/strings.html
      :parent: null

      .. ls:member:: find(find, offset::integer[, -case::boolean=false])::integer

         Finds the position in the string where the given pattern
         matches. Analogous to the :ls:meth:`String_FindPosition` method.

         Takes a parameter that specifies a pattern to search the string
         object for and returns the position in the string object where
         that pattern first begins or zero if the pattern cannot be
         found.

         :param find: a pattern to search the string object for
         :param integer offset: where to start the search
         :param -case: whether to consider character case when searching
         :ptype -case: boolean, default false
         :return:
            Position in the string object where the pattern first begins,
            or zero if the pattern cannot be found.
         :rtype: integer

   .. ls:trait:: trait_forEach

      Provides iteration over a series of values.

      :import: trait_decompose_assignment

      .. ls:require:: forEach()

      .. ls:provide:: asGenerator()::trait_generator

         Allows the type to act as a generator.

         :rtype: trait_generator


.. ls:method:: tag_exists(p0::string)

   Indicates whether a tag currently exists.

   :param string find: the tag name to search for

.. ls:member:: array->exchange(left::integer, right::integer)

   Exchanges the two elements within the array.

.. ls:type:: string

   Text in Lasso is stored and manipulated using the
   :ls:type:`String` data type or the ``string_...`` methods.

   :see:    http://lassoguide.com/operations/strings.html
   :parent: null

   .. ls:member:: find(find, offset::integer[, -case::boolean=false])::integer

      Finds the position in the string where the given pattern
      matches. Analogous to the :ls:meth:`String_FindPosition` method.

      Takes a parameter that specifies a pattern to search the string
      object for and returns the position in the string object where
      that pattern first begins or zero if the pattern cannot be
      found.

      :param find: a pattern to search the string object for
      :param integer offset: where to start the search
      :param -case: whether to consider character case when searching
      :ptype -case: boolean, default false
      :return:
         Position in the string object where the pattern first begins,
         or zero if the pattern cannot be found.
      :rtype: integer

.. ls:trait:: trait_forEach

   Provides iteration over a series of values.

   :import: trait_decompose_assignment

   .. ls:require:: forEach()

   .. ls:provide:: asGenerator()::trait_generator

      Allows the type to act as a generator.

      :rtype: trait_generator


Roles example
=============

From elsewhere in the document you can use the following syntax to link to
definitions of each element::

   Use :ls:meth:`array->exchange` to swap the position of two array elements.
   To check if a tag exists, use :ls:meth:`tag_exists(string)`.

Use :ls:meth:`array->exchange` to swap the position of two array elements.
To check if a tag exists, use :ls:meth:`tag_exists(string)`.

A set of empty parentheses ``()`` will be appended, if absent, to method
cross-references by default. This can be reversed by setting
``add_function_parentheses = False`` in your project's ``conf.py``, causing them
to be removed if present. If the parentheses contain a value, in either case
they will be included in the link title.
