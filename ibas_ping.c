#include <ibas_ping.h>


int		ping_repeat_sec = 1;		/* 1 second */
int		ping_alarm = 0;
int		ping_seq = 0;
int		ping_responses = 0;
int		ping_ctrl_c = 0;


void	custom_signal_handler(int i)
 {
   if (SIGALRM == i)  { ping_alarm = 1; }

   if (SIGINT == i)
     { ping_alarm = 1;
       ping_ctrl_c = 1;
     }
 }


void	usage(char *name)
 {
   printf("usage:\n\t%s ibasalertd_host_name_or_ip4addr ibasalertd_UDP_port my_UDP_port\n\n", name);
   exit(10);
 }



int	main(int argc, char **argv)
{ int			ibas_ip, ibas_port, i4[4], my_port, r;
  char			c_tmp, **p;
  struct sigaction	sa;
  struct hostent	*hp;
  struct in_addr	in;
  IBC_DL		dl;

		/* scan command line */

if (4 != argc) usage(argv[0]);

ibas_ip = 0;

if (4 == sscanf(argv[1], "%d.%d.%d.%d %c", &(i4[0]), &(i4[1]), &(i4[2]), &(i4[3]), &c_tmp))
  {		/* if it is numeric value, then convert directly to IP4 number */
    ibas_ip = (i4[0] << 24) + (i4[1] << 16) + (i4[2] << 8) + i4[3];
  }
else		/* otherwise, call DNS to resolve name -> IP4 (Solaris 8+: use getipnodebyname()) */
  { if (NULL != (hp = gethostbyname(argv[1])))
      { p = hp->h_addr_list; 
        if (NULL != p)  if (0 != *p)
          { memcpy(&in.s_addr, *p, sizeof(in.s_addr));
	    ibas_ip = ntohl(in.s_addr);
	  }
      }
  }

ibas_port = atoi(argv[2]);
my_port = atoi(argv[3]);

if ((0 == ibas_ip) || (0 == ibas_port) || (0 == my_port))  usage(argv[0]);

		/* before initialization, setup handler for signal forcing timeout */

sa.sa_handler = custom_signal_handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;
sigaction(SIGALRM, &sa, NULL); 

sa.sa_handler = custom_signal_handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;
sigaction(SIGINT, &sa, NULL); 

		/* initialize our stuff */

if (IBC_OK != (r = ibc_api_init(ibas_ip, ibas_port, my_port, &ping_alarm)))
  { printf("ibc_api_init() failed, error code = %d\n", r);
    exit(r);
  }

r = ibc_api_send_ping(ping_seq);
if (IBC_OK != r)
  { printf("WARNING: ibc_api_send_ping() failed, error code = %d\n", r);
    ibc_api_shutdown();
    exit(r);
  }
alarm(ping_repeat_sec);
ping_seq++;

for (;;)
 {
   r = ibc_api_listen(&dl);

   if (IBC_SIGNALLED == r)
     {
       if (ping_ctrl_c)  break;		/* ctrl-c pressed */
       r = ibc_api_send_ping(ping_seq);
       if (IBC_OK != r)
         { printf("WARNING: ibc_api_send_ping() failed, error code = %d\n", r);
           break;
         }
       alarm(ping_repeat_sec);
       ping_seq++;
       continue;
     }
   
   if (IBC_OK != r)  break;

   if (0 == (IBAS_ALERT_TEST_FLAG_R_PING & dl.a.test_flag)) continue;
   ibc_api_dump_ping_reply(&dl, ibas_ip, ibas_port);
   ping_responses++;
 }

ibc_api_shutdown();

printf("\nSummary: packets sent: %d, responses: %d\n", ping_seq, ping_responses);

return(0);
}
