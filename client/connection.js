import * as APP from './app.js';

const socket = new WebSocket('ws://localhost:9002');

socket.onopen = function(event) {
    console.log('WebSocket connection established.');

    const message = {
      type: "conreq"
    };
    socket.send(JSON.stringify(message));
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
        case 'updateData':
          updateData(message);
          break;
        case 'removeData':
          removeData(message);
          break;
        default:
          console.error('Unknown message type:', message.type);
      }
    } else {
      console.error('Invalid message:', message);
    }
};

export function sendInput(input)
{
  const message = {
    type: "input",
    input: {
      sledge: input.sledge,
      move: input.move,
      jump: input.jump
    }
  };

  if (socket.readyState === WebSocket.OPEN) {
    socket.send(JSON.stringify(message));
  }
}

function removeData(message)
{
    APP.removeData(message);
}

function updateData(message)
{
    console.log(message);
    APP.updateData(message);
}