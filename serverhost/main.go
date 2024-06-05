package main

import (
	"encoding/json"
	"net/http"
	"time"
)

type response struct {
	Status  string `json:"status"`
	Room    string `json:"room,omitempty"`
	Url     string `json:"url,omitempty"`
	Message string `json:"message,omitempty"`
}

func CreateRoomHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")

	room, err := CreateRoom()

	if err != nil {
		json.NewEncoder(w).Encode(response{Status: "error", Message: err.Error()})
		return
	}

	json.NewEncoder(w).Encode(response{
		Status: "success",
		Room:   room.Name,
		Url:    room.Url,
	})
}

func RoomHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")

	err := r.ParseForm()

	if err != nil {
		http.Error(w, "Failed to parse request", http.StatusBadRequest)
		return
	}

	name := r.Form.Get("name")

	u, err := GetRoomUrl(name)

	if err != nil {
		http.Error(w, "Room not found", http.StatusBadRequest)
		return
	}

	json.NewEncoder(w).Encode(response{
		Status: "success",
		Room:   name,
		Url:    u,
	})

}

func main() {
	http.HandleFunc("/create_room", CreateRoomHandler)
	http.HandleFunc("/room", RoomHandler)
	go func() {
		http.ListenAndServe(":8080", nil)
	}()

	ticker := time.NewTicker(2 * time.Minute)
	defer ticker.Stop()

	for range ticker.C {
		UpdateRooms()
	}
}
