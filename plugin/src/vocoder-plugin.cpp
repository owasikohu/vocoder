#include "vocoder-plugin.h"

#include <cstring>

// Webview resources, embedded as byte arrays at build time (see CMake
// `embed_resource`). Variable names match the file names with '.' -> '_'.
namespace vocoder_resources {
#include "ui/index.html.hxx"
#include "ui/app.js.hxx"
#include "ui/style.css.hxx"
#include "ui/cbor.min.js.hxx"
}

bool VocoderPlugin::webviewGetResource(const char *path, WebviewGui::Resource &resource) {
	using namespace vocoder_resources;
	auto serve = [&](const unsigned char *data, size_t len, const char *type) {
		resource.mediaType = type;
		resource.bytes.assign(data, data + len);
		return true;
	};
	if (!std::strcmp(path, "/") || !std::strcmp(path, "/index.html"))
		return serve(index_html, sizeof(index_html), "text/html");
	if (!std::strcmp(path, "/app.js"))
		return serve(app_js, sizeof(app_js), "application/javascript");
	if (!std::strcmp(path, "/style.css"))
		return serve(style_css, sizeof(style_css), "text/css");
	if (!std::strcmp(path, "/cbor.min.js"))
		return serve(cbor_min_js, sizeof(cbor_min_js), "application/javascript");
	return false;
}
