const socket = new WebSocket('ws://localhost:9002');

socket.onopen = function(event) {
    console.log('WebSocket connection established.');

    const message = "conreq";
    socket.send(message);
};

socket.onclose = function(event) {
    console.log('WebSocket connection closed.');
};

socket.onmessage = function(event) {
    const messagesDiv = document.getElementById('messages');
    messagesDiv.innerHTML += '<p>Received message: ' + event.data + '</p>';
};