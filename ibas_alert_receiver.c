#include <ibas_alert_receiver.h>


int	ping_signalled = 0;


void	custom_signal_handler(int i)
 {
   if (SIGHUP == i)  { ping_signalled = 1; }
 }


void	usage(char *name)
 {
   printf("usage:\n\t%s ibasalertd_host_name_or_ip4addr ibasalertd_UDP_port my_UDP_port [ userprog ]\n", name);
   printf("\n");
   printf("Assuming you connect to the ibasalertd machine, the typical values are :\n");
   printf("\n");
   printf("\t%s 129.194.67.222 1966 1944 [ userprog ]\n", name);
   printf("\n");
   printf("If userprog is given, then for each new alert received by this program\n");
   printf("the userprog is called with 27 arguments passed (alert data in ASCII format).\n");
   printf("You may use   ./dump_alert.sh  script as an example. This script merely\n");
   printf("dumps alert data to the standard output. It is easy to tailor it to the\n");
   printf("particular needs (redirection to a file for instance).\n");
   printf("If the userprog is not given then the alert data is simply dumped to the\n");
   printf("standard output directly by this program.\n");
   exit(10);
 }


int	main(int argc, char **argv)
{ int			ibas_ip, ibas_port, i4[4], my_port, r, pingseq;
  char			c_tmp, **p, *sysprg, syscmd[9999];
  char			buft1[IBAS_ALERT_TIME_SIZE + 1];
  char			buft2[IBAS_ALERT_TIME_SIZE + 1];
  char			buft3[IBAS_ALERT_TIME_SIZE + 1];
  char			buft4[IBAS_ALERT_COMMENT_SIZE + 1];
  struct hostent	*hp;
  struct sigaction	sa;
  struct in_addr	in;
  IBC_DL		dl;

		/* scan command line */

if ((4 != argc) && (5 != argc)) usage(argv[0]);
if (5 == argc)  { sysprg = argv[4]; } 
else  { sysprg = NULL; }

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

		/* before initialization, setup handler for signal forcing PING */

sa.sa_handler = custom_signal_handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;
sigaction(SIGHUP, &sa, NULL); 

		/* initialize our stuff */

if (IBC_OK != (r = ibc_api_init(ibas_ip, ibas_port, my_port, &ping_signalled)))
  { printf("ibc_api_init() failed, error code = %d\n", r);
    exit(r);
  }

pingseq = 0;

for (;;)
 {
   r = ibc_api_listen(&dl);
   if (IBC_SIGNALLED == r)
     {
       r = ibc_api_send_ping(pingseq);
       if (IBC_OK != r)
         { printf("WARNING: ibc_api_send_ping() failed, error code = %d\n", r); }
       else
         { printf("PING_QUERY: packet sent out: seqnum = %d\n", pingseq);
         }
       pingseq++;
       continue;
     }
   if (IBC_AGAIN == r)					/* only possible in NONBLOCK mode */
     { ibc_sleep_msec(100, NULL);
       continue;
     }
   if (IBC_OK != r) break;				/* ctrl-C breaks out of this loop */

   if (IBAS_ALERT_TEST_FLAG_R_PING & dl.a.test_flag)
     { ibc_api_dump_ping_reply(&dl, ibas_ip, ibas_port);
     }
   else
     {
       if (NULL != sysprg)
         {
	   strncpy(buft1, dl.a.pkt_time, IBAS_ALERT_TIME_SIZE);
	   buft1[IBAS_ALERT_TIME_SIZE] = 0;
	   if (0 == buft1[0])  strcat(buft1, "EMPTY_FIELD");
	   strncpy(buft2, dl.a.nx_point_time, IBAS_ALERT_TIME_SIZE);
	   buft2[IBAS_ALERT_TIME_SIZE] = 0;
	   if (0 == buft2[0])  strcat(buft2, "EMPTY_FIELD");
	   strncpy(buft3, dl.a.grb_time, IBAS_ALERT_TIME_SIZE);
	   buft3[IBAS_ALERT_TIME_SIZE] = 0;
	   if (0 == buft3[0])  strcat(buft3, "EMPTY_FIELD");
	   strncpy(buft4, dl.a.grb_time, IBAS_ALERT_COMMENT_SIZE);
	   buft4[IBAS_ALERT_COMMENT_SIZE] = 0;
	   if (0 == buft4[0])  strcat(buft4, "EMPTY_FIELD");
	   sprintf(syscmd, "%s %d %d %d %d %d %d %d %s %d %d %.5lf %.5lf %s %s %.5lf %.5lf %.5lf %.5lf %.5lf %.5lf %.5lf %.5lf %d %d %d \"%s\" %d",
	   			sysprg,
				dl.ID, dl.pid, dl.seqnum, dl.handle, dl.a.pkt_type, dl.a.test_flag, dl.a.pkt_number, buft1,
				dl.a.alert_number, dl.a.alert_subnum, dl.a.nx_point_ra, dl.a.nx_point_dec, buft2,
				buft3, dl.a.grb_time_err, dl.a.grb_ra, dl.a.grb_dec, dl.a.grb_pos_err, dl.a.grb_sigma, dl.a.grb_timescale,
				dl.a.point_ra, dl.a.point_dec, dl.a.det_flags, dl.a.att_flags, dl.a.mult_pos, dl.a.comment, dl.a.object_type);
	   system(syscmd);
         }

       ibc_api_dump_alert(&dl, ibas_ip, ibas_port);
     }
 }

ibc_api_shutdown();

return(0);
}
