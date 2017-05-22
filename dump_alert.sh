#!/bin/sh

# this simple script can be called from  ibas_alert_receiver whenever
# new IBAS alert is received. All you have to do is to specify the name
# this script as argv[4] when running ibas_alert_receiver program.
#
# version 2.0.0, 04-Sep-2004


echo "============== IBAS ALERT ============="
echo  "ID              = " ${1}
echo  "pid             = " ${2}
echo  "seqnum          = " ${3}
echo  "handle          = " ${4}
echo  "pkt_type        = " ${5}
echo  "test_flag       = " ${6}
echo  "pkt_number      = " ${7}
echo  "pkt_time        = " ${8}
echo  "alert_number    = " ${9}
echo  "alert_subnum    = " ${10}
echo  "nx_point_ra     = " ${11}
echo  "nx_point_dec    = " ${12}
echo  "nx_point_time   = " ${13}
echo  "grb_time        = " ${14}
echo  "grb_time_err    = " ${15}
echo  "grb_ra          = " ${16}
echo  "grb_dec         = " ${17}
echo  "grb_pos_err     = " ${18}
echo  "grb_sigma       = " ${19}
echo  "grb_timescale   = " ${20}
echo  "point_ra        = " ${21}
echo  "point_dec       = " ${22}
echo  "det_flags       = " ${23}
echo  "att_flags       = " ${24}
echo  "mult_pos        = " ${25}
echo  "comment         = " ${26}
echo  "object_type     = " ${27}
