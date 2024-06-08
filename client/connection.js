import { APPTYPE } from "./constants.js";
import { enableJoysticks } from "./joystick.js";
import * as APP from "./app.js";

let socket;

if (APPTYPE === "STANDALONE") {
  connectToServer(window.location.hostname + ":9002");
}

export function connectToServer(url) {
  socket = new WebSocket("ws://" + url);
  window.addEventListener("beforeunload", closeWebSocket);
  window.addEventListener("unload", closeWebSocket);

  socket.onopen = function (event) {
    console.log("WebSocket connection established.");

    //Disable main menu
    var div = document.getElementById("mainMenu");
    div.style.display = "none";

    enableJoysticks();

    const message = {
      type: "conreq",
    };
    socket.send(JSON.stringify(message));
  };

  socket.onclose = function (event) {
    console.log("WebSocket connection closed.");
  };

  socket.onmessage = function (event) {
    const message = JSON.parse(event.data);

    // Check the type of the message
    if (message && message.type) {
      switch (message.type) {
        case "status":
          console.log("SERVER - " + message.status);
          break;
        case "error":
          console.log("SERVER - " + message.error);
          break;
        case "updateData":
          updateData(message);
          break;
        case "removeData":
          removeData(message);
          break;
        case "scoreData":
          scoreData(message);
          break;
        default:
          console.error("Unknown message type:", message.type);
      }
    } else {
      console.error("Invalid message:", message);
    }
  };
}

function closeWebSocket() {
  if (socket !== undefined) {
    const code = 1000;
    const reason = "Client closing connection";

    if (
      socket.readyState === WebSocket.OPEN ||
      socket.readyState === WebSocket.CONNECTING
    ) {
      socket.close(code, reason);
    }
  }
}

export function sendInput(input) {
  if (socket !== undefined) {
    const message = {
      type: "input",
      input: {
        sledge: input.sledge,
        move: input.move,
        jump: input.jump,
      },
    };

    if (socket.readyState === WebSocket.OPEN) {
      socket.send(JSON.stringify(message));
    }
  }
}

function removeData(message) {
  APP.removeData(message);
}

function updateData(message) {
  APP.updateData(message);
}

function scoreData(message) {
  APP.scoreData(message);
}
