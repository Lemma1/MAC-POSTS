#!/usr/bin/env python
"""Long-named subcommands, with main program arguments

In this test case, we test a parser that has its own arguments as well as
multiple subcommands, one of which has an exceedingly long name. This is 
mostly to test column alignment.

We also added a bunch of paragraphs and rich formatting to show how
`reStructuredText`_ markup survives post-processing by
:obj:`sphinxcontrib.argdoc`.

----------------

Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean feugiat
tempor diam sed condimentum. Mauris aliquam interdum libero, ut aliquet
erat malesuada sed. Mauris nec venenatis sapien, a feugiat neque. Sed
pulvinar erat sit amet posuere aliquet. Phasellus non quam tincidunt,
semper velit vitae, eleifend ante. Nam finibus vulputate diam. Fusce sit
amet leo aliquam magna gravida fringilla et eu justo. Pellentesque vulputate
elit id dignissim vehicula. Sed tempor interdum lacus, in dapibus magna
interdum eu. Fusce lacinia turpis vel risus porta, eget dapibus nisi
eleifend. Maecenas dictum nec nisi sit amet dignissim. Duis vestibulum
ipsum a vestibulum placerat. Vestibulum ante ipsum primis in faucibus orci
luctus et ultrices posuere cubilia Curae; Nullam consequat nulla quis quam
interdum, eu auctor ante molestie.

Cum sociis natoque penatibus et magnis dis parturient montes, nascetur
ridiculus mus. Ut egestas nec leo a luctus. Suspendisse libero magna,
ultricies vel porttitor varius, vulputate nec orci. Ut et vehicula neque.
Quisque ut libero eget sem pretium mollis elementum vitae quam. Etiam varius
rutrum iaculis. Mauris consectetur cursus dolor nec tincidunt. Morbi aliquam
elit ipsum, at aliquam purus ultricies sed. Donec tortor ante, consectetur
et faucibus non, dignissim vitae eros. Duis pharetra convallis efficitur.
Curabitur congue in tortor luctus molestie. Donec turpis felis, sollicitudin
volutpat tristique quis, mattis at arcu. Praesent interdum luctus sodales.
Sed imperdiet augue vulputate hendrerit tincidunt. Curabitur pharetra, odio
in laoreet pretium, metus turpis posuere dui, quis aliquet leo nisl
sollicitudin ligula.

Here is a table, to show that we can have rich formatting:

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
    a number of reStructuredText structures, and still have the argument
    descriptions appended below.

Here is another item
    To show that our test works
"""
import argparse
import sys

foo_help = "Run the ``foo`` subprogram"
foo_desc = """This is a long description of what a ``foo`` program might do.
It spans multiple lines, so that we can test things reasonably.
"""

bar_help = "Take output from ``foo`` subprogram and run it through the ``bar`` subprogram"
bar_desc = """This is the long description for the ``bar`` subprogram."""

def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser()
    parser.add_argument("mainarg1")
    parser.add_argument("mainarg2",help="main positional argument #2, which we wrote helptext for (there was no help for ``mainarg1``)")
    subparsers = parser.add_subparsers(title="subcommands",
                                       description="choose one of the following:",
                                       dest="program")
    fooparser = subparsers.add_parser("foo",
                                      help=foo_help,
                                      description=foo_desc)
    barparser = subparsers.add_parser("barbarbarbarbaraaeadslfjasdlkfjljalksjflsjdfladjfklasdjkfladsjglkjdasl",
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
    
    bgroup = barparser.add_argument_group(title="An argument group",
                                          description="A special goup of arguments in the ``bar`` subparser")
    bgroup.add_argument("--b1")
    bgroup.add_argument("--b2",help="Argument 2 has help (bar argument 1 did not)")
    bgroup.add_argument("-k",nargs=2,metavar="N",help="Some other argument")

    args = parser.parse_args(argv)
 
if __name__ == "__main__":
    main()
