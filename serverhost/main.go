package main

import (
	"encoding/json"
	"net/http"
	"time"
)

type response struct {
	Status        string `json:"status"`
	ContainerName string `json:"container_name,omitempty"`
	Message       string `json:"message,omitempty"`
}

func CreateRoomHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")

	roomName, err := CreateRoom()

	if err != nil {
		json.NewEncoder(w).Encode(response{Status: "error", Message: err.Error()})
	}

	json.NewEncoder(w).Encode(response{
		Status:        "success",
		ContainerName: roomName,
	})
}

func main() {
	http.HandleFunc("/create_room", CreateRoomHandler)
	go func() {
		http.ListenAndServe(":8080", nil)
	}()

	ticker := time.NewTicker(2 * time.Minute)
	defer ticker.Stop()

	for range ticker.C {
		UpdateRooms()
	}
}
