
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "006_tg_event.h"

#if __has_include("test_by_hand.c")
#  include "test_by_hand.c"
#endif

uint32_t passed = 0;
uint32_t credit = 0;
uint32_t total_credit = 0;

static void sig_handler(int sig);
static void print_info(int ret);

static void exec_test(struct list_func *list)
{
	size_t i = 0;
	while (true) {
		if (!list[i].func)
			break;
		total_credit += list[i].credit;
		list[i].func();
		i++;
	}
}

static int handle_wait(pid_t child)
{
	int wstatus;
	pid_t wait_ret;

	wait_ret = wait(&wstatus);
	if (WIFEXITED(wstatus) && (wait_ret == child)) {
		int exit_code = WEXITSTATUS(wstatus);

		if (exit_code == 0) {
			/* Success */
			return 0;
		}

		pr_err("\x1b[31mTEST FAILED!\x1b[0m");
		pr_err("Exit code: %d", exit_code);
		if (exit_code == 99) {
			pr_err("Error from valgrind detected");
			pr_err("Please read the error message from valgrind to "
				"diagnose this problem");
			pr_err("Reading valgrind backtrace is not trivial, "
				"please be serious!");
		}
		return exit_code;
	}

	pr_err("Unknown error, please contact Ammar F");
	pr_err("Please also tell to him, how did you get into this error");
	return -1;
}


static int spawn_valgrind(char *argv[])
{
	pid_t child;
	char * const execve_argv[] = {
		"/usr/bin/valgrind",
		"--leak-check=full",
		"--show-leak-kinds=all",
		"--track-origins=yes",
		"--track-fds=yes",
		"--error-exitcode=99",
		"--exit-on-first-error=yes",
		"-s",
		argv[0],
		"1",
		NULL
	};

	char * const execve_envp[] = {
		"LD_LIBRARY_PATH=" LD_LIBRARY_PATH_ENV,
		NULL,
	};


	child = fork();
	if (child == -1) {
		pr_err("fork(): " PRERF, PREAR(errno));	
		return -1;
	}

	if (child == 0) {
		execve(execve_argv[0], execve_argv, execve_envp);
		pr_err("execve(): " PRERF, PREAR(errno));
		return -1;
	}

	return handle_wait(child);
}

int main(int argc, char *argv[])
{
	int ret = 0;

#if defined(TEST_BY_HAND)
	if (TEST_BY_HAND) {
		pr_notice("Running your custom test by hand...\n");
		ret = test_by_hand();
		return ret;
	}
#endif

	if (argc == 1)
		return spawn_valgrind(argv);

	pr_notice("Running tg_event test...");
	signal(SIGSEGV, sig_handler);
	signal(SIGABRT, sig_handler);

	exec_test(tg_event_text_list);
	exec_test(tg_event_photo_list);
	exec_test(tg_event_sticker_list);
	
	if (credit < total_credit)
		ret = 1; 

	print_info(ret);
	for (int i = 0; i < 1000; i++)
		close(i);
	return ret;
}


char *load_str_from_file(const char *file)
{
	int err;
	long pos;
	FILE *handle;
	char *ret = NULL;
	char filename[1024];

	snprintf(filename, sizeof(filename), MY_BASEPATH "/%s", file);

	handle = fopen(filename, "r");
	if (unlikely(handle == NULL)) {
		err = errno;
		pr_err("fopen(\"%s\"): " PRERF, filename, PREAR(err));
		return NULL;
	}

	if (unlikely(fseek(handle, 0, SEEK_END) != 0)) {
		err = errno;
		pr_err("fseek(): " PRERF, PREAR(err));
		goto out_err_close;
	}

	pos = ftell(handle);
	if (unlikely(pos < 0)) {
		err = errno;
		pr_err("ftell(): " PRERF, PREAR(err));
		goto out_err_close;
	}

	ret = malloc((size_t)pos + 1);
	if (unlikely(ret == NULL)) {
		err = errno;
		pr_err("malloc(): " PRERF, PREAR(err));
		goto out_err_close;
	}

	rewind(handle);
	fread(ret, sizeof(char), (size_t)pos, handle);
	ret[pos] = '\0';
	fclose(handle);
	return ret;

out_err_close:
	free(ret);
	fclose(handle);
	return NULL;
}


static void sig_handler(int sig)
{
	if (sig == SIGSEGV) {
		core_dump();
		panic("SIGSEGV caught!");
		pr_emerg("HEY, WHAT THE F*UCK!");
		pr_emerg("You crashed the program memory!");
		pr_emerg("Segmentation Fault (core dumped)");
		pr_emerg("===============================================");
		exit(1);
	} else if (sig == SIGABRT) {
		core_dump();
		panic("SIGABRT caught!");
		pr_emerg("What the hell did you do?");
		pr_emerg("Aborted (core dumped)");
		pr_emerg("===============================================");
		exit(1);
	}
	exit(0);
}


static void print_info(int ret)
{
	double accuracy;

	if (credit == 0 || total_credit == 0) {
		accuracy = 0;
	} else {
		accuracy = ((double)credit / (double)total_credit) * 100.0;
	}

	printf("==================================================\n");
	printf("\t\tSummary\n");
	printf("--------------------------------------------------\n");
	printf("   Last return value\t: %d\n", ret);
	printf("   Your accuracy\t: %.2f %c\n", accuracy, '%');
	printf("   Earned credit\t: %u of %u\n", credit, total_credit);
	printf("==================================================\n");
}


int nnstrcmp(const char *s1, const char *s2)
{
	if (s1 == NULL || s1 == NULL)
		return -1;

	return strcmp(s1, s2);
}

