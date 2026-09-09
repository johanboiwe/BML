"""
Build the complete BML documentation website.

The generated website is placed in _site/.

The site contains:

    _site/
        index.html
        version-selector.js
        version-selector.css
        <version>/
            index.html
            version-selector.js
            version-selector.css

The master documentation is generated from the current repository
checkout.

Release documentation is generated from every Git tag matching v*.

The generated _site directory is disposable build output and is not
committed to the repository.
"""

import shutil
import subprocess
import tempfile
from pathlib import Path


REPOSITORY_ROOT = Path(__file__).resolve().parents[2]
SITE_DIRECTORY = REPOSITORY_ROOT / "_site"

VERSION_SELECTOR_TEMPLATE = (
        REPOSITORY_ROOT
        / ".github"
        / "webb"
        / "version-selector.js"
)

VERSION_SELECTOR_CSS = (
        REPOSITORY_ROOT
        / ".github"
        / "webb"
        / "version-selector.css"
)

PAGES_BASE_PATH = "/BML/"


def run_command(command, cwd=None):
    """
    Run a command and stop the build if the command fails.
    """

    print("Running:", " ".join(command))

    subprocess.run(
        command,
        cwd=cwd,
        check=True,
    )


def get_release_tags():
    """
    Return all release tags, newest first.

    Only tags beginning with 'v' are considered documentation releases.
    """

    result = subprocess.run(
        [
            "git",
            "tag",
            "--list",
            "v*",
            "--sort=-version:refname",
        ],
        cwd=REPOSITORY_ROOT,
        check=True,
        capture_output=True,
        text=True,
    )

    return [
        tag.strip()
        for tag in result.stdout.splitlines()
        if tag.strip()
    ]


def prepare_site():
    """
    Remove any previous documentation build and create a clean site.
    """

    if SITE_DIRECTORY.exists():
        print("Removing previous site:", SITE_DIRECTORY)
        shutil.rmtree(SITE_DIRECTORY)

    SITE_DIRECTORY.mkdir(parents=True)


def build_documentation(source_directory, output_directory):
    """
    Configure CMake and generate Doxygen documentation.

    A separate build directory is used for every documentation build.
    """

    build_directory = source_directory / "_documentation_build"

    if build_directory.exists():
        shutil.rmtree(build_directory)

    build_directory.mkdir()

    run_command(
        [
            "cmake",
            "-S",
            ".",
            "-B",
            str(build_directory),
        ],
        cwd=source_directory,
    )

    doxyfile = build_directory / "Doxyfile"

    if not doxyfile.exists():
        raise RuntimeError(
            f"Doxygen configuration was not generated: {doxyfile}"
        )

    run_command(
        [
            "doxygen",
            str(doxyfile),
        ],
        cwd=source_directory,
    )

    generated_directory = source_directory / "docs" / "html"

    if not generated_directory.exists():
        raise RuntimeError(
            f"Doxygen output was not found: {generated_directory}"
        )

    if output_directory.exists():
        shutil.rmtree(output_directory)

    shutil.copytree(
        generated_directory,
        output_directory,
    )


def create_version_options(release_tags):
    """
    Create the HTML option elements used by version-selector.js.

    The current master documentation is represented by the root URL.
    Release directories contain the version number without the
    leading v.
    """

    options = []

    options.append(
        f'<option value="{PAGES_BASE_PATH}" selected>master</option>'
    )

    for tag in release_tags:
        version = tag[1:]
        url = f"{PAGES_BASE_PATH}{version}/"

        options.append(
            f'<option value="{url}">{version}</option>'
        )

    return "\n".join(options)


def generate_version_selector(release_tags):
    """
    Generate the version selector JavaScript and copy the CSS.
    """

    template = VERSION_SELECTOR_TEMPLATE.read_text(
        encoding="utf-8"
    )

    options = create_version_options(release_tags)

    if "__VERSION_OPTIONS__" not in template:
        raise RuntimeError(
            "Version selector placeholder was not found."
        )

    generated = template.replace(
        "__VERSION_OPTIONS__",
        options,
    )

    selector_path = SITE_DIRECTORY / "version-selector.js"

    selector_path.write_text(
        generated,
        encoding="utf-8",
    )

    css_path = SITE_DIRECTORY / "version-selector.css"

    shutil.copy2(
        VERSION_SELECTOR_CSS,
        css_path,
    )

    print("Generated:", selector_path)
    print("Copied:", css_path)


def install_version_selector():
    """
    Install the version selector JavaScript and CSS into every
    generated HTML directory and add the required references to
    every HTML page.
    """

    selector_js = SITE_DIRECTORY / "version-selector.js"
    selector_css = SITE_DIRECTORY / "version-selector.css"

    html_files = list(SITE_DIRECTORY.rglob("*.html"))

    for html_file in html_files:
        html_directory = html_file.parent

        js_destination = html_directory / "version-selector.js"
        css_destination = html_directory / "version-selector.css"

        if html_directory != SITE_DIRECTORY:
            shutil.copy2(
                selector_js,
                js_destination,
            )

            shutil.copy2(
                selector_css,
                css_destination,
            )

        html = html_file.read_text(
            encoding="utf-8"
        )

        if 'href="version-selector.css"' not in html:
            html = html.replace(
                "</head>",
                '    <link href="version-selector.css" rel="stylesheet" type="text/css" />\n'
                "</head>",
                1,
            )

        if 'src="version-selector.js"' not in html:
            html = html.replace(
                "</body>",
                '    <script type="text/javascript" src="version-selector.js"></script>\n'
                "</body>",
                1,
            )

        html_file.write_text(
            html,
            encoding="utf-8",
        )

    print(
        f"Installed version selector into {len(html_files)} HTML files."
    )


def build_master_documentation():
    """
    Build documentation from the current repository checkout.

    Master documentation is placed directly in _site/.
    """

    print()
    print("========================================")
    print("Building master documentation")
    print("========================================")

    with tempfile.TemporaryDirectory() as temporary_directory:
        temporary_directory = Path(temporary_directory)

        source_directory = temporary_directory / "master"

        run_command(
            [
                "git",
                "worktree",
                "add",
                "--detach",
                str(source_directory),
                "HEAD",
            ],
            cwd=REPOSITORY_ROOT,
        )

        try:
            build_documentation(
                source_directory,
                SITE_DIRECTORY,
            )
        finally:
            run_command(
                [
                    "git",
                    "worktree",
                    "remove",
                    "--force",
                    str(source_directory),
                ],
                cwd=REPOSITORY_ROOT,
            )


def build_release_documentation(tag):
    """
    Build documentation for one release tag.

    The leading v is removed from the directory name.

    For example:

        v2.1.0 -> _site/2.1.0/
    """

    version = tag[1:]

    print()
    print("========================================")
    print(f"Building release documentation: {tag}")
    print("========================================")

    with tempfile.TemporaryDirectory() as temporary_directory:
        temporary_directory = Path(temporary_directory)

        source_directory = temporary_directory / version
        output_directory = SITE_DIRECTORY / version

        run_command(
            [
                "git",
                "worktree",
                "add",
                "--detach",
                str(source_directory),
                tag,
            ],
            cwd=REPOSITORY_ROOT,
        )

        try:
            build_documentation(
                source_directory,
                output_directory,
            )
        finally:
            run_command(
                [
                    "git",
                    "worktree",
                    "remove",
                    "--force",
                    str(source_directory),
                ],
                cwd=REPOSITORY_ROOT,
            )


def main():
    """
    Build the complete documentation site.
    """

    print("BML documentation build")
    print("=======================")

    prepare_site()

    release_tags = get_release_tags()

    print()
    print("Release tags:")

    for tag in release_tags:
        print(f"  {tag}")

    build_master_documentation()

    for tag in release_tags:
        build_release_documentation(tag)

    generate_version_selector(release_tags)

    install_version_selector()

    print()
    print("Documentation build complete.")
    print("Site:", SITE_DIRECTORY)


if __name__ == "__main__":
    main()