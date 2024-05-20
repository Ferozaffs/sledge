FROM mcr.microsoft.com/windows/servercore:ltsc2022

COPY ./start_server.bat C:/start_server.bat 

COPY ./client/ C:/client/

COPY ./server/build/Release/ C:/server/
COPY ./server/data/ C:/server/data/

EXPOSE 5500
EXPOSE 9002

ENTRYPOINT ["C:\\start_server.bat"]