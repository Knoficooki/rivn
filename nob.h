/*
 *  This is a non-posix version of nob.h with additional tools and utilities.
 *  It is licensed under the MIT Licence and created by Alexey Kutepov.
 *  Editor and owner of this project is Quinn(n0ffie/Knoficooki).
 *  Original notice and licence below:
 */

// This is a complete backward incompatible rewrite of
// https://github.com/tsoding/nobuild because I'm really unhappy with the
// direction it is going. It's gonna sit in this repo until it's matured enough
// and then I'll probably extract it to its own repo.

// Copyright 2023 Alexey Kutepov <reximkut@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/* nob.h
 * Header-File for a nob.c build definition.
 */

#ifndef NOB_H_
#define NOB_H_

#include <stddef.h>
#ifdef __cplusplus
#pragma once
extern "C" {
	#endif

	#define NOB_ASSERT assert
	#define NOB_REALLOC realloc
	#define NOB_FREE free

	#include <assert.h>
	#include <ctype.h>
	#include <errno.h>
	#include <stdarg.h>
	#include <stdbool.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINUSER_
	#define _WINGDI_
	#define _IMM_
	#define _WINCON_
	#include <direct.h>
	#include <windows.h>
	#else
	#include <fcntl.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <unistd.h>
	#endif

	#ifdef _WIN32
	#define NOB_LINE_END "\r\n"
	#else
	#define NOB_LINE_END "\n"
	#endif

	#define NOB_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
	#define NOB_ARRAY_GET(array, index)                                            \
	(NOB_ASSERT((size_t)index < NOB_ARRAY_LEN(array)), array[(size_t)index])

	// Posix-replacements
	void nob_memcpy_s(void *dest, size_t destsz, const void *src, size_t count);
	// end Posix-replacements

	typedef enum {
		NOB_INFO = 0,
		NOB_WARNING = 1,
		NOB_ERROR = 2,
		NOB_DEBUG = 3,
	} Nob_Log_Level;

	static const char *Nob_Log_Level_strings[] = {
		"[INFO] ",
		"[WARNING] ",
		"[ERROR] ",
		"[DEBUG] ",
	};

	void nob_log(Nob_Log_Level level, const char *fmt, ...);

	/*
	 * Nob Command Line Arguments
	 * By Quinn
	 */

	typedef enum {
		NOB_NOT_IN_ARRAY = -1,
	} Nob_Codes;

	typedef struct NOB_CL_ARGS {
		size_t argc;
		char **args;
		char **values;
	} nob_cl_args;

	nob_cl_args *nob_cl_args_use(int argc, char **argv);
	size_t nob_args_has(const nob_cl_args *args, const char *arg);
	const char *nob_args_get(const nob_cl_args *args, const char *arg);
	void nob_args_free(nob_cl_args *args);

	void nob_remove_arg(nob_cl_args *args, const char *arg);
	void nob_pop_arg(nob_cl_args *args);

	/*
	 * End of Nob Command Line Arguments
	 */

	// It is an equivalent of shift command from bash. It basically pops a command
	// line argument from the beginning.
	char *nob_shift_args(int *argc, char ***argv);

	typedef struct {
		const char **items;
		size_t count;
		size_t capacity;
	} Nob_File_Paths;

	typedef enum {
		NOB_FILE_REGULAR = 0,
		NOB_FILE_DIRECTORY,
		NOB_FILE_SYMLINK,
		NOB_FILE_OTHER,
	} Nob_File_Type;

	bool nob_mkdir_if_not_exists(const char *path);
	bool nob_copy_file(const char *src_path, const char *dst_path);
	bool nob_copy_directory_recursively(const char *src_path, const char *dst_path);
	bool nob_read_entire_dir(const char *parent, Nob_File_Paths *children);
	bool nob_write_entire_file(const char *path, const void *data, size_t size);
	Nob_File_Type nob_get_file_type(const char *path);

	#define nob_return_defer(value)                                                \
	do {                                                                         \
		result = (value);                                                          \
		goto defer;                                                                \
	} while (0)

	// Initial capacity of a dynamic array
	#define NOB_DA_INIT_CAP 256

	// Append an item to a dynamic array
	#define nob_da_append(da, item)                                                \
	{                                                                            \
		if ((da)->count >= (da)->capacity) {                                       \
			(da)->capacity =                                                         \
			(da)->capacity == 0 ? NOB_DA_INIT_CAP : (da)->capacity * 2;          \
			(da)->items =                                                            \
			NOB_REALLOC((da)->items, (da)->capacity * sizeof(*(da)->items));     \
			NOB_ASSERT((da)->items != NULL && "Buy more RAM lol");                   \
		}                                                                          \
		(da)->items[(da)->count++] = (item);                                       \
	}

	#define nob_da_free(da) NOB_FREE((da).items)

	// Append several items to a dynamic array
	#define nob_da_append_many(da, new_items, new_items_count)                     \
	{                                                                            \
		if ((da)->count + (new_items_count) > (da)->capacity) {                    \
			if ((da)->capacity == 0) {                                               \
				(da)->capacity = NOB_DA_INIT_CAP;                                      \
			}                                                                        \
			while ((da)->count + (new_items_count) > (da)->capacity) {               \
				(da)->capacity *= 2;                                                   \
			}                                                                        \
			(da)->items = (const char**)NOB_REALLOC(                                \
			(da)->items, (da)->capacity * sizeof(*(da)->items));                 \
			NOB_ASSERT((da)->items != NULL && "Buy more RAM lol");                   \
		}                                                                          \
		nob_memcpy_s((da)->items + (da)->count,                                    \
		((da)->capacity) * sizeof(*(da)->items), (new_items),         \
		(new_items_count) * sizeof(*(da)->items));                    \
		(da)->count += (new_items_count);                                          \
	}

	typedef struct {
		char *items;
		size_t count;
		size_t capacity;
	} Nob_String_Builder;

	bool nob_read_entire_file(const char *path, Nob_String_Builder *sb);

	// Append a sized buffer to a string builder
	#define nob_sb_append_buf(sb, buf, size) nob_da_append_many(sb, buf, size)

	// Append a NULL-terminated string to a string builder
	#define nob_sb_append_cstr(sb, cstr)                                           \
	{                                                                            \
		const char *s = (cstr);                                                    \
		size_t n = strlen(s);                                                      \
		nob_da_append_many(sb, s, n);                                              \
	}

	// Append a single NULL character at the end of a string builder. So then you
	// can use it a NULL-terminated C string
	#define nob_sb_append_null(sb) nob_da_append_many(sb, "", 1)

	// Free the memory allocated by a string builder
	#define nob_sb_free(sb) NOB_FREE((sb).items)

	// Process handle
	#ifdef _WIN32
	typedef HANDLE Nob_Proc;
	#define NOB_INVALID_PROC INVALID_HANDLE_VALUE
	#else
	typedef int Nob_Proc;
	#define NOB_INVALID_PROC (-1)
	#endif // _WIN32

	typedef struct {
		Nob_Proc *items;
		size_t count;
		size_t capacity;
	} Nob_Procs;

	bool nob_procs_wait(Nob_Procs procs);

	// Wait until the process has finished
	bool nob_proc_wait(Nob_Proc proc);

	// A command - the main workhorse of Nob. Nob is all about building commands an
	// running them
	typedef struct {
		const char **items;
		size_t count;
		size_t capacity;
	} Nob_Cmd;

	// Render a string representation of a command into a string builder. Keep in
	// mind the the string builder is not NULL-terminated by default. Use
	// nob_sb_append_null if you plan to use it as a C string.
	void nob_cmd_render(Nob_Cmd cmd, Nob_String_Builder *render);

	#define nob_cmd_append(cmd, ...)                                               \
	nob_da_append_many(                                                          \
	cmd, ((const char *[]){__VA_ARGS__}),                                    \
	(sizeof((const char *[]){__VA_ARGS__}) / sizeof(const char *)))

	// Free all the memory allocated by command arguments
	#define nob_cmd_free(cmd) NOB_FREE(cmd.items)

	// Run command asynchronously
	Nob_Proc nob_cmd_run_async(Nob_Cmd cmd);

	// Run command synchronously
	bool nob_cmd_run_sync(Nob_Cmd cmd);

	#ifndef NOB_TEMP_CAPACITY
	#define NOB_TEMP_CAPACITY (8 * 1024 * 1024)
	#endif // NOB_TEMP_CAPACITY
	char *nob_temp_strdup(const char *cstr);
	void *nob_temp_alloc(size_t size);
	char *nob_temp_sprintf(const char *format, ...);
	void nob_temp_reset(void);
	size_t nob_temp_save(void);
	void nob_temp_rewind(size_t checkpoint);

	int is_path1_modified_after_path2(const char *path1, const char *path2);
	bool nob_rename(const char *old_path, const char *new_path);
	int nob_needs_rebuild(const char *output_path, const char **input_paths,
						  size_t input_paths_count);
	int nob_needs_rebuild1(const char *output_path, const char *input_path);
	int nob_file_exists(const char *file_path);

	// TODO: add MinGW support for Go Rebuild Urself™ Technology
	#ifndef NOB_REBUILD_URSELF
	#if _WIN32
	#if defined(__GNUC__)
	#define NOB_REBUILD_URSELF(binary_path, source_path)                           \
	"gcc", "-o", binary_path, source_path
	#elif defined(__clang__)
	#define NOB_REBUILD_URSELF(binary_path, source_path)                           \
	"clang", "-o", binary_path, source_path
	#elif defined(_MSC_VER)
	#define NOB_REBUILD_URSELF(binary_path, source_path)                           \
	"cl.exe", nob_temp_sprintf("/Fe:%s", (binary_path)), source_path
	#endif
	#else
	#define NOB_REBUILD_URSELF(binary_path, source_path)                           \
	"cc", "-o", binary_path, source_path
	#endif
	#endif

	// Go Rebuild Urself™ Technology
	//
	//   How to use it:
	//     int main(int argc, char** argv) {
	//         GO_REBUILD_URSELF(argc, argv);
	//         // actual work
	//         return 0;
	//     }
	//
	//   After your added this macro every time you run ./nobuild it will detect
	//   that you modified its original source code and will try to rebuild itself
	//   before doing any actual work. So you only need to bootstrap your build
	//   system once.
	//
	//   The modification is detected by comparing the last modified times of the
	//   executable and its source code. The same way the make utility usually does
	//   it.
	//
	//   The rebuilding is done by using the REBUILD_URSELF macro which you can
	//   redefine if you need a special way of bootstraping your build system.
	//   (which I personally do not recommend since the whole idea of nobuild is to
	//   keep the process of bootstrapping as simple as possible and doing all of
	//   the actual work inside of the nobuild)
	//
	#define NOB_GO_REBUILD_URSELF(argc, argv)												\
	do {																					\
		const char *source_path = __FILE__;													\
		assert(argc >= 1);																	\
		const char *binary_path = argv[0];													\
																							\
		int rebuild_is_needed = nob_needs_rebuild(binary_path, &source_path, 1);			\
		if (rebuild_is_needed < 0)															\
			exit(1);																		\
			if (rebuild_is_needed) {														\
				Nob_String_Builder sb = {0};												\
				nob_sb_append_cstr(&sb, binary_path);										\
				nob_sb_append_cstr(&sb, ".old");											\
				nob_sb_append_null(&sb);													\
																							\
				if (!nob_rename(binary_path, sb.items))										\
					exit(1);																\
					Nob_Cmd rebuild = {0};													\
					nob_cmd_append(&rebuild, NOB_REBUILD_URSELF(binary_path, source_path));	\
					bool rebuild_succeeded = nob_cmd_run_sync(rebuild);						\
					nob_cmd_free(rebuild);													\
					if (!rebuild_succeeded) {												\
						nob_rename(sb.items, binary_path);									\
						exit(1);															\
					}																		\
																							\
					Nob_Cmd cmd = {0};														\
					nob_da_append_many(&cmd, argv, argc);									\
					if (!nob_cmd_run_sync(cmd))												\
						exit(1);															\
						exit(0);															\
			}																				\
	} while (0)
	// The implementation idea is stolen from https://github.com/zhiayang/nabs

	// Quinn: XD yea i stole this all... looking forward to making it better

	typedef struct {
		size_t count;
		const char *data;
	} Nob_String_View;

	const char *nob_temp_sv_to_cstr(Nob_String_View sv);

	Nob_String_View nob_sv_chop_by_delim(Nob_String_View *sv, char delim);
	Nob_String_View nob_sv_trim(Nob_String_View sv);
	bool nob_sv_eq(Nob_String_View a, Nob_String_View b);
	Nob_String_View nob_sv_from_cstr(const char *cstr);
	Nob_String_View nob_sv_from_parts(const char *data, size_t count);

	// printf macros for String_View
	#ifndef SV_Fmt
	#define SV_Fmt "%.*s"
	#endif // SV_Fmt
	#ifndef SV_Arg
	#define SV_Arg(sv) (int)(sv).count, (sv).data
	#endif // SV_Arg
	// USAGE:
	//   String_View name = ...;
	//   printf("Name: "SV_Fmt"\n", SV_Arg(name));

	// minirent.h HEADER BEGIN ////////////////////////////////////////
	// Copyright 2021 Alexey Kutepov <reximkut@gmail.com>
	//
	// Permission is hereby granted, free of charge, to any person obtaining
	// a copy of this software and associated documentation files (the
	// "Software"), to deal in the Software without restriction, including
	// without limitation the rights to use, copy, modify, merge, publish,
	// distribute, sublicense, and/or sell copies of the Software, and to
	// permit persons to whom the Software is furnished to do so, subject to
	// the following conditions:
	//
	// The above copyright notice and this permission notice shall be
	// included in all copies or substantial portions of the Software.
	//
	// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
	// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
	// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	//
	// ============================================================
	//
	// minirent — 0.0.1 — A subset of dirent interface for Windows.
	//
	// https://github.com/tsoding/minirent
	//
	// ============================================================
	//
	// ChangeLog (https://semver.org/ is implied)
	//
	//    0.0.2 Automatically include dirent.h on non-Windows
	//          platforms
	//    0.0.1 First Official Release

	#ifndef _WIN32
	#include <dirent.h>
	#else // _WIN32

	#define WIN32_LEAN_AND_MEAN
	#include "windows.h"

	struct dirent {
		char d_name[MAX_PATH + 1];
	};

	typedef struct DIR DIR;

	static DIR *opendir(const char *dirpath);
	static struct dirent *readdir(DIR *dirp);
	static int closedir(DIR *dirp);
	#endif // _WIN32
	// minirent.h HEADER END ////////////////////////////////////////

	#endif // NOB_H_

	#ifdef __cplusplus
} // extern "C"

// cpp-definition

#include <cassert>
#include <future>
#include <memory>
#include <vector>

namespace nob {

	enum LANG {
		LANG_C,
		LANG_CPP,
	};

	class CMD {
		Nob_Cmd cmd;

	public:
		CMD() {}
		~CMD() { nob_cmd_free(cmd); }

		void append(const char *arg) { nob_cmd_append(&cmd, arg); }

		bool run() { return nob_cmd_run_sync(cmd); }

		std::future<bool> run_async() {
			return std::async(std::launch::async, [this]() { return this->run(); });
		}
	};

	class Target {
		std::string name;
		std::vector<std::string> sources;
		std::vector<std::string> dependencies;
		LANG lang;

		bool build_c();
		bool build_cpp();

	public:
		Target(std::string name, std::vector<std::string> sources,
			   std::vector<std::string> dependencies)
		: name(name), sources(sources), dependencies(dependencies) {}

		bool build() {
			switch (lang) {
				case LANG_C:
					return build_c();
				case LANG_CPP:
					return build_cpp();
				default:
					return false;
			}
		}
	};

} // namespace nob
#endif // __cplusplus

#ifdef NOB_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
	#endif

	#ifdef _WIN32
	#define strdup _strdup
	#endif

	/*
	 * struct nob_cl_args {
	 *     int argc;
	 *     char **args;
	 *     char **values;
	 * };
	 */

	nob_cl_args *nob_cl_args_use(int argc, char **argv) {
		// TODO: put every definition in a single sting. The same for the values.
		// TODO: faster get and seach calls

		nob_cl_args *result = calloc(1, sizeof(nob_cl_args));
		if (result == NULL)
			return NULL;

		result->argc = argc;
		result->args = calloc(1, sizeof(char *) * argc);
		result->values = calloc(1, sizeof(char *) * argc);
		assert(result->args != NULL || "Buy more RAM lol");
		assert(result->values != NULL || "Buy more RAM lol");

		size_t length = 0;
		size_t *lengths = calloc(1, sizeof(size_t) * argc);
		size_t *equal_symbols = calloc(1, sizeof(size_t) * argc);
		for (int i = 0; i < argc; ++i) {
			lengths[i] = strlen(argv[i]);
			char *equal_symbol = strchr(argv[i], '=');
			if (equal_symbols) {
				equal_symbols[i] = equal_symbol - argv[i];
			}
			length += lengths[i] + 1;
		}
		nob_log(NOB_DEBUG, "String Buffer Length: %d", length);
		for (int i = 0; i < argc; ++i) {
			nob_log(NOB_DEBUG, "arg %d: %ld", i, equal_symbols[i]);
		}

		*(result->args) = calloc(1, sizeof(char) * length + 1); // create a big string
		assert(*(result->args) != NULL || "Buy more RAM lol");

		size_t offset = 0;
		for (int i = 0; i < argc; ++i) {
			nob_memcpy_s(*(result->args) + offset, length - offset + 1, argv[i],
						 lengths[i]);
			nob_log(NOB_DEBUG, "arg %d : String: %s", i, *(result->args) + offset);
			result->args[i] = *(result->args) + offset;
			if (equal_symbols[i] != 0) {
				result->values[i] = *(result->args) + offset + equal_symbols[i];
				result->values[i][0] = '\0';
				result->values[i] += 1;
			}
			offset += lengths[i] + 1;
			nob_log(NOB_DEBUG, "arg %d : String: %s; Value: %s", i, result->args[i],
					result->values[i]);
		}
		free(lengths);
		free(equal_symbols);
		return result;
	}

	size_t nob_args_has(const nob_cl_args *args, const char *arg) {
		for (int i = 0; i < args->argc; ++i) {
			if (strcmp(args->args[i], arg) == 0) {
				return i;
			}
		}
		return NOB_NOT_IN_ARRAY;
	}

	const char *nob_args_get(const nob_cl_args *args, const char *arg) {
		size_t index = nob_args_has(args, arg);
		if (index == 0)
			return NULL;
		return args->values[index];
	}

	void nob_args_free(nob_cl_args *args) {
		free(*(args->args));
		free(args->args);
		free(args->values);
		free(args);
	}

	void nob_args_remove(nob_cl_args *args, const char *arg) {
		size_t index = nob_args_has(args, arg);
		if (index == NOB_NOT_IN_ARRAY) {
			nob_log(NOB_ERROR, "No such argument %s", arg);
			return;
		}
		for (size_t i = index + 1; i < args->argc; ++i) {
			args->args[i - 1] = args->args[i];
			args->values[i - 1] = args->values[i];
		}
		args->argc--;
		free(args->args[index]);
	}

	void nob_args_pop(nob_cl_args *args) {
		if (args->argc == 0)
			return;
		args->argc--;
		free(args->args[args->argc - 1]);
	}

	/*
	 * Nob Posix replacements
	 */

	void nob_memcpy_s(void *dest, size_t destsz, const void *src, size_t count) {
		assert(destsz >= count);
		#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
		memcpy(dest, src, count);
		#else
		memcpy_s(dest, destsz, src, count);
		#endif
	}

	void nob_fopen_s(FILE **f, const char *file, const char *rights) {
		#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
		*f = fopen(file, rights);
		#else
		fopen_s(f, file, rights);
		#endif
	}

	void nob_strcpy_s(char *dest, size_t destsz, const char *src) {
		assert(destsz < strlen(src));
		nob_memcpy_s(dest, destsz, src, strlen(src) + 1);
	}

	static size_t nob_temp_size = 0;
	static char nob_temp[NOB_TEMP_CAPACITY] = {0};

	bool nob_mkdir_if_not_exists(const char *path) {
		#ifdef _WIN32
		int result = _mkdir(path);
		#else
		int result = mkdir(path, 0755);
		#endif
		if (result < 0) {
			if (errno == EEXIST) {
				nob_log(NOB_INFO, "directory `%s` already exists", path);
				return true;
			}
			nob_log(NOB_ERROR, "could not create directory `%s`: %s", path, errno);
			return false;
		}

		nob_log(NOB_INFO, "created directory `%s`", path);
		return true;
	}

	bool nob_copy_file(const char *src_path, const char *dst_path) {
		nob_log(NOB_INFO, "copying %s -> %s", src_path, dst_path);
		#ifdef _WIN32
		if (!CopyFile(src_path, dst_path, FALSE)) {
			nob_log(NOB_ERROR, "Could not copy file: %lu", GetLastError());
			return false;
		}
		return true;
		#else
		int src_fd = -1;
		int dst_fd = -1;
		size_t buf_size = 32 * 1024;
		char *buf = NOB_REALLOC(NULL, buf_size);
		NOB_ASSERT(buf != NULL && "Buy more RAM lol!!");
		bool result = true;

		src_fd = open(src_path, O_RDONLY);
		if (src_fd < 0) {
			nob_log(NOB_ERROR, "Could not open file %s: %errno", src_path, errno);
			nob_return_defer(false);
		}

		struct stat src_stat;
		if (fstat(src_fd, &src_stat) < 0) {
			nob_log(NOB_ERROR, "Could not get mode of file %s: %errno", src_path,
					errno);
			nob_return_defer(false);
		}

		dst_fd = open(dst_path, O_CREAT | O_TRUNC | O_WRONLY, src_stat.st_mode);
		if (dst_fd < 0) {
			nob_log(NOB_ERROR, "Could not create file %s: %errno", dst_path, errno);
			nob_return_defer(false);
		}

		for (;;) {
			ssize_t n = read(src_fd, buf, buf_size);
			if (n == 0)
				break;
			if (n < 0) {
				nob_log(NOB_ERROR, "Could not read from file %s: %errno", src_path,
						errno);
				nob_return_defer(false);
			}
			char *buf2 = buf;
			while (n > 0) {
				ssize_t m = write(dst_fd, buf2, n);
				if (m < 0) {
					nob_log(NOB_ERROR, "Could not write to file %s: %errno", dst_path,
							errno);
					nob_return_defer(false);
				}
				n -= m;
				buf2 += m;
			}
		}

		defer:
		free(buf);
		close(src_fd);
		close(dst_fd);
		return result;
		#endif
	}

	void nob_cmd_render(Nob_Cmd cmd, Nob_String_Builder *render) {
		for (size_t i = 0; i < cmd.count; ++i) {
			const char *arg = cmd.items[i];
			if (arg == NULL)
				break;
			if (i > 0)
				nob_sb_append_cstr(render, " ");
			if (!strchr(arg, ' ')) {
				nob_sb_append_cstr(render, arg);
			} else {
				nob_da_append(render, '\'');
				nob_sb_append_cstr(render, arg);
				nob_da_append(render, '\'');
			}
		}
	}

	Nob_Proc nob_cmd_run_async(Nob_Cmd cmd) {
		if (cmd.count < 1) {
			nob_log(NOB_ERROR, "Could not run empty command");
			return NOB_INVALID_PROC;
		}

		Nob_String_Builder sb = {0};
		nob_cmd_render(cmd, &sb);
		nob_sb_append_null(&sb);
		nob_log(NOB_INFO, "CMD: %s", sb.items);
		nob_sb_free(sb);
		memset(&sb, 0, sizeof(sb));

		#ifdef _WIN32
		// https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output

		STARTUPINFO siStartInfo;
		ZeroMemory(&siStartInfo, sizeof(siStartInfo));
		siStartInfo.cb = sizeof(STARTUPINFO);
		// NOTE: theoretically setting NULL to std handles should not be a problem
		// https://docs.microsoft.com/en-us/windows/console/getstdhandle?redirectedfrom=MSDN#attachdetach-behavior
		// TODO: check for errors in GetStdHandle
		siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		siStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

		PROCESS_INFORMATION piProcInfo;
		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

		// TODO: use a more reliable rendering of the command instead of cmd_render
		// cmd_render is for logging primarily
		nob_cmd_render(cmd, &sb);
		nob_sb_append_null(&sb);
		BOOL bSuccess = CreateProcessA(NULL, sb.items, NULL, NULL, TRUE, 0, NULL,
									   NULL, &siStartInfo, &piProcInfo);
		nob_sb_free(sb);

		if (!bSuccess) {
			nob_log(NOB_ERROR, "Could not create child process: %lu", GetLastError());
			return NOB_INVALID_PROC;
		}

		CloseHandle(piProcInfo.hThread);

		return piProcInfo.hProcess;
		#else
		pid_t cpid = fork();
		if (cpid < 0) {
			nob_log(NOB_ERROR, "Could not fork child process: %errno", errno);
			return NOB_INVALID_PROC;
		}

		if (cpid == 0) {
			// NOTE: This leaks a bit of memory in the child process.
			// But do we actually care? It's a one off leak anyway...
			Nob_Cmd cmd_null = {0};
			nob_da_append_many(&cmd_null, cmd.items, cmd.count);
			nob_cmd_append(&cmd_null, NULL);

			if (execvp(cmd.items[0], (char *const *)cmd_null.items) < 0) {
				nob_log(NOB_ERROR, "Could not exec child process: %errno", errno);
				exit(1);
			}
			NOB_ASSERT(0 && "unreachable");
		}

		return cpid;
		#endif
	}

	bool nob_procs_wait(Nob_Procs procs) {
		bool success = true;
		for (size_t i = 0; i < procs.count; ++i) {
			success = nob_proc_wait(procs.items[i]) && success;
		}
		return success;
	}

	bool nob_proc_wait(Nob_Proc proc) {
		if (proc == NOB_INVALID_PROC)
			return false;

		#ifdef _WIN32
		DWORD result = WaitForSingleObject(proc,    // HANDLE hHandle,
										   INFINITE // DWORD  dwMilliseconds
		);

		if (result == WAIT_FAILED) {
			nob_log(NOB_ERROR, "could not wait on child process: %lu", GetLastError());
			return false;
		}

		DWORD exit_status;
		if (!GetExitCodeProcess(proc, &exit_status)) {
			nob_log(NOB_ERROR, "could not get process exit code: %lu", GetLastError());
			return false;
		}

		if (exit_status != 0) {
			nob_log(NOB_ERROR, "command exited with exit code %lu", exit_status);
			return false;
		}

		CloseHandle(proc);

		return true;
		#else
		for (;;) {
			int wstatus = 0;
			if (waitpid(proc, &wstatus, 0) < 0) {
				nob_log(NOB_ERROR, "could not wait on command (pid %d): %errno", proc,
						errno);
				return false;
			}

			if (WIFEXITED(wstatus)) {
				int exit_status = WEXITSTATUS(wstatus);
				if (exit_status != 0) {
					nob_log(NOB_ERROR, "command exited with exit code %d", exit_status);
					return false;
				}

				break;
			}

			if (WIFSIGNALED(wstatus)) {
				nob_log(NOB_ERROR, "command process was terminated by %s",
						strsignal(WTERMSIG(wstatus)));
				return false;
			}
		}

		return true;
		#endif
	}

	bool nob_cmd_run_sync(Nob_Cmd cmd) {
		Nob_Proc p = nob_cmd_run_async(cmd);
		if (p == NOB_INVALID_PROC)
			return false;
		return nob_proc_wait(p);
	}

	char *nob_shift_args(int *argc, char ***argv) {
		NOB_ASSERT(*argc > 0);
		char *result = **argv;
		(*argv) += 1;
		(*argc) -= 1;
		return result;
	}

	#define nob_assert_msg(condition, msg) assert((condition) || msg)

	bool nob_cstr_starts_with(const char *str, const char *prefix) {
		nob_assert_msg(str != NULL, "str is NULL");
		nob_assert_msg(prefix != NULL, "prefix is NULL");
		size_t strsz = strlen(str);
		size_t prefixsz = strlen(prefix);

		if (strsz < prefixsz)
			return false;

		for (size_t i = 0; i < prefixsz; ++i) {
			if (str[i] != prefix[i]) {
				return false;
			}
		}
		return true;
	}

	void nob_log(Nob_Log_Level level, const char *fmt, ...) {
		#ifndef _Debug
		if (level == NOB_DEBUG)
			return;
		#endif
		nob_assert_msg(level < 3, "Log level is out of range");
		FILE *output = stderr;

		fprintf(output, "%s", Nob_Log_Level_strings[level]);

		va_list args;
		va_start(args, fmt);

		for (const char *p = fmt; *p != '\0'; ++p) {
			if (*p != '%') {
				fputc(*p, output);
				continue;
			}

			if (nob_cstr_starts_with(p + 1, "errno")) {
				int local_errno = va_arg(args, int);
				#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
				fprintf(output, "%s", strerror(local_errno));
				#else
				char buf[1024];
				strerror_s(buf, 1024, local_errno);
				fprintf(output, "%s", buf);
				#endif
				p += 5; // Move past "%errno"
			} else {
				// Handle standard format specifiers
				const char *start = p;
				++p;
				while (*p && !strchr("diuoxXfFeEgGaAcspn%", *p))
					++p;
				if (*p) {
					int len = p - start + 1;
					char format[32]; // Assuming no format specifier is longer than 31
					// characters
					if (len < sizeof(format)) {
						memcpy(format, start, len);
						format[len] = '\0';

						switch (*p) {
							case 'd':
							case 'i':
							case 'u':
							case 'o':
							case 'x':
							case 'X':
								fprintf(output, format, va_arg(args, int));
								break;
							case 'f':
							case 'F':
							case 'e':
							case 'E':
							case 'g':
							case 'G':
							case 'a':
							case 'A':
								fprintf(output, format, va_arg(args, double));
								break;
							case 'c':
								fprintf(output, format, va_arg(args, int));
								break;
							case 's':
								fprintf(output, format, va_arg(args, char *));
								break;
							case 'p':
								fprintf(output, format, va_arg(args, void *));
								break;
							case 'n':
								fprintf(output, format, va_arg(args, int *));
								break;
							case '%':
								fputc('%', output);
								break;
						}
					} else {
						fputs(start, output);
					}
				} else {
					fputc('%', output);
				}
			}
		}

		va_end(args);
		fprintf(output, NOB_LINE_END);
	}

	bool nob_read_entire_dir(const char *parent, Nob_File_Paths *children) {
		bool result = true;
		DIR *dir = NULL;

		dir = opendir(parent);
		if (dir == NULL) {
			nob_log(NOB_ERROR, "Could not open directory %s: %errno", parent, errno);
			nob_return_defer(false);
		}

		errno = 0;
		struct dirent *ent = readdir(dir);
		while (ent != NULL) {
			nob_da_append(children, nob_temp_strdup(ent->d_name));
			ent = readdir(dir);
		}

		if (errno != 0) {
			nob_log(NOB_ERROR, "Could not read directory %s: %errno", parent, errno);
			nob_return_defer(false);
		}

		defer:
		if (dir)
			closedir(dir);
		return result;
	}

	bool nob_write_entire_file(const char *path, const void *data, size_t size) {
		bool result = true;

		FILE *f;
		nob_fopen_s(&f, path, "wb");
		if (f == NULL) {
			nob_log(NOB_ERROR, "Could not open file %s for writing: %errno", path,
					errno);
			nob_return_defer(false);
		}

		//           len
		//           v
		// aaaaaaaaaa
		//     ^
		//     data

		const char *buf = data;
		while (size > 0) {
			size_t n = fwrite(buf, 1, size, f);
			if (ferror(f)) {
				nob_log(NOB_ERROR, "Could not write into file %s: %errno", path, errno);
				nob_return_defer(false);
			}
			size -= n;
			buf += n;
		}

		defer:
		if (f)
			fclose(f);
		return result;
	}

	Nob_File_Type nob_get_file_type(const char *path) {
		#ifdef _WIN32
		DWORD attr = GetFileAttributesA(path);
		if (attr == INVALID_FILE_ATTRIBUTES) {
			nob_log(NOB_ERROR, "Could not get file attributes of %s: %lu", path,
					GetLastError());
			return -1;
		}

		if (attr & FILE_ATTRIBUTE_DIRECTORY)
			return NOB_FILE_DIRECTORY;
		// TODO: detect symlinks on Windows (whatever that means on Windows anyway)
		return NOB_FILE_REGULAR;
		#else  // _WIN32
		struct stat statbuf;
		if (stat(path, &statbuf) < 0) {
			nob_log(NOB_ERROR, "Could not get stat of %s: %errno", path, errno);
			return -1;
		}

		switch (statbuf.st_mode & S_IFMT) {
			case S_IFDIR:
				return NOB_FILE_DIRECTORY;
			case S_IFREG:
				return NOB_FILE_REGULAR;
			case S_IFLNK:
				return NOB_FILE_SYMLINK;
			default:
				return NOB_FILE_OTHER;
		}
		#endif // _WIN32
	}

	bool nob_copy_directory_recursively(const char *src_path,
										const char *dst_path) {
		bool result = true;
		Nob_File_Paths children = {0};
		Nob_String_Builder src_sb = {0};
		Nob_String_Builder dst_sb = {0};
		size_t temp_checkpoint = nob_temp_save();

		Nob_File_Type type = nob_get_file_type(src_path);
		if (type < 0)
			return false;

		switch (type) {
			case NOB_FILE_DIRECTORY: {
				if (!nob_mkdir_if_not_exists(dst_path))
					nob_return_defer(false);
				if (!nob_read_entire_dir(src_path, &children))
					nob_return_defer(false);

				for (size_t i = 0; i < children.count; ++i) {
					if (strcmp(children.items[i], ".") == 0)
						continue;
					if (strcmp(children.items[i], "..") == 0)
						continue;

					src_sb.count = 0;
					nob_sb_append_cstr(&src_sb, src_path);
					nob_sb_append_cstr(&src_sb, "/");
					nob_sb_append_cstr(&src_sb, children.items[i]);
					nob_sb_append_null(&src_sb);

					dst_sb.count = 0;
					nob_sb_append_cstr(&dst_sb, dst_path);
					nob_sb_append_cstr(&dst_sb, "/");
					nob_sb_append_cstr(&dst_sb, children.items[i]);
					nob_sb_append_null(&dst_sb);

					if (!nob_copy_directory_recursively(src_sb.items, dst_sb.items)) {
						nob_return_defer(false);
					}
				}
			} break;

			case NOB_FILE_REGULAR: {
				if (!nob_copy_file(src_path, dst_path)) {
					nob_return_defer(false);
				}
			} break;

			case NOB_FILE_SYMLINK: {
				nob_log(NOB_WARNING, "TODO: Copying symlinks is not supported yet");
			} break;

			case NOB_FILE_OTHER: {
				nob_log(NOB_ERROR, "Unsupported type of file %s", src_path);
				nob_return_defer(false);
			} break;

			default:
				NOB_ASSERT(0 && "unreachable");
		}

		defer:
		nob_temp_rewind(temp_checkpoint);
		nob_da_free(src_sb);
		nob_da_free(dst_sb);
		nob_da_free(children);
		return result;
										}

										char *nob_temp_strdup(const char *cstr) {
											size_t n = strlen(cstr);
											char *result = nob_temp_alloc(n + 1);
											NOB_ASSERT(result != NULL && "Increase NOB_TEMP_CAPACITY");
											nob_memcpy_s(result, n + 1, cstr, n);
											result[n] = '\0';
											return result;
										}

										void *nob_temp_alloc(size_t size) {
											if (nob_temp_size + size > NOB_TEMP_CAPACITY)
												return NULL;
											void *result = &nob_temp[nob_temp_size];
											nob_temp_size += size;
											return result;
										}

										char *nob_temp_sprintf(const char *format, ...) {
											va_list args;
											va_start(args, format);
											int n = vsnprintf(NULL, 0, format, args);
											va_end(args);

											NOB_ASSERT(n >= 0);
											char *result = nob_temp_alloc(n + 1);
											NOB_ASSERT(result != NULL && "Extend the size of the temporary allocator");
											// TODO: use proper arenas for the temporary allocator;
											va_start(args, format);
											vsnprintf(result, n + 1, format, args);
											va_end(args);

											return result;
										}

										void nob_temp_reset(void) { nob_temp_size = 0; }

										size_t nob_temp_save(void) { return nob_temp_size; }

										void nob_temp_rewind(size_t checkpoint) { nob_temp_size = checkpoint; }

										const char *nob_temp_sv_to_cstr(Nob_String_View sv) {
											char *result = nob_temp_alloc(sv.count + 1);
											NOB_ASSERT(result != NULL && "Extend the size of the temporary allocator");
											nob_memcpy_s(result, sv.count + 1, sv.data, sv.count);
											result[sv.count] = '\0';
											return result;
										}

										int nob_needs_rebuild(const char *output_path, const char **input_paths,
															  size_t input_paths_count) {
											#ifdef _WIN32
											BOOL bSuccess;

											HANDLE output_path_fd =
											CreateFile(output_path, GENERIC_READ, 0, NULL, OPEN_EXISTING,
													   FILE_ATTRIBUTE_READONLY, NULL);
											if (output_path_fd == INVALID_HANDLE_VALUE) {
												// NOTE: if output does not exist it 100% must be rebuilt
												if (GetLastError() == ERROR_FILE_NOT_FOUND)
													return 1;
												nob_log(NOB_ERROR, "Could not open file %s: %lu", output_path,
														GetLastError());
												return -1;
											}
											FILETIME output_path_time;
											bSuccess = GetFileTime(output_path_fd, NULL, NULL, &output_path_time);
											CloseHandle(output_path_fd);
											if (!bSuccess) {
												nob_log(NOB_ERROR, "Could not get time of %s: %lu", output_path,
														GetLastError());
												return -1;
											}

											for (size_t i = 0; i < input_paths_count; ++i) {
												const char *input_path = input_paths[i];
												HANDLE input_path_fd =
												CreateFile(input_path, GENERIC_READ, 0, NULL, OPEN_EXISTING,
														   FILE_ATTRIBUTE_READONLY, NULL);
												if (input_path_fd == INVALID_HANDLE_VALUE) {
													// NOTE: non-existing input is an error cause it is needed for building in
													// the first place
													nob_log(NOB_ERROR, "Could not open file %s: %lu", input_path,
															GetLastError());
													return -1;
												}
												FILETIME input_path_time;
												bSuccess = GetFileTime(input_path_fd, NULL, NULL, &input_path_time);
												CloseHandle(input_path_fd);
												if (!bSuccess) {
													nob_log(NOB_ERROR, "Could not get time of %s: %lu", input_path,
															GetLastError());
													return -1;
												}

												// NOTE: if even a single input_path is fresher than output_path that's 100%
												// rebuild
												if (CompareFileTime(&input_path_time, &output_path_time) == 1)
													return 1;
											}

											return 0;
											#else
											struct stat statbuf = {0};

											if (stat(output_path, &statbuf) < 0) {
												// NOTE: if output does not exist it 100% must be rebuilt
												if (errno == ENOENT)
													return 1;
												nob_log(NOB_ERROR, "could not stat %s: %errno", output_path, errno);
												return -1;
											}
											int output_path_time = statbuf.st_mtime;

											for (size_t i = 0; i < input_paths_count; ++i) {
												const char *input_path = input_paths[i];
												if (stat(input_path, &statbuf) < 0) {
													// NOTE: non-existing input is an error cause it is needed for building in
													// the first place
													nob_log(NOB_ERROR, "could not stat %s: %errno", input_path, errno);
													return -1;
												}
												int input_path_time = statbuf.st_mtime;
												// NOTE: if even a single input_path is fresher than output_path that's 100%
												// rebuild
												if (input_path_time > output_path_time)
													return 1;
											}

											return 0;
											#endif
															  }

															  int nob_needs_rebuild1(const char *output_path, const char *input_path) {
																  return nob_needs_rebuild(output_path, &input_path, 1);
															  }

															  bool nob_rename(const char *old_path, const char *new_path) {
																  nob_log(NOB_INFO, "renaming %s -> %s", old_path, new_path);
																  #ifdef _WIN32
																  if (!MoveFileEx(old_path, new_path, MOVEFILE_REPLACE_EXISTING)) {
																	  nob_log(NOB_ERROR, "could not rename %s to %s: %lu", old_path, new_path,
																			  GetLastError());
																	  return false;
																  }
																  #else
																  if (rename(old_path, new_path) < 0) {
																	  nob_log(NOB_ERROR, "could not rename %s to %s: %errno", old_path, new_path,
																			  errno);
																	  return false;
																  }
																  #endif // _WIN32
																  return true;
															  }

															  bool nob_read_entire_file(const char *path, Nob_String_Builder *sb) {
																  bool result = true;

																  FILE *f;
																  nob_fopen_s(&f, path, "rb");
																  if (f == NULL)
																	  nob_return_defer(false);
																  if (fseek(f, 0, SEEK_END) < 0)
																	  nob_return_defer(false);
																  long m = ftell(f);
																  if (m < 0)
																	  nob_return_defer(false);
																  if (fseek(f, 0, SEEK_SET) < 0)
																	  nob_return_defer(false);

																  size_t new_count = sb->count + m;
																  if (new_count > sb->capacity) {
																	  sb->items = realloc(sb->items, new_count);
																	  NOB_ASSERT(sb->items != NULL && "Buy more RAM lool!!");
																	  sb->capacity = new_count;
																  }

																  fread(sb->items + sb->count, m, 1, f);
																  if (ferror(f)) {
																	  // TODO: Afaik, ferror does not set errno. So the error reporting in defer
																	  // is not correct in this case.
																	  nob_return_defer(false);
																  }
																  sb->count = new_count;

																  defer:
																  if (!result)
																	  nob_log(NOB_ERROR, "Could not read file %s: %errno", path, errno);
																  if (f)
																	  fclose(f);
																  return result;
															  }

															  Nob_String_View nob_sv_chop_by_delim(Nob_String_View *sv, char delim) {
																  size_t i = 0;
																  while (i < sv->count && sv->data[i] != delim) {
																	  i += 1;
																  }

																  Nob_String_View result = nob_sv_from_parts(sv->data, i);

																  if (i < sv->count) {
																	  sv->count -= i + 1;
																	  sv->data += i + 1;
																  } else {
																	  sv->count -= i;
																	  sv->data += i;
																  }

																  return result;
															  }

															  Nob_String_View nob_sv_from_parts(const char *data, size_t count) {
																  Nob_String_View sv;
																  sv.count = count;
																  sv.data = data;
																  return sv;
															  }

															  Nob_String_View nob_sv_trim_left(Nob_String_View sv) {
																  size_t i = 0;
																  while (i < sv.count && isspace(sv.data[i])) {
																	  i += 1;
																  }

																  return nob_sv_from_parts(sv.data + i, sv.count - i);
															  }

															  Nob_String_View nob_sv_trim_right(Nob_String_View sv) {
																  size_t i = 0;
																  while (i < sv.count && isspace(sv.data[sv.count - 1 - i])) {
																	  i += 1;
																  }

																  return nob_sv_from_parts(sv.data, sv.count - i);
															  }

															  Nob_String_View nob_sv_trim(Nob_String_View sv) {
																  return nob_sv_trim_right(nob_sv_trim_left(sv));
															  }

															  Nob_String_View nob_sv_from_cstr(const char *cstr) {
																  return nob_sv_from_parts(cstr, strlen(cstr));
															  }

															  bool nob_sv_eq(Nob_String_View a, Nob_String_View b) {
																  if (a.count != b.count) {
																	  return false;
																  } else {
																	  return memcmp(a.data, b.data, a.count) == 0;
																  }
															  }

															  // RETURNS:
															  //  0 - file does not exists
															  //  1 - file exists
															  // -1 - error while checking if file exists. The error is logged
															  int nob_file_exists(const char *file_path) {
																  #if _WIN32
																  // TODO: distinguish between "does not exists" and other errors
																  DWORD dwAttrib = GetFileAttributesA(file_path);
																  return dwAttrib != INVALID_FILE_ATTRIBUTES;
																  #else
																  struct stat statbuf;
																  if (stat(file_path, &statbuf) < 0) {
																	  if (errno == ENOENT)
																		  return 0;
																	  nob_log(NOB_ERROR, "Could not check if file %s exists: %errno", file_path,
																			  errno);
																	  return -1;
																  }
																  return 1;
																  #endif
															  }

															  // minirent.h SOURCE BEGIN ////////////////////////////////////////
															  #ifdef _WIN32
															  struct DIR {
																  HANDLE hFind;
																  WIN32_FIND_DATA data;
																  struct dirent *dirent;
															  };

															  DIR *opendir(const char *dirpath) {
																  assert(dirpath);

																  char buffer[MAX_PATH];
																  snprintf(buffer, MAX_PATH, "%s\\*", dirpath);

																  DIR *dir = (DIR *)calloc(1, sizeof(DIR));

																  dir->hFind = FindFirstFile(buffer, &dir->data);
																  if (dir->hFind == INVALID_HANDLE_VALUE) {
																	  // TODO: opendir should set errno accordingly on FindFirstFile fail
																	  // https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror
																	  errno = ENOSYS;
																	  goto fail;
																  }

																  return dir;

																  fail:
																  if (dir) {
																	  free(dir);
																  }

																  return NULL;
															  }

															  struct dirent *readdir(DIR *dirp) {
																  assert(dirp);

																  if (dirp->dirent == NULL) {
																	  dirp->dirent = (struct dirent *)calloc(1, sizeof(struct dirent));
																  } else {
																	  if (!FindNextFile(dirp->hFind, &dirp->data)) {
																		  if (GetLastError() != ERROR_NO_MORE_FILES) {
																			  // TODO: readdir should set errno accordingly on FindNextFile fail
																			  // https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror
																			  errno = ENOSYS;
																		  }

																		  return NULL;
																	  }
																  }

																  memset(dirp->dirent->d_name, 0, sizeof(dirp->dirent->d_name));

																  strncpy_s(dirp->dirent->d_name, MAX_PATH, dirp->data.cFileName,
																			sizeof(dirp->dirent->d_name) - 1);

																  return dirp->dirent;
															  }

															  int closedir(DIR *dirp) {
																  assert(dirp);

																  if (!FindClose(dirp->hFind)) {
																	  // TODO: closedir should set errno accordingly on FindClose fail
																	  // https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror
																	  errno = ENOSYS;
																	  return -1;
																  }

																  if (dirp->dirent) {
																	  free(dirp->dirent);
																  }
																  free(dirp);

																  return 0;
															  }
															  #endif // _WIN32
															  // minirent.h SOURCE END ////////////////////////////////////////

															  #ifdef __cplusplus
} // extern "C"

// cpp-implementation

bool nob::Target::build_c() {
	nob::CMD cmd;
	cmd.append("clang"); // TODO: allowing other compilers
	#ifdef _WIN32
	cmd.append("-o", name + ".exe");
	#else
	cmd.append("-o", name);
	#endif

	for (auto &source : sources) {
		cmd.append(source);
	}
	return cmd.run();
}

bool nob::Target::build_cpp() {
	nob::CMD cmd;
	cmd.append("clang++"); // TODO: allowing other compilers
	#ifdef _WIN32
	cmd.append("-o", name + ".exe");
	#else
	cmd.append("-o", name);
	#endif

	for (auto &source : sources) {
		cmd.append(source);
	}
	return cmd.run();
}

#endif // __cplusplus
#endif // NOB_IMPLEMENTATION
