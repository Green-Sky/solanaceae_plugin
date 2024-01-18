#ifndef SOLANA_PLUGIN__H
#define SOLANA_PLUGIN__H

#include <stdint.h>

#define SOLANA_PLUGIN_VERSION 9

#if defined(_MSC_VER) || defined(__MINGW32__)
	#define SOLANA_PLUGIN_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) // also clang
	#define SOLANA_PLUGIN_EXPORT __attribute__((visibility("default")))
#else
	#error unsupported platform
#endif

#if defined(SOLANA_PLUGIN_HOST)
	#define SOLANA_PLUGIN_DECL
	// TODO: this looks like decl should be used, but it isnt
#else
	#define SOLANA_PLUGIN_DECL SOLANA_PLUGIN_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

// public plugin stuff here

struct SolanaAPI {
	void* (*resolveInstance)(const char* id, const char* version);
	// resolve_all_instances(const char* id)
	void (*provideInstance)(const char* id, const char* version, const char* plugin_name, void* instance);
};

// ---------- info ----------

// TODO: change to exported struct, so we dont have to execute this code
SOLANA_PLUGIN_EXPORT const char* solana_plugin_get_name(void);
// TODO: add author and other attributes

// get the SOLANA_PLUGIN_VERSION the plugin was compiled with
SOLANA_PLUGIN_EXPORT uint32_t solana_plugin_get_version(void);

// ---------- plugin control ----------

// return 0 on success
SOLANA_PLUGIN_EXPORT uint32_t solana_plugin_start(struct SolanaAPI* solana_api);

SOLANA_PLUGIN_EXPORT void solana_plugin_stop(void);

// ---------- called periodically ----------
// rendering needs to be called in a different interval AND needs to be garantied from mainthread
// the functions return the minimum time in seconds until the update should be called next
// the interval is not garantied. in fact, clients can call them with a much longer interval

// for compute tasks
SOLANA_PLUGIN_EXPORT float solana_plugin_tick(float delta);
// on frame rendering
SOLANA_PLUGIN_EXPORT float solana_plugin_render(float delta);

#ifdef __cplusplus
}

// in c++ we can provide generic helpers
#include "./version_helper.inl"

#include <string>

// templated helper, use or make sure vtable is right

// versioned means explicitly provided

// ---------- provide ----------
template<typename T>
static void plug_provideInstance(const SolanaAPI* solana_api, const char* id, const char* version, const char* plugin_name, T* instance) {
	solana_api->provideInstance(id, version, plugin_name, instance);
}

template<typename T>
static void plug_provideInstance(const SolanaAPI* solana_api, const char* id, const char* plugin_name, T* instance) {
	plug_provideInstance<T>(solana_api, id, internal::g_type_version<T>::version, plugin_name, instance);
}

#define PLUG_PROVIDE_INSTANCE_VERSIONED(x, ver, p, i) plug_provideInstance(solana_api, #x, ver, p, i)
#define PLUG_PROVIDE_INSTANCE(x, p, i) plug_provideInstance(solana_api, #x, p, i)

// ---------- resolve optional ----------

template<typename T>
static T* plug_resolveInstanceOptional(const SolanaAPI* solana_api, const char* id, const char* version) {
	return static_cast<T*>(solana_api->resolveInstance(id, version));
}

template<typename T>
static T* plug_resolveInstanceOptional(const SolanaAPI* solana_api, const char* id) {
	return plug_resolveInstanceOptional<T>(solana_api, id, internal::g_type_version<T>::version);
}

// ---------- resolve require ----------
// throws if id+version is not found

struct ResolveException {
	std::string what;
};

template<typename T>
static T* plug_resolveInstance(const SolanaAPI* solana_api, const char* id, const char* version) {
	T* res = plug_resolveInstanceOptional<T>(solana_api, id, version);
	if (res == nullptr) {
		throw ResolveException{"missing " + std::string{id} + " " + version};
	}
	return res;
}
template<typename T>
static T* plug_resolveInstance(const SolanaAPI* solana_api, const char* id) {
	return plug_resolveInstance<T>(solana_api, id, internal::g_type_version<T>::version);
}

#define PLUG_RESOLVE_INSTANCE_VERSIONED(x, ver) plug_resolveInstance<x>(solana_api, #x, ver)
#define PLUG_RESOLVE_INSTANCE(x) plug_resolveInstance<x>(solana_api, #x)

// optional variants dont error
#define PLUG_RESOLVE_INSTANCE_VERSIONED_OPT(x, ver) plug_resolveInstanceOptional<x>(solana_api, #x, ver)
#define PLUG_RESOLVE_INSTANCE_OPT(x) plug_resolveInstanceOptional<x>(solana_api, #x)

#endif

#endif // SOLANA_PLUGIN__H

