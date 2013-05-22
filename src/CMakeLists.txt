# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
#   set(QT_USE_OPENGL TRUE)
# -> this will cause cmake to include and link against the OpenGL module
include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# see http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4 for a complete list

# the variable "qtproject_SRCS" contains all .cpp files of this project

SET(bmodel_SOURCES
		BackgroundModel.cpp  bmodel.cpp  HistogramModel.cpp  MedianModel.cpp  PreviewModel.cpp
		CodebookModel.cpp
)

SET(bmodel_HEADERSQT )
SET(bmodel_HEADERS 
		${bmodel_HEADERSQT}
		BackgroundModel.h
		HistogramModel.h
		MedianModel.h
		PreviewModel.h
		CodebookModel.h
)

#set(ctitest_SOURCES
#)

QT4_WRAP_CPP(bmodel_HEADERS_MOC ${bmodel_HEADERSQT})


# tell cmake to create .moc files for all files in the variable qtproject_SRCS that require such a file.
# note: this assumes that you use #include "header.moc" in your files
#qt4_automoc(${ctitest_SRCS})

# create an executable file named "qtproject" from the source files in the variable "qtproject_SRCS".
add_executable(bmodel
		${bmodel_SOURCES}
		${bmodel_HEADERS_MOC}
		)

# link the "qtproject" target against the Qt libraries. which libraries exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
target_link_libraries(bmodel ${QT_LIBRARIES} ${OpenCV_LIBS})
