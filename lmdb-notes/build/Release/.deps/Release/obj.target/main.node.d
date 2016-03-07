cmd_Release/obj.target/main.node := g++ -shared -rdynamic  -Wl,-soname=main.node -o Release/obj.target/main.node -Wl,--start-group Release/obj.target/main/main.o -Wl,--end-group -llog
