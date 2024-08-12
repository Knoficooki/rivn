#ifdef __cplusplus
#	pragma once
#endif

#	ifndef CONUP_HPP
#	define CONUP_HPP

#include <stdio.h>
#include <stdint.h>

#define CONUP_RESET "\033[0m"
#define CONUP_BOLD "\033[1m"
#define CONUP_DIM "\033[2m"
#define CONUP_UNDERLINE "\033[4m"
#define CONUP_BLINK "\033[5m"
#define CONUP_INVERTED "\033[7m"
#define CONUP_HIDDEN "\033[8m"

#define CONUP_FOREGROUND "38"
#define CONUP_BACKGROUND "48"

typedef struct CONUP_CONSOLE_MODIFIER {
	const char* effect;
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	} foreground, fg;
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	} background, bg;

} conmod_t;

void apply_effect(FILE* out, const char* effect);
void apply_color(FILE* out, const char* type, const char r, const char g, const char b);


#ifdef __cplusplus
#include <string>
#include <iostream>

#include <string>
#include <array>
#include <algorithm>


class conmod {
	const char* effect;
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	} foreground, fg;
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	} background, bg;
public:
	conmod(const char* effect = CONUP_RESET) {
		this->set(245, 245, 245, 3, 3, 3, effect);
	}
	
	conmod(uint8_t fgr, uint8_t fgg, uint8_t fgb, uint8_t bgr = 3, uint8_t bgg = 3, uint8_t bgb = 3, const char* effect = CONUP_RESET) {
		this->set(fgr, fgg, fgb, bgr, bgg, bgb, effect);
	}

	conmod(conmod_t mod) {
		*this = conmod(mod.fg.r, mod.fg.g, mod.fg.b, mod.bg.r, mod.bg.g, mod.bg.b);
		this->effect = mod.effect;
	}


	template<uint8_t r, uint8_t g, uint8_t b>
	struct fg_color {
		static constexpr const char value[] = {
			'\033', '[', '3', '8', ';', '2', ';',
			'0' + (r / 100), '0' + ((r / 10) % 10), '0' + (r % 10), ';',
			'0' + (g / 100), '0' + ((g / 10) % 10), '0' + (g % 10), ';',
			'0' + (b / 100), '0' + ((b / 10) % 10), '0' + (b % 10), 'm', '\0'
		};
	};

	template<uint8_t r, uint8_t g, uint8_t b>
	static constexpr const char* make_fg() {
		return fg_color<r, g, b>::value;
	}

	static void reset() {
		std::cout << CONUP_RESET;
	}

	
	void set_bg(float r, float g, float b) {
		bg.r = (uint8_t)(r * 256);
		bg.g = (uint8_t)(g * 256);
		bg.b = (uint8_t)(b * 256);
	}
	void set_bg(uint8_t r, uint8_t g, uint8_t b) {
		bg.r = r;
		bg.g = g;
		bg.b = b;
	}
	void set_fg(float r, float g, float b) {
		fg.r = (uint8_t)(r * 256);
		fg.g = (uint8_t)(g * 256);
		fg.b = (uint8_t)(b * 256);
	}
	void set_fg(uint8_t r, uint8_t g, uint8_t b) {
		fg.r = r;
		fg.g = g;
		fg.b = b;
	}
	void set(uint8_t fgr, uint8_t fgg, uint8_t fgb, uint8_t bgr, uint8_t bgg, uint8_t bgb, const char* effect) {
		fg.r = fgr;
		fg.g = fgg;
		fg.b = fgb;

		bg.r = bgr;
		bg.g = bgg;
		bg.b = bgb;

		this->effect = effect;
	}

	void set_effect(std::string& effect) {
		this->effect = effect.c_str();
	}

	friend std::ostream& operator<<(std::ostream& os, const conmod& mod);
	friend void apply(std::ostream of, conmod mod);
};
#endif // __cplusplus


inline void apply_color(FILE* out,const char* type, const char r, const char g, const char b)
{
	if (out == NULL) {
		out = stdout;
	}
	if (type == NULL) {
		type = CONUP_FOREGROUND;
	}
	fprintf(out,"\033[%s;2;%d;%d;%dm", type, r, g, b);
}

inline void apply(FILE* out, conmod_t* modi)
{
	if (out == NULL) {
		out = stdout;
	}
	fprintf(out, modi->effect);
	fprintf(out, "\033[%s;2;%d;%d;%d;%s;2;%d;%d;%dm", CONUP_FOREGROUND, modi->fg.r, modi->fg.g, modi->fg.b, CONUP_BACKGROUND, modi->bg.r, modi->bg.g, modi->bg.b);
}

inline void apply_effect(FILE* out, const char* effect) {
	if (out == NULL) {
		out = stdout;
	}
	if (effect == NULL) {
		effect = CONUP_RESET;
	}
	fprintf(out, "%s", effect);
}

inline void reset_color(FILE* out) {
	apply_effect(out, CONUP_RESET);
}

#ifdef __cplusplus
#include <iostream>

inline std::ostream& operator<<(std::ostream& os, const conmod& mod) {
	std::string out = "\033["; 
	out += CONUP_FOREGROUND;
	out += ";2;" + std::to_string(mod.fg.r) + ";" + std::to_string(mod.fg.g) + ";" + std::to_string(mod.fg.b) + ";" + 
		CONUP_BACKGROUND + ";2;" + std::to_string(mod.bg.r) + ";" + std::to_string(mod.bg.g) + ";" + std::to_string(mod.bg.b) + "m";
	os << out;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const conmod_t& mod) {
	std::string out = "\033[";
	out += CONUP_FOREGROUND;
	out += ";2;" + std::to_string(mod.fg.r) + ";" + std::to_string(mod.fg.g) + ";" + std::to_string(mod.fg.b) + ";" +
		CONUP_BACKGROUND + ";2;" + std::to_string(mod.bg.r) + ";" + std::to_string(mod.bg.g) + ";" + std::to_string(mod.bg.b) + "m";
	os << out;
	return os;
}

inline void apply(std::ostream of, const conmod mod) {
	std::string out = "\033[";
	out += CONUP_FOREGROUND;
	out += ";2;" + std::to_string(mod.fg.r) + ";" + std::to_string(mod.fg.g) + ";" + std::to_string(mod.fg.b) + ";" +
		CONUP_BACKGROUND + ";2;" + std::to_string(mod.bg.r) + ";" + std::to_string(mod.bg.g) + ";" + std::to_string(mod.bg.b) + "m";
	of << out;
}
#endif // __cplusplus

#endif // CONUP_HPP