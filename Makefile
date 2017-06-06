GCCSRC = gcc -w -c $< -o $@
JAVAC = javac $<

all: server client

clean:
	-rm *.class
	-rm client
	-rm src/*.o

server: Server.class Person.class Task.class

Server.class: Server.java
	$(JAVAC)

Person.class: Person.java
	$(JAVAC)

Task.class: Task.java
	$(JAVAC)

client: client.c src
	gcc $< src/*.o -o client

src: src/hexdump.o src/blist.o

src/hexdump.o: src/hexdump.c
	$(GCCSRC)

src/blist.o: src/blist.c
	$(GCCSRC)
