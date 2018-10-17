#ifndef TIMER_H_
#define TIMER_H_

#define UINT32_T uint32_t
#define UINT64_T uint64_t

#define HRT_CALIBRATE(freq) do {  \
  static volatile HRT_TIMESTAMP_T t1, t2; \
  static volatile UINT64_T elapsed_ticks, min = (UINT64_T)(~0x1); \
  int notsmaller=0; \
  while(notsmaller<3) { \
    HRT_GET_TIMESTAMP(t1); \
     sleep(1);  \
    /* nanosleep((struct timespec[]){{0, 10000000}}, NULL); */ \
    HRT_GET_TIMESTAMP(t2); \
    HRT_GET_ELAPSED_TICKS(t1, t2, &elapsed_ticks); \
    notsmaller++; \
    if(elapsed_ticks<min) { \
      min = elapsed_ticks; \
      notsmaller = 0; \
    } \
  } \
  freq = min; \
} while(0);

#define HRT_INIT(freq) HRT_CALIBRATE(freq)

#define HRT_TIMESTAMP_T x86_64_timeval_t

#define HRT_GET_TIMESTAMP(t1)  __asm__ __volatile__ ("rdtsc" : "=a" (t1.l), "=d" (t1.h));

#define HRT_GET_ELAPSED_TICKS(t1, t2, numptr)   *numptr = (((( UINT64_T ) t2.h) << 32) | t2.l) - \
                                                          (((( UINT64_T ) t1.h) << 32) | t1.l);


typedef struct {
    UINT32_T l;
    UINT32_T h;
} x86_64_timeval_t;

/* global timer frequency in Hz */
extern unsigned long long g_timerfreq;

#endif /* TIMER_H_ */
