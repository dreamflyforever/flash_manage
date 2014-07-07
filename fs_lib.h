#define fs_sizeof(x) ((char *)(&x+1) - (char *)(&x))

size_t fs_strlen(const char *s)
{
    if (s == NULL)
        return 0;

	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}
void *fs_memcpy(void *dest, const void *src, size_t count)
{
	char *tmp = dest;
	const char *s = src;

	while (count--)
		*tmp++ = *s++;
	return dest;
}
