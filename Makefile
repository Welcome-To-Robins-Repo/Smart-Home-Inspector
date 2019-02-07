FLAGS = 	`pkg-config --cflags gtk+-3.0` -std=gnu++17
LIBS = 		`pkg-config --libs gtk+-3.0`
OBJS = 		shi.o window.o func.o classes.o
OUT = 		shi

default: shi

shi: $(OBJS)
	g++ $(OBJS) $(FLAGS) $(LIBS) -o$(OUT)

shi.o: shi.cpp shi.h classes.h
	g++ -c $*.cpp $(FLAGS) $(LIBS) -g3

window.o: window.cpp shi.h classes.h
	g++ -c $*.cpp $(FLAGS) $(LIBS) -g3

func.o: func.cpp shi.h classes.h
	g++ -c $*.cpp $(FLAGS) $(LIBS) -g3

classes.o: classes.cpp classes.h shi.h 
	g++ -c $*.cpp $(FLAGS) $(LIBS) -g3

clean:
	rm -f $(OBJS) $(OUT)

install: shi

	mkdir ~/.local/share/shi
	mv shi /usr/local/bin

test:
	./shi
