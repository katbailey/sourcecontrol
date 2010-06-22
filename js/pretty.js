Drupal.behaviors.sourcecontrol = function() {

  var ft   = 300; // Fade time
  var f    = $("#sourcecontrol-push-form");
  var html = "";
  var prog = false; // In progress

  var self = this;

  $("#edit-submit").click(function(){

    params = {}
    f.find("input").each(function(){
      params[$(this).attr("name")] = $(this).val();
    });
    f.find("select").each(function(){
      params[$(this).attr("name")] = $(this).val();
    });

    Drupal.settings.sourcecontrol.prog = true;
    $.post(f.attr("action"), params, function(d,s){
      Drupal.settings.sourcecontrol.prog = false;
    });

    setTimeout(function(){
      f.fadeOut(ft, function(){
        $(this).html("\
          <div id=\"status\">\
            <img src=\"/" + Drupal.settings.sourcecontrol.path + "/img/status.gif\" width=\"220\" height=\"19\" alt=\"Just a status bar\" />\
            <div class=\"log\"></div>\
            <img src=\"/" + Drupal.settings.sourcecontrol.path + "/img/status.gif\" width=\"220\" height=\"19\" alt=\"Just a status bar\" />\
          </div>\
        ");
        f.fadeIn(ft, function(){
          // Start the sync, but wait 2seconds to make sure the record we get is of the started sync
          setTimeout(sourcecontrol_updateStatus,2000);
        });
      });
    },300);

    return false;

  });

}


/**
 *
 * Recursive == awesome
 * Updates the little status message under the pretty bar every second.
 *
 */
function sourcecontrol_updateStatus() {

  $.get("/admin/build/deploy/status", {}, function(html, sts){

    var lg = $("#status .log"); // Because "log" is a keyword
    var pm = $("#persistentmessages").html();

    if (lg.html() != html) {

      lg.html(html);

      // Re-populate persistentmessages if they were nuked
      if (!$("#persistentmessages").html() && pm) {
        $("#persistentmessages").html(pm);
      }

    }

    // Run again if the process is still running
    if (Drupal.settings.sourcecontrol.prog) {
      setTimeout(sourcecontrol_updateStatus, 1000);
    } else {
      $("#status img").remove();
      $("#status").append("<div class=\"notice\">Done!</div>");
    }

  });

}

