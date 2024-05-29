package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/url"

	"github.com/gorilla/websocket"
)

type roomResponse struct {
	Status string `json:"status"`
}

var activeRooms = []string{}

func CreateRoom() (string, error) {
	roomName := GetRandomName()

	//cmd := exec.Command("docker-compose", "-f", "../docker-compose.yml", "-p", roomName, "up", "-d")
	//
	//_, err := cmd.CombinedOutput()
	//if err != nil {
	//	fmt.Println("Error:", err)
	//	return "", errors.New("failed to start server")
	//}

	activeRooms = append(activeRooms, roomName)
	return roomName, nil
}

func UpdateRooms() {
	for _, room := range activeRooms {
		status := CheckRoomHealth(room)

		if status == 0 {
			fmt.Println("server up")
		}
	}
}

func CheckRoomHealth(room string) int {
	u := url.URL{Scheme: "ws", Host: GetRoomUrl(room), Path: ""}
	fmt.Println("Connecting to:", room, u.String())

	c, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		log.Println("dial:", err)
		return 1
	}
	defer c.Close()

	requestMessage := "statusreq"

	err = c.WriteMessage(websocket.TextMessage, []byte(requestMessage))
	if err != nil {
		log.Println("write:", err)
		return 1
	}

	_, message, err := c.ReadMessage()
	if err != nil {
		log.Println("read:", err)
		return 1
	}

	var status roomResponse
	err = json.Unmarshal(message, &status)
	if err != nil {
		log.Println("json unmarshal:", err)
		return 1
	}

	if status.Status == "good" {
		return 0
	} else {
		return 1
	}
}

func GetRoomUrl(room string) string {
	return "localhost:9002"
}
