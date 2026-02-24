"use strict";

/*
  script.js - 工程级可靠实现
  - 与后端约定：后端返回 JSON { status: "success" | "error", data: ..., message: "..." }
  - 无论 HTTP 状态如何，都会尝试解析后端返回的 JSON 并把 message 展示到页面
  - renderJsonViewer: 高亮 + 折叠（用 class 控制折叠）
*/

let lastFormattedJson = ""; // 缓存用于下载

document.addEventListener("DOMContentLoaded", () => {
  // 控件
  const formatBtn = document.getElementById("formatBtn");
  const uploadBtn = document.getElementById("uploadBtn");
  const downloadBtn = document.getElementById("downloadBtn");
  const fileInput = document.getElementById("fileInput");
  const collapseAllBtn = document.getElementById("collapseAllBtn");
  const expandAllBtn = document.getElementById("expandAllBtn");

  formatBtn.addEventListener("click", onFormatClicked);
  uploadBtn.addEventListener("click", () => fileInput.click());
  downloadBtn.addEventListener("click", downloadJson);
  fileInput.addEventListener("change", onFileSelected);

  // 可选的全部折叠 / 全部展开功能
  if (collapseAllBtn) collapseAllBtn.addEventListener("click", () => toggleAll(true));
  if (expandAllBtn) expandAllBtn.addEventListener("click", () => toggleAll(false));
});

// ========== 核心：向后端发起解析请求 ==========
async function onFormatClicked() {
  clearOutput(); // 清空上一轮错误
  const inputEl = document.getElementById("input");
  if (!inputEl) return;
  const raw = inputEl.value;

  if (!raw || !raw.trim()) {
    showError("请输入 JSON");
    return;
  }

  // 发送请求（后端当前接受 text/plain；也可以调整为 application/json）
  try {
    const resp = await fetch("/api/parse", {
      method: "POST",
      headers: { "Content-Type": "text/plain" },
      body: raw
    });

    // 无论 HTTP 状态如何，优先尝试解析 JSON 响应体（后端按约定返回 JSON）
    let jsonResp = null;
    try {
      jsonResp = await resp.json();
    } catch (e) {
      // 如果后端返回非 JSON（极少），把文本内容作为错误展示
      const txt = await resp.text().catch(() => "");
      showError(`服务器返回非 JSON 数据（HTTP ${resp.status}）。响应：${txt || "(空)"} `);
      console.error("非 JSON 响应：", e);
      return;
    }

    // jsonResp 应该是 { status, data, message }
    if (jsonResp && jsonResp.status === "success") {
      // data 可能是对象或数组，统一 stringify 为漂亮的字符串
      lastFormattedJson = JSON.stringify(jsonResp.data, null, 2);
      renderJsonViewer(lastFormattedJson);
    } else {
      // 业务错误（例如 JSON Parse Error）
      const msg = (jsonResp && (jsonResp.message || `HTTP ${resp.status}`)) || `HTTP ${resp.status}`;
      showError(msg);
    }
  } catch (err) {
    // 网络或 fetch 错误
    showError(err.message || String(err));
    console.error("Fetch error:", err);
  }
}

// ========== 错误与输出管理 ==========
function getResultContainer() {
  return document.getElementById("result");
}
function getOutputContainer() {
  return document.getElementById("output");
}
function clearOutput() {
    const outputEl = document.getElementById("output");
    const resultEl = document.getElementById("result");
    if (outputEl) outputEl.innerHTML = "";
    if (resultEl) resultEl.innerHTML = "";
}
// 安全转义
function escapeHtml(text) {
  return String(text)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;");
}
function showError(msg) {
  clearOutput(); 
  const out = getOutputContainer();
  if (!out) return;
  out.innerHTML = `<div style="color:#f48771;padding:8px">${escapeHtml(msg)}</div>`;
}

// ========== JSON Viewer: 高亮 + 折叠 ==========
// 设计原则：不使用外部依赖；每次渲染重建 DOM；折叠通过 class 'collapsed' 控制（配合 CSS）
function renderJsonViewer(jsonString) {
  const container = getResultContainer();
  if (!container) return;

  // 防御：参数必须是字符串
  if (typeof jsonString !== "string") {
    container.innerHTML = `<div style="color:#f48771;padding:8px">renderJsonViewer 需要字符串参数</div>`;
    return;
  }

  container.innerHTML = ""; // 清空旧内容

  // 拆分为行并建立偏移索引（用于通过字符位置映射到行）
  const lines = jsonString.split("\n");
  const lineOffsets = new Array(lines.length);
  let offset = 0;
  for (let i = 0; i < lines.length; ++i) {
    lineOffsets[i] = offset;
    offset += lines[i].length + 1; // +1 表示换行
  }

  // 工具：字符索引 -> 行号（二分或线性查找，数据通常不是极大）
  function charIndexToLine(idx) {
    // 二分查找
    let lo = 0, hi = lineOffsets.length - 1;
    while (lo <= hi) {
      const mid = (lo + hi) >> 1;
      const s = lineOffsets[mid];
      const e = (mid + 1 < lineOffsets.length) ? lineOffsets[mid + 1] : offset;
      if (idx < s) hi = mid - 1;
      else if (idx >= e) lo = mid + 1;
      else return mid;
    }
    return lineOffsets.length - 1;
  }

  // 构建每一行 DOM： 行号 | 折叠按钮 | 内容（高亮后的 HTML）
  for (let i = 0; i < lines.length; ++i) {
    const row = document.createElement("div");
    row.className = "json-line";

    const lineNumber = document.createElement("span");
    lineNumber.className = "line-number";
    lineNumber.textContent = (i + 1);

    const foldToggle = document.createElement("span");
    foldToggle.className = "fold-toggle";
    foldToggle.textContent = ""; // 默认空，后面针对块起始行设为 ▶

    const content = document.createElement("span");
    content.className = "line-content";
    content.innerHTML = syntaxHighlight(lines[i]);

    row.appendChild(lineNumber);
    row.appendChild(foldToggle);
    row.appendChild(content);

    container.appendChild(row);
  }

  // 绑定折叠行为（扫描字符流，找到每个块起始对应的结束位置）
  const whole = jsonString;
  const children = container.children;

  for (let i = 0; i < lines.length; ++i) {
    const line = lines[i];
    // 找到本行第一个 { 或 [
    const idx1 = line.indexOf("{");
    const idx2 = line.indexOf("[");
    let openIdxInLine = -1;
    if (idx1 >= 0 && idx2 >= 0) openIdxInLine = Math.min(idx1, idx2);
    else openIdxInLine = Math.max(idx1, idx2); // 可能其中一个为 -1

    if (openIdxInLine === -1) continue;

    const startCharIndex = lineOffsets[i] + openIdxInLine;
    const openChar = whole[startCharIndex];
    const closeChar = (openChar === "{") ? "}" : "]";

    // 跳过字符串内部的 { 或 [
    if (isInsideString(whole, startCharIndex)) continue;

    const foldElem = children[i].querySelector(".fold-toggle");
    if (!foldElem) continue;
    foldElem.textContent = "▶"; // 可折叠的行显示箭头

    // 点击处理（闭包捕获 startCharIndex, i）
    foldElem.onclick = (() => {
      const start = startCharIndex;
      const startLine = i;
      return () => {
        const willCollapse = foldElem.textContent === "▶";
        foldElem.textContent = willCollapse ? "▼" : "▶";

        // 从 start+1 开始扫描，跳过字符串、统计嵌套
        let depth = 0;
        let j = start + 1;
        const n = whole.length;
        for (; j < n; ++j) {
          const ch = whole[j];
          if (ch === '"' && !isEscaped(whole, j)) {
            j = skipString(whole, j);
            if (j >= n) break;
            continue;
          }
          if (ch === openChar) depth++;
          else if (ch === closeChar) {
            if (depth === 0) break;
            else depth--;
          }
        }

        // 如果没找到闭合，直接返回
        if (j >= n) return;

        const endLine = charIndexToLine(j);
        // 折叠 startLine+1 .. endLine-1
        for (let L = startLine + 1; L <= endLine - 1; ++L) {
          const node = children[L];
          if (!node) continue;
          if (willCollapse) node.classList.add("collapsed");
          else node.classList.remove("collapsed");
        }
      };
    })();
  }

  // 给渲染后的 container 添加 aria-label（可访问性）
  container.setAttribute("aria-label", "JSON Viewer");
}

// 高亮规则（使用你 CSS 中的类）
function syntaxHighlight(line) {
  const escaped = escapeHtml(line);
  return escaped.replace(
    /("(\\u[\da-fA-F]{4}|\\[^u]|[^\\"])*"\s*:?)|\b(true|false|null)\b|\b-?\d+(\.\d+)?\b/g,
    function (match) {
      let cls = "json-number";
      if (/^"/.test(match)) cls = /:$/.test(match) ? "json-key" : "json-string";
      else if (/true|false/.test(match)) cls = "json-boolean";
      else if (/null/.test(match)) cls = "json-null";
      return `<span class="${cls}">${match}</span>`;
    }
  );
}

// 辅助函数：判断位置是否处在字符串内部（考虑转义）
function isInsideString(text, pos) {
  let inString = false;
  for (let k = 0; k < pos; ++k) {
    if (text[k] === '"' && !isEscaped(text, k)) inString = !inString;
  }
  return inString;
}
function skipString(text, startQuoteIndex) {
  let i = startQuoteIndex + 1;
  const n = text.length;
  while (i < n) {
    if (text[i] === '"' && !isEscaped(text, i)) return i;
    i++;
  }
  return n;
}
function isEscaped(text, pos) {
  let count = 0;
  let k = pos - 1;
  while (k >= 0 && text[k] === "\\") { count++; k--; }
  return (count % 2) === 1;
}

// ========== 上传 / 下载 ==========
// 触发 file input（upload 按钮已 wired）
function onFileSelected(evt) {
  const file = evt.target.files && evt.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = (e) => {
    const t = document.getElementById("input");
    if (t) t.value = e.target.result;
    lastFormattedJson = "";
    clearOutput(); // 清理错误提示
  };
  reader.readAsText(file);
}

// 下载已格式化的 JSON（从缓存）
function downloadJson() {
  if (!lastFormattedJson) {
    alert("没有 JSON 可下载");
    return;
  }
  const blob = new Blob([lastFormattedJson], { type: "application/json" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = "formatted.json";
  a.click();
  URL.revokeObjectURL(url);
}

// ========== 全部折叠 / 全部展开（可选） ==========
// toggleAll: collapse=true -> 折叠所有； collapse=false -> 展开所有
function toggleAll(collapse = true) {
  const container = document.getElementById("result");
  if (!container) return;
  const children = container.children;
  for (let i = 0; i < children.length; ++i) {
    const fold = children[i].querySelector(".fold-toggle");
    if (!fold) continue;
    // skip lines without fold arrow
    if (fold.textContent === "") continue;

    // fold.textContent 表示当前状态：
    // "▶" 表示可折叠（当前为展开），点击会折叠
    // "▼" 表示已折叠，点击会展开
    const isCurrentlyCollapsed = (fold.textContent === "▼");

    // 目标是折叠（collapse === true）
    if (collapse && !isCurrentlyCollapsed) {
      // 当前未折叠（▶），需要点击一次折叠
      fold.click();
    } else if (!collapse && isCurrentlyCollapsed) {
      // 目标展开且当前已折叠（▼），需要点击一次展开
      fold.click();
    }
    // 否则已经是期望状态，无需操作
  }
}