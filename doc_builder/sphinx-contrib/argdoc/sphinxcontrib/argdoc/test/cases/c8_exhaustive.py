#!/usr/bin/env python
# coding: utf-8
"""Exhaustively test all argument types

The following argument attributes are tested in all combinations to ensure
they are all properly parsed and rendered:

  - arguments with short or long names
  - positional arguments
  - keyword arguments starting with '-', '--', or both
  - arguments taking 0, 1, 2, (0 or more), (1 or more), or (0 or 1) arguments
  - arguments taking choices of 1 or more items
  - arguments with no help text
  - arguments with short help text, which tends to be displayed on one line
    by :mod:`argparse`
  - arguments with long help text, which tends to appear on multiple lines
  - arguments including or excluding unicode characters in their names

--------------

Here is a table, to show that we can have rich formatting in the module 
docstring, without :obj:`sphinxcontrib.argdoc` inadvertently introducing problems:

    =============  ======================================================
    **Column 1**   **Column 2**
    -------------  ------------------------------------------------------
     Some item     Some other item.

     Table row 2.  Table row 2 column 2.

     Another row.  Row with a link to `Python <https://www.python.org>`_
    =============  ======================================================
 

See also
--------
A definition list
    The purpose of this `See also` section is just to show that we can use
    a number of reStructuredText structures in the module docstring, and
    still have the argument descriptions appended below.

Here is another item
    To show that our test works
"""
import argparse
import sys

helptext = {
    "short" : "one-line help text",
    "long"  : "this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)",
}

choices = {
    "single" : ["one_choice"],
    "multi"  : ["one","two","three","four"],
}

def main(argv=sys.argv[1:]):
    """Command-line program for `simple_parser`"""
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    # positional arguments
    parser.add_argument("shortpos1")
    parser.add_argument("shortpos2",help=helptext["short"])
    parser.add_argument("shortpos3",help=helptext["long"])
    parser.add_argument("reallyreallyreallyreallyreallyreallyreallyreallylongpositionalargument1")
    parser.add_argument("reallyreallyreallyreallyreallyreallyreallyreallylongpositionalargument2",help=helptext["short"])
    parser.add_argument("reallyreallyreallyreallyreallyreallyreallyreallylongpositionalargument3",help=helptext["long"])
 
    # optional arguments, one arg
    parser.add_argument("-a",metavar="X")
    parser.add_argument("-b",help=helptext["short"],metavar="X")
    parser.add_argument("-c",help=helptext["long"],metavar="X")
    parser.add_argument("--double1",metavar="X")
    parser.add_argument("--double2",help=helptext["short"],metavar="X")
    parser.add_argument("--double3",help=helptext["long"],metavar="X")
    parser.add_argument("-d","--combo1",metavar="X")
    parser.add_argument("-e","--combo2",help=helptext["short"],metavar="X")
    parser.add_argument("-f","--combo3",help=helptext["long"],metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double1",metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double2",help=helptext["short"],metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double3",help=helptext["long"],metavar="X")
    parser.add_argument("-g","--reallyreallyreallyreallyreallyreallylong_combo1",metavar="X")
    parser.add_argument("-i","--reallyreallyreallyreallyreallyreallylong_combo2",help=helptext["short"],metavar="X")
    parser.add_argument("-j","--reallyreallyreallyreallyreallyreallylong_combo3",help=helptext["long"],metavar="X")

    # optional arguments, 2 args
    parser.add_argument("-k",nargs=2,metavar="X")
    parser.add_argument("-l",help=helptext["short"],nargs=2,metavar="X")
    parser.add_argument("-m",help=helptext["long"],nargs=2,metavar="X")
    parser.add_argument("--double4",nargs=2,metavar="X")
    parser.add_argument("--double5",help=helptext["short"],nargs=2,metavar="X")
    parser.add_argument("--double6",help=helptext["long"],nargs=2,metavar="X")
    parser.add_argument("-n","--combo4",nargs=2,metavar="X")
    parser.add_argument("-o","--combo5",help=helptext["short"],nargs=2,metavar="X")
    parser.add_argument("-p","--combo6",help=helptext["long"],nargs=2,metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double4",nargs=2,metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double5",help=helptext["short"],nargs=2,metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double6",help=helptext["long"],nargs=2,metavar="X")
    parser.add_argument("-q","--reallyreallyreallyreallyreallyreallylong_combo4",nargs=2,metavar="X")
    parser.add_argument("-r","--reallyreallyreallyreallyreallyreallylong_combo5",help=helptext["short"],nargs=2,metavar="X")
    parser.add_argument("-s","--reallyreallyreallyreallyreallyreallylong_combo6",help=helptext["long"],nargs=2,metavar="X")

    # optional arguments, + args
    parser.add_argument("-t",nargs='+',metavar="X")
    parser.add_argument("-u",help=helptext["short"],nargs='+',metavar="X")
    parser.add_argument("-v",help=helptext["long"],nargs='+',metavar="X")
    parser.add_argument("--double7",nargs='+',metavar="X")
    parser.add_argument("--double8",help=helptext["short"],nargs='+',metavar="X")
    parser.add_argument("--double9",help=helptext["long"],nargs='+',metavar="X")
    parser.add_argument("-w","--combo7",nargs='+',metavar="X")
    parser.add_argument("-x","--combo8",help=helptext["short"],nargs='+',metavar="X")
    parser.add_argument("-y","--combo9",help=helptext["long"],nargs='+',metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double7",nargs='+',metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double8",help=helptext["short"],nargs='+',metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double9",help=helptext["long"],nargs='+',metavar="X")
    parser.add_argument("-z","--reallyreallyreallyreallyreallyreallylong_combo7",nargs='+',metavar="X")
    parser.add_argument("-A","--reallyreallyreallyreallyreallyreallylong_combo8",help=helptext["short"],nargs='+',metavar="X")
    parser.add_argument("-B","--reallyreallyreallyreallyreallyreallylong_combo9",help=helptext["long"],nargs='+',metavar="X")

    # optional arguments, ? args
    parser.add_argument("-C",nargs='?',metavar="X")
    parser.add_argument("-D",help=helptext["short"],nargs='?',metavar="X")
    parser.add_argument("-E",help=helptext["long"],nargs='?',metavar="X")
    parser.add_argument("--double10",nargs='?',metavar="X")
    parser.add_argument("--double11",help=helptext["short"],nargs='?',metavar="X")
    parser.add_argument("--double12",help=helptext["long"],nargs='?',metavar="X")
    parser.add_argument("-F","--combo10",nargs='?',metavar="X")
    parser.add_argument("-G","--combo11",help=helptext["short"],nargs='?',metavar="X")
    parser.add_argument("-H","--combo12",help=helptext["long"],nargs='?',metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double10",nargs='?',metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double11",help=helptext["short"],nargs='?',metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double12",help=helptext["long"],nargs='?',metavar="X")
    parser.add_argument("-I","--reallyreallyreallyreallyreallyreallylong_combo10",nargs='?',metavar="X")
    parser.add_argument("-J","--reallyreallyreallyreallyreallyreallylong_combo11",help=helptext["short"],nargs='?',metavar="X")
    parser.add_argument("-K","--reallyreallyreallyreallyreallyreallylong_combo12",help=helptext["long"],nargs='?',metavar="X")

    # optional arguments, * args
    parser.add_argument("-L",nargs='*',metavar="X")
    parser.add_argument("-M",help=helptext["short"],nargs='*',metavar="X")
    parser.add_argument("-N",help=helptext["long"],nargs='*',metavar="X")
    parser.add_argument("--double13",nargs='*',metavar="X")
    parser.add_argument("--double14",help=helptext["short"],nargs='*',metavar="X")
    parser.add_argument("--double15",help=helptext["long"],nargs='*',metavar="X")
    parser.add_argument("-O","--combo13",nargs='*',metavar="X")
    parser.add_argument("-P","--combo14",help=helptext["short"],nargs='*',metavar="X")
    parser.add_argument("-Q","--combo15",help=helptext["long"],nargs='*',metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double13",nargs='*',metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double14",help=helptext["short"],nargs='*',metavar="X")
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double15",help=helptext["long"],nargs='*',metavar="X")
    parser.add_argument("-R","--reallyreallyreallyreallyreallyreallylong_combo13",nargs='*',metavar="X")
    parser.add_argument("-S","--reallyreallyreallyreallyreallyreallylong_combo14",help=helptext["short"],nargs='*',metavar="X")
    parser.add_argument("-T","--reallyreallyreallyreallyreallyreallylong_combo15",help=helptext["long"],nargs='*',metavar="X")

    # optional arguments, single choices
    parser.add_argument("-U",choices=choices["single"],)
    parser.add_argument("-V",help=helptext["short"],choices=choices["single"],)
    parser.add_argument("-W",help=helptext["long"],choices=choices["single"],)
    parser.add_argument("--double16",choices=choices["single"],)
    parser.add_argument("--double17",help=helptext["short"],choices=choices["single"],)
    parser.add_argument("--double18",help=helptext["long"],choices=choices["single"],)
    parser.add_argument("-X","--combo16",choices=choices["single"],)
    parser.add_argument("-Y","--combo17",help=helptext["short"],choices=choices["single"],)
    parser.add_argument("-Z","--combo18",help=helptext["long"],choices=choices["single"],)
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double16",choices=choices["single"],)
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double17",help=helptext["short"],choices=choices["single"],)
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double18",help=helptext["long"],choices=choices["single"],)
    parser.add_argument("-1","--reallyreallyreallyreallyreallyreallylong_combo16",choices=choices["single"],)
    parser.add_argument("-2","--reallyreallyreallyreallyreallyreallylong_combo17",help=helptext["short"],choices=choices["single"],)
    parser.add_argument("-3","--reallyreallyreallyreallyreallyreallylong_combo18",help=helptext["long"],choices=choices["single"],)

    # optional arguments, multi choices
    parser.add_argument("-4",choices=choices["multi"],)
    parser.add_argument("-5",help=helptext["short"],choices=choices["multi"],)
    parser.add_argument("-6",help=helptext["long"],choices=choices["multi"],)
    parser.add_argument("--double19",choices=choices["multi"],)
    parser.add_argument("--double20",help=helptext["short"],choices=choices["multi"],)
    parser.add_argument("--double21",help=helptext["long"],choices=choices["multi"],)
    parser.add_argument("-7","--combo19",choices=choices["multi"],)
    parser.add_argument("-8","--combo20",help=helptext["short"],choices=choices["multi"],)
    parser.add_argument("-9","--combo21",help=helptext["long"],choices=choices["multi"],)
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double19",choices=choices["multi"],)
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double20",help=helptext["short"],choices=choices["multi"],)
    parser.add_argument("--reallyreallyreallyreallyreallyreallylong_double21",help=helptext["long"],choices=choices["multi"],)
    parser.add_argument("-Â","--reallyreallyreallyreallyreallyreallylong_combo19",choices=choices["multi"],)
    parser.add_argument("-Ã","--reallyreallyreallyreallyreallyreallylong_combo20",help=helptext["short"],choices=choices["multi"],)
    parser.add_argument("-Ä","--reallyreallyreallyreallyreallyreallylong_combo21",help=helptext["long"],choices=choices["multi"],)

    args = parser.parse_args(argv)

if __name__ == "__main__":
    main()
