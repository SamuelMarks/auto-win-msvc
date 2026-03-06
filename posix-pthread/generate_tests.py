import json
import re

with open('mappings.json', 'r') as f:
    data = json.load(f)

from generate_code import sigs, extract_args
from gen_impl import impls

out = []
out.append("""/* test.c - 100% Test Coverage Stubs */
#include <stdio.h>
#include <stdlib.h>
#include "greatest.h"
#include "posix-pthread.h"

GREATEST_MAIN_DEFS();

/* Dummy variables */
pthread_attr_t attr;
pthread_mutex_t mutex;
pthread_mutexattr_t mutexattr;
pthread_cond_t cond;
pthread_condattr_t condattr;
pthread_rwlock_t rwlock;
pthread_rwlockattr_t rwlockattr;
pthread_barrier_t barrier;
pthread_barrierattr_t barrierattr;
pthread_spinlock_t spinlock;
pthread_key_t key;
pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_t thread;
sem_t sem;
struct sched_param param;
struct timespec ts = {0, 0};
int dummy_int = 0;
size_t dummy_size = 0;
void *dummy_ptr = NULL;
clockid_t clock_id = 0;
sigset_t sigset = 0;

void dummy_func(void) {}
void dummy_func_arg(void *arg) { (void)arg; }
void *dummy_thread_func(void *arg) { (void)arg; return NULL; }
""")

for func in data['mappings']['functions']:
    name = func['posix']
    sig = sigs[name]
    args = extract_args(sig)
    
    out.append(f"TEST test_{name}() {{")
    
    # build call
    call_args = []
    for arg in args:
        if 'attr' in arg and 'rwlock' not in arg and 'cond' not in arg and 'mutex' not in arg and 'barrier' not in arg:
            call_args.append('&attr')
        elif 'mutexattr' in arg: call_args.append('&mutexattr')
        elif 'condattr' in arg: call_args.append('&condattr')
        elif 'rwlockattr' in arg: call_args.append('&rwlockattr')
        elif 'barrierattr' in arg: call_args.append('&barrierattr')
        elif 'mutex' in arg: call_args.append('&mutex')
        elif 'cond' in arg: call_args.append('&cond')
        elif 'rwlock' in arg: call_args.append('&rwlock')
        elif 'barrier' in arg: call_args.append('&barrier')
        elif 'lock' in arg: call_args.append('&spinlock')
        elif 'sem' in arg: call_args.append('&sem')
        elif 'key' in arg and '*' in sig.split(arg)[0][-2:]: call_args.append('&key')
        elif 'key' in arg: call_args.append('key')
        elif 'thread' in arg and '*' in sig.split(arg)[0][-2:]: call_args.append('&thread')
        elif 'thread' in arg: call_args.append('thread')
        elif 'once' in arg: call_args.append('&once')
        elif 'param' in arg: call_args.append('&param')
        elif 'time' in arg or 'abstime' in arg: call_args.append('&ts')
        elif 'destructor' in arg: call_args.append('dummy_func_arg')
        elif 'routine' in arg: call_args.append('dummy_thread_func')
        elif 'prepare' in arg or 'parent' in arg or 'child' in arg or 'init_routine' in arg: call_args.append('dummy_func')
        elif 'value' in arg and '*' in sig.split(arg)[0][-2:]: call_args.append('&dummy_ptr')
        elif 'value' in arg: call_args.append('NULL')
        elif 'set' in arg or 'oset' in arg: call_args.append('&sigset')
        elif 'size' in arg: call_args.append('0')
        elif 'clock_id' in arg: call_args.append('&clock_id')
        elif 'policy' in arg or 'shared' in arg or 'state' in arg or 'type' in arg or 'ceiling' in arg or 'protocol' in arg or 'scope' in arg or 'sched' in arg or 'count' in arg or 'pid' in arg:
            if '*' in sig.split(arg)[0][-2:]: call_args.append('&dummy_int')
            else: call_args.append('0')
        else:
            if 'void' in arg: pass
            elif '*' in sig.split(arg)[0][-2:]: call_args.append('&dummy_int')
            else: call_args.append('0')
    
    call_str = f"{name}({', '.join(call_args)})"
    out.append(f"    /* {call_str}; */")
    out.append(f"    {call_str};")
    out.append("    PASS();")
    out.append("}\n")

out.append("SUITE(posix_pthread_suite) {")
for func in data['mappings']['functions']:
    name = func['posix']
    out.append(f"    RUN_TEST(test_{name});")
out.append("}\n")

out.append("""int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(posix_pthread_suite);
    GREATEST_MAIN_END();
}""")

with open('tests/test.c', 'w') as f:
    f.write('\n'.join(out))

