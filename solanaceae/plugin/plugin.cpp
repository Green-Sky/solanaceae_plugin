#include "./plugin.hpp"

// https://youtu.be/RsHGUL5E1_s

#define SOLANA_PLUGIN_HOST
#include "./solana_plugin_v1.h"

#if (defined(_WIN32) || defined(_WIN64))
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <dlfcn.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
	#define WINDOWS
#elif defined(__APPLE__)
	#define APPLE
#else
	#define LINUX
#endif

#include <iostream>
#include <cassert>

void* Plugin::loadSymbol(const char* s_name) {
#if defined(_WIN32) || defined(_WIN64)
	return GetProcAddress(_dl, s_name);
#else
	return dlsym(_dl, s_name);
#endif
}

// loads lib and gets name (and version)
Plugin::Plugin(const char* path) {
#if defined(_WIN32) || defined(_WIN64)
	_dl = (void*)LoadLibraryA(path);
#else
	_dl = (void*)dlopen(path, RTLD_NOW /*| RTLD_LOCAL*/);
#endif

	if (!_dl) {
		std::cerr << "PLG opening '" << path << "' failed\n";
		return;
	}

	_fn_name = loadSymbol("solana_plugin_get_name");
	_fn_version = loadSymbol("solana_plugin_get_version");
	_fn_start = loadSymbol("solana_plugin_start");
	_fn_stop = loadSymbol("solana_plugin_stop");
	_fn_tick = loadSymbol("solana_plugin_tick");

	if (!_fn_name || !_fn_version || !_fn_start || !_fn_stop || !_fn_tick) {
		std::cerr << "PLG '" << path << "' misses functions\n";
		return;
	}

	name = reinterpret_cast<decltype(&solana_plugin_get_name)>(_fn_name)();

	if (name.empty()) {
		std::cerr << "PLG '" << path << "' misses name\n";
		return;
	}

	version = reinterpret_cast<decltype(&solana_plugin_get_version)>(_fn_version)();

	if (version != SOLANA_PLUGIN_VERSION) {
		std::cerr << "PLG '" << path << "' version mismatch IS:" << version << " SHOULD:" << SOLANA_PLUGIN_VERSION << "\n";
		return;
	}

	valid_plugin = true;
}

Plugin::Plugin(Plugin&& other) {
	valid_plugin = other.valid_plugin;
	name = other.name;

	_dl = other._dl;
	other._dl = nullptr;

	_fn_name = other._fn_name;
	other._fn_name = nullptr;

	_fn_start = other._fn_start;
	other._fn_start = nullptr;

	_fn_stop = other._fn_stop;
	other._fn_stop = nullptr;

	_fn_tick = other._fn_tick;
	other._fn_tick = nullptr;
}

// unloads the plugin
Plugin::~Plugin(void) {
	if (_dl != nullptr) {
#if defined(_WIN32) || defined(_WIN64)
		FreeLibrary(_dl);
#else
		dlclose(_dl);
#endif
	}
}

// runs the start function
uint32_t Plugin::start(SolanaAPI* solana_api) const {
	assert(valid_plugin);
	return reinterpret_cast<decltype(&solana_plugin_start)>(_fn_start)(solana_api);
}

// stop function
void Plugin::stop(void) const {
	assert(valid_plugin);
	reinterpret_cast<decltype(&solana_plugin_stop)>(_fn_stop)();
}

// tick function
void Plugin::tick(float delta) const {
	assert(valid_plugin);
	reinterpret_cast<decltype(&solana_plugin_tick)>(_fn_tick)(delta);
}

