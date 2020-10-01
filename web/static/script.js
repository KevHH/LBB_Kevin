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

    $("#read_ard").on("click", function(){
        socket.emit("read_ard")
    });

    $("#whisper_submit").on("click", function(){
        socket.emit("whisper", $("#whisper_content").value());
    });
});

