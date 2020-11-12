all: bin bin/server bin/client
	
bin:
	mkdir bin

bin/server: server.c
	gcc server.c -o bin/server

bin/client: client.c
	gcc client.c -o bin/client

clean:
	rm bin/server bin/client
	rmdir bin
