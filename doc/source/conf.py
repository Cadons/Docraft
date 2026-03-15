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
version   = '1.0'

# ── Base URL (required by sphinx-sitemap) ────────────────────────────────
html_baseurl = 'https://cadons.github.io/docraft/'

# ── Extensions ────────────────────────────────────────────────────────────
extensions = [
    'breathe',
    'sphinx.ext.autosectionlabel',
    'sphinx_design',
    'sphinx_sitemap',
    'sphinxext.opengraph',
]

# ── autosectionlabel configuration ───────────────────────────────────────
autosectionlabel_prefix_document = True

# ── Breathe configuration ────────────────────────────────────────────────
breathe_projects         = {"docraft": os.path.join(_doc_root, "doxy_files", "xml")}
breathe_default_project  = "docraft"
breathe_default_members  = ('members', 'undoc-members')

# ── Sitemap (sphinx-sitemap) ──────────────────────────────────────────────
sitemap_url_scheme    = '{link}'          # relative links inside the sitemap
sitemap_filename      = 'sitemap.xml'
sitemap_excludes      = [
    'search.html',
    'genindex.html',
]

# ── Global HTML meta tags (SEO) ───────────────────────────────────────────
_description = (
    'Docraft – a self-contained C++ library for generating '
    'professionally rendered PDF documents without external tools '
    'or runtime dependencies.'
)
_keywords = (
    'Docraft, C++ PDF library, PDF generation, C++, libharu, '
    'document crafting, open source, PDF rendering'
)

html_meta = {
    # Standard SEO
    'description'       : _description,
    'keywords'          : _keywords,
    'author'            : 'Cadons',
    'robots'            : 'index, follow',
    'revisit-after'     : '7 days',
    'language'          : 'en',
    # Open Graph fallback (og: tags are also handled by sphinxext-opengraph)
    'og:type'           : 'website',
    'og:site_name'      : 'Docraft Documentation',
}

# ── Open Graph / social preview (sphinxext-opengraph) ────────────────────
ogp_site_url         = html_baseurl
ogp_description_length = 200
ogp_type             = 'website'
ogp_site_name        = 'Docraft Documentation'
ogp_social_cards     = {
    'enable': False,   # set True and install pillow to generate card images
}
# Twitter / X card (added as extra HTML meta by opengraph extension)
ogp_custom_meta_tags = [
    '<meta name="twitter:card"        content="summary" />',
    '<meta name="twitter:site"        content="@cadons" />',
    '<meta name="twitter:creator"     content="@cadons" />',
    '<meta name="twitter:description" content="{}" />'.format(_description),
]

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

