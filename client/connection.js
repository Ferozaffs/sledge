import * as APP from './app.js';

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
    const message = JSON.parse(event.data);

    // Check the type of the message
    if (message && message.type) {
      switch (message.type) {
        case 'status':
          console.log('SERVER - ' + message.status);
          break;
        case 'error':
            console.log('SERVER - ' + message.error);
          break;
        case 'addData':
            console.log(message);
            addData(message);
          break;
        // Add more cases as needed for other message types
        default:
          console.error('Unknown message type:', message.type);
      }
    } else {
      console.error('Invalid message:', message);
    }
};

function addData(message)
{
    APP.addData(message);
}