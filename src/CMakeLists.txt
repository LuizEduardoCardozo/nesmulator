set(SOURCES memory.cpp  cpu.cpp instruction.cpp  Instructions.cpp  AddressingMode.cpp  LookupTable.cpp  AddressingTypes.cpp AddressingMode.cpp types/AbstractCpu.cpp types/AbstractCpu.h)

set(GLOB HEADERS memory.h cpu.h utils/types.h instruction.h Instructions.h AddressingMode.h LookupTable.h utils/typeDefinitions.h)

add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${SOURCES} ${HEADERS})