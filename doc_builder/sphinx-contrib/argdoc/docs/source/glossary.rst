Glossary of terms
=================

 .. glossary ::
    :sorted:

    argument
        Terms or options that control or modify a function or script's behavior.

    docstring
        Documentation for a Python function, method, class, module, or object.
        These must appear as triple-quoted strings immediately  below the `def`
        line for functions and methods, and immediately below the `class`
        line for classes. :term:`module docstrings <module docstring>` appear
        immediately at the top of the module.

    executable scripts
        Scripts that can be executed from a :term:`shell`, as opposed to
        (or in addition to) inside the Python interpreter.

    module docstring
        Documentation for a Python module, which appears as a triple-quoted
        string starting on the first code line::

            #!/usr/bin/env python
            """This is the module docstring. It can be very long
            and span multiple lines, contain tables, et c
            """

            import argparse
            ...

    shell
        An environment that executes commands. This could be a command-line
        environment like the `bash`_ prompt, or a graphical environment like
        the OSX Finder.

    main-like function
        A function that is called when a script is executed from a
        :term:`shell`, as opposed to inside an interactive Python session.
        These are the functions from whose arguments :obj:`sphinxcontrib.argdoc`
        generates documentation.
        
        These are typically named `main`, and in idiomatically written
        Python are called by the following lines, which appear as the 
        final lines of executable scripts::

            if __name__ == "__main__":
                main()

        :obj:`sphinxcontrib.argdoc` detects :term:`main-like functions <main-like function>`
        by scanning modules for functions whose names match the current 
        value of the configuration parameter `argdoc_main_func` (set
        in ``conf.py``, with default value `'main'`).
