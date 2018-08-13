// gcc -shared -fpic -o libvector.so addvec.c
// gcc -o prog21 main2.c ./libvector.so

int addcnt = 0;

void addvec(int *x, int *y, int *z, int n)
{
	int i;

	addcnt++;

	for (i = 0; i < n; ++i)
		z[i] = x[i] + y[i];
}