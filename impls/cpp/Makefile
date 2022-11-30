CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror

build: main

clean:
	rm -f main

main: main.cpp reader.cpp printer.cpp types.cpp core.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
