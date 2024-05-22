#!/bin/sh

cd /app/server
./sledge &

nginx -g "daemon off;" &