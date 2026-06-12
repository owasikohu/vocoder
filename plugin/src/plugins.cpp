#include "plugins.h"

#include "clap/clap.h"
#include "vocoder-plugin.h"

#include <cstring>

std::string clapBundleResourceDir;

// ---- Plugin factory ----

static uint32_t pluginFactoryGetPluginCount(const struct clap_plugin_factory *) {
	return 1;
}
static const clap_plugin_descriptor_t *pluginFactoryGetPluginDescriptor(const struct clap_plugin_factory *, uint32_t index) {
	if (index == 0) return VocoderPlugin::getPluginDescriptor();
	return nullptr;
}
static const clap_plugin_t *pluginFactoryCreatePlugin(const struct clap_plugin_factory *, const clap_host_t *host, const char *pluginId) {
	if (!std::strcmp(pluginId, VocoderPlugin::getPluginDescriptor()->id)) {
		return VocoderPlugin::create(host);
	}
	return nullptr;
}

// ---- Bundle entry points (called from clap_entry.cpp) ----

bool clapEntryInit(const char *path) {
	clapBundleResourceDir = path ? path : "";
#if defined(__APPLE__)
	clapBundleResourceDir += "/Contents/Resources";
#endif
	return true;
}
void clapEntryDeinit() {
	clapBundleResourceDir = "";
}
const void *clapEntryGetFactory(const char *factoryId) {
	if (!std::strcmp(factoryId, CLAP_PLUGIN_FACTORY_ID)) {
		static const clap_plugin_factory clapPluginFactory{
			.get_plugin_count = pluginFactoryGetPluginCount,
			.get_plugin_descriptor = pluginFactoryGetPluginDescriptor,
			.create_plugin = pluginFactoryCreatePlugin
		};
		return &clapPluginFactory;
	}
	return nullptr;
}
