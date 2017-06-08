cmake_minimum_required(VERSION 3.7)

include_directories(src)

add_executable(client
  client.c
  src/hexdump.c
  src/blist.c
  src/parser.c
  src/descriptor.c
  src/task.c
  src/person.c
)
