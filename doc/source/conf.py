# Configuration file for the Sphinx documentation builder.
#
# Docraft — auto-generated API docs via Doxygen + Breathe.
# Doxygen is executed automatically at build time so the XML
# is always up-to-date without manual steps.
import os
import sys

sys.path.insert(0, os.path.abspath("_themes"))
import subprocess, os

# ── Run Doxygen automatically ──────────────────────────────────────────────
_doc_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
subprocess.call(["doxygen"], cwd=_doc_root)

# ── Project information ───────────────────────────────────────────────────
project   = 'Docraft'
copyright = '2026, Cadons'
author    = 'Cadons'
release   = '1.0.0'

# ── Extensions ────────────────────────────────────────────────────────────
extensions = [
    'breathe',
    'sphinx.ext.autosectionlabel',
    'sphinx_design',
]

# ── Breathe configuration ────────────────────────────────────────────────
breathe_projects         = {"docraft": os.path.join(_doc_root, "doxy_files", "xml")}
breathe_default_project  = "docraft"
breathe_default_members  = ('members', 'undoc-members')

# ── General ───────────────────────────────────────────────────────────────
templates_path  = ['_templates']
exclude_patterns = []

# ── HTML output ───────────────────────────────────────────────────────────
html_theme = "sphinx_rtd_theme"
html_theme_options = {
    'analytics_anonymize_ip': False,
    'logo_only': False,
    'prev_next_buttons_location': 'bottom',
    'style_external_links': False,
    'vcs_pageview_mode': '',
    'flyout_display': 'hidden',
    'version_selector': True,
    'language_selector': False,
    # Toc options
    'collapse_navigation': True,
    'sticky_navigation': True,
    'navigation_depth': 4,
    'includehidden': True,
    'titles_only': False
}
html_static_path = ['_static']

# Remove the paragraph symbol (¶) from section headers
html_add_permalinks = ""

# Collapsible sections configuration
html_collapsible_definitions = True

