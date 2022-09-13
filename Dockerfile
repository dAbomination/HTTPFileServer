FROM ubuntu:22.04

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \    
    cmake 
	
RUN apt-get install -y pkg-config
RUN apt-get update && apt-get install -y software-properties-common
RUN add-apt-repository ppa:pistache+team/unstable
RUN apt-get install -y libpq-dev
RUN apt-get install -y libpqxx-dev
RUN apt install -y libpistache-dev
RUN apt-get update

COPY HTTPFileServer /HTTPFileServer
RUN mkdir /HTTPFileServer/HTTPFileServer/build/
WORKDIR /HTTPFileServer/HTTPFileServer/build/
RUN cmake ..
RUN cmake --build .

CMD ["./HTTPFileServer"]
EXPOSE 80
RUN 