#pragma once

#define SOLANA_PLUGIN_HOST
#include <solanaceae/plugin/solana_plugin_v1.h>

#include <solanaceae/plugin/plugin.hpp>

#include <vector>
#include <map>
#include <string>

// TODO: save plug name to instance
// id -> version -> ptr
extern std::map<std::string, std::map<std::string, void*>> g_instance_map;

extern "C" {

void* g_resolveInstance__internal(const char* id, const char* version);

void g_provideInstance__internal(const char* id, const char* version, const char* plugin_name, void* instance);

} // extern C

#include "./version_helper.inl"

// templated helper, use or make sure vtable is right
template<typename T>
void g_provideInstance(const char* id, const char* version, const char* plugin_name, T* instance) {
	g_provideInstance__internal(id, version, plugin_name, instance);
}

template<typename T>
void g_provideInstance(const char* id, const char* plugin_name, T* instance) {
	g_provideInstance__internal(id, internal::g_type_version<T>::version, plugin_name, instance);
}

// only on host!
struct PluginManager {
	SolanaAPI _sapi {
		&g_resolveInstance__internal,
		&g_provideInstance__internal,
	};

	std::vector<Plugin> _plugins;

	~PluginManager(void);

	bool add(const std::string& plug_path);

	// returns the minimum time until next call is wanted in seconds
	float tick(float delta);
	float render(float delta);
};

