package main

import (
	"math/rand"
	"strings"
	"time"
)

func GetRandomName() string {
	rand.Seed(time.Now().UnixNano())

	words := []string{
		"cat", "dog", "bird", "fish", "horse", "lion", "tiger", "bear", "frog", "deer",
		"apple", "bread", "milk", "cheese", "grape", "banana", "carrot", "peach", "rice", "egg",
		"chair", "table", "book", "phone", "lamp", "car", "pen", "clock", "key", "door",
		"tree", "flower", "grass", "river", "hill", "star", "cloud", "rain", "wind", "sun",
		"run", "jump", "swim", "read", "write", "draw", "sing", "dance", "play", "cook",
		"red", "blue", "green", "yellow", "black", "white", "purple", "orange", "pink", "brown",
	}

	rand.Shuffle(len(words), func(i, j int) {
		words[i], words[j] = words[j], words[i]
	})

	selectedWords := words[:3]

	randomString := strings.Join(selectedWords, "")

	return randomString
}
