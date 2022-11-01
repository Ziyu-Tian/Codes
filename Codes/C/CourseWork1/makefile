final: finalTest.o meminfo.o stat.o uptime.o
	g++ -Wall -o final finalTest.o meminfo.o stat.o uptime.o

finalTest.o: finalTest.cpp
	g++ -o finalTest.o -c finalTest.cpp

meminfo.o: meminfo.cpp
	g++ -o meminfo.o -c meminfo.cpp

stat.o: stat.cpp
	g++ -o stat.o -c stat.cpp

uptime.o: uptime.cpp
	g++ -o uptime.o -c uptime.cpp

clean:
	rm -f final 
	rm -f *.o