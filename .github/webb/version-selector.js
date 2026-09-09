/*
 * BML Documentation Version Selector
 *
 * This file is used as a template by build_docs.py.
 *
 * The Python build script replaces:
 *
 *     __VERSION_OPTIONS__
 *
 * with the list of available documentation versions.
 *
 * The generated JavaScript file is copied into the generated
 * Doxygen documentation directories and loaded by every HTML page.
 *
 * The selector uses only standard browser APIs.
 */

(function () {
  "use strict";

  function addVersionSelector() {
    if (!document.body) {
      return;
    }

    if (document.getElementById("bml-version-selector")) {
      return;
    }

    const wrapper = document.createElement("div");

    wrapper.id = "bml-version-selector";

    const label = document.createElement("label");

    label.htmlFor = "bml-version-select";
    label.textContent = "Version: ";

    const select = document.createElement("select");

    select.id = "bml-version-select";
    select.innerHTML = `__VERSION_OPTIONS__`;

    select.addEventListener("change", function () {
      window.location.href = this.value;
    });

    wrapper.appendChild(label);
    wrapper.appendChild(select);

    document.body.appendChild(wrapper);
  }

  if (document.readyState === "loading") {
    document.addEventListener(
        "DOMContentLoaded",
        addVersionSelector
    );
  } else {
    addVersionSelector();
  }
})();