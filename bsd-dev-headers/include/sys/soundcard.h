#ifndef SYS_SOUNDCARD_H
#define SYS_SOUNDCARD_H

/**
 * @file soundcard.h
 * @brief OSS/FreeBSD audio device ioctl definitions (<sys/soundcard.h>).
 */

/* clang-format off */
#include <sys/ioccom.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#define AFMT_QUERY 0x00000000
#define AFMT_MU_LAW 0x00000001
#define AFMT_A_LAW 0x00000002
#define AFMT_IMA_ADPCM 0x00000004
#define AFMT_U8 0x00000008
#define AFMT_S16_LE 0x00000010
#define AFMT_S16_BE 0x00000020
#define AFMT_S8 0x00000040
#define AFMT_U16_LE 0x00000080
#define AFMT_U16_BE 0x00000100
#define AFMT_MPEG 0x00000200
#define AFMT_AC3 0x00000400
#define AFMT_S32_LE 0x00001000
#define AFMT_S32_BE 0x00002000
#define AFMT_S24_LE 0x00010000
#define AFMT_S24_BE 0x00020000
#define AFMT_F32_LE 0x00100000
#define AFMT_S16_NE AFMT_S16_LE

#define SNDCTL_DSP_RESET _IO('P', 0)
#define SNDCTL_DSP_SYNC _IO('P', 1)
#define SNDCTL_DSP_SPEED _IOWR('P', 2, int)
#define SNDCTL_DSP_STEREO _IOWR('P', 3, int)
#define SNDCTL_DSP_GETBLKSIZE _IOWR('P', 4, int)
#define SNDCTL_DSP_SETFMT _IOWR('P', 5, int)
#define SNDCTL_DSP_CHANNELS _IOWR('P', 6, int)
#define SNDCTL_DSP_POST _IO('P', 8)
#define SNDCTL_DSP_SUBDIVIDE _IOWR('P', 9, int)
#define SNDCTL_DSP_SETFRAGMENT _IOWR('P', 10, int)
#define SNDCTL_DSP_GETFMTS _IOR('P', 11, int)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_SOUNDCARD_H */
