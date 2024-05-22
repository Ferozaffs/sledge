FROM ubuntu:latest AS build

RUN apt-get update
RUN apt-get -y install \
    git \
    cmake \
    g++

WORKDIR /src

RUN git clone https://github.com/Ferozaffs/sledge

WORKDIR /src/sledge/server

RUN mkdir build
WORKDIR /src/sledge/server/build

RUN cmake ..
RUN make

#---------------------------------------
FROM ubuntu:latest

RUN apt-get update
RUN apt-get -y install nginx

COPY --from=build /src/sledge/client /app/client
COPY --from=build /src/sledge/server/build/sledge /app/server/sledge
COPY --from=build /src/sledge/server/data /app/server/data  

COPY --from=build /src/sledge/nginx.conf /etc/nginx/conf.d/default.conf
COPY --from=build /src/sledge/start.sh /start.sh
RUN chmod +x /start.sh

EXPOSE 5500
EXPOSE 9002

CMD ["/start.sh"]