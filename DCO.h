#ifndef DCO_H_
#define DCO_H_

#define CPU_FREQ 24000000
#define __delay_us(t_us) (__delay_cycles((((uint64_t)t_us)*CPU_FREQ) / 1000000))

#define FREQ_15_MHZ 1500000
#define FREQ_3_MHZ 3000000
#define FREQ_6_MHZ 6000000
#define FREQ_12_MHZ 12000000
#define FREQ_24_MHZ 24000000

#define PERIOD 2400

void set_DCO(uint32_t frequency);

#endif /* DCO_H_ */