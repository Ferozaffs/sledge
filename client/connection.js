import { APPTYPE } from "./constants.js";
import { enableJoysticks } from "./joystick.js";
import { readUInt32 } from "./bytereader.js";
import * as APP from "./app.js";

let socket;

if (APPTYPE === "STANDALONE") {
  connectToServer("", window.location.hostname + ":9002");
}

export function connectToServer(rn, url) {
  socket = new WebSocket("ws://" + url);
  window.addEventListener("beforeunload", closeWebSocket);
  window.addEventListener("unload", closeWebSocket);

  socket.onopen = function (event) {
    console.log("WebSocket connection established.");

    //Disable main menu
    var div = document.getElementById("mainMenu");
    div.style.display = "none";

    //Display room name
    div = document.getElementById("roomName");
    div.textContent = rn;

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
    const reader = new FileReader();
    reader.onload = function () {
      const bytes = new Uint8Array(reader.result);

      let type = bytes[0];
      switch (type) {
        case 0:
          switch (bytes[5]) {
            case 0:
              console.log("SERVER - Unknown request");
              break;
            default:
              console.log("SERVER - Unknown");
          }
          break;
        case 1:
          switch (bytes[5]) {
            case 0:
              console.log("SERVER - Empty");
              break;
            case 0:
              console.log("SERVER - Good");
              break;
            case 0:
              console.log("SERVER - PendingConnection");
              break;
            case 0:
              console.log("SERVER - ConnectionEstablished");
              break;
            default:
              console.log("SERVER - Unknown");
          }
          console.log("SERVER - " + message.status);
          break;
        case 2:
          let assetSize = readUInt32(bytes, 1);
          assetSize -= 1;
          let command = bytes[5];
          switch (command) {
            case 0:
              createAsset(bytes, 6, assetSize);
              break;
            case 1:
              removeAsset(bytes, 6, assetSize);
              break;
            case 2:
              updateAsset(bytes, 6, assetSize);
              break;
            default:
              console.error("Unknown asset command:", command);
          }
          break;
        case 3:
          let scoreSize = readUInt32(bytes, 1);
          updateScore(bytes, 5, scoreSize);
          break;
        default:
          console.error("Unknown message type:", type);
      }
    };
    reader.readAsArrayBuffer(event.data);
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

function createAsset(bytes, offset, size) {
  APP.createAsset(bytes, offset, size);
}

function removeAsset(bytes, offset, size) {
  APP.removeAsset(bytes, offset, size);
}

function updateAsset(bytes, offset, size) {
  APP.updateAsset(bytes, offset, size);
}

function updateScore(bytes, offset, size) {
  APP.updateScore(bytes, offset, size);
}
