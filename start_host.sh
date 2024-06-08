#!/bin/sh

nginx -g "daemon off;" &
cd /app
./serverhost
