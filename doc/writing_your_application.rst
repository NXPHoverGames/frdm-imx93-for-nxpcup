.. _writing-your-application:

Writing your own application
============================

To write your own application, you can use the ``src`` directory, which
contains a series of files that can be used as a starting point. Its
structure is shown below:

.. code-block:: text

   src/
   ├── CMakeLists.txt
   ├── Kconfig
   ├── frdm_imx93.overlay
   ├── main.c
   └── prj.conf

where:

* ``CMakeLists.txt``: tells the cmake build system which sources to compile
* ``Kconfig``: can be used to add your own configuration options
* ``frdm_imx93.overlay``: can be used to modify the board devicetree
* ``main.c``: contains the implementation for the ``main`` function
* ``prj.conf``: can be used to assign values to the configuration options

.. warning::

   It is mandatory for your application directory to contain the following
   files:

      * ``CMakeLists.txt``
      * ``prj.conf``
      * ``main.c`` (or any other .c file which defines the ``main`` function)

   If you don't need to assign value to any configuration options, you can
   leave ``prj.conf`` empty. However, the file must exist!

The ``CMakeLists.txt`` file
---------------------------

The Zephyr build system is based on `cmake`_. Therefore, you're going to
need this file to tell cmake which source files are needed to build your
application. Its content (taken from ``src/CMakeLists.txt``) is shown
below:

.. code-block:: cmake

   # DON'T TOUCH
   cmake_minimum_required(VERSION 3.20.0)

   # DON'T TOUCH - pulls in the Zephyr build system and defines the "app" target
   find_package(Zephyr)

   # TODO: give your project a name
   project(nxpcup)

   # TODO: include your project sources here - you MUST use the "app" target
   target_sources(app PRIVATE main.c)

.. _cmake: https://cmake.org/

where:

* ``cmake_minimum_required()``: required by cmake. This statement is **mandatory**.
* ``find_package(Zephyr)``: pulls in the Zephyr build system and defines the
  ``app`` target. This statement is **mandatory**.
* ``project()``: defines the project name for your application. This statement
  is **mandatory**. You are allowed to choose your own name here.
* ``target_sources()``: used to tell cmake to compile your source files.

If you want to include multiple source files, you can do so using the
``target_sources()`` cmake command. For example:

.. code-block::

   target_sources(app PRIVATE main.c)
   target_soruces(app PRIVATE foo.c)

This would instruct cmake to include the ``main.c`` and ``foo.c`` source files
in the build.

If you want to conditionally include sources to the build, you can do so
using the ``target_sources_ifdef()`` command. For example:

.. code-block:: cmake

   # this snippet is taken from samples/pixy2/CMakeLists.txt

   target_sources_ifdef(CONFIG_NXPCUP_PIXY2_I2C_TRANSPORT app PRIVATE pixy2_transport_i2c.c)
   target_sources_ifdef(CONFIG_NXPCUP_PIXY2_SPI_TRANSPORT app PRIVATE pixy2_transport_spi.c)

This would include ``pixy2_transport_i2c.c`` in the build only if
``CONFIG_NXPCUP_PIXY2_I2C_TRANSPORT`` is set to ``y``. The same rationale
applies to ``pixy2_transport_spi.c``.

For more information on the ``CMakeLists.txt`` file, check out this Zephyr
`documentation`_, section **Creating an Application by Hand**.

The Kconfig file
----------------

Zephyr uses the same `Kconfig`_-based configuration system as the Linux kernel.
Therefore, if you need to define some additional configuration options, you can
do so through the optional ``Kconfig`` file.

For help on adding your own configuration options, you can have a look at:

1. The `Zephyr Kconfig`_ documentation
2. The `Kconfig Language`_ documentation
3. ``samples/hbridge/Kconfig``
4. ``samples/pixy2/Kconfig``

The devicetree overlay
----------------------

Zephyr uses the devicetree language to decide which hardware devices are available
to it and which drivers need to be probed (or "initialized"). To change the content
of the devicetree at build time, you can use the optional devicetree overlay file:
``frdm_imx93.overlay``.

.. note::

   It's not mandatory to call this file ``frdm_imx93.overlay``. You can choose
   whatever name you see fit.

If you device to use this file, you're going to have to inform ``west`` about
it when trying to build your application. You can do so by invoking ``west``
with the ``DTC_OVERLAY_FILE=<overlay>`` cmake option. For instance, assuming
your devicetree overlay file is called ``frdm_imx93.overlay``, you'd build
your application by running:

.. code-block:: bash

   west build -p -b frdm_imx93//a55 src/ -D DTC_OVERLAY_FILE=frdm_imx93.overlay

For more information, see the **Build an application and apply a devicetree overlay**
bit from :ref:`linux-useful-commands` and :ref:`command-cheatsheet`.

The ``prj.conf`` file
---------------------

See :ref:`non-interactive-configuration` for more details on how to use
this file.


.. _documentation: https://docs.zephyrproject.org/latest/develop/application/index.html
.. _Kconfig: https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html
.. _Kconfig language: https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html
.. _Zephyr Kconfig: https://docs.zephyrproject.org/latest/build/kconfig/index.html
