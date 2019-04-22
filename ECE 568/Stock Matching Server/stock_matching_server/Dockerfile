FROM ubuntu:18.04
ENV DEBIAN_FRONTEND=noninteractive
RUN mkdir /code
RUN apt-get update && apt-get install -y build-essential libpqxx-dev libtbb-dev make
ADD . /code/
WORKDIR /code
RUN chmod +rx ./server.cpp
RUN make