package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"net/url"
	"os"
	"os/exec"
	"strconv"
	"time"

	"github.com/gorilla/websocket"
)

type requestMessage struct {
	Type string `json:"type"`
}

type room struct {
	Name   string
	Port   int
	Health int
}

var activeRooms []room
var port int = 56000

const roomhealth int = 3

func readStatusPacket(message []byte) (int, error) {
	buf := bytes.NewReader(message)

	var packetType uint8
	err := binary.Read(buf, binary.LittleEndian, &packetType)
	if err != nil {
		return 0, fmt.Errorf("read type: %w", err)
	}

	var size uint32
	err = binary.Read(buf, binary.LittleEndian, &size)
	if err != nil {
		return 0, fmt.Errorf("read size: %w", err)
	}

	data := make([]byte, size)
	if _, err := buf.Read(data); err != nil {
		return 0, fmt.Errorf("read data: %w", err)
	}

	if size > 1 {
		return 0, fmt.Errorf("packet size too large: %d", size)
	}

	if packetType == 1 && len(data) >= 1 {
		if data[0] == 1 {
			return 1, nil
		}
	}

	return 0, nil
}

func CreateRoom() (room, error) {
	roomName := GetRandomName(3)

	cmd := exec.Command("docker", "run", "--name", roomName, "--cpus=1.0", "-p", strconv.Itoa(port)+":9002", "-d", "sledge/room:latest")

	_, err := cmd.CombinedOutput()
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

	roomtest := room{"test", 9002, roomhealth}
	activeRooms = append(activeRooms, roomtest)

	var removal []string
	for idx := range activeRooms {
		status, err := checkRoomHealth(activeRooms[idx].Port)

		fmt.Printf("Room: %s\tStatus: %d\n", activeRooms[idx].Name, status)
		if status != 1 {
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

func getHost(port int) string {
	if host := os.Getenv("APP_HOST"); host != "" {
		return host + ":" + strconv.Itoa(port)
	}
	return "localhost:" + strconv.Itoa(port)
}

func checkRoomHealth(port int) (int, error) {
	host := getHost(port)
	u := url.URL{Scheme: "ws", Host: host, Path: ""}
	fmt.Println("Connecting to:", host, u.String())

	c, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		log.Println("dial:", err)
		return 0, err
	}
	defer func() {
		err := c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Client closing connection"))

		if err != nil {
			log.Println("write close:", err)
		}

		c.SetReadDeadline(time.Now().Add(time.Second))
		_, _, _ = c.ReadMessage()

		c.Close()
	}()

	requestMessage := requestMessage{
		Type: "statusreq",
	}

	messageBytes, err := json.Marshal(requestMessage)
	if err != nil {
		log.Println("json:", err)
		return 0, err
	}

	fmt.Println("Sending request")
	err = c.WriteMessage(websocket.TextMessage, messageBytes)
	if err != nil {
		log.Println("write:", err)
		return 0, err
	}

	fmt.Println("Reading response")
	_, message, err := c.ReadMessage()
	if err != nil {
		log.Println("read:", err)
		return 0, err
	}

	fmt.Println("Parsing packet")
	status, err := readStatusPacket(message)
	if err != nil {
		log.Println("packet read failed:", err)
		return 0, err
	}

	return status, nil
}

func closeRoom(r *room) error {
	fmt.Println("Closing room:", r.Name)

	cmd := exec.Command("docker", "rm", "-f", r.Name)

	_, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Println("Error:", err)
		return errors.New("failed to start server")
	}

	return nil
}
