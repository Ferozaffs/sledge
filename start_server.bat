@echo off
cd C:\\client
start python -m http.server 5500

cd C:\\server
start sledge.exe