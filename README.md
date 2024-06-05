# Sledge
2D PvP brawler

## Install
To install only the dockerfile and docker-compose file is needed.
Run ```docker-compose -f Dockerfile-Standalone up --build``` to setup a linux based server and frontend.

### WIP
Currently working on splitting application into frontend/host and seperate game rooms.

## Dependencies

### Frontend
* PixiJS - https://pixijs.com/

### Gamer server
* Box2D - https://github.com/erincatto/box2d
* GLFW - https://github.com/glfw/glfw
* linmath - https://github.com/datenwolf/linmath.h
* Boost - https://www.boost.org/
* nlohmann json - https://github.com/nlohmann/json
* websocketpp - https://github.com/zaphoyd/websocketpp

### Server host
* Gorilla WebSocket - https://github.com/gorilla/websocket