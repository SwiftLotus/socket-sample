/*
  author: chenmingbiao(CMB)
*/


#ifndef LOTUS_H_INCLUDED
#define LOTUS_H_INCLUDED

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

//////////////////////////////////////////////////
//  Symbol Visibility
//////////////////////////////////////////////////

#if defined LOTUS_NO_EXPORTS
#   define LOTUS_EXPORT
#else
#   if defined _WIN32
#      if defined LOTUS_EXPORTS
#          define LOTUS_EXPORT __declspec(dllexport)
#      else
#          define LOTUS_EXPORT __declspec(dllimport)
#      endif
#   else
#      if defined __SUNPRO_C
#          define LOTUS_EXPORT __global
#      elif (defined __GNUC__ && __GNUC__ >= 4) || \
             defined __INTEL_COMPILER || defined __clang__
#          define LOTUS_EXPORT __attribute__ ((visibility("default")))
#      else
#          define LOTUS_EXPORT
#      endif
#   endif
#endif

//////////////////////////////////////////////////
//  IP Address Library
//////////////////////////////////////////////////

#define IPADDR_IPV4 1
#define IPADDR_IPV6 2
#define IPADDR_PREF_IPV4 3
#define IPADDR_PREF_IPV6 4

typedef struct {
  char data[128];
} ipaddr;

LOTUS_EXPORT ipaddr localIp(const char *name, int port);
LOTUS_EXPORT ipaddr remoteIp(const char *name, int port);

//////////////////////////////////////////////////
//  TCP Library
//////////////////////////////////////////////////

typedef struct lotus_tcpsock *tcpsock;

LOTUS_EXPORT tcpsock tcpServer(ipaddr addr, int backlog);
LOTUS_EXPORT int tcpPort(tcpsock s);
LOTUS_EXPORT tcpsock tcpAccept(tcpsock s);
LOTUS_EXPORT tcpsock tcpConnect(ipaddr addr);
LOTUS_EXPORT size_t tcpSend(tcpsock s, const void *buff, size_t len);
LOTUS_EXPORT void tcpFlush(tcpsock s);
LOTUS_EXPORT size_t tcpRecv(tcpsock s, void *buff, size_t len);
LOTUS_EXPORT size_t tcpRecvUntil(tcpsock s, void *buff, size_t len, const char *delims, size_t delimcount);
LOTUS_EXPORT void tcpClose(tcpsock s);
LOTUS_EXPORT tcpsock tcpAttach(int fd, int listening);
LOTUS_EXPORT int tcpDetach(tcpsock s);
LOTUS_EXPORT int tcpFd(tcpsock s);

#endif
