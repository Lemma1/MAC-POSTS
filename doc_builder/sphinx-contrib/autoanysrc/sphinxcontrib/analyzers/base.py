# -*- coding: utf-8 -*-


class BaseAnalyzer(object):
    """Base class for all domains analyzers"""

    def __init__(self, documenter):
        self.documenter = documenter

    def process(self, fileconent):
        raise NotImplementedError()


class BaseCommentAnalyzer(BaseAnalyzer):
    """General anaylzer for fishing docs from comment blocks"""

    # this variables must be overrided in subclass
    comment_starts_with = None
    comment_ends_with = None

    def process(self, content):

        if not self.comment_starts_with or not self.comment_ends_with:
            raise RuntimeError(
                'comment_starts_with or comment_ends_with must be defined'
            )

        in_comment_block = False
        comment_block_indent_len = 0

        for lineno, srcline in enumerate(content.split('\n')):

            # remove indent
            line = srcline.lstrip()

            # check block begins
            if not in_comment_block \
                    and line.startswith(self.comment_starts_with):
                in_comment_block = True
                comment_block_indent_len = len(srcline) - len(line)
                continue  # goto next line

            # skip if line is not a docs
            if not in_comment_block:
                continue

            # check blocks ends
            if line.startswith(self.comment_ends_with):
                in_comment_block = False
                yield '', lineno  # empty line in docs
                continue  # goto next line

            # calculate indent
            indent_len = len(srcline) - len(line) - comment_block_indent_len
            if srcline and indent_len:
                indent_char = srcline[0]
                line = indent_char * indent_len + line

            yield line, lineno
