#INCS = -I/usr/include -I/usr/include/opencv2
#LIBS = -L/usr/lib

#DEFINCS = -I/usr/include -I/usr/include/opencv2
#DEFLIBS = -L/usr/lib 

CVINCPATH = /home/lizian/.local/usr/opencv3.4.12/usr/local/include
CVLINKPATH = /home/lizian/.local/usr/opencv3.4.12/usr/local/lib64 

DEFINCS = -I$(CVINCPATH) -I/usr/include 
DEFLIBS = -L$(CVLINKPATH) -L/usr/lib64

LINKLIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio

CPP  = g++
CC   = gcc

OBJ_UNDISTORT = undistort.o ocam_functions.o 
LINKOBJ_UNDISTORT = undistort.o ocam_functions.o 

OBJ_TESTREPRO = testrepro.o ocam_functions.o 
LINKOBJ_TESTREPRO = testrepro.o ocam_functions.o 

OBJ_CONTOUR = contour.o ocam_functions.o 
LINKOBJ_CONTOUR = contour.o ocam_functions.o 

OBJ_CORNER = harris.o ocam_functions.o 
LINKOBJ_CORNER = harris.o ocam_functions.o 

BIN = Undistort TestRepro Contour HarrisCorner
RM = rm -f

all: Undistort TestRepro Contour HarrisCorner

clean:
	${RM} $(OBJ) $(BIN)

Undistort: $(OBJ_UNDISTORT)
	$(CPP) $(LINKOBJ_UNDISTORT) -o Undistort $(DEFLIBS) $(LINKLIBS)
undistort.o: undistort.cpp
	$(CPP) -c undistort.cpp -o undistort.o $(DEFINCS)

TestRepro: $(OBJ_TESTREPRO)
	$(CPP) $(LINKOBJ_TESTREPRO) -o TestRepro $(DEFLIBS) $(LINKLIBS)
testrepro.o: testrepro.cpp
	$(CPP) -c testrepro.cpp -o testrepro.o $(DEFINCS)

Contour: $(OBJ_CONTOUR)
	$(CPP) $(LINKOBJ_CONTOUR) -o Contour $(DEFLIBS) $(LINKLIBS)
contour.o: contour.cpp
	$(CPP) -c contour.cpp -o contour.o $(DEFINCS)

HarrisCorner: $(OBJ_CORNER)
	$(CPP) $(LINKOBJ_CORNER) -o HarrisCorner $(DEFLIBS) $(LINKLIBS)
harris.o: harris.cpp
	$(CPP) -c harris.cpp -o harris.o $(DEFINCS)

ocam_functions.o: ocam_functions.cpp
	$(CPP) -c ocam_functions.cpp -o ocam_functions.o $(DEFINCS)
