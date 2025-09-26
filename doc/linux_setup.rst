.. _preparing-environment-linux:

Preparing the environment (Linux)
=================================

This page describes the steps required for setting up the development
infrastructure on a Linux-based machine.

.. warning::

   Although not a requirement, the setup was tested using a machine
   running Ubuntu natively (i.e. no virtualization).

.. note::

   You can also create the development environment inside a virtual machine
   or WSL2. However, **booting the board inside a virtualized environment
   should be avoided** because of the potential USB-related problems that
   might appear.

Prerequisites
-------------

Setting up the environment requires the installation of some additional
packages. On Debian-based distributions, you can install them by running:

.. warning::

   If you're using a Debian-based distribution (e.g. Ubuntu), please make
   sure to run ``sudo apt-get update`` before installing the prerequisite
   packages or using the setup script!

.. code-block:: bash

   sudo apt-get install -y git

We also need to create the work directory in which the setup script will
install the toolchain, clone the required repositories, etc. To do so, you
can run:

.. code-block:: bash

    mkdir -p ~/work/repos/nxpcup_root && cd ~/work/repos/nxpcup_root

.. note::

   You can place the work directory wherever you see fit. Additionally,
   you can choose to use a different name for it.

   Throughout this guide we'll assume that the work directory
   is called ``nxpcup_root`` and is placed in ``~/work/repos``.

.. warning::

   Make sure the location you choose to place the work directory in
   allows you to create, execute, and read files without having to
   use ``sudo``. You can check this by running ``ls -l .`` inside
   the location and looking at the permission bits of the ``.``
   directory.

Cloning the repository
----------------------

To clone the project repository:

.. code-block:: bash

   git clone https://github.com/LaurentiuM1234/nxpcup.git && cd nxpcup

.. warning::

   Please make sure you clone the project repository inside the work directory!

For information on the project structure, please see: :ref:`the-project-structure`.

Supported environments
----------------------

Below you may find a list containing the currently supported development
environments:

1. :ref:`native-development`
2. :ref:`docker-development`
3. :ref:`vm-development`

.. note::

   Please make sure you follow the steps from the corresponding section to set
   up your desired environment.

.. _native-development:

Native development
------------------

The setup script
~~~~~~~~~~~~~~~~

The script used for setting up the environment for native development can
be found under ``scripts/setup.sh``. The script has **three** major
functionalities:

1. Installing the additional packages required for building the application.
2. Downloading ``uuu``, which is the binary used to boot the board.
3. Setting up the ``west`` environment, which is required for building the
   application.

.. warning::

   The package installation step might not work unless you're using a Linux
   distribution tha is Debian-based (e.g. Ubuntu). This is because the script
   uses the ``apt`` package manager, which might not be present on
   Red Hat-based distributions.

   In this particular case, you'll have to use the ``-p`` option when running
   the script (i.e.: ``./scripts/setup.sh -p``) and install the packages
   manually before running the script. You can find the list of required
   packages by opening the script and having a look at the ``PACKAGES``
   variable, which is the list of pacakges to be installed.


.. warning::

   By default, the script will try to install an ``uuu`` binary, which was
   compiled for the ``x86`` architecture and is targeted for Linux-based
   systems (simply called ``uuu``). Therefore, if your host PC doesn't meet
   this criteria, trying to run ``uuu`` will fail.

   In this case, you'll have to use the ``-d`` option when running the script
   and download uuu manually from the `release`_ page (you can perform this
   step before or after running the script).

   To find the suitable version of the binary, take a look at its name. For
   instance, if your host PC is an x86-based Mac, then you'll have to download
   ``uuu_mac_x86``.

The script can be invoked with three options [#]_:

1. ``-p``: if specified, the script will skip the package installation step
   (i.e.: functionality 1)
2. ``-d``: if specified, the script will skip the uuu download step
   (i.e.: functionality 2)
3. ``-u UUU_NAME``: if specified, the script will use the name passed through
   the ``UUU_NAME`` argument to download the ``uuu`` binary.

To print its usage, you can invoke the script with the ``-h`` option. The output
is shown below:

.. code-block:: text

   Usage: ./scripts/setup.sh [-h] [-p] [-d] [-u UUU_NAME]
   Options:
    -h                print help information
    -p                skip package installation
    -d                skip uuu download
    -u                download specified uuu binary

Below you may find some snippets, which demonstrate how the script can be run:

.. note::

   Please make sure your current directory is ``~/work/repos/nxpcup_root/nxpcup``.

.. note::

  The script may ask you to input your password. This is because package
  installation uses ``sudo``.

.. code-block:: bash

   # run the script with no steps skipped
   ./scripts/setup.sh

.. code-block:: bash

   # run the script with no package installation
   ./scripts/setup.sh -p

.. code-block:: bash

   # run the script with no package installation and no uuu download
   ./scripts/setup.sh -p -d

.. code-block:: bash

   # run the script with no steps skipped and dowload uuu_mac_arm instead of uuu
   ./scripts/setup.sh -u uuu_mac_arm

.. warning::

   Do not run the script using ``sudo``! The script already uses ``sudo``
   for the bits that require admin privilege.

Looking at the structure of the work directory (i.e. ``nxpcup_root``), we can
see the following, newly created directories:

.. code-block:: text

   .
   ├── .venv
   ├── .west
   ├── modules
   ├── nxpcup
   ├── zephyr
   └── zephyr-sdk-0.17.2

Their meaning is described below:

* ``.venv``: result of the virtual environment creation
* ``.west``: result of the west initialization step
* ``modules``: additional repositories
* ``zephyr``: source code of the Zephyr RTOS
* ``zephyr-sdk-0.17.2``: toolchain directory

Additionally, the ``uuu`` binary is placed under: ``nxpcup/boot/uuu``.

.. note::

   The name of the file depends on the value you've passed to the script
   via the ``-u`` option.

Building the ``hello_world`` sample
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To check if the development environment was properly intialized, you
can build the ``hello_world`` sample application. Before doing so, however,
you'll have to activate the python virtual environment by running:

.. code-block:: bash

   source ~/work/repos/nxpcup_root/.venv/bin/activate


.. warning::

   Please remember to activate the python virtual environment every
   time you open a new terminal session. This step needs to be performed
   before you build your application.

Now, build the ``hello_world`` application by running:

.. code-block::

   west build -p -b frdm_imx93//a55 samples/hello_world

Summary
~~~~~~~

Below you may find a summary of the steps required for initializing the
environment for native development:

1. Create the work directory:

.. code-block:: bash

    mkdir -p ~/work/repos/nxpcup_root && cd ~/work/repos/nxpcup_root

2. Clone the repository:

.. code-block:: bash

    git clone https://github.com/LaurentiuM1234/nxpcup.git && cd nxpcup

3. Run the ``setup.sh`` script:

.. code-block:: bash

   ./scripts/setup.sh

4. Activate the python virtual environment:

.. code-block:: bash

   source ~/work/repos/nxpcup_root/.venv/bin/activate

5. Build the ``hello_world`` application:

.. code-block:: bash

   west build -p -b frdm_imx93//a55 samples/hello_world

.. _docker-development:

Docker development (experimental)
---------------------------------

Prerequisites
~~~~~~~~~~~~~

Setting up the environment for Docker-based development requires the
installation of Docker before anything else. To do so, open up the
`install <https://docs.docker.com/engine/install>`__ page, select your
Linux distribution and then follow the detailed steps. If the installation
went well, running ``docker --version`` should print something like this:

.. code-block:: text

   Docker version 28.3.3, build 980b856


The setup script
~~~~~~~~~~~~~~~~

The script used for setting up the environment for Docker development can
be found under ``scripts/docker_setup.sh``. The script has **two** major
functionalities:

1. Downloading ``uuu``, which is the binary used to boot the board.
2. Building the Docker image.

The script can be invoked with one parameter [#]_:

1. ``-u UUU_NAME``: if specified, the script will use the name passed
through the ``UUU_NAME`` argument to download the ``uuu`` binary.

.. warning::

   By default, the script will try to install an ``uuu`` binary, which was
   compiled for the ``x86`` architecture and is targeted for Linux-based
   systems (simply called ``uuu``). Therefore, if your host PC doesn't meet
   this criteria, trying to run ``uuu`` will fail.

   In this case, you'll have to use the ``-d`` option when running the script
   and download uuu manually from the `release`_ page (you can perform this
   step before or after running the script).

   To find the suitable version of the binary, take a look at its name. For
   instance, if your host PC is an x86-based Mac, then you'll have to download
   ``uuu_mac_x86``.

To print its usage, you can invoke the script with the ``-h`` option.
The output is shown below:

.. code-block:: text

   docker_setup.sh [-h] [-u UUU_NAME]
   Options:
    -h                print help information
    -u                download specified uuu binary


Below you may find some snippets, which demonstrate how the script can be run:

.. note::

   Please make sure your current directory is ``~/work/repos/nxpcup_root/nxpcup``.

.. code-block:: text

   # run the script with no steps skipped
   ./scripts/docker_setup.sh

.. code-block:: text

   # run the script with no steps skipped and dowload uuu_mac_arm instead of uuu
   ./scripts/docker_setup.sh -u uuu_mac_arm

The Docker image
~~~~~~~~~~~~~~~~

The Docker image is built using the Dockerfile found under ``docker/Dockerfile``
and is based on ``Ubuntu 22.04``. The work directory is found under
``/home/root/nxpcup_root``.

When starting a container, Docker will mount the ``nxpcup`` directory inside
the container under ``/home/root/nxpcup_root/nxpcup``. Therefore, any
changes made to this directory on the host side will also be visible inside
the container. The reverse is also applicable.

.. warning::

   Changes that are made on the container side **WILL NOT** be persistent
   unless they target the directory structure under ``/home/root/nxpcup_root/nxpcup``.
   Consequently, if you power off your container, you'll lose all of the
   changes made to the container' file system.

Building the ``hello_world`` sample
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To check if the development environment was properly initialized, you can
build the ``hello_world`` sample application by running:

.. code-block:: bash

    docker compose -f ./docker/compose.yaml up --detach
    docker compose -f ./docker/compose.yaml exec zephyr bash -c "source ../.venv/bin/activate && west build -p -b frdm_imx93//a55 samples/hello_world"
    docker compose -f ./docker/compose.yaml down

Summary
~~~~~~~

Below you may find a summary of the steps required for initializing the
environment for Docker development:

1. Create the work directory:

.. code-block:: bash

   mkdir -p ~/work/repos/nxpcup_root && cd ~/work/repos/nxpcup_root

2. Clone the repository:

.. code-block:: bash

   mkdir -p ~/work/repos/nxpcup_root && cd ~/work/repos/nxpcup_root

3. Install Docker by following the steps from `here <https://docs.docker.com/engine/install/>`__

4. Run the ``docker_setup.sh`` script:

.. code-block:: bash

   ./scripts/docker_setup.sh

3. Build the ``hello_world`` sample application:

.. code-block:: bash

   docker compose -f ./docker/compose.yaml up --detach
   docker compose -f ./docker/compose.yaml exec zephyr bash -c "source ENV/bin/activate && west build -p -b frdm_imx93//a55 samples/hello_world"
   docker compose -f ./docker/compose.yaml down

.. _vm-development:

VM development
--------------

If you're running a Linux-based distribution inside a virtual machine (VM)
(WSL2 included), you'll have to follow the steps from :ref:`native-development`
to prepare the environment.

We **strongly** recommend you use the virtual machine for the sole purpose
of building your application. For booting the board, we suggest using your
host OS to avoid potential USB-related issues during the boot process.

.. note::

   Unless the host OS matches the guest OS, you should instruct the setup
   script to skip the ``uuu`` download step using the ``-d`` option. This
   is because the binary downloaded by the setup script will most likely
   not work on the host OS if there's a mis-match between the two.

.. _linux-west-command: 

The ``west`` command
--------------------

During development, you'll use the ``west`` command to perform tasks such
as building your application, updating the repositories, etc. You can read
more about ``west`` `here`_.

The general format of a west command is:

.. code-block:: bash

   west <options> <subcommand> <subcommand_options>

where:

* ``options``: options passed to the ``west`` command.
* ``subcommand``: subcommand to invoke. For instance: ``build``, ``update``, ``init``, etc...
* ``subcommand_options``: options passed to the subcommand.

For help on the ``west`` command, run:

.. code-block:: bash

   west -h

For help on a subcommand, run:

.. code-block:: bash

   west <subcommand> -h

For example, run:

.. code-block:: bash

   west build -h

to display help information on the ``build`` subcommand.

.. _linux-useful-commands:

Useful commands
~~~~~~~~~~~~~~~

**Update the repositories**

.. code-block:: bash

   west update

It is recommended that you run this command each time you start the
development process. This way, you'll make sure that you're using
the latest versions of the dependency repositories (e.g. Zephyr).

**Build an application**

.. code-block:: bash

   west build -p -b frdm_imx93//a55 <path_to_your_application>

where:

* ``path_to_your_application``: path to the application/sample to build (e.g.
  ``samples/hello_world``, ``samples/servo``, etc...)

**Build an application and apply a devicetree overlay**:

.. code-block:: bash

   west build -p -b frdm_imx93//a55 <path_to_your_sample> -D DTC_OVERLAY_FILE=<overlay>

where:

* ``overlay``: name of the devicetree overlay to apply. For instance, ``frdm_imx93.overlay``.

**Build an application and save preprocessed files**

.. code-block:: bash

   west build -p -b frdm_imx93//a55 <path_to_your_sample> -D CONFIG_COMPILER_SAVE_TEMPS=y

Since Zephyr uses a lot of macros, this command may come in handy when you
want to investigate compilation errors caused by their usage.

The name of the preprocessed files will end in ``.i``.

For instance, let's say you want to see how ``samples/pixy2/pixy2_command.c``
looks like after it has been processed by the C preprocessor. To do so, you'd
run:

.. code-block:: bash

   west build -p -b frdm_imx93//a55 samples/pixy2 -D CONFIG_COMPILER_SAVE_TEMPS=y

To find the preprocessed file:

.. code-block:: bash

   find build/ -name "pixy2_command*"

This results in the following output:

.. code-block:: text

   build/CMakeFiles/app.dir/pixy2_command.c.i
   build/CMakeFiles/app.dir/pixy2_command.c.s
   build/CMakeFiles/app.dir/pixy2_command.c.obj

Based on this, the path to the file we're interested in would be
``build/CMakeFiles/app.dir/pixy2_command.c.i`` since it's the only file
using the ``.i`` ending.


.. _release: https://github.com/nxp-imx/mfgtools/releases/latest/
.. _here: https://docs.zephyrproject.org/latest/develop/west/index.html

.. [#] The help parameter (``-h``) is intentionally omitted.
.. [#] The help parameter (``-h``) is intentionally omitted.
