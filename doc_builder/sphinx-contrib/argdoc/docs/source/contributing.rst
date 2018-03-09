Contributing
============

We welcome contributions! But we are new at this, so please be patient. Right
now, we follow these conventions:

Workflow
--------

  #. *Testing:* This is a pretty low-key project, but we will not accept
     feature additions without companion tests. Tests should be written
     using `nose`_ or Python's :mod:`unittest` framework. And, if
     you submit patches without adding new features, please make sure
     all tests in the current test suite still pass. See
     :mod:`sphinxcontrib.argdoc.test.test_argdoc` for details on how tests are
     structured.

  #. Our main repository is at http://bitbucket.org/birkenfeld/sphinx-contrib .
     To submit a patch, fork the repository and submit a pull request.

Formatting
----------

  #. *Code formatting:* Code should be formatted as described
     in `PEP8`_, noting that we use four spaces for indentation.

  #. *Docstrings and documentation:* These should be formatted for `Sphinx`_, 
     as described in the `numpy documentation guide`_ with guidance from 
     `PEP257`_. This means that docstrings are formatted 
     in `reStructuredText`_.

     Beyond that:
   
         - If you use technical terms, please check if a synonym of your term
           is already defined in the :doc:`glossary <glossary>`, and then use 
           that.

           If no synonym is present, please add your term to the
           :doc:`glossary <glossary>`, and refer to it using the ``:term:``
           directive.

         - Rather than define links inline, please put them in
           ``docs/source/links.txt``. 
        
         - Use four-space indentation in ``rst`` files as well as in source

         - Refer to files using ``fixed-width`` formatting e.g. '\`\`fixed-width\`\`'
