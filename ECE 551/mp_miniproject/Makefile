CFLAGS=-std=gnu++98 -Wall -Werror -pedantic -ggdb3
all: myShell test_step2
myShell: main.cpp myShell.cpp myShell.h
	g++ $(CFLAGS) main.cpp myShell.cpp -o myShell
test_step2: test_step2.cpp
	g++ $(CFLAGS) test_step2.cpp -o test_step2
.PHONY: clean
clean:
	rm -f myShell *.o *.cpp~ *.h~
