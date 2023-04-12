/* 大域的変数宣言 */
/* 20220914 revised */
char
      my_station_name[20],
      str_latitude[40],str_longitude[40],
      latitude[16][40],longitude[16][40],
      station_name[16][20],
      code_name[16],
      version[64],      
      file_name[20];       /* save_corr_data(),save_raw_data()で使用 */
//char  bf[128];             /* workingに使用 */      
char  record_media[8];     /* Added 20220906 */
int   synchro_flag;

/* cfgから読み込まれれる変数 */
int
      gps_operation,       /* Added in 20210519 */
      receive_mode,        /* nornal receive(0) or ring buffer pileup receive(1) */
      richtig_freq1000,    /* 2013 1.31 added */
      total_st,            /* 総局数 */
      my_station=0,        /* 自局番号 */
      wakeup_interval,     /* wakeup interval（programの中では秒単位の値） */
      round_interval,      /* 各局間のinterval(sec) */
      t_freq_common,
      mod_num,             /* 変調速度(wave no/digit) */
      code_repeat,         /* 音波出力回数 */
      extra_record_width,  /* extra A/D record length (msec) */
      sound_speed,
      burst_total_num,     /* バーストモードでの送信回数 */
      burst_interval,      /* バースト間隔 */
      code_no[16],         /* 最大16 station分用意する */
      t_freq[16],          /* round_robinで使われるtransmit frequency(Hz) */
      test_time,
      start_time,          /* 実験開始時刻（秒） */
      end_time;

/* experiment_set,prepare_reveive_concurrentで計算されるもの */
int
      distance[16],        /* my_stationからの距離(m) */
      tr_time[16],         /* my_stationに到達する音の伝播予想時間(msec) */
      order[16],           /* 到達時間の早い順のstation番号 */
      code_length,
      true_sampling_freq,
      one_period_pair,     /* mod_num*2*code_length (experiment_set.c) */
      sound_width_count,   /* sound_widthをclockでカウントした数 */
      ad_start_count,
      ad_wait_count,       /* use at receive_raw.c */
      save_pair_no,
      ad_convert_pair_no,  /* (cos,sin) pair number */
      superimpose_count;   /* superimpose no */

int   m_seq[16][128];         /* m-sequence */
short *mcode;
short *p_data;
