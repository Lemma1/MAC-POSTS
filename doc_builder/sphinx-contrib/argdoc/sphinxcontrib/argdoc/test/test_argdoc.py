#!/usr/bin/env python
# coding: utf-8
"""Test suite for :data:`sphinxcontrib.argdoc`.

Test implementation
-------------------
  - Unit tests are provided for regular expressions used to identify different
    patterns from :obj:`argparse` output
    
  - Unit and functional tests e.g. for :func:`sphinxcontrib.argdoc.ext.format_argparser_as_docstring` are
    performed as follows:
    
      1. `Sphinx`_ is run on the test cases in :obj:`sphinxcontrib.argdoc.test.cases` using
         the `reStructuredText`_ (`rst`) document stubs and configuration file
         (``conf.py``) in ``sphinxcontrib/argdoc/test/testdocroot``. `rst` stubs are
         automatically detected by `Sphinx`_, and the corresponding 
         :term:`executable scripts` in :obj:`sphinxcontrib.argdoc.test.cases` auto-detected
         by :class:`TestArgdoc`.  
         
      2. The `rst` output is saved after `sphinxcontrib.argdoc` completes, before conversion
         to HTML (or other formats) by `Sphinx`_. The `rst` output is compared
         against reference output that can be found in ``sphinxcontrib/argdoc/test/testbuild``.
         Comparing the `rst` output makes the tests
         robust to potential changes in html rendering in `Sphinx`_.

To add a test case for a new :mod:`argparse` configuration
----------------------------------------------------------
  1. Create an executable script using argparse following the general form
     of the other tests. Put it in the subpackage :obj:`sphinxcontrib.argdoc.test.cases`
     
  2. Create a matching `rst` document stub, and put it into ``sphinxcontrib/argdoc/test/testdocroot``
  
  3. Add an entry for the `rst` document stub in ``sphinxcontrib/argdoc/test/testdocroot/master_toctree.rst``
   

"""
__date__   = "2015-06-09"
__author__ = "Joshua Griffin Dunn"

import os
import tempfile
import shlex
import shutil
import importlib
import sys
import codecs
import argparse

if sys.version_info < (3,):
    import StringIO as StringIOWrapper
else:
    import io as StringIOWrapper

import sphinxcontrib.argdoc.test.cases

from modulefinder import ModuleFinder
from pkg_resources import resource_filename, cleanup_resources
from nose.tools import assert_equal, assert_true, assert_dict_equal, assert_list_equal
from nose.plugins.attrib import attr
from sphinx import main as sphinxbuild
from sphinxcontrib.argdoc.ext import get_patterns, get_col1_text, get_col2_text, noargdoc,\
                                     post_process_automodule,\
                                     format_argparser_as_docstring,\
                                     make_rest_table,\
                                     safeunicode



class TestArgdoc():
    """Test case for functions defined in :mod:`sphinxcontrib.argdoc.ext`"""

    @classmethod
    def setUpClass(cls):
        # retain record indicating whether builder has been run,
        # so we run it a maximum of once, and only if we decide to do 
        # the expensive tests
        cls.built = False

        # options for sphinx-build runs
        cls.optdict = { "sourcedir" : resource_filename("sphinxcontrib.argdoc","test/testdocroot"),
                        "conf"      : resource_filename("sphinxcontrib.argdoc","test/testdocroot/conf.py"),
                        "outdir"    : tempfile.mkdtemp(prefix="argdoc"),
                       }

        cls.sphinxopts = "-Q -N -b html %(sourcedir)s %(outdir)s" % cls.optdict

        # test cases for patterns
        cls.pattern_tests = {}
        cls.pattern_tests["positional_arg"] = [("  arg1",{"arg1":"arg1","desc":None}),
                                               ("  some_arg         some_description with lots of words",
                                                { "arg1" : "some_arg",
                                                  "desc" : "some_description with lots of words"
                                                }
                                               ),
                                               ("optional arguments:",None),
                                               ("  --kwarg M",None),
                                               ("  -k M",None),
                                               ("  -k",None),
                                               ("  --kwarg",None),
                                               ("  -k, --kwarg",None),
                                               ("  -k M, --kwarg M",None),
                                               ("  -k             some_description with lots of words",None),
                                               ("  --kwarg        some_description with lots of words",None),
                                               ("  -k, --kwarg    some_description with lots of words",None),
                                               ("  -k M           some_description with lots of words",None),
                                               ("  --kwarg M      some_description with lots of words",None),
                                               ("  -k M, --kwarg M   some_description with lots of words",None),
                ]
        cls.pattern_tests["section_title"] = [("optional arguments:", ("optional arguments",)),
                                           ("optional arguments: ",None),
                                           (" optional arguments:",None),
                                           ("optional: arguments:",("optional: arguments",)),
                                           ("positional arguments:",("positional arguments",)),
                                           ("some long string (with parentheses):",("some long string (with parentheses)",)),
                                           ]
        cls.pattern_tests["arg_only"] = [
                                      ("  positional1",None),
                                      ("  po3413134",None),
                                      ("  reallyreallyreallyreallyreallyreallyreallyreallylongpositional",None),
                                      ("  --help",    ('--help', None)),
                                      ("  -h",        ('-h', None)),
                                      ("  -h, --help",('-h', '--help')),
                                      # arg + vals + desc
                                      ("  -n M, --ne M            some description", None),
                                      ("  -n M M, --ne M M        some description", None),
                                      ("  -n M M M, --ne M M M    some description", None),
                                      ("  -n M                    some description", None),
                                      ("  -n M M                  some description", None),
                                      ("  -n M M M                some description", None),
                                      ("  --ne M                  some description", None),
                                      ("  --ne M M                some description", None),
                                      ("  --ne M M M              some description", None),
                                      # arg + desc
                                      ("  -n, --ne                some description", None),
                                      ("  -n                      some description", None),
                                      ("  --ne                    some description", None),
                                      # arg + vals
                                      ("-n M, --ne M", None),
                                      ("-n M M, --ne M M", None),
                                      ("-n M M M, --ne M M M", None),
                                      ("-n M", None),
                                      ("-n M M", None),
                                      ("-n M M M", None),
                                      ("--ne M", None),
                                      ("--ne M M", None),
                                      ("--ne M M M", None),
                                      ]
        cls.pattern_tests["arg_plus_val"] = [("  -o FILENAME, --out FILENAME",('-o', ' FILENAME', '--out', ' FILENAME')),
                                           ("  -o FILENAME",('-o', ' FILENAME', None, None)),
                                           ("  --out FILENAME",('--out', ' FILENAME', None, None)),
                                           ("-o FILENAME, --out FILENAME",None),
                                           ("-o FILENAME",None),
                                           ("--out FILENAME",None),                               
                                           ("  -n M M, --num M M",('-n', ' M M', '--num', ' M M')),
                                           ("  -n M M",('-n', ' M M', None,None)),
                                           ("  --num M M",('--num', ' M M',None,None)),
                                           ("-n M M, --num M M",None),
                                           ("-n M M",None),
                                           ("--num M M",None),
                                           # arg + vals + desc
                                           ("  -n M, --ne M            some description", None),
                                           ("  -n M M, --ne M M        some description", None),
                                           ("  -n M M M, --ne M M M    some description", None),
                                           ("  -n M                    some description", None),
                                           ("  -n M M                  some description", None),
                                           ("  -n M M M                some description", None),
                                           ("  --ne M                  some description", None),
                                           ("  --ne M M                some description", None),
                                           ("  --ne M M M              some description", None),
                                           # arg + desc
                                           ("  -n, --ne                some description", None),
                                           ("  -n                      some description", None),
                                           ("  --ne                    some description", None),
                                           # arg only
                                           ("  --help", None),    
                                           ("  -h",     None),
                                           ("  -h, --help", None),
                                            ]
        cls.pattern_tests["arg_plus_desc"] = [("  -h, --help            show this help message and exit",('-h','--help','show this help message and exit')),
                                           ("  -h                    show this help message and exit",('-h',None, 'show this help message and exit')),
                                           ("  --help                show this help message and exit",('--help',None, 'show this help message and exit')),
                                           ("  -h, --help     show this help message and exit",('-h','--help','show this help message and exit')),
                                           ("  -h             show this help message and exit",('-h',None, 'show this help message and exit')),
                                           ("  --help         show this help message and exit",('--help',None, 'show this help message and exit')),
                                           ("-h, --help     show this help message and exit",None),
                                           ("-h             show this help message and exit",None),
                                           ("--help         show this help message and exit",None),
                                           # arg only
                                           ("  --help",    None),
                                           ("  -h",        None),
                                           ("  -h, --help",None),
                                           # arg + vals + desc
                                           ("  -n M, --ne M            some description", None),
                                           ("  -n M M, --ne M M        some description", None),
                                           ("  -n M M M, --ne M M M    some description", None),
                                           ("  -n M                    some description", None),
                                           ("  -n M M                  some description", None),
                                           ("  -n M M M                some description", None),
                                           ("  --ne M                  some description", None),
                                           ("  --ne M M                some description", None),
                                           ("  --ne M M M              some description", None),
                                           # arg + vals
                                           ("-n M, --ne M", None),
                                           ("-n M M, --ne M M", None),
                                           ("-n M M M, --ne M M M", None),
                                           ("-n M", None),
                                           ("-n M M", None),
                                           ("-n M M M", None),
                                           ("--ne M", None),
                                           ("--ne M M", None),
                                           ("--ne M M M", None),
                                           ]
        cls.pattern_tests["arg_plus_val_desc"] = [
             ("  -n M, --ne M            some description", {"arg1" : "-n", "val1" : " M",    "arg2" : "--ne", "val2" : " M",     "desc" : "some description"}),
             ("  -n M M, --ne M M        some description", {"arg1" : "-n", "val1" : " M M",  "arg2" : "--ne", "val2" : " M M",   "desc" : "some description"}),
             ("  -n M M M, --ne M M M    some description", {"arg1" : "-n", "val1" : " M M M","arg2" : "--ne", "val2" : " M M M", "desc" : "some description"}),
             ("  -n M                    some description", {"arg1" : "-n", "val1" : " M",    "arg2" : None,   "val2" : None,     "desc" : "some description"}),
             ("  -n M M                  some description", {"arg1" : "-n", "val1" : " M M",  "arg2" : None,   "val2" : None,     "desc" : "some description"}),
             ("  -n M M M                some description", {"arg1" : "-n", "val1" : " M M M","arg2" : None,   "val2" : None,     "desc" : "some description"}),
             ("  --ne M                  some description", {"arg1" : "--ne", "val1" : " M",     "arg2" : None,   "val2" : None, "desc" : "some description"}),
             ("  --ne M M                some description", {"arg1" : "--ne", "val1" : " M M",   "arg2" : None,   "val2" : None, "desc" : "some description"}),
             ("  --ne M M M              some description", {"arg1" : "--ne", "val1" : " M M M", "arg2" : None,   "val2" : None, "desc" : "some description"}),
             # arg + vals
             ("  -n M, --ne M            ", None),
             ("  -n M M, --ne M M        ", None),
             ("  -n M M M, --ne M M M    ", None),
             ("  -n M                    ", None),
             ("  -n M M                  ", None),
             ("  -n M M M                ", None),
             ("  --ne M                  ", None),
             ("  --ne M M                ", None),
             ("  --ne M M M              ", None),
             # arg only
             ("  --help", None),    
             ("  -h",     None),
             ("  -h, --help", None),
             # arg + desc
             ("  -n, --ne                some description", None),
             ("  -n                      some description", None),
             ("  --ne                    some description", None),
             # positional
             ("  positional1",None),
             ("  po3413134",None),
             ("  reallyreallyreallyreallyreallyreallyreallyreallylongpositional",None),
                          
                                                ]
        cls.pattern_tests["subcommand_names"] = {("  {one,another,four,five}",("one,another,four,five",)),
                                              ("  {one,another,four}",("one,another,four",)),
                                              ("  {one,another}",("one,another",)),
                                              ("  {just_one}",("just_one",)),
                                              ("{one,another,four,five}",None),
                                              ("{one,another,four}",None),
                                              ("{one,another}",None),
                                              ("{just_one}",None),                                              
                                              }
        cls.pattern_tests["continue_desc"] = []
        cls.pattern_tests["section_desc"] = [
            ("  choose one of the following:",("choose one of the following:",)),
            ("  Sometimes it is useful to group arguments that relate to each other in an",
             ("Sometimes it is useful to group arguments that relate to each other in an",)),
            ("  Description of second argument group",("Description of second argument group",)),
            ("  A special group of arguments in the `bar` subparser",("A special group of arguments in the `bar` subparser",)),
            ("  Oneworddescription",None),

             # arg + vals
             ("  -n M, --ne M            ", None),
             ("  -n M M, --ne M M        ", None),
             ("  -n M M M, --ne M M M    ", None),
             ("  -n M                    ", None),
             ("  -n M M                  ", None),
             ("  -n M M M                ", None),
             ("  --ne M                  ", None),
             ("  --ne M M                ", None),
             ("  --ne M M M              ", None),
             # arg + vals + desc
             ("  -n , --ne             some description", None),
             ("  -n  , --ne          some description", None),
             ("  -n   , --ne       some description", None),
             ("  -n                     some description", None),
             ("  -n                    some description", None),
             ("  -n                   some description", None),
             ("  --ne                   some description", None),
             ("  --ne                  some description", None),
             ("  --ne                 some description", None),
             # arg only
             ("  --help", None),    
             ("  -h",     None),
             ("  -h, --help", None),
             ("  arg1",None),
             ("  some_arg         some_description with lots of words",None),
        ]

        # test cases for test_get_col1_text, test_get_col2_text
        cls.match_dicts = [
                { "arg1" : "ARG",
                  "col1" : "``ARG``",
                  "col2" : "",
                },
                { "arg1" : "ARG",
                  "desc" : "some description",
                  "col1" : "``ARG``",
                  "col2" : "some description",
                },
                { "arg1" : "-v",
                  "val1" : "ARG",
                  "col1" : "``-v ARG``",
                  "col2" : "",
                },
                { "arg1" : "--val",
                  "val1" : "ARG",
                  "col1" : "``--val ARG``",
                  "col2" : ""
                },
                { "arg1" : "-v",
                  "val1" : "ARG",
                  "arg2" : "--val",
                  "val2" : "ARG",
                  "desc" : "some description",
                  "col1" : "``-v ARG``, ``--val ARG``",
                  "col2" : "some description",
                },
                { "arg1" : "-v",
                  "val1" : "ARG",
                  "desc" : "some description",
                  "col1" : "``-v ARG``",
                  "col2" : "some description",
                },
                { "arg1" : "--val",
                  "val1" : "ARG",
                  "desc" : "some description",
                  "col1" : "``--val ARG``",
                  "col2" : "some description",
                },
                { "arg1" : "-v",
                  "val1" : "ARG",
                  "arg2" : "--val",
                  "val2" : "ARG",
                  "desc" : "some description",
                  "col1" : "``-v ARG``, ``--val ARG``",
                  "col2" : "some description",
                },
                { "arg1" : "-v",
                  "arg2" : "--val",
                  "col1" : "``-v``, ``--val``",
                  "col2" : ""
                },
                { "arg1" : "-v",
                  "arg2" : "--val",
                  "desc" : "some description",
                  "col1" : "``-v``, ``--val``",
                  "col2" : "some description",
                },

                ]

        # automatically load module test cases for functional tests
        # testcase names mapped to (module, expected rst output, built rst output)
        cls.test_cases = {}
        mf = ModuleFinder()
        for modname in mf.find_all_submodules(sphinxcontrib.argdoc.test.cases):
            if modname not in (__name__,"__init__"):
                mod = importlib.import_module("sphinxcontrib.argdoc.test.cases.%s" % modname)
                basename = "sphinxcontrib.argdoc.test.cases.%s_postargdoc.rst" % modname
                tup = (mod,
                       resource_filename("sphinxcontrib.argdoc","test/testbuild/%s" % basename),
                       os.path.join(cls.optdict["outdir"],basename))
                cls.test_cases[modname] = tup
 
    @classmethod
    def tearDownClass(cls):
        """Clean up temp files after tests are complete"""
        cleanup_resources()
        shutil.rmtree(cls.optdict["outdir"])

    @classmethod
    def run_builder(cls):
        """Run sphinx builder only the first time it is needed

        Raises
        ------
        AssertionError
            If builder exists with non-zero status
        """ 
        if cls.built == False:
            try:
                sphinxbuild(shlex.split(cls.sphinxopts))
            except SystemExit as e:
                if e.code != 0:
                    raise AssertionError("Error running sphinx-build (exited with code %s)" % e.code)

            cls.built = True 

    @staticmethod
    def check_pattern(test_name,pat,inp,expected):
        """Check patterns for matching, or non-matching

        Parameters
        ----------
        test_name : str
            Name of test set being executed

        pat : :class:`re.compile`
            Pattern to test

        inp : str
            Input to test

        expected : dict, tuple, or None
            Expected result. If a `dict`, equivalence is tested with
            `pat.match(inp).groupdict()` is called to test equivalence.
            If a 'tuple' equivalence is tested with `pat.match(inp).groups()`,
            :meth:`re.compile.groups` is called. If `None`, it is asserted
            that `pat.match(inp)` is `None` 
        """
        if expected is None:
            msg = "For test '%s', pattern %s' matched '%s', " % (test_name,
                                                                 pat.pattern,
                                                                 inp)
            assert_true(pat.match(inp) is None,msg)
        else:
            if isinstance(expected,dict):
                groups = pat.match(inp).groupdict()
                fn = assert_dict_equal
            else:
                groups = pat.match(inp).groups()
                fn = assert_equal
            msg = "For test '%s', pattern %s' input '%s': expected %s, got %s " % (test_name,
                                                                                   pat.pattern,
                                                                                   inp,
                                                                                   expected,
                                                                                   groups)
            fn(expected,groups,msg)
    
    def test_patterns(self):
        # test all patterns
        patterns = get_patterns("-")
        for name, cases in self.pattern_tests.items():
            for inp,expected in cases:
                yield self.check_pattern, name, patterns["-"][name], inp, expected

    @staticmethod
    def check_equal(expected,found,casename=""):
        """Helper method just to allow us to use test generators in other tests"""
        if isinstance(expected,list):
            idx = 2
        elif isinstance(expected,str):
            idx = 80
        else:
            idx = None

        ellip = "..." if len(expected) > idx else ""

        message = "Expected '%s%s', found '%s%s'" % (expected[:idx],ellip,found[:idx],ellip)
        if casename != "":
            message = "test '%s': %s" % (casename,message)

        assert_equal(expected,found,message)

    def test_prefix_chars_matches(self):
        app = FakeApp(argdoc_prefix_chars="+")
        parser = argparse.ArgumentParser(prefix_chars="+",
                                         description="")
        parser.add_argument("pos1",help="Positional argument 1")
        parser.add_argument("pos2")
        parser.add_argument("+o","++option",metavar="N",help="Some argument")
        parser.add_argument("+x",nargs=2,metavar="N",help="argument with multiple values")
        parser.add_argument("++other",action="store_true",default=False,help="No-value argument")
        parser.add_argument("++argumentwithreallyreallyreallylongname",help="""An argument with
        a really really really really long name, and a really reallly really long multi-line
        help""")

        lines = parser.format_help().split("\n")
        found_lines    = format_argparser_as_docstring(app,None,lines,get_patterns("+"))
        expected_lines = [
                 '',
                 '------------',
                 '',
                 '',
                 u'Command-line arguments',
                 '----------------------',
                 u'',
                 'Positional arguments',
                 '~~~~~~~~~~~~~~~~~~~~',
                 u'',
                 u'    =============    ==========================',
                 u'    **Argument**     **Description**           ',
                 u'    -------------    --------------------------',
                 u'    ``pos1``         Positional argument 1     ',
                 u'    ``pos2``                                   ',
                 u'    =============    ==========================',
                 u'',
                 u'',
                 'Optional arguments',
                 '~~~~~~~~~~~~~~~~~~',
                 u'',
                 u'    =========================================================================================    ================================================================================================================',
                 u'    **Argument**                                                                                 **Description**                                                                                                 ',
                 u'    -----------------------------------------------------------------------------------------    ----------------------------------------------------------------------------------------------------------------',
                 u'    ``+h``, ``++help``                                                                           show this help message and exit                                                                                 ',
                 u'    ``+o  N``, ``++option  N``                                                                   Some argument                                                                                                   ',
                 u'    ``+x  N N``                                                                                  argument with multiple values                                                                                   ',
                 u'    ``++other``                                                                                  No-value argument                                                                                               ',
                 u'    ``++argumentwithreallyreallyreallylongname  ARGUMENTWITHREALLYREALLYREALLYLONGNAME``          An argument with a really really really really long name, and a really reallly really long multi-line help     ',
                 u'    =========================================================================================    ================================================================================================================',
                 u'']

        n1 = n2 = 0
        for line in expected_lines:
            if line[:23] != "Command-line arguments":
                n1 += 1
            else:
                break

        for line in found_lines:
            if line[:23] != "Command-line arguments":
                n2 += 1   
            else:
                break

        yield self.check_list_equal, expected_lines[n1:], found_lines[n2:], "prefix_chars"

    @staticmethod
    def check_not_match(pattern,inp,msg):
        # make sure a pattern does not match inp
        assert_true(pattern.match(inp) is None,msg)

    def test_prefix_chars_does_not_match_wrong(self):
        # make sure patterns with prefix char "+" don't match examples with prefix char "-"
        patterns = get_patterns("+")
        for k in patterns["+"]:
            if k.startswith("arg"):
                for inp, _ in self.pattern_tests[k]:
                    msg = "pattern_does_not_match_wrong test %s: '%s' matched, should not have." % (k,inp)
                    yield self.check_not_match, patterns["+"][k], inp , msg

    def test_prefix_chars_does_not_mix(self):
        # make sure pattenr dicts with multiple prefix chars have no crosstalk
        patterns = get_patterns("-+")
        for k in patterns["+"]:
            if k.startswith("arg"):
                for inp, _ in self.pattern_tests[k]:
                    msg = "prefix_chars_does_not_mix test %s: '%s' matched, should not have." % (k,inp)
                    yield self.check_not_match, patterns["+"][k], inp , msg
        for name, cases in self.pattern_tests.items():
            for inp, expected in cases:
                yield self.check_pattern, name, patterns["-"][name], inp, expected

    def test_get_col1_text(self):
        for my_dict in self.match_dicts:
            yield self.check_equal, get_col1_text(my_dict), my_dict["col1"]

    def test_get_col2_text(self):
        for my_dict in self.match_dicts:
            yield self.check_equal, get_col2_text(my_dict), my_dict["col2"]

    def test_make_rest_table_with_title(self):
        rows = [("Column 1","Column 2"),
                ("1","a"),
                ("2","b"),
                ("30000000000","something really long, or, somewhat long"),
                ("12315132","a line with ``special characters`` and *stars*")]
        expected = [
            safeunicode('================    ==================================================='),
            safeunicode('**Column 1**        **Column 2**                                       '),
            safeunicode('----------------    ---------------------------------------------------'),
            safeunicode('1                   a                                                  '),
            safeunicode('2                   b                                                  '),
            safeunicode('30000000000         something really long, or, somewhat long           '),
            safeunicode('12315132            a line with ``special characters`` and *stars*     '),
            safeunicode('================    ==================================================='),
            safeunicode('')
        ]
        found = make_rest_table(rows,title=True,indent=0)
        assert_list_equal(expected,found)

    def test_make_rest_table_without_title(self):
        rows = [("Column 1","Column 2"),
                ("1","a"),
                ("2","b"),
                ("30000000000","something really long, or, somewhat long"),
                ("12315132","a line with ``special characters`` and *stars*")]
        expected = [
            safeunicode('============    ==============================================='),
            safeunicode('Column 1        Column 2                                       '),
            safeunicode('1               a                                              '),
            safeunicode('2               b                                              '),
            safeunicode('30000000000     something really long, or, somewhat long       '),
            safeunicode('12315132        a line with ``special characters`` and *stars* '),
            safeunicode('============    ==============================================='),
            safeunicode('')
        ]
        found = make_rest_table(rows,title=False,indent=0)
        assert_list_equal(expected,found)

    def test_make_rest_table_with_indent(self):
        rows = [("Column 1","Column 2"),
                ("1","a"),
                ("2","b"),
                ("30000000000","something really long, or, somewhat long"),
                ("12315132","a line with ``special characters`` and *stars*")]
        expected = [
            safeunicode('    ============    ==============================================='),
            safeunicode('    Column 1        Column 2                                       '),
            safeunicode('    1               a                                              '),
            safeunicode('    2               b                                              '),
            safeunicode('    30000000000     something really long, or, somewhat long       '),
            safeunicode('    12315132        a line with ``special characters`` and *stars* '),
            safeunicode('    ============    ==============================================='),
            safeunicode('')
        ]
        found = make_rest_table(rows,title=False,indent=4)
        assert_list_equal(expected,found)

    def test_noargdoc_adds_attribute(self):
        def my_func():
            pass

        b = noargdoc(my_func)
        assert_true(b.__dict__["noargdoc"])

    @staticmethod
    def check_list_equal(l1,l2,test_name):
        mismatched = 0 
        in_l1 = []
        in_l2 = []
        i = j = 0
        while i < len(l1) and j < len(l2):
            line1 = l1[i]
            line2 = l2[j]
            if line1.rstrip() != line2.rstrip():
                mismatched += 1
                if line1 not in l2:
                    in_l1.append((i,line1))
                    i += 1
                if line2 not in l1:
                    in_l2.append((j,line2))
                    j += 1
            i += 1
            j += 1
         
        message = ""
        if mismatched > 0:
            message  = safeunicode("-"*75 + "\n")
            message += "Failed list equality for test %s\n" % test_name
            message += "%s mismatches (expected 0).\n" % mismatched
            message += "In list 1 only:\n"
            for l in in_l1:
                message += ("%s: %s\n" % l)
  
            message += "In list 2 only:\n"
            for l in in_l2:
                message += ("%s: %s\n" % l)
 
            message += "-"*75 + "\n"

        assert_equal(mismatched,0,message)
        
    def test_format_argparser_as_docstring(self):
        # look at output & test against known RST
        app = FakeApp(outdir=self.optdict["outdir"],argdoc_prefix_chars="-+")
        for k in self.test_cases:
            testname = "test_format_argparser_as_docstring '%s'" % k            
            mod, expected, _ = self.test_cases[k]
            with codecs.open(expected,encoding="utf-8",mode="r") as f:
                expected_lines = f.read().split("\n")
            f.close()
            
            buf = StringIOWrapper.StringIO()
            old_out = sys.stdout
            sys.stdout = buf

            try:
                mod.main(["--help"])
            except SystemExit as e:
                if e.code != 0:
                    raise(AssertionError("Exit code for '%s --help' was %s instead of zero" % (mod.__name__,e.code)))
 
            sys.stdout = old_out
            buf.seek(0)
            lines = buf.read().split("\n")
            found_lines = format_argparser_as_docstring(app,mod,lines,
                                                        section_head=True,
                                                        header_level=1,
                                                        patterns=get_patterns(prefix_chars="-+"))

            n1 = n2 = 0
            for line in expected_lines:
                if line[:23] != "Command-line arguments":
                    n1 += 1
                else:
                    break

            for line in found_lines:
                if line[:23] != "Command-line arguments":
                    n2 += 1   
                else:
                    break

            yield self.check_list_equal, expected_lines[n1:], found_lines[n2:], testname

    @attr(kind="functional")
    def test_post_process_automodule(self):
        self.run_builder()
        for k, (_,expected,built) in self.test_cases.items():
            if k == "noargdoc":
                continue
            with open(expected) as f:
                expected_lines = f.read().split("\n")

            with open(built) as f:
                built_lines = f.read().split("\n")

            testname = "test_post_process_automodule '%s'" % k
            yield self.check_list_equal, expected_lines, built_lines, testname

    def test_post_process_automodule_emits_event(self):
        for k, (mod,_,_) in self.test_cases.items():
            testname = "test_post_process_automodule_emits_event '%s'" % k
            app = FakeApp(outdir=self.optdict["outdir"])
            options = {}
            expected = ["argdoc-process-docstring"]
            _ = post_process_automodule(app,"module",mod.__name__,mod,options,[])
            yield self.check_equal, expected, app.emitted, testname


class Record(object):
    """Proxy object that allows addition of arbitrary properties"""
    def __init__(self):
        pass


class FakeApp(object):
    """Proxy for a Sphinx application object. Implements minimial methods
    required for us to test functions in :mod:`sphinxcontrib.argdoc.ext` that require
    a Sphinx application instance
    """
    def __init__(self,argdoc_main_func="main",argdoc_save_rst=True,outdir="/tmp/",argdoc_prefix_chars="-"):
        self.config = Record()
        self.config.argdoc_main_func    = argdoc_main_func
        self.config.argdoc_save_rst     = argdoc_save_rst
        self.config.argdoc_prefix_chars = argdoc_prefix_chars
        self.outdir  = outdir
        self.emitted = []

    def warn(self,*args,**kwargs):
        pass

    def debug(self,*args,**kwargs):
        pass

    def debug2(self,*args,**kwargs):
        pass

    def emit(self,*args,**kwargs):
        """Simulate `emit` method. Save event name in `self.emitted` at each call"""
        self.emitted.append(args[0])
