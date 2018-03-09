Quickstart
==========

Installation
------------
Install stable versions via `pip`_. For a single-user installation:

 .. code-block:: shell

    $ pip install --user sphinxcontrib-argdoc


For a system-wide installation:

 .. code-block:: shell

    $ sudo pip install sphinxcontrib-argdoc


Using :obj:`sphinxcontrib.argdoc` in your project
--------------------------------------------------------------
Setting up :obj:`sphinxcontrib.argdoc` only takes a few steps:

 #. Find the `extensions` definition in your `Sphinx`_ configuration file,
    ``conf.py``, and add `'sphinxcontrib.argdoc'` to the list. For example::

        # inside conf.py
        extensions = [
            'sphinx.ext.autodoc',
            'sphinx.ext.autosummary',
            'sphinx.ext.intersphinx',
            'sphinxcontrib.argdoc' # <---- ta-da!
        ]

 #. Generate document stubs for your package. It is easiest to use
    `sphinx-apidoc`_. From the terminal:
     
     .. code-block:: shell

        $ cd my_project_folder/my_package_folder
        $ sphinx-apidoc -e -o docs/source/generated my_package_name
  
    Or you can make your document stubs manually. Just make sure the
    final document stubs for your :term:`executable scripts` include the
    ``.. automodule :`` `directive`_. A stub file for a demo script
    (e.g. ``demo.rst``) might look like this:

     .. code-block:: rest

        `argdoc` demo script
        ====================

         .. automodule:: some_package.some_module
            :members:
            :undoc-members:
            :show-inheritance:


         .. toctree::
            :maxdepth: 2


 #. Make sure your :term:`executable scripts`:

     #. use :class:`~argparse.ArgumentParser` rather than
        :class:`~optparse.OptionParser` or :mod:`getopt` for argument parsing

     #. contain a :term:`main-like function` (typically called `main`) that
        is called when the script is executed from the :term:`shell`.
    
    If you want your documentation to be extra nice, write a user-friendly
    description of your script in its :term:`module docstring`, and pass
    the docstring contents as a `description` to your
    :class:`~argparse.ArgumentParser`. For example::

        #!/usr/bin/env python
        """This is my module docstring, which describes what my script does
        at length, so that users can figure out what it does. Conveniently
        this text is used both by argparse as help text in the shell, and
        by Sphinx when generating HTML documentation.
        """
        import argparse

        # other functions et c here
        pass

        def main():
            """This is the body of the program"""
            my_parser = argparse.ArgumentParser(description=__doc__,
                                                formatter_class=argparse.RawDescriptionHelpFormatter)
            my_parser.add_argument("some_arg",type=str,help="some helptext, if you want")
            my_parser.add_argument("--some_keyword",type=int,help="Some other helptext")
            # et c. other options & program body

            args = argparse.parse_args()

            # rest of main()
            pass

        if __name__ == "__main__":
            main()


    That's it! There is nothing else you need to do. For further info
    or configuration options, see :doc:`advanced`. For examples, see
    :doc:`examples`.

 .. warning::
    :obj:`sphinxcontrib.argdoc` generates its documentation by calling your executable
    scripts with the argument ``--help``. Therefore, any side effects
    caused by executing your script will take effect during the documentation
    build process.
