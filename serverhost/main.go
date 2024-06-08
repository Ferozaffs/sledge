package main

import (
	"encoding/json"
	"net/http"
	"time"
)

type response struct {
	Status  string `json:"status"`
	Room    string `json:"room,omitempty"`
	Port    int    `json:"port,omitempty"`
	Message string `json:"message,omitempty"`
}

func CreateRoomHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.Header().Set("Content-Type", "application/json")

	room, err := CreateRoom()

	if err != nil {
		json.NewEncoder(w).Encode(response{Status: "error", Message: err.Error()})
		return
	}

	json.NewEncoder(w).Encode(response{
		Status:  "success",
		Room:    room.Name,
		Port:    room.Port,
		Message: "Room is now ready!",
	})
}

func RoomHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.Header().Set("Content-Type", "application/json")

	err := r.ParseForm()

	if err != nil {
		http.Error(w, "Failed to parse request", http.StatusBadRequest)
		return
	}

	name := r.Form.Get("name")

	port, err := GetRoomPort(name)

	if err != nil {
		http.Error(w, "Room not found", http.StatusBadRequest)
		return
	}

	json.NewEncoder(w).Encode(response{
		Status:  "success",
		Room:    name,
		Port:    port,
		Message: "Room found!",
	})

}

func main() {
	http.HandleFunc("/create_room", CreateRoomHandler)
	http.HandleFunc("/room", RoomHandler)
	go func() {
		http.ListenAndServe(":5501", nil)
	}()

	ticker := time.NewTicker(2 * time.Minute)
	defer ticker.Stop()

	for range ticker.C {
		UpdateRooms()
	}
}
