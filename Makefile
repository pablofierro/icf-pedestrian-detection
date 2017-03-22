CC = clang-omp++ -fopenmp -std=c++11 $(shell pkg-config --cflags --libs opencv) -laf -lboost_system -lboost_filesystem -lmlpack -larmadillo -lboost_serialization
BINDIR := ./bin/
SRCDIR := ./src/

clean:
	@rm -f $(BINDIR)/*
build: clean
	@$(CC) $(SRCDIR)lib/*.cpp $(SRCDIR)lib/pascal/*.cpp  $(SRCDIR)$(PROGRAM).cpp -o $(BINDIR)$(PROGRAM)
run: build
	@$(BINDIR)$(PROGRAM) $(ARGS)
