"""
Build the version selector for the generated BML documentation.

This script is run after Doxygen has generated the HTML documentation.

It performs two tasks:

1. Generates version-selector.js from the JavaScript template.
   The placeholder "__VERSION_OPTIONS__" is replaced with a list
   of <option> elements representing the available documentation
   versions.

2. Adds a reference to version-selector.js to every HTML document
   that does not already contain one.

The generated documentation is expected to have the following layout:

    /BML/
        index.html
        ...

        1.0.0/
            index.html
            ...

        1.1.0/
            index.html
            ...

The master documentation is located at /BML/, while released
versions are located at /BML/<version>/.

Environment variables:

    BUILD_REF
        Git reference currently being built.

        A tag such as "v1.2.3" becomes version "1.2.3".
        Any other value is treated as "master".
"""

import os
import subprocess
from pathlib import Path


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

REPOSITORY_PATH = "/BML/"
DOCS_DIRECTORY = Path("docs/html")
JAVASCRIPT_TEMPLATE = Path("version-selector.js")


# ---------------------------------------------------------------------------
# Git version handling
# ---------------------------------------------------------------------------

def get_tags():
    """
    Return all version tags in descending version order.

    Only tags beginning with "v" are considered.

    For example:

        v1.2.0
        v1.1.0
        v1.0.0

    The "v" prefix is retained here and removed later when
    constructing the documentation paths.
    """

    result = subprocess.check_output(
        [
            "git",
            "tag",
            "--list",
            "v*",
            "--sort=-version:refname",
        ],
        text=True,
    )

    return result.splitlines()


def get_current_version():
    """
    Determine which documentation version should be selected.

    BUILD_REF is normally supplied by the GitHub Actions workflow.

    A tagged build such as:

        BUILD_REF=v1.2.0

    produces:

        1.2.0

    Any non-tag build defaults to:

        master
    """

    build_ref = os.environ.get("BUILD_REF", "master")

    if build_ref.startswith("v"):
        return build_ref[1:]

    return "master"


# ---------------------------------------------------------------------------
# Documentation entries
# ---------------------------------------------------------------------------

def build_entries(tags):
    """
    Build the list of documentation versions.

    The master documentation is always included first.

    Each Git tag is converted from:

        v1.2.0

    to:

        ("1.2.0", "/BML/1.2.0/")
    """

    entries = [
        ("master", REPOSITORY_PATH),
    ]

    for tag in tags:
        if tag.startswith("v"):
            version = tag[1:]
            path = f"{REPOSITORY_PATH}{version}/"

            entries.append((version, path))

    return entries


def build_options(entries, current_version):
    """
    Generate the HTML <option> elements used by the selector.

    The currently built version receives the HTML "selected"
    attribute.
    """

    options = []

    for name, path in entries:
        selected = ""

        if name == current_version:
            selected = " selected"

        options.append(
            f'<option value="{path}"{selected}>{name}</option>'
        )

    return "\n".join(options)


# ---------------------------------------------------------------------------
# JavaScript generation
# ---------------------------------------------------------------------------

def build_javascript(options):
    """
    Generate the final version-selector.js file.

    version-selector.js is used as a template.

    The template contains:

        __VERSION_OPTIONS__

    This placeholder is replaced with the generated HTML options.
    """

    javascript = JAVASCRIPT_TEMPLATE.read_text(
        encoding="utf-8"
    )

    return javascript.replace(
        "__VERSION_OPTIONS__",
        options,
    )


# ---------------------------------------------------------------------------
# HTML modification
# ---------------------------------------------------------------------------

def add_selector_to_html():
    """
    Add version-selector.js to every generated HTML document.

    The script is inserted immediately before </body>.

    If an HTML file already contains "version-selector.js", it is
    left unchanged.

    If an HTML document does not contain </body>, the script
    reference is appended to the end of the file.
    """

    for html_file in DOCS_DIRECTORY.rglob("*.html"):
        text = html_file.read_text(
            encoding="utf-8"
        )

        # Do not add the script more than once.
        if "version-selector.js" in text:
            continue

        script = (
            '<script src="version-selector.js"></script>'
        )

        if "</body>" in text:
            text = text.replace(
                "</body>",
                script + "\n</body>",
                1,
                )
        else:
            text += "\n" + script + "\n"

        html_file.write_text(
            text,
            encoding="utf-8",
        )


# ---------------------------------------------------------------------------
# Main build process
# ---------------------------------------------------------------------------

def main():
    """
    Generate the version selector and update the documentation HTML.

    The build process is:

        Git tags
            |
            v
        documentation entries
            |
            v
        HTML <option> elements
            |
            v
        JavaScript template
            |
            v
        docs/html/version-selector.js
            |
            v
        add script reference to HTML files
    """

    tags = get_tags()
    current_version = get_current_version()

    entries = build_entries(tags)

    options = build_options(
        entries,
        current_version,
    )

    javascript = build_javascript(options)

    selector = DOCS_DIRECTORY / "version-selector.js"

    selector.write_text(
        javascript,
        encoding="utf-8",
    )

    add_selector_to_html()


if __name__ == "__main__":
    main()