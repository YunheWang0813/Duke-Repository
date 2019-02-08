simulator: simulator.o cache.o set.o way.o
	g++ -ggdb3 -o simulator simulator.o cache.o set.o way.o

simulator.o: simulator.cpp simulator.h cache.h set.h way.h
	g++ -ggdb3 --std=gnu++11 -pedantic -Werror -Wall -c simulator.cpp
cache.o: cache.cpp cache.h set.h way.h
	g++ -ggdb3 --std=gnu++11 -pedantic -Werror -Wall -c cache.cpp 
set.o: set.cpp set.h way.h
	g++ -ggdb3 --std=gnu++11 -pedantic -Werror -Wall -c set.cpp
way.o: way.cpp way.h
	g++ -ggdb3 --std=gnu++11 -pedantic -Werror -Wall -c way.cpp


