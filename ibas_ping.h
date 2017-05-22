#include <ibas_client.h>

#include <netdb.h>
#include <sys/socket.h>


extern	IBC_DL		dl;
extern	int		ping_sock;
extern	unsigned short	ping_port;
extern	unsigned long	ping_ip4addr;
extern	unsigned short	my_port;
extern	int		ping_signalled;
extern	int		ping_timeout_sec;
extern	int		ping_repeat_msec;


void	ping_sighandler_polite(int i);
void	ping_sighandler_brute(int i);
int	ping_sighandler_init(void);
int	ping_init(int my_port);
