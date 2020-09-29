// global variables
var socket = io();

jQuery(function($){
    function update_time(){
        socket.emit('get_time');
    };
    setInterval(update_time, 1000);

    socket.on('update_time', function(response){
        $("#clock").html(response);    
    });
});

