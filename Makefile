OBJECTS := \
	form2post.o
OUTPUT := form2post
LDFLAGS := -lxml2
CFLAGS := -I/usr/include/libxml2/ -Wall
.PHONY: all clean

all: $(OUTPUT)

clean:
	-rm $(OUTPUT) $(OBJECTS)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(OUTPUT) $(LDFLAGS)
