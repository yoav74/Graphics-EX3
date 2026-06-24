CXX      = g++
CXXFLAGS = -Wall -O2
LDFLAGS  = -lGL -lGLU -lglut

TARGET = ex3
SRC    = main.cpp trackball.cpp drawing.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o *.d *~
