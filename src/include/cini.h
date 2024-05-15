#ifndef CINI_H
#define CINI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#ifndef CINI_COMMENT_INDICATOR
#define CINI_COMMENT_INDICATOR ';'
#endif

#ifdef __cplusplus
#define restrict 
#endif

#ifndef NO_STD_TYPES

#include <stdint.h>


#ifndef _MAX_TYPES_DEFINED
#define _MAX_TYPES_DEFINED

typedef intmax_t smax;
typedef size_t umax;
#define PRIMAX_T "d"

#endif /* _MAX_TYPES_DEFINED */

typedef umax ptr_t;

#endif /* NO_STD_TYPES */

#if !defined(CINI_COSTUME_MACROS_DEFINED) && !defined(CINI_MACROS_DEFINED)
/*
 * UTILITY_DEFINES
 * -----------------
 * 
 * CINI_FUNC(name)
 * CINI_PUBLIC_UTIL(name)
 * CINI_PRIVATE_UTIL(name)
 * CINI_MEM(name)			// memory func wrappers
 * CINI_GCC(name)			// gcc funcs for msvc
 * 
 */

#define CINI_FUNC(name) cini_##name
#define CINI_PUBLIC_UTIL(name) CINI_FUNC(util_##name)
#define CINI_PRIVATE_UTIL(name) __cini_util_##name
#define CINI_MEM(name) cinim_x##name
#define CINI_DMEM(name) cinim_x##name
#define CINI_GCC(name) cinigcc_##name

/*
* DEBUG
*/
#if defined(CINI_DEBUG) || defined(_INISG_DEBUG)
#	ifndef CINI_DEBUG
#		define CINI_DEBUG 0
#	endif
#	define DEBUG CINI_DEBUG
#endif /* defined(DEBUG) || defined(_DEBUG) */

/* Error handling */
#ifdef CINI_DEBUG
#	define _ferror_cini(str) CINI_FUNC(fatal)(str, __FILE__, __LINE__)
#	define ifdebug(code) code
#else 
#	define _ferror_cini(str) CINI_GCC(fatal)(str)
#	define ifdebug(code) 
#endif /* DEBUG */

/*
* Debug Logging
* Use Debug Modes
*/

#ifdef CINI_DEBUG 
#	if DEBUG == 2
#		undef DEBUG
#		define DEBUG 1
#	endif
#	if DEBUG == 1
	/*
	* Allocation
	*/
#		define CINI_DEBUG_L1
#		undef ifdebug1
#		define ifdebug1(code) code

#		undef CINI_DMEM
#		undef CINI_MEM
#		define CINI_DMEM(name) debug_cinim_x##name
#		define CINI_MEM(name) CINI_DB_##name

#		define CINI_DB_malloc(size) CINI_DMEM(malloc)(size, __FILE__, __LINE__)
#		define CINI_DB_realloc(ptr, size) CINI_DMEM(realloc)(ptr, size, __FILE__, __LINE__)
#		undef DEBUG
#		define DEBUG 0
#	else
#		define ifdebug1(code) 
#	endif /* Debug level 1 */
#else
#	define ifdebug1(code) 
#endif


#define CINI_MACROS_DEFINED
#endif /* !defined(CINI_COSTUME_MACROS_DEFINED) && !defined(CINI_MACROS_DEFINED) */

typedef struct CINI_ENTRY {
	char* key;
	char* value;
} ciniEntry_t;

typedef struct CINI_SECTION {
	char* name;
	umax numEntries;
	ciniEntry_t *entries;
} ciniSection_t;

typedef struct CINI_DATA {
	umax numSections;
	ciniSection_t* sections;
} cini_t;



cini_t*			CINI_FUNC( parse 	) 		(const char*); 								// arg0 is a string that contains the ini in text format
char* 			CINI_FUNC( print 	)		(const cini_t* const); 						// return the ini in text format

ciniEntry_t* 	CINI_FUNC( GetEntry 	) 	(const ciniSection_t* const, const char*);	// returns the entry in the section arg0 
																						// with the same content in key as in arg1
																						// if entry doesn't exist, return NULL

ciniSection_t* 	CINI_FUNC( GetSection 	) 	(const cini_t* const, const char*);			// returns the section in the ini structure arg0
																						// with the same content in section.name as in arg1
																						// if section does not exist, return NULL

ciniEntry_t* 	CINI_FUNC( AddEntry 	)	(ciniSection_t*, const char*, const char*);	// returns the Entry created. 
																						// Takes the section, the key and the value.
																						// if a entry with that key exists it returns it.

ciniSection_t* 	CINI_FUNC( AddSection 	) 	(cini_t*, const char*);						// returns the Section created. 
																						// Takes the ini structure and the section-name.
																						// if a section with that name exists it return it.

char			CINI_FUNC( ClearEntry )	(ciniEntry_t*);								// frees everthing inside the entry 
																						// pointer stays valid, even though the entry is empty

char			CINI_FUNC( ClearSection )	(ciniSection_t*);							// frees and deletes everything inside the section
																						// pointer stays valide, even though the section is empty

void 			CINI_FUNC( RemoveEntry	) 	(ciniSection_t*, const char*);				// removes the entry in a section, 
																						// that corresponds to the second argument given

void 			CINI_FUNC( RemoveSection ) (cini_t*, const char*);						// removes the section in the ini structure,
																						// that correspondes to the section-name given in the second argument

void 			CINI_FUNC( RemoveIni 	)	(cini_t*);									// frees the complete ini structure with all it sub-structures

void			CINI_FUNC( ChangeValue )	(ciniEntry_t*, const char*);					// changes the value of the entry given in arg0
																						// to the value in arg1 that has the size in arg2

/*
 * The way it works
 *
 * get	-> linear search
 *
 * 
 * add	-> does it exist? yes? get it, return it. 
 *		-> no? create it. return it.
 *
*/

#if !defined(INISG_SMEM_DEFINED)

#include <stdlib.h>

void* CINI_DMEM( malloc	)	(umax size 
#ifdef CINI_DEBUG_L1
	, const char* file, umax line
#endif
	);

void* CINI_DMEM( realloc	)	(void *ptr, umax size 
#ifdef CINI_DEBUG_L1
	, const char* file, umax line
#endif
	);

#endif /* !defined(INISG_SMEM_DEFINED) */

/*
* Compiler specific defines
*/

#if !defined(INISG_GCC_DEFINED)
void CINI_GCC(fatal) (const char*);
char* CINI_GCC(strdup) (const char*);
char* CINI_GCC(strtok) (char* restrict, const char* restrict);
#endif /* !defined(INISG_GCC_DEFINED) */


#ifdef CINI_IMPLEMENTATION

void CINI_GCC(fatal) (const char* c) {
	fprintf(stderr, "\n%s\n", c);
	abort();
}

void CINI_FUNC(fatal) (const char* str, const char* file, umax line) {
	fprintf(stderr, "Fatal Error:\n\t%s\n\nIn file:\t%s\nOn line:\t%" PRIMAX_T "\n\n", str, file, line);
	abort();
}

char* CINI_GCC(strdup) (const char* str) {
	char* newstr = NULL;
#ifdef _WIN32 
	newstr = _strdup(str);
#else
	newstr = strdup(str);
#endif
	return newstr;
}

char* CINI_GCC(strtok) (char* restrict str, const char* restrict delim) {
#ifdef _WIN32
	static char* next_token;
	if(str) next_token = NULL;
	return strtok_s(str, delim, &next_token);
#else
	return strtok(str, delim);
#endif /*_MSC_VER*/
}

#if !defined(INISG_SMEM_DEFINED)

/* malloc */
void * CINI_DMEM( malloc ) (umax size 
#ifdef CINI_DEBUG_L1
	, const char* file, umax line
#endif
	)
{
	ifdebug1(printf("malloc:   %" PRIMAX_T "\tLine: %" PRIMAX_T "\n", size, line));
	void *value = malloc (size);
	if (!value) {
		ifdebug1(fprintf(stderr, "mallocation failure\nFile:\t%s\t\tLine:\t%" PRIMAX_T "\n", file, line));
		_ferror_cini("Virtual memory exhausted");
	}
	return value; 
}

void* CINI_DMEM( realloc ) (void *ptr, umax size 
#ifdef CINI_DEBUG_L1
	, const char* file, umax line
#endif
	)
{
	ifdebug1(printf("realloc:  %" PRIMAX_T "\tLine: %" PRIMAX_T "\n", size, line));
	void* value = realloc(ptr, size);
	if (!value) {
		ifdebug1(fprintf(stderr, "reallocation failure.\nFile:\t%s\t\tLine:\t%" PRIMAX_T "\n", file, line));
		_ferror_cini("Virtual memory exhausted");
	}
	return value;
}

#endif // !defined(INISG_SMEM_DEFINED)

#if !defined(CINI_COSTUME_UTILS_DEFINED) && !defined(CINI_UTILS_DEFINED)

/* ----------------------
 * 	Utilities
 * ----------------------
 * strtrm:	trims the given string and returns it.
 * 		creates a new string to return 
 *		the original is not touched.
*/

char * CINI_PRIVATE_UTIL( ltrim_inp ) (char *s)
{
	while(isspace(*s)) s++;
	return s;
}

char* CINI_PRIVATE_UTIL( rtrim_inp ) (char *s)
{
	char* back = s + strlen(s);
	while(isspace(*--back));
	*(back+1) = '\0';
	return s;
}

char* CINI_PRIVATE_UTIL( strtrim_inp )(char *s)
{
	return CINI_PRIVATE_UTIL( rtrim_inp )(CINI_PRIVATE_UTIL( ltrim_inp )(s));
}

char* CINI_PUBLIC_UTIL( strtrm )( const char *s ) {
	if(!s) {
		return NULL;
	}
	char* str = CINI_GCC(strdup)(s);
	char* tmp = CINI_PRIVATE_UTIL(strtrim_inp)(str);
	if(!tmp) {
		return NULL;
	}
	char* res = CINI_GCC(strdup)(tmp);
	res[strlen(tmp)] = '\0';
	free(str);
	tmp = NULL;
	return res;
}


void CINI_PRIVATE_UTIL( removeCommentFromLine ) (char** line) {
	char* seperator = strchr(*line, CINI_COMMENT_INDICATOR);
	if(!seperator) {return;}
	ptr_t len = (ptr_t)(seperator - *line);
	char* newstr = (char*)CINI_MEM(malloc)(len + 1);
	memcpy(newstr, *line, len * sizeof(char));
	newstr[len] = 0;
	free(line);
	*line = newstr;
}

char*  CINI_PUBLIC_UTIL(strcat)(const char* str1, const char* str2) {
	if(!str1 || !str2) {
		fprintf(stderr, "inisg string-cat error: \n\tstring pointer is NULL;\n");
		return NULL;
	}
	char* new_str = (char*)CINI_MEM(malloc)(sizeof(char) * (strlen(str1) + strlen(str2) + 1));
	new_str[0] = '\0';
	strcat(new_str, str1);
	strcat(new_str, str2);
	return new_str;
}

/* returns a pointer to the first argument */
void CINI_PUBLIC_UTIL(strapp)(char** str, const char* str2) {
	if(!str || !(*str)) {
		fprintf(stderr, "inisg string-append error: \n\tdest pointer is NULL;\n");
		return;
	}
	if(!str2) {
		return;
	}
	umax len = strlen(*str);
	umax slen = len + strlen(str2);
	if(slen < 1) return;
	*str = (char*)CINI_MEM(realloc)(*str, sizeof(char) * (slen + 1));
	memset(&(*str)[len], 0, slen - len);
	strcpy(&((*str)[len]), str2);
	(*str)[slen] = 0;
}

#define CINI_UTILS_DEFINED
#endif /* !defined(CINI_COSTUME_UTILS) && !defined(CINI_UTILS_DEFINED) */

/* parse */

#define CINI_SECNAME_MAX_LEN 64

cini_t* CINI_FUNC( parse ) (const char* iniContentString) {
	char* inistr = CINI_GCC(strdup)(iniContentString);
	char* token = CINI_GCC(strtok)(inistr, "\n\0");

	/* resulting ini */
	cini_t* inires = (cini_t*)CINI_MEM(malloc)(sizeof(cini_t));
	inires->sections = NULL;
	inires->numSections = 0;
	/* active section */
	ciniSection_t* acsec = NULL;

	while(token) {
		char* tline = CINI_GCC(strdup)(token);
		CINI_PRIVATE_UTIL(removeCommentFromLine)(&tline); 	/* Comments will not be saved and therefore,
									 * will vanish when being unparsed */
		char* line = CINI_PUBLIC_UTIL(strtrm)(tline);
		free(tline);

		if(!strlen(line)) { /* skip if the line is empty */
			free(line);
			token = CINI_GCC(strtok)(NULL, "\n\0");	
			continue;
		}

		/* section */
		/* Syntax: [{name}] */
		if(*line == '[') {
			char* tmp = line;

			char* secname = (char*)CINI_MEM(malloc)(sizeof(char*) * (CINI_SECNAME_MAX_LEN+1));
			char* strtmp = secname;
			/* go from '[' to ']' in within the string lays. syntax: "[...]" */
			while(*tmp++ != ']' && (tmp - line) < (CINI_SECNAME_MAX_LEN + 1)) {
				*strtmp++ = *tmp;
			}
			*(strtmp-1) = '\0';
			
			char* name = CINI_GCC(strdup)(secname);
			free(secname);
			char* trmname = CINI_PUBLIC_UTIL(strtrm)(name);
			free(name);

			if(!strlen(trmname)) {
				free(trmname);
				trmname = CINI_GCC(strdup)("empty");
			}

			acsec = CINI_FUNC(AddSection)(inires, name);
			free(trmname);
			free(line);
			token = CINI_GCC(strtok)(NULL, "\n\0");	
			continue;
		}

		/* entry */
		/* Syntax: {key} = {value} */
		char* equal = strchr(line, '=');
		if(equal) {
			ptr_t index = (ptr_t)(equal - line);
			if(index < 1) { 
				free(line);
				token = CINI_GCC(strtok)(NULL, "\n\0");
			}
			char* value = CINI_GCC(strdup)(equal + 1);
			*equal = '\0';
			char* key = CINI_GCC(strdup)(line);
			*equal = '=';

			char* k = CINI_PUBLIC_UTIL(strtrm)(key);
			char* v = CINI_PUBLIC_UTIL(strtrm)(value);
			free(key);
			free(value);
			if(!strlen(k)) {
				fprintf(stderr, "inisg: key is empty!\n");
				
				free(k);
				free(v);
				free(line);
				token = CINI_GCC(strtok)(NULL, "\n\0");	
				continue;
			}

			if(!acsec) {
				acsec = CINI_FUNC(AddSection(inires, "empty"));
			}
			if(!strlen(v)) {
				CINI_FUNC(AddEntry)(acsec, k, NULL);
				
				free(k);
				free(v);
				free(line);
				token = CINI_GCC(strtok)(NULL, "\n\0");	
				continue;
	
			}
			CINI_FUNC(AddEntry)(acsec, k, v);
			free(k);
			free(v);
		}
	

		free(line);
		token = CINI_GCC(strtok)(NULL, "\n\0");
	}
	
	free(inistr);
	ifdebug1(printf("finished parsing\n"));
	return inires;
end:
	free(inistr);
	CINI_FUNC(RemoveIni)(inires);
	return NULL;
}

/* print(unparse) */


char* CINI_FUNC( print ) (const cini_t* const ini) {
	char* inistr = (char*)CINI_MEM(malloc)(sizeof(char));
	if(!ini->numSections && !ini->sections) return NULL;
	*inistr = 0;
	ciniSection_t* acsec = CINI_FUNC(GetSection)(ini, "empty");
	ciniEntry_t* acetr = NULL;
	if(acsec) {
		if(!acsec->numEntries || !acsec->entries) goto skipempty;
		for(umax i = 0; i < acsec->numEntries; i++) {
			acetr = &acsec->entries[i];
			char* entry = (char*)CINI_MEM(malloc)(sizeof(char));
			*entry = 0;
			CINI_PUBLIC_UTIL(strapp)(&entry, acetr->key);
			CINI_PUBLIC_UTIL(strapp)(&entry, " = ");
			CINI_PUBLIC_UTIL(strapp)(&entry, acetr->value);
			
			CINI_PUBLIC_UTIL(strapp)(&inistr, entry);
			CINI_PUBLIC_UTIL(strapp)(&inistr, "\n");
			free(entry);
		}
	}
skipempty:
	for(umax as = 0; as < ini->numSections; as++) {
		acsec = &ini->sections[as];
		if(!acsec->name || (!strcmp(acsec->name, "empty"))) {
			CINI_PUBLIC_UTIL(strapp)(&inistr, "\n");
			continue;
		}
		char* section = (char*)CINI_MEM(malloc)(sizeof(char));
		*section = 0;

		CINI_PUBLIC_UTIL(strapp)(&section, "[ ");
		CINI_PUBLIC_UTIL(strapp)(&section, acsec->name);
		CINI_PUBLIC_UTIL(strapp)(&section, " ]\n");
		
		if(!acsec->numEntries || !acsec->entries) {
			CINI_PUBLIC_UTIL(strapp)(&inistr, section);
			free(section);
			CINI_PUBLIC_UTIL(strapp)(&inistr, "\n");
			continue;
		}

		for(umax ae = 0; ae < acsec->numEntries; ae++) {
			acetr = &acsec->entries[ae];
			char* entry = (char*)CINI_MEM(malloc)(sizeof(char));
			*entry = 0;

			CINI_PUBLIC_UTIL(strapp)(&entry, acetr->key);
			CINI_PUBLIC_UTIL(strapp)(&entry, " = ");
			CINI_PUBLIC_UTIL(strapp)(&entry, acetr->value);

			CINI_PUBLIC_UTIL(strapp)(&section, entry);
			CINI_PUBLIC_UTIL(strapp)(&section, "\n");
			free(entry);
		}
		CINI_PUBLIC_UTIL(strapp)(&inistr, section);
		free(section);
		CINI_PUBLIC_UTIL(strapp)(&inistr, "\n");
	}
	return inistr;
}

// GetEntry
ciniEntry_t* CINI_FUNC( GetEntry ) (const ciniSection_t* const sec, const char* key) {
	for(umax i = 0; i < sec->numEntries; i++) {
		if (!strcmp(sec->entries[i].key, key)) {
			return &sec->entries[i];
		}
	}
	return NULL;
}

// GetSection
ciniSection_t* CINI_FUNC( GetSection ) (const cini_t* const ini, const char* name) {
	if(!ini) return NULL;
	for(umax i = 0; i < ini->numSections; i++) {
		if(!strcmp(ini->sections[i].name, name)) {
			return &ini->sections[i];
		}
	}
	return NULL;
}
// AddEntry
ciniEntry_t* CINI_FUNC( AddEntry ) (ciniSection_t* sec, const char* key, const char* value) {
	ciniEntry_t* e = CINI_FUNC(GetEntry)(sec, key);
	if(e && sec) return e;
	if(!sec->entries) {
		sec->entries = (ciniEntry_t*)CINI_MEM(malloc)(0);
	}

	sec->entries = (ciniEntry_t*)CINI_MEM(realloc)
				(sec->entries ,sizeof(ciniEntry_t) * (sec->numEntries+1));
	e = &sec->entries[sec->numEntries++];
	e->key = CINI_GCC(strdup)(key);
	e->value = CINI_PUBLIC_UTIL(strtrm)(CINI_GCC(strdup)(value));
	return e;
}

// AddSection
ciniSection_t* CINI_FUNC( AddSection ) (cini_t* ini, const char* name) {
	ciniSection_t* sec = CINI_FUNC(GetSection)(ini, name);
	if(sec && ini) return sec;
	if(!ini->sections) {
		ini->sections = (ciniSection_t*)CINI_MEM(malloc)(0);
	}
	ini->sections = (ciniSection_t*)CINI_MEM(realloc)
				(ini->sections, sizeof(ciniSection_t) * (ini->numSections+1));
	sec = &ini->sections[ini->numSections++];
	sec->name = CINI_GCC(strdup)(name);
	sec->entries = NULL;
	sec->numEntries = 0;
	return sec;
}

// ClearEntry
char CINI_FUNC( ClearEntry ) (ciniEntry_t* e) {
	if(!e) return 1;
	if(e->key) {
		free(e->key);
	}
	if(e->value) {
		free(e->value);
	}
	return 0;
}

// ClearSection
char CINI_FUNC( ClearSection ) (ciniSection_t* sec) {
	if(!sec) return 1;
	if(sec->name) {
		free(sec->name);
	}
	if(sec->entries) {
		for(umax i = 0; i < sec->numEntries; i++) {
			ciniEntry_t* e = &sec->entries[i];
			char r = CINI_FUNC(ClearEntry)(e);
		}
	}
	free(sec->entries);
	sec->numEntries = 0;
	return 0;
}

// RemoveEntry
void CINI_FUNC( RemoveEntry ) (ciniSection_t* sec, const char* key) {
	ciniEntry_t* entry = CINI_FUNC(GetEntry)(sec, key);
	if(!entry) {
		return;	
	}
	char r = CINI_FUNC(ClearEntry)(entry);
	if(r) {
		printf("error in clearing entry\n");
		return;
	}

	ptr_t index = (ptr_t)(entry - sec->entries);
	
	umax nbefore = index;
	umax nafter  = sec->numEntries - (index+1);
	ciniEntry_t* entriesAfter;
	if(nafter) {
		entriesAfter = (ciniEntry_t*)CINI_MEM(malloc)
						(sizeof(ciniEntry_t) * nafter);
		memcpy(entriesAfter, 
			entry + 1, 
			sizeof(ciniEntry_t) * nafter);
	}
	sec->entries = (ciniEntry_t*)CINI_MEM(realloc)
				(sec->entries, 
				sizeof(ciniEntry_t) * (sec->numEntries-1) );
	if(nafter) {
		memcpy(
			sec->entries +  index, 
			entriesAfter,
			sizeof(ciniEntry_t) * nafter);
		free(entriesAfter);
	}
	sec->numEntries -= 1;
}

// RemoveSection
void CINI_FUNC( RemoveSection ) (cini_t* ini, const char* name) {
	ciniSection_t* sec = CINI_FUNC(GetSection)(ini, name);
	if(!sec) {
		return;
	}
	char r = CINI_FUNC(ClearSection)(sec);
	if(r) {
		printf("error in clearing section\n");
		return;
	}

	ptr_t index = (ptr_t)(sec - ini->sections);

	umax nbefore = index;
	umax nafter  = ini->numSections - (index+1);


	ciniSection_t* secsAfter;

	if(nafter) {
		secsAfter = (ciniSection_t*)CINI_MEM(malloc)
						(sizeof(ciniSection_t) * nafter);
		memcpy(secsAfter,
			sec + 1,
			sizeof(ciniSection_t) * nafter);
	}
	ini->sections = (ciniSection_t*)CINI_MEM(realloc)
				(ini->sections,
				sizeof(ciniSection_t) * (ini->numSections - 1) );
	if(nafter) {
		memcpy(
			ini->sections + index,
			secsAfter,
			sizeof(ciniSection_t) * nafter);
		free(secsAfter);
	}
	ini->numSections -= 1;
}


// RemoveIni
void CINI_FUNC( RemoveIni ) (cini_t* ini) {
	for(umax i = 0; i < ini->numSections; i++) {
		char r = CINI_FUNC(ClearSection)(&ini->sections[i]);
		if(r) {
			printf("error in clearing section");
		}
	}
	free(ini->sections);
	free(ini);
}

// ChangeValue

void CINI_FUNC( ChangeValue ) (ciniEntry_t* e, const char* str) {
	if(e->value) {
		free(e->value);
	}
	e->value = CINI_PUBLIC_UTIL(strtrm)(str);
}

#endif // CINI_IMPLEMENTATION

#undef _ferror_inisg
#undef ifdebug

#define cini_ForEachSection(element, ini) for(element = (ini->numSections > 0 && ini->sections) ? ini->sections : NULL; (element != NULL && ((element - ini->sections) < ini->numSections)); element++)

#define cini_ForEachEntry(element, sec) for(element = (sec->numEntries > 0 && sec->entries) ? sec->entries : NULL; (element != NULL && ((element - sec->entries) < sec->numEntries)); element++)


#if defined(__cplusplus) && !defined(CINI_NO_CPP)

namespace cini {
	using string = std::string;

	namespace helper {

		template<typename T>
		class ciniLIST {
			using itemtype = T;

			itemtype* list = nullptr;
			size_t nAllocated = 0;
			size_t nUsed = 0;

		public:
			ciniLIST(size_t _alloc = 2);
			~ciniLIST();

			itemtype& operator[](size_t index);
			
			itemtype& get(size_t index);
			void add(itemtype& item);
			void del(size_t index);
		};
	}

	class cini_Entry {
	private:
		string mkey = "";
		string mvalue = "";
	public:
		cini_Entry(string _key = "", string _value = "");

		string getKey();
		string& getValue();

		// get/set value
		string& operator()();
		void operator(string& value);

		// get/set key
		string operator[]();
		void operator[string&];
	}

	
	class cini_Section {
		helper::ciniLIST<cini_Entry> entries;
		string name;
	public:
		cini_Section(string name = "");
		
		cini_Entry& get(string key = "");
		cini_Entry& get(size_t index);

	};


	class cINI {
		helper::ciniLIST<cini_Section> sections;
		
	public:
		size_t size();
		cini_Section& get(string name = "");
		cini_Section& get(size_t index);
	};


#ifdef CINI_IMPLEMENTATION

namespace helper {
	template<typename T>
	ciniLIST<T>::ciniLIST(size_t _alloc) {
		list = new itemtype[_alloc];
		nAllocated = _alloc;
	}

	template<typename T>
	ciniLIST<T>::~ciniLIST() {
		delete[] list;
	}
}

#endif // CINI_IMPLEMENTATION

}
#endif // CPP


#endif // CINI_H
