FROM python:3.9-slim-buster

# Install needed debian packages
RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get -yq --no-install-recommends install \
    sudo \
    clang \
    llvm \
    libclang-7-dev \
 && apt-get clean && rm -rf /var/lib/apt/lists/*

# Add a default developer user
RUN useradd -m -G sudo developer \
    && echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Install dependencies
ADD requirements.txt /
RUN pip3 install -r /requirements.txt
# Open Links to GitLab in a new tab
RUN sed -i 's/fa-gitlab">/fa-gitlab" target="_blank">/' /usr/local/lib/python3.11/site-packages/sphinx_rtd_theme/breadcrumbs.html
