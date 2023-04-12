/* Port番地の指定　Yellow推奨の記述法 */
#define  pio16_0 (*((volatile unsigned short *)(0x800000))) /* SH2MB */
#define  pio16_1 (*((volatile unsigned short *)(0x800002)))
#define  pio_cntl (*((volatile unsigned short *)(0x800004)))

