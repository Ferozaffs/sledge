import { connectToServer } from "./connection.js";

function createRoom() {
  fetch("http://" + window.location.hostname + ":8080/create_room")
    .then((response) => {
      if (!response.ok) {
        throw new Error("Error: " + response.statusText);
      }
      return response.json();
    })
    .then((data) => {
      console.log(data);
      connectToServer(window.location.hostname + ":" + data.port);
    })
    .catch((error) => console.error("Error: ", error));
}

window.createRoom = createRoom;
