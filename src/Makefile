TARGET =libratelimiter.so
CC =g++ -fPIC -g -O3 -Wall -std=c++17
INCLUDE =$(shell find ./ -name "*.hpp")
SOURCE =$(shell find ./ -name "*.cpp")
OBJS =$(SOURCE:%.cpp=%.o)

$(TARGET):$(OBJS)
	$(CC) -shared -o $(TARGET) $(OBJS) $(LIBS)
	
%.o: %.cpp $(INCLUDE)
	$(CC) -c $< -o $@ $(LIBS)
	
all:$(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)