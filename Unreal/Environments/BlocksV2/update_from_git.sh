#! /bin/bash

# get path of current script: https://stackoverflow.com/a/39340259/207661
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
pushd "$SCRIPT_DIR" >/dev/null

set -e
set -x

./clean.sh

rsync -a  --exclude 'temp' --delete ../../Plugins/Colosseum Plugins/
rsync -a  --exclude 'temp' --delete ../../../ColosseumLib Plugins/Colosseum/Source/

popd >/dev/null