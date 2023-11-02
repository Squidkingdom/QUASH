StuID = 3028989

all:
	g++ -g main.cpp executive.cpp parser.cpp built-ins.cpp -o quash

clean:
	rm quash