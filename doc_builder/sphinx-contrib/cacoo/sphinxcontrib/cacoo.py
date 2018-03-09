import re
import json
from time import mktime
from urllib2 import urlopen
from email.utils import parsedate
from sphinxcontrib.imagehelper import (
    ImageConverter, add_image_type, add_image_directive, add_figure_directive
)


def cacoo_url_to_diagramid(url):
    return re.sub('https://cacoo\.com/diagrams/', '', url)


class Cacoo(object):
    def __init__(self, apikey):
        self.apikey = apikey

    def get_image_info(self, diagramid):
        URLBASE = "https://cacoo.com/api/v1/diagrams/%s.json?apiKey=%s"
        diagramid = re.sub('[#-].*', '', diagramid)  # remove sheetid
        url = URLBASE % (diagramid, self.apikey)
        return json.loads(urlopen(url).read())

    def get_last_modified(self, diagramid):
        image_info = self.get_image_info(diagramid)
        return mktime(parsedate(image_info['updated']))

    def get_image(self, diagramid):
        URLBASE = "https://cacoo.com/api/v1/diagrams/%s.png?apiKey=%s"
        diagramid = diagramid.replace('#', '-')
        url = URLBASE % (diagramid, self.apikey)
        return urlopen(url)


class CacooConverter(ImageConverter):
    @property
    def apikey(self):
        return self.app.config.cacoo_apikey

    def get_last_modified_for(self, node):
        try:
            diagramid = cacoo_url_to_diagramid(node['uri'])
            return Cacoo(self.apikey).get_last_modified(diagramid)
        except Exception:
            self.warn('Fail to download cacoo image: %s (check your cacoo_apikey or diagramid)' % node['uri'])
            return None

    def get_filename_for(self, node):
        diagramid = cacoo_url_to_diagramid(node['uri'])
        return "cacoo-%s.png" % diagramid.replace('#', '-')

    def convert(self, node, filename, to):
        try:
            cacoo = Cacoo(self.apikey)
            with open(to, 'wb') as fd:
                diagramid = cacoo_url_to_diagramid(node['uri'])
                fd.write(cacoo.get_image(diagramid).read())
                return True
        except Exception:
            self.warn('Fail to download cacoo image: %s (check your cacoo_apikey or diagramid)' % node['uri'])
            return False


def setup(app):
    add_image_type(app, 'cacoo', 'https://cacoo.com/', CacooConverter)
    add_image_directive(app, 'cacoo')
    add_figure_directive(app, 'cacoo')

    app.add_config_value('cacoo_apikey', None, 'html')
