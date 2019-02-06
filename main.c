#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>



#define INPUT_FILE "input.dat"
#define OUTPUT_FILE "output.dat"

static void print_help(const char* prog_name)
{
	printf("Usage: %s [OPTIONS]\n\n", prog_name);
	printf("Example:\n\t %s -o out.txt -i in.txt\n\n", prog_name);
	printf("Options:\n");

	printf("\t -h");
	printf("\t\tDisplay options\n");

	printf("\t -i [FILE]");
	printf("\tSpecify input file name (Default: %s)\n", INPUT_FILE);

	printf("\t -o [FILE]");
	printf("\tSpecify output file name (Default: %s)\n", OUTPUT_FILE);
}

struct stack
{
	size_t size;
	size_t max;
	int* data;
	int a;
};

static void stack_init(struct stack* s, size_t max)
{
	s->size = 0;
	s->max = max;
	s->data = malloc(sizeof(int) * max);
}

static void stack_push(struct stack* s, int data)
{
	s->data[s->size] = data;
	++s->size;
}

static void stack_pop(struct stack* s)
{
	if(s->size)
	{
		--s->size;
	}
}

static int stack_top(const struct stack* const s)
{
	return s->data[s->size - 1];
}

static void skip_till_new(FILE* in)
{
	char c;
	do
	{
		fscanf(in, "%c", &c);
	} while (c != '\n');
}

static void read_n(FILE* in, struct stack* s, size_t n)
{
	stack_init(s, n);
	size_t i;
	for(i = 0; i < n; ++i)
	{
		int data;
		fscanf(in, "%d", &data);
		stack_push(s, data);
	}
}

static void print_data(FILE* out, struct stack* s)
{
	int i;
	for(i = s->size; i > 0; --i)
	{
		fprintf(out, "%d ", stack_top(s));
		stack_pop(s);
	}
	fprintf(out, "\n");

}

static void iterate_file(FILE* in, FILE* out)
{
	size_t n;
	fscanf(in, "%zu", &n);
	skip_till_new(in);

	pid_t* pids = (pid_t*)malloc(sizeof(pid_t) * n);

	size_t i;
	for(i = 0; i < n; ++i)
	{
		pid_t id = fork();
		if(id == 0)
		{
			size_t m;
			fscanf(in, "%zu", &m);

			struct stack s;
			read_n(in, &s, m);

			fprintf(out, "%d: ", getpid());
			print_data(out, &s);

			exit(EXIT_SUCCESS);
		}
		else
		{
			pids[i] = id;
			skip_till_new(in);
			skip_till_new(in);
			wait(NULL);
		}
	}

	fprintf(out, "All children were:");
	for(i = 0; i < n; ++i)
	{
		fprintf(out, " %d", pids[i]);
	}
	fprintf(out, "\n");
}

int main(int argc, char *argv[])
{
	char* input_file = INPUT_FILE;
	char* output_file = OUTPUT_FILE;

	int opt;
	while((opt = getopt(argc, argv, "hi:o:")) != -1)
	{
		switch(opt)
		{
		case 'h':
			print_help(argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			input_file = optarg;
			break;
		case 'o':
			output_file = optarg;
			break;
		default:
			exit(EXIT_FAILURE);
			break;
		};
	}

	char error_prefix[128];
	strcpy(error_prefix, argv[0]);
	strcat(error_prefix, ": Error: ");

	FILE* in = fopen(input_file, "r");

	if(!in)
	{
		strcat(error_prefix, input_file);
		perror(error_prefix);
		exit(EXIT_FAILURE);
	}

	FILE* out = fopen(output_file, "w");

	iterate_file(in, out);

	fclose(in);
	fclose(out);

	return 0;
}
