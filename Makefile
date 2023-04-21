##
## Makefile for building tomo.exe
##
## Created: Apr. 21, 2023
##

# WINGCC directory installed
WINGCC_DIR=C:/wingcc

TARGET=tomo.exe
SRCS=main_KIOST.c default_set.c cpu_register_set.c experiment_set.c read_cfg_file.c calc_distance.c \
	modification_code_set.c make_tr_data.c ping_time_schedule.c concurrent.c round_robin.c wait_time.c \
	transmit_fast.c receive.c save_raw_data.c shutdown.c standby.c user_mode.c term_replace_cfg.c \
	prepare_receive_conccurent.c CSAC_command.c ublox_LEA_6_8T.c SH2_serial_10MHz.c SH2_RTC.c power_cntrl.c \
	date_sec_convert.c clock_pulse_generate.c command_like.c adjust_clock_using_GPS.c log.c

OBJS=$(SRCS:.c=.o)


CC=sh-coff-gcc
CFLAGS=-B $(WINGCC_DIR)/local/sh-tools/sh-coff -I$(WINGCC_DIR)/app -m2 -O2
LDFLAGS=-Wl,--section-start=.stack=0x600 -r -B $(WINGCC_DIR)/local/sh-tools/sh-coff -L$(WINGCC_DIR)/app/ -m2 \
	-T $(WINGCC_DIR)/app/shpic.x -nostartfiles $(WINGCC_DIR)/app/shcrt0.S
LDLIBS=-lmes_sh -lm

all : $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $^ $(LDLIBS)

clean:
	del $(OBJS) $(TARGET)
