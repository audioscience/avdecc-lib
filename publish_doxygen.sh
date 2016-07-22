#!/bin/bash

# Settings
REPO_PATH=https://github.com/audioscience/avdecc-lib.git
BUILD_PATH=build-doc
HTML_PATH=doc/public_lib


##### Configure git.
# Set the push default to simple i.e. push only the current branch.
git config --global push.default simple
# Pretend to be an user called Travis CI.
git config --global user.name "Travis CI"
git config --global user.email "travis@travis-ci.org"

# Pull the HTML repo
mkdir -p ${BUILD_PATH}
git clone -b gh-pages "${REPO_PATH}" --single-branch ${BUILD_PATH}
echo "After clone"
pwd
ls

# Create HTML directory
cd ${BUILD_PATH}
mkdir -p ${HTML_PATH}
echo "After mkdir build path"
cd -
ls ${BUILD_PATH}

##### Generate the Doxygen code documentation and log the output.          #####
echo 'Generating Doxygen code documentation...'
doxygen Doxyfile-public_lib

# Create and commit the documentation repo.
cd ${BUILD_PATH}
echo "After cd build path"
pwd
ls
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

	# Don't have authentication figured out at this point.
	echo 'push to github here - if it worked'
    #git push --force https://andrew-elder:${GH_AVDECC_TOKEN}@github.com/audioscience/avdecc-lib.git
else
    echo 'Warning: No documentation (html) files have been found!'
    echo 'Warning: Not going to push the documentation to GitHub!'
    exit 0
fi
cd -
