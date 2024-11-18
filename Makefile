all:build test 
build:
	gcc .\Helloworld.c -o helloworld
test:
	.\Helloworld.exe
clean:
	rm *.exe