#!/bin/bash

# Step into the build folder
cd build || exit 1

# Force a rebuild
cmake --build . --clean-first

# Run the compiled binary
./trading_engine  # or whatever your executable is named
