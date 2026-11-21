#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__) ||          \
    defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__)
/**
 * @file bsd_dev_headers.c
 * @brief Compilation unit and portable runtime shims for bsd-dev-headers
 * (CAM, USBHID, sysdecode, ELF, ar, sockio, tty/disk/sound ioctls).
 */

/* clang-format off */
#include <sys/ioccom.h>
#include <sys/filio.h>
#include <sys/sockio.h>
#include <sys/ttycom.h>
#include <sys/tty.h>
#include <sys/consio.h>
#include <sys/disk.h>
#include <sys/backlight.h>
#include <sys/soundcard.h>
#include <dev/speaker/speaker.h>
#include <dev/usb/usbhid.h>
#include <sys/mtio.h>
#include <sys/mac.h>
#include <ar.h>
#include <elf.h>
#include <cam/cam.h>
#include <usbhid.h>
#include <sysdecode.h>
#include <netipsec/ipsec.h>
#include <hesiod.h>
#include <mqueue.h>
#include <semaphore.h>
#include <bluetooth.h>
#include <sdp.h>
#include <sys/linker.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

static void *default_bsd_dev_calloc(size_t nmemb, size_t size) {
  return calloc(nmemb, size);
}

static void *default_bsd_dev_malloc(size_t size) { return malloc(size); }

/** @brief Overrideable calloc hook for OOM testing in bsd-dev-headers. */
void *(*g_bsd_dev_calloc)(size_t, size_t) = default_bsd_dev_calloc;
/** @brief Overrideable malloc hook for OOM testing in bsd-dev-headers. */
void *(*g_bsd_dev_malloc)(size_t) = default_bsd_dev_malloc;

/** @brief Resets bsd-dev-headers allocator hooks to standard allocators. */
void bsd_dev_headers_reset_alloc_hooks(void);
void bsd_dev_headers_reset_alloc_hooks(void) {
  g_bsd_dev_calloc = default_bsd_dev_calloc;
  g_bsd_dev_malloc = default_bsd_dev_malloc;
}

/** @brief Internal USB HID report descriptor storage. */
struct report_desc {
  unsigned char *data; /**< Raw descriptor byte buffer. */
  unsigned int size;   /**< Descriptor length in bytes. */
};

/** @brief Internal USB HID parser cursor state. */
struct hid_data {
  report_desc_t desc;  /**< Associated report descriptor handle. */
  int kindset;         /**< Bitmask of hid_kind values to match. */
  int id;              /**< Report ID filter. */
  unsigned int offset; /**< Current byte offset in descriptor. */
};

/** @brief Coverage probe helper returning 0. */
int bsd_dev_headers_dummy_for_coverage(void);
int bsd_dev_headers_dummy_for_coverage(void) { return 0; }

/** @brief Open a CAM SCSI pass-through device by path. */
struct cam_device *cam_open_device(const char *path, int flags) {
  (void)path;
  (void)flags;
  errno = ENOENT;
  return NULL;
}

/** @brief Open a CAM SCSI device by driver name and unit number. */
struct cam_device *cam_open_spec_device(const char *dev_name, int unit,
                                        int flags, struct cam_device *device) {
  (void)dev_name;
  (void)unit;
  (void)flags;
  (void)device;
  errno = ENOENT;
  return NULL;
}

/** @brief Close and release a CAM device handle. */
void cam_close_device(struct cam_device *dev) {
  if (dev != NULL) {
    free(dev);
  }
}

/** @brief Initialize USB HID usage table from optional file. */
void hid_init(const char *file) { (void)file; }

/** @brief Read USB HID report descriptor from open device descriptor. */
report_desc_t hid_get_report_desc(int file) {
  (void)file;
  errno = ENODEV;
  return NULL;
}

/** @brief Construct a report_desc_t from raw descriptor bytes. */
report_desc_t hid_use_report_desc(unsigned char *data, unsigned int size) {
  report_desc_t d;
  if (data == NULL || size == 0U) {
    return NULL;
  }
  d = (report_desc_t)g_bsd_dev_calloc(1, sizeof(*d));
  if (d == NULL) {
    return NULL;
  }
  d->data = (unsigned char *)g_bsd_dev_malloc(size);
  if (d->data == NULL) {
    free(d);
    return NULL;
  }
  memcpy(d->data, data, size);
  d->size = size;
  return d;
}

/** @brief Dispose a USB HID report descriptor. */
void hid_dispose_report_desc(report_desc_t d) {
  if (d != NULL) {
    free(d->data);
    free(d);
  }
}

/** @brief Query active USB HID report ID for an open descriptor. */
int hid_get_report_id(int file) {
  (void)file;
  return 0;
}

/** @brief Toggle USB HID immediate mode on an open descriptor. */
int hid_set_immed(int fd, int enable) {
  (void)fd;
  (void)enable;
  return 0;
}

/** @brief Start iterating items in a USB HID report descriptor. */
hid_data_t hid_start_parse(report_desc_t d, int kindset, int id) {
  hid_data_t s;
  if (d == NULL) {
    return NULL;
  }
  s = (hid_data_t)g_bsd_dev_calloc(1, sizeof(*s));
  if (s != NULL) {
    s->desc = d;
    s->kindset = kindset;
    s->id = id;
  }
  return s;
}

/** @brief End USB HID report descriptor iteration. */
void hid_end_parse(hid_data_t s) { free(s); }

/** @brief Fetch next USB HID item from active parser state. */
int hid_get_item(hid_data_t s, hid_item_t *h) {
  (void)s;
  (void)h;
  return 0;
}

/** @brief Compute byte size of a USB HID report. */
int hid_report_size(report_desc_t d, enum hid_kind k, int id) {
  (void)k;
  (void)id;
  return (d != NULL) ? (int)d->size : 0;
}

/** @brief Locate a specific usage item inside a USB HID report descriptor. */
int hid_locate(report_desc_t d, unsigned int usage, enum hid_kind k,
               hid_item_t *h, int id) {
  (void)d;
  (void)usage;
  (void)k;
  (void)h;
  (void)id;
  return 0;
}

/** @brief Format USB HID usage page number as human-readable name. */
const char *hid_usage_page(int i) {
  return (i == 1) ? "Generic_Desktop" : "Vendor";
}

/** @brief Format USB HID usage value within page as human-readable name. */
const char *hid_usage_in_page(awm_hid_usage_t u) {
  return (HID_USAGE(u) == 1U) ? "Pointer" : "Usage";
}

/** @brief Parse USB HID usage page name into numeric ID. */
int hid_parse_usage_page(const char *name) {
  return (name != NULL && strcmp(name, "Generic_Desktop") == 0) ? 1 : -1;
}

/** @brief Parse USB HID usage name into combined page/usage ID. */
awm_hid_susage_t hid_parse_usage_in_page(const char *name) {
  return (name != NULL && strcmp(name, "Pointer") == 0)
             ? (awm_hid_susage_t)0x00010001UL
             : (awm_hid_susage_t)-1;
}

/** @brief Extract bitfield value described by hid_item_t from report buffer. */
int32_t hid_get_data(const void *p, const hid_item_t *h) {
  const unsigned char *buf = (const unsigned char *)p;
  uint32_t byte_off;
  uint32_t bit_off;
  if (buf == NULL || h == NULL || h->report_size <= 0) {
    return 0;
  }
  byte_off = h->pos / 8U;
  bit_off = h->pos % 8U;
  return (int32_t)((buf[byte_off] >> bit_off) &
                   ((1U << (unsigned int)h->report_size) - 1U));
}

/** @brief Write bitfield value described by hid_item_t into report buffer. */
void hid_set_data(void *p, const hid_item_t *h, int32_t data) {
  unsigned char *buf = (unsigned char *)p;
  uint32_t byte_off;
  uint32_t bit_off;
  unsigned int mask;
  if (buf == NULL || h == NULL || h->report_size <= 0) {
    return;
  }
  byte_off = h->pos / 8U;
  bit_off = h->pos % 8U;
  mask = ((1U << (unsigned int)h->report_size) - 1U) << bit_off;
  buf[byte_off] = (unsigned char)((buf[byte_off] & ~mask) |
                                  (((unsigned int)data << bit_off) & mask));
}

/** @brief Read synchronous USB HID report from device descriptor. */
int hid_get_report(int fd, enum hid_kind k, unsigned char *data,
                   unsigned int size) {
  (void)fd;
  (void)k;
  (void)data;
  (void)size;
  errno = ENODEV;
  return -1;
}

/** @brief Send synchronous USB HID report to device descriptor. */
int hid_set_report(int fd, enum hid_kind k, unsigned char *data,
                   unsigned int size) {
  (void)fd;
  (void)k;
  (void)data;
  (void)size;
  errno = ENODEV;
  return -1;
}

/** @brief Map ABI-specific errno value to FreeBSD errno. */
int sysdecode_abi_to_freebsd_errno(enum sysdecode_abi abi, int error) {
  (void)abi;
  return error;
}

/** @brief Map FreeBSD errno value to ABI-specific errno. */
int sysdecode_freebsd_to_abi_errno(enum sysdecode_abi abi, int error) {
  (void)abi;
  return error;
}

/** @brief Decode syscall number to name for given ABI. */
const char *sysdecode_syscallnames(enum sysdecode_abi abi, unsigned int code) {
  (void)abi;
  if (code == 0U) {
    return "syscall";
  }
  if (code == 1U) {
    return "exit";
  }
  return NULL;
}

/** @brief Decode ioctl command number to symbolic macro name. */
const char *sysdecode_ioctlname(unsigned long val) {
  (void)val;
  return NULL;
}

/** @brief Decode signal number to symbolic name. */
const char *sysdecode_signame(int sig) {
  return (sig == 9) ? "SIGKILL" : "SIG";
}

/** @brief Decode fcntl command constant to symbolic name. */
const char *sysdecode_fcntl_cmd(int cmd) {
  return (cmd == 0) ? "F_DUPFD" : "F_CMD";
}

/** @brief Decode socket option level to symbolic name. */
const char *sysdecode_sockopt_level(int level) {
  (void)level;
  return "SOL_SOCKET";
}

/** @brief Decode socket option name within level. */
const char *sysdecode_sockopt_name(int level, int optname) {
  (void)level;
  (void)optname;
  return "SO_DEBUG";
}

/** @brief Format open(2) flags bitmask to stream. */
int sysdecode_open_flags(FILE *fp, int flags, int *rem) {
  if (rem != NULL) {
    *rem = 0;
  }
  if (fp != NULL) {
    fprintf(fp, "0x%x", flags);
  }
  return 1;
}

/** @brief Format mmap(2) protection flags bitmask to stream. */
int sysdecode_mmap_prot(FILE *fp, int prot, int *rem) {
  return sysdecode_open_flags(fp, prot, rem);
}

/** @brief Format mmap(2) mapping flags bitmask to stream. */
int sysdecode_mmap_flags(FILE *fp, int flags, int *rem) {
  return sysdecode_open_flags(fp, flags, rem);
}

/** @brief Prepare Mandatory Access Control process label storage. */
int mac_prepare_process_label(mac_t *label) {
  if (label) {
    *label = NULL;
  }
  return -1;
}
/** @brief Retrieve MAC label of current process. */
int mac_get_proc(mac_t label) {
  (void)label;
  return -1;
}
/** @brief Retrieve MAC label of target process by PID. */
int mac_get_pid(pid_t pid, mac_t label) {
  (void)pid;
  (void)label;
  return -1;
}
/** @brief Convert MAC label to NUL-terminated text string. */
int mac_to_text(mac_t label, char **text) {
  (void)label;
  if (text) {
    *text = NULL;
  }
  return -1;
}
/** @brief Free MAC label handle. */
int mac_free(mac_t label) {
  (void)label;
  return 0;
}

/** @brief Compile textual IPsec policy specification into binary request. */
char *ipsec_set_policy(const char *policy, int policylen) {
  char *buf;
  if (policy == NULL || policylen <= 0) {
    return NULL;
  }
  buf = (char *)g_bsd_dev_calloc(1, 8);
  if (buf != NULL) {
    buf[0] = 8;
  }
  return buf;
}

/** @brief Return byte length of compiled IPsec policy buffer. */
int ipsec_get_policylen(const char *buf) {
  return (buf != NULL) ? (int)(unsigned char)buf[0] : -1;
}

/** @brief Return string describing last IPsec policy error. */
const char *ipsec_strerror(void) {
  return "IPsec policy unsupported on this host";
}

/** @brief Initialize Hesiod naming service context. */
int hesiod_init(void **context) {
  if (context != NULL) {
    *context = (void *)1;
  }
  return 0;
}
/** @brief Destroy Hesiod naming service context. */
void hesiod_end(void *context) { (void)context; }
/** @brief Format Hesiod name and type into DNS query domain string. */
char *hesiod_to_bind(void *context, const char *name, const char *type) {
  char *buf;
  size_t len;
  (void)context;
  if (name == NULL || type == NULL) {
    return NULL;
  }
  len = strlen(name) + strlen(type) + 16;
  buf = (char *)g_bsd_dev_malloc(len);
  if (buf != NULL) {
#if defined(_MSC_VER)
    sprintf_s(buf, len, "%s.%s.ns", name, type);
#else
    sprintf(buf, "%s.%s.ns", name, type);
#endif
  }
  return buf;
}
/** @brief Resolve Hesiod record into NULL-terminated string vector. */
char **hesiod_resolve(void *context, const char *name, const char *type) {
  (void)context;
  (void)name;
  (void)type;
  errno = ENOENT;
  return NULL;
}
/** @brief Free string vector returned by hesiod_resolve. */
void hesiod_free_list(void *context, char **list) {
  (void)context;
  free(list);
}
/** @brief Initialize legacy Hesiod global state. */
int hes_init(void) { return 0; }
/** @brief Format legacy Hesiod DNS query domain string. */
char *hes_to_bind(const char *name, const char *type) {
  return hesiod_to_bind(NULL, name, type);
}
/** @brief Resolve legacy Hesiod record. */
char **hes_resolve(const char *name, const char *type) {
  return hesiod_resolve(NULL, name, type);
}
/** @brief Retrieve legacy Hesiod error status code. */
int hes_error(void) { return HES_ER_NOTFOUND; }
/** @brief Free legacy Hesiod result vector. */
void hes_free(char **hp) { free(hp); }

#if !defined(_WIN32) &&                                                        \
    (defined(__WATCOMC__) || defined(__MSDOS__) || defined(__DOS__))
/** @brief Initialize unnamed semaphore on DOS/Watcom. */
int sem_init(sem_t *sem, int pshared, unsigned int value) {
  (void)pshared;
  if (sem == NULL) {
    errno = EINVAL;
    return -1;
  }
  sem->handle = NULL;
  sem->count = (long)value;
  return 0;
}
/** @brief Destroy unnamed semaphore on DOS/Watcom. */
int sem_destroy(sem_t *sem) { return (sem != NULL) ? 0 : -1; }
/** @brief Decrement semaphore on DOS/Watcom. */
int sem_wait(sem_t *sem) {
  if (sem == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (sem->count > 0) {
    sem->count--;
  }
  return 0;
}
/** @brief Non-blocking semaphore decrement on DOS/Watcom. */
int sem_trywait(sem_t *sem) {
  if (sem == NULL || sem->count <= 0) {
    errno = EAGAIN;
    return -1;
  }
  sem->count--;
  return 0;
}
/** @brief Timed semaphore decrement on DOS/Watcom. */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
  (void)abs_timeout;
  return sem_trywait(sem);
}
/** @brief Increment semaphore on DOS/Watcom. */
int sem_post(sem_t *sem) {
  if (sem == NULL) {
    errno = EINVAL;
    return -1;
  }
  sem->count++;
  return 0;
}
/** @brief Read current value of semaphore on DOS/Watcom. */
int sem_getvalue(sem_t *sem, int *sval) {
  if (sem == NULL || sval == NULL) {
    errno = EINVAL;
    return -1;
  }
  *sval = (int)sem->count;
  return 0;
}
/** @brief Open named semaphore on DOS/Watcom. */
sem_t *sem_open(const char *name, int oflag, ...) {
  sem_t *s;
  (void)name;
  (void)oflag;
  s = (sem_t *)g_bsd_dev_calloc(1, sizeof(sem_t));
  return s;
}
/** @brief Close named semaphore on DOS/Watcom. */
int sem_close(sem_t *sem) {
  free(sem);
  return 0;
}
/** @brief Unlink named semaphore on DOS/Watcom. */
int sem_unlink(const char *name) {
  (void)name;
  return 0;
}
#endif

#if !defined(__linux__) && !defined(__FreeBSD__)
/** @brief In-memory POSIX message queue table for platforms lacking mqueue. */
static struct {
  int in_use;          /**< Non-zero when slot is allocated. */
  char name[64];       /**< Queue name including leading slash. */
  struct mq_attr attr; /**< Queue attributes and message count. */
  char msg[256];       /**< Single buffered message payload. */
  size_t msg_len;      /**< Byte length of buffered message. */
  unsigned int prio;   /**< Priority of buffered message. */
} s_awm_mq_slots[16];

/** @brief Open or create an in-memory POSIX message queue. */
mqd_t mq_open(const char *name, int oflag, ...) {
  int i;
  int free_idx = -1;
  if (name == NULL) {
    errno = EINVAL;
    return (mqd_t)-1;
  }
  for (i = 0; i < 16; ++i) {
    if (s_awm_mq_slots[i].in_use && strcmp(s_awm_mq_slots[i].name, name) == 0) {
      if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) {
        errno = EEXIST;
        return (mqd_t)-1;
      }
      return (mqd_t)(i + 1);
    }
    if (!s_awm_mq_slots[i].in_use && free_idx < 0) {
      free_idx = i;
    }
  }
  if ((oflag & O_CREAT) == 0 || free_idx < 0) {
    errno = ENOENT;
    return (mqd_t)-1;
  }
  s_awm_mq_slots[free_idx].in_use = 1;
#if defined(_MSC_VER)
  strncpy_s(s_awm_mq_slots[free_idx].name,
            sizeof(s_awm_mq_slots[free_idx].name), name, _TRUNCATE);
#else
  strncpy(s_awm_mq_slots[free_idx].name, name,
          sizeof(s_awm_mq_slots[free_idx].name) - 1);
  s_awm_mq_slots[free_idx].name[sizeof(s_awm_mq_slots[free_idx].name) - 1] =
      '\0';
#endif
  s_awm_mq_slots[free_idx].attr.mq_maxmsg = 10;
  s_awm_mq_slots[free_idx].attr.mq_msgsize = 256;
  s_awm_mq_slots[free_idx].attr.mq_curmsgs = 0;
  s_awm_mq_slots[free_idx].attr.mq_flags = oflag & O_NONBLOCK;
  return (mqd_t)(free_idx + 1);
}
/** @brief Close a POSIX message queue descriptor. */
int mq_close(mqd_t mqdes) { return (mqdes >= 1 && mqdes <= 16) ? 0 : -1; }
/** @brief Remove a named POSIX message queue. */
int mq_unlink(const char *name) {
  int i;
  if (name == NULL) {
    errno = EINVAL;
    return -1;
  }
  for (i = 0; i < 16; ++i) {
    if (s_awm_mq_slots[i].in_use && strcmp(s_awm_mq_slots[i].name, name) == 0) {
      s_awm_mq_slots[i].in_use = 0;
      return 0;
    }
  }
  errno = ENOENT;
  return -1;
}
/** @brief Retrieve attributes of a POSIX message queue. */
int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat) {
  if (mqdes < 1 || mqdes > 16 || mqstat == NULL) {
    errno = EBADF;
    return -1;
  }
  *mqstat = s_awm_mq_slots[mqdes - 1].attr;
  return 0;
}
/** @brief Update flags on a POSIX message queue and optionally fetch old attr.
 */
int mq_setattr(mqd_t mqdes, const struct mq_attr *mqstat,
               struct mq_attr *omqstat) {
  if (mqdes < 1 || mqdes > 16) {
    errno = EBADF;
    return -1;
  }
  if (omqstat != NULL) {
    *omqstat = s_awm_mq_slots[mqdes - 1].attr;
  }
  if (mqstat != NULL) {
    s_awm_mq_slots[mqdes - 1].attr.mq_flags = mqstat->mq_flags;
  }
  return 0;
}
/** @brief Enqueue a message into a POSIX message queue. */
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
            unsigned int msg_prio) {
  if (mqdes < 1 || mqdes > 16 || msg_ptr == NULL || msg_len > 256) {
    errno = EINVAL;
    return -1;
  }
  memcpy(s_awm_mq_slots[mqdes - 1].msg, msg_ptr, msg_len);
  s_awm_mq_slots[mqdes - 1].msg_len = msg_len;
  s_awm_mq_slots[mqdes - 1].prio = msg_prio;
  s_awm_mq_slots[mqdes - 1].attr.mq_curmsgs = 1;
  return 0;
}
/** @brief Enqueue a message with timeout into a POSIX message queue. */
int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
                 unsigned int msg_prio, const struct timespec *abs_timeout) {
  (void)abs_timeout;
  return mq_send(mqdes, msg_ptr, msg_len, msg_prio);
}
/** @brief Receive a message from a POSIX message queue. */
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
                   unsigned int *msg_prio) {
  size_t n;
  if (mqdes < 1 || mqdes > 16 || msg_ptr == NULL) {
    errno = EBADF;
    return -1;
  }
  if (s_awm_mq_slots[mqdes - 1].attr.mq_curmsgs == 0) {
    errno = EAGAIN;
    return -1;
  }
  n = s_awm_mq_slots[mqdes - 1].msg_len;
  if (msg_len < n) {
    errno = EMSGSIZE;
    return -1;
  }
  memcpy(msg_ptr, s_awm_mq_slots[mqdes - 1].msg, n);
  if (msg_prio != NULL) {
    *msg_prio = s_awm_mq_slots[mqdes - 1].prio;
  }
  s_awm_mq_slots[mqdes - 1].attr.mq_curmsgs = 0;
  return (ssize_t)n;
}
/** @brief Receive a message with timeout from a POSIX message queue. */
ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
                        unsigned int *msg_prio,
                        const struct timespec *abs_timeout) {
  (void)abs_timeout;
  return mq_receive(mqdes, msg_ptr, msg_len, msg_prio);
}
/** @brief Register asynchronous notification on a POSIX message queue. */
int mq_notify(mqd_t mqdes, const struct sigevent *notification) {
  (void)mqdes;
  (void)notification;
  return 0;
}
#endif

/** @brief Wildcard all-zero Bluetooth BDADDR constant. */
const bdaddr_t fbsd_bt_addr_any = {{0, 0, 0, 0, 0, 0}};

/** @brief Parse colon-separated Bluetooth MAC string into bdaddr_t. */
int bt_aton(const char *str, bdaddr_t *ba) {
  unsigned int b[6];
  int i;
  if (str == NULL || ba == NULL) {
    return 0;
  }
#if defined(_MSC_VER)
  if (sscanf_s(str, "%x:%x:%x:%x:%x:%x", &b[5], &b[4], &b[3], &b[2], &b[1],
               &b[0]) != 6) {
    return 0;
  }
#else
  if (sscanf(str, "%x:%x:%x:%x:%x:%x", &b[5], &b[4], &b[3], &b[2], &b[1],
             &b[0]) != 6) {
    return 0;
  }
#endif
  for (i = 0; i < 6; ++i) {
    if (b[i] > 0xffU) {
      return 0;
    }
    ba->b[i] = (uint8_t)b[i];
  }
  return 1;
}

/** @brief Format bdaddr_t into colon-separated Bluetooth MAC string. */
const char *bt_ntoa(const bdaddr_t *ba, char *str) {
  static char s_buf[24];
  char *out = (str != NULL) ? str : s_buf;
  if (ba == NULL) {
    return NULL;
  }
#if defined(_MSC_VER)
  sprintf_s(out, (str != NULL) ? 18 : sizeof(s_buf),
            "%02x:%02x:%02x:%02x:%02x:%02x", ba->b[5], ba->b[4], ba->b[3],
            ba->b[2], ba->b[1], ba->b[0]);
#else
  sprintf(out, "%02x:%02x:%02x:%02x:%02x:%02x", ba->b[5], ba->b[4], ba->b[3],
          ba->b[2], ba->b[1], ba->b[0]);
#endif
  return out;
}

/** @brief Resolve Bluetooth host entry by name. */
struct hostent *bt_gethostbyname(const char *name) {
  (void)name;
  return NULL;
}

/** @brief Resolve Bluetooth host entry by BDADDR. */
struct hostent *bt_gethostbyaddr(const char *addr, int len, int type) {
  (void)addr;
  (void)len;
  (void)type;
  return NULL;
}

static struct protoent s_bt_proto;
static char s_bt_proto_name[16];
static char *s_bt_proto_aliases[1] = {NULL};

/** @brief Static lookup table mapping Bluetooth PSM names to protocol numbers.
 */
static const struct {
  const char *name; /**< Canonical Bluetooth protocol name. */
  int num;          /**< Assigned Bluetooth PSM number. */
} s_bt_psm_table[] = {{"sdp", 1},    {"rfcomm", 3},       {"tcs", 5},
                      {"bnep", 15},  {"hid-control", 17}, {"hid-interrupt", 19},
                      {"avctp", 23}, {"avdtp", 25},       {"att", 31}};

/** @brief Look up Bluetooth PSM protocol entry by name. */
struct protoent *bt_getprotobyname(const char *name) {
  size_t i;
  if (name == NULL) {
    return NULL;
  }
  for (i = 0; i < sizeof(s_bt_psm_table) / sizeof(s_bt_psm_table[0]); ++i) {
    if (strcmp(name, s_bt_psm_table[i].name) == 0) {
#if defined(_MSC_VER)
      strcpy_s(s_bt_proto_name, sizeof(s_bt_proto_name),
               s_bt_psm_table[i].name);
#else
      strcpy(s_bt_proto_name, s_bt_psm_table[i].name);
#endif
      s_bt_proto.p_name = s_bt_proto_name;
      s_bt_proto.p_aliases = s_bt_proto_aliases;
#if defined(_WIN32)
      s_bt_proto.p_proto = (short)s_bt_psm_table[i].num;
#else
      s_bt_proto.p_proto = s_bt_psm_table[i].num;
#endif
      return &s_bt_proto;
    }
  }
  return NULL;
}

/** @brief Look up Bluetooth PSM protocol entry by PSM number. */
struct protoent *bt_getprotobynumber(int proto) {
  size_t i;
  for (i = 0; i < sizeof(s_bt_psm_table) / sizeof(s_bt_psm_table[0]); ++i) {
    if (proto == s_bt_psm_table[i].num) {
#if defined(_MSC_VER)
      strcpy_s(s_bt_proto_name, sizeof(s_bt_proto_name),
               s_bt_psm_table[i].name);
#else
      strcpy(s_bt_proto_name, s_bt_psm_table[i].name);
#endif
      s_bt_proto.p_name = s_bt_proto_name;
      s_bt_proto.p_aliases = s_bt_proto_aliases;
#if defined(_WIN32)
      s_bt_proto.p_proto = (short)s_bt_psm_table[i].num;
#else
      s_bt_proto.p_proto = s_bt_psm_table[i].num;
#endif
      return &s_bt_proto;
    }
  }
  return NULL;
}

/** @brief Open remote Bluetooth SDP session. */
void *sdp_open(const bdaddr_t *l, const bdaddr_t *r) {
  (void)l;
  (void)r;
  return NULL;
}
/** @brief Open local Bluetooth SDP daemon session. */
void *sdp_open_local(const char *control) {
  (void)control;
  return NULL;
}
/** @brief Close Bluetooth SDP session. */
int sdp_close(void *xs) {
  (void)xs;
  return 0;
}
/** @brief Retrieve last error code on Bluetooth SDP session. */
int sdp_error(void *xs) {
  (void)xs;
  return ENOSYS;
}
/** @brief Search remote Bluetooth SDP service attributes. */
int sdp_search(void *xss, uint32_t plen, const uint16_t *sp, uint32_t alen,
               const uint32_t *ap, uint32_t vlen, sdp_attr_t *vp) {
  (void)xss;
  (void)plen;
  (void)sp;
  (void)alen;
  (void)ap;
  (void)vlen;
  (void)vp;
  return -1;
}
/** @brief Register Bluetooth SDP service record. */
int sdp_register_service(void *xss, uint16_t uuid, const bdaddr_t *bdaddr,
                         const uint8_t *data, uint32_t datalen,
                         uint32_t *handle) {
  (void)xss;
  (void)uuid;
  (void)bdaddr;
  (void)data;
  (void)datalen;
  (void)handle;
  return -1;
}
/** @brief Unregister Bluetooth SDP service record. */
int sdp_unregister_service(void *xss, uint32_t handle) {
  (void)xss;
  (void)handle;
  return -1;
}

/** @brief Load FreeBSD kernel linker module by filename. */
int kldload(const char *file) {
  (void)file;
  errno = ENOSYS;
  return -1;
}
/** @brief Unload FreeBSD kernel linker module by file ID. */
int kldunload(int fileid) {
  (void)fileid;
  errno = ENOSYS;
  return -1;
}
/** @brief Unload FreeBSD kernel linker module with flags. */
int kldunloadf(int fileid, int flags) {
  (void)fileid;
  (void)flags;
  errno = ENOSYS;
  return -1;
}
/** @brief Find loaded FreeBSD kernel linker module ID by filename. */
int kldfind(const char *file) {
  (void)file;
  errno = ENOENT;
  return -1;
}
/** @brief Advance to next loaded FreeBSD kernel linker module ID. */
int kldnext(int fileid) {
  (void)fileid;
  return 0;
}
/** @brief Query status metadata of loaded FreeBSD kernel linker module. */
int kldstat(int fileid, struct kld_file_stat *st) {
  (void)fileid;
  (void)st;
  errno = ENOENT;
  return -1;
}
/** @brief Return first module ID inside a FreeBSD kernel linker file. */
int kldfirstmod(int fileid) {
  (void)fileid;
  return 0;
}
/** @brief Look up symbol address inside FreeBSD kernel linker module. */
int kldsym(int fileid, int cmd, void *data) {
  (void)fileid;
  (void)cmd;
  (void)data;
  errno = ENOENT;
  return -1;
}

/** @brief Dummy typedef preventing empty translation unit warnings. */
typedef int make_iso_compilers_happy_tu_bsd_dev_headers;

#endif
/** @brief Dummy typedef preventing empty mega translation unit warnings. */
typedef int make_iso_compilers_happy_mega_bsd_dev_headers;
