====================
py_directive Example
====================

The default behavior is to output pretty math.

 .. py::

 	x = 1

Multiple inputs lines are supported.

.. py::

	y = 2
	z = y*3./2

The showsource option shows each lines equation and then result

.. py::
   :showsource:

   e = 2
   f = e*3./2

Lines spaning multiple lines are supported.

.. py::

   a = [1,2,3, 
        4,5,6]

Lines without assignment are supported.

.. py::

   x*x

Namespace is maintained.

.. py::

   w = x*y*z

You can suppress output.

.. py::
   :suppress:

   s = 'NO'

Or echo the source back in the document.

.. py::
   :showsource:

   t = s.replace('NO', 'YES')

Output precision for floats can be controlled

.. py::
   :prec: 3

   x = 1.23456789

Everything can be code also.

.. py::
   :context: block
   :showsource:

   u = s.upper()
   v = u.lower()






