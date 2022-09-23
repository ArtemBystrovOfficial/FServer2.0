add_library(libFS STATIC Server/FServer.cpp
                         Server/Servise/basic.cpp 
                         Server/IO/proxy.cpp
                         Server/IO/io.cpp
                         Server/IO/buffer.cpp
)


