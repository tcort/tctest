/*
 * tctest - small and easy to use unit testing framework for C.
 * Copyright (C) 2019  Thomas Cort <linuxgeek@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TCTEST_H
#define __TCTEST_H

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int pass_count = 0;
int fail_count = 0;

#define TCTEST_CHECK(condition, message) 			\
	do {							\
		if ((condition)) {				\
			fprintf(stdout, "PASS: %s\n", message);	\
			pass_count++;				\
		} else {					\
			fprintf(stderr, "FAIL: %s\n", message);	\
			fail_count++;				\
		}						\
	} while (0)

#define TCTEST_PATH_MAX 4096

extern const char *__progname;

int main(int argc, char *argv[]) {
	int exit_code = EXIT_FAILURE;
	Elf *elf;
	Elf_Scn *scn = NULL;
	GElf_Shdr shdr;
	Elf_Data *data;
	void *r;
	int fd;
	int i;
	int count;
	char path[TCTEST_PATH_MAX];
	pid_t pid;
	void (*f)(void);
	void *d;

	if (elf_version(EV_CURRENT) == EV_NONE) {
		fprintf(stderr, "libelf library out of daten");
		goto fail_44;
	}

	pid = getpid();
	bzero(path, TCTEST_PATH_MAX);
	snprintf(path, TCTEST_PATH_MAX - 1, "/proc/%d/exe", pid);

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open failed: %sn", strerror(errno));
		goto fail_44;
	}

	elf = elf_begin(fd, ELF_C_READ, NULL);
	if (elf == NULL) {
		fprintf(stderr, "elf_begin failed: %sn", elf_errmsg(elf_errno()));
		goto fail_55;
	}


        d = dlopen(NULL, RTLD_LAZY);
	if (d == NULL) {
		fprintf(stderr, "dlopen failed: %sn", dlerror()); 
		goto fail_77;
	}

	while ((scn = elf_nextscn(elf, scn)) != 0) {
		r = (void*) gelf_getshdr(scn, &shdr);
		if (r == NULL) {
			fprintf(stderr, "elf_getshdr failed: %sn", elf_errmsg(elf_errno()));
			goto fail_99;
		} else if (shdr.sh_type == SHT_SYMTAB) {
			data = elf_getdata(scn, NULL);
			if (data == NULL) {
				fprintf(stderr, "elf_getdata failed: %sn", elf_errmsg(elf_errno()));
				goto fail_99;
			}

			count = shdr.sh_size / shdr.sh_entsize;
			for (i = 0; i < count; i++) {
				char *symbol_name;
				GElf_Sym sym;

				r = (void*) gelf_getsym(data, i, &sym);
				if (r == NULL) {
					fprintf(stderr, "gelf_getsym failed: %sn", elf_errmsg(elf_errno()));
					goto fail_99;
				}

				symbol_name = elf_strptr(elf, shdr.sh_link, sym.st_name);
				if (symbol_name == NULL) {
					fprintf(stderr, "elf_strptr failed: %sn", elf_errmsg(elf_errno()));
					goto fail_99;
				}

				if (strncmp(symbol_name, "test_", strlen("test_")) == 0) {
					f = (void (*)(void))dlsym(d, symbol_name);
					if (f) {
						f();
					} else {
						fprintf(stderr, "couldn't find functionn");
						goto fail_99;
					}
				}

			}
		}

	}

	exit_code = EXIT_SUCCESS;

fail_99:
	dlclose(d);
fail_77:
	elf_end(elf);
fail_55:
	close(fd);
fail_44:
	fprintf(stdout, "DONE: %d Assertions, %d Passed, %d Failed\n",
		(pass_count + fail_count), pass_count, fail_count);

	if (fail_count > 0) {
		exit_code = EXIT_FAILURE;
	}

	return exit_code;
}

#endif
