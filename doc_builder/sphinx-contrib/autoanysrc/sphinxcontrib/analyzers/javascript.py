# -*- coding: utf-8 -*-
from .base import BaseCommentAnalyzer


class JSAnalyzer(BaseCommentAnalyzer):
    """JavaScript anaylzer.

    Will grab documentations from comments block started '/*\"\"\"'
    and ended by '*/'
    """

    # sphinx domain for .. default-domain:: directive
    domain = 'js'

    comment_starts_with = '/*"""'
    comment_ends_with = '*/'
