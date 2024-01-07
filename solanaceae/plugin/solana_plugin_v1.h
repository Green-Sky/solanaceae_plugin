#ifndef SOLANA_PLUGIN__H
#define SOLANA_PLUGIN__H

#include <stdint.h>

#define SOLANA_PLUGIN_VERSION 7

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
	void* (*resolveInstance)(const char* id);

	// resolve_all_instances(const char* id)
	void (*provideInstance)(const char* id, const char* plugin_name, void* instance);
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

// for compute tasks
SOLANA_PLUGIN_EXPORT float solana_plugin_tick(float delta);
// on frame rendering
SOLANA_PLUGIN_EXPORT float solana_plugin_render(float delta);

#ifdef __cplusplus
}
#endif

#endif // SOLANA_PLUGIN__H

