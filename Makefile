all: server client

server: Server.java
	javac $<

client: client.c
	gcc $< -o client
