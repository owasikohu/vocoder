#pragma once
//
// VocoderPlugin: the CLAP plugin wrapper around VocoderEngine.
//
// Structure and the webview/CBOR plumbing follow the MIT/Boost-licensed
// signalsmith-clap-cpp examples (https://github.com/geraintluff/signalsmith-clap-cpp),
// in particular `example-audio-plugin` (webview + params) and `example-synth`
// (note input). The DSP is the Faust-generated vocoder, made polyphonic by
// VocoderEngine.
//
#include "clap/clap.h"

#include "signalsmith-clap/cpp.h"
#include "cbor-walker/cbor-walker.h"
#include "webview-gui/webview-gui.h"

#include "vocoder-engine.h"

#include <array>
#include <atomic>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

struct VocoderPlugin {
	using Plugin = VocoderPlugin;

	static const clap_plugin_descriptor *getPluginDescriptor() {
		static const char *features[] = {
			CLAP_PLUGIN_FEATURE_INSTRUMENT,
			CLAP_PLUGIN_FEATURE_SYNTHESIZER,
			CLAP_PLUGIN_FEATURE_STEREO,
			nullptr
		};
		static clap_plugin_descriptor descriptor{
			.clap_version = CLAP_VERSION_INIT,
			.id = "com.owasikohu.vocoder",
			.name = "Vocoder",
			.vendor = "owasikohu",
			.url = "https://github.com/owasikohu/vocoder",
			.manual_url = nullptr,
			.support_url = nullptr,
			.version = "0.1.0",
			.description = "Faust polyphonic vocoder synth with a webview UI",
			.features = features
		};
		return &descriptor;
	}

	static const clap_plugin *create(const clap_host *host) {
		return &(new VocoderPlugin(host))->clapPlugin;
	}

	const clap_host *host;
	const clap_host_state *hostState = nullptr;
	const clap_host_audio_ports *hostAudioPorts = nullptr;
	const clap_host_note_ports *hostNotePorts = nullptr;
	const clap_host_params *hostParams = nullptr;
	const clap_host_gui *hostGui = nullptr;

	std::unique_ptr<VocoderEngine> engine = std::make_unique<VocoderEngine>();

	// One automatable CLAP parameter per engine "knob".
	struct Param {
		clap_param_info info;
		double value;
		int knobIndex;
		VocoderEngine::Kind kind;
		std::atomic_flag sentValue = ATOMIC_FLAG_INIT;
		std::atomic_flag sentGestureStart = ATOMIC_FLAG_INIT;
		std::atomic_flag sentGestureEnd = ATOMIC_FLAG_INIT;
		std::atomic_flag sentUiState = ATOMIC_FLAG_INIT;
	};
	std::vector<std::unique_ptr<Param>> params;

	VocoderPlugin(const clap_host *host) : host(host) {
		engine->setup();
		for (auto &k : engine->knobs()) {
			auto p = std::make_unique<Param>();
			p->knobIndex = int(params.size());
			p->kind = k.kind;
			p->value = k.init;
			uint32_t flags = CLAP_PARAM_IS_AUTOMATABLE;
			if (k.kind != VocoderEngine::Kind::slider) flags |= CLAP_PARAM_IS_STEPPED;
			p->info = {
				.id = k.id,
				.flags = flags,
				.cookie = p.get(),
				.name = {},
				.module = {},
				.min_value = k.min,
				.max_value = k.max,
				.default_value = k.init
			};
			std::strncpy(p->info.name, k.name.c_str(), CLAP_NAME_SIZE - 1);
			std::strncpy(p->info.module, k.group.c_str(), CLAP_PATH_SIZE - 1);
			params.push_back(std::move(p));
		}
	}

	template<auto methodPtr>
	auto clapPluginMethod() -> decltype(signalsmith::clap::pluginMethod<methodPtr>()) {
		return signalsmith::clap::pluginMethod<methodPtr>();
	}

	const clap_plugin clapPlugin{
		.desc = getPluginDescriptor(),
		.plugin_data = this,
		.init = clapPluginMethod<&Plugin::pluginInit>(),
		.destroy = clapPluginMethod<&Plugin::pluginDestroy>(),
		.activate = clapPluginMethod<&Plugin::pluginActivate>(),
		.deactivate = clapPluginMethod<&Plugin::pluginDeactivate>(),
		.start_processing = clapPluginMethod<&Plugin::pluginStartProcessing>(),
		.stop_processing = clapPluginMethod<&Plugin::pluginStopProcessing>(),
		.reset = clapPluginMethod<&Plugin::pluginReset>(),
		.process = clapPluginMethod<&Plugin::pluginProcess>(),
		.get_extension = clapPluginMethod<&Plugin::pluginGetExtension>(),
		.on_main_thread = clapPluginMethod<&Plugin::pluginOnMainThread>()
	};

	bool pluginInit() {
		using namespace signalsmith::clap;
		getHostExtension(host, CLAP_EXT_STATE, hostState);
		getHostExtension(host, CLAP_EXT_AUDIO_PORTS, hostAudioPorts);
		getHostExtension(host, CLAP_EXT_NOTE_PORTS, hostNotePorts);
		getHostExtension(host, CLAP_EXT_PARAMS, hostParams);
		getHostExtension(host, CLAP_EXT_GUI, hostGui);
		return true;
	}
	void pluginDestroy() { delete this; }

	bool pluginActivate(double sampleRate, uint32_t /*minFrames*/, uint32_t maxFrames) {
		engine->prepare(maxFrames);
		engine->init(sampleRate);
		return true;
	}
	void pluginDeactivate() {}
	bool pluginStartProcessing() { return true; }
	void pluginStopProcessing() {}
	void pluginReset() { engine->reset(); }

	// ---- events ----------------------------------------------------------

	void processParamEvent(const clap_event_param_value *eventParam) {
		Param *param = nullptr;
		if (eventParam->cookie) {
			param = (Param *)eventParam->cookie;
		} else {
			for (auto &p : params) if (p->info.id == eventParam->param_id) { param = p.get(); break; }
		}
		if (!param) return;
		param->value = eventParam->value;
		param->sentUiState.clear();
		engine->setKnob(param->knobIndex, param->value);

		stateDirty = true;
		sentWebviewState.clear();
		host->request_callback(host);
	}

	void processEvent(const clap_event_header *event) {
		if (event->space_id != CLAP_CORE_EVENT_SPACE_ID) return;
		switch (event->type) {
			case CLAP_EVENT_PARAM_VALUE:
				processParamEvent((const clap_event_param_value *)event);
				break;
			case CLAP_EVENT_NOTE_ON: {
				auto *e = (const clap_event_note *)event;
				engine->noteOn(e->channel, e->key, e->velocity);
				break;
			}
			case CLAP_EVENT_NOTE_OFF:
			case CLAP_EVENT_NOTE_CHOKE: {
				auto *e = (const clap_event_note *)event;
				engine->noteOff(e->channel, e->key);
				break;
			}
			case CLAP_EVENT_MIDI: {
				auto *e = (const clap_event_midi *)event;
				uint8_t status = e->data[0] & 0xF0;
				uint8_t channel = e->data[0] & 0x0F;
				if (status == 0x90 && e->data[2] > 0) {
					engine->noteOn(channel, e->data[1], e->data[2] / 127.0);
				} else if (status == 0x80 || (status == 0x90 && e->data[2] == 0)) {
					engine->noteOff(channel, e->data[1]);
				} else if (status == 0xE0) {
					int value = (e->data[1] | (e->data[2] << 7)) - 8192;
					engine->pitchWheel(channel, value / 8192.0);
				}
				break;
			}
			default:
				break;
		}
	}

	clap_process_status pluginProcess(const clap_process *process) {
		auto *eventsIn = process->in_events;
		auto *eventsOut = process->out_events;
		uint32_t eventCount = eventsIn->size(eventsIn);
		// We do not split the block on event times (no sample-accurate automation),
		// which keeps the integration simple. Events are applied up-front.
		for (uint32_t i = 0; i < eventCount; ++i) {
			auto *event = eventsIn->get(eventsIn, i);
			processEvent(event);
			eventsOut->try_push(eventsOut, event);
		}

		const float *mic = nullptr;
		if (process->audio_inputs_count > 0 && process->audio_inputs[0].channel_count > 0) {
			mic = process->audio_inputs[0].data32[0];
		}
		float *outL = process->audio_outputs[0].data32[0];
		float *outR = (process->audio_outputs[0].channel_count > 1)
			? process->audio_outputs[0].data32[1] : outL;
		engine->process(process->frames_count, mic, outL, outR);

		for (auto &param : params) sendParamEvents(*param, eventsOut);
		return CLAP_PROCESS_CONTINUE;
	}

	void sendParamEvents(Param &param, const clap_output_events *outEvents) {
		if (!param.sentGestureStart.test_and_set()) {
			clap_event_param_gesture event{
				.header = {sizeof(event), 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_PARAM_GESTURE_BEGIN, CLAP_EVENT_IS_LIVE},
				.param_id = param.info.id
			};
			outEvents->try_push(outEvents, &event.header);
		}
		if (!param.sentValue.test_and_set()) {
			clap_event_param_value event{
				.header = {sizeof(event), 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_PARAM_VALUE, CLAP_EVENT_IS_LIVE},
				.param_id = param.info.id, .cookie = &param,
				.note_id = -1, .port_index = -1, .channel = -1, .key = -1,
				.value = param.value
			};
			outEvents->try_push(outEvents, &event.header);
		}
		if (!param.sentGestureEnd.test_and_set()) {
			clap_event_param_gesture event{
				.header = {sizeof(event), 0, CLAP_CORE_EVENT_SPACE_ID, CLAP_EVENT_PARAM_GESTURE_END, CLAP_EVENT_IS_LIVE},
				.param_id = param.info.id
			};
			outEvents->try_push(outEvents, &event.header);
		}
	}

	bool stateDirty = false;
	void pluginOnMainThread() {
		if (stateDirty && hostState) {
			hostState->mark_dirty(host);
			stateDirty = false;
		}
		webviewSendIfNeeded();
	}

	const void *pluginGetExtension(const char *extId) {
		if (!std::strcmp(extId, CLAP_EXT_STATE)) {
			static const clap_plugin_state ext{
				.save = clapPluginMethod<&Plugin::stateSave>(),
				.load = clapPluginMethod<&Plugin::stateLoad>(),
			};
			return &ext;
		} else if (!std::strcmp(extId, CLAP_EXT_AUDIO_PORTS)) {
			static const clap_plugin_audio_ports ext{
				.count = clapPluginMethod<&Plugin::audioPortsCount>(),
				.get = clapPluginMethod<&Plugin::audioPortsGet>(),
			};
			return &ext;
		} else if (!std::strcmp(extId, CLAP_EXT_NOTE_PORTS)) {
			static const clap_plugin_note_ports ext{
				.count = clapPluginMethod<&Plugin::notePortsCount>(),
				.get = clapPluginMethod<&Plugin::notePortsGet>(),
			};
			return &ext;
		} else if (!std::strcmp(extId, CLAP_EXT_PARAMS)) {
			static const clap_plugin_params ext{
				.count = clapPluginMethod<&Plugin::paramsCount>(),
				.get_info = clapPluginMethod<&Plugin::paramsGetInfo>(),
				.get_value = clapPluginMethod<&Plugin::paramsGetValue>(),
				.value_to_text = clapPluginMethod<&Plugin::paramsValueToText>(),
				.text_to_value = clapPluginMethod<&Plugin::paramsTextToValue>(),
				.flush = clapPluginMethod<&Plugin::paramsFlush>(),
			};
			return &ext;
		} else if (!std::strcmp(extId, CLAP_EXT_GUI)) {
			static const clap_plugin_gui ext{
				.is_api_supported = clapPluginMethod<&Plugin::guiIsApiSupported>(),
				.get_preferred_api = clapPluginMethod<&Plugin::guiGetPreferredApi>(),
				.create = clapPluginMethod<&Plugin::guiCreate>(),
				.destroy = clapPluginMethod<&Plugin::guiDestroy>(),
				.set_scale = clapPluginMethod<&Plugin::guiSetScale>(),
				.get_size = clapPluginMethod<&Plugin::guiGetSize>(),
				.can_resize = clapPluginMethod<&Plugin::guiCanResize>(),
				.get_resize_hints = clapPluginMethod<&Plugin::guiGetResizeHints>(),
				.adjust_size = clapPluginMethod<&Plugin::guiAdjustSize>(),
				.set_size = clapPluginMethod<&Plugin::guiSetSize>(),
				.set_parent = clapPluginMethod<&Plugin::guiSetParent>(),
				.set_transient = clapPluginMethod<&Plugin::guiSetTransient>(),
				.suggest_title = clapPluginMethod<&Plugin::guiSuggestTitle>(),
				.show = clapPluginMethod<&Plugin::guiShow>(),
				.hide = clapPluginMethod<&Plugin::guiHide>(),
			};
			return &ext;
		}
		return nullptr;
	}

	// ---- state (CBOR map of {paramId: value}) ----------------------------

	bool stateSave(const clap_ostream_t *stream) {
		std::vector<unsigned char> bytes;
		signalsmith::cbor::CborWriter cbor{bytes};
		cbor.openMap(params.size());
		for (auto &param : params) {
			cbor.addInt(param->info.id);
			cbor.addFloat(param->value);
		}
		return signalsmith::clap::writeAllToStream(bytes, stream);
	}
	bool stateLoad(const clap_istream_t *stream) {
		std::vector<unsigned char> bytes;
		if (!signalsmith::clap::readAllFromStream(bytes, stream) || bytes.empty()) return false;
		using Cbor = signalsmith::cbor::CborWalker;
		Cbor cbor{bytes};
		if (!cbor.isMap()) return false;
		cbor.forEachPair([&](Cbor key, Cbor value) {
			uint32_t id = uint32_t(int64_t(key));
			for (auto &param : params) {
				if (param->info.id == id) {
					param->value = double(value);
					engine->setKnob(param->knobIndex, param->value);
					param->sentUiState.clear();
				}
			}
		});
		sentWebviewState.clear();
		return true;
	}

	// ---- audio ports (mono mic in, stereo out) ---------------------------

	uint32_t audioPortsCount(bool /*isInput*/) { return 1; }
	bool audioPortsGet(uint32_t index, bool isInput, clap_audio_port_info *info) {
		if (index >= 1) return false;
		if (isInput) {
			*info = {
				.id = 0x1,
				.name = {'m', 'i', 'c'},
				.flags = CLAP_AUDIO_PORT_IS_MAIN,
				.channel_count = 1,
				.port_type = CLAP_PORT_MONO,
				.in_place_pair = CLAP_INVALID_ID
			};
		} else {
			*info = {
				.id = 0x2,
				.name = {'o', 'u', 't'},
				.flags = CLAP_AUDIO_PORT_IS_MAIN,
				.channel_count = 2,
				.port_type = CLAP_PORT_STEREO,
				.in_place_pair = CLAP_INVALID_ID
			};
		}
		return true;
	}

	// ---- note ports ------------------------------------------------------

	uint32_t notePortsCount(bool isInput) { return isInput ? 1 : 0; }
	bool notePortsGet(uint32_t index, bool isInput, clap_note_port_info *info) {
		if (!isInput || index >= 1) return false;
		*info = {
			.id = 0x10,
			.supported_dialects = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI | CLAP_NOTE_DIALECT_MIDI_MPE,
			.preferred_dialect = CLAP_NOTE_DIALECT_CLAP,
			.name = {'n', 'o', 't', 'e', 's'}
		};
		return true;
	}

	// ---- parameters ------------------------------------------------------

	uint32_t paramsCount() { return uint32_t(params.size()); }
	bool paramsGetInfo(uint32_t index, clap_param_info *info) {
		if (index >= params.size()) return false;
		*info = params[index]->info;
		return true;
	}
	bool paramsGetValue(clap_id paramId, double *value) {
		for (auto &param : params) if (param->info.id == paramId) { *value = param->value; return true; }
		return false;
	}
	bool paramsValueToText(clap_id paramId, double value, char *text, uint32_t textCapacity) {
		for (auto &param : params) {
			if (param->info.id == paramId) {
				if (param->kind == VocoderEngine::Kind::toggle) {
					std::strncpy(text, value >= 0.5 ? "on" : "off", textCapacity);
				} else if (param->kind == VocoderEngine::Kind::intSlider) {
					std::snprintf(text, textCapacity, "%d", int(std::lround(value)));
				} else {
					std::snprintf(text, textCapacity, "%.2f", value);
				}
				return true;
			}
		}
		return false;
	}
	bool paramsTextToValue(clap_id, const char *, double *) { return false; }
	void paramsFlush(const clap_input_events *eventsIn, const clap_output_events *eventsOut) {
		uint32_t eventCount = eventsIn->size(eventsIn);
		for (uint32_t i = 0; i < eventCount; ++i) {
			auto *event = eventsIn->get(eventsIn, i);
			processEvent(event);
			eventsOut->try_push(eventsOut, event);
		}
		for (auto &param : params) sendParamEvents(*param, eventsOut);
	}

	// ---- GUI (webview) ---------------------------------------------------

	using WebviewGui = webview_gui::WebviewGui;
	std::unique_ptr<WebviewGui> webview;
	std::atomic_flag sentWebviewState = ATOMIC_FLAG_INIT;
	std::atomic_flag sentWebviewInit = ATOMIC_FLAG_INIT;

	static WebviewGui::Platform clapApiToPlatform(const char *api) {
		if (!std::strcmp(api, CLAP_WINDOW_API_WIN32)) return WebviewGui::HWND;
		if (!std::strcmp(api, CLAP_WINDOW_API_COCOA)) return WebviewGui::COCOA;
		if (!std::strcmp(api, CLAP_WINDOW_API_X11)) return WebviewGui::X11EMBED;
		return WebviewGui::NONE;
	}
	bool guiIsApiSupported(const char *api, bool isFloating) {
		if (isFloating) return false;
		return WebviewGui::supports(clapApiToPlatform(api));
	}
	bool guiGetPreferredApi(const char **api, bool *isFloating) {
		*isFloating = false;
		*api = nullptr;
		if (WebviewGui::supports(WebviewGui::HWND)) *api = CLAP_WINDOW_API_WIN32;
		if (WebviewGui::supports(WebviewGui::COCOA)) *api = CLAP_WINDOW_API_COCOA;
		if (WebviewGui::supports(WebviewGui::X11EMBED)) *api = CLAP_WINDOW_API_X11;
		return *api != nullptr;
	}
	bool guiCreate(const char *api, bool isFloating) {
		if (isFloating) return false;
		if (webview) return true;
		webview = WebviewGui::createUnique(clapApiToPlatform(api), "/", [this](const char *path, WebviewGui::Resource &resource) {
			return webviewGetResource(path, resource);
		});
		if (webview) {
			uint32_t w, h;
			guiGetSize(&w, &h);
			webview->setSize(w, h);
			webview->receive = [this](const unsigned char *bytes, size_t length) {
				webviewReceive(bytes, length);
			};
		}
		return bool(webview);
	}
	void guiDestroy() {
		webview = nullptr;
		sentWebviewInit.clear();
	}
	bool guiSetScale(double) { return true; }
	bool guiGetSize(uint32_t *width, uint32_t *height) { *width = 720; *height = 540; return true; }
	bool guiCanResize() { return false; }
	bool guiGetResizeHints(clap_gui_resize_hints *) { return false; }
	bool guiAdjustSize(uint32_t *width, uint32_t *height) { return guiGetSize(width, height); }
	bool guiSetSize(uint32_t, uint32_t) { return false; }
	bool guiSetParent(const clap_window *window) {
		if (webview) { webview->attach(window->ptr); return true; }
		return false;
	}
	bool guiSetTransient(const clap_window *) { return false; }
	void guiSuggestTitle(const char *) {}
	bool guiShow() { return true; }
	bool guiHide() { return true; }

	bool webviewGetResource(const char *path, WebviewGui::Resource &resource);

	void webviewReceive(const unsigned char *bytes, size_t length) {
		using Cbor = signalsmith::cbor::CborWalker;
		Cbor cbor{bytes, length};

		if (cbor.utf8View() == "ready") {
			sentWebviewInit.clear();
			for (auto &param : params) param->sentUiState.clear();
			sentWebviewState.clear();
			webviewSendIfNeeded();
			return;
		}

		uint32_t id = 0;
		double value = 0;
		bool isSet = false, isGesture = false, gestureDown = false;
		cbor.forEachPair([&](Cbor key, Cbor val) {
			auto k = key.utf8View();
			if (k == "t") {
				auto t = val.utf8View();
				if (t == "set") isSet = true;
				else if (t == "g") isGesture = true;
			} else if (k == "id") {
				id = uint32_t(int64_t(val));
			} else if (k == "v") {
				value = double(val);
			} else if (k == "d") {
				gestureDown = bool(val);
			}
		});

		for (auto &param : params) {
			if (param->info.id != id) continue;
			if (isSet) {
				param->value = value;
				engine->setKnob(param->knobIndex, value);
				param->sentValue.clear();
			} else if (isGesture) {
				if (gestureDown) param->sentGestureStart.clear();
				else param->sentGestureEnd.clear();
			}
			break;
		}
		if (hostParams) hostParams->request_flush(host);
	}

	void sendInit() {
		std::vector<unsigned char> bytes;
		signalsmith::cbor::CborWriter cbor{bytes};
		cbor.openMap(2);
		cbor.addUtf8("t");
		cbor.addUtf8("init");
		cbor.addUtf8("params");
		auto &knobs = engine->knobs();
		cbor.openArray(knobs.size());
		for (size_t i = 0; i < knobs.size(); ++i) {
			auto &k = knobs[i];
			cbor.openMap(8);
			cbor.addUtf8("id");    cbor.addInt(k.id);
			cbor.addUtf8("name");  cbor.addUtf8(k.name);
			cbor.addUtf8("group"); cbor.addUtf8(k.group);
			cbor.addUtf8("min");   cbor.addFloat(k.min);
			cbor.addUtf8("max");   cbor.addFloat(k.max);
			cbor.addUtf8("step");  cbor.addFloat(k.step);
			cbor.addUtf8("kind");  cbor.addInt(int(k.kind));
			cbor.addUtf8("value"); cbor.addFloat(params[i]->value);
		}
		cbor.close();
		cbor.close();
		webview->send(bytes.data(), bytes.size());
	}

	void webviewSendIfNeeded() {
		if (!webview) return;
		if (!sentWebviewInit.test_and_set()) sendInit();
		if (sentWebviewState.test_and_set()) return;

		std::vector<unsigned char> bytes;
		signalsmith::cbor::CborWriter cbor{bytes};
		cbor.openMap(2);
		cbor.addUtf8("t");
		cbor.addUtf8("val");
		cbor.addUtf8("v");
		cbor.openArray();
		for (auto &param : params) {
			if (param->sentUiState.test_and_set()) continue;
			cbor.openArray(2);
			cbor.addInt(param->info.id);
			cbor.addFloat(param->value);
		}
		cbor.close();
		cbor.close();
		webview->send(bytes.data(), bytes.size());
	}
};
