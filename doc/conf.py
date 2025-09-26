# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'FRDM-IMX93 for NXP CUP'
copyright = '2025, NXP'
author = 'NXP'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ["sphinx_tabs.tabs"]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

numfig = True

# we refer to figures, tables, etc. as we'd do in Latex
numfig_format = {
    'figure': 'Figure %s',
    'table': 'Table %s',
    'code-block': 'Listing %s',
    'section': 'Section %s',
}

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_css_files = ['css/custom.css']
