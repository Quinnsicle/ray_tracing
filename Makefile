CFLAGS = -std=c++17 -O2

INCLUDE = -Iinclude

RayTracer: RayTracer/main.cpp
	@mkdir -p bin
	g++ $(INCLUDE) $(CFLAGS) -o bin/RayTracer RayTracer/main.cpp 

test: RayTracer
	bin/RayTracer

Performance: RayTracer/performance.cpp
	@mkdir -p bin
	g++ $(INCLUDE) $(CFLAGS) -o bin/Performance RayTracer/performance.cpp 

speed: Performance
	bin/Performance

.PHONY: test speed clean

clean:
	rm -f bin/RayTracer
