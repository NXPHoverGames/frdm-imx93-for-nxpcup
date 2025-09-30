.. NXPCUP documentation master file, created by
   sphinx-quickstart on Mon Aug 18 15:30:34 2025.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to FRDM-IMX93 for NXP CUP!
==================================

About this project
------------------

Through a series of guides and various pieces of documentation, this project aims
to help the students participating in the `NXP CUP`_ contest get started with the
development on the `FRDM-IMX93`_ board.

The software provided in this project is meant to be run as an application in the
context of the `Zephyr RTOS`_, which is why you'll notice the usage of tools such
as `west`_, `ninja`_, and `cmake`_.

Getting started guides
----------------------

Check out the following guides to help you prepare your development environment:

- ✅ :doc:`Getting acquainted with the FRDM-IMX93 board <development_board>`
- ✅ :doc:`Preparing the development environment (Linux) <linux_setup>`
- ✅ :doc:`Preparing the development environment (Windows) <windows_setup>`
- ✅ :doc:`Booting the FRDM-IMX93 board <booting_the_board>`
- ✅ :doc:`Writing your own application <writing_your_application>`

Tutorials
---------

Check out the following tutorials to help you with the software development:

- 📘 :doc:`Command cheatsheet <tutorials/commands>`
- 📘 :doc:`Configuring your application <tutorials/configuring_your_application>`
- 📘 :doc:`Devicetree basics <tutorials/devicetree_basics>`
- 📘 :doc:`Zephyr FRDM-IMX93 devicetree files <tutorials/zephyr_and_devicetrees>`
- 📘 :doc:`The Zephyr device driver model <tutorials/zephyr_device_drivers>`
- 📘 :doc:`Zephyr devicetree bindings <tutorials/zephyr_dt_bindings>`
- 📘 :doc:`i.MX93 pin control <tutorials/imx93_pinctrl>`

Connecting the kit compontents
------------------------------

Ready to connect the kit components? Check out the following guides:

- 🔧 :doc:`Connecting the Pixy2 camera <hardware/pixy2>`
- 🔧 :doc:`Connecting the MG996R servo motor <hardware/mg996r>`
- 🔧 :doc:`Connecting the L298N H-BRIDGE motor <hardware/l298n>`
- 🔧 :doc:`Connecting the battery <hardware/battery>`

.. _supported-samples:

Supported samples
-----------------

Want to test out a certain functionality/hardware module? Check out this list of
supported samples:

.. list-table::
   :header-rows: 1
   :widths: 30 30
   :align: center

   * - Sample name
     - Hardware

   * - :ref:`hello-world-sample`
     - N/A

   * - :ref:`servo-sample`
     - :ref:`MG996R servo motor <mg996r-hw>`

   * - :ref:`hbridge-sample`
     - :ref:`L298N H-BRIDGE <l298n-hw>`

   * - :ref:`pixy2-sample`
     - :ref:`Pixy2 camera <pixy2-camera-hw>`

Is your hardware module not listed here? That's fine! You can use one
of the supported modules as a starting point and add support for your own.

Sample API documentation
------------------------

Check out the `main <doxygen/index.html>`_ documentation page for the various
APIs used throughout the samples.

.. _issues-and-contributions:

Issues and contributions
------------------------

Since this is an open-source project, all kinds of contributions are encouraged
and highly appreciated. Your contribution will help improve the infrastructure
and guides for future editions of the contest.

If you encounter a bug or an issue, please make sure you file a report under
the `Issues`_ tab of our `GitHub repository`_. Additionally, we encourage
students to try and fix the issues/bugs themselves by submitting a pull
request.

Contributing to the `Zephyr project`_ is also highly encouraged. To do so,
please make sure you follow the `Contributing to Zephyr`_ guide. We
recommend checking out the aforementioned guide before contributing to this
project as well.


.. _NXP CUP: https://nxpcup.nxp.com/
.. _FRDM-IMX93: https://www.nxp.com/design/design-center/development-boards-and-designs/frdm-i-mx-93-development-board:FRDM-IMX93
.. _Issues: https://github.com/NXPHoverGames/frdm-imx93-for-nxpcup/issues
.. _GitHub repository: https://github.com/NXPHoverGames/frdm-imx93-for-nxpcup
.. _Zephyr project: https://www.zephyrproject.org/
.. _Zephyr RTOS: https://www.zephyrproject.org/
.. _Contributing to Zephyr: https://docs.zephyrproject.org/latest/contribute/index.html#
.. _west: https://docs.zephyrproject.org/latest/develop/west/index.html
.. _ninja: https://ninja-build.org/
.. _cmake: https://cmake.org/

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Getting started guides

   development_board
   linux_setup
   windows_setup
   the_project_structure
   booting_the_board
   writing_your_application

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Samples

   samples/hello_world
   samples/servo
   samples/hbridge
   samples/pixy2

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Hardware

   hardware/pixy2
   hardware/mg996r
   hardware/l298n
   hardware/battery

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Tutorials

   tutorials/commands
   tutorials/configuring_your_application
   tutorials/devicetree_basics
   tutorials/zephyr_and_devicetrees
   tutorials/zephyr_device_drivers
   tutorials/zephyr_dt_bindings
   tutorials/imx93_pinctrl
