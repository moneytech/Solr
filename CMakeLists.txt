cmake_minimum_required(VERSION 3.2)
project(Solr)

include_directories(include)

add_executable(Solr main.c include/solr/object.h include/solr/common.h src/murmur.c include/solr/class.h src/symbol.c include/solr/vm.h src/vm.c src/class.c include/solr/opcode.h include/solr/gc.h src/gc.c src/object.c)