# Compilers
NVCC   = /usr/bin/nvcc
CXX    = /usr/bin/g++ -fopenmp
LINK   = $(CXX) -fPIC

# Flags
COMMONFLAGS = -O2
NVCCFLAGS   = -Xcompiler -fopenmp --compiler-options -fno-strict-aliasing 
CXXFLAGS    = -fno-strict-aliasing -fopenmp
CFLAGS      = -g -fno-strict-aliasing

# Paths
INCLUDES    = -I/usr/local/inc -I/home/moeryn/NVIDIA_GPU_Computing_SDK/C/common/inc/
LIBS        = -L/usr/local/cuda/lib64 -L/home/moeryn/NVIDIA_GPU_Computing_SDK/C/lib -lcufft -lcutil_x86_64 -lcudart -lgomp

# Program-specific
TARGET= matrixMultiplication
SRC =  matrixMultiplication.cu matrixMultiplicationKernel.cu 
OBJ = matrixMultiplication.cu.o

# Build Rules
.SUFFIXES: .cu

$(TARGET): $(OBJ) 
	$(LINK) -o $(TARGET) $(OBJ) $(LIBS)

$(OBJ): $(SRC)
	$(NVCC) $(NVCCFLAGS) $(INCLUDES) $(LIBS) -o $(OBJ) -c $< 

clean:
	rm -f $(OBJ) $(TARGET)
