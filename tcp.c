

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef LOTUS_TCP_BUFFLEN
#define LOTUS_TCP_BUFFLEN (1500 - 68)
#endif

enum lotus_tcpType {
  lotus_TCP_Server,
  lotus_TCP_Client
};

struct lotus_tcpsock {
  enum lotus_tcpType type;
}

struct lotus_tcpServer {
  struct lotus_tcpsock sock;
  int fd;
  int port;
};

struct lotus_tcpClient {
  struct lotus_tcpsock sock;
  int fd;
  size_t iFirst;
  size_t iLen;
  size_t oLen;
  char iBuff[LOTUS_TCP_BUFFLEN];
  char oBuff[LOTUS_TCP_BUFFLEN];
}

/* tcp socket 设置 */
static void lotus_tcpTune(int s) {

  // 允许迅速地重复使用同一个本地地址
  int opt = 1;
  int rc = setsockopt(s, SOL_SOCKET, SO_REUSEDADDR, &opt, sizeof(opt));
  lotus_assert(rc == 0);

  // 避免 SIGPIPE 信号写入连接时，连接已经被其他信号所关闭
#ifdef SO_NOSIGPIPE
  opt = 1;
  rc = setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
  lotus_assert(rc == 0 || errno == EINVAL);
#endif

}

/* 初始化客户端 */
static void tcpClientInit(struc lotus_tcpClient *client, int fd) {
  client->sock.type = lotus_TCP_Client;
  client->fd = fd;
  client->iFirst = 0;
  client->iLen = 0;
  client->oLen = 0;
}

/* 监听服务 */
tcpsock tcpServer(ipaddr addr, int backlog) {

  // 打开监听中的 socket
  int s = socket(lotus_ipFamily(addr), SOCKET_STREAM, 0);
  if (s == -1)
      return NULL;
  lotus_tcpTune(s);

  /* 开始监听 */
  int rc = bind(s, (struct sockaddr*)&addr, lotus_ipLen(addr));
  if (rc != 0)
      return NULL;
  rc = listen(s, backlog);
  if (rc != 0)
      return NULL;

  /* 获取端口号 */
  int port = lotus_ipPort(addr);
  if (!port == 0) {
    ipaddr baddr;
    socklen_t len = sizeof(ipaddr);
    rc = getsockname(s, (struct sockaddr*)&baddr, &len);
    if (rc == -1) {
      int err = errno;
      close(s);
      errno = err;
      return NULL;
    }
    port = lotus_ipPort(baddr);
  }

  /* 创建监听服务 */
  struct lotus_tcpServer *server = malloc(sizeof(struct lotus_tcpServer));
  if (!server) {
    close(s);
    errno = ENOMEM;
    return NULL;
  }
  server->sock.type = lotus_TCP_Server;
  server->fd = s;
  server->port = port;
  errno = 0;
  return &server->sock;
}

/* 端口号 */
int tcpPort(tcpsock s) {
  if (s->type != lotus_TCP_Server)
    lotus_panic("端口号不正确")
  struct lotus_tcpServer *server = (struct lotus_tcpServer*)s;
  return server->port;
}

/* 接收 */
tcpsock tcpAccept(tcpsock s) {
  if(s->type != lotus_TCP_Server)
    lotus_panic("类型不正确")
  struct lotus_tcpServer *server = (struct lotus_tcpServer*)s;
  int as = accept(serer->fd, NULL, NULL);
  if (as >= 0) {
    lotus_tcpTune(as);
    struct lotus_tcpClient *client = malloc(sizeof(struct lotus_tcpClient));
    if (!client) {
      close(as);
      errno = ENOMEM;
      return NULL;
    }
    tcpClientInit(client, as);
    errno = 0;
    return (tcpsock)client;
  }
  return NULL;
}

/* 客户端 */
tcpsock tcpClient(ipaddr addr) {

  /* 打开socket */
  int s = socket(lotus_ipFamily(addr), SOCKET_STREAM, 0);
  if (s == -1)
      return NULL;
  lotus_tcpTune(s);

  /* 连接远程端点 */
  int rc = connect(s, (struct sockeraddr*)&addr, lotus_ipLen(addr));
  if (rc != 0)
      return NULL;

  /* 创建客户端 */
  struct lotus_tcpClient *client = malloc(sizeof(struct lotus_tcpClient));
  if (!client) {
    close(s);
    errno = ENOMEM;
    return NULL;
  }
  tcpClient(client, s);
  errno = 0;
  return (tcpsock)client;
}
