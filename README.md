# SH7145_KIOST

## Prerequisites
### git 설치
portablegit 다운로드 및 설치: https://git-scm.com/download/win
### git으로 SH7145_KIOST repository를 clone 하기
git 설치 후에 git 실행경로를 지정한 후에 아래와 같이 clone을 한다.
```
e:\SH7145>git clone https://github.com/Sung-Chun/SH7145_KIOST.git
```

## 빌드환경 셋업
### wingcc 컴파일러 설치
* SH7145_KIOST repository를 clone하고 나면, 해당 폴더 아래에 소스코드와 gcc_install 폴더가 존재한다.
* gcc_install\gcc25r19.exe 설치파일을 이용하여 윈도우에 wingcc를 설치한다.
* 설치가 완료되면 C:\wingcc 폴더에 설치가 된다.

### wingcc 컴파일러 실행경로 지정
* command 창을 실행한다.
* command 창에서 wingcc의 실행경로를 지정한다.
```
e:\SH7145>set path=%path%;c:\wingcc\local\sh-tools\bin
```
![image](https://user-images.githubusercontent.com/76480749/233659546-da3132ed-669a-4689-bbeb-2c504220d50d.png)

## 소스코드 빌드
#### SH7145_KIOST repository 폴더로 이동하여 make를 수행한다.
* make 명령어는 c:\wingcc\local\sh-tools\bin 폴더의 make.exe를 실행하는 것이므로 command 창에서 wingcc 컴파일러 실행경로 지정이 되어 있어야 합니다.
* 빌드 옵션 설정은 Makefile을 참고하시고, 소스코드 파일을 추가하시려면 Makefile을 수정하시면 됩니다.

```
e:\SH7145> cd e:\SH7145\SH7145_KIOST

e:\SH7145\SH7145_KIOST>
e:\SH7145\SH7145_KIOST> make
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o main_KIOST.o main_KIOST.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o default_set.o default_set.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o cpu_register_set.o cpu_register_set.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o experiment_set.o experiment_set.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o read_cfg_file.o read_cfg_file.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o calc_distance.o calc_distance.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o modification_code_set.o modification_code_set.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o make_tr_data.o make_tr_data.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o ping_time_schedule.o ping_time_schedule.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o concurrent.o concurrent.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o round_robin.o round_robin.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o wait_time.o wait_time.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o transmit_fast.o transmit_fast.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o receive.o receive.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o save_raw_data.o save_raw_data.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o shutdown.o shutdown.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o standby.o standby.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o user_mode.o user_mode.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o term_replace_cfg.o term_replace_cfg.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o prepare_receive_conccurent.o prepare_receive_conccurent.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o CSAC_command.o CSAC_command.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o ublox_LEA_6_8T.o ublox_LEA_6_8T.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o SH2_serial_10MHz.o SH2_serial_10MHz.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o SH2_RTC.o SH2_RTC.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o power_cntrl.o power_cntrl.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o date_sec_convert.o date_sec_convert.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o clock_pulse_generate.o clock_pulse_generate.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o command_like.o command_like.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o adjust_clock_using_GPS.o adjust_clock_using_GPS.c
sh-coff-gcc -B C:/wingcc/local/sh-tools/sh-coff -IC:/wingcc/app -m2 -O2   -c -o log.o log.c
sh-coff-gcc -Wl,--section-start=.stack=0x600 -r -B C:/wingcc/local/sh-tools/sh-coff -LC:/wingcc/app/ -m2 -T C:/wingcc/app/shpic.x -nostartfiles C:/wingcc/app/shcrt0.S -o tomo.exe main_KIOST.o default_set.o cpu_register_set.o experiment_set.o read_cfg_file.o calc_distance.o modification_code_set.o make_tr_data.o ping_time_schedule.o concurrent.o
round_robin.o wait_time.o transmit_fast.o receive.o save_raw_data.o shutdown.o standby.o user_mode.o term_replace_cfg.o prepare_receive_conccurent.o CSAC_command.o ublox_LEA_6_8T.o SH2_serial_10MHz.o SH2_RTC.o power_cntrl.o date_sec_convert.o clock_pulse_generate.o command_like.o adjust_clock_using_GPS.o log.o -lmes_sh -lm
```

#### tomo.exe 파일이 생성된 것을 확인한다.
```
e:\SH7145\SH7145_KIOST> dir tomo.exe

2023-04-21  오후 11:26           273,047 tomo.exe
               1개 파일             273,047 바이트
               0개 디렉터리  403,373,252,608 바이트 남음
```

#### 추가 전달 사항
* 소스코드 수정 후에 make를 실행하면, 수정된 파일만 빌드합니다.
* 클린 빌드를 하려면, make clean을 먼저 실행해서 tomo.exe랑 obj 파일들을 삭제한 후에 다시 make를 실행하십시오.

```
e:\SH7145\SH7145_KIOST> make clean
del main_KIOST.o default_set.o cpu_register_set.o experiment_set.o read_cfg_file.o calc_distance.o modification_code_set.o make_tr_data.o ping_time_schedule.o concurrent.o
round_robin.o wait_time.o transmit_fast.o receive.o save_raw_data.o shutdown.o standby.o user_mode.o term_replace_cfg.o prepare_receive_conccurent.o CSAC_command.o ublox_LEA_6_8T.o SH2_serial_10MHz.o SH2_RTC.o power_cntrl.o date_sec_convert.o clock_pulse_generate.o command_like.o adjust_clock_using_GPS.o log.o tomo.exe

e:\SH7145\SH7145_KIOST> make
  ...
```

## <strike>Log 폴더 rolling 설정</strike>
#### Log 폴더의 rolling은 tomo.cfg 파일 설정에 따라 "매일", "매월 d일", "매주 w요일" rolling이 되도록 설정을 할 수 있다.

|설정 값|설명|비고|
|------|---|---------|
|D 0|매일 폴더명이 변경|보통 테스트 목적으로 활용|
|D d|매월 d일 마다 폴더명이 변경|d는 1 ~ 28 사이의 값만 설정 가능|
|W w|매주 w요일 마다 폴더명이 변경|w는 0 ~ 6 사이의 숫자 값만 설정 가능 (0: 일, 1:월, 2:화, 3:수, 4:목, 5:금, 6:토)|

## Test Report
#### 설정 대로 로그폴더 생성되고 저장되는지 테스트 진행한 결과입니다.

설정 대로 로그폴더 잘 생성되고 저장되는 것 테스트하여 모두 정상 동작하는 것 확인 완료
|테스트|설정|결과|설명|
|------|---|--|---------|
|매일 폴더명 변경|D 0|OK|매일 0시에 폴더 생성 확인|
|매월 d일마다 폴더명 변경|D 22|OK|2023/4/21에 동작 시 20230322 폴더 생성 및 4/22일 0시에 20230422 폴더 생성 확인|
|매주 w요일마다 폴더명 변경|W 0|OK|2023/4/22에 동작 시 20230416 폴더 생성 및 4/23일 0시에 20230423 폴더 생성 확인|
|연도 변경 시 폴더명 변경 1|D 28|OK|2023/1/3로 날짜 강제설정 후 동작 시 20221228 폴더 생성 확인|
|연도 변경 시 폴더명 변경 2|W 3|OK|2023/1/3로 날짜 강제설정 후 동작 시 20221228 폴더 생성 확인|

## <strike>USB 메모리로 이동할 지 여부를 설정</strike>
#### 매번 standby 진입 직전에 USB 메모리로 raw file들을 이동할 지 여부를 tomo.cfg 파일에서 설정할 수 있다.
|설정 값|설명|
|------|--------|
|Y|USB 메모리로 raw file들을 이동한다.|
|N|이동하지 않는다.|

## Test Report
#### 설정 대로 raw file 들을 이동하는지 테스트 진행한 결과입니다..

설정 대로 로그폴더 잘 생성되고 저장되는 것 테스트하여 모두 정상 동작하는 것 확인 완료
|테스트|설정|결과|설명|
|------|---|--|---------|
|이동하지 않음 설정|N|OK|raw file들을 이동하지 않음 확인|
|/spc0/data 폴더에 아무 것도 없는 상태에서 테스트|Y|OK|20230506 폴더가 생성되며 1~5개 파일이 /spc0/data_bak 폴더로 이동 확인|
|/mmc0/data 폴더에 아무 것도 없는 상태에서 테스트|Y|테스트 예정|-|
|/mmc0/data 폴더에 1GB 쌓인 상태에서 테스트|Y|테스트 예정|-|
|/mmc0/data 폴더에 2GB 쌓인 상태에서 테스트|Y|테스트 예정|-|

## 2023-05-23: Revoke to no YYYYMMDD log folder and no backup to USB
* 날짜 로그 폴더 (YYYYMMDD) 생성하는 로직을 삭제함
* USB로 백업하는 로직을 삭제함
* tomo.cfg 에서 날짜 및 USB 백업 설정 부분을 삭제함
