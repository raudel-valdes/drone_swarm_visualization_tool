CC = g++
CFLAGS = -c -ggdb -std=c++11 -I include/c_library_v2
LDFLAGS = -pthread 

BUILDDIR = build
OBJDIR = build/obj
SOURCEDIR = src

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
# same as sources but with .o extension
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))
EXECUTABLE = drones





all: dir $(SOURCES) $(EXECUTABLE)

dir: 
	mkdir -p $(BUILDDIR) $(OBJDIR)

# $@ is the target name
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(BUILDDIR)/$@ 

# in order to build a file ending in .o, first set the corresponding .cpp to a dependency
# $< is the first dependency, referring to the .cpp file
# the prereq %.cpp is computed from the original OBJECT by substitution of .o with .cpp
$(OBJECTS): $(OBJDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)
