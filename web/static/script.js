// global variables
var socket = io();

jQuery(function($){
    /**
     *  Talk to server 
     */
    $("#nudge").on("click", function(){
        socket.emit("nudge");
    });

    $("#whisper_submit").on("click", function(){
        socket.emit("whisper", $("#whisper_content").val());
    });

    $("#shutdown").on("click", function(){
        socket.emit("shutdown");
    });

    $("#test_submit").on("click", function(){
        socket.emit("test", $("#test_msg").val() + "&" + $("#test_data").val());
    });
});

