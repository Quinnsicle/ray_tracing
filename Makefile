CFLAGS = -std=c++17 -O2

INCLUDE = -Iinclude

RayTracer: RayTracer/main.cpp
	@mkdir -p bin
	g++ $(INCLUDE) $(CFLAGS) -o bin/RayTracer RayTracer/main.cpp 

.PHONY: test clean

test: RayTracer
	bin/RayTracer

clean:
	rm -f bin/RayTracer
