/* ���I�ϐ��錾 */
/* 20220914 revised */
char
      my_station_name[20],
      str_latitude[40],str_longitude[40],
      latitude[16][40],longitude[16][40],
      station_name[16][20],
      code_name[16],
      version[64],      
      file_name[20];       /* save_corr_data(),save_raw_data()�Ŏg�p */
//char  bf[128];             /* working�Ɏg�p */      
char  record_media[8];     /* Added 20220906 */
int   synchro_flag;

/* cfg����ǂݍ��܂���ϐ� */
int
      gps_operation,       /* Added in 20210519 */
      receive_mode,        /* nornal receive(0) or ring buffer pileup receive(1) */
      richtig_freq1000,    /* 2013 1.31 added */
      total_st,            /* ���ǐ� */
      my_station=0,        /* ���ǔԍ� */
      wakeup_interval,     /* wakeup interval�iprogram�̒��ł͕b�P�ʂ̒l�j */
      round_interval,      /* �e�ǊԂ�interval(sec) */
      t_freq_common,
      mod_num,             /* �ϒ����x(wave no/digit) */
      code_repeat,         /* ���g�o�͉� */
      extra_record_width,  /* extra A/D record length (msec) */
      sound_speed,
      burst_total_num,     /* �o�[�X�g���[�h�ł̑��M�� */
      burst_interval,      /* �o�[�X�g�Ԋu */
      code_no[16],         /* �ő�16 station���p�ӂ��� */
      t_freq[16],          /* round_robin�Ŏg����transmit frequency(Hz) */
      test_time,
      start_time,          /* �����J�n�����i�b�j */
      end_time;

/* experiment_set,prepare_reveive_concurrent�Ōv�Z�������� */
int
      distance[16],        /* my_station����̋���(m) */
      tr_time[16],         /* my_station�ɓ��B���鉹�̓`�d�\�z����(msec) */
      order[16],           /* ���B���Ԃ̑�������station�ԍ� */
      code_length,
      true_sampling_freq,
      one_period_pair,     /* mod_num*2*code_length (experiment_set.c) */
      sound_width_count,   /* sound_width��clock�ŃJ�E���g������ */
      ad_start_count,
      ad_wait_count,       /* use at receive_raw.c */
      save_pair_no,
      ad_convert_pair_no,  /* (cos,sin) pair number */
      superimpose_count;   /* superimpose no */

int   m_seq[16][128];         /* m-sequence */
short *mcode;
short *p_data;
