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
################################################################################
##### Upload the documentation to the gh-pages branch of the repository.   #####
# Only upload if Doxygen successfully created the documentation.
# Check this by verifying that the html directory and the file html/index.html
# both exist. This is a good indication that Doxygen did it's work.
if [ -d "${HTML_PATH}/html" ] && [ -f "${HTML_PATH}/html/index.html" ]; then

    echo 'Uploading documentation to the gh-pages branch...'
    # Add everything in this directory (the Doxygen code documentation) to the
    # gh-pages branch.
    # GitHub is smart enough to know which files have changed and which files have
    # stayed the same and will only update the changed files.
    git add --all

    # Commit the added files with a title and description containing the Travis CI
    # build number and the GitHub commit reference that issued this build.
    git commit -m "Deploy code docs to GitHub Pages Travis build: ${TRAVIS_BUILD_NUMBER}" -m "Commit: ${TRAVIS_COMMIT}"

    # Force push to the remote gh-pages branch.
    # The ouput is redirected to /dev/null to hide any sensitive credential data
    # that might otherwise be exposed.
    git push --force "https://${GH_REPO_TOKEN}@github.com/audioscience/avdecc-lib.git" > /dev/null 2>&1
else
    echo '' >&2
    echo 'Warning: No documentation (html) files have been found!' >&2
    echo 'Warning: Not going to push the documentation to GitHub!' >&2
    exit 1
fi
cd -
