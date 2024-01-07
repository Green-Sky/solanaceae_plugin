#include "./plugin_manager.hpp"

#include <limits>

#include <iostream>

// def
std::map<std::string, void*> g_instance_map {};

extern "C" {

void* g_resolveInstance__internal(const char* id) {
	if (auto it = g_instance_map.find(id); it != g_instance_map.end()) {
		return it->second;
	}
	return nullptr;
}

void g_provideInstance__internal(const char* id, const char* plugin_name, void* instance) {
	g_instance_map[id] = instance;
	std::cout << "PLM '" << plugin_name << "' provided '" << id << "'\n";
}


} // extern C

PluginManager::~PluginManager(void) {
	// destruct in reverse!
	for (auto it = _plugins.rbegin(); it != _plugins.rend(); it++) {
		it->stop();
	}
}

bool PluginManager::add(const std::string& plug_path) {
	Plugin p{plug_path.c_str()};

	if (!p) {
		return false;
	}

	if (p.start(&_sapi) != 0) {
		return false;
	}

	_plugins.emplace_back(std::move(p));

	return true;
}

float PluginManager::tick(float delta) {
	float min_interval {std::numeric_limits<float>::max()};

	for (const auto& p : _plugins) {
		const float plug_interval = p.tick(delta);
		if (plug_interval < min_interval) {
			min_interval = plug_interval;
		}
	}

	return min_interval;
}

float PluginManager::render(float delta) {
	float min_interval {std::numeric_limits<float>::max()};

	for (const auto& p : _plugins) {
		const float plug_interval = p.render(delta);
		if (plug_interval < min_interval) {
			min_interval = plug_interval;
		}
	}

	return min_interval;
}

