FROM alpine:latest AS build

RUN apk add --no-cache \
    git \
    cmake \
    make \
    clang \
    openssl-dev

WORKDIR /src

RUN git clone https://github.com/Ferozaffs/sledge

WORKDIR /src/sledge/server

RUN mkdir build
WORKDIR /src/sledge/server/build

RUN cmake -DCMAKE_CXX_COMPILER=clang ..
RUN make

#---------------------------------------
FROM alpine:latest

RUN apk add --no-cache \
    nginx \
    openssl

COPY --from=build /src/sledge/client /app/client
COPY --from=build /src/sledge/server/build/sledge /app/server/sledge
COPY --from=build /src/sledge/server/data/app/server/data  

COPY --from=build /src/sledge/nginx.conf /etc/nginx/nginx.conf
COPY --from=build /src/sledge/nginx.conf /start.sh
RUN chmod +x /start.sh

EXPOSE 5500
EXPOSE 9002

CMD ["/start.sh"]