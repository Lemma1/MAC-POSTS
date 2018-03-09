/* 
 *
 *
 */
$(function(){
   $('object.swf-zoom-to-fit object').each(function(){
      var e = this;
 
      function JingFix(){
          if (e['Zoom'])
          { setTimeout(function(){ e.Zoom(0); }, 1000); }
          else
          { setTimeout(JingFix, 100); }
      };
      setTimeout(JingFix, 100);
    });
});

