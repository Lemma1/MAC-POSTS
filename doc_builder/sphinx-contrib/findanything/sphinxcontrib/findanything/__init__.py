from docutils.writers.docutils_xml import Writer
from docutils.io import DocTreeInput, StringOutput
from docutils.core import Publisher
from docutils.utils import new_document
from sphinx.util.pycompat import any, b
from docutils.readers.doctree import Reader as DoctreeReader
import sys, re, os, json

def env_updated(app, env):
    config = app.builder.config

    doctree = env.get_doctree(config.master_doc)
    from sphinx import addnodes

    toctrees = []
    for toctreenode in doctree.traverse(addnodes.toctree):
        toctree = env.resolve_toctree(config.master_doc, app.builder, 
            toctreenode, prune = False, includehidden = True, maxdepth = 0,
            collapse = False)
        toctrees.append(toctree)

    if not toctrees:
        toc = None
    else:
        toc = toctrees[0]
        for toctree in toctrees[1:]:
            toc.extend(toctree.children)

    # toc = env.get_toctree_for(config.master_doc, app.builder, False)

    node = toc

    doc = new_document(b('<partial node>'))
    doc.append(node)

    pub = Publisher(
            source_class = DocTreeInput,
            destination_class=StringOutput)

    pub.set_components('standalone', 'restructuredtext', 'pseudoxml')

    pub.reader = DoctreeReader()
    pub.writer = Writer()
    pub.writer.format = 'pseudoxml'
    pub.process_programmatic_settings(
        None, {'output_encoding': 'unicode'}, None)
    pub.set_source(doc, None)
    pub.set_destination(None, None)
    pub.publish()

    import xml.etree.cElementTree as ET
    from cStringIO import StringIO

    #out = re.sub(r'^<!DOCTYPE[^>]*>\s*', '<?xml version="1.0"?>', pub.writer.output)
    out = pub.writer.output.encode('utf-8').replace(' encoding="unicode"', ' encoding="utf-8"')

    #pprint.pprint(out)

    doctree = ET.fromstring(out)

    #pprint.pprint(doctree)
    #pprint.pprint(dir(doctree))
    if hasattr(doctree, 'getroot'):
        doctree = doctree.getroot()
    #root = doctree.getroot()

    fuzzy_find_entries = []
    docs = {}

    def indexentries(entry, links, cap = ''):
        if links:
            fuzzy_find_entries.append(dict(
                href = links[0][1],
                name = entry,
                path = "index/%s/%s" % (char,cap),
                info = "INDEX",
                detail = '',
            ))

            for i, (ismain, link) in enumerate(links[1:], start=1):
                doclink = link.split('#', 1)[0]
                docname = docs.get(doclink, i)
                fuzzy_find_entries.append(dict(
                    href = link,
                    name = "%s (%s)" % (entry, docname),
                    path = "index/%s/%s" % (char, cap),
                    info = "INDEX",
                    detail = '',
                ))
            return ''
        else:
            return entry

    if app.config.html_findanything_add_topics:
        if hasattr(doctree, 'iter'):
            references = doctree.iter('reference')
        else:
            references = doctree.getiterator('reference')

    #            fuzzy_find_entries.append(dict(
    #                    href = link,
    #                    name = entry,
    #                    path = "index/%s/" % char,
    #                    info = "INDEX",
    #                    detail = '',
    #                ))

        refset = set()

        for ref in references:
            refuri = ref.attrib['refuri']

            docs[refuri] = ref.text
            path = "/"
            if "/" in refuri:
                path = refuri.rsplit('/', 1)[0]+"/"

            if '#' in refuri:
                docname = docs.get(refuri.split('#', 1)[0])
                if docname:
                    path += docname + "/"
                info = 'SECTION'
            else:
                info = 'PAGE'

            e = dict(
                    href = ref.attrib['refuri'],
                    name = ref.text,
                    info = info,
                    path = path,
                    detail = '',
                )

            refid = "%(href)s^%(path)s^%(name)s^%(info)s" % e

            if refid in refset: continue

            refset.add(refid)

            fuzzy_find_entries.append(e)


    if app.config.html_findanything_add_indexentries:
        genindex = env.create_index(app.builder)

        for char,char_list in genindex:
            for entry, (links, subitems) in char_list:
                cap = indexentries(entry, links)
                if subitems:
                    if cap: cap += '/'
                    for subname, sublinks in subitems:
                        indexentries(subname, sublinks, cap=cap)
    
    s = json.dumps(fuzzy_find_entries)

    static_dir = os.path.join(app.builder.outdir, '_static')

    if not os.path.exists(static_dir):
        os.makedirs(static_dir)

    with open(os.path.join(static_dir, 'fuzzyindex.js'), 'wb') as f:
        f.write("DOCUMENTATION_OPTIONS.FIND_ANYTHING_ENTRIES = %s;" % s);


        if app.config.html_findanything_use_cached_hits:
            f.write("DOCUMENTATION_OPTIONS.FIND_ANYTHING_USE_CACHED_HITS = true;")

        f.write("DOCUMENTATION_OPTIONS.FIND_ANYTHING_WIDTH = '%s';" 
            % app.config.html_findanything_width);

#       {
#            href: "foo.html",  // href to open on selection
#            name: "Foo",       // caption of the entry (main subject of search)
#            info: "ALT+F",     // some info e.g. shortcut key to get this function or whatever
#            path: "bar/glork", // path in subject of search, something like a namespace
#            detail: ""         // some detail for display
#        }


def on_builder_inited(app):
    app.config.html_static_path.append( os.path.relpath(
          os.path.join(os.path.dirname(__file__), 'javascript'),
          app.confdir
          ))
    app.config.html_static_path.append( os.path.relpath(
          os.path.join(os.path.dirname(__file__), 'css'),
          app.confdir
          ))

    app.add_javascript('jquery.cursor.min.js')
    app.add_javascript('jquery.keymap.min.js',)
    app.add_javascript('jquery.simplemodal.1.4.3.min.js')

    if app.config.html_findanything_hit_cacher == 'cookie':
        app.add_javascript('jquery.cookie.js')

    app.add_javascript('jquery.fuzzyfind.js')
    app.add_javascript('fuzzyindex.js')
    app.add_javascript('fuzzyfinder.js')

    if app.config.html_findanything_css:
        app.add_stylesheet(app.config.html_findanything_css)


def setup(app):
    app.add_config_value('html_findanything_css', 'findanything.css', False)
    app.add_config_value('html_findanything_width', '400px', False)
    app.add_config_value('html_findanything_add_indexentries', True, False)
    app.add_config_value('html_findanything_add_topics', True, False)
    app.add_config_value('html_findanything_use_cached_hits', True, False)
    app.add_config_value('html_findanything_hit_cacher', 'cookie', False)

    app.connect('builder-inited', on_builder_inited)
    app.connect('env-updated', env_updated)


#if __name__ == '__main__':
    #sys.stdout.write("hello world\n")
