#ifndef DEFINES_H
#define DEFINES_H

extern bool debugOn;
extern bool throwOn;
#define DEBUG if(debugOn)
#define THROW(x) if(throwOn) throw x

#endif