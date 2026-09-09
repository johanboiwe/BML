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
 * Doxygen documentation directory and loaded by every HTML page.
 *
 * The selector uses only standard browser APIs and has no
 * external JavaScript dependencies.
 */

(function () {
  "use strict";

  /*
   * Add the version selector to the documentation page.
   *
   * The selector is placed in the upper-right corner of the
   * browser window and remains visible while the page is scrolled.
   */
  function addVersionSelector() {
    /*
     * The document body must exist before elements can be added.
     */
    if (!document.body) {
      return;
    }

    /*
     * Prevent the selector from being inserted more than once.
     */
    if (document.getElementById("bml-version-selector")) {
      return;
    }

    /*
     * Create the outer container.
     */
    const wrapper = document.createElement("div");

    wrapper.id = "bml-version-selector";

    /*
     * Position the selector in the upper-right corner.
     */
    wrapper.style.position = "fixed";
    wrapper.style.top = "10px";
    wrapper.style.right = "10px";
    wrapper.style.zIndex = "9999";

    /*
     * Basic visual styling.
     */
    wrapper.style.padding = "6px 8px";
    wrapper.style.background = "white";
    wrapper.style.border = "1px solid #888";
    wrapper.style.borderRadius = "4px";
    wrapper.style.boxShadow = "0 1px 4px rgba(0,0,0,0.2)";

    /*
     * Create the "Version:" label.
     */
    const label = document.createElement("label");

    label.htmlFor = "bml-version-select";
    label.textContent = "Version: ";

    /*
     * Create the drop-down list.
     *
     * The placeholder is replaced by build_docs.py.
     */
    const select = document.createElement("select");

    select.id = "bml-version-select";
    select.innerHTML = `__VERSION_OPTIONS__`;

    /*
     * Navigate to the selected documentation version.
     */
    select.addEventListener("change", function () {
      window.location.href = this.value;
    });

    /*
     * Assemble the selector.
     */
    wrapper.appendChild(label);
    wrapper.appendChild(select);

    document.body.appendChild(wrapper);
  }

  /*
   * Wait for the document body to exist.
   *
   * This handles both cases where the script is loaded before
   * DOMContentLoaded and where the body already exists.
   */
  if (document.readyState === "loading") {
    document.addEventListener(
        "DOMContentLoaded",
        addVersionSelector
    );
  } else {
    addVersionSelector();
  }
})();