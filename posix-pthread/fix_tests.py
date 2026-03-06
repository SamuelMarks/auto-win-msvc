with open('tests/test.c', 'r') as f:
    c = f.read()

c = c.replace('sigset_t sigset = 0;', 'sigset_t dummy_sigset = 0;')
c = c.replace('&sigset', '&dummy_sigset')

c = c.replace('pthread_cleanup_push(dummy_thread_func, dummy_ptr);', 'pthread_cleanup_push(dummy_func_arg, dummy_ptr);')
c = c.replace('pthread_key_create(&key, dummy_thread_func);', 'pthread_key_create(&key, dummy_func_arg);')

if not c.endswith('\n'):
    c += '\n'

with open('tests/test.c', 'w') as f:
    f.write(c)

