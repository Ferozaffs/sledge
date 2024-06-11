import { connectToServer } from "./connection.js";

function createRoom() {
  fetch("http://" + window.location.hostname + ":5501/create_room")
    .then((response) => {
      if (!response.ok) {
        throw new Error("Error: " + response.statusText);
      }
      return response.json();
    })
    .then((data) => {
      console.log(data);
      connectToServer(data.room, window.location.hostname + ":" + data.port);
    })
    .catch((error) => console.error("Error: ", error));
}

function joinRoom() {
  var roomName = document.getElementById("room-input").value;
  fetch("http://" + window.location.hostname + ":5501/room?name=" + roomName)
    .then((response) => {
      if (!response.ok) {
        throw new Error("Error: " + response.statusText);
      }
      return response.json();
    })
    .then((data) => {
      console.log(data);
      connectToServer(roomName, window.location.hostname + ":" + data.port);
    })
    .catch((error) => console.error("Error: ", error));
}

function goJoinRoom() {
  var div = document.getElementById("mainMenu_main");
  div.style.display = "none";

  var div = document.getElementById("mainMenu_back");
  div.style.display = "block";

  var div = document.getElementById("mainMenu_join");
  div.style.display = "flex";
}

function back() {
  var div = document.getElementById("mainMenu_main");
  div.style.display = "flex";

  var div = document.getElementById("mainMenu_back");
  div.style.display = "none";

  var div = document.getElementById("mainMenu_join");
  div.style.display = "none";
}

let button = document.getElementById("backBtn");
button.addEventListener("click", function () {
  back();
});

button = document.getElementById("createRoomBtn");
button.addEventListener("click", function () {
  createRoom();
});

button = document.getElementById("goJoinRoomBtn");
button.addEventListener("click", function () {
  goJoinRoom();
});

button = document.getElementById("joinRoomBtn");
button.addEventListener("click", function () {
  joinRoom();
});
