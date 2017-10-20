# -*- coding: utf-8 -*-
import os
import glob
import codecs
import importlib

from sphinx.util.console import bold
from sphinx.ext.autodoc import Documenter

from . import analyzers


class AnySrcDocumenter(Documenter):
    """
    Specialized Documenter subclass for any source files
    """
    objtype = 'anysrc'
    content_indent = u''
    titles_allowed = True

    # proxy directive paramenters only
    option_spec = {
        'src': lambda x: x,
        'analyzer': lambda x: x,
    }

    # default analyzers
    analyzer_by_key = {
        'js': analyzers.JSAnalyzer,
    }

    @classmethod
    def can_document_member(cls, *args, **kwargs):
        return False  # stop documenters chain

    @classmethod
    def register_analyzer(cls, key, analyzer_class):
        """Register analyzer

        :param domain: one of sphinx domain (js, c, cpp, etc.)
        :param anaylyzer_class: subclass of BaseAnalyzer
        """
        cls.analyzer_by_key[key] = analyzer_class

    @classmethod
    def setup_analyzers(cls, config):

        def import_class(path):
            parts = path.split('.')
            module, class_name = parts[:-1], parts[-1]
            return getattr(
                importlib.import_module('.'.join(module)),
                class_name
            )

        custom_analyzers = config.autoanysrc_analyzers or {}
        for key, value in custom_analyzers.items():
            cls.register_analyzer(key, import_class(value))

    def info(self, msg):
        self.directive.env.app.info('    <autoanysrc> %s' % msg)

    def collect_files(self):
        arg = self.options.src
        if arg is None:
            return []
        if not os.path.isabs(arg):
            arg = os.path.join(
                self.directive.env.srcdir,
                arg,
            )
        self.info('collect by: ' + bold(arg))
        return glob.glob(arg)

    def process(self):
        """process files one by one with analyzer"""

        for filepath in self.collect_files():

            self.info(
                '%s processing: ' % self.analyzer.__class__.__name__
                + bold(filepath)
            )
            self.directive.env.note_dependency(filepath)

            with codecs.open(filepath, 'r', 'utf-8') as f:
                content = f.read()

            for line, lineno in self.analyzer.process(content):
                self.add_line(line, filepath, lineno)

    def generate(
            self, more_content=None, real_modname=None,
            check_module=False, all_members=False):

        # initialize analyzer
        analyzer_class = AnySrcDocumenter.analyzer_by_key.get(
            self.options.analyzer
        )
        if not analyzer_class:
            self.info(
                'Analyzer not defined for: %s' % self.options.anaylyzer
            )
            return
        self.analyzer = analyzer_class(self)

        # start generate docs
        self.add_line('', '<autoanysrc>')

        # set default domain if it specified in anaylyzer instance
        if hasattr(self.analyzer, 'domain'):
            self.add_line(
                '.. default-domain:: %s' % self.analyzer.domain, '<autoanysrc>'
            )
            self.add_line('', '<autoanysrc>')

        # inject docs from sources
        self.process()


def setup_custom_analyzers(app):
    # setup custom analyzers from config
    AnySrcDocumenter.setup_analyzers(app.builder.config)


def setup(app):
    app.add_config_value('autoanysrc_analyzers', None, False)
    app.add_autodocumenter(AnySrcDocumenter)
    app.connect('builder-inited', setup_custom_analyzers)
    return {
        'version': '0.0.0',  # where docs?
        'parallel_read_safe': True,
    }
