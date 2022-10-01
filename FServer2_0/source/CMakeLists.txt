add_library(libFS STATIC Server/FServer.cpp
                         Server/Servise/basic.cpp 
                         Server/Servise/filter_io.cpp
                         Server/IO/io.cpp
                         Server/IO/buffer.cpp
)

add_library(libFC STATIC Client/FClient.cpp
                         Client/Servise/basic.cpp 
                         Client/Servise/filter_io.cpp
                         Server/IO/io.cpp
                         Server/IO/buffer.cpp
)

