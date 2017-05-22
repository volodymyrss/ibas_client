
all:
		@echo "To build IBAS_CLIENT software please type one of the following :"
		@echo " "
		@echo "      make -f Makefile.linux-gcc"
		@echo "      make -f Makefile.solaris-gcc"
		@echo "      make -f Makefile.solaris-workshop"
		@echo "      make -f Makefile.macosx-cc"
		@echo " "
		@echo "Only 32-bit architectures were tested ! It __may__ work on 64-bit archs though ..."
		@echo " "

clean:
		/bin/rm -f *.o *.a *~ *.bak ibas_ping ibas_client-*.*.*.tar* ibas_ping ibas_alert_receiver

targz:		clean
		tar cvf ibas_client-2.1.0.tar *
		gzip ibas_client-2.1.0.tar

# everything below is for compatibility with ISDC's makefiles
# please ignore everything below this line (but do not delete)

global_install:
		@if test "x" != "x`grep Linux /proc/version`"; then \
			$(MAKE) -f Makefile.linux-gcc; \
		else \
			$(MAKE) -f Makefile.solaris-workshop; \
		fi;
		/bin/cp ibas_ping ${ISDC_ENV}/bin/ibas_ping
		/bin/chmod 755 ${ISDC_ENV}/bin/ibas_ping
		/bin/cp ibas_alert_receiver ${ISDC_ENV}/bin/ibas_alert_receiver
		/bin/chmod 755 ${ISDC_ENV}/bin/ibas_alert_receiver
		/bin/cp dump_alert.sh ${ISDC_ENV}/bin/dump_alert.sh
		/bin/chmod 755 ${ISDC_ENV}/bin/dump_alert.sh
		/bin/cp libibas_client.a ${ISDC_ENV}/lib/libibas_client.a
		/bin/chmod 644 ${ISDC_ENV}/lib/libibas_client.a
		/bin/cp ibas_client*.h ${ISDC_ENV}/include/.
		/bin/chmod 644 ${ISDC_ENV}/include/ibas_client*.h
		@echo " "
		@echo " "
		@echo "      ibas_client software has been installed under ${ISDC_ENV}"
		@echo " "
		@echo " "

test:
		( cd unit_test && README.test )
