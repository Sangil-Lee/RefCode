CXX = g++
CFLAGS = -I/usr/local/mdsplus/include -L/usr/local/mdsplus/lib \
		 -I./ \
		 -lMdsShr -lMdsLib -lCacheShr -lMdsObjectsCppShr

#bin:mdsobjex2
bin:mdsobjex mdsobjex1 mdsobjex2 mdsobjex4

mdsobjex: mdsobjex.cpp
	$(CXX) $(CFLAGS) mdsobjex.cpp -o mdsobjex

mdsobjex1: mdsobjex1.cpp
	$(CXX) $(CFLAGS) mdsobjex1.cpp -o mdsobjex1

mdsobjex2: mdsobjex2.cpp
	$(CXX) $(CFLAGS) mdsobjex2.cpp -o mdsobjex2

mdsobjex4: mdsobjex4.cpp
	$(CXX) $(CFLAGS) mdsobjex4.cpp -o mdsobjex4

clean:
	rm -f *.o mdsobjex mdsobjex1 mdsobjex2 mdsobjex4
