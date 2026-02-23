// static/js/script.js
"use strict";

let lastFormattedJson = "";

document.addEventListener("DOMContentLoaded", () => {

    document
        .getElementById("formatBtn")
        .addEventListener("click", onFormatClicked);

    document
        .getElementById("uploadBtn")
        .addEventListener("click", uploadJson);

    document
        .getElementById("downloadBtn")
        .addEventListener("click", downloadJson);

        document
        .getElementById("fileInput")
        .addEventListener("change", onFileSelected);

});

async function onFormatClicked() {
    const input = document.getElementById("input").value.trim();
    if (!input) { showError("Please enter JSON"); return; }

    try {
        const response = await fetch("/api/parse", {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: input
        });

        const result = await response.json();
        if (result.status === "success") {
            lastFormattedJson = JSON.stringify(result.data, null, 4);
            renderJsonViewer(lastFormattedJson);
        } else {
            showError(result.message);
        }
    } catch (e) {
        showError(e.message || String(e));
    }
}

function showError(msg) {
    const container = document.getElementById("result");
    container.innerHTML = `<div style="color:#f48771;padding:8px">${escapeHtml(msg)}</div>`;
}

function escapeHtml(text) {
    return String(text)
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;");
}

/* ==========================
   JSON Viewer + folding
   ========================== */
function renderJsonViewer(jsonString) {
    const container = document.getElementById("result");
    container.innerHTML = "";

    // split lines and build offsets to map char-pos -> line
    const lines = jsonString.split("\n");
    const lineOffsets = new Array(lines.length);
    let offset = 0;
    for (let i = 0; i < lines.length; ++i) {
        lineOffsets[i] = offset;
        // +1 for the newline that was removed by split
        offset += lines[i].length + 1;
    }

    // helper: map absolute char index to line index
    function charIndexToLine(idx) {
        // binary search for performance on big files
        let lo = 0, hi = lineOffsets.length - 1;
        while (lo <= hi) {
            const mid = (lo + hi) >> 1;
            const start = lineOffsets[mid];
            const end = (mid + 1 < lineOffsets.length) ? lineOffsets[mid + 1] : offset;
            if (idx < start) { hi = mid - 1; }
            else if (idx >= end) { lo = mid + 1; }
            else return mid;
        }
        return lineOffsets.length - 1;
    }

    // build DOM rows first (so indexes stable)
    for (let i = 0; i < lines.length; ++i) {
        const row = document.createElement("div");
        row.className = "json-line";

        const lineNumber = document.createElement("span");
        lineNumber.className = "line-number";
        lineNumber.textContent = i + 1;

        const fold = document.createElement("span");
        fold.className = "fold-toggle";
        // default empty; set symbol later if block start
        fold.textContent = "";

        const content = document.createElement("span");
        content.className = "line-content";
        content.innerHTML = syntaxHighlight(lines[i]);

        row.appendChild(lineNumber);
        row.appendChild(fold);
        row.appendChild(content);

        container.appendChild(row);
    }

    // After rows exist, attach fold behavior by scanning characters
    const whole = jsonString; // full text to scan char-wise
    const containerChildren = container.children;

    for (let i = 0; i < lines.length; ++i) {
        const line = lines[i];
        const trimmed = line.trim();

        // find first brace in this line (only consider { or [ as block start)
        const openIdxInLine = (() => {
            const idx1 = line.indexOf("{");
            const idx2 = line.indexOf("[");
            if (idx1 === -1) return (idx2 === -1 ? -1 : idx2);
            if (idx2 === -1) return idx1;
            return Math.min(idx1, idx2);
        })();

        if (openIdxInLine === -1) continue; // nothing to fold on this line

        // character absolute index in whole string
        const startCharIndex = lineOffsets[i] + openIdxInLine;
        const openChar = whole[startCharIndex];
        const closeChar = openChar === "{" ? "}" : "]";

        // We should ensure this open is really an opening brace that starts a block (not inside string)
        // A robust check: count quotes up to this position to see if we are inside a string.
        // If inside a string, skip folding here.
        if (isInsideString(whole, startCharIndex)) continue;

        // mark fold symbol
        const foldElem = containerChildren[i].querySelector(".fold-toggle");
        foldElem.textContent = "▶";

        // attach handler that finds matching closing char by scanning with stack
        foldElem.onclick = (() => {
            // capture startCharIndex and i
            const start = startCharIndex;
            const startLine = i;
            return () => {
                const collapsed = foldElem.textContent === "▶";
                foldElem.textContent = collapsed ? "▼" : "▶";

                // stack scan
                let depth = 0;
                let j = start + 1;
                const n = whole.length;
                for (; j < n; ++j) {
                    const ch = whole[j];
                    // skip characters inside strings (ignore braces inside strings)
                    if (ch === '"' && !isEscaped(whole, j)) {
                        // toggle inside-string, advance to closing quote
                        j = skipString(whole, j);
                        if (j >= n) break;
                        continue;
                    }
                    if (ch === openChar) {
                        depth++;
                    } else if (ch === closeChar) {
                        if (depth === 0) {
                            // found the matching close at position j
                            break;
                        } else {
                            depth--;
                        }
                    }
                }

                if (j >= n) {
                    // can't find matching close — nothing to do
                    return;
                }

                // compute end line index from j
                const endLine = charIndexToLine(j);

                // hide/show lines between startLine+1 .. endLine-1
                const from = startLine + 1;
                const to = endLine - 1;
                for (let lineIdx = from; lineIdx <= to; ++lineIdx) {
                    const node = containerChildren[lineIdx];
                    if (!node) continue;
                    node.style.display = collapsed ? "none" : "flex";
                }
            };
        })();
    }

    // helper: return true if pos is inside a JSON string (naive but works for pretty JSON)
    function isInsideString(text, pos) {
        // count unescaped quotes before pos
        let inString = false;
        for (let k = 0; k < pos; ++k) {
            if (text[k] === '"' && !isEscaped(text, k)) inString = !inString;
        }
        return inString;
    }

    // helper: skip over string starting at quote (returns index of closing quote or end)
    function skipString(text, startQuoteIndex) {
        let i = startQuoteIndex + 1;
        const n = text.length;
        while (i < n) {
            if (text[i] === '"' && !isEscaped(text, i)) return i;
            i++;
        }
        return n;
    }

    // helper: check if character at pos is escaped (immediately preceded by odd number of backslashes)
    function isEscaped(text, pos) {
        let count = 0;
        let k = pos - 1;
        while (k >= 0 && text[k] === "\\") { count++; k--; }
        return (count % 2) === 1;
    }
}

/* syntaxHighlight: same as before (keeps VSCode-like colors) */
function syntaxHighlight(line) {
    line = escapeHtml(line);
    return line.replace(
        /("(\\u[\da-fA-F]{4}|\\[^u]|[^\\"])*"\s*:?)|\b(true|false|null)\b|\b-?\d+(\.\d+)?\b/g,
        function (match) {
            let cls = "json-number";
            if (/^"/.test(match)) {
                cls = /:$/.test(match) ? "json-key" : "json-string";
            } else if (/true|false/.test(match)) {
                cls = "json-boolean";
            } else if (/null/.test(match)) {
                cls = "json-null";
            }
            return `<span class="${cls}">${match}</span>`;
        }
    );
}

/* === file upload / download === */
function uploadJson() { document.getElementById("fileInput").click(); }

function onFileSelected(evt) {
    const file = evt.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = (e) => { document.getElementById("input").value = e.target.result; };
    reader.readAsText(file);
}

function downloadJson() {
    if (!lastFormattedJson) { alert("No JSON to download"); return; }
    const blob = new Blob([lastFormattedJson], { type: "application/json" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url; a.download = "formatted.json"; a.click(); URL.revokeObjectURL(url);
}