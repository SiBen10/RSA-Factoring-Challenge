#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <gmp.h>
#include <sys/types.h>
#include <sys/stat.h>

#define Size 1000000

int sieve(int stat)
{
	bool prime[Size];
	static int primes[Size];
	static int n_primes;
	static int current;

	if (stat == 0)
	{
		n_primes = 0;
		current = 0;
		memset(prime, true, sizeof(prime));

		for (int p = 2; p * p < Size; p++)
			if (prime[p] == true)
				for (int i = p * p; i <= Size; i += p)
					prime[i] = false;

		// Print all prime numbers
		for (int p = 2; p < Size; p++)
			if (prime[p])
			{
				primes[n_primes] = p;
				n_primes += 1;
			}
	}
	else if (stat == 1)
	{
		current++;
		if (current - 1 < n_primes)
			return (primes[current - 1]);
		return (-1);
	}
	else
		current = 0;
}

/**
 * print_factor - prints two factors of num in the followgin format
 * n=p*q, where n is the number and p&q are factors
 * p&q are not necessarly primes
 * @num: the number whose factors are to be printed
 * @fs: File stream to print to
 *Return: the number of characters printed
 */
size_t print_factor(mpz_t num, FILE *fs)
{
	mpz_t i, ans, j, mod;
	size_t printed = 0;
	int prime = 2;


	mpz_init(i);
	mpz_init(mod);
	mpz_init(ans);
	mpz_init(j);
	mpz_set_ui(j, 1);
	mpz_set_ui(i, 2);

	while (1)
	{
		if (mpz_cmp(i, num) >= 0)
			break;
		if (prime != -1)
		{
			prime = sieve(1);
			//printf("%i\n", prime);
			if (prime != -1)
				mpz_set_ui(i, prime);
		}
		mpz_mod (mod, num, i);
		if (mpz_cmp_d(mod, 0) == 0)
		{
			mpz_div(ans, num, i);
			printed += mpz_out_str(fs, 10, num);
			printed += fprintf(fs, "=");
			printed += mpz_out_str(fs, 10, ans);
			printed += fprintf(fs, "*");
			printed += mpz_out_str(fs, 10, i);
			printed += fprintf(fs, "\n");
			return (printed);
		}
		mpz_add(i, i, j);
	}
	/*incase the number is prime*/
	printed += mpz_out_str(fs, 10, num);
	printed += fprintf(fs, "=");
	printed += mpz_out_str(fs, 10, num);
	printed += fprintf(fs, "*");
	printed += mpz_out_str(fs, 10, j);
	printed += fprintf(fs, "\n");

	return (printed);
}


/**
 *exit_fail-exits with EXIT_FAILURE after printing error
 */
void exit_fail(void)
{
	perror(NULL);
	exit(EXIT_FAILURE);
}
/**
 * reader - reads a text from a file descrriptor
 * until a new line is found and returns when found
 * deallocating memory is caller's responsibility
 * @fd: file descriptor
 * Return: returns a single line of string fromm the open file discreptor
 */
char *reader(int fd)
{
	char *line = (char *)malloc(15 * sizeof(char));
	int read_p = 0, stat, size = 15;
	char *_line = line;

	if (!line)
		exit_fail();
	while ((stat = read(fd, _line + read_p, 1)) != -1)
	{
		/*incase nothing is read, using  read_p==0 isn't a good idea in the case
		 * where the line iss exactly 10 chars long, after which read_p is set
		 * back to 0 but som char is read*/
		if (line == _line && stat == 0)
		{
			free(line);
			return (NULL);

		}
		if (_line[read_p] == '\n')
		{
			_line[read_p] = '\0';
			return (line);
		}

		read_p++;
		if (read_p == 15)
		{
			size += 15;
			line = (char *)realloc(line, size);
			_line = line + read_p;
			read_p = 0;
		}
	}
	return (line);
}

int main(int argc, char **argv)
{
	int fd, stat;
	mpz_t num;
	size_t printed = 0;
	char *read_line = NULL;
	FILE *tmp_file = tmpfile();

	mpz_init(num);
	tmp_file = fopen("tmp_file.txt", "w+");
	sieve(0);

	if (argc != 2)
	{
		dprintf(2, "Usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*if tmporary file couldn't be opened*/
	if (!tmp_file)
		exit_fail();

	/*open the file*/
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		exit_fail();

	while ((read_line = reader(fd)))
	{
		sieve(3);
		mpz_set_str(num, read_line, 10);
		printed += print_factor(num, tmp_file);
		free(read_line);
	}
	read_line = (char *)malloc(sizeof(char) * printed + 1);

	rewind(tmp_file);
	stat = fread(read_line, sizeof(char), printed, tmp_file);
	if (stat == -1)
		exit_fail();

	printf("%s", read_line);

	//free(read_line);
	close(fd);
	exit(EXIT_SUCCESS);
}
