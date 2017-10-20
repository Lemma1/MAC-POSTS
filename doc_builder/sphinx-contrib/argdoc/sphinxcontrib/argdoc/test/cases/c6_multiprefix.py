#!/usr/bin/env python
# coding: utf-8
"""Multiple prefix characters

:class:`~argparse.ArgumentParser` takes a `prefix_chars` option which allows
the use of keyword arguments that begin with characters other than `'-'`.
Here we test a :class:`~argparse.ArgumentParser` that uses both `'-'` and `'+'`.
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
                                     formatter_class=argparse.RawDescriptionHelpFormatter,
                                     prefix_chars="-+")
    # positional arguments
    parser.add_argument("shortpos1")
    parser.add_argument("shortpos2",help=helptext["short"])
    parser.add_argument("shortpos3",help=helptext["long"])
    parser.add_argument("reallyreallyreallyreallyreallyreallyreallyreallylongpositionalargument1")
    parser.add_argument("reallyreallyreallyreallyreallyreallyreallyreallylongpositionalargument2",help=helptext["short"])
    parser.add_argument("reallyreallyreallyreallyreallyreallyreallyreallylongpositionalargument3",help=helptext["long"])
 
    # optional arguments, one arg
    parser.add_argument("-a",metavar="X")
    parser.add_argument("--bar",help=helptext["short"],metavar="X")
    parser.add_argument("--c","--arg3",nargs=2,help=helptext["long"],metavar="X")
    parser.add_argument("+x")
    parser.add_argument("++another",help="Another argument")
    parser.add_argument("+y","++arg4",nargs="+",help="An argument beginning with '+'")
    args = parser.parse_args(argv)

if __name__ == "__main__":
    main()
