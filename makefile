all:
	gcc *.c -lpthread -o main -std=c99
clean:
	rm main