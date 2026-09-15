#! /bin/bash

RPC_VERSION_FOLDER="rpclib-2.3.0"
folder_name="Release"
build_dir=build


mkdir -p build
cd build

CC=/usr/bin/clang-18 CXX=/usr/bin/clang++-18 cmake ../cmake -DCMAKE_CXX_FLAGS='-stdlib=libc++'

make -j$(nproc)

cd ..

mkdir -p ColosseumLib/lib/x64/$folder_name
mkdir -p ColosseumLib/deps/rpclib/lib
mkdir -p ColosseumLib/deps/MavLinkCom/lib
cp $build_dir/output/lib/libColosseumLib.a ColosseumLib/lib
cp $build_dir/output/lib/libMavLinkCom.a ColosseumLib/deps/MavLinkCom/lib
cp $build_dir/output/lib/librpc.a ColosseumLib/deps/rpclib/lib/librpc.a

# Update ColosseumLib/lib, ColosseumLib/deps, Plugins folders with new binaries
rsync -a --delete build/output/lib/ ColosseumLib/lib/x64/$folder_name
rsync -a --delete external/rpclib/$RPC_VERSION_FOLDER/include ColosseumLib/deps/rpclib
rsync -a --delete MavLinkCom/include ColosseumLib/deps/MavLinkCom
rsync -a --delete ColosseumLib Unreal/Plugins/Colosseum/Source
rm -rf Unreal/Plugins/Colosseum/Source/ColosseumLib/src

# Update all environment projects 
for d in ~/Documents/Unreal\ Projects/*; do
    # Skip if not a directory
    [ -d "$d" ] || continue
    # Skip if symbolic link
    [ -L "${d%/}" ] && continue

    # Execute clean.sh if it exists and is executable
    if [ -x "$d/clean.sh" ]; then
        "$d/clean.sh"
    fi

    # Ensure Plugins directory exists
    mkdir -p "$d/Plugins"

    # Sync Colosseum plugin into Plugins directory
    rsync -a --delete Unreal/Plugins/Colosseum/ "$d/Plugins/Colosseum/"
done

echo ""
echo ""
echo "=================================================================="
echo " Colosseum plugin is built! Here's how to build Unreal project."
echo "=================================================================="
echo "All environments under Unreal/Environments have been updated."
echo ""
echo "For further info see the docs:"
echo "https://codexlabsllc.github.io/Colosseum/build_linux/"
echo "=================================================================="