TARGETS=proxy

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

proxy: server.cpp http.h cache.h
	g++ -g -pthread -o  $@ $<

