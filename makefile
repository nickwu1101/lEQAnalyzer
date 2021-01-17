SRCDIR=src
BUILDDIR=obj_file
TARGET=go

SRCTXT=cpp
SOURCES=$(shell find $(SRCDIR) -type f -name *.$(SRCTXT))
OBJECTS=$(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.$(SRCTXT)=.o))

INC=-I include
INCDIR=include

CXXFLAGS=-g -m64 -O2 -Wall $(INC)
ROOTFLAGS=$(shell root-config --libs --cflags --glibs)

$(TARGET):		$(OBJECTS)
	g++ $^ -o $@ $(ROOTFLAGS)

$(BUILDDIR)/main.o:	$(SRCDIR)/main.cpp
	g++ -c $(CXXFLAGS) $(ROOTFLAGS) $< -o $@

$(BUILDDIR)/%.o:	$(SRCDIR)/%.$(SRCTXT) $(INCDIR)/%.h
	g++ -c $(CXXFLAGS) $(ROOTFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(BUILDDIR)/*.o include/*~ $(SRCDIR)/*~ ./*~ recordDoc/*~
