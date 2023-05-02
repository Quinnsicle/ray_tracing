CFLAGS = -std=c++17 -O2

RayTracing: main.cpp
	g++ $(CFLAGS) -o bin/RayTracing main.cpp 

.PHONY: test clean

test: RayTracing
	./bin/RayTracing

clean:
	rm -f bin/RayTracing
