/*! Dual-licensed under MIT and GPL. Homepage: http://quelltexter.org/fuzzyfind.html */

/*=
Fuzzy Find
==========

.. highlight:: js

Overview
--------

Fuzzy Find JQuery Plugin provides an easy to use Fuzzy Searcher
in a given set of terms.

Please go to ... for a demonstration.

Depends on jquery.cursor.js and jquery.simplemodal.1.4.3.min.js


License
-------

Copyright (c) 2012-2013 Kay-Uwe 'Kiwi' Lorenz <kiwi@franka.dyndns.org>

Dual licensed under the MIT and GPL licenses:
 * http://www.opensource.org/licenses/mit-license.php
 * http://www.gnu.org/licenses/gpl.html


Reference
---------

=*/
;(function($){

/*=

.. js:function:: JQuery.fn.fuzzyfinder()

    ::
        $(selector).(opts, ...)

    Each selected element may have a correspondent option record. If there are
    selected more elements than options each extra element gets the last option
    record for options.

    *selector* is an input element
        In this case there is needed a target selector im.

    *selector* is a non-input element
        Selected element will be container for fuzzyfind UI. 

    .. js:attribute:: opts.target

        If *selector* is an input element, this selects the element, where the
        output of fuzzyfinder is inserted.


::
    $(selector).fuzzyfinder({
        target: '#my-results',
        data: "url", // data: function(){}; data: [ {href: ..., name: ..., shortcut: ...}]
        data: $('a.foo')
    });

if selector is an input:
   container form will be overridden:
   * onsubmit and ...

if selector is another element:
   within this division there will be created our ui div. (no target needed)


=*/

    $.fn.fuzzyfinder = function() {
        var argv = arguments;

        this.each(function(i){
            var opts = $.extend({}, (i < argv.length) ? argv[i] : argv[argv.length-1]);

            if ($(this).is('input')) {
                var i = _fuzzyId();

                opts.$input   = $(this);
                opts.$results = $(opts.target);
                opts.$form    = $(this).parents('form').eq(0);
                if (!opts.submit_selector) {
                    opts.$form.append('<input type="hidden" id="fuzzy-finder-submit-'+i+'" name="submitted" value="">');
                    opts.submit_selector = '#fuzzy-finder-submit-'+i;
                }
                opts.$submit  = $(opts.submit_selector);
            }

            else { // `this` is considered beeing a container for fuzzysearcher
                var x = _fuzzyFindUi;
                this.append(x.html);

                $.extend(opts, x);
            }

            this.fuzzyFinderObject = new FuzzyFinder(opts);
        });

        return this;
    };

/*=

.. js:function:: JQuery.fuzzyfinder(opts, data)

    This will open a fuzzy-finder window.

    Create a new fuzzy-finder with given options and data.  Data (if present)
    is expected to be an Array of something, like::

        {
            href: "foo.html",  // href to open on selection
            name: "Foo",       // caption of the entry (main subject of search)
            info: "ALT+F",     // some info e.g. shortcut key to get this function or whatever
            path: "bar/glork", // path in subject of search, something like a namespace
            detail: ""         // some detail for display
        }

    There are following options possible:

    .. js:attribute:: opts.activate

        Keys used to activate fuzzyfind window. Can be a regex, or a keyspec.

    .. js:attribute:: opts.keepKey

        Shall activation key be passed to input field.  Default is *true*.

    .. js:attribute:: opts.delay

        How long shall be waited before start searching.  Default is *200ms*.

    .. js:attribute:: opts.maxResults

        How many results shall be displayed at a time.  Default is *10*.

    .. js:attribute:: opts.onSubmit

        What shall be done on submitting the input field form.

    .. js:attribute:: opts.data

        Data.


    Example (fuzzyfind links of current document)::

        // setup data array
        var data = [];
        $('a').each(function(){
            data.push({
                name: $(this).text(),
                href: $(this).attr('href'),
                path: $(this).attr('href').replace(/[^\/]*$/, '')
            });
        });

        // launch fuzzy finder
        $.fuzzyfinder({
            data: data, 
            activate: [ "ctrl_p", "space", /[A-Za-z0-9_]/ ]
        });  

=*/

    $.fuzzyfinder = function(opts, data) {
        //var data = (data instanceof FuzzyData) ? data : new FuzzyData(data, opts);

        var opts = $.extend({
            activate: /[A-Za-z0-9_\-"]/,

            keepKey: true,
            autoFocus: true,
            delay: 200,
            maxResults: 10,
            use_cached_hits: true,
            hit_cacher: 'cookie',
            width: '400px',
            submit_mimic_anchor: true,
            url_prefix: '',
            aging: [
//                60*60*1000,         // 6
                2*60*60*1000,      // 5
                12*60*60*1000,     // 4
                2*24*60*60*1000,   // maxentries 3
                7*24*60*60*1000,   // maxentries 2 with date > this
                30*24*60*60*1000  // maxentries 1 with date > this
                ],
            onSubmit: function(val){
                this.submit(val);
                $.modal.close();
            }
        }, opts);

        $.extend(opts, _fuzzyFindUi());


        var theCharacter;

        // setup model
        opts.modal = $.extend({
            overlayClose: true,
            autoPosition: true,
            autoResize: true,

            onShow: function(dialog) {
                ff = new FuzzyFinder(opts, data);

                // this.update(ff.$results.height(), 400);
                
                dialog.ff = ff;

                if (theCharacter.length)
                    ff.$input.val(theCharacter);

                ff.$input.focus();
                ff.$input.setCursorPosition(ff.$input.val().length)

                if ($(document.activeElement).attr('id') != ff.$input.attr('id')) {
                    ff.$input.focus();
                }

                ff.$form.submit();

                _loadingFuzzyFinder = false;
            },

            onClose: function(dialog) {
                $.modal.close();
                dialog.ff.$ui.remove();
            }

        }, opts.modal);

        var ff;

        // setup keymap
        var keymap = {};

        var user_default = null;
/*
        if (opts.keymap) {
            if ('default' in opts.keymap) {
                user_default = opts.keymap.default;
            }
        }
        $.extend(keymap, opts.keymap);
*/
        var _fuzzyfind = function(char) {
            _loadingFuzzyFinder = true;
            theCharacter = char || '';
            $.modal(opts.html, opts.modal);
        };

        opts.fuzzyfind = _fuzzyfind;

        function keypressDefault(e) {
            var charCode = e.which || e.charcode;
            if (!charCode) return false;

            var char = String.fromCharCode(charCode);

            var modifier = e.ctrlKey || e.altKey ;

            // make sure case is set correctly
            char = char.toLowerCase();
            if (e.shiftKey) {
                char = char.toUpperCase();
            }
    
            if (modifier) return false;

            var result = false;

            $.each(opts.activate, function(){
                if (!(this instanceof RegExp))
                    return true;

                if (this.test(char)) {
                    _fuzzyfind(char);
                    result = true;
                    return false;
                }
            });

            return result;
        }

        $.each(opts.activate, function(){
            if (this instanceof RegExp) {
                keymap.default = keypressDefault;
            }

            keymap[this] = function(e) { _fuzzyfind(); return true; }
        });

        $('body').keymap(keymap, {includeTargets: 'body', preventDefault: true, stopPropagation: true});

        if (opts.onInit) {
            opts.onInit.call(opts);
        };

    };

    // find in set using fuzzyfinder
    $.fn.fuzzyfind = function(term, opts){
        $.fuzzygrep(term, this, opts);
    };


/*=

.. js:function:: $.fuzzygrep(term, data[, opts])

   Find ``term`` in ``data``, where data is a jquery object containing
   possible results.  If term matches, item will be displayed, else hidden.
   before displaying, current highlighting will be removed and new highlighing
   will be generated.

   onMatch accepts, entry, pattern, matches, num_match

=*/

    $.fuzzygrep = function(term, data, opts){
        var opts = $.extend({
            match: function(e, p) {
                return p.patternMatch(e);
            },

            highlight: function(e, p) {
                p.patternHighlight(e);
            },

            onMatch: function(e, p) {
                opts.highlight(e, p);
                if ($(e).is(':hidden')) $(e).show();
            },

            onNotMatch: function(e, p) {
                if ($(e).is(':visible')) $(e).hide();
            },

            nameSelector: ':self',
            pathSelector: false,

            getNameElement: function(e) {
                if (!opts.nameSelector) return $();
                if (opts.nameSelector == ':self') return $(e);
                return $(e).find(opts.nameSelector);
            },

            getPathElement: function(e) {
                if (!opts.pathSelector) return $();
                if (opts.pathSelector == ':self') return $(e);
                return $(e).find(opts.pathSelector);
            },

            getName: function(e) {
                return opts.getNameElement(e).text();
            },

            getPath: function(e) {
                return opts.getPathElement(e).text();
            },

            matchName: true,
            matchPath: true,

            highlightShortestMatch: false
        }, opts);
        var fuzzy_pattern = new FuzzyPattern(term, opts);
        var num_matches = 0;

        $.each(data, function(i,v){
            if (opts.maxMatch && num_matches >= opts.maxMatch)
                return false;

            var matches = opts.match(this, fuzzy_pattern);

            if (matches.length) {
                num_matches += 1;

                opts.onMatch(this, fuzzy_pattern, matches, num_matches);

            } else {
                if (opts.onNotMatch) {
                    opts.onNotMatch(this, fuzzy_pattern);
                }
            }
        });
    };

    var _fuzzyFinders = [];
    var _id = 0;
    var _fuzzyId = function() {
    	_id += 1;
    	return _id;
    }

/*=
.. js:function:: _fuzzyFindUi()

   This function creates a unique UI for fuzzy-finding.

   :returns: uispec

       JQuery selectors here are usually simply IDs, because
       selecting by ID is fastest.

       .. js:attribute:: uispec.html

           HTML code for UI.

       .. js:attribute:: uispec.input_selector

           JQuery selector for input element

       .. js:attribute:: uispec.results_selector

           JQuery selector for results container

       .. js:attribute:: uispec.submit_selector

           JQuery selector for input, which toggles if this is
           a data submit or a lookup.

       .. js:attribute:: uispec.form_selector

           JQuery selector for form.

       .. js:attribute:: uispec.ui_selector

           JQuery selector for entire UI, e.g. to remove it
           later.

=*/

    var _fuzzyFindUi = function() {
    	var i = _fuzzyId();

    	return {
    		input_selector:   '#fuzzy-finder-input-'+i,
    		results_selector: '#fuzzy-finder-results-'+i,
    		submit_selector:  '#fuzzy-finder-submit-'+i,
    		form_selector:    '#fuzzy-finder-form-'+i,
    		ui_selector:      '#fuzzy-finder-ui-'+i,
    		html:
         		'<div class="fuzzy-finder" id="fuzzy-finder-ui-'+i+'">'
        		+'<form id="fuzzy-finder-form-'+i+'">'
        		+'<input type="text" autocomplete="off" id="fuzzy-finder-input-'+i+'" name="fuzzy_q" value="">'
        		+'<input type="hidden" name="submitted" id="fuzzy-finder-submit-'+i+'" value="">'
        		+'</form>'
        		+'<div id="fuzzy-finder-results-'+i+'" class="fuzzy-find-results"></div></div>'
    	};
    }

    // this variable is showing if fuzzy finder is loading data 
    // at the moment.
    var _loadingFuzzyFinder = false;

/*=
.. js:class:: FuzzyFinder(opts[, data])

    :param uispec opts: An uispec containing options for FuzzyFinder.

        .. js:attribute:: opts.input_selector

            see :js:attr:`uispec.input_selector`.

        .. js:attribute:: opts.form_selector

            see :js:attr:`uispec.form_selector`.

        ...

        .. js:attribute:: opts.$submit

            jQuery object of submit input element.  This is set if user
            does a real submit after selecting an item using fuzzy search.

            Default is to create it using :js:attr:`opts.submit_selector`.

        .. js:attribute:: opts.$input

            jQuery object of search term input element.  

        ...

        .. js:attribute:: opts.onLoad

        .. js:attribute:: opts.getParam

        .. js:attribute:: opts.searchNameWithPath

           This toggles if name and href shall be searched.  

           * "query" tries to match name. If not matches, it is
             tried to match also path.

        .. js:attribute:: opts.data

            If there is no data given by parameter, opts.data will be taken.

    :param data: Something, which overrides :js:attr:`opts.data`, if
        present.

=*/

    function FuzzyFinder(opts, data) {
    	var data = data ? data : opts.data;
    	var ff = this;
        this.delay = 200;
    	$.extend(this, opts);

    	_fuzzyFinders.push(this);

    	$.each( ['input', 'results', 'form', 'submit', 'ui'], 
    		function(k,v){
    			if (!ff['$'+v]) ff['$'+v] = $(ff[v+'_selector']);
    		});

    	// setup ui
    	ff.$form.submit(function(){
            if (ff.$submit.val()) {
                if (ff.onSubmit) {
                    ff.onSubmit(ff.$input.val());
                } else {
                    ff.submit(ff.$input.val());
                }

                //ff.$ui.remove();
            }
            else {
                if (this.onSearch) {
                    ff.onSearch(ff.$input.val());
                } else {
	                ff.search(ff.$input.val());
                }
            }

            return false;
        });



        this.keymap = $.extend({
            return: function(){ ff.$submit.val("on"); },

            escape: function(){ return; },

            down: function() { 
                var n = ff.$results.find('li.selected');
                n.removeClass('selected');
                n = n.next();
                if (n.length) {
                    n.addClass('selected');
                } else {
                    ff.$results.find('li:first').addClass('selected');
                }
            },

            up: function() {
                var n = ff.$results.find('li.selected');
                n.removeClass('selected');
                n = n.prev();
                if (n.length) {
                    n.addClass('selected');
                } else {
                    ff.$results.find('li:last').addClass('selected');
                }
            },

            tab: function() { this.down(); },

            default: function(e){
                this.fuzzyFinder.triggerUpdate();
            },

            fuzzyFinder: this

        }, opts.keymap);

    	ff.$input.keymap(this.keymap);

    	// initialize data

    	this.data = [];


        if (!this.search) {

        	if (typeof(data) == "string") {  // url

        		// on update of input, there is posted a new ajax request, to
        		// fill results.

        		// this.dataType defaults to 'html', but can be also 'json'

    	    	if (!this.getParam)
    	    		this.getParam = function(term){
                        var x = {};
                        $('input, select', ff.$form).each(function(){
                            var $this = $(this);
                            x[$this.attr('name')] = $this.val();

                        });
                        return x;
                    };
    
    	    	if (!this.onLoad)
    	    		this.onLoad = function(data){
                        this.handleAjaxResponse(data);
    	    		};

                this.data = data;
    
    	    	this.search = this.ajaxSearch;
        	}
        	else if (typeof(data) == "function") {
    
        		
        	}
    
        	else {

                // setup cached_hits
                if (this.use_cached_hits) {
                    if (this.hit_cacher == 'cookie') {
                        $.cookie.json = true;
                        this.cached_hits = $.cookie('findanything_hits');

                        this.cached_hits_last_aged = $.cookie('findanything_last_aged');
                        if (typeof(this.cached_hits_last_aged) == "undefined") {
                            this.cached_hits_last_aged = Date.now();
                        }
                    }
                }

                if (typeof(this.cached_hits) == "undefined")
                    this.cached_hits = {};

                // data is list of {href: ... name: ... info: ...}
                this.max_path_len = 0;
                this.max_name_len = 0;

                for (i in data) {
                    var e = data[i];
                    if (e.name.length > this.max_name_len) {
                        this.max_name_len = e.name.length;
                    }
                    if (e.path.length > this.max_path_len) {
                        this.max_path_len = e.path.length;
                    }
                    e.user_hits = 0;

                    if (this.use_cached_hits) {
                        if (this.cached_hits[e.href]) {
                            // cached hits here

                            // this only for migrating old numbers
                            if (!$.isArray(this.cached_hits[e.href])) {
                                this.cached_hits[e.href] = [];
                                this.cached_hits[e.href].push(Date.now());
                            }

                            // assume we have following entries
                            // 10s, 2min, 2.10min, 5min, 1.5h, 1.7h,
                            // 3h, 4h, 10h, 11.5h, 1d, 1.5d, 2d, 2.2d,
                            // 5d, 7d, 10d, 14d
                            //
                            // 13d
                            //
                            // within last hour: max(entries) =  6
                            // within last 6h:   max(entries) =  5
                            // within last 12h:  max(entries) =  4
                            // within last 1d:  max(entries)  =  3
                            // within last 7d:  max(entries)  =  2
                            // within last 30d:  max(entries) =  1
                            //

                            // age hits here
                            var max_count = this.aging.length;
                            var j = 0,   // iterate over this.aging
                                k = 0,   // iterate over my_hits
                                cnt = 0; // count 
                            var my_hits = this.cached_hits[e.href];

                            var age = Date.now() - this.aging[j];

                            var maxi = 0;
                            for (var j=0; j<this.aging.length; j++) {
                                maxi += j;
                            }

                            if (my_hits.length > maxi) {
                                var new_hits = [];

                                // my_hits is ascending
                                for (var k in my_hits) {
                                    if (my_hits[k] > age) {
                                        if (cnt < max_count) {
                                            new_hits.push(my_hits[k])
                                            cnt += 1;
                                        } else {
                                            j += 1;
                                            cnt = 0;
                                            max_count -= 1;
                                            age = Date.now() - this.aging[j];
                                        }
                                    } else if (new_hits.length < maxi) {
                                        new_hits.push(my_hits[k])
                                    }
                                }
    
                                this.cached_hits[e.href] = new_hits;
                            }
                            e.user_hits = this.cached_hits[e.href].length;
                        }
                    }
                }

                // if maxmatch is used, order of entries is of essence, so
                // sort entries by user_hits descending

                if (this.use_cached_hits) {
                    data.sort(function(a,b){
                        return b.user_hits - a.user_hits;
                    });
                }

                this.search = function(query){
                    var selected = ff.$results.find('.selected').children('a').attr('href');

                    var results = [];

                    $.fuzzygrep(query, data, {
                        getName: function(x){return x.name},
                        getPath: function(x){return x.path || x.href},
                        maxMatch: false,
                        nameSelector: '.name',
                        pathSelector: '.path',
                        onMatch: function(d, p, m, i){
                            results.push({entry: d, pattern: p, matches: m, index: i});

                            // /var li = ff.renderResult($ul, d, i);
                            // p.patternHighlight(li);
                        },

                        onNotMatch: null
                    });

                    // path query length and name query length
                    var pql = 0, nql = 0;

                    m = /(.*\/)(.*)/.exec(query);
                    if (m) {
                        pql = m[1].length;
                        nql = m[2].length;
                    }
                    else {
                        pql = 0;
                        nql = query.length;
                    }
                    var logged = 0;
                    var mpl = this.max_path_len;
                    var mnl = this.max_name_len;

                    results.sort( function(a, b) {
                        var _l, i, amin = Infinity, bmin = Infinity;

                        var ma = a.matches;
                        var mb = b.matches;

                        var a_prate = 0, a_nrate = 0, b_prate = 0, b_nrate = 0;

                        if (!b.matches[1]) {
                            var xyz = 1;
                        }

                        var ampl = a.matches[0].length,
                            aepl = a.entry.path.length,
                            amnl = a.matches[1].length,
                            aenl = a.entry.name.length,

                            bmpl = b.matches[0].length,
                            bepl = b.entry.path.length,
                            bmnl = b.matches[1].length,
                            benl = b.entry.name.length;

                        if (0){

                        var a_prate = (!ampl) ? 0 :
                                      (1 - (ampl - pql) / (mpl - aepl) )
                                      * (1 - aepl*0.0001);

                        var a_nrate = (!amnl) ? 0 :
                                      (1 - (amnl - nql) / (mnl - aenl))
                                      * (1 - aenl*0.0001);

                        var b_prate = (!bmpl) ? 0 :
                                      (1 - (bmpl - pql) / (mpl - bepl) )
                                      * (1 - bepl*0.0001);

                        var b_nrate = (!bmnl) ? 0 :
                                      (1 - (bmnl - nql) / (mnl - benl) )
                                      * (1 - benl*0.0001);
                        } else {
                            var a_prate = (!ampl) ? 0 :
                                (
                                    (1 - (ampl - pql)/ampl)*3 + 
                                    (1 - (aepl - ampl)/aepl)*2 +
                                    ( (mpl - aepl)/mpl )*1 
                                ) / 6;

                            var b_prate = (!bmpl) ? 0 :
                                (
                                    (1 - (bmpl - pql)/bmpl)*3 + 
                                    (1 - (bepl - bmpl)/bepl)*2 +
                                    ( (mpl - bepl)/mpl )*1 
                                ) / 6;
                            var a_nrate = (!amnl) ? 0 :
                                (
                                    (1 - (amnl - nql)/amnl)*3 + 
                                    (1 - (aenl - amnl)/aenl)*2 +
                                    ( (mnl - aenl)/mnl )*1 
                                ) / 6;
                            var b_nrate = (!bmnl) ? 0 :
                                (
                                    (1 - (bmnl - nql)/bmnl)*3 + 
                                    (1 - (benl - bmnl)/benl)*2 +
                                    ( (mnl - benl)/mnl )*1 
                                ) / 6;
                        }

                        var a_rate = (a_prate*pql + a_nrate*nql) / (pql+nql);

                        var b_rate = (b_prate*pql + b_nrate*nql) / (pql+nql);

                        // following is for debugging
                        //a.entry.detail = "ahits: "+a.entry.user_hits+", ampl: "+ampl+", pql: "+pql+", mpl: "+mpl+", aepl: "+aepl+"amnl: "+amnl+", nql: "+nql+", mnl: "+mnl+", aenl: "+aenl+", a_prate: "+a_prate+", a_nrate: "+a_nrate+", rate: "+a_rate;
                        //b.entry.detail = "bhits: "+b.entry.user_hits+", bmpl: "+bmpl+", pql: "+pql+", mpl: "+mpl+", bepl: "+bepl+"bmnl: "+bmnl+", nql: "+nql+", mnl: "+mnl+", benl: "+benl+", b_prate: "+b_prate+", b_nrate: "+b_nrate+", rate: "+b_rate;

                        a.entry.rating = (a.entry.user_hits + a_rate).toFixed(4);
                        b.entry.rating = (b.entry.user_hits + b_rate).toFixed(4);

                        return (b.entry.user_hits + b_rate) - (a.entry.user_hits + a_rate);

//                                                                 1 - (ml - ql) / ll        (ml - ql) / (ll - ql)
// q = "CI", in "CI/",                ql = 2, ll = 3,  ml = 2      1 - (2 - 2)  /  3 = 1 * (1-3*0.0001)
// q = "CI", in "Machine Simulation", ql = 2, ll = 18, ml = 14     1 - (14 - 2) / 18 = 1/3 * (1-18*0.0001)
// q = "CI", in "CI Serv",            ql = 2, ll = 7,  ml = 2      1 - (2 - 2)  /  7 = 1 * 0.9993
// q = "CI", in "foobar",             ql = 2, ll = 6,  ml = 0      if ml == 0 :     0
//
// ci/sev ("CI/CI Services Documentation") => P: bmpl: 3, pql: 3, bepl: 3, bmnl: 4, nql: 3, benl: 25
// ci/sev ("CI/CI Services")               => P: bmpl: 3, pql: 3, bepl: 3, bmnl: 4, nql: 3, benl: 25

// bmpl: x, pql: x, bepl: x    => 1
// bmpl: 6, pql: 5, bepl: 20   => 
// bmpl: 6, pql: 5, bepl: 50   => ?
// bmpl: 50, pql: 5, bepl: 50  => ?
// bmpl: 50, pql: 5, bepl: 50  => ?
// bmpl: 45, pql: 40, bepl: 50 => ?
// 18 * 0.0001

// 1 - 0 => 1
// 
// f(a,b,c) : a == b == c => 1
// f(a,b,c) : a - b is small c is small is better
// f(a,b,c) : a - b is small c is big is worse

                    });

                    console.debug(results.length + " matches");

                    var result_html = '<ul></ul>';
                    if (results.length > 1) {
                        result_html = '<p class="num-results">'+results.length+' hits</p><ul></ul>';
                    }

                    ff.$results.html(result_html);
                    var $ul = ff.$results.find('ul');

                    var i;
                    for (i = 0 ; i < results.length; i++) {
                        if (i > 9) break;
                        var li = ff.renderResult($ul, results[i].entry, i, ff.url_prefix);
                        results[i].pattern.patternHighlight(li);
                    }

                    var s = ff.$results.find('a[href="'+selected+'"]');

                    if (s.length) {
                        s = s.parent();
                    } else {
                        s = ff.$results.find('li:first');
                    }

                    s.addClass('selected');

                    $ul.find('a')/*.each(function(){
                        $(this).attr('ref', $(this).attr('href'));
                        $(this).attr('href', 'javascript:void(null);return false');
                    })*/.click(function(){
                        var $a = $(this);
                        if (ff.use_cached_hits) {
                            var href = $a.get(0).findanything_data.href;
                            if (!ff.cached_hits[href])
                                ff.cached_hits[href] = [];

                            ff.cached_hits[href].push(Date.now());

                            if (ff.hit_cacher == 'cookie') {
                                $.cookie.json = true;
                                $.cookie('findanything_hits', ff.cached_hits);
                                $.cookie('findanything_last_aged', this.cached_hits_last_aged);
                            }
                        }
                        return true;
                    });

                    // this must not be here! There must be rather a onSearch...
                    $.modal.update(ff.$ui.outerHeight()+'px', ff.width);
                    //$.modal.update(ff.$ui.height()+'px');
                }
    	    }
        }
    };

    FuzzyFinder.prototype.ajaxSearch = function(term) {
        var ff = this;
        $.post(ff.data, ff.getParam(term), 
            function(data){
                ff.onLoad.call(ff, data); 
            });
    };

    FuzzyFinder.prototype.handleAjaxResponse = function(data) {
        var ff = this;

        if ( (/^\s*\[/.test(data) && /\]\s*$/.test(data)) ||
             (/^\s*\{/.test(data) && /\}\s*$/.test(data)) )
        {
            data = /^\s*\[/.test(data) ? eval(data)[0] : eval('['+data+']')[0];

            var selected = ff.$results.find('.selected').children('a').attr('href');

            var html = '';

            var order = data.preface ? data.preface : ['title', 'hint', 'message'];

            $.each(order, function(){
                if(data[this])
                    html += '<div class="'+this+'">'+data[this]+'</div>';
            });

            html += '<ul></ul>'

            ff.$results.html(html);
            var $ul = ff.$results.find('ul');

            $.each(data.results, function(i,d){
                var li = ff.renderResult($ul, d, i);
//                                p.patternHighlight(li);
            });

            var s = ff.$results.find('a[href="'+selected+'"]');

            if (s.length) {
                s = s.parent()
            } else {
                s = ff.$results.find('li:first');
            }

            s.addClass('selected');

        } else {
            ff.$results.html(data);
        }
    };

    var _updateResults = null;
    FuzzyFinder.prototype.triggerUpdate = function() {
        var ff = this;
        if (_updateResults) {
            clearTimeout(_updateResults);
            _updateResults = null;
        }
        _updateResults = setTimeout(
            function(){ ff.$form.submit() }, 
            ff.delay
            );
    };

    FuzzyFinder.prototype.submit = function(term) {
        var ff = this;
        var $a = ff.$results.find('.selected a');
/*
        if (this.use_cached_hits) {

            var href = $a.eq(0).attr('href');
            if (!this.cached_hits[href])
                this.cached_hits[href] = 0;
            this.chached_hits[href] += 1;

            if (this.hit_cacher == 'cookie') {
                $.cookie.json = true;
                $.cookie('findanything_hits', this.cached_hits);
            }
        }
*/
        // document.location = ff.$results.find('.selected a').attr('href');

        var ev = $a.data('events') || {};
        if ('click' in ev) {
            $a.click();
        }

        if (ff.submit_mimic_anchor) {
            document.location = $a.attr('href');
        }

    };

    FuzzyFinder.prototype.renderResults = function(element, data) {
        var ff = this;
        $(element).append("<ul></ul>")
        var $ul = $(element).find('ul');
        $(data).each(function(i,v){ 
            if (ff.maxResults && ff.maxResults < i) return false;
            ff.renderResult($ul, v, i);
        });
    };

    FuzzyFinder.prototype.renderResult = function(element, data, index, url_prefix) {
        /* {name: ..., href: ..., info: ... } */
        var href = data.href || '';
        var name = data.name || '';
        var info = data.info || '';
        var detail = data.detail || '';
        var rating = data.rating || '';

        if (detail) detail = '<span class="detail">'+detail+'</span>';
        if (rating) rating = '<span class="rating">'+rating+'</span>';

        if (url_prefix) href = url_prefix + href;

        var display_href = data.path || href.replace(/#.*$/, '').replace(/\/[^\/]*$/, '/').replace(/^(\.\.\/)*/, '');

        var cl = (index%2 == 0) ? 'even' : 'odd';

        $('<li class="'+cl+'"><a href="'+href+'">'+
            '<span class="path">'+display_href+
            '</span><span class="name">'+name+
            '</span><span class="info">'+info+
            '</span>'+rating+'</a>'+detail+'</li>').appendTo(element);

        var last = $(element).children(':last-child');
        last.find('a').get(0).findanything_data = data;

        return last;
    };


    // $('...').fuzzyfinder(target, opts) make selected beeing fuzzyfinder inputs, 
    // with target div specified in opts

    var FuzzyPattern = function(term, opts){
    	var m = /(.*\/)(.*)/.exec(term);

    	var _name = "", _path = "";
    	if (m) {
    		_name = m[2]; _path = m[1];
    	}
    	else {
    		_name = term;
    	}

    	var R = function(x){
            var y = x.split('"');
            var result = '';
            if (y.length > 1) {
                for(var i = 0; i < y.length ; i++) {
                    if (!y[i]) continue;

                    if (i%2 == 1) {
                        result += '.*'+y[i];
                    } else {
                        result += '.*'+y[i].split('').join('.*');
                    }
                }
            } else {
                result = '.*'+x.split('').join('.*');
            }
            result = new RegExp(result.replace('.*/', '[^/]*/').replace(/^\.\*/, '.*(') + ")", 'i');

            return result;
    	};

    	var H = function(x){
    		var r = [];
            var p;
            var regexes = [];

            var y = x.split('"');
            var z = [];
            $.each(y, function(i,v){
                if (!v) return true;

                if (i%2 == 1) {
                    z.push(v);
                } else {
                    $.each(v, function(j,c){z.push(c);})
                }
            });

    		$.each(z, function(i,v) {
                if (!opts.highlightShortestMatch) {
                    if (v == '/') {
                        regexes[regexes.length-1] += '([^/]*)(/)';
                    } else {
                        regexes.push('(.*)('+v+')');
                    }
                } else {
                    if (v == '/') {
                        regexes[regexes.length-1] += '([^/]*)(/)';
                    } else {
                        var x = v;
                        if (!/[A-Za-z0-9_]/.test(x)) x = '\\'+x;
                        regexes.push('([^'+x+']*)('+x+')');
                    }

                }
            });
            $.each(regexes, function(i,v){
                r.push(new RegExp(v, 'i'))
            })

            r.reverse();
            return r;
    	}

        this._term_highlight = H(term);
        this._term_match = R(term);
        this._path_part_hl    = this._term_highlight;
        this._path_part_match = null;
        this._name_part_hl    = this._term_highlight;
        this._name_part_match = null;

        //console.debug("_path:", _path, "_name:", _name)

    	if (_path.length) {
    		this._path_part_hl = H(_path);
    		this._path_part_match = R(_path);
    	}

	    if (_name.length) {
	    	this._name_part_hl = H(_name);
	    	this._name_part_match = R(_name);
	    }

    	$.extend(this, opts);
    };

    FuzzyPattern.prototype.patternMatch = function(e){
    	var result = [];
        var m;

    	if (this.matchPath && this.matchName) {
            if (this._path_part_match && this._name_part_match) {
                m = this._path_part_match.exec(this.getPath(e));
                if (m) result.push(m[1]);

                m = this._name_part_match.exec(this.getName(e));
                if (m) result.push(m[1]);

                // have to match both
                if (result.length < 2) return [];
            } else {

                if (this._path_part_match)
                    m = this._path_part_match.exec(this.getPath(e));
                else
                    m = this._term_match.exec(this.getPath(e));

                if (m) result.push(m[1]);

                if (this._name_part_match)
                    m = this._name_part_match.exec(this.getName(e));
                else
                    m = this._term_match.exec(this.getName(e));

                if (m) {
                    if (!result.length) result.push('');
                    result.push(m[1]);
                }
                else if (result.length) {
                    result.push('');
                }
            }
    	}
    	else if(this.matchPath) {
            m = this._term_match.exec(this.getPath(e));
            if (m) {
                result.push(m[1]);
                result.push('');
            }
    	}
    	else if (this.matchName) {
            m = this._term_match.exec(this.getName(e));
            if (m) {
                result.push('');
                result.push(m[1]);
            }
    	}

    	return result;
    };

    var _highlight = function($e, parts) {
        if (!$e.length) return;

   		var txt = $e.text();
   		var html = "";

        var do_highlight = true;

   		$.each(parts, function(i,v){
   			var m = v.exec(txt);
   			if (!m) {
                do_highlight = false;
                return false;
            }

            html = txt.substring(m[0].length) + html

            for (i = m.length-1 ; i > 1 ; i--) {
                if (i % 2 == 0) html = '<b>'+m[i]+'</b>'+html;
                else html = m[i]+html;
            }

            txt = m[1]; //txt.substring(0,m[1].length);
   		});
        if (do_highlight) {
   		   html = txt + html;
   		   $e.html(html);
        }
    }

    FuzzyPattern.prototype.patternHighlight = function(e) {
    	if (this.matchPath && this.matchName) {
    		_highlight(this.getNameElement(e), this._name_part_hl);
    		_highlight(this.getPathElement(e), this._path_part_hl);
    	}
    	else if (this.matchPath) {
    		_highlight(this.getPathElement(e), this._term_highlight);
    	}
    	else if (this.matchName) {
    		_highlight(this.getNameElement(e), this._term_highlight);
    	}
    };

})(jQuery);