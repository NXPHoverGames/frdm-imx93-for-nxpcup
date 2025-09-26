.. _the-project-structure:

The project structure
=====================

Regardless of your native OS (Windows or Linux), you should end up with the
following project structure:

.. code-block:: text

    .
    ├── README
    ├── boot
    ├── doc
    ├── docker
    ├── samples
    ├── scripts
    ├── src
    └── west.yml

where:

1. ``boot``: contains all of the file/binaries required for booting the
   board (application binary excluded).
2. ``doc``: contains the documentation-related files
3. ``docker``: contains the Docker-related configuration files
4. ``samples``: contains the source code for the sample applications
5. ``scripts``: contains the utility scripts used for setting up the
   development environment
6. ``src``: starting point for your application.
7. ``west.yml``: west manifest file

The manifest file
-----------------

This ``west.yml`` file is used to tell ``west`` where to clone the required
repositories (i.e. Zephyr and the NXP HAL) and has the following content:

.. code-block:: text

   manifest:
     version: "0.13"

     remotes:
       - name: zephyrproject
         url-base: https://github.com/nxp-upstream

     projects:
       - name: zephyr
         remote: zephyrproject
         revision: nxpcup-2026
         import:
           name-whitelist:
             - hal_nxp

Based on its content, we can see that the Zephyr repository is cloned from
``https://github.com/nxp-upstream/zephyr`` and uses the ``nxpcup-2026``
branch. This is a fork of the ``main`` branch from the `official`_ Zephyr
repository with some additional patches applied on it for temporary
fixes/workarounds that need to be upstreamed.

You should always run ``west update`` each time you start developing to make
sure that you're using the latest version of the Zephyr repository since
additional fixes may be added to the ``nxpcup-2026`` branch.

If you encounter an issue that's related to Zephyr, please make sure to create
a GitHub issue under the FRDM-IMX93 for NXP CUP project repository first as
described in :ref:`issues-and-contributions`.

The ``samples`` directory
-------------------------

This directory is made up of several sub-directories as shown below:

.. code-block:: text

   samples
   ├── hbridge
   ├── hello_world
   ├── pixy2
   └── servo

Each of these sub-directories corresponds to a sample application. The
purpose of such an application is to demonstrate certain functionalities
and help in checking if the hardware setup is sane.

See :ref:`supported-samples` for the list of supported samples.

The ``src`` directory
---------------------

See :ref:`writing-your-application` for more information on the ``src``
directory.

.. _official: https://github.com/zephyrproject-rtos/zephyr
