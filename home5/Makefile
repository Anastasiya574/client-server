all: bin bin/web-server bin/web-client
	
bin:
	mkdir bin

bin/web-server: web-server.c
	gcc web-server.c -o bin/web-server

bin/web-client: web-client.c
	gcc web-client.c -o bin/web-client

clean:
	rm bin/web-server bin/web-client
	rmdir bin
