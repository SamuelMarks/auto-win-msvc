with open('src/posix-pthread.c', 'r') as f:
    c = f.read()
c = c.replace("""void *pthread_getspecific(pthread_key_t key) {
    (void)key;

    return;
}""", """void *pthread_getspecific(pthread_key_t key) {
    (void)key;

    return 0;
}""")
with open('src/posix-pthread.c', 'w') as f:
    f.write(c)
