package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"net"
	"net/url"
	"os"
	"os/exec"
	"strconv"

	"github.com/gorilla/websocket"
)

type requestMessage struct {
	Type string `json:"type"`
}

type roomResponse struct {
	Status string `json:"status"`
}

type room struct {
	Name   string
	Port   int
	Health int
}

var activeRooms []room

const roomhealth int = 3

func CreateRoom() (room, error) {
	var l net.Listener
	var err error

	for port := 56000; port <= 57000; port++ {
		l, err = net.Listen("tcp", ":"+strconv.Itoa(port))
		if err == nil {
			break
		}
	}

	port := l.Addr().(*net.TCPAddr).Port
	l.Close()

	roomName := GetRandomName(3)

	os.Setenv("HOST_PORT", strconv.Itoa(port))
	cmd := exec.Command("docker-compose", "-f", "../docker-compose-room.yml", "-p", roomName, "up", "-d")

	_, err = cmd.CombinedOutput()
	if err != nil {
		fmt.Println("Error:", err)
		return room{}, errors.New("failed to start server")
	}

	fmt.Println("Creating room:", roomName)
	room := room{roomName, port, roomhealth}
	activeRooms = append(activeRooms, room)

	port++
	return room, nil
}

func GetRoomPort(name string) (int, error) {
	for _, room := range activeRooms {
		if room.Name == name {
			return room.Port, nil
		}
	}

	return -1, errors.New("room not found")
}

func UpdateRooms() {

	var removal []string
	for idx := range activeRooms {
		status, err := checkRoomHealth(activeRooms[idx].Port)

		fmt.Printf("Room: %s\tStatus: %s\n", activeRooms[idx].Name, status)
		if status != "good" {
			activeRooms[idx].Health--
		}

		if err != nil || activeRooms[idx].Health <= 0 {
			closeRoom(&activeRooms[idx])
			removal = append(removal, activeRooms[idx].Name)
		}
	}

	if len(removal) > 0 {
		var newSlice []room
		for _, room := range activeRooms {
			for _, remove := range removal {
				if room.Name == remove {
					break
				}
				newSlice = append(newSlice, room)
			}
		}

		activeRooms = newSlice
	}

}

func checkRoomHealth(port int) (string, error) {
	host := "localhost:" + strconv.Itoa(port)
	u := url.URL{Scheme: "ws", Host: host, Path: ""}
	fmt.Println("Connecting to:", host, u.String())

	c, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		log.Println("dial:", err)
		return "", err
	}
	defer func() {
		err := c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Client closing connection"))

		if err != nil {
			log.Println("write close:", err)
		}

		c.Close()
	}()

	requestMessage := requestMessage{
		Type: "statusreq",
	}

	messageBytes, err := json.Marshal(requestMessage)
	if err != nil {
		log.Println("json:", err)
		return "", err
	}

	err = c.WriteMessage(websocket.TextMessage, messageBytes)
	if err != nil {
		log.Println("write:", err)
		return "", err
	}

	_, message, err := c.ReadMessage()
	if err != nil {
		log.Println("read:", err)
		return "", err
	}

	var status roomResponse
	err = json.Unmarshal(message, &status)
	if err != nil {
		log.Println("json unmarshal:", err)
		return "", err
	}

	return status.Status, nil
}

func closeRoom(r *room) error {
	fmt.Println("Closing room:", r.Name)

	cmd := exec.Command("docker-compose", "-p", r.Name, "down")

	_, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Println("Error:", err)
		return errors.New("failed to start server")
	}

	return nil
}
