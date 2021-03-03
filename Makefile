CFLAGS_DEBUG=-O0 -ggdb -DDEBUG
CFLAGS_RELEASE=-O3 -s
CFLAGS=-Wall -Werror -Wextra -pedantic
DIR_BINARY=bin
DIR_LIBRARY=lib

SOURCES_MAIN=$(wildcard src/*.c)
SOURCES_LIB=$(wildcard src/libhue/*.c)
OBJECTS_LIB=$(patsubst %.c, %.o, $(SOURCES_LIB))
HEADERS_LIB=$(wildcard src/libhue/*.h)

ifeq (${DEBUG}, y)
	CFLAGS+=$(CFLAGS_DEBUG)
else
	CFLAGS+=$(CFLAGS_RELEASE)
endif

all: hue-static hue-dynamic libhue.so

$(DIR_BINARY):
	@mkdir -p $(DIR_BINARY)

$(DIR_LIBRARY):
	@mkdir -p $(DIR_LIBRARY)

hue-static: $(SOURCES_MAIN) $(OBJECTS_LIB) | $(DIR_BINARY)
	$(CC) $(CFLAGS) -o $(DIR_BINARY)/$@ $(SOURCES_MAIN) $(OBJECTS_LIB)

$(OBJECTS_LIB): src/libhue/%.o : src/libhue/%.c
	$(CC) $(CFLAGS) -c $< -o $@

hue-dynamic: libhue.so
	$(CC) $(CFLAGS) -L$(DIR_LIBRARY) -o $(DIR_BINARY)/$@ src/main.c -lhue

libhue.so: $(OBJECTS_LIB) | $(DIR_LIBRARY)
	$(CC) -shared -o $(DIR_LIBRARY)/$@ $(OBJECTS_LIB)

clean:
	rm src/libhue/*.o
	rm -r bin lib

