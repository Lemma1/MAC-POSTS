$(function(){
    $.fuzzyfinder({
        data: DOCUMENTATION_OPTIONS.FIND_ANYTHING_ENTRIES,
        use_cached_hits: DOCUMENTATION_OPTIONS.FIND_ANYTHING_CACHE_HITS,
        url_prefix: DOCUMENTATION_OPTIONS.URL_ROOT,
        width: DOCUMENTATION_OPTIONS.FIND_ANYTHING_WIDTH,
        activate: [ "ctrl_p", "space", /[A-Za-z0-9_"]/ ], // "
        onInit: function() {
            var ff = this;
            $('p.logo a').attr('href', '#').click(function(){
                ff.fuzzyfind();
            });
        }
    });  
});
