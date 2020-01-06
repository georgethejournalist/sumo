#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    version.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2007

"""
This script rebuilds "<BUILD_DIR>/src/version.h", the file which
 lets the applications know the version of their build.
It does this by parsing the output of git describe where the function is
 implemented in sumolib.
If the version file is newer than the .git index file or the revision cannot be
determined any existing version.h is kept.
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
from os.path import dirname, exists, getmtime, join, abspath

sys.path.append(dirname(dirname(abspath(__file__))))
import sumolib  # noqa


GITFILE = "index"


def create_version_file(versionFile, revision, vcsFile):
    print('generating %s from revision in %s' % (versionFile, vcsFile))
    with open(versionFile, 'w') as f:
        print('#define VERSION_STRING "%s"' % revision, file=f)


def main():
    sumoRoot = join(dirname(__file__), '..', '..')
    # determine output file
    if len(sys.argv) > 1:
        versionDir = sys.argv[1]
        if sys.argv[1] == "-":
            sys.stdout.write(sumolib.version.gitDescribe())
            return
    else:
        versionDir = join(sumoRoot, "src")
    versionFile = join(versionDir, 'version.h')

    vcsFile = join(sumolib.version.GITDIR, GITFILE)
    if exists(vcsFile):
        if not exists(versionFile) or getmtime(versionFile) < getmtime(vcsFile):
            # vcsFile is newer. lets update the revision number
            try:
                create_version_file(versionFile, sumolib.version.gitDescribe(), vcsFile)
            except Exception as e:
                print("Error creating", versionFile, e)
    else:
        print("unknown revision - version control file '%s' not found" % vcsFile)
    if not exists(versionFile):
        create_version_file(versionFile, sumolib.version.UNKNOWN_REVISION, "<None>")


if __name__ == "__main__":
    main()
