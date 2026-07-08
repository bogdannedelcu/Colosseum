import setuptools
from colosseum import __version__

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="colosseum",
    version=__version__,
    author="Codex Labs, LLC",
    author_email="support@codex-labs-llc.com",
    description="Open source simulator based on Unreal Engine for autonomous vehicles, maintained by Codex Labs",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/CodexLabsLLC/Colosseum",
    packages=setuptools.find_packages(),
	license='MIT',
    classifiers=(
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ),
    install_requires=[
          'msgpack-rpc-python', 'numpy', 'opencv-contrib-python'
    ]
)
