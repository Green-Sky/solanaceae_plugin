#pragma once

#include <string>
#include <cstdint>

extern "C" {
struct SolanaAPI;
}

struct Plugin {
	bool valid_plugin = false;

	std::string name;
	uint32_t version;

	// void* is platform independent enough, maybe use uint64_t
	void* _dl = nullptr;
	void* _fn_name = nullptr; // TODO: make variable instead of function?
	void* _fn_version = nullptr; // TODO: make variable instead of function?
	void* _fn_start = nullptr;
	void* _fn_stop = nullptr;
	void* _fn_tick = nullptr;
	void* _fn_render = nullptr;

	void* loadSymbol(const char* name);

	// loads lib and gets name (and version)
	Plugin(const char* path);
	Plugin(Plugin&& other);
	Plugin(const Plugin& other) = delete;

	// unloads the plugin
	~Plugin(void);


	// runs the start function
	uint32_t start(SolanaAPI* solana_api) const;

	// stop function
	void stop(void) const;

	// update functions
	float tick(float delta) const;
	float render(float delta) const;

	operator bool(void) const {
		return valid_plugin;
	}
};

