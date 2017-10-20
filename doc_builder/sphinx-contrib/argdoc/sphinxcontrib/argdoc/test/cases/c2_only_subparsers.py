#!/usr/bin/env python
"""Subcommands with no main program arguments

:class:`~argparse.ArgumentParser` supports use of subcommands, 
each of which might have their own arguments and argument groups.
Here these are tested for a parser that has none of its own arguments,
except ``--help``.
"""
import argparse
import sys

foo_help = "Run the foo subprogram"
foo_desc = """This is a long description of what a ``foo`` program might do.
It spans multiple lines, so that we can test things reasonably.
"""

bar_help = "Take output from foo subprogram and run it through the ``bar`` subprogram"
bar_desc = """This is the long description for the ``bar`` subprogram."""

def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(title="subcommands",
                                       description="choose one of the following:",
                                       dest="program")
    fooparser = subparsers.add_parser("foo",
                                      help=foo_help,
                                      description=foo_desc)
    barparser = subparsers.add_parser("bar",
                                      help=bar_help,
                                      description=bar_desc)
    
    fooparser.add_argument("fooarg1",help="foo argument 1")
    fooparser.add_argument("fooarg2",help="foo argument 2")
    fooparser.add_argument("-f",help="short foo argument",type=str)
    fooparser.add_argument("--fookwarg",help="foo keyword argument",type=str)
    fooparser.add_argument("-v","--verbose",help="foo verbosely")

    barparser.add_argument("bararg",help="bar argument")
    barparser.add_argument("--choice",choices=("option1","option2","option3"),
                           help="A keyword that requries a choice")

    args = parser.parse_args(argv)
 
if __name__ == "__main__":
    main()
