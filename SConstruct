sources = [
    "nettest.c",
    "config.c",
    "interface.c",
    "transmitter.c"]
debug_flag='-DDEBUG'

Program('nettest', sources, LIBS=['libconfig', 'pthread'], CCFLAGS=debug_flag)
