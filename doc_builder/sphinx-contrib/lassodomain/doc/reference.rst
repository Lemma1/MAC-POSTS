======================
Lasso Domain Reference
======================

The Lasso domain (name **ls**) provides directives for each language element, as
well as corresponding roles for cross-referencing. See the docs for `Sphinx
domains`_ for more details on syntax.


Directives
==========

The following directives act as containers for Lasso method descriptions.

.. directive:: .. ls:type:: name
               .. ls:trait:: name
               .. ls:thread:: name

   Describes a type, trait, or thread.

Options for container directives include:

*  ``import``, ``imports``:
   Name of an imported trait. Multiple entries are combined into a
   comma-separated list.
*  ``parent``, ``super``:
   The ``parent`` statement can appear in types and threads, which denotes
   the type that the current type is derived from.

These directives describe types of methods, and can be associated with their
container element either by being nested within the body of one of the above
directives, or by having a fully-qualified name using the ``container->name``
syntax.

.. directive:: .. ls:method:: name(signature)
               .. ls:member:: name(signature)

   Describes an unbound or member method. The ``member`` directive is intended
   for methods belonging to a type, although both are processed identically.

.. directive:: .. ls:provide:: name(signature)

   Describes a provide method for a trait or type. Prefixed with **provide** in
   output to distinguish from methods and members.

   Although a type's provide methods and import statements need to be inside a
   ``trait`` block in Lasso code, they can appear alongside member methods in
   reST markup.

.. directive:: .. ls:require:: name(signature)

   Describes a require signature for a trait. Prefixed with **require** in
   output.

Each directive with a signature supports the following options:

*  ``param``, ``parameter``:
   Descriptions of parameters, with or without a type constraint. For an unnamed
   rest parameter, use ``...`` for the name.
*  ``ptype``, ``paramtype``, ``type``:
   Description of parameter type if more than one word is required.
*  ``return``, ``returns``:
   Description of the value returned.
*  ``rtype``, ``returntype``:
   Further description of the return value type.

Every directive also supports the ``see`` or ``url`` option for adding links to
more info, and the ``author`` or ``authors`` option for adding an attribution.
An index entry is created for each directive.

Signatures of methods can be specified as they appear in code, and may also
indicate which parameters are optional either using square brackets or ``=?``,
including the default value if desired.


Quick examples
--------------

::

   .. ls:member:: string->encodeSQL92()::string
   .. ls:member:: string->merge(where::integer, what::string[, offset::integer, length::integer])
   .. ls:member:: string->beginsWith(find::string, -case::boolean =?)
   .. ls:member:: string->endsWith(find::string, -case::boolean = false)


.. ls:member:: string->encodeSQL92()::string
.. ls:member:: string->merge(where::integer, what::string[, offset::integer, length::integer])
.. ls:member:: string->beginsWith(find::string, -case::boolean =?)
.. ls:member:: string->endsWith(find::string, -case::boolean = false)

::

   .. ls:type:: rhino

      Description of the type

      :parent: string
      :import: trait_horned
      :import: trait_herbivore
      :import: trait_foreach
      :see: https://en.wikipedia.org/wiki/Rhinoceros

      .. ls:member:: numberOfHorns(species::string)::integer

         Description of the member method

         :param string species: Specifies the species name to look up
         :return: The number of horns
         :rtype: integer


.. ls:type:: rhino

   Description of the type

   :parent: string
   :import: trait_horned
   :import: trait_herbivore
   :import: trait_foreach
   :see: https://en.wikipedia.org/wiki/Rhinoceros

   .. ls:member:: numberOfHorns(species::string)::integer

      Description of the member method

      :param string species: Specifies the species name to look up
      :return: The number of horns
      :rtype: integer


Roles
=====

Cross-referencing is done with the same role syntax as other domains, except
that member tag syntax is used to associate member methods with their containing
type, trait, or thread using the arrow operator ``->``, such as
``:meth:`bytes->getrange```. References are made with case-insensitive matching.
All other syntax follows what's described in the docs for `Sphinx domains`_.

Use the following roles to link to definitions of each element:

.. role:: ls:meth

   Reference a type member method, trait provide method, trait require
   signature, or unbound method. Be sure to include the enclosing type or trait
   if outside its description block.

.. role:: ls:type
          ls:trait
          ls:thread

   Reference a type, trait, or thread.


Quick example
-------------

::

   The :ls:type:`Pair <pair>` type always contains two elements which are accessed
   with the :ls:meth:`pair->first` and :ls:meth:`~pair->second` methods.


The :ls:type:`Pair <pair>` type always contains two elements which are accessed
with the :ls:meth:`pair->first` and :ls:meth:`~pair->second` methods.


More Info
=========

*  Docs for `Sphinx domains`_
*  `LassoGuide`_
*  `LassoSoft`_

.. _`Sphinx domains`: http://www.sphinx-doc.org/en/stable/domains.html
.. _`LassoGuide`: http://www.lassoguide.com/
.. _`LassoSoft`: http://www.lassosoft.com/
