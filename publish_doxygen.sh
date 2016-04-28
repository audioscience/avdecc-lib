#!/bin/bash

# Settings
REPO_PATH=https://github.com/audioscience/avdecc-lib.git
BUILD_PATH=build-doc
HTML_PATH=doc/public_lib
COMMIT_USER="Documentation Builder"
COMMIT_EMAIL="aelder@audioscience.com"

# Pull the HTML repo
mkdir -p ${BUILD_PATH}
git clone -b gh-pages "${REPO_PATH}" --single-branch ${BUILD_PATH}

# Create HTML directory
cd ${BUILD_PATH}
mkdir -p ${HTML_PATH}
cd -

doxygen Doxyfile-public_lib

# Create and commit the documentation repo.
cd ${BUILD_PATH}
git add .
git config user.name "${COMMIT_USER}"
git config user.email "${COMMIT_EMAIL}"
git commit -m "Automated documentation build."
git push origin gh-pages
cd -
