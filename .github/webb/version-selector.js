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

    /*
     * Find the documentation version from the current URL.
     *
     * Examples:
     *
     *     /BML/
     *     /BML/index.html
     *
     *     -> master
     *
     *     /BML/2.0.0/
     *     /BML/2.0.0/index.html
     *     /BML/2.0.0/classes.html
     *
     *     -> 2.0.0
     */

    const path = window.location.pathname;

    let currentVersion = "master";

    const parts = path.split("/");

    /*
     * /BML/<version>/...
     *
     * parts:
     *
     *     ["", "BML", "<version>", ...]
     */

    if (parts.length >= 3 && parts[1] === "BML") {
      if (parts[2] !== "") {
        currentVersion = parts[2];
      }
    }

    /*
     * Select the matching option.
     */

    for (const option of select.options) {
      if (currentVersion === "master") {
        if (option.value === "/BML/") {
          option.selected = true;
          break;
        }
      } else {
        if (option.value === "/BML/" + currentVersion + "/") {
          option.selected = true;
          break;
        }
      }
    }

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