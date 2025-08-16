package main

import (
	"encoding/json"
	"net/http"
	"sync"
	"time"
)

type RateLimiter struct {
	requests map[string][]time.Time
	mu       sync.Mutex
	window   time.Duration
}

type response struct {
	Status  string `json:"status"`
	Room    string `json:"room,omitempty"`
	Port    int    `json:"port,omitempty"`
	Message string `json:"message,omitempty"`
}

func NewRateLimiter(window time.Duration) *RateLimiter {
	return &RateLimiter{
		requests: make(map[string][]time.Time),
		window:   window,
	}
}

func (rl *RateLimiter) Allow(user string, limit int) bool {
	rl.mu.Lock()
	defer rl.mu.Unlock()

	now := time.Now()
	if times, exists := rl.requests[user]; exists {
		validTimes := []time.Time{}
		for _, t := range times {
			if now.Sub(t) <= rl.window {
				validTimes = append(validTimes, t)
			}
		}
		rl.requests[user] = validTimes

		if len(validTimes) >= limit {
			return false
		}
	}

	rl.requests[user] = append(rl.requests[user], now)
	return true
}

func RateLimitMiddleware(getLimit func(*http.Request) int, window time.Duration) func(http.Handler) http.Handler {
	limiter := NewRateLimiter(window)

	return func(next http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			w.Header().Set("Access-Control-Allow-Origin", "*")
			user := r.RemoteAddr
			limit := getLimit(r)

			if !limiter.Allow(user, limit) {
				http.Error(w, http.StatusText(http.StatusTooManyRequests), http.StatusTooManyRequests)
				return
			}

			next.ServeHTTP(w, r)
		})
	}
}

func CreateRoomHandler(w http.ResponseWriter, r *http.Request) {
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
	mux := http.NewServeMux()

	mux.HandleFunc("/create_room", CreateRoomHandler)
	mux.HandleFunc("/room", RoomHandler)

	getLimit := func(r *http.Request) int {
		if r.URL.Path == "/create_room" {
			return 2
		}
		return 20
	}

	go func() {
		http.ListenAndServe(":5501", RateLimitMiddleware(getLimit, time.Minute)(mux))
	}()

	ticker := time.NewTicker(2 * time.Second)
	defer ticker.Stop()

	for range ticker.C {
		UpdateRooms()
	}
}
