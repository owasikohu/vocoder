// Webview UI for the Vocoder CLAP plugin.
//
// Talks to the C++ plugin over `window.parent.postMessage` using CBOR-encoded
// messages (see vocoder-plugin.h for the C++ side):
//   plugin -> UI : {t:"init", params:[...]}      build the controls
//                  {t:"val",  v:[[id,value],...]} update control values
//   UI -> plugin : "ready"                        request the current state
//                  {t:"set", id, v}               a control moved
//                  {t:"g",   id, d}               gesture begin(true)/end(false)
//
// Kind: 0 = continuous slider, 1 = toggle (checkbox), 2 = stepped int slider.

const KIND_SLIDER = 0, KIND_TOGGLE = 1, KIND_INT = 2;
const GROUP_ORDER = ["OSC1", "OSC2", "NOISE", "VOCODER", ""];

const controls = new Map(); // id -> { setValue(v) }

function send(obj) {
	window.parent.postMessage(CBOR.encode(obj), "*");
}

function formatValue(param, value) {
	if (param.kind === KIND_TOGGLE) return value >= 0.5 ? "on" : "off";
	if (param.kind === KIND_INT) return String(Math.round(value));
	return Number(value).toFixed(2);
}

function buildToggle(param) {
	const wrap = document.createElement("div");
	wrap.className = "param toggle";
	const row = document.createElement("div");
	row.className = "row";
	const label = document.createElement("label");
	label.textContent = param.name;
	const input = document.createElement("input");
	input.type = "checkbox";
	input.checked = param.value >= 0.5;
	input.addEventListener("change", () => {
		const v = input.checked ? 1 : 0;
		send({ t: "g", id: param.id, d: true });
		send({ t: "set", id: param.id, v });
		send({ t: "g", id: param.id, d: false });
	});
	row.appendChild(label);
	row.appendChild(input);
	wrap.appendChild(row);
	controls.set(param.id, { setValue: (v) => { input.checked = v >= 0.5; } });
	return wrap;
}

function buildSlider(param) {
	const wrap = document.createElement("div");
	wrap.className = "param";
	const row = document.createElement("div");
	row.className = "row";
	const label = document.createElement("label");
	label.textContent = param.name;
	const value = document.createElement("span");
	value.className = "value";
	value.textContent = formatValue(param, param.value);
	row.appendChild(label);
	row.appendChild(value);

	const input = document.createElement("input");
	input.type = "range";
	input.min = param.min;
	input.max = param.max;
	input.step = param.kind === KIND_INT ? (param.step || 1) : (param.step ? Math.min(param.step, 0.001) : 0.001);
	input.value = param.value;

	input.addEventListener("pointerdown", () => send({ t: "g", id: param.id, d: true }));
	input.addEventListener("pointerup", () => send({ t: "g", id: param.id, d: false }));
	input.addEventListener("input", () => {
		const v = parseFloat(input.value);
		value.textContent = formatValue(param, v);
		send({ t: "set", id: param.id, v });
	});

	wrap.appendChild(row);
	wrap.appendChild(input);
	controls.set(param.id, {
		setValue: (v) => { input.value = v; value.textContent = formatValue(param, v); }
	});
	return wrap;
}

function buildUI(params) {
	controls.clear();
	const root = document.getElementById("groups");
	root.innerHTML = "";

	const byGroup = new Map();
	for (const p of params) {
		const g = p.group || "";
		if (!byGroup.has(g)) byGroup.set(g, []);
		byGroup.get(g).push(p);
	}

	const groups = [...byGroup.keys()].sort((a, b) => {
		const ia = GROUP_ORDER.indexOf(a), ib = GROUP_ORDER.indexOf(b);
		return (ia < 0 ? 99 : ia) - (ib < 0 ? 99 : ib);
	});

	for (const g of groups) {
		const section = document.createElement("section");
		section.className = "group";
		const h2 = document.createElement("h2");
		h2.textContent = g || "MASTER";
		section.appendChild(h2);
		for (const p of byGroup.get(g)) {
			section.appendChild(p.kind === KIND_TOGGLE ? buildToggle(p) : buildSlider(p));
		}
		root.appendChild(section);
	}
}

window.addEventListener("message", (e) => {
	let msg;
	try { msg = CBOR.decode(e.data); } catch (err) { return; }
	if (!msg || typeof msg !== "object") return;
	if (msg.t === "init") {
		buildUI(msg.params);
	} else if (msg.t === "val" && Array.isArray(msg.v)) {
		for (const [id, value] of msg.v) {
			const c = controls.get(id);
			if (c) c.setValue(value);
		}
	}
});

// Ask the plugin for the current parameter layout & values.
send("ready");
