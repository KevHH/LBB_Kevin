// global variables
var socket = io();

jQuery(function($){
    /**
     *  Listen to server 
     */

    socket.on('update_age', function(response){
        $("#age").html(response);    
    });

    /**
     *  Talk to server 
     */
    $("#nudge").on("click", function(){
        socket.emit("nudge");
    });

    $("#whisper_submit").on("click", function(){
        socket.emit("whisper", $("#whisper_content").val());
    });
});

