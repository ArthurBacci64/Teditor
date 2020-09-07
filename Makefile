all: ted

ted: ted.c
	$(CC) -o ted ted.c -Wall -W -pedantic -std=c99

install:
	$(CC) -o /usr/bin/ted ted.c -Wall -W -pedantic -std=c99

clean:
	rm ted
