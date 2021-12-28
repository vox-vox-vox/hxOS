#include <stdio.h>
int main()
{
	FILE* f = fopen("io.txt", "w+"); // open for read/write
	if (f == NULL) 
	{
		perror("fopen");
		return 0;
	}

	char* c1 = "abcd";
	fwrite(c1, 1, 4, f); // make sure the stream has 4 bytes
	rewind(f); // reset position indicator	
	fwrite(c1, 1, 1, f); // write one byte at the beginning

	char c2 = '\0';
	fread(&c2, 1, 1, f); // read after write	
	printf("%c\n", c2); // what's in c2?

	fwrite(&c2, 1, 1, f); // write after read
	fclose(f); // what's in the file now?

	return 0;
}
