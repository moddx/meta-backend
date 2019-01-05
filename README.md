#meta-backend

This repository represents backend-library of the project, also known as ComputeUnit-Wrapper.
It is used by the service plattform to deploy self-sustained ComputeUnits.

It uses CMake.

## Dependencies
This library depends on [AMQPcpp](https://github.com/moddx/amqpcpp), 
and the [meta-core](https://github.com/moddx/meta-core) library.

## How to use
You would normaly use this via the [meta-plattform](https://github.com/moddx/meta-plattform) repo.

To use it without the plattform just do the following:

1. Clone `git clone https://github.com/moddx/meta-frontend`
2. Fetch submodules

        git submodules update --init --recursive

