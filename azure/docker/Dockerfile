FROM nvidia/cudagl:9.0-devel

RUN apt-get update
RUN apt-get install \
	sudo \
	libglu1-mesa-dev \
	xdg-user-dirs \
	pulseaudio \
	sudo \
	x11-xserver-utils \
	unzip \
	wget \
	software-properties-common \
	-y --no-install-recommends

RUN apt-add-repository ppa:deadsnakes/ppa
RUN apt-get update
RUN apt-get install -y \
	python3.6 \
	python3-pip 

RUN python3.6 -m pip install --upgrade pip

RUN python3.6 -m pip install setuptools wheel

RUN adduser --force-badname --disabled-password --gecos '' --shell /bin/bash colosseum_user && \ 
	echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers && \ 
	adduser colosseum_user sudo && \ 
	adduser colosseum_user audio && \ 
	adduser colosseum_user video

USER colosseum_user
WORKDIR /home/colosseum_user

# Change the following values to use a different Colosseum binary
# Also change the COLOSSEUM_EXECUTABLE variable in docker-entrypoint.sh
ENV COLOSSEUM_BINARY_ZIP_URL=https://github.com/CodexLabsLLC/Colosseum/releases/download/v1.3.1-linux/Blocks.zip 
ENV COLOSSEUM_BINARY_ZIP_FILENAME=Blocks.zip

ENV SDL_VIDEODRIVER_VALUE=offscreen
ENV SDL_HINT_CUDA_DEVICE=0

# Download and unzip the Colosseum binary
RUN wget -c $COLOSSEUM_BINARY_ZIP_URL
RUN unzip $COLOSSEUM_BINARY_ZIP_FILENAME
RUN rm $COLOSSEUM_BINARY_ZIP_FILENAME

# Add the Python app to the image
ADD ./app /home/colosseum_user/app

WORKDIR /home/colosseum_user
RUN mkdir -p /home/colosseum_user/Documents/Colosseum
ADD ./app/settings.json /home/colosseum_user/Documents/Colosseum
ADD ./docker/docker-entrypoint.sh /home/colosseum_user/docker-entrypoint.sh

RUN sudo chown -R colosseum_user /home/colosseum_user

WORKDIR /home/colosseum_user/app

# Install Python requirements
RUN python3.6 -m pip install -r requirements.txt

ENTRYPOINT /home/colosseum_user/docker-entrypoint.sh